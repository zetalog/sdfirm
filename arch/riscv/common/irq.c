#include <target/irq.h>
#include <target/arch.h>
#include <target/task.h>
#include <target/arch.h>

uint32_t irq_nesting;

__noreturn void __bad_interrupt(irq_t irq)
{
	bh_panic();
}

static void riscv_handle_irq(irq_t irq)
{
#ifdef CONFIG_RISCV_IRQ_VERBOSE
	if (irq == IRQ_SOFT)
		printf("Soft IRQ\n");
	if (irq == IRQ_TIMER)
		printf("Timer IRQ\n");
	if (irq == IRQ_DTC)
		printf("Debug/trace IRQ\n");
	if (irq == IRQ_SERR)
		printf("Bus/system error\n");
#ifdef CONFIG_RAS
	if (irq == IRQ_RAS_CE)
		printf("Corrected RAS error\n");
	if (irq == IRQ_RAS_UE)
		printf("Uncorrected RAS error\n");
#endif
	if (irq == IRQ_DVFS)
		printf("Power/thermal IRQ\n");
#endif
	irq_nesting++;
	do_IRQ(irq);
	irq_nesting--;
}

#ifdef CONFIG_RISCV_AIA
#define RISCV_REGS2IRQ(regs)		(csr_read(CSR_TOPI) >> TOPI_IID_SHIFT)
#else
#define RISCV_REGS2IRQ(regs)		((regs)->cause & ~ICR_IRQ_FLAG)
#endif

void do_riscv_interrupt(struct pt_regs *regs)
{
	irq_t irq = RISCV_REGS2IRQ(regs);

	if (irq == IRQ_EXT) {
		irqc_hw_handle_irq();
		return;
	}
	/* TODO: insert major interrupts to the NMI vectors */
	if (irq < NR_INT_IRQS) {
		riscv_handle_irq(irq);
		return;
	}

	printf("Unexpected interrupt cause 0x%lx\n", regs->cause);
	show_regs(regs);
	bh_panic();
}
