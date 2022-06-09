/*
 * ZETALOG's Personal COPYRIGHT
 *
 * Copyright (c) 2022
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
 * @(#)ddr4.h: double data rate 4 framework interface
 * $Id: ddr4.h,v 1.0 2022-05-30 08:46:00 zhenglv Exp $
 */

#ifndef __DDR4_H_INCLUDE__
#define __DDR4_H_INCLUDE__

#ifdef CONFIG_DDR4
#define __ddr4_spd(spd)		((spd) - DDR4_MIN_SPD)
#define ddr4_spd		__ddr4_spd(ddr_spd)
#define ddr4_spd_valid(spd)	\
	(!!(((spd) >= DDR4_MIN_SPD) && ((spd) <= DDR4_MAX_SPD)))

/* 2.8 DDR4 SDRAM Addressing */
#define DDR4_2GB			0
#define DDR4_4GB			1
#define DDR4_8GB			2
#define DDR4_16GB			3
#define DDR4_MAX_ADDRESSINGS		4

#define DDR4_BANK_GROUP_x4		4
#define DDR4_BANK_GROUP_x8		4
#define DDR4_BANK_GROUP_x16		2
#define DDR4_PAGE_SIZE_x4		512
#define DDR4_PAGE_SIZE_x8		(DDR4_PAGE_SIZE_x4 << 1)
#define DDR4_PAGE_SIZE_x16		(DDR4_PAGE_SIZE_x8 << 1)

#define DDR4_DEV(x)			((x) << 16)
#define DDR4_CONF(cap, x)		(DDR4_DEV(x) | (cap))
/* 2.8.1 2Gb Addressing Table */
#define DDR4_2GB_512M_x4		DDR4_CONF(512, 4)
#define DDR4_2GB_258M_x8		DDR4_CONF(256, 8)
#define DDR4_2GB_128M_x16		DDR4_CONF(128, 16)
/* 2.8.2 4Gb Addressing Table */
#define DDR4_4GB_1G_x4			DDR4_CONF(1024, 4)
#define DDR4_4GB_512M_x8		DDR4_CONF(512, 8)
#define DDR4_4GB_256M_x16		DDR4_CONF(256, 16)
/* 2.8.3 8Gb Addressing Table */
#define DDR4_8GB_2G_x4			DDR4_CONF(2048, 4)
#define DDR4_8GB_1G_x8			DDR4_CONF(1024, 8)
#define DDR4_8GB_512M_x16		DDR4_CONF(512, 16)
/* 2.8.4 4Gb Addressing Table */
#define DDR4_16GB_4G_x4			DDR4_CONF(4096, 4)
#define DDR4_16GB_2G_x8			DDR4_CONF(2048, 8)
#define DDR4_16GB_1G_x16		DDR4_CONF(1024, 16)

/* 3.1 Simplified State Diagram */
#define DDR4_state_PowerOn		0
#define DDR4_state_ResetProcedure	1
#define DDR4_state_ConnectivityTest	2
#define DDR4_state_Initialization	3
#define DDR4_state_ZQCalibration	4
#define DDR4_state_Idle			5
#define DDR4_state_MPSM			6
#define DDR4_state_PADMode		7
#define DDR4_state_MRS			8
#define DDR4_state_MPR			9
#define DDR4_state_WriteLeveling	10
#define DDR4_state_VrefDQTraining	11
#define DDR4_state_SelfRefreshing	12
#define DDR4_state_Refreshing		13
#define DDR4_state_PrechargePowerDown	14
#define DDR4_state_Activating		15
#define DDR4_state_BankActive		16
#define DDR4_state_ActivePowerDown	17
#define DDR4_state_Reading		18
#define DDR4_state_Writing		19
#define DDR4_state_Precharging		20

#define DDR4_func_ACT			0 /* Activate */
#define DDR4_func_PRE			1 /* Precharge */
#define DDR4_func_PREA			2 /* PRECHARGE All */
#define DDR4_func_MRS			3 /* Mode Register Set */
#define DDR4_func_REF			4 /* Refresh,Fine granularity Refresh */
#define DDR4_func_TEN			5 /* Boundary Scan Mode Enable */
#define DDR4_func_Read			6 /* RD,RDS4,RDS8 */
#define DDR4_func_ReadA			7 /* RDA,RDAS4,RDAS8 */
#define DDR4_func_Write			8 /* WR,WRS4,WRS8 with/without CRC */
#define DDR4_func_WriteA		9 /* WRA,WRAS4,WRAS8 with/without CRC */
#define DDR4_func_RESET_n		10 /* Start RESET procedure */
#define DDR4_func_PDE			11 /* Enter Power-down */
#define DDR4_func_PDX			12 /* Exit Power-down */
#define DDR4_func_SRE			13 /* Self-Refresh entry */
#define DDR4_func_SRX			14 /* Self-Refresh exit */
#define DDR4_func_MPR			15 /* Multi Purpose Register */

/* Modes can be controlled by MR */
#define DDR4_MODE_TsensReadout		_BV(0)
#define DDR4_MODE_GeardownRate_4	_BV(1)
#define DDR4_MODE_PDA			_BV(2)
#define DDR4_MODE_VrefDQTraining	_BV(3)
#define DDR4_MODE_ReadDBI		_BV(4)
#define DDR4_MODE_WriteDBI		_BV(5)
#define DDR4_MODE_DataMask		_BV(6)
#define DDR4_MODE_CAPAR			_BV(7)
#define DDR4_MODE_CAPARLatency		_BV(8)
#define DDR4_MODE_ODTInputBuffer	_BV(9)
#define DDR4_MODE_hPPR			_BV(10)
#define DDR4_MODE_sPPR			_BV(11)
#define DDR4_MODE_WritePreamble_2T	_BV(12)
#define DDR4_MODE_ReadPreamble_2T	_BV(13)
#define DDR4_MODE_ReadPreambleTraining	_BV(14)
#define DDR4_MODE_SelfRefreshAbort	_BV(15)
#define DDR4_MODE_InternalVrefMonitor	_BV(16)
#define DDR4_MODE_TCR			_BV(17)
#define DDR4_MODE_TCR_RangeExtended	_BV(18)
#define DDR4_MODE_MPDM			_BV(19)
#define DDR4_MODE_WriteCRC		_BV(20)
#define DDR4_MODE_Qoff			_BV(21)
#define DDR4_MODE_TDQS			_BV(22)
#define DDR4_MODE_WriteLeveling		_BV(23)
#define DDR4_MODE_DLL			_BV(24)
#define DDR4_MODE_DLLReset		_BV(25)
#define DDR4_MODE_TM			_BV(26)
#define DDR4_MODE_CS_CA_Latency		_BV(27)
#define DDR4_MODE_RTT_WR_DynamicODT	_BV(28)
#define DDR4_MODE_ReadBurstInterleave	_BV(29)

/* Refresh modes */
#define DDR4_REFRESH_Fixed_1x			0x000
#define DDR4_REFRESH_Fixed_2x			0x001
#define DDR4_REFRESH_Fixed_4x			0x002
#define DDR4_REFRESH_OnTheFly			0x004
#define DDR4_REFRESH_OnTheFly_2x		\
	(DDR4_REFRESH_OnTheFly | DDR4_REFRESH_Fixed_2x)
#define DDR4_REFRESH_OnTheFly_4x		\
	(DDR4_REFRESH_OnTheFly | DDR4_REFRESH_Fixed_4x)
#define DDR4_MAX_REFRESHES			3
#define DDR4_MAX_REFRESH_MODES			7
#define DDR4_REFRESH_MASK			0x003
#define DDR4_FGR_MODE_MASK			\
	(DDR4_REFRESH_OnTheFly | DDR4_REFRESH_MASK)
#define DDR4_REFRESH_TCR_ModeEnabled		0x080
#define DDR4_REFRESH_TCR_RangeExtended		0x100
#define DDR4_REFRESH_TCR_Normal			(DDR4_REFRESH_TCR_ModeEnabled)
#define DDR4_REFRESH_TCR_Extended		\
	(DDR4_REFRESH_TCR_ModeEnabled | DDR4_REFRESH_TCR_RangeExtended)
#define DDR4_TCR_MODE_MASK			\
	(DDR4_REFRESH_TCR_ModeEnabled | DDR4_REFRESH_TCR_RangeExtended)

/* MPR Related */
#define DDR4_MPR_ReadFormat_Serial	0
#define DDR4_MPR_ReadFormat_Parallel	1
#define DDR4_MPR_ReadFormat_Staggered	2
#define DDR4_MPR_Operation_Normal	0
#define DDR4_MPR_Operation_MPR		1

/* Table 15 - VrefDQ Training: Range */
#define DDR4_TRAINING_VrefDQ_Range1	0
#define DDR4_TRAINING_VrefDQ_Range2	1

/* C/A Parity */
#define DDR4_CAPAR_Clear		0 /* C/A Parity Error Status */
#define DDR4_CAPAR_Error		1 /* C/A Parity Error Status */
#define DDR4_CRC_Clear			0 /* CRC Error Clear */
#define DDR4_CRC_Error			1 /* CRC Error Clear */

/* Low Power Auto Self Refresh LP_ASR */
#define DDR4_LP_MANUAL_Normal		0
#define DDR4_LP_MANUAL_Reduced		1
#define DDR4_LP_MANUAL_Extended		2
#define DDR4_LP_ASR			3

/* RTT_WR special value */
#define DDR4_RTT_WR_HiZ			0
#define DDR4_RTT_NOM_Disable		0
#define DDR4_RTT_PARK_Disable		0

/* Additive Latency */
#define DDR4_AdditiveLatency_Disable	0
#define DDR4_AdditiveLatency_CL_1	1
#define DDR4_AdditiveLatency_CL_2	2

/* Read Burst Type */
#define DDR4_ReadBurst_Sequential	0
#define DDR4_ReadBurst_Interleave	1

/* Burst Length */
#define DDR4_BL8MRS			0
#define DDR4_BL8_BC4_OTF		1
#define DDR4_BC4MRS			2

/* 3.5 Mode Register */

/* MR Select: Encoded in BG0, BA1:BA0
 * BG0,BA1:BA0 = 000: MR0
 * BG0,BA1:BA0 = 001: MR1
 * BG0,BA1:BA0 = 010: MR2
 * BG0,BA1:BA0 = 011: MR3
 * BG0,BA1:BA0 = 100: MR4
 * BG0,BA1:BA0 = 101: MR5
 * BG0,BA1:BA0 = 110: MR6
 * BG0,BA1:BA0 = 111: RCW
 *  Reserved for Register control word setting.
 *  DRAM ignores MR command with BG0,BA1;BA0=111 and doesn’t respond.
 *  When RFU MR code setting is inputted, DRAM operation is not defined.
 */
#define DDR4_MRS_BG0			_BV(2)
#define DDR4_MRS_BA1			_BV(1)
#define DDR4_MRS_BA0			_BV(0)
#define DDR4_MRS_MR(n)			(n)
#define DDR4_MR0			0
#define DDR4_MR1			1
#define DDR4_MR2			2
#define DDR4_MR3			3
#define DDR4_MR4			4
#define DDR4_MR5			7
#define DDR4_MR6			6
#define DDR4_RCW			7

/* Mode Register: Encoded in A17-A0
 *
 * However we use 16-bit to save MR values, A17 is stored in the highest
 * bit-15, lower 14-bits are used for A0-A13.
 */
#define DDR4_MR182MR16(v)		\
	(((v) & _BV(17) ? _BV(15) : 0) | ((v) & REG_14BIT_MASK))
#define DDR4_MR162MR18(v)		\
	(((v) & _BV(15) ? _BV(17) : 0) | ((v) & REG_14BIT_MASK))

#define DDR4_MR_A17_OFFSET		15
#define DDR4_MR_A17_MASK		REG_1BIT_MASK
#define DDR4_MR_A17(value)		_SET_FV(DDR4_MR_A17, value)
#define DDR4_MR_A13_OFFSET		13
#define DDR4_MR_A13_MASK		REG_1BIT_MASK
#define DDR4_MR_A13(value)		_SET_FV(DDR4_MR_A13, value)
#define DDR4_MR_A12_A11_OFFSET		11
#define DDR4_MR_A12_A11_MASK		REG_2BIT_MASK
#define DDR4_MR_A12_A11(value)		_SET_FV(DDR4_MR_A12_A11, value)
#define DDR4_MR_A12_A10_OFFSET		10
#define DDR4_MR_A12_A10_MASK		REG_3BIT_MASK
#define DDR4_MR_A12_A10(value)		_SET_FV(DDR4_MR_A12_A10, value)
#define DDR4_MR_A12_OFFSET		12
#define DDR4_MR_A12_MASK		REG_1BIT_MASK
#define DDR4_MR_A12(value)		_SET_FV(DDR4_MR_A12, value)
#define DDR4_MR_A11_OFFSET		11
#define DDR4_MR_A11_MASK		REG_1BIT_MASK
#define DDR4_MR_A11(value)		_SET_FV(DDR4_MR_A11, value)
#define DDR4_MR_A11_A9_OFFSET		9
#define DDR4_MR_A11_A9_MASK		REG_3BIT_MASK
#define DDR4_MR_A11_A9(value)		_SET_FV(DDR4_MR_A11_A9, value)
#define DDR4_MR_A10_A9_OFFSET		9
#define DDR4_MR_A10_A9_MASK		REG_2BIT_MASK
#define DDR4_MR_A10_A9(value)		_SET_FV(DDR4_MR_A10_A9, value)
#define DDR4_MR_A10_A8_OFFSET		8
#define DDR4_MR_A10_A8_MASK		REG_3BIT_MASK
#define DDR4_MR_A10_A8(value)		_SET_FV(DDR4_MR_A10_A8, value)
#define DDR4_MR_A10_OFFSET		10
#define DDR4_MR_A10_MASK		REG_1BIT_MASK
#define DDR4_MR_A10(value)		_SET_FV(DDR4_MR_A10, value)
#define DDR4_MR_A9_OFFSET		9
#define DDR4_MR_A9_MASK			REG_1BIT_MASK
#define DDR4_MR_A9(value)		_SET_FV(DDR4_MR_A9, value)
#define DDR4_MR_A8_A6_OFFSET		6
#define DDR4_MR_A8_A6_MASK		REG_3BIT_MASK
#define DDR4_MR_A8_A6(value)		_SET_FV(DDR4_MR_A8_A6, value)
#define DDR4_MR_A8_OFFSET		8
#define DDR4_MR_A8_MASK			REG_1BIT_MASK
#define DDR4_MR_A8(value)		_SET_FV(DDR4_MR_A8, value)
#define DDR4_MR_A7_A6_OFFSET		6
#define DDR4_MR_A7_A6_MASK		REG_2BIT_MASK
#define DDR4_MR_A7_A6(value)		_SET_FV(DDR4_MR_A7_A6, value)
#define DDR4_MR_A7_OFFSET		7
#define DDR4_MR_A7_MASK			REG_1BIT_MASK
#define DDR4_MR_A7(value)		_SET_FV(DDR4_MR_A7, value)
#define DDR4_MR_A6_A4_OFFSET		4
#define DDR4_MR_A6_A4_MASK		REG_3BIT_MASK
#define DDR4_MR_A6_A4(value)		_SET_FV(DDR4_MR_A6_A4, value)
#define DDR4_MR_A6_OFFSET		6
#define DDR4_MR_A6_MASK			REG_1BIT_MASK
#define DDR4_MR_A6(value)		_SET_FV(DDR4_MR_A6, value)
#define DDR4_MR_A5_A3_OFFSET		3
#define DDR4_MR_A5_A3_MASK		REG_3BIT_MASK
#define DDR4_MR_A5_A3(value)		_SET_FV(DDR4_MR_A5_A3, value)
#define DDR4_MR_A5_A0_OFFSET		0
#define DDR4_MR_A5_A0_MASK		REG_6BIT_MASK
#define DDR4_MR_A5_A0(value)		_SET_FV(DDR4_MR_A5_A0, value)
#define DDR4_MR_A5_OFFSET		5
#define DDR4_MR_A5_MASK			REG_1BIT_MASK
#define DDR4_MR_A5(value)		_SET_FV(DDR4_MR_A5, value)
#define DDR4_MR_A4_A3_OFFSET		3
#define DDR4_MR_A4_A3_MASK		REG_2BIT_MASK
#define DDR4_MR_A4_A3(value)		_SET_FV(DDR4_MR_A4_A3, value)
#define DDR4_MR_A4_OFFSET		4
#define DDR4_MR_A4_MASK			REG_1BIT_MASK
#define DDR4_MR_A4(value)		_SET_FV(DDR4_MR_A4, value)
#define DDR4_MR_A3_OFFSET		3
#define DDR4_MR_A3_MASK			REG_1BIT_MASK
#define DDR4_MR_A3(value)		_SET_FV(DDR4_MR_A3, value)
#define DDR4_MR_A2_A1_OFFSET		1
#define DDR4_MR_A2_A1_MASK		REG_2BIT_MASK
#define DDR4_MR_A2_A1(value)		_SET_FV(DDR4_MR_A2_A1, value)
#define DDR4_MR_A2_A0_OFFSET		0
#define DDR4_MR_A2_A0_MASK		REG_3BIT_MASK
#define DDR4_MR_A2_A0(value)		_SET_FV(DDR4_MR_A2_A0, value)
#define DDR4_MR_A2_OFFSET		2
#define DDR4_MR_A2_MASK			REG_1BIT_MASK
#define DDR4_MR_A2(value)		_SET_FV(DDR4_MR_A2, value)
#define DDR4_MR_A1_A0_OFFSET		0
#define DDR4_MR_A1_A0_MASK		REG_2BIT_MASK
#define DDR4_MR_A1_A0(value)		_SET_FV(DDR4_MR_A1_A0, value)
#define DDR4_MR_A1_OFFSET		1
#define DDR4_MR_A1_MASK			REG_1BIT_MASK
#define DDR4_MR_A1(value)		_SET_FV(DDR4_MR_A1, value)
#define DDR4_MR_A0_OFFSET		0
#define DDR4_MR_A0_MASK			REG_1BIT_MASK
#define DDR4_MR_A0(value)		_SET_FV(DDR4_MR_A0, value)

/* MR0 */
#define DDR4_MR0_WT_RTP(value)				\
	(DDR4_MR_A13((DDR4_MR0_WR(value)) >> 3) |	\
	 DDR4_MR_A11_A9(DDR4_MR0_WR(value)))
#define DDR4_MR0_DLL_Reset(value)	DDR4_MR_A8(!!(value))
#define DDR4_MR0_TM(value)		DDR4_MR_A7(!!(value))
#define DDR4_MR0_CASLatency(value)			\
	(DDR4_MR_A12((DDR4_MR0_CL(value)) >> 4) |	\
	 DDR4_MR_A6_A4((DDR4_MR0_CL(value)) >> 1) |	\
	 DDR4_MR_A2(DDR4_MR0_CL(value)))
#define DDR4_MR0_ReadBurstType(value)	DDR4_MR_A3(!!(value))
#define DDR4_MR0_BurstLength(value)	DDR4_MR_A1_A0(value)
#define DDR4_MR_MR0(mode, wr_rtp, cas_lat, bl)				\
	(DDR4_MR0_WT_RTP(wr_rtp) |					\
	 DDR4_MR0_DLL_Reset((mode) & DDR4_MODE_DLLReset) |		\
	 DDR4_MR0_TM((mode) & DDR4_MODE_TM) |				\
	 DDR4_MR0_CASLatency(cas_lat) |					\
	 DDR4_MR0_BurstLength(bl) |					\
	 DDR4_MR0_ReadBurstType((mode) & DDR4_MODE_ReadBurstInterleave))
/* Table 2 - Write Recovery and Read to Precharge (cycles) */
#define DDR4_MR0_WR(n)			DDR4_MR0_RTP((n) >> 1)
#define DDR4_MR0_RTP(n)			\
	(((((n) - 5) & 6) == 6) ? (((((n) - 5) & 1) ? 0 : 1) | 6) : (n) - 5)
#define DDR4_MR0_WR_MAX			26
#define DDR4_MR0_RTP_MAX		13
/* Table 3 - CAS Latency:
 * 9-16:        0-7
 * 18/20/22/24: 8-11
 * 23:          12
 * 17/19/21:    13-15
 * 25-32:       16-23
 */
#define DDR4_MR0_CL(n)				\
	((((n) - 9) & 0x8) ?			\
	 ((((n) - 16) & 0x1) ?			\
	  (((((n) - 17) >> 1) + 1) & 0x3) + 12 :\
	  (((n) - 18) >> 1) + 8) :	\
	 ((n) - 9))

/* MR1 */
#define DDR4_MR1_Qoff(value)			DDR4_MR_A12(!!(value))
#define DDR4_MR1_TDQSEnable(value)		DDR4_MR_A11(!!(value))
#define DDR4_MR1_RTT_NOM(value)				\
	DDR4_MR_A10_A8(DDR4_MR1_RTT_NOM_RZQ(value))
#define DDR4_MR1_WriteLevelingEnable(value)	DDR4_MR_A7(!!(value))
#define DDR4_MR1_AdditiveLatency(value)		DDR4_MR_A4_A3(value)
#define DDR4_MR1_OutputDriverImpedanceControl(value)	\
	DDR4_MR_A2_A1(DDR4_MR1_OutputDriverImpedanceControl_RZQ(value))
#define DDR4_MR1_DLLEnable(value)		DDR4_MR_A0(!!(value))
#define DDR4_MR_MR1(mode, rtt_nom, add_lat, out_imp) 		\
	(DDR4_MR1_Qoff((mode) & DDR4_MODE_Qoff) |		\
	 DDR4_MR1_TDQSEnable((mode) & DDR4_MODE_TDQS) |		\
	 DDR4_MR1_RTT_NOM(rtt_nom) |				\
	 DDR4_MR1_WriteLevelingEnable((mode) &			\
		 DDR4_MODE_WriteLeveling) |			\
	 DDR4_MR1_AdditiveLatency(add_lat) |			\
	 DDR4_MR1_OutputDriverImpedanceControl(out_imp) |	\
	 DDR4_MR1_DLLEnable((mode) & DDR4_MODE_DLL))
/* Table 4 - RTT_NOM */
#define DDR4_MR1_RTT_NOM_RZQ(rzq_div)	bitrev3(rzq_div)
/* Table 5 - Output Driver Impedance Control */
#define DDR4_MR1_OutputDriverImpedanceControl_RZQ(rzq_div)	\
	((7 - (rzq_div)) >> 1)

/* MR2 */
#define DDR4_MR2_WriteCRC(value)	DDR4_MR_A12(!!(value))
#define DDR4_MR2_RTT_WR(value)		\
	DDR4_MR_A11_A9(DDR4_MR2_RTT_WR_RZQ(value))
#define DDR4_MR2_LP_ASR(value)		DDR4_MR_A7_A6(value)
#define DDR4_MR2_CASWriteLatency(value)	DDR4_MR_A5_A3(DDR4_MR2_CWL(value))
#define DDR4_MR_MR2(mode, rtt_wr, lp_asr, cwl)			\
	(DDR4_MR2_WriteCRC((mode) & DDR4_MODE_WriteCRC) |	\
	 ((mode) & DDR4_MODE_RTT_WR_DynamicODT ?		\
	  DDR4_MR2_RTT_WR(rtt_wr) : 0) |			\
	 DDR4_MR2_LP_ASR(lp_asr) | DDR4_MR2_CASWriteLatency(cwl))
/* Table 6 - RTT_WR */
#define DDR4_MR2_RTT_WR_RZQ(rzq_div)		\
	(((rzq_div) > 2) ? ((rzq_div) + 1) : 3 - (rzq_div))
/* Table 7 - CWL (CAS Write Lantency) */
#define DDR4_MR2_CWL(n)				\
	(((n) - 9 < 4) ? ((n) - 9) : (((n) - 14) >> 1) + 4)

/* MR3 */
#define DDR4_MR3_MPRReadFormat(value)		DDR4_MR_A12_A11(value)
#define DDR4_MR3_WriteCMDLatency(value)			\
	DDR4_MR_A10_A9(DDR4_MR3_Write_CL(value))
#define DDR4_MR3_FineGranularityRefreshMode(value)	\
	DDR4_MR_A8_A6(value)
#define DDR4_MR3_Temperature_sensor_readout(value)	\
	DDR4_MR_A5(!!(value))
#define DDR4_MR3_PerDRAMAddressability(value)		\
	DDR4_MR_A4(!!(value))
#define DDR4_MR3_GeardownMode(value)		DDR4_MR_A3(!!(value))
#define DDR4_MR3_MPROperation(value)		DDR4_MR_A2(value)
#define DDR4_MR3_MPRPageSelection(value)	DDR4_MR_A1_A0(value)
#define DDR4_MR_MR3(mode, wr_cmd_lat, fine_rfsh)			\
	(DDR4_MR3_MPRReadFormat(DDR4_MPR_ReadFormat_Serial) |		\
	 DDR4_MR3_MPROperation(DDR4_MPR_Operation_Normal) |		\
	 DDR4_MR3_MPRPageSelection(0) |					\
	 DDR4_MR3_WriteCMDLatency(wr_cmd_lat) |				\
	 DDR4_MR3_FineGranularityRefreshMode(fine_rfsh) |		\
	 DDR4_MR3_Temperature_sensor_readout((mode) &			\
					     DDR4_MODE_TsensReadout) |	\
	 DDR4_MR3_PerDRAMAddressability((mode) & DDR4_MODE_PDA) |	\
	 DDR4_MR3_GeardownMode((mode) & DDR4_MODE_GeardownRate_4))
/* Table 8 - Fine Granularity Refresh Mode */
#define DDR4_MR3_Refresh_Fixed_1x		DDR4_REFRESH_Fixed_1x
#define DDR4_MR3_Refresh_Fixed_2x		DDR4_REFRESH_Fixed_2x
#define DDR4_MR3_Refresh_Fixed_4x		DDR4_REFRESH_Fixed_4x
#define DDR4_MR3_Refresh_OnTheFly_2x		DDR4_REFRESH_OnTheFly_2x
#define DDR4_MR3_Refresh_OnTheFly_4x		DDR4_REFRESH_OnTheFly_4x
/* Table 9 - Write Command Latency when CRC and DM are both enabled */
#define DDR4_MR3_Write_CL(nCK)			((nCK) - 4)

/* MR4 */
#define DDR4_MR4_hPPR(value)			DDR4_MR_A13(!!(value))
#define DDR4_MR4_WritePreamble(value)		DDR4_MR_A12(!!(value))
#define DDR4_MR4_ReadPreamble(value)		DDR4_MR_A11(!!(value))
#define DDR4_MR4_ReadPreambleTrainingMode(value)\
	DDR4_MR_A10(!!(value))
#define DDR4_MR4_SelfRefreshAbort(value)	DDR4_MR_A9(!!(value))
#define DDR4_MR4_CStoCALatency(value)		\
	DDR4_MR_A8_A6(DDR4_MR4_CS_CA_L(value))
#define DDR4_MR4_sPPR(value)			DDR4_MR_A5(!!(value))
#define DDR4_MR4_InternalVrefMonitor(value)	DDR4_MR_A4(!!(value))
#define DDR4_MR4_TemperatureControlledRefreshMode(value)	\
	DDR4_MR_A3(!!(value))
#define DDR4_MR4_TemperatureControlledRefreshRange(value)	\
	DDR4_MR_A2(!!(value))
#define DDR4_MR4_MaximumPowerDownMode(value)	DDR4_MR_A1(!!(value))
#define DDR4_MR_MR4(mode, cs_ca_lat)					\
	(DDR4_MR4_hPPR((mode) & DDR4_MODE_hPPR) |			\
	 DDR4_MR4_sPPR((mode) & DDR4_MODE_sPPR) |			\
	 DDR4_MR4_WritePreamble((mode) & DDR4_MODE_WritePreamble_2T) |	\
	 DDR4_MR4_ReadPreamble((mode) & DDR4_MODE_ReadPreamble_2T) |	\
	 DDR4_MR4_ReadPreambleTrainingMode((mode) &			\
		DDR4_MODE_ReadPreambleTraining) |			\
	 DDR4_MR4_SelfRefreshAbort((mode) &				\
		 DDR4_MODE_SelfRefreshAbort) |				\
	 ((mode) & DDR4_MODE_CS_CA_Latency ?				\
	  DDR4_MR4_CStoCALatency(cs_ca_lat) : 0) |			\
	 DDR4_MR4_InternalVrefMonitor((mode) &				\
		 DDR4_MODE_InternalVrefMonitor) |			\
	 DDR4_MR4_TemperatureControlledRefreshMode((mode) &		\
		 DDR4_MODE_TCR) |					\
	 DDR4_MR4_TemperatureControlledRefreshRange((mode) &		\
		 DDR4_MODE_TCR_RangeExtended) |				\
	 DDR4_MR4_MaximumPowerDownMode((mode) & DDR4_MODE_MPDM))
/* Table 11 - CS to CMD/ADDR Latency Mode Setting */
#define DDR4_MR4_CS_CA_L(lat)		\
	(((lat) < 8) ? (lat) - 2 : (((lat) - 6) >> 1) + 4)

/* MR5 */
#define DDR4_MR5_ReadDBI(value)			DDR4_MR_A12(!!(value))
#define DDR4_MR5_WriteDBI(value)		DDR4_MR_A11(!!(value))
#define DDR4_MR5_DataMask(value)		DDR4_MR_A10(!!(value))
#define DDR4_MR5_CAParityPersistentError(value)	DDR4_MR_A9(!!(value))
#define DDR4_MR5_RTT_PARK(value)		\
	DDR4_MR_A8_A6(DDR4_MR5_RTT_PARK_RZQ(value))
#define DDR4_MR5_ODTInputBuffer(value)		DDR4_MR_A5(!!(!(value)))
#define DDR4_MR5_CAParityErrorStatus(value)	DDR4_MR_A4(!!(value))
#define DDR4_MR5_CRCErrorClear(value)		DDR4_MR_A3(!!(value))
#define DDR4_MR5_CAParityLatencyMode(value)	\
	DDR4_MR_A2_A0(DDR4_MR5_CAPL(value))
#define DDR4_MR_MR5(mode, rtt_park, capar_lat)				\
	(DDR4_MR5_ReadDBI((mode) & DDR4_MODE_ReadDBI) |			\
	 DDR4_MR5_WriteDBI((mode) & DDR4_MODE_WriteDBI) |		\
	 DDR4_MR5_DataMask((mode) & DDR4_MODE_DataMask) |		\
	 DDR4_MR5_ODTInputBuffer((mode) & DDR4_MODE_ODTInputBuffer) |	\
	 DDR4_MR5_CAParityPersistentError((mode) & DDR4_MODE_CAPAR) |	\
	 ((mode) & DDR4_MODE_CAPARLatency ?				\
	  DDR4_MR5_CAParityLatencyMode(capar_lat) : 0) |		\
	 DDR4_MR5_RTT_PARK(rtt_park) |					\
	 DDR4_MR5_CAParityErrorStatus(DDR4_CAPAR_Clear) |		\
	 DDR4_MR5_CRCErrorClear(DDR4_CRC_Clear))
/* Table 12 - RTT_PARK */
#define DDR4_MR5_RTT_PARK_RZQ(rzq_div)	bitrev3(rzq_div)
/* Table 13 - C/A Parity Latency Mode */
#define DDR4_MR5_CAPL(lat)				\
	(((lat) < 8) ? (lat) - 3 : (((lat) - 6) >> 1) + 3)

/* MR6 */
#define DDR4_MR6_tCCD_L(value)			\
	DDR4_MR_A12_A10(DDR4_MR6_tCCD_L_min(value))
#define DDR4_MR6_VrefDQTrainingEnable(value)	DDR4_MR_A7(!!(value))
#define DDR4_MR6_VrefDQTrainingRange(value)	DDR4_MR_A6(!!(value))
#define DDR4_MR6_VrefDQTrainingValue(value)	DDR4_MR_A5_A0(value)
#define DDR4_MR6_VrefDQTrainingRangeValue(r, v)	\
	(DDR4_MR6_VrefDQTrainingRange(r) |	\
	 DDR4_MR6_VrefDQTrainingValue(DDR4_MR6_VrefDQTraining(r, v)))
#define DDR4_MR_MR6(mode, t_ccd_l, vref_dq_r, vref_dq_v)		\
	(DDR4_MR6_VrefDQTrainingEnable((mode) &				\
				       DDR4_MODE_VrefDQTraining) |	\
	 DDR4_MR6_tCCD_L(t_ccd_l) |					\
	 DDR4_MR6_VrefDQTrainingRangeValue(vref_dq_r, vref_dq_v))
/* Table 14 - tCCD_L and tDLLK */
#define DDR4_MR6_tCCD_L_min(nCK)	((nCK) - 4)
/* Table 16 - VrefDQ Training: Values */
#define DDR4_MR6_VrefDQTraining(r, value)	\
	((((r) == DDR4_TRAINING_VrefDQ_Range1 ?	\
	  ((value) - 6000) : ((value) - 4500)) + 64) / 65)

/* ZQ calibration timings (nCK) */
#define DDR4_tZQinit	1024
#define DDR4_tZQoper	512
#define DDR4_tZQCS	128

#define DDR_SPD2tCK(spd)		__DDR_SPD2tCK(spd, 2)

struct ddr4_dev {
	uint32_t mode;
	uint16_t tCK;

	/* FGR related */
	uint8_t FGR_mode; /* Fine Granularity Refresh */
	uint16_t tRFCmin;
	uint32_t tREFI;

	uint8_t LP_ASR_mode;

	/* Core parameters */
	uint8_t tCCD_Lmin;

	uint16_t tDLLK;

	uint8_t CAPAR_latency_mode;
	uint8_t CS_CA_latency_mode;
	uint8_t WR_CMD_latency;
	uint8_t additive_latency;
	uint8_t CAS_latency;

	uint8_t RTT_WR;
	uint8_t RTT_PARK;
	uint8_t RTT_NOM;
	uint8_t output_driver_impedance_control;

	uint8_t CWL;
	uint8_t WR_RTP;
	uint8_t burst_length;
};

void ddr4_config_speed(uint8_t n, uint8_t spd);
void ddr4_config_refresh(uint8_t n, uint8_t cap, uint8_t mode);
void ddr4_powerup_init(uint8_t n);
void ddr4_reset_init(uint8_t n);
#else
#define ddr4_config_speed(n, spd)		do { } while (0)
#define ddr4_config_refresh(n, cap, mode)	do { } while (0)
#define ddr4_powerup_init(n)			do { } while (0)
#define ddr4_reset_init(n)			do { } while (0)
#endif /* CONFIG_DDR4 */

#endif /* __DDR4_H_INCLUDE__ */
