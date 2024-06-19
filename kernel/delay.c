/*
 * ZETALOG's Personal COPYRIGHT
 *
 * Copyright (c) 2009
 *    ZETALOG - "Lv ZHENG".  All rights reserved.
 *    Author: Lv "Zetalog" Zheng
 *    Internet: zetalog@gmail.com
 *
 * This COPYRIGHT used to protect Personal Intelligence Rights.
 * Redistribution and use in source and binary forms with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *    This product includes software developed by the Lv "Zetalog" ZHENG.
 * 3. Neither the name of this software nor the names of its developers may
 *    be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 * 4. Permission of redistribution and/or reuse of souce code partially only
 *    granted to the developer(s) in the companies ZETALOG worked.
 * 5. Any modification of this software should be published to ZETALOG unless
 *    the above copyright notice is no longer declaimed.
 *
 * THIS SOFTWARE IS PROVIDED BY THE ZETALOG AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE ZETALOG OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * @(#)delay.c: delay calibration functions
 * $Id: delay.c,v 1.72 2011-10-26 09:06:42 zhenglv Exp $
 */
#include <target/delay.h>
#include <target/jiffies.h>
#include <target/irq.h>
#include <target/cmdline.h>
#include <time.h>

/* XXX: Wrap Over of "Delay Tick Elapsed"
 *
 * The simpler version of the "delay_ticks_elapsed" macro could be:
 * ============================================================
 * #define delay_ticks_elapsed(o, n)	\
 *	((tsc_count_t)(((n)-(o)) & (DELAY_TICKS_MAX-1)))
 * ============================================================
 * Almost all of the time systems use such elapsed time determination
 * algorithm.  This can only work when the wrap over value of the time is
 * (DELAY_TICKS_MAX+1)/2.  Notice that the simpler version would introduce
 * 1 count error if a wrap over value is (DELAY_TICKS_MAX+1).  Only the
 * above macro can cover those smallest bit width tick counters if a wrap
 * over value of (DELAY_TICKS_MAX+1) is expected.
 */
#define delay_ticks_elapsed(o, n)	\
	((tsc_count_t)((n>=o) ? (n-o) : (DELAY_TICKS_MAX-o+n+1)))

#if defined(CONFIG_LPS_NO_LPS) || defined(CONFIG_LPS_PRESET)
/* Stub calibrate_delay_start() as tick_init() should have already been
 * invoked.
 */
#ifdef CONFIG_TICK
#define calibrate_delay_start()
#else
#define calibrate_delay_start()		tsc_hw_ctrl_init()
#endif
#define calibrate_delay_stop()
#define __calibrate_delay()
#define DELAY_TICKS_MAX			TSC_MAX
#else /* !CONFIG_LPS_NO_LPS && !CONFIG_LPS_PRESET */
#ifdef CONFIG_TICK_PERIODIC
typedef tick_t lps_count_t;
#define DELAY_TICKS_PER_CALIBRATION	1
#define DELAY_TICKS_MAX			65535
#define delay_ticks()			jiffies
#define calibrate_delay_start()		irq_local_enable()
#define calibrate_delay_stop()		irq_local_disable()
#define delay_calc(loops)		(loops_per_ms = loops)
#else /* !CONFIG_TICK_PERIODIC */
typedef tsc_count_t lps_count_t;
#ifdef CALIBRATION_FREQ
#define DELAY_TICKS_PER_CALIBRATION	((tsc_count_t)(TSC_FREQ/CALIBRATION_FREQ))
#define delay_calc(loops)		(loops_per_ms = __lps_mul(loops, CALIBRATION_FREQ))
#else
/* Following definition is true since HZ=1000 and TSC_FREQ=tsc_freq/1000 */
#define DELAY_TICKS_PER_CALIBRATION	((tsc_count_t)TSC_FREQ)
#define delay_calc(loops)		(loops_per_ms = loops)
#endif
#define DELAY_TICKS_MAX			TSC_MAX
#define delay_ticks()			tsc_read_counter()
#define calibrate_delay_start()		tsc_hw_ctrl_init()
#define calibrate_delay_stop()
#endif /* CONFIG_TICK_PERIODIC */

#ifdef CONFIG_LPS_WEIGHT
#define LPS_INIT		CONFIG_LPS_WEIGHT
#else
#define LPS_INIT		0
#endif

lps_t loops_per_ms;
lps_t loops_per_us;

static lps_count_t __calibrate_elapsed(lps_t loops_per_calibration)
{
	lps_count_t ticks_old, ticks_new;

	ticks_old = delay_ticks();
	while (ticks_old == delay_ticks());
	ticks_old = delay_ticks();
	__delay(loops_per_calibration);
	ticks_new = delay_ticks();
	return delay_ticks_elapsed(ticks_old, ticks_new);
}

static void __calibrate_delay(void)
{
	lps_t loopbit;
	lps_t loops_per_calibration;
	lps_count_t elapsed, tickerr, elapsed_last;

	/* init loops_per_calibration */
	tickerr = elapsed_last = 0;
	loops_per_calibration = (1 << LPS_INIT);
	while ((loops_per_calibration <<= 1) != 0) {
		elapsed = __calibrate_elapsed(loops_per_calibration);

		/* XXX: Find Calibration Ticks Overhead
		 *
		 * We need to find ticks overhead to ensure the delayed
		 * time can always be greater than expected.
		 *
		 * Considering there is ticks overhead in the elapsed
		 * ticks returned by the above function, then the elapsed
		 * ticks should be composed of:
		 * elapsed = (ticks_per_loop * loops) + tickerr
		 * given loops2 = loops1<<1 as in this 1st round
		 * calibration, we could have:
		 * tickerr = 2*elapsed1 - elapsed2
		 *
		 * The process may fail if calibration times are less than
		 * tow.  The first tow calibration can tell us the most
		 * accurate tickerr.
		 */
		/* TODO: Calculate Estimated Calibration Result
		 *
		 * We can also estimate an loops_per_calibration from 2
		 * times calibration from DELAY_TICKS_PER_CALIBRATION
		 * which implies the tick source's frequency.  But this
		 * should be done by the biggest loops value rather than
		 * the smallest loops value.
		 */
		if (!tickerr) {
			if (elapsed_last) {
				tickerr = elapsed_last-(elapsed-elapsed_last);
			} else {
				elapsed_last = elapsed;
			}
		}

#ifdef DBG_CALIBRATION
		dbg_dump(0xAA);
		dbg_dump(HIBYTE(loops_per_calibration));
		dbg_dump(LOBYTE(loops_per_calibration));
		dbg_dump(tickerr);
		dbg_dump(elapsed);
		dbg_dump(HIBYTE(DELAY_TICKS_PER_CALIBRATION));
		dbg_dump(LOBYTE(DELAY_TICKS_PER_CALIBRATION));
		dbg_dump(0xAA);
#endif

		if (elapsed >= (DELAY_TICKS_PER_CALIBRATION + tickerr))
			break;
	}

	/* init loopbit */
	loopbit = (loops_per_calibration >> 1);
	while ((loopbit >>= 1) != 0) {
		loops_per_calibration -= loopbit;
		elapsed = __calibrate_elapsed(loops_per_calibration);

#ifdef DBG_CALIBRATION
		dbg_dump(0xBB);
		dbg_dump(HIBYTE(loops_per_calibration));
		dbg_dump(LOBYTE(loops_per_calibration));
		dbg_dump(tickerr);
		dbg_dump(elapsed);
		dbg_dump(HIBYTE(DELAY_TICKS_PER_CALIBRATION));
		dbg_dump(LOBYTE(DELAY_TICKS_PER_CALIBRATION));
		dbg_dump(0xBB);
#endif

		if (elapsed < (DELAY_TICKS_PER_CALIBRATION + tickerr))
			loops_per_calibration += loopbit;
	}

#ifdef DBG_CALIBRATION
	dbg_dump(0xCC);
	dbg_dump(HIBYTE(loops_per_calibration));
	dbg_dump(LOBYTE(loops_per_calibration));
	dbg_dump(0xCC);
#endif

	delay_calc(loops_per_calibration);
	loops_per_us = __lps_div(loops_per_ms, 1000);
}
#endif

#ifdef CONFIG_LPS_NO_LPS
void udelay(uint8_t us)
{
	tsc_count_t tsc = tsc_read_counter();

	while (delay_ticks_elapsed(tsc, tsc_read_counter()) <
	       TICKS_TO_MICROSECONDS * us);
}

void mdelay(uint8_t ms)
{
	while (ms--) {
		udelay(250);
		udelay(250);
		udelay(250);
		udelay(250);
	}
}
#else
void mdelay(uint8_t ms)
{
	while (ms--) {
		__delay(loops_per_ms);
	}
}

void udelay(uint8_t us)
{
	while (us--) {
		__delay(loops_per_us);
	}
}
#endif

void delay(uint8_t s)
{
	while (s--) {
		mdelay(250);
		mdelay(250);
		mdelay(250);
		mdelay(250);
	}
}

static void calibrate_delay(void)
{
	__calibrate_delay();
}

void delay_init(void)
{
	calibrate_delay_start();
	calibrate_delay();
	calibrate_delay_stop();
}

static int do_time(int argc, char *argv[])
{
	if (argc < 2)
		return -EINVAL;

	if (strcmp(argv[1], "tsc") == 0) {
		printf("%lld\n", (unsigned long long)tsc_read_counter());
		return 0;
	}
	if (strcmp(argv[1], "clock") == 0) {
		printf("%lld\n", (unsigned long long)clock());
		return 0;
	}
	return -EINVAL;
}

DEFINE_COMMAND(time, do_time, "Time measurement commands",
	"time tsc\n"
	"    -current TSC timestamp\n"
	"time clock\n"
	"    -current standard clock value (us)\n"
);
