obj-y :=

obj-$(CONFIG_SMP) += smpboot.rel
obj-$(CONFIG_MMU) += mmu.rel
obj-$(CONFIG_PSCI) += psci_entry.rel
obj-$(CONFIG_PSCI) += psci.rel
obj-$(CONFIG_GIC) += gic.rel
obj-$(CONFIG_GICv2) += gicv2.rel
obj-$(CONFIG_GICv3) += gicv3.rel
obj-$(CONFIG_VMSA) += vmsa.rel
obj-$(CONFIG_ARM_DCC) += dcc/
obj-$(CONFIG_PMUv3) += pmuv3.rel
obj-$(CONFIG_CORESIGHT) += coresight/

obj-y += cache.rel
obj-y += timer.rel
obj-y += except.rel
obj-y += entry.rel
head-y += head.rel

extra-y := $(head-y) sdfirm.lds
