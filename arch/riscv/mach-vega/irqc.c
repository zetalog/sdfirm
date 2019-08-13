#include <target/irq.h>
#include <target/arch.h>

void irqc_hw_handle_irq(void)
{
	irq_t irq = csr_read(mcause);

	do_IRQ(irq);
}
