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

#ifndef PMU_HW_MAX_COUNTERS
#define PMU_HW_MAX_COUNTERS		32
#endif

#define HPM_NO_EVENT			0
#define HPM_CYCLE			0
#define HPM_TIME			1
#define HPM_INSTRET			2
#define PMU_HW_DEFAULT_EVENT		HPM_CYCLE

#define pmu_hw_get_counters()		PMU_HW_MAX_COUNTERS
#define pmu_hw_ctrl_init()		do { } while (0)
#define pmu_hw_enable_event(event)	csr_clear(CSR_MCOUNTINHIBIT, event)
#define pmu_hw_disable_event(event)	csr_clear(CSR_MCOUNTINHIBIT, event)

#ifdef CONFIG_HPM_EVENT
void pmu_hw_configure_event(uint8_t event);
#else
#define pmu_hw_configure_event(event)	do { } while (0)
#endif
uint32_t pmu_hw_get_event_count(uint8_t event);
void pmu_hw_set_event_count(uint8_t event, uint32_t count);

#endif /* __HPM_RISCV_H_INCLUDE__ */
