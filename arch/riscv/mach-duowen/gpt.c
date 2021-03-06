#include <target/timer.h>
#include <target/jiffies.h>
#include <target/smp.h>
#include <target/sbi.h>
#include <target/irq.h>

#ifdef CONFIG_SBI
/* TODO use hvc sbi_console like Linux kernel */
#else
#ifdef CONFIG_DUOWEN_IMC
void tmr_irq_handler(irq_t irq)
{
	uint8_t cpu = smp_processor_id();

	tmr_irq_clear(cpu);
	tmr_disable_irq(cpu);
	irqc_clear_irq(IRQ_TIMER);
	irqc_disable_irq(IRQ_TIMER);
	tick_handler();
}

#ifdef SYS_BOOTLOAD
void gpt_hw_irq_poll(void)
{
	if (riscv_irq_raised(IRQ_TIMER))
		tmr_irq_handler(IRQ_TIMER);
}
#endif

void gpt_hw_oneshot_timeout(timeout_t tout_ms)
{
	uint8_t cpu = smp_processor_id();
	uint64_t next;

#ifdef CONFIG_DUOWEN_TIMECMP_CONST
	next = tmr_read_counter() + CONFIG_DUOWEN_TIMECMP_CONST_TOUT;
	tmr_write_compare(cpu, next);
#else
	next = tick_get_counter() + tout_ms;
	tmr_write_compare(cpu, TSC_FREQ * next);
#endif
	tmr_enable_irq(cpu);
	irqc_enable_irq(IRQ_TIMER);
}

void gpt_hw_ctrl_init(void)
{
	irq_register_vector(IRQ_TIMER, tmr_irq_handler);
}
#endif
#ifdef CONFIG_DUOWEN_APC
void clint_irq_handler(irq_t irq)
{
	uint8_t cpu = smp_processor_id();

	clint_unset_mtimecmp(cpu);
	irqc_clear_irq(IRQ_TIMER);
	irqc_disable_irq(IRQ_TIMER);
	tick_handler();
}

#ifdef SYS_BOOTLOAD
void gpt_hw_irq_poll(void)
{
	if (riscv_irq_raised(IRQ_TIMER))
		clint_irq_handler(IRQ_TIMER);
}
#endif

void gpt_hw_oneshot_timeout(timeout_t tout_ms)
{
	uint8_t cpu = smp_processor_id();
	uint64_t next;

#ifdef CONFIG_DUOWEN_TIMECMP_CONST
	next = clint_read_mtime() + CONFIG_DUOWEN_TIMECMP_CONST_TOUT;
	clint_set_mtimecmp(cpu, next);
#else
	next = tick_get_counter() + tout_ms;
	clint_set_mtimecmp(cpu, TSC_FREQ * next);
#endif
	irqc_enable_irq(IRQ_TIMER);
}

void gpt_hw_ctrl_init(void)
{
	irq_register_vector(IRQ_TIMER, clint_irq_handler);
}
#endif
#endif /* CONFIG_SBI */
