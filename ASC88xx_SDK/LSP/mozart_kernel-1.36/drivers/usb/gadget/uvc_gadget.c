/*
 *	uvc_gadget.c  --  USB Video Class Gadget driver
 *
 *	Copyright (C) 2009
 *	    Laurent Pinchart (laurent.pinchart@ideasonboard.com)
 *
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 *
 */

#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/list.h>
#include <linux/mutex.h>
#include <linux/usb_ch9.h>
#include <linux/usb_gadget.h>
#include <linux/vmalloc.h>
#include <linux/wait.h>

#include <media/v4l2-dev.h>

#include "uvc.h"

#define DRIVER_AUTHOR				"Laurent Pinchart"
#define DRIVER_DESCRIPTION			"USB Video Class Gadget"

unsigned int uvc_trace_param;

/* --------------------------------------------------------------------------
 * Streaming endpoint operations
 */

static int
uvc_endpoint_set_interface(struct uvc_device *dev, u16 altsetting)
{
	if (usb_endpoint_xfer_bulk(&dev->desc.vs_ep))
		return altsetting ? -EINVAL : 0;

	switch (altsetting) {
	case 0:
		if (dev->video.ep)
			usb_ep_disable(dev->video.ep);

		dev->event.type = UVC_EVENT_STREAMOFF;
		dev->event_ready = 1;
		wake_up(&dev->event_wait);
		break;

	case 1:
		if (dev->video.ep)
			usb_ep_enable(dev->video.ep, &dev->desc.vs_ep);

		dev->event.type = UVC_EVENT_STREAMON;
		dev->event_ready = 1;
		wake_up(&dev->event_wait);
		break;

	default:
		return -EINVAL;
	}

	return 0;
}

static int
uvc_endpoint_set_configuration(struct uvc_device *dev, u16 config)
{
	switch (config) {
	case 0:
		dev->power = 8;
		break;

	case 1:
		dev->power = dev->desc.config.bMaxPower * 2;
		break;

	default:
		return -EINVAL;
	}

	return 0;
}

static int
uvc_endpoint_init(struct uvc_device *dev)
{
	/* Find a matching endpoint. */
	usb_ep_autoconfig_reset(dev->gadget);
	dev->video.ep = usb_ep_autoconfig(dev->gadget, &dev->desc.vs_ep);
	if (dev->video.ep == NULL)
		return -EINVAL;

	dev->desc.vs_ep.wMaxPacketSize = dev->video.ep->maxpacket;
	dev->video.ep->driver_data = dev;

	/* TODO Make max_payload_size configurable. */
	if (usb_endpoint_xfer_isoc(&dev->desc.vs_ep)) {
		printk(KERN_INFO "g_uvc: Isochronous mode selected\n");
		dev->video.max_payload_size = 0;
	} else {
		printk(KERN_INFO "g_uvc: Bulk mode selected\n");
		dev->video.max_payload_size = 16 * 1024;
		usb_ep_enable(dev->video.ep, &dev->desc.vs_ep);
	}

	return 0;
}

/* --------------------------------------------------------------------------
 * Descriptor operations
 */

#define UVC_INTF_VIDEO_CONTROL			0
#define UVC_INTF_VIDEO_STREAMING		1

static const struct usb_device_descriptor uvc_device_descriptor = {
	.bLength		= USB_DT_DEVICE_SIZE,
	.bDescriptorType	= USB_DT_DEVICE,
	.bcdUSB			= cpu_to_le16(0x0200),
	.bDeviceClass		= USB_CLASS_MISC,
	.bDeviceSubClass	= 0x02,
	.bDeviceProtocol	= 0x01,
	.bMaxPacketSize0	= 0,
	.idVendor		= 0,
	.idProduct		= 0,
	.bcdDevice		= 0,
	.iManufacturer		= 0,
	.iProduct		= 0,
	.iSerialNumber		= 0,
	.bNumConfigurations	= 1,
};

static const struct usb_config_descriptor uvc_config_descriptor = {
	.bLength		= USB_DT_CONFIG_SIZE,
	.bDescriptorType	= USB_DT_CONFIG,
	.wTotalLength		= 0,
	.bNumInterfaces		= 2,
	.bConfigurationValue	= 1,
	.iConfiguration		= 0,
	.bmAttributes		= USB_CONFIG_ATT_ONE,
	.bMaxPower		= 100,
};

static const struct usb_interface_assoc_descriptor uvc_iad = {
	.bLength		= USB_DT_INTERFACE_ASSOCIATION_SIZE,
	.bDescriptorType	= USB_DT_INTERFACE_ASSOCIATION,
	.bFirstInterface	= 0,
	.bInterfaceCount	= 2,
	.bFunctionClass		= USB_CLASS_VIDEO,
	.bFunctionSubClass	= 0x03,
	.bFunctionProtocol	= 0x00,
	.iFunction		= 0,
};

static const struct usb_interface_descriptor uvc_video_control_descriptor = {
	.bLength		= USB_DT_INTERFACE_SIZE,
	.bDescriptorType	= USB_DT_INTERFACE,
	.bInterfaceNumber	= UVC_INTF_VIDEO_CONTROL,
	.bAlternateSetting	= 0,
	.bNumEndpoints		= 0,
	.bInterfaceClass	= USB_CLASS_VIDEO,
	.bInterfaceSubClass	= 0x01,
	.bInterfaceProtocol	= 0x00,
	.iInterface		= 0,
};

#define UVC_DT_CS_ENDPOINT_SIZE		5

static const struct uvc_interrupt_endpoint_descriptor uvc_interrupt_descriptor = {
	.bLength		= UVC_DT_CS_ENDPOINT_SIZE,
	.bDescriptorType	= USB_DT_CS_ENDPOINT,
	.bDescriptorSubType	= EP_INTERRUPT,
	.wMaxTransferSize	= 0x0010,
};

static const struct usb_interface_descriptor uvc_video_streaming_descriptor = {
	.bLength		= USB_DT_INTERFACE_SIZE,
	.bDescriptorType	= USB_DT_INTERFACE,
	.bInterfaceNumber	= UVC_INTF_VIDEO_STREAMING,
	.bAlternateSetting	= 0,
	.bNumEndpoints		= 0,
	.bInterfaceClass	= USB_CLASS_VIDEO,
	.bInterfaceSubClass	= 0x02,
	.bInterfaceProtocol	= 0x00,
	.iInterface		= 0,
};

static int
uvc_make_qualifier(struct uvc_device *dev)
{
	struct usb_qualifier_descriptor qual;

	qual.bLength = sizeof(qual);
	qual.bDescriptorType = USB_DT_DEVICE_QUALIFIER;
	qual.bcdUSB = cpu_to_le16(0x0200);
	qual.bRESERVED = 0;

	qual.bDeviceClass = dev->desc.device.bDeviceClass;
	qual.bDeviceSubClass = dev->desc.device.bDeviceSubClass;
	qual.bDeviceProtocol = dev->desc.device.bDeviceProtocol;
	qual.bMaxPacketSize0 = dev->desc.device.bMaxPacketSize0;
	qual.bNumConfigurations = dev->desc.device.bNumConfigurations;

	memcpy(dev->ep0buf, &qual, sizeof(qual));
	return sizeof(qual);
}

#define UVC_APPEND_DESCRIPTOR_RAW(buf, desc, size) \
	do { \
		memcpy(buf, desc, size); \
		buf += size; \
	} while (0)

#define UVC_APPEND_DESCRIPTOR(buf, desc) \
	UVC_APPEND_DESCRIPTOR_RAW(buf, desc, sizeof(typeof(*desc)))

static int
uvc_make_config(struct uvc_device *dev, u8 type)
{
	struct usb_config_descriptor *config;
	struct usb_interface_descriptor *intf;
	struct usb_endpoint_descriptor *endp;
	void *buf = dev->ep0buf;
	int len;

	UVC_APPEND_DESCRIPTOR(buf, &dev->desc.config);
	UVC_APPEND_DESCRIPTOR(buf, &uvc_iad);

	/* Video control. */
	intf = buf;
	UVC_APPEND_DESCRIPTOR(buf, &uvc_video_control_descriptor);
	UVC_APPEND_DESCRIPTOR_RAW(buf, dev->desc.vc_data, dev->desc.vc_size);

	if (dev->desc.vc_ep.bInterval) {
		endp = buf;
		UVC_APPEND_DESCRIPTOR_RAW(buf, &dev->desc.vc_ep, USB_DT_ENDPOINT_SIZE);
		endp->wMaxPacketSize = 16;
		UVC_APPEND_DESCRIPTOR(buf, &uvc_interrupt_descriptor);

		intf->bNumEndpoints = 1;
	}

	/* Video streaming. */
	intf = buf;
	UVC_APPEND_DESCRIPTOR(buf, &uvc_video_streaming_descriptor);
	UVC_APPEND_DESCRIPTOR_RAW(buf, dev->desc.vs_data, dev->desc.vs_size);

	if (usb_endpoint_xfer_isoc(&dev->desc.vs_ep)) {
		intf = buf;
		UVC_APPEND_DESCRIPTOR(buf, &uvc_video_streaming_descriptor);
		intf->bAlternateSetting = 1;
	}

	intf->bNumEndpoints = 1;
	UVC_APPEND_DESCRIPTOR_RAW(buf, &dev->desc.vs_ep, USB_DT_ENDPOINT_SIZE);

	len = buf - (void *)dev->ep0buf;
	config = (struct usb_config_descriptor *)dev->ep0buf;
	config->wTotalLength = cpu_to_le16(len);

	return len;
}

static ssize_t
uvc_config_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct usb_gadget *gadget = container_of(dev, struct usb_gadget, dev);
	struct uvc_device *udev = get_gadget_data(gadget);
	struct usb_string *string;
	char *p = buf;

	for (string = udev->desc.strings.strings; string->id; ++string)
		p += sprintf(p, "%u %s\n", string->id, string->s);

	return p - buf;
}

static ssize_t
uvc_config_store(struct device *dev, struct device_attribute *attr,
		 const char *buf, size_t count)
{
	struct usb_gadget *gadget = container_of(dev, struct usb_gadget, dev);
	struct uvc_device *udev = get_gadget_data(gadget);
	const struct usb_descriptor_header *usb_header;
	const struct usb_device_descriptor *usb_dd;
	const struct usb_config_descriptor *usb_cd;
	const struct usb_endpoint_descriptor *usb_ep;
	const char *end = buf + count;
	const char *data, *p, *s;
	unsigned int nstrings;
	unsigned int len, i;
	int ret = -EINVAL;

	if (udev->state != UVC_STATE_DISABLED)
		return -EEXIST;

	udev->desc.vc_ep.bLength = USB_DT_ENDPOINT_SIZE;
	udev->desc.vc_ep.bDescriptorType = USB_DT_ENDPOINT;
	udev->desc.vc_ep.bEndpointAddress = USB_DIR_IN | 0x03;
	udev->desc.vc_ep.bmAttributes = USB_ENDPOINT_XFER_INT;
	udev->desc.vc_ep.wMaxPacketSize = 0;
	udev->desc.vc_ep.bInterval = 0;

	udev->desc.vs_ep.bLength = USB_DT_ENDPOINT_SIZE;
	udev->desc.vs_ep.bDescriptorType = USB_DT_ENDPOINT;
	udev->desc.vs_ep.bEndpointAddress = USB_DIR_IN;
	udev->desc.vs_ep.bmAttributes = 0;
	udev->desc.vs_ep.wMaxPacketSize = 0;
	udev->desc.vs_ep.bInterval = 1;

	/*
	 * Offset    | Size | Description
	 * ----------+------+------------------------------------------------
	 *  0        | 4    | Number of string descriptors (n)
	 *  4        | p(1) | Zero-terminated string 1
	 *  4+p(1)   | p(2) | Zero-terminated string 2
	 *  ...      |      |
	 *  4+p(1)   | p(n) | Zero-terminated string n
	 *   +...    |      |
	 *   +p(n-1) |      |
	 *  4+p      | 18   | Device descriptor
	 *  22+p     | 9    | Configuration descriptor
	 *  31+p     | 8    | Interface association descriptor
	 *  39+p     | 9    | Video control interface descriptor
	 *  48+p     | m    | Class-specific video control descriptors
	 *  48+p+m   | 9    | Video streaming interface descriptor
	 *  57+p+m   | q    | Class-specific video streaming descriptors
	 *
	 * - Device: idVendor, idProduct, bcdDevice, iManufacturer, iProduct,
	 *   iSerial
	 * - Configuration: iConfiguration, bmAttributes, bMaxPower
	 * - Interface association: none
	 * - Video control interface: none
	 * - Video streaming interface: none
	 */

	/* Parse string data and create string descriptors. The first pass
	 * computes the total string data length to allocate memory in one
	 * chunk and the second pass creates and fills the string descriptors.
	 */
	nstrings = *(u32 *)buf;
	buf += 4;

	for (p = buf, i = nstrings; p < end && i > 0; ++p) {
		if (*p == '\0')
			i--;
	}

	if (i > 0)
		return -EINVAL;

	len = (nstrings + 1) * sizeof(*udev->desc.string_data);
	len += p - buf + 1;

	udev->desc.string_data = kzalloc(len, GFP_KERNEL);
	if (udev->desc.string_data == NULL)
		return -ENOMEM;

	memcpy(&udev->desc.string_data[nstrings+1], buf, len);
	s = (char *)&udev->desc.string_data[nstrings+1];

	for (i = 0; i < nstrings; ++i) {
		udev->desc.string_data[i].id = i + 1;
		udev->desc.string_data[i].s = s;
		s += strlen(s) + 1;
	}

	udev->desc.strings.language = 0x0409;
	udev->desc.strings.strings = udev->desc.string_data;

	/* Fill device and configuration descriptors. */
	if (end - p < USB_DT_DEVICE_SIZE + USB_DT_CONFIG_SIZE)
		goto error;

	usb_dd = (const struct usb_device_descriptor *)p;
	if (usb_dd->bDescriptorType != USB_DT_DEVICE)
		goto error;

	memcpy(&udev->desc.device, &uvc_device_descriptor,
		sizeof(udev->desc.device));
	udev->desc.device.bMaxPacketSize0 = gadget->ep0->maxpacket;
	udev->desc.device.idVendor = usb_dd->idVendor;
	udev->desc.device.idProduct = usb_dd->idProduct;
	udev->desc.device.bcdDevice = usb_dd->bcdDevice;
	udev->desc.device.iManufacturer = usb_dd->iManufacturer;
	udev->desc.device.iProduct = usb_dd->iProduct;
	udev->desc.device.iSerialNumber = usb_dd->iSerialNumber;
	p += USB_DT_DEVICE_SIZE;

	usb_cd = (const struct usb_config_descriptor *)p;
	if (usb_cd->bDescriptorType != USB_DT_CONFIG)
		goto error;

	memcpy(&udev->desc.config, &uvc_config_descriptor,
		sizeof(udev->desc.config));
	udev->desc.config.iConfiguration = usb_cd->iConfiguration;
	udev->desc.config.bmAttributes = usb_cd->bmAttributes | USB_CONFIG_ATT_ONE;
	udev->desc.config.bMaxPower = usb_cd->bMaxPower;
	p += USB_DT_CONFIG_SIZE;

	/* Video interface association descriptor. */
	if (end - p < USB_DT_INTERFACE_ASSOCIATION_SIZE)
		goto error;

	usb_header = (const struct usb_descriptor_header *)p;
	if (usb_header->bDescriptorType != USB_DT_INTERFACE_ASSOCIATION)
		goto error;
	p += USB_DT_INTERFACE_ASSOCIATION_SIZE;

	/* Video control interface descriptors. */
	if (end - p < USB_DT_INTERFACE_SIZE)
		goto error;

	usb_header = (const struct usb_descriptor_header *)p;
	if (usb_header->bDescriptorType != USB_DT_INTERFACE)
		goto error;
	p += USB_DT_INTERFACE_SIZE;

	for (data = p; p + 3 <= end && p[1] == USB_DT_CS_INTERFACE; p += p[0]) {
		switch (p[2]) {
		case VC_HEADER:
			break;
		}
	}

	if (p > end)
		goto error;

	udev->desc.vc_size = p - data;
	udev->desc.vc_data = kmalloc(udev->desc.vc_size, GFP_KERNEL);
	if (udev->desc.vc_data == NULL) {
		ret = -ENOMEM;
		goto error;
	}

	memcpy(udev->desc.vc_data, data, udev->desc.vc_size);

	/* Optional interrupt endpoint descriptor. */
	if (end - p < sizeof(usb_header))
		goto error;

	usb_header = (const struct usb_descriptor_header *)p;
	if (usb_header->bDescriptorType == USB_DT_ENDPOINT) {
		if (end - p < USB_DT_ENDPOINT_SIZE + UVC_DT_CS_ENDPOINT_SIZE)
			goto error;

		usb_ep = (const struct usb_endpoint_descriptor *)p;
		udev->desc.vc_ep.bInterval = usb_ep->bInterval;

		p += USB_DT_ENDPOINT_SIZE + UVC_DT_CS_ENDPOINT_SIZE;
	}

	/* Video streaming interface descriptors. */
	if (end - p < USB_DT_INTERFACE_SIZE)
		goto error;

	usb_header = (const struct usb_descriptor_header *)p;
	if (usb_header->bDescriptorType != USB_DT_INTERFACE)
		goto error;
	p += USB_DT_INTERFACE_SIZE;

	for (data = p; p + 3 <= end && p[1] == USB_DT_CS_INTERFACE; p += p[0]) {
		switch (p[2]) {
		case VS_INPUT_HEADER:
			break;
		}
	}

	if (p > end)
		goto error;

	udev->desc.vs_size = p - data;
	udev->desc.vs_data = kmalloc(udev->desc.vs_size, GFP_KERNEL);
	if (udev->desc.vs_data == NULL) {
		ret = -ENOMEM;
		goto error;
	}

	memcpy(udev->desc.vs_data, data, udev->desc.vs_size);

	/* Video streaming bulk endpoint. */
	usb_header = (const struct usb_descriptor_header *)p;
	if (end - p >= USB_DT_ENDPOINT_SIZE &&
	    usb_header->bDescriptorType == USB_DT_ENDPOINT) {
		usb_ep = (const struct usb_endpoint_descriptor *)p;
		if (usb_ep->bmAttributes != USB_ENDPOINT_XFER_BULK)
			goto error;

		udev->desc.vs_ep.bmAttributes = usb_ep->bmAttributes;
		p += USB_DT_ENDPOINT_SIZE;

		if (p != end)
			goto error;
	}

	/* Video streaming alternate settings and isochronous endpoints. */
	while (end - p >= USB_DT_INTERFACE_SIZE + USB_DT_ENDPOINT_SIZE) {
		usb_header = (const struct usb_descriptor_header *)p;
		if (usb_header->bDescriptorType != USB_DT_INTERFACE)
			goto error;
		p += USB_DT_INTERFACE_SIZE;

		usb_header = (const struct usb_descriptor_header *)p;
		if (usb_header->bDescriptorType != USB_DT_ENDPOINT)
			goto error;

		usb_ep = (const struct usb_endpoint_descriptor *)p;
		if (usb_ep->bmAttributes != (0X04 | USB_ENDPOINT_XFER_ISOC))
			goto error;

		udev->desc.vs_ep.bmAttributes = usb_ep->bmAttributes;
		p += USB_DT_ENDPOINT_SIZE;
	}

	if (p != end)
		goto error;

	/* Select endpoints. */
	ret = uvc_endpoint_init(udev);
	if (ret < 0)
		goto error;

	udev->state = UVC_STATE_UNCONNECTED;
	usb_gadget_connect(gadget);
	return count;

error:
	kfree(udev->desc.string_data);
	udev->desc.string_data = NULL;
	kfree(udev->desc.vc_data);
	udev->desc.vc_data = NULL;
	kfree(udev->desc.vs_data);
	udev->desc.vs_data = NULL;

	return ret;
}

static DEVICE_ATTR(config, 0644, uvc_config_show, uvc_config_store);

/* --------------------------------------------------------------------------
 * Endpoint 0
 */

static void
uvc_ep0_complete(struct usb_ep *ep, struct usb_request *req)
{
	struct uvc_device *dev = req->context;

	if (dev->event_setup_out) {
		dev->event_setup_out = 0;

		dev->event.type = UVC_EVENT_DATA;
		dev->event.data.length = req->actual;
		memcpy(&dev->event.data.data, &req->buf, req->actual);

		dev->event_ready = 1;
		wake_up(&dev->event_wait);
	}
}

#define DeviceInRequest \
	((USB_DIR_IN | USB_TYPE_STANDARD | USB_RECIP_DEVICE) << 8)
#define DeviceOutRequest \
	((USB_DIR_OUT | USB_TYPE_STANDARD | USB_RECIP_DEVICE) << 8)
#define InterfaceInRequest \
	((USB_DIR_IN | USB_TYPE_STANDARD | USB_RECIP_INTERFACE) << 8)
#define InterfaceOutRequest \
	((USB_DIR_OUT | USB_TYPE_STANDARD | USB_RECIP_INTERFACE) << 8)

static int
uvc_setup_standard(struct usb_gadget *gadget,
	const struct usb_ctrlrequest *ctrl)
{
	struct uvc_device *dev = get_gadget_data(gadget);
	u16 wIndex = le16_to_cpu(ctrl->wIndex);
	u16 wValue = le16_to_cpu(ctrl->wValue);
	int value = -EOPNOTSUPP;

	switch ((ctrl->bRequestType << 8) | ctrl->bRequest) {
	case DeviceInRequest | USB_REQ_GET_DESCRIPTOR:
		switch (wValue >> 8) {
		case USB_DT_DEVICE:
			value = sizeof(dev->desc.device);
			dev->ep0req->buf = &dev->desc.device;
			break;

		case USB_DT_DEVICE_QUALIFIER:
			value = uvc_make_qualifier(dev);
			break;

		case USB_DT_CONFIG:
		case USB_DT_OTHER_SPEED_CONFIG:
			value = uvc_make_config(dev, wValue >> 8);
			break;

		case USB_DT_STRING:
			value = usb_gadget_get_string(&dev->desc.strings,
					wValue & 0xff, dev->ep0buf);
			break;

		case USB_DT_DEBUG:
		default:
			printk(KERN_INFO "Unsupported descriptor type %u\n",
				wIndex >> 8);
			break;
		}
		break;

	case DeviceInRequest | USB_REQ_GET_CONFIGURATION:
		*(u8 *)dev->ep0buf = dev->config;
		value = 1;
		break;

	case DeviceOutRequest | USB_REQ_SET_CONFIGURATION:
		if (uvc_endpoint_set_configuration(dev, wValue) == 0) {
			value = 0;
			dev->config = wValue;
			usb_gadget_vbus_draw(gadget, dev->power);
		}
		break;

	case InterfaceInRequest | USB_REQ_GET_INTERFACE:
		switch (wIndex) {
		case UVC_INTF_VIDEO_CONTROL:
			/* Video control interface has a single alternate
			 * setting.
			 */
			*(u8 *)dev->ep0buf = 0;
			value = 1;
			break;

		case UVC_INTF_VIDEO_STREAMING:
			*(u8 *)dev->ep0buf = dev->altsetting;
			value = 1;
			break;
		}
		break;

	case InterfaceOutRequest | USB_REQ_SET_INTERFACE:
		if (wIndex != UVC_INTF_VIDEO_STREAMING)
			break;

		if (uvc_endpoint_set_interface(dev, wValue) == 0) {
			value = 0;
			dev->altsetting = wValue;
		}
		break;

	default:
		printk(KERN_INFO "Unsupported request bRequestType 0x%02x "
			"bRequest 0x%02x wValue 0x%04x wIndex 0x%04x\n",
			ctrl->bRequestType, ctrl->bRequest, wValue, wIndex);
		break;
	}

	return value;
}

static int
uvc_setup_class(struct usb_gadget *gadget,
	const struct usb_ctrlrequest *ctrl)
{
	struct uvc_device *dev = get_gadget_data(gadget);

	/* printk(KERN_INFO "setup request %02x %02x value %04x index %04x %04x\n",
	 *	ctrl->bRequestType, ctrl->bRequest, le16_to_cpu(ctrl->wValue),
	 *	le16_to_cpu(ctrl->wIndex), le16_to_cpu(ctrl->wLength));
	 */

	/* TODO stall too big requests */

	memcpy(&dev->event.req, ctrl, sizeof(dev->event.req));
	dev->event.type = UVC_EVENT_SETUP;
	dev->event_ready = 1;
	wake_up(&dev->event_wait);

	return -EAGAIN;
}

static int
uvc_setup(struct usb_gadget *gadget, const struct usb_ctrlrequest *ctrl)
{
	struct uvc_device *dev = get_gadget_data(gadget);
	u16 wLength = le16_to_cpu(ctrl->wLength);
	int value = -EOPNOTSUPP;

	dev->state = UVC_STATE_CONNECTED;

	dev->ep0req->buf = &dev->ep0buf;

	switch (ctrl->bRequestType & USB_TYPE_MASK) {
	case USB_TYPE_STANDARD:
		value = uvc_setup_standard(gadget, ctrl);
		break;

	case USB_TYPE_CLASS:
		value = uvc_setup_class(gadget, ctrl);
		break;

	default:
		printk(KERN_INFO "Unsupported request type 0x%02x\n",
			ctrl->bRequestType);
		break;
	}

	if (value >= 0) {
		dev->ep0req->length = min((int)wLength, value);
		dev->ep0req->zero = value < wLength;
		dev->ep0req->dma = DMA_ADDR_INVALID;

		value = usb_ep_queue(gadget->ep0, dev->ep0req, GFP_ATOMIC);
	}

	return (value == -EAGAIN) ? 0 : value;
}

/* --------------------------------------------------------------------------
 * USB probe and disconnect
 */

static int
uvc_register_video(struct uvc_device *dev)
{
	struct video_device *video;

	/* TODO reference counting. */
	video = video_device_alloc();
	if (video == NULL)
		return -ENOMEM;

	video->dev = &dev->gadget->dev;
	video->minor = -1;
	video->fops = &uvc_v4l2_fops;
	video->release = video_device_release;
	strncpy(video->name, dev->gadget->name, sizeof(video->name));

	dev->vdev = video;
	video_set_drvdata(video, dev);

	return video_register_device(video, VFL_TYPE_GRABBER, -1);
}

static void
uvc_disconnect(struct usb_gadget *gadget)
{
	struct uvc_device *dev = get_gadget_data(gadget);

	printk(KERN_DEBUG "uvc_disconnect: gadget %s\n", gadget->name);
	if (dev->state != UVC_STATE_DISABLED)
		dev->state = UVC_STATE_UNCONNECTED;
}

static void
uvc_unbind(struct usb_gadget *gadget)
{
	struct uvc_device *dev = get_gadget_data(gadget);

	printk(KERN_DEBUG "uvc_unbind: gadget %s\n", gadget->name);

	if (dev->vdev) {
		if (dev->vdev->minor == -1)
			video_device_release(dev->vdev);
		else
			video_unregister_device(dev->vdev);
		dev->vdev = NULL;
	}

	device_remove_file(&gadget->dev, &dev_attr_config);

	set_gadget_data(gadget, NULL);
	kfree(dev->desc.string_data);
	kfree(dev->desc.vc_data);
	kfree(dev->desc.vs_data);
	kfree(dev);
}

static int
uvc_bind(struct usb_gadget *gadget)
{
	struct uvc_device *dev;
	int ret;

	printk(KERN_INFO "uvc_bind: gadget %s\n", gadget->name);

	dev = kzalloc(sizeof(*dev), GFP_KERNEL);
	if (dev == NULL)
		return -ENOMEM;

	dev->state = UVC_STATE_DISABLED;
	init_waitqueue_head(&dev->event_wait);
	mutex_init(&dev->event_lock);

	dev->gadget = gadget;
	set_gadget_data(gadget, dev);

	/* Create the sysfs configuration file. */
	device_create_file(&gadget->dev, &dev_attr_config);

	/* Preallocate control endpoint request. */
	dev->ep0req = usb_ep_alloc_request(gadget->ep0, GFP_KERNEL);
	if (dev->ep0req == NULL) {
		ret = -ENOMEM;
		goto error;
	}

	dev->ep0req->complete = uvc_ep0_complete;
	dev->ep0req->context = dev;

	/* Initialise video. */
	ret = uvc_video_init(&dev->video);
	if (ret < 0)
		goto error;

	/* Register a V4L2 device. */
	ret = uvc_register_video(dev);
	if (ret < 0) {
		printk(KERN_INFO "Unable to register video device\n");
		goto error;
	}

	usb_gadget_disconnect(gadget);
	return 0;

error:
	uvc_unbind(gadget);
	return ret;
}

/* --------------------------------------------------------------------------
 * Suspend/resume
 */

static void
uvc_suspend(struct usb_gadget *gadget)
{
	printk(KERN_INFO "uvc_suspend: gadget %s\n", gadget->name);
}

static void
uvc_resume(struct usb_gadget *gadget)
{
	printk(KERN_INFO "uvc_resume: gadget %s\n", gadget->name);
}

/* --------------------------------------------------------------------------
 * Driver
 */

static struct usb_gadget_driver uvc_driver = {
	.function	= "USB Video Class",
	.speed		= USB_SPEED_HIGH,
	.bind		= uvc_bind,
	.unbind		= uvc_unbind,
	.setup		= uvc_setup,
	.disconnect	= uvc_disconnect,
	.suspend	= uvc_suspend,
	.resume		= uvc_resume,
	.driver		= {
		.name		= "g_uvc",
		.owner		= THIS_MODULE,
	},
};

static int __init uvc_init (void)
{
	printk(KERN_INFO "%s (%s)\n", DRIVER_DESCRIPTION, DRIVER_VERSION);
	return usb_gadget_register_driver(&uvc_driver);
}

static void __exit uvc_cleanup (void)
{
	usb_gadget_unregister_driver(&uvc_driver);
}

module_init(uvc_init);
module_exit(uvc_cleanup);

module_param_named(trace, uvc_trace_param, uint, S_IRUGO|S_IWUSR);
MODULE_PARM_DESC(trace, "Trace level bitmask");

MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_AUTHOR(DRIVER_DESCRIPTION);
MODULE_LICENSE("GPL");
MODULE_VERSION(DRIVER_VERSION);

