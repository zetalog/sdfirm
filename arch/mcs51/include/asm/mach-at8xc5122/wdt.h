#ifndef __WDT_AT8XC5122_H_INCLUDE__
#define __WDT_AT8XC5122_H_INCLUDE__

Sfr(WDTRST,	0xA6);		/* WatchDog Timer Reset */
Sfr(WDTPRG,	0xA7);		/* WatchDog Timer Program */

#define MSK_WDTPRG_S2		0x04	/* WDTPRG */
#define MSK_WDTPRG_S1		0x02
#define MSK_WDTPRG_S0		0x01

#endif /* __WDT_AT8XC5122_H_INCLUDE__ */
