#ifndef __GPT_AT90SCR100_H_INCLUDE__
#define __GPT_AT90SCR100_H_INCLUDE__

#include <target/config.h>
#include <target/jiffies.h>
#include <asm/reg.h>
#include <asm/mach/pm.h>
#include <asm/mach/clk.h>
#include <asm/mach/timer.h>

/* Timer 2 (8bit) for GPT */

/* For timer 2, the prescaling range is 0, 1, 8, 32, 64, 128, 256, 1024 */

/* XXX: Timer 2 counter is up to 255.
 * GPT_FREQ / HZ < 255, so the GPT_FREQ must be less than 255K and HZ is 1000. */

#ifdef CONFIG_GPT_AT90SCR100_FREQ_250K
#define GPT_FREQ	250	/* K */
#if CLK_IO == 8000
#define GPT_CS		TIMER2_HW_CS_32
#elif CLK_IO == 16000
#define GPT_CS		TIMER2_HW_CS_64
#endif /* if */
#endif

#ifdef CONFIG_GPT_AT90SCR100_FREQ_188K
#define GPT_FREQ	188	/* K */
#if CLK_IO == 24000
#define GPT_CS		TIMER2_HW_CS_128
#endif /* if */
#endif

#ifdef CONFIG_GPT_AT90SCR100_FREQ_125K
#define GPT_FREQ	125	/* K */
#if CLK_IO == 8000
#define GPT_CS		TIMER2_HW_CS_64
#elif CLK_IO == 16000
#define GPT_CS		TIMER2_HW_CS_128
#endif /* if */
#endif

#ifdef CONFIG_GPT_AT90SCR100_FREQ_24K
#define GPT_FREQ	24	/* K */
#if CLK_IO == 24000
#define GPT_CS		TIMER2_HW_CS_1024
#endif /* if */
#endif

#ifndef GPT_CS
#error "Invalid GPT clock source frequency"
#endif

AT90SCR100_TIMER_8BIT(2)

#endif /* __GPT_AT90SCR100_H_INCLUDE__ */
