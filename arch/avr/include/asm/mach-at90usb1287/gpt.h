#ifndef __GPT_AT90USB1287_H_INCLUDE__
#define __GPT_AT90USB1287_H_INCLUDE__

#include <target/config.h>
#include <asm/reg.h>
#include <asm/mach/pm.h>
#include <asm/mach/timer.h>

/* Timer/Counter Interrupt Flag, Mask Register */
#define TIFR3   _SFR_IO8(0x38)
#define TOV3	0
#define OCF3A	1
#define OCF3B	2
#define OCF3C	3
#define ICF3	5

#define TIMSK3  _SFR_MEM8(0x71)
#define TOIE3	0
#define OCIE3A	1
#define OCIE3B	2
#define OCIE3C	3
#define ICIE3	5

#define TCCR3A	_SFR_MEM8(0x90)
#define WGM30	0
#define WGM31	1
#define COM3C0	2
#define COM3C1	3
#define COM3B0	4
#define COM3B1	5
#define COM3A0	6
#define COM3A1	7

#define TCCR3B	_SFR_MEM8(0x91)
#define CS30	0
#define CS31	1
#define CS32	2
#define WGM32	3
#define WGM33	4
#define ICES3	6
#define ICNC3	7

#define TCCR3C	_SFR_MEM8(0x92)
#define F0C3C	5
#define F0C3B	6
#define F0C3A	7

#define TCNT3	_SFR_MEM16(0x94)
#define TCNT3L	_SFR_MEM8(0x94)
#define TCNT3H	_SFR_MEM8(0x95)

/* Input Capture Register */
#define ICR3	_SFR_MEM16(0x96)
#define ICR3L	_SFR_MEM8(0x96)
#define ICR3H	_SFR_MEM8(0x97)

/* Output Compare Register */
#define OCR3A   _SFR_MEM16(0x98)
#define OCR3AL  _SFR_MEM8(0x98)
#define OCR3AH  _SFR_MEM8(0x99)

#define OCR3B   _SFR_MEM16(0x9A)
#define OCR3BL  _SFR_MEM8(0x9A)
#define OCR3BH  _SFR_MEM8(0x9B)

#define OCR3C   _SFR_MEM16(0x9C)
#define OCR3CL  _SFR_MEM8(0x9C)
#define OCR3CH  _SFR_MEM8(0x9D)

#define GPT_CS		TIMER_HW_CS_64

AT90USB1287_TIMER_16BIT(3)

#endif /* __GPT_AT90USB1287_H_INCLUDE__ */
