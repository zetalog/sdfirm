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
 * @(#)falkor.c: falkor core power clamp reset implementation
 * $Id: falkor.c,v 1.279 2019-04-14 10:19:18 zhenglv Exp $
 */

#include <target/cpus.h>
#include <target/delay.h>
#include <target/panic.h>
#include <target/clk.h>
#include <target/jiffies.h>
#include <target/barrier.h>
#include <target/pmic.h>

#define FALKOR_INIT_APR				0
#define FALKOR_APR_BEGIN_COLD_BOOT		(FALKOR_INIT_APR+0)
#define FALKOR_APR_BEGIN_PMIC_SUPPLY		(FALKOR_INIT_APR+1)
#define FALKOR_APR_END_PMIC_SUPPLY		(FALKOR_INIT_APR+2)
#define FALKOR_APR_ASSERT_POR_CLAMP		(FALKOR_INIT_APR+3)
#define FALKOR_APR_DEASSERT_CLAMP		(FALKOR_INIT_APR+4)
#define FALKOR_APR_DEASSERT_POR			(FALKOR_INIT_APR+5)
#define FALKOR_APR_END_COLD_BOOT		(FALKOR_INIT_APR+6)
#define FALKOR_FINI_APR				FALKOR_APR_END_COLD_BOOT

#define FALKOR_INIT_APC				FALKOR_FINI_APR
#define FALKOR_INIT_QLL				FALKOR_INIT_APC
#define FALKOR_APC_BEGIN_QLL_INIT		(FALKOR_INIT_QLL+0)
#define FALKOR_APC_INIT_QLL_REGISTER		(FALKOR_INIT_QLL+1)
#define FALKOR_APC_CLEAR_QLL_ERROR		(FALKOR_INIT_QLL+2)
#define FALKOR_APC_END_QLL_INIT			(FALKOR_INIT_QLL+3)
#define FALKOR_FINI_QLL				FALKOR_APC_END_QLL_INIT

#define FALKOR_INIT_FREQ			FALKOR_FINI_QLL
#define FALKOR_FINI_FREQ			(FALKOR_INIT_FREQ+1)

#define FALKOR_INIT_L2				FALKOR_FINI_FREQ
#define FALKOR_APC_BEGIN_L2_COLD_BOOT		(FALKOR_INIT_L2+0)
#define FALKOR_APC_ASSERT_L2_SPM_WAKEUP		(FALKOR_INIT_L2+1)
#define FALKOR_APC_ASSERT_L2_POR_CLAMP_GFMUX	(FALKOR_INIT_L2+2)
#define FALKOR_APC_DEASSERT_L2_GFMUX		(FALKOR_INIT_L2+3)
#define FALKOR_APC_DEASSERT_L2_CLAMP		(FALKOR_INIT_L2+4)
#define FALKOR_APC_DEASSERT_L2_POR		(FALKOR_INIT_L2+5)
#define FALKOR_APC_DEASSERT_L2_SPM_WAKEUP	(FALKOR_INIT_L2+6)
#define FALKOR_APC_END_L2_COLD_BOOT		(FALKOR_INIT_L2+7)
#define FALKOR_FINI_L2				FALKOR_APC_END_L2_COLD_BOOT

#define FALKOR_INIT_CPU				FALKOR_FINI_L2
#define FALKOR_APC_BEGIN_CPU_COLD_BOOT		(FALKOR_INIT_CPU+0)
#define FALKOR_APC_ASSERT_CPU_SPM_WAKEUP	(FALKOR_INIT_CPU+1)
#define FALKOR_APC_ASSERT_CPU_POR_CLAMP		(FALKOR_INIT_CPU+2)
#define FALKOR_APC_DEASSERT_CPU_CLAMP		(FALKOR_INIT_CPU+3)
#define FALKOR_APC_DEASSERT_CPU_POR		(FALKOR_INIT_CPU+4)
#define FALKOR_APC_ASSERT_CPU_POWERED_ON	(FALKOR_INIT_CPU+5)
#define FALKOR_APC_DEASSERT_CPU_SPM_WAKEUP	(FALKOR_INIT_CPU+6)
#define FALKOR_APC_END_CPU_COLD_BOOT		(FALKOR_INIT_CPU+7)
#define FALKOR_FINI_CPU				FALKOR_APC_END_CPU_COLD_BOOT
#define FALKOR_FINI_APC				FALKOR_FINI_CPU

static uint64_t falkor_init_mask = 0;
static uint8_t falkor_async_state[MAX_CPU_NUM];
static tick_t falkor_async_timeout;

extern bool Clock_HMSSInit(void);

#ifdef CONFIG_AVM
static bool avm_is_enabled(void)
{
	return true;
}
#else
static bool avm_is_enabled(void)
{
	return false;
}
#endif

static inline uint8_t falkor_get_rail_state(uint8_t rail)
{
	return falkor_async_state[rail * CPUS_PER_RAIL];
}

static inline void falkor_set_rail_state(uint8_t rail, uint8_t state)
{
	uint8_t cpu;

	for (cpu = 0; cpu < CPUS_PER_RAIL; cpu++)
		falkor_async_state[rail * CPUS_PER_RAIL + cpu] = state;
}

static inline uint8_t falkor_get_cluster_state(uint8_t cluster)
{
	return falkor_async_state[cluster * CPUS_PER_CLUSTER];
}

static inline void falkor_set_cluster_state(uint8_t cluster, uint8_t state)
{
	uint8_t cpu;

	for (cpu = 0; cpu < CPUS_PER_CLUSTER; cpu++)
		falkor_async_state[cluster * CPUS_PER_CLUSTER + cpu] = state;
}

static inline uint8_t falkor_get_cpu_state(uint8_t cpu)
{
	return falkor_async_state[cpu];
}

static inline void falkor_set_cpu_state(uint8_t cpu, uint8_t state)
{
	falkor_async_state[cpu] = state;
}

static inline void falkor_set_async_timeout(unsigned long us)
{
#ifdef CONFIG_BUILD_PRESIL
	us /= 1000;
#endif
	falkor_async_timeout = tick_get_counter() + us;
}

static inline bool falkor_is_async_timeout(void)
{
	return !!time_after(tick_get_counter(), falkor_async_timeout);
}

#define FALKOR_L2_SW_SPM_EVENT_SPM_WAKEUP_BITMASK	0x2
#define FALKOR_CPU_SW_SPM_EVENT_SPM_WAKEUP_BITMASK	0x2

static inline void falkor_l2_assert_spm_wakeup(uint8_t cluster,
					       bool allow_wait)
{
	/* Assert APR SPM wakeup. */
	__raw_setl(SW_SPM_WAKEUP, apcs_apc_l2_sw_spm_event[cluster]);
	if (!allow_wait)
		falkor_set_async_timeout(FALKOR_APC_L2_SPM_DELAY_US);
}

static inline bool falkor_l2_is_spm_wakeup_asserted(uint8_t cluster,
						    bool allow_wait)
{
	if (allow_wait) {
		return (__raw_readl(apcs_apc_l2_pwr_ctl[cluster]) &
			PWR_CTL_SPM_ACTIVE) == PWR_CTL_SPM_ACTIVE;
	} else {
		return falkor_is_async_timeout();
	}
}

static inline void falkor_l2_deassert_spm_wakeup(uint8_t cluster,
						 bool allow_wait)
{
	/* De-assert Assert APR SPM wakeup. */
	__raw_clearl(SW_SPM_WAKEUP, apcs_apc_l2_sw_spm_event[cluster]);
	falkor_set_async_timeout(FALKOR_APC_L2_SPM_DELAY_US);
}

static inline bool falkor_l2_is_spm_wakeup_deasserted(uint8_t cluster,
						      bool allow_wait)
{
	return falkor_is_async_timeout();
}

static inline void falkor_cpu_assert_spm_wakeup(uint8_t cpu,
						bool allow_wait)
{
	/* Assert APR SPM wakeup. */
	__raw_setl(SW_SPM_WAKEUP, apcs_apc_cpu_sw_spm_event[cpu]);
	if (!allow_wait)
		falkor_set_async_timeout(FALKOR_APC_CPU_SPM_DELAY_US);
}

static inline bool falkor_cpu_is_spm_wakeup_asserted(uint8_t cpu,
						     bool allow_wait)
{
	if (allow_wait) {
		return __raw_readl(apcs_apc_cpu_pwr_ctl[cpu]) &
		       PWR_CTL_L2_ACTIVE;
	} else {
		return falkor_is_async_timeout();
	}
}

static inline void falkor_cpu_deassert_spm_wakeup(uint8_t cpu,
						  bool allow_wait)
{
	/* Assert APR SPM wakeup. */
	__raw_clearl(SW_SPM_WAKEUP, apcs_apc_cpu_sw_spm_event[cpu]);
	falkor_set_async_timeout(FALKOR_APC_CPU_SPM_DELAY_US);
}

static inline bool falkor_cpu_is_spm_wakeup_deasserted(uint8_t cpu,
						       bool allow_wait)
{
	return falkor_is_async_timeout();
}

/*======================================================================
 *  Function :  falkor_apc_l2_cold_boot_async
 * =====================================================================
 *
 * Refer to <<QDF2400 Programming Manual>>
 * 39 Falkor Processor Subsystem
 * 39.4.1.1 L2 cold boot sequences
 * Table 39-10 L2 cold boot sequence for APCy_L2
 */
bool falkor_apc_l2_cold_boot_async(uint8_t cluster, bool allow_wait)
{
	switch (falkor_get_cluster_state(cluster)) {
	case FALKOR_APC_BEGIN_L2_COLD_BOOT:
		wmb();
		falkor_l2_assert_spm_wakeup(cluster, allow_wait);
		falkor_set_cluster_state(cluster,
					 FALKOR_APC_ASSERT_L2_SPM_WAKEUP);
	case FALKOR_APC_ASSERT_L2_SPM_WAKEUP:
		if (!falkor_l2_is_spm_wakeup_asserted(cluster, allow_wait))
			break;
		/* Assert POR and GFMX Reset, Clamp, Close L2 APM HS and L2
		 * logic BHS
		 */
		wmb();
		__raw_writel(PWR_CTL_GF_MUX_RST | PWR_CTL_CLAMP_APMPD |
			     PWR_CTL_POR_RST | PWR_CTL_CLAMP_L2PD,
			     apcs_apc_l2_pwr_ctl[cluster]);
		if (!allow_wait)
			falkor_set_async_timeout(FALKOR_APC_L2_POWER_DELAY_US);
		falkor_set_cluster_state(cluster,
					 FALKOR_APC_ASSERT_L2_POR_CLAMP_GFMUX);
	case FALKOR_APC_ASSERT_L2_POR_CLAMP_GFMUX:
		if (allow_wait) {
			if (!(__raw_readl(apcs_apc_l2_pwr_ctl[cluster]) &
			      PWR_CTL_GF_MUX_RST_STATUS))
				break;
		} else if (!falkor_is_async_timeout())
			break;
		/* De-assert GFMUX_Reset */
		wmb();
		__raw_writel(PWR_CTL_CLAMP_APMPD | PWR_CTL_POR_RST |
			     PWR_CTL_CLAMP_L2PD,
			     apcs_apc_l2_pwr_ctl[cluster]);
		if (!allow_wait)
			falkor_set_async_timeout(FALKOR_APC_L2_POWER_DELAY_US);
		falkor_set_cluster_state(cluster, FALKOR_APC_DEASSERT_L2_GFMUX);
	case FALKOR_APC_DEASSERT_L2_GFMUX:
		if (allow_wait) {
			if (!(__raw_readl(apcs_apc_l2_pwr_ctl[cluster]) &
			      PWR_CTL_HY_RST_COMP_REQ))
				break;
		} else if (!falkor_is_async_timeout())
			break;
		/* De-assert Clamp; Assert RST_CMPLT_ACK */
		wmb();
		__raw_writel(PWR_CTL_POR_RST | PWR_CTL_RST_COMP_ACK,
			     apcs_apc_l2_pwr_ctl[cluster]);
		if (!allow_wait)
			falkor_set_async_timeout(FALKOR_APC_L2_POWER_DELAY_US);
		falkor_set_cluster_state(cluster, FALKOR_APC_DEASSERT_L2_CLAMP);
	case FALKOR_APC_DEASSERT_L2_CLAMP:
		if (allow_wait) {
			if (__raw_readl(apcs_apc_l2_pwr_ctl[cluster]) &
			    PWR_CTL_HY_RST_COMP_REQ)
				break;
		} else if (!falkor_is_async_timeout())
			break;
		/* De-assert POR Reset */
		wmb();
		__raw_writel(PWR_CTL_L2_PWRD_UP,
			     apcs_apc_l2_pwr_ctl[cluster]);
		falkor_set_async_timeout(FALKOR_APC_L2_POWER_DELAY_US);
		falkor_set_cluster_state(cluster, FALKOR_APC_DEASSERT_L2_POR);
	case FALKOR_APC_DEASSERT_L2_POR:
		if (!falkor_is_async_timeout())
			break;
		wmb();
		falkor_l2_deassert_spm_wakeup(cluster, allow_wait);
		falkor_set_cluster_state(cluster,
					 FALKOR_APC_DEASSERT_L2_SPM_WAKEUP);
	case FALKOR_APC_DEASSERT_L2_SPM_WAKEUP:
		if (!falkor_l2_is_spm_wakeup_deasserted(cluster, allow_wait))
			break;
		wmb();
		falkor_set_cluster_state(cluster, FALKOR_APC_END_L2_COLD_BOOT);
	default:
		return true;
	}
	return false;
}

/*======================================================================
 *  Function :  falkor_apc_cpu_cold_boot_async
 * =====================================================================
 *
 * Refer to <<QDF2400 Programming Manual>>
 * 39 Falkor Processor Subsystem
 * 39.4.1.1 L2 cold boot sequences
 * Table 39-10 L2 cold boot sequence for APCy_L2
 */
bool falkor_apc_cpu_cold_boot_async(uint8_t cpu, bool allow_wait)
{
	switch (falkor_get_cpu_state(cpu)) {
	case FALKOR_APC_BEGIN_CPU_COLD_BOOT:
		wmb();
		falkor_cpu_assert_spm_wakeup(cpu, allow_wait);
		falkor_set_cpu_state(cpu, FALKOR_APC_ASSERT_CPU_SPM_WAKEUP);
	case FALKOR_APC_ASSERT_CPU_SPM_WAKEUP:
		if (!falkor_cpu_is_spm_wakeup_asserted(cpu, allow_wait))
			break;
		/* Assert POR Reset, Clamp, Close CPU APM HS and CPU logic
		 * BHS
		 */
		wmb();
		__raw_writel(PWR_CTL_CLAMP_APMPD | PWR_CTL_POR_RST |
			     PWR_CTL_CLAMP_CPUPD,
			     apcs_apc_cpu_pwr_ctl[cpu]);
		if (!allow_wait)
			falkor_set_async_timeout(FALKOR_APC_CPU_POWER_DELAY_US);
		falkor_set_cpu_state(cpu, FALKOR_APC_ASSERT_CPU_POR_CLAMP);
	case FALKOR_APC_ASSERT_CPU_POR_CLAMP:
		if (allow_wait) {
			if (!(__raw_readl(apcs_apc_cpu_pwr_ctl[cpu]) &
			      PWR_CTL_HY_RST_COMP_REQ))
				break;
		} else if (!falkor_is_async_timeout())
			break;
		/* De-assert Clamp; Assert RST_CMPLT_ACK */
		wmb();
		__raw_writel(PWR_CTL_RST_COMP_ACK | PWR_CTL_POR_RST,
			     apcs_apc_cpu_pwr_ctl[cpu]);
		if (!allow_wait)
			falkor_set_async_timeout(FALKOR_APC_CPU_POWER_DELAY_US);
		falkor_set_cpu_state(cpu, FALKOR_APC_DEASSERT_CPU_CLAMP);
	case FALKOR_APC_DEASSERT_CPU_CLAMP:
		if (allow_wait) {
			if (__raw_readl(apcs_apc_cpu_pwr_ctl[cpu]) &
			    PWR_CTL_HY_RST_COMP_REQ)
				break;
		} else if (!falkor_is_async_timeout())
			break;
		/* De-assert POR reset */
		wmb();
		__raw_writel(0x0, apcs_apc_cpu_pwr_ctl[cpu]);
		falkor_set_async_timeout(FALKOR_APC_CPU_POWER_DELAY_US);
		falkor_set_cpu_state(cpu, FALKOR_APC_DEASSERT_CPU_POR);
	case FALKOR_APC_DEASSERT_CPU_POR:
		if (!falkor_is_async_timeout())
			break;
		/* Assert PWRDUP */
		wmb();
		__raw_writel(PWR_CTL_CORE_PWRD_UP,
			     apcs_apc_cpu_pwr_ctl[cpu]);
		falkor_set_async_timeout(FALKOR_APC_CPU_POWER_DELAY_US);
		falkor_set_cpu_state(cpu, FALKOR_APC_ASSERT_CPU_POWERED_ON);
	case FALKOR_APC_ASSERT_CPU_POWERED_ON:
		if (!falkor_is_async_timeout())
			break;
		wmb();
		falkor_cpu_deassert_spm_wakeup(cpu, allow_wait);
		falkor_set_cpu_state(cpu, FALKOR_APC_DEASSERT_CPU_SPM_WAKEUP);
	case FALKOR_APC_DEASSERT_CPU_SPM_WAKEUP:
		if (!falkor_cpu_is_spm_wakeup_deasserted(cpu, allow_wait))
			break;
		wmb();
		falkor_set_cpu_state(cpu, FALKOR_APC_END_CPU_COLD_BOOT);
	default:
		return true;
	}
	return false;
}

/*======================================================================
 *  Function :  falkor_apr_cold_boot_async
 * =====================================================================
 *
 * Refer to <<QDF2400 Programming Manual>>
 * 39 Falkor Processor Subsystem
 * 39.4.1 Cold boot sequences
 * Table 39-8 APR cold boot sequence
 */
/* PMIC/CVR sequences are different. Uses CVR sequence to meet test
 * requirements.
 */
bool falkor_apr_cold_boot_async(uint8_t rail, bool allow_wait)
{
	switch (falkor_get_rail_state(rail)) {
	case FALKOR_APR_BEGIN_COLD_BOOT:
		/* Turn APC rail at PMIC */
		wmb();
		if (!PM_IS_MONZA_APC_RAIL_SUPPLY()) {
			falkor_set_rail_state(rail,
					      FALKOR_APR_END_PMIC_SUPPLY);
			break;
		}
		/* Turn APC rail at PMIC. */
		__raw_writel(SAW4_VCTL_APC_ENABLE, saw4_vctl[rail]);
		if (!allow_wait)
			falkor_set_async_timeout(FALKOR_APR_PMIC_POWER_DELAY_US);
		falkor_set_rail_state(rail, FALKOR_APR_BEGIN_PMIC_SUPPLY);
	case FALKOR_APR_BEGIN_PMIC_SUPPLY:
		if (allow_wait) {
			if ((__raw_readl(saw4_avs_sts[rail]) &
			     SAW4_STS_AVS_FSM_STATE(SAW4_STS_AVS_FSM_STATE_MASK)) ==
			    SAW4_STS_AVS_FSM_STATE_LAST)
				break;
		} else if (!falkor_is_async_timeout())
			break;
		wmb();
		falkor_set_rail_state(rail, FALKOR_APR_END_PMIC_SUPPLY);
	case FALKOR_APR_END_PMIC_SUPPLY:
		/* Assert APCpd POR Reset, Clamp */
		wmb();
		__raw_writel(PWR_CTL_POR_RST | PWR_CTL_CLAMP_APCPD,
			     apcs_apr_pwr_ctl[rail]);
		falkor_set_async_timeout(FALKOR_APR_CVR_POWER_DELAY_US);
		falkor_set_rail_state(rail, FALKOR_APR_ASSERT_POR_CLAMP);
	case FALKOR_APR_ASSERT_POR_CLAMP:
		if (!falkor_is_async_timeout())
			break;
		/* De-assert APCpd Clamp */
		wmb();
		__raw_writel(PWR_CTL_POR_RST, apcs_apr_pwr_ctl[rail]);
		falkor_set_async_timeout(FALKOR_APR_CVR_POWER_DELAY_US);
		falkor_set_rail_state(rail, FALKOR_APR_DEASSERT_CLAMP);
	case FALKOR_APR_DEASSERT_CLAMP:
		if (!falkor_is_async_timeout())
			break;
		/* De-assert APCpd POR reset and assert APCCPwrCtlPmicOn */
		wmb();
		__raw_writel(PWR_CTL_PMIC_APC_ON, apcs_apr_pwr_ctl[rail]);
		falkor_set_async_timeout(FALKOR_APR_CVR_POWER_DELAY_US);
		falkor_set_rail_state(rail, FALKOR_APR_DEASSERT_POR);
	case FALKOR_APR_DEASSERT_POR:
		if (!falkor_is_async_timeout())
			break;
		wmb();
		falkor_set_rail_state(rail, FALKOR_APR_END_COLD_BOOT);
	default:
		return true;
	}
	return false;
}

/*======================================================================
 *  Function :  falkor_apc_init_qll_register_async
 * =====================================================================
 *
 * Refer to <<QDF2400 Programming Manual>>
 * 39 Falkor Processor Subsystem
 * 39.4.1.1 L2 cold boot sequences
 * Table 39-9 Falkor duplex QLL register initialization sequence
 */
bool falkor_apc_init_qll_register_async(uint8_t cluster)
{
	uint32_t data;
	uint64_t base = agera_hw_reg_base[cluster];

	switch (falkor_get_cluster_state(cluster)) {
	case FALKOR_APC_BEGIN_QLL_INIT:
		data = AGERA_APC_CLKCR_RESET;
		/* Disable primary GFMUX dynamic clock gating if AVM is
		 * enabled.
		 */
		if (avm_is_enabled())
			data &= ~AGERA_APC_EPRICLKAG;
		/* Enable GFMUX clock gating, Enable autoclock select. */
		wmb();
		__raw_writel(data, AGERA_APC_CLKCR(base));
		falkor_set_async_timeout(FALKOR_APC_QLL_CLOCK_DELAY_US);
		falkor_set_cluster_state(cluster, FALKOR_APC_INIT_QLL_REGISTER);
	case FALKOR_APC_INIT_QLL_REGISTER:
		if (!falkor_is_async_timeout())
			break;
		wmb();
		__raw_writel(AGERA_APC_QLL_ESR_RESET, AGERA_APC_QLL_ESR(base));
		falkor_set_async_timeout(FALKOR_APC_QLL_CLOCK_DELAY_US);
		falkor_set_cluster_state(cluster, FALKOR_APC_CLEAR_QLL_ERROR);
	case FALKOR_APC_CLEAR_QLL_ERROR:
		if (!falkor_is_async_timeout())
			break;
		wmb();
		falkor_set_cluster_state(cluster, FALKOR_APC_END_QLL_INIT);
	default:
		return true;
	}
	return false;
}

void falkor_configure_entry_point(uint64_t entry_point)
{
	uint64_t entry_point_mask;

	/* ACPS register only supports a 44 bit entry point with lower 5
	 * bits 0.
	 */
	entry_point_mask = MAKELLONG(__raw_readl(BOOT_CFG_START_ADDR_LO),
				     __raw_readl(BOOT_CFG_START_ADDR_HI));
	entry_point_mask = ~entry_point_mask;

	BUG_ON((entry_point & entry_point_mask) != 0);

	/* Load the register with final value. */
	__raw_writel(LODWORD(entry_point), BOOT_CFG_START_ADDR_LO);
	__raw_writel(HIDWORD(entry_point), BOOT_CFG_START_ADDR_HI);
}

bool falkor_configure_clock_async(uint8_t cpu, bool allow_wait)
{
	uint8_t rail = CPU_TO_RAIL(cpu);
	uint8_t cluster = CPU_TO_CLUSTER(cpu);
	uint8_t state = 0;

	if (falkor_init_mask & CPU_TO_MASK(cpu))
		return true;

	if (!(falkor_init_mask &
	      RAIL_TO_CPU_MASK(rail))) {
		/* Configure PMIC APC Rail. */
		state = falkor_get_rail_state(rail);
		if (state < FALKOR_FINI_APR) {
			if (!falkor_apr_cold_boot_async(rail, allow_wait))
				return false;
		}
	}

	if (!(falkor_init_mask &
	      CLUSTER_TO_CPU_MASK(cluster))) {
		/* Configure QLL MBSU/DMSU. */
		state = falkor_get_cluster_state(cluster);
		if (state < FALKOR_FINI_QLL) {
			if (!falkor_apc_init_qll_register_async(cluster))
				return false;
		}
		state = falkor_get_cluster_state(cluster);
		if (state < FALKOR_FINI_FREQ) {
#ifdef CONFIG_TESTOS
			/* The following code is used to parallel booting
			 * all cores in TestOS. It is not useful for SBL
			 * which only boots one core.
			 */
			agera_enable_apc(cluster, CPU_BOOT_FREQ_MHZ);
#endif
			falkor_set_cluster_state(cluster, FALKOR_FINI_FREQ);
		}
	}
	return true;
}

bool falkor_deassert_reset_async(uint8_t cpu, bool allow_wait)
{
	uint8_t cluster = CPU_TO_CLUSTER(cpu);
	uint8_t state = 0;

	if (falkor_init_mask & CPU_TO_MASK(cpu))
		return true;

	if (!falkor_configure_clock_async(cpu, allow_wait))
		return false;

	if (!(falkor_init_mask &
	      CLUSTER_TO_CPU_MASK(cluster))) {
		state = falkor_get_cluster_state(cluster);
		if (state < FALKOR_FINI_L2) {
			if (!falkor_apc_l2_cold_boot_async(cluster,
							   allow_wait))
				return false;
		}
	}

	state = falkor_get_cpu_state(cpu);
	if (state < FALKOR_FINI_CPU) {
		if (!falkor_apc_cpu_cold_boot_async(cpu, allow_wait))
			return false;
	}
	falkor_init_mask |= CPU_TO_MASK(cpu);
	return true;
}

bool falkor_is_cluster_clocked(uint16_t cluster)
{
	return !!(falkor_get_cluster_state(cluster) >= FALKOR_FINI_FREQ);
}

void falkor_configure_clock(uint8_t cpu, bool allow_wait)
{
	while (!falkor_configure_clock_async(cpu, allow_wait));
}

void falkor_deassert_reset(uint8_t cpu, bool allow_wait)
{
	BUG_ON(cpu >= MAX_CPU_NUM);
	Clock_HMSSInit();
	while (!falkor_deassert_reset_async(cpu, allow_wait));
}

void falkor_configure_pmic_apcc(uint64_t cpu_status, bool allow_wait)
{
	uint8_t rail = 0;

	for (rail = 0; rail < MAX_CPU_RAILS; rail++) {
		if (!(cpu_status & RAIL_TO_CPU_MASK(rail)))
			continue;
		if (falkor_get_rail_state(rail) >= FALKOR_FINI_APR)
			continue;
		while (!falkor_apr_cold_boot_async(rail, allow_wait));
	}
}

void falkor_configure_qll_sus(uint64_t cpu_status)
{
	uint8_t cluster;

	for (cluster = 0; cluster < MAX_CPU_CLUSTERS; cluster++) {
		if (!(cpu_status & CLUSTER_TO_CPU_MASK(cluster)))
			continue;
		if (falkor_get_cluster_state(cluster) >= FALKOR_FINI_QLL)
			continue;
		while (!falkor_apc_init_qll_register_async(cluster));
		if (falkor_get_cluster_state(cluster) >= FALKOR_FINI_FREQ)
			continue;
#ifdef CONFIG_TESTOS
		/* The following code is used to parallel booting all cores
		 * in TestOS. It is not useful for SBL which only boots one
		 * core.
		 */
		agera_enable_apc(cluster, CPU_BOOT_FREQ_MHZ);
#endif
		falkor_set_cluster_state(cluster, FALKOR_FINI_FREQ);
	}
}

void falkor_power_on_l2(uint8_t cpu, bool allow_wait)
{
	uint8_t cluster = CPU_TO_CLUSTER(cpu);

	BUG_ON(cpu >= MAX_CPU_NUM);
	if (falkor_get_cluster_state(cluster) >= FALKOR_FINI_L2)
		return;
	while (!falkor_apc_l2_cold_boot_async(cluster, allow_wait));
}

void falkor_power_on_core(uint8_t cpu, bool allow_wait)
{
	BUG_ON(cpu >= MAX_CPU_NUM);
	if (falkor_get_cpu_state(cpu) >= FALKOR_FINI_CPU)
		return;
	while (!falkor_apc_cpu_cold_boot_async(cpu, allow_wait));
}

/* This function enables the errata to correct the behavior of CPPC
 * feedback counters in the presence on hardware throttling.
 */
void falkor_configure_cppc_feedback(uint64_t cpu_status)
{
	uint8_t rail;

	for (rail = 0; rail < MAX_CPU_RAILS; rail++) {
		if (cpu_status & RAIL_TO_CPU_MASK(rail)) {
			__raw_setl(WAR_CPPC_FEEDBACK,
				   apcs_apr_war[rail]);
		}
	}
}

void falkor_sysdbg_init(uint8_t cluster)
{
	falkor_set_cluster_state(cluster, FALKOR_INIT_L2);
}

/* Warm reset sequence for L2 */
void falkor_warm_reset_l2(uint8_t cpu)
{
	uint32_t pwr_ctl;

	/* 2. Read APCS_APC[00-23]_L2_PWR_CTL (L2 register) */
	/*
	SDI_VERIFY(in_dword(apcs_apc_l2_pwr_ctl[cpu]) == 0x00000141);
	 */

	/* 5. Assert POR and GFMX Reset */
	__raw_writel_mask(PWR_CTL_POR_RST | PWR_CTL_GF_MUX_RST,
			  PWR_CTL_POR_RST | PWR_CTL_GF_MUX_RST,
			  apcs_apc_l2_pwr_ctl[cpu]);
	udelay(FALKOR_APC_L2_POWER_DELAY_US);

	while (!(__raw_readl(apcs_apc_l2_pwr_ctl[cpu]) &
		 PWR_CTL_GF_MUX_RST_STATUS));
	udelay(FALKOR_APC_L2_POWER_DELAY_US);

	/* 7. De-assert GFMUX_Reset */
	__raw_clearl(PWR_CTL_GF_MUX_RST, apcs_apc_l2_pwr_ctl[cpu]);
	udelay(FALKOR_APC_L2_POWER_DELAY_US);

	while (!(__raw_readl(apcs_apc_l2_pwr_ctl[cpu]) &
		 PWR_CTL_HY_RST_COMP_REQ));
	udelay(FALKOR_APC_L2_POWER_DELAY_US);

	/* 9. De-assert Clamp; Assert RST_CMPLT_ACK */
	pwr_ctl = __raw_readl(apcs_apc_l2_pwr_ctl[cpu]);
	pwr_ctl &= ~(PWR_CTL_CLAMP_APMPD | ~PWR_CTL_CLAMP_L2PD);
	pwr_ctl |= PWR_CTL_RST_COMP_ACK;
	__raw_writel(pwr_ctl, apcs_apc_l2_pwr_ctl[cpu]);
	udelay(FALKOR_APC_L2_POWER_DELAY_US);

	while (__raw_readl(apcs_apc_l2_pwr_ctl[cpu]) &
	       PWR_CTL_HY_RST_COMP_REQ);
	udelay(FALKOR_APC_L2_POWER_DELAY_US);

	/* 11. De-assert POR Reset */
	__raw_clearl(PWR_CTL_POR_RST, apcs_apc_l2_pwr_ctl[cpu]);
	udelay(FALKOR_APC_L2_POWER_DELAY_US);
}

/* Warm reset sequence for CPU */
void falkor_warm_reset_cpu(uint8_t cpu)
{
	uint32_t pwr_ctl;

	/* 12. Read APCS_APC[00-23]_CPU[0-1]_PWR_CTL   */
	/*
	SDI_VERIFY(in_dword(apcs_apc_cpu_pwr_ctl[cpu]) == 0x10000141);
	 */

	/* 14. Assert POR_RST */
	__raw_setl(PWR_CTL_POR_RST, apcs_apc_cpu_pwr_ctl[cpu]);
	udelay(FALKOR_APC_CPU_POWER_DELAY_US);

	while (!(__raw_readl(apcs_apc_cpu_pwr_ctl[cpu]) &
		 PWR_CTL_HY_RST_COMP_REQ));
	udelay(FALKOR_APC_CPU_POWER_DELAY_US);

	/* 16. De-assert Clamp; Assert RST_CMPLT_ACK */
	pwr_ctl = __raw_readl(apcs_apc_cpu_pwr_ctl[cpu]);
	pwr_ctl &= ~(PWR_CTL_CLAMP_APMPD | PWR_CTL_CLAMP_CPUPD);
	pwr_ctl |= PWR_CTL_RST_COMP_ACK;
	udelay(FALKOR_APC_CPU_POWER_DELAY_US);

	while (__raw_readl(apcs_apc_cpu_pwr_ctl[cpu]) &
	       PWR_CTL_HY_RST_COMP_REQ);
	udelay(FALKOR_APC_CPU_POWER_DELAY_US);

	/* 18. De-assert POR Reset */
	__raw_clearl(PWR_CTL_POR_RST, apcs_apc_cpu_pwr_ctl[cpu]);
	udelay(FALKOR_APC_CPU_POWER_DELAY_US);
}

/* Target specific function to place a core back in POR reset. Used to
 * prevent a core from going rouge if something goes wrong
 */
void falkor_POR_reset_cpu(uint64_t cpu)
{
	__raw_writel_mask(PWR_CTL_CLAMP_APMPD | PWR_CTL_CLAMP_CPUPD,
			  PWR_CTL_CLAMP_APMPD | PWR_CTL_CLAMP_CPUPD,
			  apcs_apc_cpu_pwr_ctl[cpu]);
}

bool falkor_apr_is_apc_on(uint8_t rail)
{
	return !!(__raw_readl(apcs_apr_pwr_ctl[rail]) == PWR_CTL_PMIC_APC_ON);
}

void falkor_release_first_cpu(void)
{
	uint64_t cpu_mask = get_cpu_mask();
	uint8_t cpu;
#ifdef CONFIG_PRESIL
	bool allow_wait = false;
#else
	bool allow_wait = true;
#endif

	/* Evaluate the cpu status for the first available core number.
	 * LSB of cpu_status is core 0 with each bit representing a core.
	 * A high bit '1' indicates the core is good.  A low bit '0'
	 * indicates the core is not.
	 */
	for (cpu = 0; cpu < MAX_CPU_NUM; cpu++) {
		if (cpu_mask & CPU_TO_MASK(cpu))
			break;
	}
	BUG_ON(cpu >= MAX_CPU_NUM);

	/* Configure PMIC APC Rail. */
	falkor_configure_pmic_apcc(cpu_mask, allow_wait);
	/* Configure QLL SUs. */
	falkor_configure_qll_sus(cpu_mask);
	/* Configure and release the core. */
	falkor_deassert_reset(cpu, allow_wait);
}
