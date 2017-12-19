
obj-y :=
obj-$(CONFIG_NVIC) += nvic.o

obj-y += entry.rel

ifeq ($(CONFIG_ARM_THUMB2),y)
head-y += head-thumb.rel
else
head-y := head-arm.rel
endif

extra-y := $(head-y) sdfirm.lds
