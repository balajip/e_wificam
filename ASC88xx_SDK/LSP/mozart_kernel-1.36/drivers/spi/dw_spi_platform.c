
#include <linux/platform_device.h>
#include <linux/interrupt.h>
#include <linux/slab.h>
#include <linux/spi/spi.h>


#include "dw_spi.h"

struct dw_spi_platform {
	struct platform_device	*pdev;
	struct dw_spi	dws;
};

static int mozart_dw_spi_probe(struct platform_device *pdev)
{
	struct dw_spi_platform *dwplat;
	struct dw_spi *dws;

	int ret;

	printk("DW: Call mozart_dw_spi_probe()\n");



	dwplat = kzalloc(sizeof(struct dw_spi_platform), GFP_KERNEL);
	if (!dwplat) {
		ret = -ENOMEM;
		goto exit;
	}

	dwplat->pdev = pdev;
	dws = &dwplat->dws;

	/* Get basic io resource and map it */
	dws->paddr = pdev->resource[0].start;
	dws->iolen = pdev->resource[0].end - pdev->resource[0].start + 1;



	dws->regs =ioremap_nocache(pdev->resource[0].start,
				pdev->resource[0].end - pdev->resource[0].start + 1);


	if (!dws->regs) {
		ret = -ENOMEM;
		goto err_kfree;
	}

	dws->parent_dev = &pdev->dev;
	dws->bus_num = 0;// The slave driver can  call spi_busnum_to_master() to get the corresponding master handle.
	//dws->num_cs = 1;
	dws->num_cs = 16;//DW SPI controller supports up to 16 slave-select ouput signals.
	dws->irq = pdev->resource[1].start;
	dws->max_freq=CONFIG_SYS_CLK/2;//dws->max_freq means the APB clk. Its value is (AHB_CLK)/2 .


	ret = dw_spi_add_host(dws);
	if (ret)
		goto err_unmap;

	/* PCI hook and SPI hook use the same drv data */
	platform_set_drvdata(pdev, dwplat);
	return 0;

err_unmap:
	iounmap(dws->regs);
err_kfree:
	kfree(dwplat);
exit:
	return ret;



}

static int mozart_dw_spi_remove(struct platform_device *pdev)
{
  	struct dw_spi_platform *dwplat = platform_get_drvdata(pdev);
	struct dw_spi *dws=&dwplat->dws;
	printk("DW: Call mozart_dw_spi_remove()\n");
	dw_spi_remove_host(dws);

	iounmap(dws->regs);


	kfree(dwplat);

	return 0;

  return 0;
}


#define mozart_dw_spi_suspend NULL
#define mozart_dw_spi_resume NULL

/* work with hotplug and coldplug */
MODULE_ALIAS("platform:mozart_dw_spi");

static struct platform_driver mozart_spigpio_drv = {
	.probe		= mozart_dw_spi_probe,
        .remove		= mozart_dw_spi_remove,
        .suspend	= mozart_dw_spi_suspend,
        .resume		= mozart_dw_spi_resume,
        .driver		= {
		.name	= "mozart_dw_spi",
		.owner	= THIS_MODULE,
        },
};

static int __init dw_spi_init(void)
{
    printk("DW: Call dw_spi_init()\n");
        return platform_driver_register(&mozart_spigpio_drv);
}

static void __exit dw_spi_exit(void)
{ printk("DW: Calldw_spi_exit()\n");
        platform_driver_unregister(&mozart_spigpio_drv);
}

module_init(dw_spi_init);
module_exit(dw_spi_exit);

MODULE_DESCRIPTION("DW SPI Core platform bus Driver");
MODULE_LICENSE("GPL");
