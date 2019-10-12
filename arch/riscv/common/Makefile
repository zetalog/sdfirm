obj-y :=

obj-y += entry.rel
head-y += head.rel

obj-y += irq.rel
obj-$(CONFIG_CLINT) += clint.rel
obj-$(CONFIG_PLIC) += plic.rel

extra-y := $(head-y) sdfirm.lds
