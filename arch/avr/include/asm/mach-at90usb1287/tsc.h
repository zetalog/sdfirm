#ifndef __TSC_AT90USB1287_H_INCLUDE__
#define __TSC_AT90USB1287_H_INCLUDE__

#include <target/config.h>
#include <asm/reg.h>
#include <asm/mach/pm.h>
#include <asm/mach/timer.h>

/* Timer/Counter Interrupt Flag, Mask Register */
#define TIFR1   _SFR_IO8(0x16)
#define TOV1	0
#define OCF1A	1
#define OCF1B	2
#define OCF1C	3
#define ICF1	5

#define TIMSK1  _SFR_MEM8(0x6F)
#define TOIE1	0
#define OCIE1A	1
#define OCIE1B	2
#define OCIE1C	3
#define ICIE1	5

#define TCCR1A	_SFR_MEM8(0x80)
#define WGM10	0
#define WGM11	1
#define COM1C0	2
#define COM1C1	3
#define COM1B0	4
#define COM1B1	5
#define COM1A0	6
#define COM1A1	7

#define TCCR1B	_SFR_MEM8(0x81)
#define CS10	0
#define CS11	1
#define CS12	2
#define WGM12	3
#define WGM13	4
#define ICES1	6
#define ICNC1	7

#define TCCR1C	_SFR_MEM8(0x82)
#define F0C1C	5
#define F0C1B	6
#define F0C1A	7

#define TCNT1	_SFR_MEM16(0x84)
#define TCNT1L	_SFR_MEM8(0x84)
#define TCNT1H	_SFR_MEM8(0x85)

/* Input Capture Register */
#define ICR1	_SFR_MEM16(0x86)
#define ICR1L	_SFR_MEM8(0x86)
#define ICR1H	_SFR_MEM8(0x87)

/* Output Compare Register */
#define OCR1A   _SFR_MEM16(0x88)
#define OCR1AL  _SFR_MEM8(0x88)
#define OCR1AH  _SFR_MEM8(0x89)

#define OCR1B   _SFR_MEM16(0x8A)
#define OCR1BL  _SFR_MEM8(0x8A)
#define OCR1BH  _SFR_MEM8(0x8B)

#define OCR1C   _SFR_MEM16(0x8C)
#define OCR1CL  _SFR_MEM8(0x8C)
#define OCR1CH  _SFR_MEM8(0x8D)

#ifdef CONFIG_TSC_AT90USB1287_FREQ_8M
#define TSC_FREQ	8000
#define TSC_MAX		65535
#define TSC_CS		TIMER_HW_CS_1
#endif
#ifdef CONFIG_TSC_AT90USB1287_FREQ_1M
#define TSC_FREQ	1000
#define TSC_MAX		65535
#define TSC_CS		TIMER_HW_CS_8
#endif
#ifdef CONFIG_TSC_AT90USB1287_FREQ_125K
#define TSC_FREQ	125
#define TSC_MAX		65535
#define TSC_CS		TIMER_HW_CS_64
#endif

AT90USB1287_TIMER_16BIT(1)

tsc_count_t tsc_hw_read_counter(void);

#endif /* __TSC_AT90USB1287_H_INCLUDE__ */
