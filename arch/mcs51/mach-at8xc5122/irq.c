#include <target/irq.h>

#define irq_hw_clear_priority0(ip0)	(IPH0 &= ~ip0, IPL0 &= ~ip0)
#define irq_hw_clear_priority1(ip1)	(IPH1 &= ~ip1, IPL1 &= ~ip1)
#define irq_hw_set_priority0_high(iph0)	(IPH0 |= iph0)
#define irq_hw_set_priority0_low(ipl0)	(IPL0 |= ipl0)
#define irq_hw_set_priority1_high(iph1)	(IPH1 |= iph1)
#define irq_hw_set_priority1_low(ipl1)	(IPL1 |= ipl1)

void irq_hw_set_priority(uint8_t irq, uint8_t prio)
{
	uint8_t iph = (prio & 0x02) >> 1;
	uint8_t ipl = prio & 0x01;

	if (irq < 7) {
		irq_hw_clear_priority0(1<<irq);
		irq_hw_set_priority0_high(iph << irq);
		irq_hw_set_priority0_low(ipl << irq);
	} else {
		irq -= 7;
		irq_hw_clear_priority1(1<<irq);
		irq_hw_set_priority1_high(iph << irq);
		irq_hw_set_priority1_low(ipl << irq);
	}
}

#ifdef CONFIG_TICK
__near__ uint8_t irq_ien0 = 0;
__near__ uint8_t irq_ien1 = 0;
__near__ uint8_t irq_msk0 = 0;
__near__ uint8_t irq_msk1 = 0;

void irq_hw_flags_enable(void)
{
	IEN0 |= irq_ien0;
	IEN1 |= irq_ien1;
}

void irq_hw_flags_disable(void)
{
	IEN0 &= ~(irq_ien0 & irq_msk0);
	IEN1 &= ~(irq_ien1 & irq_msk1);
}

#define irq_hw_register_flag0(ien0)	(irq_ien0 |= ien0)
#define irq_hw_register_flag1(ien1)	(irq_ien1 |= ien1)
#define irq_hw_register_mask0(ien0)	(irq_msk0 |= ien0)
#define irq_hw_register_mask1(ien1)	(irq_msk1 |= ien1)

void irq_hw_enable_vector(uint8_t irq, boolean maskable)
{
	if (irq < 7) {
		irq_hw_register_flag0(1<<irq);
		if (maskable)
			irq_hw_register_mask0(1<<irq);
	} else {
		irq_hw_register_flag1(1<<(irq-7));
		if (maskable)
			irq_hw_register_mask1(1<<(irq-7));
	}
}

void irq_hw_ctrl_init(void)
{
	/* disable all flags */
	IEN0 = 0;
	IEN1 = 0;
	irq_hw_ctrl_enable();
	/*irq_hw_flags_disable();*/
}
#else
void irq_hw_flags_enable(void)
{
	EA = 1;
}

void irq_hw_flags_disable(void)
{
	EA = 0;
}

#define irq_hw_register_flag0(ien0)	(IEN0 |= ien0)
#define irq_hw_register_flag1(ien1)	(IEN1 |= ien1)

void irq_hw_enable_vector(uint8_t irq, boolean maskable)
{
	if (irq < 7) {
		irq_hw_register_flag0(1<<irq);
	} else {
		irq_hw_register_flag1(1<<(irq-7));
	}
}

void irq_hw_ctrl_init(void)
{
}
#endif
