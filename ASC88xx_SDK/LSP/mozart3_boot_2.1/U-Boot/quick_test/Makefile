# # (C) Copyright 2004-2006
# Wolfgang Denk, DENX Software Engineering, wd@denx.de.
#
# See file CREDITS for list of people who contributed to this
# project.
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation; either version 2 of
# the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston,
# MA 02111-1307 USA
#

include $(TOPDIR)/config.mk

LIB	= $(obj)libquicktest.a

AOBJS	=

COBJS-$(CONFIG_QUICK_TEST_SPI) += spi_flash_test.o
COBJS-$(CONFIG_QUICK_TEST_UART) += uart_test.o
COBJS-$(CONFIG_QUICK_TEST_DRAM) += dram_test.o
COBJS-$(CONFIG_QUICK_TEST_TIMER) += timer_test.o
COBJS-$(CONFIG_QUICK_TEST_WDT) += wdt_test.o
COBJS-$(CONFIG_QUICK_TEST_GMAC) += gmac_test.o
COBJS-$(CONFIG_QUICK_TEST_DIDO) += dido_test.o
COBJS-$(CONFIG_QUICK_TEST_NAND) += nand_test.o
COBJS-$(CONFIG_QUICK_TEST_PCIE) += pcie_test.o
COBJS-$(CONFIG_QUICK_TEST_RTC) += rtc_test.o
COBJS-$(CONFIG_QUICK_TEST_AUDIO_SSM2603) += audio_test_SSM2603.o
COBJS-$(CONFIG_QUICK_TEST_AUDIO_TW2866) += audio_test_TW2866.o
#COBJS-$(CONFIG_QUICK_TEST_VIDEO_MT9P031) += video_test_MT9P031.o
COBJS-$(CONFIG_QUICK_TEST_VIDEO_TW2866) += video_test_TW2866.o
COBJS-$(CONFIG_QUICK_TEST_VIDEO_OV9715) += video_test_OV9710.o
COBJS-$(CONFIG_QUICK_TEST_MSHC) += mshc_test.o
COBJS-$(CONFIG_QUICK_TEST_USBC) += usbc_test.o
COBJS-$(CONFIG_QUICK_TEST_RS485) += rs485_test.o

COBJS	:= $(sort $(COBJS-y))
SRCS	:= $(AOBJS:.o=.S) $(COBJS:.o=.c)
OBJS	:= $(addprefix $(obj),$(AOBJS) $(COBJS))

CPPFLAGS += -I..

all:	$(LIB) $(AOBJS)

$(LIB): $(obj).depend $(OBJS)
	$(AR) $(ARFLAGS) $@ $(OBJS)

#########################################################################

# defines $(obj).depend target
include $(SRCTREE)/rules.mk

sinclude $(obj).depend

#########################################################################
