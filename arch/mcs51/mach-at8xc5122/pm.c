#include <target/generic.h>
#include <target/arch.h>

#ifdef CONFIG_CPU_AT8XC5122_X2
void clk_hw_resume_dev(uint8_t dev)
{
	if (dev < 8)
		CKCON0 |= _BV(dev);
	else
		CKCON1 |= _BV(dev);
}

void clk_hw_suspend_dev(uint8_t dev)
{
	if (dev < 8)
		CKCON0 &= ~_BV(dev);
	else
		CKCON1 &= ~_BV(dev);
}
#endif
