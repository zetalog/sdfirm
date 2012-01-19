#ifndef __TIMER_AT90USB1287_H_INCLUDE__
#define __TIMER_AT90USB1287_H_INCLUDE__

/* Asynchronous Status Register */
#define ASSR    _SFR_MEM8(0xB6)

/* Timer/Counter Control Register */
#define GTCCR	_SFR_IO8(0x23)

/* Timer/Counter Interrupt Flag, Mask Register */
#define TIFR0   _SFR_IO8(0x15)
#define TOV0	0
#define OCF0A	1
#define OCF0B	2

#define TIMSK0  _SFR_MEM8(0x6E)
#define TOIE0	0
#define OCIE0A	1
#define OCIE0B	2

#define TCCR0A	_SFR_IO8(0x24)
#define WGM00	0
#define WGM01	1
#define COM0B0	4
#define COM0B1	5
#define COM0A0	6
#define COM0A1	7

#define TCCR0B	_SFR_IO8(0x25)
#define CS00	0
#define CS01	1
#define CS02	2
#define WGM02	3
#define FOC0B	6
#define FOC0A	7

/* Timer/Counter Register */
#define TCNT0	_SFR_IO8(0X26)

/* Output Compare Register */
#define OCR0A	_SFR_IO8(0x27)
#define OCR0B	_SFR_IO8(0X28)

/* Timer/Counter Interrupt Flag, Mask Register */
#define TIFR2   _SFR_IO8(0x17)
#define TIMSK2  _SFR_MEM8(0x70)

#define TCCR2A  _SFR_MEM8(0xB0)
#define TCCR2B  _SFR_MEM8(0xB1)

/* Timer/Counter Register */
#define TCNT2   _SFR_MEM8(0xB2)

/* Output Compare Register */
#define OCR2A   _SFR_MEM8(0xB3)
#define OCR2B   _SFR_MEM8(0xB4)

/* fast/phase PWM modes are not used */
#define TIMER_HW_WGM_NORMAL	0x00
/* CTC mode for 8bits timer */
#define TIMER_HW_WGM_CTC_OC8	0x02
/* CTC mode for 16bits timer */
#define TIMER_HW_WGM_CTC_OC16	0x04
#define TIMER_HW_WGM_CTC_IC	0x0C

#define TIMER_HW_COM_NORMAL	0x00
#define TIMER_HW_COM_TOGGLE	0x01
#define TIMER_HW_COM_LOW	0x02
#define TIMER_HW_COM_HIGH	0x03

#define TIMER_HW_COM_A		0
#define TIMER_HW_COM_B		1
#define TIMER_HW_COM_C		2

/* TCCRnA */
#define TIMER_HW_COM_BASE	2
#define TIMER_HW_COM_MASK	0x03
#define TIMER_HW_COM(com, mode)	((mode)<<(((com)<<1)+TIMER_HW_COM_BASE))
#define TIMER_HW_WGM_LOW(wgm)	((wgm)&0x03)

/* TCCRnB */
#define TIMER_HW_ICNC_ENABLE	(1<<7)
#define TIMER_HW_ICES_FALLING	(1<<6)
#define TIMER_HW_WGM_HIGH(wgm)	(((wgm)&0x0C)<<1)
#define TIMER_HW_CS_0		0
#define TIMER_HW_CS_1		1
#define TIMER_HW_CS_8		2
#define TIMER_HW_CS_64		3
#define TIMER_HW_CS_256		4
#define TIMER_HW_CS_1024	5
#define TIMER_HW_CS_MASK	0x07

/* TCCRnC */
#define TIMER_HW_FOC_BASE	5
#define TIMER_HW_FOC(com)	(1<<((com)+TIMER_HW_FOC_BASE))

#define AT90USB1287_TIMER(n)					\
static inline void __timer##n##_hw_disable_irq(void)		\
{								\
	TIMSK##n = 0;						\
}								\
static inline void __timer##n##_hw_enable_tov(void)		\
{								\
	TIMSK##n |= _BV(TOIE##n);				\
}								\
static inline void __timer##n##_hw_enable_ocfa(void)		\
{								\
	TIMSK##n |= _BV(OCIE##n##A);				\
}								\
static inline void __timer##n##_hw_enable_ocfb(void)		\
{								\
	TIMSK##n |= _BV(OCIE##n##B);				\
}								\
static inline void __timer##n##_hw_disable_cs(void)		\
{								\
	TCCR##n##B &= ~TIMER_HW_CS_MASK;			\
}								\
static inline void __timer##n##_hw_enable_cs(uint8_t cs)	\
{								\
	TCCR##n##B |= cs;					\
}

#define AT90USB1287_TIMER_8BIT(n)				\
static inline void __timer##n##_hw_config_mode(uint8_t wgm)	\
{								\
	TCCR##n##A = TIMER_HW_WGM_LOW(wgm);			\
	TCCR##n##B = TIMER_HW_WGM_HIGH(wgm);			\
}								\
static inline void __timer##n##_hw_enable_oc(uint8_t com)	\
{								\
	TCCR##n##B |= TIMER_HW_FOC(com);			\
}								\
static inline void __timer##n##_hw_disable_oc(uint8_t com)	\
{								\
	TCCR##n##B &= ~TIMER_HW_FOC(com);			\
}								\
static inline uint8_t __timer##n##_hw_read_counter(void)	\
{								\
	return TCNT##n;						\
}								\
static inline void __timer##n##_hw_write_counter(uint8_t c)	\
{								\
	TCNT##n = c;						\
}								\
AT90USB1287_TIMER(n)

/* Suppose compiler optimization's harmful.
 * For a 16-bit write, the high byte must be written before the low byte.
 * For a 16-bit read, the low byte must be read before the high byte.
 */
#define AT90USB1287_TIMER_16BIT(n)				\
static inline void __timer##n##_hw_config_mode(uint8_t wgm)	\
{								\
	TCCR##n##A = TIMER_HW_WGM_LOW(wgm);			\
	TCCR##n##B = TIMER_HW_WGM_HIGH(wgm);			\
	TCCR##n##C = 0;						\
}								\
static inline void __timer##n##_hw_enable_foc(uint8_t com)	\
{								\
	TCCR##n##C |= TIMER_HW_FOC(com);			\
}								\
static inline void __timer##n##_hw_disable_foc(uint8_t com)	\
{								\
	TCCR##n##C &= ~TIMER_HW_FOC(com);			\
}								\
static inline void __timer##n##_hw_disable_icnc(void)		\
{								\
	TCCR##n##B &= ~TIMER_HW_ICNC_ENABLE;			\
}								\
static inline void __timer##n##_hw_set_falling(void)		\
{								\
	TCCR##n##B |= TIMER_HW_ICES_FALLING;			\
}								\
static inline void __timer##n##_hw_set_rising(void)		\
{								\
	TCCR##n##B &= ~TIMER_HW_ICES_FALLING;			\
}								\
static inline uint16_t __timer##n##_hw_read_counter(void)	\
{								\
	uint8_t low, high;					\
	low = TCNT##n##L;					\
	high = TCNT##n##H;					\
	return MAKEWORD(low, high);				\
}								\
static inline void __timer##n##_hw_write_counter(uint16_t c)	\
{								\
	TCNT##n##H = HIBYTE(c);					\
	TCNT##n##L = LOBYTE(c);					\
}								\
static inline void __timer##n##_hw_enable_ocfc(void)		\
{								\
	TIMSK##n |= _BV(OCIE##n##C);				\
}								\
static inline void __timer##n##_hw_enable_icf(void)		\
{								\
	TIMSK##n |= _BV(ICIE##n);				\
}								\
static inline void __timer##n##_hw_write_ocra(uint16_t c)	\
{								\
	OCR##n##AH = HIBYTE(c);					\
	OCR##n##AL = LOBYTE(c);					\
}								\
static inline void __timer##n##_hw_write_ocrb(uint16_t c)	\
{								\
	OCR##n##BH = HIBYTE(c);					\
	OCR##n##BL = LOBYTE(c);					\
}								\
static inline void __timer##n##_hw_write_ocrc(uint16_t c)	\
{								\
	OCR##n##CH = HIBYTE(c);					\
	OCR##n##CL = LOBYTE(c);					\
}								\
AT90USB1287_TIMER(n)

#endif /* __TIMER_AT90USB1287_H_INCLUDE__ */
