obj-y :=

obj-$(CONFIG_GIC) += gic.rel
obj-$(CONFIG_GICv2) += gicv2.rel
obj-$(CONFIG_GICv3) += gicv3.rel
obj-$(CONFIG_ARM_DCC) += dcc/
obj-$(CONFIG_PMUv3) += pmuv3.rel
obj-$(CONFIG_CORESIGHT) += coresight/

obj-$(CONFIG_UART_PL01X) += uart_pl01x.rel

obj-y += cache.rel
obj-y += timer.rel
obj-y += except.rel
obj-y += entry.rel
head-y += head.rel

extra-y := $(head-y) sdfirm.lds
