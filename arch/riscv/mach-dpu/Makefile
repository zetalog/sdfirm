EXTRA_CFLAGS += -DSDFIRM -DDPU

obj-$(CONFIG_RISCV_VIRQ) += entry.rel
obj-y += mach.rel
obj-y += tsc.rel
obj-$(CONFIG_MEM) += mem.rel
obj-$(CONFIG_TIMER) += gpt.rel
obj-$(CONFIG_DPU_TCSR) += tcsr.rel
obj-$(CONFIG_DPU_PLL) += pll.rel
obj-$(CONFIG_DPU_PLL) += clk.rel
obj-$(CONFIG_DPU_SIM_GPIO_IRQ) += gpio.rel
obj-$(CONFIG_DPU_TMR) += tmr.rel
obj-$(CONFIG_DPU_SPI_FLASH) += spi_flash.rel
obj-$(CONFIG_DPU_SSI_FLASH) += ssi_flash.rel
obj-$(CONFIG_DPU_DDR) += ddr.rel
obj-$(CONFIG_DPU_PCI_SUBSYS) += pcie_dpu.rel

obj-$(CONFIG_DPU_BOOT_PE) += pe.rel
obj-$(CONFIG_DPU_BOOT_VPU) += vpu.rel

obj-y += gpt_util.rel
