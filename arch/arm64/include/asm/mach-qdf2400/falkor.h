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
 * @(#)falkor.h: falkor core power clamp reset interfaces
 * $Id: falkor.h,v 1.279 2019-04-14 10:19:18 zhenglv Exp $
 */

#ifndef __QDF2400_FALKOR_H_INCLUDE__
#define __QDF2400_FALKOR_H_INCLUDE__

#include <asm/mach/hmss_periph.h>

#ifndef __ASSEMBLY__
#define FALKOR_RESPECTABLE_DELAY_US		10 /* 10000 */
/* Such delays are supposed to be software delay to help TZ-IMC to
 * reach the sync point. Unless IMC changes its mind to have longer
 * default wait time, such delay must be introduced into the APC
 * power_on sequences.
 */
#define FALKOR_UNRESPECTABLE_DELAY_US		10 /* 10000 */

/* The delays are useful for TZ-IMC to reach the sync point */
#define FALKOR_APR_CVR_POWER_DELAY_US		FALKOR_UNRESPECTABLE_DELAY_US
#define FALKOR_APC_QLL_CLOCK_DELAY_US		FALKOR_UNRESPECTABLE_DELAY_US
#define FALKOR_APC_L2_POWER_DELAY_US		FALKOR_UNRESPECTABLE_DELAY_US
#define FALKOR_APC_CPU_POWER_DELAY_US		FALKOR_UNRESPECTABLE_DELAY_US
/* The delay is used for waiting external peripheral */
#define FALKOR_APR_PMIC_POWER_DELAY_US		FALKOR_RESPECTABLE_DELAY_US
#define FALKOR_APC_L2_SPM_DELAY_US		FALKOR_RESPECTABLE_DELAY_US
#define FALKOR_APC_CPU_SPM_DELAY_US		FALKOR_RESPECTABLE_DELAY_US

/* All of the bitmask values for the PMIC APCC, QLL, L2 and Core blocks do
 * not change. No need to duplicate redundant data.
 */
#define SAW4_VCTL_APC_ENABLE		SAW4_VCTL_PMIC_DATA(0xAE)

/* Power features which affect Falkor initialization.
 * TODO:
 *   Get from q_fw_common.h or some other shared header.
 */
#define PWR_FEATURE_BOOT_VAR		L"power_features_bm"
#define PWR_FEATURE_AVM			4

void falkor_configure_entry_point(uint64_t entry_point);
void falkor_deassert_reset(uint8_t cpu, bool allow_wait);
bool falkor_deassert_reset_async(uint8_t cpu, bool allow_wait);
void falkor_configure_clock(uint8_t cpu, bool allow_wait);
bool falkor_configure_clock_async(uint8_t cpu, bool allow_wait);
bool falkor_is_cluster_clocked(uint16_t cluster);

void falkor_configure_cppc_feedback(uint64_t cpu_status);

void falkor_configure_pmic_apcc(uint64_t cpu_status, bool allow_wait);
void falkor_configure_qll_sus(uint64_t cpu_status);
void falkor_power_on_l2(uint8_t cpu, bool allow_wait);
void falkor_power_on_core(uint8_t cpu, bool allow_wait);

void falkor_sysdbg_init(uint8_t cluster);
void falkor_warm_reset_l2(uint8_t cpu);
void falkor_warm_reset_cpu(uint8_t cpu);
void falkor_POR_reset_cpu(uint64_t cpu);
bool falkor_apr_is_apc_on(uint8_t rail);

void falkor_release_first_cpu(void);

extern const uint64_t apcs_apc_cpu_pwr_ctl[MAX_CPU_NUM];
extern const uint64_t apcs_apc_l2_pwr_ctl[MAX_CPU_CLUSTERS];
extern const uint64_t apcs_apr_pwr_ctl[MAX_CPU_RAILS];
extern const uint64_t apcs_apc_l2_sw_spm_event[MAX_CPU_CLUSTERS];
extern const uint64_t apcs_apc_cpu_sw_spm_event[MAX_CPU_NUM];
extern const uint64_t saw4_avs_sts[MAX_CPU_RAILS];
extern const uint64_t saw4_vctl[MAX_CPU_RAILS];
extern const uint64_t apcs_apr_war[MAX_CPU_RAILS];
#endif /* __ASSEMBLY__ */

#endif /* __QDF2400_FALKOR_H_INCLUDE__ */
