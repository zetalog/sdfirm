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
 * @(#)ddr1.h: double data rate 1 framework interface
 * $Id: ddr1.h,v 1.0 2022-05-30 08:45:00 zhenglv Exp $
 */

#ifndef __DDR1_H_INCLUDE__
#define __DDR1_H_INCLUDE__

#ifdef CONFIG_DDR1
#define DDR_SPD_SIZE		DDR1_SPD_SIZE

/* JESD79 - Double Data Rate (DDR) SDRAM Specification */
/* INITIALIZATION
 * VDD -> VDDQQ -> VREF/VTT:
 * VTT must be applied after VDDQ to avoid device latch-up, which may cause
 * permanent damage to the device.
 * VREF can be applied any time after VDDQ, but is expected to be nominally
 * conincident with VTT.
 * CKE is an SSTL_2 input, but will detect an LVCMOS LOW level after VDD is
 * applied.
 * After all power supply and reference voltage are stable, and the clock
 * is stable, the 200us delay prior to applying an executable command is
 * required.
 */
#define DDR_BA_OFFSET			12
#define DDR_BA_MASK			REG_2BIT_MASK
#define DDR_BA(value)			_SET_FV(DDR_BA, value)
#define DDR_ba(value)			_GET_FV(DDR_BA, value)
#define DDR_BA_MR			0
#define DDR_BA_EMR			1
/* Mode Register */
#define DDR_MR_BurstLength_OFFSET	0
#define DDR_MR_BurstLength_MASK		REG_3BIT_MASK
#define DDR_MR_BurstLength(value)	_SET_FV(DDR_MR_BurstLength, value)
#define DDR_MR_burstlength(value)	_GET_FV(DDR_MR_BurstLength, value)
#define DDR_MR_BurstType_OFFSET		3
#define DDR_MR_BurstType_MASK		REG_1BIT_MASK
#define DDR_MR_BurstType(value)		_SET_FV(DDR_MR_BurstType, value)
#define DDR_MR_bursttype(value)		_GET_FV(DDR_MR_BurstType, value)
#define DDR_MR_BurstType_Sequential	0
#define DDR_MR_BurstType_interleaved	1
#define DDR_MR_CASLatency_OFFSET	4
#define DDR_MR_CASLatency_MASK		REG_3BIT_MASK
#define DDR_MR_CASLatency(value)	_SET_FV(DDR_MR_CASLatency, value)
#define DDR_MR_caslatency(value)	_GET_FV(DDR_MR_CASLatency, value)
#define DDR_MR_CASLatency_2		2
#define DDR_MR_CASLatency_3		3
#define DDR_MR_CASLatency_1_5		5
#define DDR_MR_CASLatency_2_5		6
#define DDR_MR_OperatingMode_OFFSET	7
#define DDR_MR_OperatingMode_MASK	REG_5BIT_MASK
#define DDR_MR_OperatingMode(value)	_SET_FV(DDR_MR_OperatingMode, value)
#define DDR_MR_operatingmode(value)	_GET_FV(DDR_MR_OperatingMode, value)
#define DDR_MR_NormalOperation		0
#define DDR_MR_NormalOperationResetDLL	2
#define DDR_MR_VendorSpecificTestMode	1

/* EXTENDED MODE REGISTER */
#define DDR_EMR_DLLDisable		_BV(0)
#define DDR_EMR_DriveStrength_OFFSET	1
#define DDR_EMR_DriveStrength_MASK	REG_1BIT_MASK
#define DDR_EMR_DriveStrength(value)	_SET_FV(DDR_EMR_DriveStrength, value)
#define DDR_EMR_drivestrength(value)	_GET_FV(DDR_EMR_DriveStrength, value)
#define DDR_EMR_QFCEnable		_BV(2)
#define DDR_EMR_OperatingMode_OFFSET	3
#define DDR_EMR_OperatingMode_MASK	REG_9BIT_MASK
#define DDR_EMR_OperatingMode(value)	_SET_FV(DDR_EMR_OperatingMode, value)
#define DDR_EMR_operatingmode(value)	_GET_FV(DDR_EMR_OperatingMode, value)
#define DDR_EMR_NormalOperation		0

/* COMMANDS */
#ifndef DDR_S
#define DDR_S				_BV(0)
#endif
#ifndef DDR_RAS
#define DDR_RAS				_BV(1)
#endif
#ifndef DDR_CAS
#define DDR_CAS				_BV(2)
#endif
#ifndef DDR_WE
#define DDR_WE				_BV(3)
#endif
#define DDR_CMD_DESELECT		DDR_S
#define DDR_CMD_NOP			(DDR_RAS | DDR_CAS | DDR_WE)
#define DDR_CMD_ACTIVE			(DDR_CAS | DDR_WE)
#define DDR_CMD_READ			(DDR_RAS | DDR_WE)
#define DDR_CMD_WRITE			(DDR_RAS)
#define DDR_CMD_BURST_TERMINATE		(DDR_RAS | DDR_CAS)
#define DDR_CMD_PRECHARGE		DDR_CAS
/* The 64 Mb DDR SDRAM requires AUTO REFRESH cycles at an average periodici
 * interval of 15.6 ms (maximum).
 */
#define DDR_CMD_AUTO_REFRESH		DDR_WE
#define DDR_CMD_SELF_REFRESH		DDR_WE
#define DDR_CMD_MODE_REGISTER_SET	0

#define DDR_SPD2tCK(spd)		__DDR_SPD2tCK(spd, 1)
#endif /* CONFIG_DDR1 */

#endif /* __DDR1_H_INCLUDE__ */
