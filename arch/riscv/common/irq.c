#include <target/irq.h>
#include <target/arch.h>
#include <target/task.h>

uint32_t irq_nesting;

struct task_entry init_task;

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

	irq = cause & ~ICR_IRQ_FLAG;
	irq_nesting++;
	do_IRQ(irq);
	irq_nesting--;
}

void do_riscv_interrupt(struct pt_regs *regs)
{
	irq_t irq;

	irq = regs->cause & ~ICR_IRQ_FLAG;
	if (irq == IRQ_TIMER || irq == IRQ_EXT) {
		irq_hw_handle_irq();
		return;
	}

	printf("Unexpected interrupt cause 0x%lx\n", regs->cause);
	show_regs(regs);
	hart_hang();
}
