obj-y :=

CFLAGS_mmu.o := -mcmodel=medany

extra-$(CONFIG_FDT_BUILTIN) += fdt_builtin.dtb
obj-$(CONFIG_FDT_BUILTIN) += fdt_builtin.dtb.rel

head-y += head.rel
obj-y += trap.rel
obj-y += entry.rel
obj-y += irq.rel
obj-y += tlb.rel
obj-y += smpboot.rel
obj-y += csr_expected_trap.rel
obj-$(CONFIG_RISCV_PMP) += pmp.rel
obj-$(CONFIG_RISCV_IOPMP) += iopmp.rel
obj-$(CONFIG_RISCV_F) += float.rel
obj-$(CONFIG_MMU) += mmu.rel
obj-$(CONFIG_HPM) += hpm.rel
obj-$(CONFIG_TASK) += task.rel
obj-$(CONFIG_CLINT) += clint.rel
obj-$(CONFIG_PLIC) += plic.rel
obj-$(CONFIG_HTIF) += htif.rel
obj-$(CONFIG_RI5CY_PC) += ri5cy_pc.rel
obj-$(CONFIG_VAISRA_TBOX) += tbox.rel
obj-$(CONFIG_VAISRA_PMA) += vaisra_pma.rel
obj-$(CONFIG_VAISRA_RAS) += vaisra_ras.rel

extra-y := $(head-y) sdfirm.lds
