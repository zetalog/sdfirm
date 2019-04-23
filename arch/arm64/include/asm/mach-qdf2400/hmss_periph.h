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
 * @(#)hmss_periph.h: QDF2400 specific platofrm HMSS periph interface
 * $Id: hmss_periph.h,v 1.279 2019-04-14 10:19:18 zhenglv Exp $
 */

#ifndef __QDF2400_HMSS_PERIPH_H_INCLUDE__
#define __QDF2400_HMSS_PERIPH_H_INCLUDE__

#ifndef HMSS_PERIPH_BASE
#define HMSS_PERIPH_BASE		0xFF7E000000
#endif

#define HMSS_PERIPH_REG(off)		(HMSS_PERIPH_BASE + (off))

/* HMSS_PM_CBF */
#define APCS_PM_CBF_L3U_REG(blk, off)	\
	(APCS_PM_CBF_L3U_BASE(blk) + (off))
#define APCS_PM_CBF_L3U_PWR_CTL(blk)	APCS_PM_CBF_L3U_REG(blk, 0x0000)
#define APCS_PM_CBF_L3U_L3_CONFIG(blk)	APCS_PM_CBF_L3U_REG(blk, 0x0010)

/* HPSS (Hydra Processor Subsystem) */
#define ACPS_BLOCK_SIZE					0x8000
#define APCS_BLOCK_BASE(apr, off)			\
	(HMSS_PERIPH_REG(off) + (apr) * ACPS_BLOCK_SIZE)
#define APCS_APR_REG(apr, reg)				\
	(APCS_##reg##_BASE(apr) + 0x6000)
#define APCS_APC_L2_REG(apc, reg)			\
	(APCS_##reg##_BASE((apc) / CLUSTERS_PER_RAIL) +	\
	 0x4000 + 0x800 * ((apc) % CLUSTERS_PER_RAIL))
#define APCS_APC_CPU_REG(cpu, reg)			\
        (APCS_##reg##_BASE((cpu) / CPUS_PER_RAIL) +	\
         0x800 * ((cpu) % CPUS_PER_RAIL))

/* SAW4 (SPM/AVM Wrapper) */
#define SAW4_SPM_CTL(apr)		SAW4_SPM_REG(apr, 0x0000)
#define SAW4_SPM_DLY(apr)		SAW4_SPM_REG(apr, 0x0004)
#define SAW4_SPM_WAKEUP_CNT(apr)	SAW4_SPM_REG(apr, 0x0008)
#define SAW4_SPM_PMIC_DATA(apr, n)	SAW4_SPM_REG(apr, 0x40 + 0x4 * (n))
#define SAW4_SPM_SLP_SEQ_ENTRY(apr, n)	SAW4_SPM_REG(apr, 0x400 + 0x4 * (n))
#define SAW4_VCTL(apr)			SAW4_AVS_REG(apr, 0x0100)
#define SAW4_AVS_VCTL(apr)		SAW4_AVS_REG(apr, 0x0104)
#define SAW4_AVS_LIMIT(apr)		SAW4_AVS_REG(apr, 0x0108)
#define SAW4_AVS_DLY(apr)		SAW4_AVS_REG(apr, 0x010C)
#define SAW4_VCTL_DLY(apr)		SAW4_AVS_REG(apr, 0x0110)
#define SAW4_AVS_CLR_STS(apr)		SAW4_AVS_REG(apr, 0x0114)
#define SAW4_CVS_CTL(apr)		SAW4_AVS_REG(apr, 0x0120)
#define SAW4_CVS_DLY(apr)		SAW4_AVS_REG(apr, 0x0124)
#define SAW4_CVS_PMIC_DATA(n)		SAW4_AVS_REG(apr, 0x140 + 0x4 * (n))
#define SAW4_CVS_SEQ_ENTRY(n)		SAW4_AVS_REG(apr, 0x200 + 0x4 * (n))
#define SAW4_SECURE(apr)		SAW4_STS_REG(apr, 0x0000)
#define SAW4_ID(apr)			SAW4_STS_REG(apr, 0x0004)
#define SAW4_SPM_STS(apr)		SAW4_STS_REG(apr, 0x000C)
#define SAW4_SPM_STS2(apr)		SAW4_STS_REG(apr, 0x0010)
#define SAW4_AVS_STS(apr)		SAW4_STS_REG(apr, 0x0014)
#define SAW4_PMIC_STS(apr)		SAW4_STS_REG(apr, 0x0018)
#define SAW4_RST(apr)			SAW4_STS_REG(apr, 0x001C)
#define SAW4_SPM_STS3(apr)		SAW4_STS_REG(apr, 0x0020)
#define SAW4_CVS_STS(apr)		SAW4_STS_REG(apr, 0x0040)
#define SAW4_CVS_STS2(apr)		SAW4_STS_REG(apr, 0x0044)
#define SAW4_CVS_STS3(apr)		SAW4_STS_REG(apr, 0x0048)
#define SAW4_VERSION(apr)		SAW4_STS_REG(apr, 0x03D0)

/* APCS_CBF_PWR_CTL */
#define PWR_CTL_CLAMP_CBFPD		_BV(0)
#define PWR_CTL_CBF_WARM_RST		_BV(1)
#define PWR_CTL_CBF_POR_RST		_BV(2)
#define PWR_CTL_RET_SAVE		_BV(3)
#define PWR_CTL_RET_RESTORE		_BV(4)
#define PWR_CTL_PMIC_APC_ON		_BV(31)

/* APCS_CBF_L3U_PWR_CTL */
#define PWR_CTL_L3U_CLAMP_APMPD		_BV(3)
#define PWR_CTL_L3U_WARM_RST		_BV(4)
#define PWR_CTL_L3U_POR_RST		_BV(5)
#define PWR_CTL_L3U_HS_RST		_BV(6)
#define PWR_CTL_FORCE_CLOCKS_OFF	_BV(30)

/* (APCS_APC_CPU/APCS_APC_L2/APCS_APR)_PWR_CTL */
#define PWR_CTL_POR_RST			_BV(2)
#define PWR_CTL_SLP_STATE_STATUS	_BV(29)
/* (APCS_APC_CPU/APCS_APC_L2)_PWR_CTL */
#define PWR_CTL_WARM_RST		_BV(1)
#define PWR_CTL_HS_RST			_BV(3)
#define PWR_CTL_HS_DSBL			_BV(4)
#define PWR_CTL_MA_DSBL			_BV(5)
#define PWR_CTL_CLAMP_APMPD		_BV(6)
#define PWR_CTL_RST_COMP_ACK		_BV(24)
#define PWR_CTL_HY_RST_COMP_REQ		_BV(30)
#define PWR_CTL_DISABLE_SPM_WAKEUP	_BV(31)
/* APCS_CPU_PWR_CTL */
#define PWR_CTL_CLAMP_CPUPD		_BV(0)
#define PWR_CTL_CORE_PWRD_UP		_BV(8)
#define PWR_CTL_L2_ACTIVE		_BV(27)
#define PWR_CTL_MAS_PGS_ACTIVE_STATUS	_BV(28)
/* APCS_L2_PWR_CTL */
#define PWR_CTL_CLAMP_L2PD		_BV(0)
#define PWR_CTL_GF_MUX_RST		_BV(7)
#define PWR_CTL_L2_PWRD_UP		_BV(8)
#define PWR_CTL_APR_ACTIVE		_BV(26)
#define PWR_CTL_CBF_ACTIVE		_BV(27)
#define PWR_CTL_SPM_ACTIVE		\
	(PWR_CTL_APR_ACTIVE | PWR_CTL_CBF_ACTIVE)
#define PWR_CTL_GF_MUX_RST_STATUS	_BV(28)
/* APCS_APR_PWR_CTL */
#define PWR_CTL_CLAMP_APCPD		_BV(0)
#define PWR_CTL_PMIC_APC_ON		_BV(31)

/* (APCS_CPU/APCS_L2/APR)_SW_SPM_EVENT */
#define SW_SPM_EVENT			_BV(0)
/* (APCS_CPU/APCS_L2)_SW_SPM_EVENT */
#define SW_SPM_WAKEUP			_BV(1)

/* WAR */
#define WAR_CLAMP_APCPD			_BV(0)
#define WAR_GATE_PMIC_ON		_BV(1)
#define WAR_DISABLE_REF_GEN		_BV(2)
#define WAR_LDO_VREF_RET_UPDT		_BV(3)
#define WAR_LDO_VREF_FUNC_UPDT		_BV(4)
#define WAR_POR_RST			_BV(5)
#define WAR_MEMCELL_VMIN_VREF_UPDT	_BV(6)
#define WAR_APC_PLL_PWR_DWN		_BV(7)
#define WAR_PWRING_UP			_BV(8)
#define WAR_BOUNCE_UPDATE		_BV(9)
#define WAR_DROOP_UPDATE		_BV(10)
#define WAR_VOLTAGE_RAMP		_BV(30)
#define WAR_SLEEP_STATE			_BV(31)
/* workaround from original falkor.h */
#define WAR_CPPC_FEEDBACK		_BV(23)

/* SAW4_VCTL */
#define SAW4_VCTL_PMIC_DATA_OFFSET	0
#define SAW4_VCTL_PMIC_DATA_MASK	REG_8BIT_MASK
#define SAW4_VCTL_PMIC_DATA(value)	_SET_FV(SAW4_VCTL_PMIC_DATA, value)
#define SAW4_VCTL_ADR_IDX_OFFSET	16
#define SAW4_VCTL_ADR_IDX_MASK		REG_3BIT_MASK
#define SAW4_VCTL_ADR_IDX(value)	_SET_FV(SAW4_VCTL_ADR_IDX, value)
#define SAW4_VCTL_SIZE			_BV(20)
/* SAW4_AVS_STS */
#define SAW4_STS_AVS_FSM_STATE_OFFSET	0
#define SAW4_STS_AVS_FSM_STATE_MASK	REG_2BIT_MASK
#define SAW4_STS_AVS_FSM_STATE(value)	_GET_FV(SAW4_STS_AVS_FSM_STATE, value)
#define SAW4_STS_AVS_FSM_STATE_LAST	SAW4_STS_AVS_FSM_STATE(3)
#define SAW4_STS_AVS_DN_REQ		_BV(2)
#define SAW4_STS_AVS_UP_REQ		_BV(3)
#define SAW4_STS_MIN_INT		_BV(4)
#define SAW4_STS_MAX_INT		_BV(5)
#define SAW4_STS_SW_WR_PEND		_BV(6)
#define SAW4_STS_VOLTAGE_RAMP		_BV(7)
#define SAW4_STS_AVS_ACK		_BV(8)
#define SAW4_STS_AVS_ERR		_BV(9)
#define SAW4_STS_SIC_IRQ		_BV(10)
#define SAW4_STS_SIC_SW_DONE_IRQ	_BV(11)
#define SAW4_STS_MIN_LMT_STKY		_BV(12)
#define SAW4_STS_MAX_LMT_STKY		_BV(13)
#define SAW4_STS_MIN_ERR_STKY		_BV(14)
#define SAW4_STS_MAX_ERR_STKY		_BV(15)
#define SAW4_STS_AVS_LD_REQ		_BV(16)
#define SAW4_STS_AVS_LDCEIL_REQ		_BV(17)
#define SAW4_STS_AVS_FLOOR_REQ		_BV(18)
#define SAW4_STS_AVS_CURR_DLY_OFFSET	20
#define SAW4_STS_AVS_CURR_DLY_MASK	REG_10BIT_MASK
#define SAW4_STS_AVS_CURR_DLY(value)	_GET_FV(SAW4_STS_AVS_CURR_DLY, value)

/* HMSS_PERIPH_CSR */
#define BOOT_CFG_START_ADDR_LO		HMSS_PERIPH_REG(0x960008)
#define BOOT_CFG_START_ADDR_HI		HMSS_PERIPH_REG(0x96000C)

/* HMSS_PM_CBF */
#define APCS_PM_CBF_PWR_CTL		HMSS_PERIPH_REG(0x8EE008)
#define APCS_PM_CBF_L3U_BASE(blk)	\
	HMSS_PERIPH_REG(0x8E0000 + ((blk) * 0x1000))

/* PWR_CTL */
#define APCS_PWR_CTL_BASE(apr)		APCS_BLOCK_BASE(apr, 0x040000)
#define APCS_APR_PWR_CTL(apr)		APCS_APR_REG(apr, PWR_CTL)
#define APCS_APC_L2_PWR_CTL(apc)	APCS_APC_L2_REG(apc, PWR_CTL)
#define APCS_APC_CPU_PWR_CTL(cpu)	APCS_APC_CPU_REG(cpu, PWR_CTL)

/* SW_SPM_EVENT */
#define APCS_SW_SPM_EVENT_BASE(apr)	APCS_BLOCK_BASE(apr, 0x100000)
#define APCS_APR_SW_SPM_EVENT(apr)	APCS_APR_REG(apr, SW_SPM_EVENT)
#define APCS_APC_L2_SW_SPM_EVENT(apc)	APCS_APC_L2_REG(apc, SW_SPM_EVENT)
#define APCS_APC_CPU_SW_SPM_EVENT(cpu)	APCS_APC_CPU_REG(cpu, SW_SPM_EVENT)

/* WAR */
#define APCS_WAR_BASE(apr)		APCS_BLOCK_BASE(apr, 0x140000)
#define APCS_APR_WAR(apr)		APCS_APR_REG(apr, WAR)
#define APCS_APC_L2_WAR(apc)		APCS_APC_L2_REG(apc, WAR)
#define APCS_APC_CPU_WAR(cpu)		APCS_APC_CPU_REG(cpu, WAR)

/* SAW4 (SPM/AVM Wrapper) */
#define APCS_SAW4_SPM_BASE(apr)		APCS_BLOCK_BASE(apr, 0x000000)
#define SAW4_SPM_REG(apr, off)		(APCS_APR_REG(apr, SAW4_SPM) + (off))
#define APCS_SAW4_AVS_BASE(apr)		APCS_BLOCK_BASE(apr, 0x240000)
#define SAW4_AVS_REG(apr, off)		(APCS_APR_REG(apr, SAW4_AVS) + (off))
#define APCS_SAW4_STS_BASE(apr)		APCS_BLOCK_BASE(apr, 0x2C0000)
#define SAW4_STS_REG(apr, off)		(APCS_APR_REG(apr, SAW4_STS) + (off))

#endif /* __QDF2400_HMSS_PERIPH_H_INCLUDE__ */
