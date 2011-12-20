#ifndef __GPT_AT8XC5122_H_INCLUDE__
#define __GPT_AT8XC5122_H_INCLUDE__

#include <target/config.h>
#include <target/generic.h>

#include <asm/reg.h>
#include <asm/mach/irq.h>
#include <asm/mach/clk.h>
#include <asm/mach/timer.h>

/* Timer0 registers */
Sbit(TR0,	0x88, 4);	/* reload */
Sfr(TL0,	0x8A);		/* counter low */
Sfr(TH0,	0x8C);		/* counter high */
#define MSK_TCON_TF0		0x20
#define MSK_TCON_TR0		0x10
#define MSK_TCON_IE0		0x02
#define MSK_TCON_IT0		0x01
#define MSK_TMOD_GATE0		0x08
#define MSK_TMOD_C0		0x04
#define MSK_TMOD_T0		0x04
#define MSK_TMOD_M10		0x02
#define MSK_TMOD_M00		0x01

#define GPT_MAX_TIMEOUT		(0x10000/TIMER_FREQ)

#endif /* __GPT_AT8XC5122_H_INCLUDE__ */
