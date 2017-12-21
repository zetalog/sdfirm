#ifndef __MPCORE_GBL_TIMER_VEXPRESSA9_H_INCLUDE__
#define __MPCORE_GBL_TIMER_VEXPRESSA9_H_INCLUDE__

#define MPCORE_GTMR_OFFSET(off)	(MPCORE_GTMR+(off))

#define GTMR_COUNTER_LO		MPCORE_GTMR_OFFSET(0x00)
#define GTMR_COUNTER_HI		MPCORE_GTMR_OFFSET(0x04)
#define GTMR_CONTROL		MPCORE_GTMR_OFFSET(0x08)
#define GTMR_IRQ_STATUS		MPCORE_GTMR_OFFSET(0x0C)
#define GTMR_COMPARE_LO		MPCORE_GTMR_OFFSET(0x10)
#define GTMR_COMPARE_HI		MPCORE_GTMR_OFFSET(0x14)
#define GTMR_AUTO_INCREMENT	MPCORE_GTMR_OFFSET(0x18)

#define __GTMR_HW_MAX_COUNT	0xFFFFFFFFFFFFFFFFULL

#define TIMER_EN		0
#define COMP_EN			1
#define IRQ_EN			2
#define AUTO_INCREMENT_EN	3
#define PRESCALER_OFFSET	8
#define PRESCALER_MASK		0xFF

static inline uint64_t __gtmr_hw_read_counter(void)
{
	uint32_t __h, __l;

	do {
		__h = __raw_readl(GTMR_COUNTER_HI);
		__l = __raw_readl(GTMR_COUNTER_LO);
	} while (__h != __raw_readl(GTMR_COUNTER_HI));
	return ((((uint64_t)__h) << 32) | __l);
}

static inline void __gtmr_hw_disable_tsc(void)
{
	__raw_clearl((_BV(TIMER_EN) | _BV(COMP_EN) |		\
		      _BV(IRQ_EN) | _BV(AUTO_INCREMENT_EN)),	\
		     GTMR_CONTROL);
}

static inline void __gtmr_hw_enable_tsc(void)
{
	__gtmr_hw_disable_tsc();
	__raw_setl(_BV(TIMER_EN), GTMR_CONTROL);
}

#endif /* __MPCORE_GBL_TIMER_VEXPRESSA9_H_INCLUDE__ */
