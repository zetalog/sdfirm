#ifndef __TSC_AT90SCR100_H_INCLUDE__
#define __TSC_AT90SCR100_H_INCLUDE__

#include <target/config.h>
#include <asm/reg.h>
#include <asm/mach/pm.h>
#include <asm/mach/clk.h>
#include <asm/mach/timer.h>

/* Timer 1 (16bit) for TSC */

/* For timer 1, the prescaling range is 0, 1, 8, 64, 256, 1024 */
#ifdef CONFIG_TSC_AT90SCR100_FREQ_8M
#define TSC_FREQ	8000
#define TSC_MAX		65535
#if CLK_IO == 8000
#define TSC_CS		TIMER01_HW_CS_1
#endif /* if */
#endif

#ifdef CONFIG_TSC_AT90SCR100_FREQ_1M
#define TSC_FREQ	1000
#define TSC_MAX		65535
#if CLK_IO == 8000
#define TSC_CS		TIMER01_HW_CS_8
#elif CLK_IO == 64000
#define TSC_CS		TIMER01_HW_CS_64
#endif /* if */
#endif

#ifdef CONFIG_TSC_AT90SCR100_FREQ_375K
#define TSC_FREQ	375
#define TSC_MAX		65535
#define TSC_CS		TIMER01_HW_CS_64
#endif

#ifdef CONFIG_TSC_AT90SCR100_FREQ_125K
#define TSC_FREQ	125
#define TSC_MAX		65535
#if CLK_IO == 1000
#define TSC_CS		TIMER01_HW_CS_8
#elif CLK_IO == 8000
#define TSC_CS		TIMER01_HW_CS_64
#endif /* if */
#endif

#ifndef TSC_CS
#error "Invalid TSC clock source frequency"
#endif

AT90SCR100_TIMER_16BIT(1)

tsc_count_t tsc_hw_read_counter(void);

#endif /* __TSC_AT90SCR100_H_INCLUDE__ */
