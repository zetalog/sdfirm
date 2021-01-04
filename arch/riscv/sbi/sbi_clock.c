#include <target/sbi.h>

unsigned long sbi_clock_get_freq(unsigned long clkid)
{
	return clk_get_frequency(clkid);
}

void sbi_clock_set_freq(unsigned long clkid, unsigned long freq)
{
	clk_set_frequency(clkid, freq);
}

void sbi_clock_enable(unsigned long clkid)
{
	clk_enable(clkid);
}

void sbi_clock_disable(unsigned long clkid)
{
	clk_disable(clkid);
}
