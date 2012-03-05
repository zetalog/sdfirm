# Copyright (C) 2008 by Lv Zheng <lv.zheng@soliton.com.cn>
#
# Licensed under the GPL v2, see the file LICENSE in this tarball.

obj-y:=

obj-y += panic.rel
obj-y += irq.rel
obj-y += state.rel
obj-y += delay.rel
obj-$(CONFIG_TASK) += task.rel
obj-$(CONFIG_TICK) += tick.rel
obj-$(CONFIG_TIMER) += timer.rel
obj-$(CONFIG_HEAP) += heap.rel
obj-$(CONFIG_HEAP_DLMALLOC) += heap_dlmalloc.rel
obj-$(CONFIG_BULK) += bulk.rel

obj-y += io/
obj-$(CONFIG_UART) += uart/
obj-$(CONFIG_USB) += usb/
obj-$(CONFIG_SCS) += scs/
obj-$(CONFIG_MTD) += mtd/
obj-$(CONFIG_SCSI) += scsi/
obj-$(CONFIG_NET) += net/
