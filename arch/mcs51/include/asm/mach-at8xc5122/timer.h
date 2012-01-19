#ifndef __TIMER_AT89C5122_H_INCLUDE__
#define __TIMER_AT89C5122_H_INCLUDE__

/* Timer registers */
Sfr(TCON,	0x88);		/* control */
Sfr(TMOD,	0x89);		/* mode */

/*
 * Timer Modes:
 *
 * Mode 0: 13 bit timer
 * Mode 1: 16 bit timer
 * Mode 2: 8 bit timer with auto reload
 * Mode 3: two 8 bit timers (timer0) or halt (timer1)
 */
#define TIMER_HW_MODE_13BIT	0x00
#define TIMER_HW_MODE_16BIT	0x01
#define TIMER_HW_MODE_8BIT	0x02
#define TIMER_HW_MODE_2X8BIT	0x03
#define TIMER_HW_MODE_HALT	0x03

#define TIMER_HW_MODE_TIMER	0x00
#define TIMER_HW_MODE_COUNTER	0x04

#define TIMER_HW_MODE_GATE	0x08

#define TIMER_HW_TMOD_MASK	0x0F
#define TIMER_FREQ		((CLK_PERIPH/6)*1000) /* K */

#define __timer_hw_config_mode(i, mode)			\
	do {						\
		uint8_t offset = ((i) << 2);		\
		TMOD &= ~(TIMER_HW_TMOD_MASK<<offset);	\
		TMOD |= (mode)<<offset;			\
	} while (0)
/* Overflow Period Formula:
 * period_ms/1000 = 6 * (65536 - TH.TL) / (CLK_PERIPH * 1000000)
 * period_ms * 1000 = (65536 - TH.TL) / (1000 * CLK_PERIPH / 6)
 * Reload Counter Value:
 * TH.TL = 65536 - ((1000 * CLK_PERIPH / 6) * period_ms)
 */
#define __timer_hw_calc_reload(period_ms)		\
	(65535 - mul16u(TIMER_FREQ, (period_ms)) + 1)

#endif /* __TIMER_AT89C5122_H_INCLUDE__ */
