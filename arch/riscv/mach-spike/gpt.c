#include <target/timer.h>
#include <target/jiffies.h>
#include <target/smp.h>
#include <target/sbi.h>
#include <target/irq.h>

void riscv_timer(irq_t irq)
{
	irqc_clear_irq(IRQ_TIMER);
	irqc_disable_irq(IRQ_TIMER);
#ifdef CONFIG_RISCV_IRQ_VERBOSE
	/* Useful for testing exception entry code with
	 * CONFIG_TIMER_TEST.
	 */
	printf("SP %016llx on %d\n",
	       (uint64_t)get_sp(), smp_processor_id());
#endif
	tick_handler();
}

#ifdef SYS_BOOTLOAD
void gpt_hw_irq_poll(void)
{
	if (riscv_irq_raised(IRQ_TIMER))
		riscv_timer(IRQ_TIMER);
}
#endif

#ifdef CONFIG_SBI
void gpt_hw_oneshot_timeout(timeout_t tout_ms)
{
	uint64_t next = tick_get_counter() + tout_ms;

	irqc_enable_irq(IRQ_TIMER);
	sbi_set_timer(TSC_FREQ * next);
}
#else
void gpt_hw_oneshot_timeout(timeout_t tout_ms)
{
	uint64_t next = tick_get_counter() + tout_ms;

	irqc_enable_irq(IRQ_TIMER);
	clint_set_mtimecmp(smp_processor_id(), TSC_FREQ * next);
}
#endif

void gpt_hw_ctrl_init(void)
{
	irq_register_vector(IRQ_TIMER, riscv_timer);
}
