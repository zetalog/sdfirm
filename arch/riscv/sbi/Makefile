EXTRA_CFLAGS += -fno-strict-aliasing

obj-y :=

obj-y += riscv_unpriv.rel
obj-y += sbi_entry.rel
obj-y += sbi_fifo.rel

obj-y += sbi_hart.rel
obj-y += sbi_init.rel
obj-y += sbi_scratch.rel
obj-y += sbi_system.rel

obj-y += sbi_trap.rel
obj-y += sbi_emulate_csr.rel
obj-y += sbi_illegal_insn.rel
obj-y += sbi_misaligned_ldst.rel
obj-y += sbi_ecall.rel
obj-y += sbi_tlb.rel

obj-$(CONFIG_CONSOLE_OUTPUT) += sbi_console.rel
obj-$(CONFIG_ARCH_HAS_SBI_IPI) += sbi_ipi.rel
obj-$(CONFIG_ARCH_HAS_SBI_TIMER) += sbi_timer.rel
obj-$(CONFIG_ARCH_HAS_SBI_CLOCK) += sbi_clock.rel
obj-$(CONFIG_ARCH_HAS_SBI_PINCTRL) += sbi_pinctrl.rel

ifdef CONFIG_SBI_PAYLOAD
sbi_payload: $(CONFIG_SBI_PAYLOAD_PATH)
	if $(READELF) -h $< 2> /dev/null > /dev/null; then $(OBJCOPY) -O binary --set-section-flags .bss=alloc,load,contents $< $@; else cp $< $@; fi
endif
