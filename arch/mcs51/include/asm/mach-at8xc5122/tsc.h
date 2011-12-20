#ifndef __TSC_AT8XC5122_H_INCLUDE__
#define __TSC_AT8XC5122_H_INCLUDE__

#include <target/config.h>
#include <target/generic.h>

#include <asm/reg.h>
#include <asm/mach/clk.h>
#include <asm/mach/irq.h>
#include <asm/mach/timer.h>

/* Timer1 registers */
Sbit(TR1,	0x88, 6);	/* reload */
Sfr(TL1,	0x8B);		/* counter low */
Sfr(TH1,	0x8D);		/* counter high */
#define MSK_TCON_TF1		0x80	/* TCON */
#define MSK_TCON_TR1		0x40
#define MSK_TCON_IE1		0x08
#define MSK_TCON_IT1		0x04
#define MSK_TMOD_GATE1		0x80	/* TMOD */
#define MSK_TMOD_C1		0x40
#define MSK_TMOD_T1		0x40
#define MSK_TMOD_M11		0x20
#define MSK_TMOD_M01		0x10

/* TSC driver specific definitions */
#ifdef CONFIG_CPU_AT8XC5122_FREQ_48M
#define CALIBRATION_FREQ	80
#endif
#ifdef CONFIG_CPU_AT8XC5122_FREQ_24M
#define CALIBRATION_FREQ	40
#endif
#define TSC_MAX			0xFF
#define TSC_FREQ		TIMER_FREQ

#define tsc_hw_read_counter()	TL1

#endif /* __TSC_AT8XC5122_H_INCLUDE__ */
