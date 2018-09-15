
obj-y :=
obj-$(CONFIG_GIC) += gic.rel
obj-$(CONFIG_GICv2) += gicv2.rel
obj-$(CONFIG_GICv3) += gicv3.rel
obj-$(CONFIG_ARM_DCC) += dcc/

obj-$(CONFIG_UART_PL01X) += uart_pl01x.rel

obj-y += timer.rel
obj-y += entry.rel
head-y += head.rel

extra-y := $(head-y) sdfirm.lds
