/*
 * ZETALOG's Personal COPYRIGHT
 *
 * Copyright (c) 2021
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
 * @(#)rio.h: RapidIO interconnect controller interface
 * $Id: rio.h,v 1.0 2021-11-02 13:30:00 zhenglv Exp $
 */

#ifndef __RIO_H_INCLUDE__
#define __RIO_H_INCLUDE__

#include <target/generic.h>
#include <target/jiffies.h>

/* Part 1: Input/Output Logical Specification */

/* Chapter 5 Input/Output Registers */

/* 5.4 Capbility Registers (CARs) */
#define RIO_CAR_DeviceIdentity			0x00
#define RIO_CAR_DeviceInformation		0x04
#define RIO_CAR_AssemblyIdentity		0x08
#define RIO_CAR_AssemblyInformation		0x0C
#define RIO_CAR_ProcessingElementFeatures	0x10
#define RIO_CAR_SwitchPortInformation		0x14
#define RIO_CAR_SourceOperations		0x18
#define RIO_CAR_DestinationOperations		0x1C

/* 5.5 Command and Status Registers (CSRs) */
#define RIO_CSR_ProcessingElementLogicalLayerControl	0x4C
#define RIO_CSR_LocalConfigurationSpaceBaseAddress0	0x58
#define RIO_CSR_LocalConfigurationSpaceBaseAddress1	0x5C

/* Short aliases */
#define RIO_DEV_ID_CAR		RIO_CAR_DeviceIdentity
#define RIO_DEV_INFO_CAR	RIO_CAR_DeviceInformation
#define RIO_ASM_ID_CAR		RIO_CAR_AssemblyIdentity
#define RIO_ASM_INFO_CAR	RIO_CAR_AssemblyInformation
#define RIO_PE_FEAT_CAR		RIO_CAR_ProcessingElementFeatures
#define RIO_SP_INFO_CAR		RIO_CAR_SwitchPortInformation
#define RIO_SRC_OPS_CAR		RIO_CAR_SourceOperations
#define RIO_DST_OPS_CAR		RIO_CAR_DestinationOperations
#define RIO_PE_LLC_CSR		RIO_CSR_ProcessingElementLogicalLayerControl
#define RIO_LCSBA_0_CSR		RIO_CSR_LocalConfigurationSpaceBaseAddress0
#define RIO_LCSBA_1_CSR		RIO_CSR_LocalConfigurationSpaceBaseAddress1
#define RIO_LCSBA_HI_CSR	RIO_LCSBA_0_CSR
#define RIO_LCSBA_LO_CSR	RIO_LCSBA_1_CSR

/* 5.4.1 Device Identity CAR */
#define RIO_DeviceIdentity_OFFSET	0
#define RIO_DeviceIdentity_MASK		REG_16BIT_MASK
#define RIO_DeviceIdentity(value)	_GET_FV(RIO_DeviceIdentity, value)
#define RIO_DeviceVendorIdentity_OFFSET	16
#define RIO_DeviceVendorIdentity_MASK	REG_16BIT_MASK
#define RIO_DeviceVendorIdentity(value)	_GET_FV(RIO_DeviceVendorIdentity, value)

/* 5.4.2 Device Information CAR */
#define RIO_DeviceRev_OFFSET		0
#define RIO_DeviceRev_MASK		REG_32BIT_MASK
#define RIO_DeviceRev(value)		_GET_FV(RIO_DeviceRev, value)

/* 5.4.3 Assembly Identity CAR */
#define RIO_AssyIdentity_OFFSET		0
#define RIO_AssyIdentity_MASK		REG_16BIT_MASK
#define RIO_AssyIdentity(value)		_GET_FV(RIO_AssyIdentity, value)
#define RIO_AssyVendorIdentity_OFFSET	16
#define RIO_AssyVendorIdentity_MASK	REG_16BIT_MASK
#define RIO_AssyVendorIdentity(value)	_GET_FV(RIO_AssyVendorIdentity, value)

/* 5.4.4 Assmebly Information CAR */
#define RIO_AssyRev_OFFSET		0
#define RIO_AssyRev_MASK		REG_16BIT_MASK
#define RIO_AssyRev(value)		_GET_FV(RIO_AssyRev, value)
#define RIO_ExtendedFeaturePtr_OFFSET	16
#define RIO_ExtendedFeaturePtr_MASK	REG_16BIT_MASK
#define RIO_ExtendedFeaturePtr(value)	_GET_FV(RIO_ExtendedFeaturePtr, value)

/* 5.4.5 Processing Element Features CAR */
#define RIO_Bridge			_BV(0)
#define RIO_Memory			_BV(1)
#define RIO_Processor			_BV(2)
#define RIO_Switch			_BV(3)
#define RIO_ExtendedFeatures		_BV(28)
#define RIO_ExtendedAddressingSupport_OFFSET	29
#define RIO_ExtendedAddressingSupport_MASK	REG_3BIT_MASK
#define RIO_ExtendedAddressingSupport(value)	_GET_FV(RIO_ExtendedAddressingSupport, value)

/* 5.4.6 Switch Port Information CAR */
#define RIO_PortTotal_OFFSET		16
#define RIO_PortTotal_MASK		REG_8BIT_MASK
#define RIO_PortTotal(value)		_GET_FV(RIO_PortTotal, value)
#define RIO_PortNumber_OFFSET		24
#define RIO_PortNumber_MASK		REG_8BIT_MASK
#define RIO_PortNumber(value)		_GET_FV(RIO_PortNumber, value)

/* 5.4.7 Source Operations CAR
 * 5.4.8 Destination Operation CAR
 */
#define RIO_Read			_BV(16)
#define RIO_Write			_BV(17)
#define RIO_Streaming_write		_BV(18)
#define RIO_Write_with_response		_BV(19)
#define RIO_Atomic_compare_and_swap	_BV(22)
#define RIO_Atomic_test_and_swap	_BV(23)
#define RIO_Atomic_increment		_BV(24)
#define RIO_Atomic_decrement		_BV(25)
#define RIO_Atomic_set			_BV(26)
#define RIO_Atomic_clear		_BV(27)
#define RIO_Atomic_swap			_BV(28)
#define RIO_Port_write			_BV(29)

/* 5.5.1 Processing Element Logical Layer Control CSR */
#define RIO_ExtendedAddressingControl_OFFSET	29
#define RIO_ExtendedAddressingControl_MASK	REG_3BIT_MASK
#define RIO_ExtendedAddressingControl(value)	_SET_FV(RIO_ExtendedAddressingControl, value)

/* 5.4.5 Processing Element Features CAR
 * 5.5.1 Processing Element Logical Layer Control CSR
 */
#define RIO_ExtendedAddressing_66	0x04
#define RIO_ExtendedAddressing_50	0x02
#define RIO_ExtendedAddressing_34	0x01

/* Short aliases */
#define RIO_EFP(value)			RIO_ExtendedFeaturePtr(value)
#define RIO_EAS(value)			RIO_ExtendedAddressingSupport(value)
#define RIO_EAC(value)			RIO_ExtendedAddressingControl(value)
#define RIO_EA_66			RIO_ExtendedAddressing_66
#define RIO_EA_50			RIO_ExtendedAddressing_50
#define RIO_EA_34			RIO_ExtendedAddressing_34

/* 5.3 Extended Features Data Structure */
#define RIO_NextExtendedFeaturePtr_OFFSET	0
#define RIO_NextExtendedFeaturePtr_MASK		REG_16BIT_MASK
#define RIO_NextExtendedFeaturePtr(value)	_GET_FV(RIO_NextExtendedFeaturePtr, value)
#define RIO_ExtendedFeatureID_OFFSET		16
#define RIO_ExtendedFeatureID_MASK		REG_16BIT_MASK
#define RIO_ExtendedFeatureID(value)		_GET_FV(RIO_ExtendedFeatureID, value)

#include <target/rio_srio.h>
#include <driver/rio.h>

#endif /* __RIO_H_INCLUDE__ */
