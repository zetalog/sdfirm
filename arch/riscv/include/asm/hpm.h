/*
 * ZETALOG's Personal COPYRIGHT
 *
 * Copyright (c) 2020
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
 * @(#)hpm.h: RISCV hardware performance monitor definition
 * $Id: hpm.h,v 1.1 2020-07-23 22:54:00 zhenglv Exp $
 */

#ifndef __HPM_RISCV_H_INCLUDE__
#define __HPM_RISCV_H_INCLUDE__

#include <target/arch.h>

#define HPM_MAX_COUNTERS		32

#define HPM_NO_EVENT			0
#define HPM_CYCLE			0
#define HPM_TIME			1
#define HPM_INSTRET			2
#define PMU_HW_DEFAULT_EVENT		HPM_CYCLE

#ifdef CONFIG_RISCV_SSCOFPMF
#define COFPMF_OF		_BV_ULL(63)
#define COFPMF_MINH		_BV_ULL(62)
#define COFPMF_SINH		_BV_ULL(61)
#define COFPMF_UINH		_BV_ULL(60)
#define COFPMF_VSINH		_BV_ULL(59)
#define COFPMF_VUINH		_BV_ULL(58)
#endif

#ifdef CONFIG_RISCV_EXIT_M
#ifdef CONFIG_RISCV_SSCOFPMF
void hpm_configure_filter(perf_evt_t evt, uint8_t inh);
void hpm_count_init(void)
#else
#define hpm_configure_filter(inh)	do { } while (0)
#define hpm_count_init()		do { } while (0)
#endif
#else
#define hpm_configure_filter(inh)	do { } while (0)
#define hpm_count_init()		do { } while (0)
#endif
#define hpm_enable_event(event)		csr_clear(CSR_MCOUNTINHIBIT, event)
#define hpm_disable_event(event)	csr_clear(CSR_MCOUNTINHIBIT, event)

void hpm_init(void);
void hpm_start(void);
void hpm_stop(void);
void hpm_configure_event(perf_evt_t event);
perf_cnt_t hpm_get_event_count(perf_evt_t event);
void hpm_set_event_count(perf_evt_t event, perf_cnt_t count);

#endif /* __HPM_RISCV_H_INCLUDE__ */
