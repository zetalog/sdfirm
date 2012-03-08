#ifndef __TIMER_LM3S9B92_H_INCLUDE__
#define __TIMER_LM3S9B92_H_INCLUDE__

#define GPT_BASE	0x40030000
#define GPT(offset)	(GPT_BASE + offset)

/* GPTM Configuration */
#define GPTMCFG(n)	GPT(0x##n##000)
#define __TIMER_HW_TYPE_GPT32	0x00
#define __TIMER_HW_TYPE_RTC32	0x01
#define __TIMER_HW_TYPE_GPT16	0x04
#define __TIMER_HW_TYPE_MASK	0x07

/* GPTM Timer A Mode */
#define GPTMTAMR(n)	GPT(0x##n##004)
/* GPTM Timer B Mode */
#define GPTMTBMR(n)	GPT(0x##n##008)
#define __TIMER_HW_MODE_SNAP_SHOT	0x80
#define __TIMER_HW_MODE_WAIT_ON_TRIG	0x40
#define __TIMER_HW_MODE_MATCH_IRQ	0x20
#define __TIMER_HW_MODE_COUNT_DOWN	0x00
#define __TIMER_HW_MODE_COUNT_UP	0x10
#define __TIMER_HW_MODE_PWM		0x08
#define __TIMER_HW_CAPTURE_EDGE_COUNT	0x00
#define __TIMER_HW_CAPTURE_EDGE_TINER	0x04
#define __TIMER_HW_MODE_ONESHOT		0x01
#define __TIMER_HW_MODE_PERIODIC	0x02
#define __TIMER_HW_MODE_CAPTURE		0x03

/* GPTM Control */
#define GPTMCTL(n)	GPT(0x##n##00c)
#define TBPWML		14
#define TBOTE		13
#define TBSTALL		9
#define TBEN		8
#define TAPWML		6
#define TAOTE		5
#define RTCEN		4
#define TASTALL		1
#define TAEN		0
#define __TIMER_HW_EVENTS_OFFSET	2
#define __TIMER_HW_EVENTS_MASK		0x03
#define __TIMER_HW_EVENT_POSITIVE	0x01
#define __TIMER_HW_EVENT_NEGATIVE	0x02

/* GPTM Interrupt Mask */
#define GPTMIMR(n)	GPT(0x##n##018)
/* GPTM Raw Interrupt Status */
#define GPTMRIS(n)	GPT(0x##n##01c)
/* GPTM Masked Interrupt Status */
#define GPTMMIS(n)	GPT(0x##n##020)
/* GPTM Interrupt Clear */
#define GPTMICR(n)	GPT(0x##n##024)
#define TBM		11
#define CBE		10
#define CBM		9
#define TBTO		8
#define TAM		4
#define RTC		3
#define CAE		2
#define CAM		1
#define TATO		0
#define __TIMER_HW_IRQ_ALL		(_BV(TBM)  | \
					 _BV(CBE)  | \
					 _BV(CBM)  | \
					 _BV(TBTO) | \
					 _BV(TAM)  | \
					 _BV(RTC)  | \
					 _BV(CAE)  | \
					 _BV(CAM)  | \
					 _BV(TATO))

/* GPTM Timer A Interval Load */
#define GPTMTAILR(n)	GPT(0x##n##028)
/* GPTM Timer B Interval Load */
#define GPTMTBILR(n)	GPT(0x##n##02c)
/* GPTM Timer A Match */
#define GPTMTAMATCHR(n)	GPT(0x##n##030)
/* GPTM Timer B Match */
#define GPTMTBMATCHR(n)	GPT(0x##n##034)
/* GPTM Timer A Prescale */
#define GPTMTAPR(n)	GPT(0x##n##038)
/* GPTM Timer B Prescale */
#define GPTMTBPR(n)	GPT(0x##n##03C)
/* GPTM Timer A Prescale Match */
#define GPTMTAPMR(n)	GPT(0x##n##040)
/* GPTM Timer B Prescale Match */
#define GPTMTBPMR(n)	GPT(0x##n##044)
/* GPTM Timer A */
#define GPTMTAR(n)	GPT(0x##n##048)
/* GPTM Timer B */
#define GPTMTBR(n)	GPT(0x##n##04c)
/* GPTM Timer A Value */
#define	GPTMTAV(n)	GPT(0x##n##050)
/* GPTM Timer B Value */
#define GPTMTBV(n)	GPT(0x##n##054)

/* GPT or TSC mode */
#define LM3S9B92_GPT(n)							\
static inline void __timer##n##_hw_config_type(uint8_t type)		\
{									\
	__raw_writel((uint32_t)type, GPTMCFG(n));			\
}									\
static inline void __timer##n##a_hw_config_mode(uint8_t mode)		\
{									\
	__raw_writel((uint32_t)mode, GPTMTAMR(n));			\
}									\
static inline void __timer##n##_hw_irq_disable(uint8_t irq)		\
{									\
	__raw_clearl_atomic(irq, GPTMIMR(n));				\
}									\
static inline void __timer##n##_hw_irq_enable(uint8_t irq)		\
{									\
	__raw_setl_atomic(irq, GPTMIMR(n));				\
}									\
static inline boolean __timer##n##_hw_irq_raw(uint8_t irq)		\
{									\
	return __raw_testl_atomic(irq, GPTMRIS(n));			\
}									\
static inline boolean __timer##n##_hw_irq_masked(uint8_t irq)		\
{									\
	return __raw_testl_atomic(irq, GPTMMIS(n));			\
}									\
static inline void __timer##n##_hw_irq_clear(uint8_t irq)		\
{									\
	__raw_setl_atomic(irq, GPTMICR(n));				\
}									\
static inline void __timer##n##a_hw_stall_enable(void)			\
{									\
	__raw_clearl_atomic(TASTALL, GPTMCTL(n));			\
}									\
static inline void __timer##n##a_hw_stall_disable(void)			\
{									\
	__raw_setl_atomic(TASTALL, GPTMCTL(n));				\
}									\
static inline void __timer##n##_hw_ctrl_disable(void)			\
{									\
	__raw_clearl(_BV(TAEN) | _BV(TBEN), GPTMCTL(n));		\
	__raw_clearl(__TIMER_HW_IRQ_ALL, GPTMIMR(n));			\
}									\
static inline void __timer##n##a_hw_ctrl_enable(void)			\
{									\
	__raw_setl_atomic(TAEN, GPTMCTL(n));				\
}

#define LM3S9B92_GPT_16BIT(n)						\
static inline void __timer##n##b_hw_config_mode(uint8_t mode)		\
{									\
	__raw_writel((uint32_t)mode, GPTMTBMR(n));			\
}									\
static inline void __timer##n##b_hw_stall_enable(void)			\
{									\
	__raw_clearl_atomic(TBSTALL, GPTMCTL(n));			\
}									\
static inline void __timer##n##b_hw_stall_disable(void)			\
{									\
	__raw_setl_atomic(TBSTALL, GPTMCTL(n));				\
}									\
static inline void __timer##n##b_hw_ctrl_enable(void)			\
{									\
	__raw_setl_atomic(TBEN, GPTMCTL(n));				\
}									\
static inline void __timer##n##a_hw_write_reload(uint16_t ilr)		\
{									\
	__raw_writel((uint32_t)(ilr), GPTMTAILR(n));			\
}									\
static inline void __timer##n##b_hw_write_reload(uint16_t ilr)		\
{									\
	__raw_writel((uint32_t)(ilr), GPTMTBILR(n));			\
}									\
static inline void __timer##n##a_hw_write_match(uint16_t mr)		\
{									\
	__raw_writel((uint32_t)(mr), GPTMTAMATCHR(n));			\
}									\
static inline void __timer##n##b_hw_write_match(uint16_t mr)		\
{									\
	__raw_writel((uint32_t)(mr), GPTMTBMATCHR(n));			\
}									\
static inline void __timer##n##a_hw_write_prescale(uint8_t pr)		\
{									\
	__raw_writel((uint32_t)(pr), GPTMTAPR(n));			\
}									\
static inline void __timer##n##b_hw_write_prescale(uint8_t pr)		\
{									\
	__raw_writel((uint32_t)(pr), GPTMTBPR(n));			\
}									\
static inline void __timer##n##a_hw_write_prescale_match(uint8_t pmr)	\
{									\
	__raw_writel((uint32_t)(pmr), GPTMTAPMR(n));			\
}									\
static inline void __timer##n##b_hw_write_prescale_match(uint8_t pmr)	\
{									\
	__raw_writel((uint32_t)(pmr), GPTMTBPMR(n));			\
}									\
static inline uint16_t __timer##n##a_hw_read_count(void)		\
{									\
	return __raw_readl(GPTMTAR(n) & 0x0000FFFF);			\
}									\
static inline uint32_t __timer##n##b_hw_read_count(void)		\
{									\
	return __raw_readl(GPTMTBR(n) & 0x0000FFFF);			\
}									\
static inline uint16_t __timer##n##a_hw_read_value(void)		\
{									\
	return __raw_readl(GPTMTAV(n) & 0x0000FFFF);			\
}									\
static inline void __timer##n##a_hw_write_count(uint16_t c)		\
{									\
	__raw_writel((uint32_t)(c), GPTMTAV(n));			\
}									\
static inline uint16_t __timer##n##b_hw_read_value(void)		\
{									\
	return __raw_readl(GPTMTBV(n) & 0x0000FFFF);			\
}									\
static inline void __timer##n##b_hw_write_count(uint16_t c)		\
{									\
	__raw_writel((uint32_t)(c), GPTMTBV(n));			\
}									\
LM3S9B92_GPT(n)

#define LM3S9B92_GPT_32BIT(n)						\
static inline void __timer##n##a_hw_write_reload(uint32_t ilr)		\
{									\
	__raw_writel(ilr, GPTMTAILR(n));				\
}									\
static inline void __timer##n##a_hw_write_match(uint32_t mr)		\
{									\
	__raw_writel(mr, GPTMTAMATCHR(n));				\
}									\
static inline uint32_t __timer##n##a_hw_read_count(void)		\
{									\
	return __raw_readl(GPTMTAR(n));					\
}									\
static inline uint32_t __timer##n##a_hw_read_value(void)		\
{									\
	return __raw_readl(GPTMTAV(n));					\
}									\
static inline void __timer##n##a_hw_write_count(uint32_t c)		\
{									\
	__raw_writel((c), GPTMTAV(n));					\
}									\
LM3S9B92_GPT(n)

/* RTC mode */
#define LM3S9B92_RTC_32BIT(n)						\
static inline void __timer##n##_hw_rtc_disable(void)			\
{									\
	__raw_clearl_atomic(RTCEN, GPTMCTL(n));				\
}									\
static inline void __timer##n##_hw_rtc_enable(void)			\
{									\
	__raw_setl_atomic(RTCEN, GPTMCTL(n));				\
}									\
LM3S9B92_GPT_32BIT(n)

/* PWM or capture mode */
#define LM3S9B92_PWM(n)							\
static inline void __timer##n##a_hw_level_direct(void)			\
{									\
	__raw_clearl_atomic(TAPWML, GPTMCTL(n));			\
}									\
static inline void __timer##n##a_hw_level_invert(void)			\
{									\
	__raw_setl_atomic(TAPWML, GPTMCTL(n));				\
}									\
static inline void __timer##n##a_hw_output_disable(void)		\
{									\
	__raw_clearl_atomic(TAOTE, GPTMCTL(n));				\
}									\
static inline void __temer##n##a_hw_output_enable(void)			\
{									\
	__raw_setl_atomic(TAOTE, GPTMCTL(n));				\
}									\
static inline void __timer##n##a_hw_config_events(uint8_t events)	\
{									\
	__raw_writel_mask(events<<(__TIMER_HW_EVENTS_OFFSET),		\
		__TIMER_HW_EVENTS_MASK<<(__TIMER_HW_EVENTS_OFFSET),	\
		GPTMCTL(n));						\
}									\
LM3S9B92_GPT(n)

#define LM3S9B92_PWM_16BIT(n)						\
static inline void __timer##n##b_hw_level_direct(void)			\
{									\
	__raw_clearl_atomic(TBPWML, GPTMCTL(n));			\
}									\
static inline void __timer##n##b_hw_level_invert(void)			\
{									\
	__raw_setl_atomic(TBPWML, GPTMCTL(n));				\
}									\
static inline void __timer##n##b_hw_output_disable(void)		\
{									\
	__raw_clearl_atomic(TBOTE, GPTMCTL(n));				\
}									\
static inline void __temer##n##b_hw_output_enable(void)			\
{									\
	__raw_setl_atomic(TBOTE, GPTMCTL(n));				\
}									\
static inline void __timer##n##b_hw_config_events(uint8_t events)	\
{									\
	__raw_writel_mask(events<<(8+__TIMER_HW_EVENTS_OFFSET),		\
		__TIMER_HW_EVENTS_MASK<<(8+__TIMER_HW_EVENTS_OFFSET),	\
		GPTMCTL(n));						\
}									\
LM3S9B92_PWM(n)

#define LM3S9B92_PWM_32BIT(n)						\
LM3S9B92_PWM(n)

#define CLK_GPT32	CLK_SYS

#endif /* __TIMER_LM3S9B92_H_INCLUDE__ */
