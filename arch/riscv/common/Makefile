obj-y :=

CFLAGS_mmu.o := -mcmodel=medany

obj-$(CONFIG_ARCH_HAS_NOVEC) += entry.rel
obj-$(CONFIG_ARCH_HAS_NOVEC) += trap.rel

head-y += head.rel
obj-y += irq.rel

obj-$(CONFIG_RISCV_PMP) += pmp.rel
obj-$(CONFIG_RISCV_F) += float.rel
obj-$(CONFIG_MMU) += mmu.rel
obj-$(CONFIG_TASK) += task.rel
obj-$(CONFIG_CLINT) += clint.rel
obj-$(CONFIG_PLIC) += plic.rel
obj-$(CONFIG_HTIF) += htif.rel

extra-y := $(head-y) sdfirm.lds
