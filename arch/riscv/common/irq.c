#include <target/irq.h>
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

void irq_hw_handle_irq(void)
{
	unsigned long cause = csr_read(CSR_CAUSE);
	irq_t irq;

	irq = cause & ~SCAUSE_IRQ_FLAG;
	irq_nesting++;
	do_IRQ(irq);
	irq_nesting--;
}

void do_riscv_interrupt(struct pt_regs *regs)
{
	irq_t irq;

	irq = regs->cause & ~SCAUSE_IRQ_FLAG;
	if (irq == IRQ_TIMER || irq == IRQ_EXT) {
		irq_hw_handle_irq();
		return;
	}

	printf("Unexpected interrupt cause 0x%lx\n", regs->cause);
	show_regs(regs);
	hart_hang();
}
