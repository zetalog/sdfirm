#include <target/irq.h>
#include <target/arch.h>
#include <target/task.h>
#include <target/arch.h>

uint32_t irq_nesting;

__noreturn void hart_hang(void)
{
	while (1) wait_irq();
	__builtin_unreachable();
}

__noreturn void __bad_interrupt(void)
{
	hart_hang();
}

static void riscv_handle_irq(irq_t irq)
{
#ifdef CONFIG_RISCV_IRQ_VERBOSE
	if (irq == IRQ_SOFT)
		printf("Soft IRQ\n");
	if (irq == IRQ_TIMER)
		printf("Timer IRQ\n");
#endif
	irq_nesting++;
	do_IRQ(irq);
	irq_nesting--;
}

void do_riscv_interrupt(struct pt_regs *regs)
{
	irq_t irq;

	irq = regs->cause & ~ICR_IRQ_FLAG;
	if (irq == IRQ_EXT) {
		irqc_hw_handle_irq();
		return;
	}
	if (irq < NR_INT_IRQS) {
		riscv_handle_irq(irq);
		return;
	}

	printf("Unexpected interrupt cause 0x%lx\n", regs->cause);
	show_regs(regs);
	hart_hang();
}
