# Copyright (C) 2008 by Lv Zheng <lv.zheng@soliton.com.cn>
#
# Licensed under the GPL v2, see the file LICENSE in this tarball.

obj-y:=

obj-y += panic.rel
obj-y += bh.rel
obj-y += irq.rel
obj-y += delay.rel
obj-y += stack.rel
obj-y += smp.rel
obj-$(CONFIG_SMP) += percpu.rel
obj-$(CONFIG_ARCH_HAS_QSPINLOCK) += spinlock.rel
obj-$(CONFIG_MEM) += mem.rel
obj-$(CONFIG_CLK) += clk.rel
obj-$(CONFIG_CONSOLE) += console.rel
obj-$(CONFIG_CONSOLE_INPUT) += readline.rel
obj-$(CONFIG_COMMAND) += cmdline.rel
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
obj-$(CONFIG_PAGE) += page.rel
obj-$(CONFIG_MMU) += paging.rel
obj-$(CONFIG_MBOX) += mbox.rel
#obj-y += cpus.rel

obj-y += io/
obj-y += uart/
obj-$(CONFIG_DMA) += dma/
obj-$(CONFIG_SPI) += spi/
obj-$(CONFIG_I2C) += i2c/
obj-$(CONFIG_I3C) += i3c/
obj-$(CONFIG_USB) += usb/
obj-$(CONFIG_PCI) += pci/
obj-$(CONFIG_RIO) += rio/
obj-$(CONFIG_SCS) += scs/
obj-$(CONFIG_MTD) += mtd/
obj-$(CONFIG_SCSI) += scsi/
obj-$(CONFIG_NET) += net/
obj-$(CONFIG_VIDEO) += video/
obj-y += mmc/
obj-$(CONFIG_IOMMU) += iommu/
obj-y += ddr/

obj-$(CONFIG_PERF) += perf.rel
obj-$(CONFIG_RAS) += ras.rel
obj-$(CONFIG_FDT) += fdt/
obj-$(CONFIG_UEFI) += uefi/
obj-y += ipmi/
