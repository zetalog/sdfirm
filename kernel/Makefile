# Copyright (C) 2008 by Lv Zheng <lv.zheng@soliton.com.cn>
#
# Licensed under the GPL v2, see the file LICENSE in this tarball.

obj-y:=

obj-y += panic.rel
obj-y += bh.rel
obj-y += irq.rel
obj-y += delay.rel
obj-$(CONFIG_CONSOLE) += console.rel
obj-$(CONFIG_TASK) += task.rel
obj-$(CONFIG_TICK) += tick.rel
obj-$(CONFIG_TIMER) += timer.rel
#obj-$(CONFIG_BULK) += bulk.rel
# may be deprecated in the future
obj-$(CONFIG_BULK) += bulk_dio.rel
obj-$(CONFIG_TERM) += term.rel
obj-$(CONFIG_GPIO) += gpio.rel
obj-$(CONFIG_HEAP) += heap.rel
obj-$(CONFIG_HEAP_DLMALLOC) += heap_dlmalloc.rel

obj-y += io/
obj-y += uart/
obj-$(CONFIG_SPI) += spi/
obj-$(CONFIG_I2C) += i2c/
obj-$(CONFIG_USB) += usb/
obj-$(CONFIG_SCS) += scs/
obj-$(CONFIG_MTD) += mtd/
obj-$(CONFIG_SCSI) += scsi/
obj-$(CONFIG_NET) += net/
obj-$(CONFIG_VIDEO) += video/

obj-$(CONFIG_PERF) += perf.rel
