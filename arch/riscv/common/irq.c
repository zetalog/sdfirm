#include <target/init.h>
#include <target/irq.h>
#include <target/arch.h>

uint32_t irq_nesting;

void __bad_interrupt(void)
{
	asm volatile ("j ." : : : "memory");
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

asmlinkage void __vectors(void);

__init void trap_init(void)
{
	csr_write(CSR_SCRATCH, 0);
	csr_write(CSR_TVEC, &__vectors);
	csr_write(CSR_IE, -1);
}
