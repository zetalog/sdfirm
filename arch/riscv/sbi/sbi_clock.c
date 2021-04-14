#include <target/sbi.h>
#include <target/spinlock.h>

DEFINE_SPINLOCK(sbi_clock_lock);

unsigned long sbi_clock_get_freq(unsigned long clkid)
{
	unsigned long freq;
	irq_flags_t flags;

	spin_lock_irqsave(&sbi_clock_lock, flags);
	freq = clk_get_frequency(clkid);
	spin_unlock_irqrestore(&sbi_clock_lock, flags);
	return freq;
}

void sbi_clock_set_freq(unsigned long clkid, unsigned long freq)
{
	irq_flags_t flags;

	spin_lock_irqsave(&sbi_clock_lock, flags);
	clk_set_frequency(clkid, freq);
	spin_unlock_irqrestore(&sbi_clock_lock, flags);
}

void sbi_clock_enable(unsigned long clkid)
{
	irq_flags_t flags;

	spin_lock_irqsave(&sbi_clock_lock, flags);
	clk_enable(clkid);
	spin_unlock_irqrestore(&sbi_clock_lock, flags);
}

void sbi_clock_disable(unsigned long clkid)
{
	irq_flags_t flags;

	spin_lock_irqsave(&sbi_clock_lock, flags);
	clk_disable(clkid);
	spin_unlock_irqrestore(&sbi_clock_lock, flags);
}
