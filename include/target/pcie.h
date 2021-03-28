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
 * @(#)pci.h: PCI express (PCIe) definitions
 * $Id: pcie.h,v 1.1 2021-3-28 15:29:00 zhenglv Exp $
 */

#ifndef __PCIE_H_INCLUDE__
#define __PCIE_H_INCLUDE__

/* 7.5.3 PCI Express Capability Structure */
#define PCIE_CapabilitiesRegister		0x000 /*  8-bits */
#define PCIE_DeviceCapabilities			0x004 /*  8-bits */
#define PCIE_DeviceControl			0x008 /* 16-bits */
#define PCIE_DeviceStatus			0x00A /* 16-bits */
#define PCIE_LinkCapabilities			0x00C /* 32-bits */
#define PCIE_LinkControl			0x010 /* 16-bits */
#define PCIE_LinkStatus				0x012 /* 16-bits */
#define PCIE_SlotCapabilities			0x014 /* 32-bits */
#define PCIE_SlotControl			0x018 /* 16-bits */
#define PCIE_SlotStatus				0x01A /* 16-bits */
#define PCIE_RootControl			0x01C /* 16-bits */
#define PCIE_RootCapabilities			0x01E /* 16-bits */
#define PCIE_RootStatus				0x020 /* 32-bits */
#define PCIE_DeviceCapabilities2		0x024 /* 32-bits */
#define PCIE_DeviceControl2			0x028 /* 16-bits */
#define PCIE_DeviceStatus2			0x02A /* 16-bits */
#define PCIE_LinkCapabilities2			0x02C /* 32-bits */
#define PCIE_LinkControl2			0x030 /* 16-bits */
#define PCIE_LinkStatus2			0x032 /* 16-bits */
#define PCIE_SlotCapabilities2			0x034 /* 32-bits */
#define PCIE_SlotControl2			0x038 /* 16-bits */
#define PCIE_SlotStatus2			0x03A /* 16-bits */

/* 7.5.3.2 PCI Express Capabilities Register (Offset 02h) */
#define PCIE_CapabilityVersion_OFFSET		2
#define PCIE_CapabilityVersion_MASK		REG_4BIT_MASK
#define PCIE_CapabilityVersion(value)		\
	_GET_FV(PCIE_CapabilityVersion, value)
#define PCIE_DevicePortType_OFFSET		4
#define PCIE_DevicePortType_MASK		REG_4BIT_MASK
#define PCIE_DevicePortType(value)		\
	_GET_FV(PCIE_DevicePortType, value)
#define PCIE_PCIExpressEndpoint			0
#define PCIE_LegacyPCIExpressEndpoint		1
#define PCIE_RCiEP				9
#define PCIE_RootComplexEventCollector		10
#define PCIE_RootPortOfPCIExpressRootComplex	4
#define PCIE_UpstreamPortOfPCIExpressSwitch	5
#define PCIE_DownstreamPortOfPCIExpressSwitch	6
#define PCIE_PCIExpressToPCI_PCIXBridge		7
#define PCIE_PCI_PCIXToPCIExpressBridge		8
#define PCIE_SlotImplemented			_BV(8)
#define PCIE_InterruptMessageNumber_OFFSET	9
#define PCIE_InterruptMessageNumber_MASK	REG_5BIT_MASK
#define PCIE_InterruptMessageNumber(value)	\
	_GET_FV(PCIE_InterruptMessageNumber, value)

/* 7.5.3.3 Device Capabilities Register (Offset 04h) */
#define PCIE_MaxPayloadSizeSupported_OFFSET		0
#define PCIE_MaxPayloadSizeSupported_MASK		REG_3BIT_MASK
#define PCIE_MaxPayloadSizeSupported(value)		\
	_GET_FV(PCIE_MaxPayloadSizeSupported, value)
#define PCIE_128Bytes					0
#define PCIE_256Bytes					1
#define PCIE_512Bytes					2
#define PCIE_1024Bytes					3
#define PCIE_2048Bytes					4
#define PCIE_4096Bytes					5
#define PCIE_PhantomFunctionsSupported_OFFSET		3
#define PCIE_PhantomFunctionsSupported_MASK		REG_2BIT_MASK
#define PCIE_PhantomFunctionsSupported(value)		\
	_GET_FV(PCIE_PhantomFunctionsSupported, value)
#define PCIE_ExtendedTagFieldSupported			_BV(5)
#define PCIE_EndpointL0sAcceptableLatency_OFFSET	6
#define PCIE_EndpointL0sAcceptableLatency_MASK		REG_3BIT_MASK
#define PCIE_EndpointL0sAcceptableLatency(value)	\
	_GET_FV(PCIE_EndpointL0sAcceptableLatency, value)
#define PCIE_EndpointL1sAcceptableLatency_OFFSET	9
#define PCIE_EndpointL1sAcceptableLatency_MASK		REG_3BIT_MASK
#define PCIE_EndpointL1sAcceptableLatency(value)	\
	_GET_FV(PCIE_EndpointL1sAcceptableLatency, value)
#define PCIE_RoleBasedErrorReporting			_BV(15)
#define PCIE_ERRCORSubclassCapable			_BV(16)
#define PCIE_CapturedSlotPowerLimitValue_OFFSET		18
#define PCIE_CapturedSlotPowerLimitValue_MASK		REG_8BIT_MASK
#define PCIE_CapturedSlotPowerLimitValue(value)		\
	_GET_FV(PCIE_CapturedSlotPowerLimitValue, value)
#define PCIE_CapturedSlotPowerLimitScale_OFFSET		26
#define PCIE_CapturedSlotPowerLimitScale_MASK		REG_2BIT_MASK
#define PCIE_CapturedSlotPowerLimitScale(value)		\
	_GET_FV(PCIE_CapturedSlotPowerLimitScale, value)
#define PCIE_FunctionLevelResetCapability		_BV(28)

/* 7.5.3.4 Device Control Register (Offset 08h) */
#define PCIE_CorrectableErrorReportingEnable		_BV(0)
#define PCIE_NonFatalErrorReportingEnable		_BV(1)
#define PCIE_FatalErrorReportingEnable			_BV(2)
#define PCIE_UnsupportedRequestReportingEnable		_BV(3)
#define PCIE_EnableRelaxedOrdering			_BV(4)
#define PCIE_MaxPayloadSize_OFFSET			5
#define PCIE_MaxPayloadSize_MASK			REG_3BIT_MASK
#define PCIE_MaxPayloadSize(value)			\
	_SET_FV(PCIE_MaxPayloadSize, value)
#define PCIE_ExtendedTagFieldEnable			_BV(8)
#define PCIE_PhantomFunctionsEnable			_BV(9)
#define PCIE_AuxPowerPMEnable				_BV(10)
#define PCIE_EnableNoSnoop				_BV(11)
#define PCIE_MaxReadRequestSize_OFFSET			12
#define PCIE_MaxReadRequestSize_MASK			REG_3BIT_MASK
#define PCIE_MaxReadRequestSize(value)			\
	_SET_FV(PCIE_MaxReadRequestSize, value)
#define PCIE_BridgeConfigurationRetryEnable		_BV(15)
#define PCIE_InitiateFunctionLevelReset			_BV(15)

/* 7.5.3.5 Device Status Register (Offset 0Ah) */
#define PCIE_CorrectableErrorDetected			_BV(0)
#define PCIE_NonFatalErrorDetected			_BV(1)
#define PCIE_FatalErrorDetected				_BV(2)
#define PCIE_UnsupportedRequestDetected			_BV(3)
#define PCIE_AUXPowerDetected				_BV(4)
#define PCIE_TransactionsPending			_BV(5)
#define PCIE_EmergencyPowerReductionDetected		_BV(6)

/* 7.5.3.6 Link Capabilities Register (Offset 0Ch) */
#define PCIE_MaxLinkSpeed_OFFSET			0
#define PCIE_MaxLinkSpeed_MASK				REG_4BIT_MASK
#define PCIE_MaxLinkSpeed(value)			\
	_GET_FV(PCIE_MaxLinkSpeed, value)
#define PCIE_MaximumLinkWidth_OFFSET			4
#define PCIE_MaximumLinkWidth_MASK			REG_6BIT_MASK
#define PCIE_MaximumLinkWidth(value)			\
	_GET_FV(PCIE_MaximumLinkWidth, value)
#define PCIE_ASPMSupport_OFFSET				10
#define PCIE_ASPMSupport_MASK				REG_2BIT_MASK
#define PCIE_ASPMSupport(value)				\
	_GET_FV(PCIE_ASPMSupport, value)
#define PCIE_L0sExitLatency_OFFSET			12
#define PCIE_L0sExitLatency_MASK			REG_3BIT_MASK
#define PCIE_L0sExitLatency(value)			\
	_GET_FV(PCIE_L0sExitLatency, value)
#define PCIE_L1ExitLatency_OFFSET			15
#define PCIE_L1ExitLatency_MASK				REG_3BIT_MASK
#define PCIE_L1ExitLatency(value)			\
	_GET_FV(PCIE_L1ExitLatency, value)
#define PCIE_ClockPowerManagement_OFFSET		_BV(18)
#define PCIE_SurpriseDownErrorReportingCapable		_BV(19)
#define PCIE_DataLinkLayerLinkActiveReportingCapable	_BV(20)
#define PCIE_LinkBandwidthNotificationCapability	_BV(21)
#define PCIE_ASPMOptionalityCompliance			_BV(22)
#define PCIE_PortNumber_OFFSET				24
#define PCIE_PortNumber_MASK				REG_8BIT_MASK
#define PCIE_PortNumber(value)				\
	_GET_FV(PCIE_PortNumber, value)

/* CapabilityID */
#define PCI_CAP_PCIExpress			0x10

#endif /* __PCIE_H_INCLUDE__ */
