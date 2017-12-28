
obj-y :=
obj-$(CONFIG_GIC) += vic_gic.o
obj-$(CONFIG_NVIC) += vic_nvic.o

# CoreLink peripharals
obj-$(CONFIG_UART_PL01X) += uart_pl01x.o

obj-y += entry.rel

ifeq ($(CONFIG_ARM_THUMB2),y)
head-y += head-thumb.rel
else
head-y := head-arm.rel
endif

extra-y := $(head-y) sdfirm.lds
