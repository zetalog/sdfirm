#include <target/irq.h>
#include <target/arch.h>

uint32_t irq_nesting;

irq_handler riscv_irqs[NR_INT_IRQS];

void __bad_interrupt(void)
{
	asm volatile ("j ." : : : "memory");
}

void riscv_register_irq(irq_t irq, irq_handler h)
{
	BUG_ON(irq <= 0 || irq >= NR_INT_IRQS);
	riscv_irqs[irq] = h;
}

void irq_hw_handle_irq(void)
{
	uint32_t mcause;
	irq_t irq;

	mcause = csr_read(mcause);
	if (mcause & 0x80000000) {
		irq = (irq_t)(mcause & 0x1F);
		irq_nesting++;
		do_IRQ(irq);
		irq_nesting--;
	}
}

void do_riscv_interrupt(struct pt_regs *regs)
{
	irq_t irq = regs->cause & ~SCAUSE_IRQ_FLAG;

	if (irq >= NR_INT_IRQS || irq == IRQ_EXT) {
		irq_hw_handle_irq();
		return;
	}
	if (riscv_irqs[irq] != NULL) {
		riscv_irqs[irq]();
		return;
	}

	printf("unexpected interrupt cause 0x%lx", regs->cause);
	BUG();
}
