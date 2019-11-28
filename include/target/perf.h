/*
 * ZETALOG's Personal COPYRIGHT
 *
 * Copyright (c) 2019
 *    ZETALOG - "Lv ZHENG".  All rights reserved.
 *    Author: Lv "Zetalog" Zheng
 *    Internet: zhenglv@hotmail.com
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
 * @(#)perf.h: performance measurement mechanism
 * $Id: perf.h,v 1.279 2019-04-14 10:19:18 zhenglv Exp $
 */

#ifndef __PERF_H_INCLUDE__
#define __PERF_H_INCLUDE__

#include <target/generic.h>
#include <asm/perf.h>

#define NR_PERF_EVTS		PERF_HW_MAX_COUNTERS
#define INVALID_PERF_EVT	NR_PERF_EVTS

#if NR_PERF_COUNTERS < 256
typedef uint8_t perf_evt_t;
#endif
typedef uint64_t perf_cnt_t;

#ifdef CONFIG_PERF
#define perf_event_count(event)	\
	perf_hw_get_event_count(event)

int perf_event_id(perf_evt_t event);
int perf_register_event(perf_evt_t event);
void perf_unregister_all_events(void);
int perf_init(void);
#else
static inline int perf_event_count(perf_evt_t event)
{
	return 0;
}

static inline int perf_event_id(perf_evt_t event)
{
	return -1;
}

static inline int perf_register_event(perf_evt_t event)
{
	return -ENODEV;
}

static inline void perf_unregister_all_events(void)
{
}

static inline int perf_init(void)
{
	return 0;
}
#endif

#endif /* __PERF_H_INCLUDE__ */
