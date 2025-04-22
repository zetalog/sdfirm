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
 * @(#)pci.h: peripheral component interconnect (PCI) definitions
 * $Id: pci.h,v 1.1 2021-3-28 12:10:00 zhenglv Exp $
 */

 #ifndef __PCI_H_INCLUDE__
 #define __PCI_H_INCLUDE__
 
 #include <target/generic.h>
 #include <driver/pci.h>
 
 /* 7.5.1.1 Type 0/1 Common Configuration Space */
 #define PCI_VendorID				0x000 /* 16-bits */
 #define PCI_DeviceID				0x002 /* 16-bits */
 #define PCI_Command				0x004 /* 16-bits */
 #define PCI_Status				0x006 /* 16-bits */
 #define PCI_RevisionID				0x008 /*  8-bits */
 #define PCI_ClassCode				0x009 /* 24-bits */
 #define PCI_CacheLineSize			0x00C /*  8-bits */
 #define PCI_LatencyTimer			0x00D /*  8-bits */
 #define PCI_HeaderType				0x00E /*  8-bits */
 #ifndef PCI_BIST
 #define PCI_BIST				0x00F /*  8-bits */
 #endif
 #define PCI_CapabilitiesPointer			0x034 /*  8-bits */
 #define PCI_InterruptLine			0x03C /*  8-bits */
 #define PCI_InterruptPin			0x03D /*  8-bits */
 
 /* 7.5.1.2 Type 0 Configuration Space Header
  * 7.5.1.3 Type 1 Configuration Space Header
  */
 #define PCI_BaseAddressRegister0		0x010 /* 32-bits */
 #define PCI_BaseAddressRegister1		0x014 /* 32-bits */
 
 /* 7.5.1.2 Type 0 Configuration Space Header */
 #define PCI_BaseAddressRegister2		0x018 /* 32-bits */
 #define PCI_BaseAddressRegister3		0x01C /* 32-bits */
 #define PCI_BaseAddressRegister4		0x020 /* 32-bits */
 #define PCI_BaseAddressRegister5		0x024 /* 32-bits */
 #define PCI_CardbusCISPointer			0x028 /* 32-bits */
 #define PCI_SubsystemVendorID			0x02C /* 16-bits */
 #define PCI_SubsystemID				0x02E /* 16-bits */
 #define PCI_ExpansionROMBaseAddress0		0x030 /* 32-bits */
 #define PCI_MinGnt				0x03E /*  8-bits */
 #define PCI_MaxLat				0x03F /*  8-bits */
 
 /* 7.5.1.3 Type 1 Configuration Space Header */
 #define PCI_PrimaryBusNumber			0x018 /*  8-bits */
 #define PCI_SecondaryBusNumber			0x019 /*  8-bits */
 #define PCI_SubordinateBusNumber		0x01A /*  8-bits */
 #define PCI_SecondaryLatencyTimer		0x01B /*  8-bits */
 #define PCI_IOBase				0x01C /*  8-bits */
 #define PCI_IOLimit				0x01D /*  8-bits */
 #define PCI_SecondaryStatus			0x01E /* 16-bits */
 #define PCI_MemoryBase				0x020 /* 16-bits */
 #define PCI_MemoryLimit				0x022 /* 16-bits */
 #define PCI_PrefetchableMemoryBase		0x024 /* 16-bits */
 #define PCI_PrefetchableMemoryLimit		0x026 /* 16-bits */
 #define PCI_PrefetchableMemoryBaseUpper32Bits	0x028 /* 32-bits */
 #define PCI_PrefetchableMemoryLimitUpper32Bits	0x02C /* 32-bits */
 #define PCI_IOBaseUpper16Bits			0x030 /* 16-bits */
 #define PCI_IOLimitUpper16Bits			0x032 /* 16-bits */
 #define PCI_ExpansionROMBaseAddress1		0x038 /* 32-bits */
 #define PCI_BridgeControl			0x03E /* 16-bits */
 
 /* 7.5.2 PCI Power Management Capability Structure */
 #define PCI_CapabilityID			0x000 /*  8-bits */
 #define PCI_NextCapabilityPointer		0x001 /*  8-bits */
 #define PCI_PowerManagementCapabilities		0x002 /* 16-bits */
 /* Power Management Control/Status */
 #define PCI_PowerManagementControlStatus	0x004 /* 16-bits */
 #define PCI_PowerManagementData			0x007 /*  8-bits */
 
 /* 7.5.1.1.3 Command Register (Offset 04h) */
 #define PCI_IOSpaceEnable				_BV(0)
 #define PCI_MemorySpaceEnable				_BV(1)
 #define PCI_BusMasterEnable				_BV(2)
 #define PCI_SpecialCycleEnable				_BV(3)
 #define PCI_MemoryWriteAndInvalidate			_BV(4)
 #define PCI_VGAPaletteSnoop				_BV(5)
 #define PCI_ParityErrorResponse				_BV(6)
 #define PCI_IDSELSteppingWaitCycleControl		_BV(7)
 #define PCI_CommandSERREnable				_BV(8)
 #define PCI_CommandFastBackToBackTransactionsEnable	_BV(9)
 #define PCI_InterruptDisable				_BV(10)
 
 /* 7.5.1.1.4 Status Register (Offset 06h)
  * 7.5.1.3.7 Secondary Status Register (Offset 1Eh)
  */
 #define PCI_ImmediateReadiness			_BV(0)
 #define PCI_InterruptStatus			_BV(3)
 #define PCI_CapabilitiesList			_BV(4)
 #define PCI_66MHzCapable			_BV(5)
 #define PCI_FastBackToBackTransactionsCapable	_BV(7)
 #define PCI_MasterDataParityError		_BV(8)
 #define PCI_DEVSELTiming_OFFSET			9
 #define PCI_DEVSELTiming_MASK			REG_2BIT_MASK
 #define PCI_DEVSELTiming(value)			\
	 _GET_FV(PCI_DEVSELTiming, value)
 #define PCI_SignaledTargetAbort			_BV(11)
 #define PCI_ReceivedTargetAbort			_BV(12)
 #define PCI_ReceivedMasterAbort			_BV(13)
 #define PCI_SignaledSystemError			_BV(14)
 #define PCI_DetectedParityError			_BV(15)
 
 /* 7.5.1.1.6 Class Code Register (Offset 09h) */
 #define PCI_ProgrammingInterface_OFFSET		0
 #define PCI_ProgrammingInterface_MASK		REG_8BIT_MASK
 #define PCI_ProgrammingInterface(value)		\
	 _GET_FV(PCI_ProgrammingInterface, value)
 #define PCI_SubClassCode_OFFSET			8
 #define PCI_SubClassCode_MASK			REG_8BIT_MASK
 #define PCI_SubClassCode(value)			\
	 _GET_FV(PCI_SubClassCode, value)
 #define PCI_BaseClassCode_OFFSET		16
 #define PCI_BaseClassCode_MASK			REG_8BIT_MASK
 #define PCI_BaseClassCode(value)		\
	 _GET_FV(PCI_BaseClassCode, value)
 
 /* 7.5.1.1.9 Header Type Register (Offset 0Eh) */
 #define PCI_HeaderLayout_OFFSET			0
 #define PCI_HeaderLayout_MASK			REG_7BIT_MASK
 #define PCI_HeaderLayout(value)			\
	 _GET_FV(PCI_HeaderLayout, value)
 #define PCI_HeaderType0                 0x0
 #define PCI_HeaderType1                 0x1
 #define PCI_MultiFunctionDevice			_BV(7)
 
 /* 7.5.1.1.10 BIST Register (Offset 0Fh) */
 #define PCI_CompletionCode_OFFSET		0
 #define PCI_CompletionCode_MASK			REG_4BIT_MASK
 #define PCI_CompletionCode(value)		\
	 _GET_FV(PCI_CompletionCode, value)
 #define PCI_StartBIST				_BV(6)
 #define PCI_BISTCapable				_BV(7)
 
 /* 7.5.1.2.1 Base Address Registers (Offset 10h - 24h) */
 #define PCI_SpaceIndicator			_BV(0)
 #define PCI_SpaceIndicator_MemoryType   0
 #define PCI_SpaceIndicator_IOType       1
 #define PCI_MemoryType_OFFSET			1
 #define PCI_MemoryType_MASK			REG_2BIT_MASK
 #define PCI_MemoryType(value)			\
	 _GET_FV(PCI_MemoryType, value)
 #define PCI_Memory_Type_32BIT       0x0
 #define PCI_Memory_Type_64BIT       0x2
 #define PCI_MemoryPrefetchable			_BV(3)
 #define PCI_MemoryBaseAddress_OFFSET		4
 #define PCI_MemoryBaseAddress_MASK		REG_28BIT_MASK
 #define PCI_MemoryBaseAddress(value)		\
	 _GET_FV(PCI_MemoryBaseAddress, value)
 #define PCI_IOBaseAddress_OFFSET		2
 #define PCI_IOBaseAddress_MASK			REG_30BIT_MASK
 #define PCI_IOBaseAddress(value)		\
	 _GET_FV(PCI_IOBaseAddress, value)
 
 /* 7.5.1.2.4 Expansion ROM Base Address Register (Offset 30h) */
 #define PCI_ExpansionROMEnable				_BV(0)
 #define PCI_ExpansionROMValidationStatus_OFFSET		2
 #define PCI_ExpansionROMValidationStatus_MASK		REG_3BIT_MASK
 #define PCI_ExpansionROMValidationStatus(value)		\
	 _GET_FV(PCI_ExpansionROMValidationStatus, value)
 #define PCI_ValidationNotSupported			0
 #define PCI_ValidationInProgress			1
 #define PCI_ValidationPassValidContents			2
 #define PCI_ValidationPassValidTrustedContents		3
 #define PCI_ValidationFailInvalidContents		4
 #define PCI_ValidationFailValidUnstrustedContents	5
 #define PCI_ValidationPassValidContentsWarning		6
 #define PCI_ValidationPassValidTrustedContentsWarning	7
 #define PCI_ExpansionROMValidationDetails_OFFSET	4
 #define PCI_ExpansionROMValidationDetails_MASK		REG_4BIT_MASK
 #define PCI_ExpansionROMValidationDetails(value)	\
	 _GET_FV(PCI_ExpansionROMValidationDetails, value)
 
 /* 7.5.1.3.6 I/O Base/I/O Limit Registers(Offset 1Ch/1Dh) */
 #define PCI_IOAddressingCapability_OFFSET	0
 #define PCI_IOAddressingCapability_MASK		REG_4BIT_MASK
 #define PCI_IOAddressingCapability(value)	\
	 _GET_FV(PCI_IOAddressingCapability, value)
 
 /* 7.5.1.3.13 Bridge Control Register (Offset 3Eh) */
 #define PCI_ParityErrorResponseEnable			_BV(0)
 #define PCI_BridgeSERREnable				_BV(1)
 #define PCI_ISAEnable					_BV(2)
 #define PCI_VGAEnable					_BV(3)
 #define PCI_VGA16bitEnable				_BV(4)
 #define PCI_MasterAbortMode				_BV(5)
 #define PCI_SecondaryBusReset				_BV(6)
 #define PCI_BridgeFastBackToBackTransactionsEnable	_BV(7)
 #define PCI_PrimaryDiscardTimer				_BV(8)
 #define PCI_SecondaryDiscardTimer			_BV(9)
 #define PCI_DiscardTimerStatus				_BV(10)
 #define PCI_DiscardTimerSERREnable			_BV(11)
 
 /* 7.5.2.1 Power Management Capabilities Register (Offset 00h) */
 #define PCI_PM_Version_OFFSET				16
 #define PCI_PM_Version_MASK				REG_3BIT_MASK
 #define PCI_PM_Version(value)				\
	 _GET_FV(PCI_PM_Version, value)
 #define PCI_PM_PMEClock					_BV(19)
 #define PCI_PM_ImmediateReadinessOnReturnToD0		_BV(20)
 #define PCI_PM_DeviceSpecificInitialization		_BV(21)
 #define PCI_PM_AuxCurrent_OFFSET			22
 #define PCI_PM_AuxCurrent_MASK				REG_3BIT_MASK
 #define PCI_PM_AuxCurrent(value)			\
	 _GET_FV(PCI_PM_AuxCurrent, value)
 #define PCI_PM_SelfPowered				0
 #define PCI_PM_55mA					1
 #define PCI_PM_100mA					2
 #define PCI_PM_160mA					3
 #define PCI_PM_220mA					4
 #define PCI_PM_270mA					5
 #define PCI_PM_320mA					6
 #define PCI_PM_375mA					7
 #define PCI_PM_D1Support				_BV(23)
 #define PCI_PM_D2Support				_BV(24)
 #define PCI_PM_PMESupport_OFFSET			27
 #define PCI_PM_PMESupport_MASK				REG_5BIT_MASK
 #define PCI_PM_PMESupport				\
	 _GET_FV(PCI_PM_PMESupport, value)
 #define PCI_PM_PMED0					_BV(0)
 #define PCI_PM_PMED1					_BV(1)
 #define PCI_PM_PMED2					_BV(2)
 #define PCI_PM_PMED3Hot					_BV(3)
 #define PCI_PM_PMED3Cold				_BV(4)
 
 /* 7.5.2.2 Power Management Control/Status Register (Offset 04h) */
 #define PCI_PM_PowerState_OFFSET			0
 #define PCI_PM_PowerState_MASK				REG_2BIT_MASK
 #define PCI_PM_PowerState(value)			\
	 _SET_FV(PCI_PM_PowerState, value)
 #define PCI_PM_NoSoftReset				_BV(3)
 #define PCI_PM_PMEEn					_BV(8)
 #define PCI_PM_DataSelect_OFFSET			9
 #define PCI_PM_DataSelect_MASK				REG_4BIT_MASK
 #define PCI_PM_DataSelect(value)			\
	 _SET_FV(PCI_PM_DataSelect, value)
 #define PCI_PM_DataScale_OFFSET				13
 #define PCI_PM_DataScale_MASK				REG_2BIT_MASK
 #define PCI_PM_DataScale(value)				\
	 _SET_FV(PCI_PM_DataScale, value)
 #define PCI_PM_PMEStatus				_BV(15)
 
 #include <target/pcie.h>
 
 typedef union {
     struct {
	 uint32_t VendorID                 : 16;    /* 16-bits */
	 uint32_t DeviceID                 : 16;    /* 16-bits */
	 uint32_t Command                  : 16;    /* 16-bits */
	 uint32_t Status                   : 16;    /* 16-bits */
	 uint32_t RevisionID               : 8;     /*  8-bits */
	 uint32_t ClassCode                : 24;    /* 24-bits */
	 uint32_t CacheLineSize            : 8;     /*  8-bits */
	 uint32_t LatencyTimer             : 8;     /*  8-bits */
	 uint32_t HeaderType               : 8;     /*  8-bits */
	 uint32_t BIST                     : 8;     /*  8-bits */
	 uint32_t BAR0                     : 32;    /* 32-bits */
	 uint32_t BAR1                     : 32;    /* 32-bits */
	 uint32_t BAR2                     : 32;    /* 32-bits */
	 uint32_t BAR3                     : 32;    /* 32-bits */
	 uint32_t BAR4                     : 32;    /* 32-bits */
	 uint32_t BAR5                     : 32;    /* 32-bits */
	 uint32_t CardbusCISPointer        : 32;    /* 32-bits */
	 uint32_t SubsystemVendorID        : 16;    /* 16-bits */
	 uint32_t SubsystemID              : 16;    /* 16-bits */
	 uint32_t ExpansionROMBaseAddress0 : 32;    /* 32-bits */
	 uint32_t CapPointer               : 8;     /*  8-bits */
	 uint32_t Reserved_0               : 24;    /* 24-bits */
	 uint32_t Reserved_1               : 32;    /* 32-bits */
	 uint32_t MinGnt                   : 8;     /*  8-bits */
	 uint32_t MaxLat                   : 8;     /*  8-bits */
	 uint32_t InterruptLine            : 8;     /*  8-bits */
	 uint32_t InterruptPin             : 8;     /*  8-bits */
     } cfg0;
     struct {
	 uint32_t VendorID                 : 16;    /* 16-bits */
	 uint32_t DeviceID                 : 16;    /* 16-bits */
	 uint32_t Command                  : 16;    /* 16-bits */
	 uint32_t Status                   : 16;    /* 16-bits */
	 uint32_t RevisionID               : 8;     /*  8-bits */
	 uint32_t ClassCode                : 24;    /* 24-bits */
	 uint32_t CacheLineSize            : 8;     /*  8-bits */
	 uint32_t LatencyTimer             : 8;     /*  8-bits */
	 uint32_t HeaderType               : 8;     /*  8-bits */
	 uint32_t BIST                     : 8;     /*  8-bits */
	 uint32_t BAR0                     : 32;    /* 32-bits */
	 uint32_t BAR1                     : 32;    /* 32-bits */
	 uint32_t PrimaryBusNumber         : 8;     /*  8-bits */
	 uint32_t SecondaryBusNumber       : 8;     /*  8-bits */
	 uint32_t SubordinateBusNumber     : 8;     /*  8-bits */
	 uint32_t SecondaryLatencyTimer    : 8;     /*  8-bits */
	 uint32_t IOBase                   : 8;     /*  8-bits */
	 uint32_t IOLimit                  : 8;     /*  8-bits */
	 uint32_t SecondaryStatus          : 16;    /* 16-bits */
	 uint32_t MemoryBase               : 16;    /* 16-bits */
	 uint32_t MemoryLimit              : 16;    /* 16-bits */
	 uint32_t PrefetchableMemoryBase   : 16;    /* 16-bits */
	 uint32_t PrefetchableMemoryLimit  : 16;    /* 16-bits */
	 uint32_t PrefetchableMemoryBaseUpper32Bits : 32;    /* 32-bits */
	 uint32_t PrefetchableMemoryLimitUpper32Bits : 32;    /* 32-bits */
	 uint32_t IOBaseUpper16Bits        : 16;    /* 16-bits */
	 uint32_t IOLimitUpper16Bits       : 16;    /* 16-bits */
	 uint32_t ExpansionROMBaseAddress0 : 32;    /* 32-bits */
	 uint32_t CapPointer               : 8;     /*  8-bits */
	 uint32_t Reserved_0               : 24;    /* 24-bits */
	 uint32_t BridgeControl            : 16;    /* 16-bits */
	 uint32_t InterruptLine            : 8;     /*  8-bits */
	 uint32_t InterruptPin             : 8;     /*  8-bits */
	} cfg1;
	uint32_t value[16];
} pci_cfg;
 
/* CapabilityID */
#define PCI_CAP_PowerManagement				0x01

#ifdef CONFIG_PCIE_DEBUG
void pcie_dbg(const char *fmt, ...);
#else
#define pcie_dbg(...)	do { } while (0)
#endif
 
int pci_enum(int bus, int index);

#endif /* __PCI_H_INCLUDE__ */
 
