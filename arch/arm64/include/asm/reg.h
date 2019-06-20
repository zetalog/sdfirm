#ifndef __REG_ARM64_H_INCLUDE__
#define __REG_ARM64_H_INCLUDE__

#include <target/config.h>
#include <target/types.h>
#include <asm/io.h>
#include <asm/mach/reg.h>

#if defined(__ASSEMBLY__) && !defined(LINKER_SCRIPT)
/* Macro to switch to label based on current el */
.macro asm_switch_el xreg label1 label2 label3
	mrs	\xreg, CurrentEL
	/* Currently at EL1 */
	cmp	\xreg, #(ARM_EL1 << CURRENT_EL_OFFSET)
	b.eq	\label1
	/* Currently at EL2 */
	cmp	\xreg, #(ARM_EL2 << CURRENT_EL_OFFSET)
	b.eq	\label2
	/* Currently at EL3 */
	cmp	\xreg, #(ARM_EL3 << CURRENT_EL_OFFSET)
	b.eq	\label3
.endm
#endif

/* =================================================================
 * DDI0487B_b ARMv8 ARM
 * ARM Architecture Reference Manual - ARMv8, for ARMv8-A
 * architecture profile
 * ================================================================= */
/* Exception level */
#define ARM_EL0			0
#define ARM_EL1			1
#define ARM_EL2			2
#define ARM_EL3			3

/* Address size */
#define ARM_4GB			0
#define ARM_64GB		1
#define ARM_1TB			2
#define ARM_4TB			3
#define ARM_16TB		4
#define ARM_256TB		5 /* default value */
#define ARM_4PB			6

/* =================================================================
 * C.5.2 Special-purpose registers
 * ================================================================= */
/* C.5.2.1 CurrentEL, Current Exception Level */
#define CURRENT_EL_OFFSET	1
#define CURRENT_EL_MASK		REG_2BIT_MASK
#define CURRENT_EL(value)	_GET_FV(CURRENT_EL, value)

/* C5.2.17 SPSR_abt, Saved Program Status Register (Abort mode)
 * C5.2.18 SPSR_EL1, Saved Program Status Register (EL1)
 * C5.2.19 SPSR_EL2, Saved Program Status Register (EL2)
 * C5.2.20 SPSR_EL3, Saved Program Status Register (EL3)
 * G7.2.31 CPSR, Current Program Status Register
 */
/* AARCH64/AARCH32 common */
#define PSR_N			_BV(31)
#define PSR_Z			_BV(30)
#define PSR_C			_BV(29)
#define PSR_V			_BV(28)
#define PSR_SS			_BV(21)
#define PSR_IL			_BV(20)
#define PSR_A			_BV(8)
#define PSR_I			_BV(7)
#define PSR_F			_BV(6)
#define PSR_M			_BV(4)
#define PSR_MODE_OFFSET		0
#define PSR_MODE_MASK		REG_4BIT_MASK
#define PSR_MODE(value)		_GET_FV(PSR_MODE, value)
#define PSR_PAN			_BV(22)
#ifdef CONFIG_CPU_64v8_1_PAN
#define PSR_PAN_RES0		0
#else
#define PSR_PAN_RES0		PSR_PAN
#endif
#define CPSR_RES0		(_BV(23) | _BV(21) | _BV(20))
#define CPSR_RES1		_BV(4)

/* AARCH64 specific */
#define PSR_D			_BV(9)
#define PSR_UAO			_BV(23)
#ifdef CONFIG_CPU_64v8_2_UAO
#define PSR_UAO_RES0		0
#else
#define PSR_UAO_RES0		PSR_UAO
#endif
/* AARCH64 specific PSR_MODE */
#define PSR_MODE_EL0t		0x0
#define PSR_MODE_EL1t		0x4
#define PSR_MODE_EL1h		0x5
#define PSR_MODE_EL2t		0x8
#define PSR_MODE_EL2h		0x9
#define PSR_MODE_EL3t		0xC
#define PSR_MODE_EL3h		0xD
#define SPSR_AARCH64_RES0	(_BV(27) | _BV(26) | _BV(25) | _BV(24) | \
				 PSR_UAO_RES0 | PSR_PAN_RES0 | \
				 _BV(19）| _BV(18) | _BV(17) | _BV(16) | \
				 _BV(15) | _BV(14) | _BV(13) | _BV(12) | _BV(11) | _BV(10) \
				 _BV(5))

/* AARCH32 specific */
#define PSR_Q			_BV(27)
#define PSR_IT_lo_OFFSET	25
#define PSR_IT_lo_MASK		REG_2BIT_MASK
#define PSR_IT_lo(value)	_GET_FV(PSR_IT_lo, value)
#define PSR_J			_BV(24)
#define PSR_GE_OFFSET		16
#define PSR_GE_MASK		REG_4BIT_MASK
#define PSR_GE(value)		_GET_FV(PSR_GE, value)
#define PSR_IT_hi_OFFSET	10
#define PSR_IT_hi_MASK		REG_6BIT_MASK
#define PSR_IT_hi(value)	_GET_FV(PSR_IT_hi, value)
#define PSR_IT(value)		\
	(PSR_IT_lo(value) | (PSR_IT_hi(value) << PSR_IT_lo_OFFSET))
#define PSR_E			_BV(9)
#define PSR_T			_BV(5)
/* AARCH32 specific PSR_MODE */
#define PSR_MODE_USR		0x0
#define PSR_MODE_FIQ		0x1
#define PSR_MODE_IRQ		0x2
#define PSR_MODE_SVC		0x3
#define PSR_MODE_ABT		0x7
#define PSR_MODE_HYP		0xA
#define PSR_MODE_UND		0xB
#define PSR_MODE_SYS		0xF
#define PSR_ABT_RES0		(_BV(23) | PSR_PAN_RES0 | _BV(21))
#define SPSR_AARCH32_RES0	(_BV(23) | PSR_PAN_RES0)

/* =================================================================
 * D.10.2 General system control registers
 * ================================================================= */
/* D.10.2.19 CPTR_EL2, Architectural Feature Trap Register (EL2)
 * D.10.2.20 CPTR_EL3, Architectural Feature Trap Register (EL3)
 */
#define CPTR_TFP		_BV(10)
#define CPTR_TTA		_BV(20)
#define CPTR_TCPAC		_BV(31)

#define CPTR_EL2_RES0				\
	(_BV(11)|_BV(14)|_BV(15)|_BV(16)|	\
	 _BV(17)|_BV(18)|_BV(19)|_BV(21)|	\
	 _BV(22)|_BV(23)|_BV(24)|_BV(25)|	\
	 _BV(26)|_BV(27)|_BV(28)|_BV(29)|_BV(30))
#define CPTR_EL2_RES1				\
	(_BV(0)|_BV(1)|_BV(2)|_BV(3)|		\
	 _BV(4)|_BV(5)|_BV(6)|_BV(7)|		\
	 _BV(8)|_BV(9)|_BV(12)|_BV(13))
#define CPTR_EL3_RES0				\
	(CPTR_EL2_RES0|CPTR_EL2_RES1)

/* D.10.2.34 HCR_EL2, Hypervisor Configuration Register */
#define	HCR_FV(name, value)	_FV(HCR_##name, value)

#define HCR_VM			_BV(0) /* Enable virtualization */
#define HCR_SWIO		_BV(1) /* Set/way invalidation override */
#define HCR_PTW			_BV(2) /* Protected table walk */
#define HCR_FMO			_BV(3) /* Physical FIQ routing */
#define HCR_IMO			_BV(4) /* Physical IRQ routing */
#define HCR_AMO			_BV(5) /* Async external abort and SError IRQ routing */
#define HCR_VF			_BV(6) /* Virtual FIQ */
#define HCR_VI			_BV(7) /* Virtual IRQ */
#define HCR_VSE			_BV(8) /* Virtual system error or async abort */
#define HCR_FB			_BV(9) /* Force broadcast */

#define HCR_BSU_OFFSET		10
#define HCR_BSU_MASK		0x03
#define HCR_BSU(value)		HCR_FV(BSU, value) /* Barrier sharebility upgrade */
#define HCR_BSU_NO_EFFECT	0
#define HCR_BSU_INNER_SHAREABLE	1
#define HCR_BSU_OUTER_SHAREABLE	2
#define HCR_BSU_FULL_SYSTEM	3

#define HCR_DC			_DV(12) /* Default cacheable */
#define HCR_TWI			_BV(13) /* Trap WFI */
#define HCR_TWE			_BV(14) /* Trap WFE */
#define HCR_TID0		_BV(15) /* Trap ID group 0 */
#define HCR_TID1		_BV(16) /* Trap ID group 1 */
#define HCR_TID2		_BV(17) /* Trap ID group 2 */
#define HCR_TID3		_BV(18) /* Trap ID group 3 */
#define HCR_TSC			_BV(19) /* Trap SMC */
#define HCR_TIDCP		_BV(20) /* Trap implementation defined functionality */
#define HCR_TACR		_BV(21) /* Trap Auxiliary Control Register */
#define HCR_TSW			_BV(22) /* Trap data or cache maintenance by set/way */
#define HCR_TPCP		_BV(23) /* Trap data or cache maintenance of POC */
#define HCR_TPU			_BV(24) /* Trap cache maintenance */
#define HCR_TTLB		_BV(25) /* Trap TLB maintenance */
#define HCR_TVM			_BV(26) /* Trap virtual memory controls */
#define HCR_TGE			_BV(27) /* Trap general exception */
#define HCR_TDZ			_BV(28) /* Trap DC ZVA */
#define HCR_HCD			_BV(29) /* Disable HVC */
#define HCR_TRVM		_BV(30) /* Trap virtual memory control reads */
#define HCR_RW			_BV(31) /* Execution state for lower level */
#define HCR_CD			_BV(32) /* Disable data cache */
#define HCR_ID			_BV(33) /* Disable instruction cache */
#define HCR_E2H			_BV(34) /* EL2 Host */
#define HCR_TLOR		_BV(35) /* Trap LOR registers */
#define HCR_TERR		_BV(36) /* Trap Error record accesses */
#define HCR_TEA			_BV(37) /* Route synchronous External abort */
#define HCR_MIOCNCE		_BV(38) /* Enable mismatched Inner/Outer Cacheable Non-Coherency */

#define HCR_EL2_RES0				\
	(_BV(63)|_BV(62)|_BV(61)|_BV(60)|	\
	 _BV(59)|_BV(58)|_BV(57)|_BV(56)|	\
	 _BV(55)|_BV(54)|_BV(53)|_BV(52)|	\
	 _BV(51)|_BV(50)|_BV(49)|_BV(48)|	\
	 _BV(47)|_BV(46)|_BV(45)|_BV(44)|	\
	 _BV(43)|_BV(42)|_BV(41)|_BV(40)|	\
	 _BV(39)|_BV(37)|_BV(36)|_BV(35)|	\
	 _BV(34))

/* D10.2.74 MIDR_EL1, Main ID Register */
#define MIDR_IMPL_OFFSET	24
#define MIDR_IMPL_MASK		0xFF
#define MIDR_IMPL(value)	_GET_FV(MIDR_IMPL, value)

#define MIDR_IMPL_ARM		0x41 /* A */
#define MIDR_IMPL_BROADCOM	0x42 /* B */
#define MIDR_IMPL_CAVIUM	0x43 /* C */
#define MIDR_IMPL_DEC		0x44 /* D */
#define MIDR_IMPL_INFINEON	0x49 /* I */
#define MIDR_IMPL_MOTOROLA	0x4D /* M */
#define MIDR_IMPL_NVIDIA	0x4E /* N */
#define MIDR_IMPL_APPLE		0x50 /* P */
#define MIDR_IMPL_QUALCOMM	0x51 /* Q */
#define MIDR_IMPL_MARVELL	0x56 /* V */
#define MIDR_IMPL_HXT		0x68 /* h */
#define MIDR_IMPL_INTEL		0x69 /* i */

#define MIDR_VAR_OFFSET		20
#define MIDR_VAR_MASK		0x0F
#define MIDR_VAR(value)		_GET_FV(MIDR_VAR, value)

#define MIDR_ARCH_OFFSET	16
#define MIDR_ARCH_MASK		0x0F
#define MIDR_ARCH(value)	_GET_FV(MIDR_ARCH, value)

#define MIDR_ARCH_ARMV4		0x01
#define MIDR_ARCH_ARMV4T	0x02
#define MIDR_ARCH_ARMV5		0x03
#define MIDR_ARCH_ARMV5T	0x04
#define MIDR_ARCH_ARMV5TE	0x05
#define MIDR_ARCH_ARMV5TEJ	0x06
#define MIDR_ARCH_ARMV6		0x07

#define MIDR_PN_OFFSET		4
#define MIDR_PN_MASK		0x0FFF
#define MIDR_PN(value)		_GET_FV(MIDR_PN, value)

#define MIDR_PN_QCOM_FALKOR	0x0800
#define MIDR_PN_QCOM_FALKOR_V2	0x0C00
#define MIDR_PN_HXT_1		0x0000

#define MIDR_REV_OFFSET		0
#define MIDR_REV_MASK		0x0F
#define MIDR_REV(value)		_GET_FV(MIDR_REV, value)

/* D.10.2.81 RMR_EL1, Reset Management Register (EL1)
 * D.10.2.82 RMR_EL2, Reset Management Register (EL2)
 * D.10.2.83 RMR_EL3, Reset Management Register (EL3)
 */
#define RMR_AA64		_BV(0)
#define RMR_RR			_BV(1)
#define RMR_RES0		\
	(_BV(31)|_BV(30)|_BV(29)|_BV(28)|	\
	 _BV(27)|_BV(26)|_BV(25)|_BV(24)|	\
	 _BV(23)|_BV(22)|_BV(21)|_BV(20)|	\
	 _BV(19)|_BV(18)|_BV(17)|_BV(16)|	\
	 _BV(15)|_BV(14)|_BV(13)|_BV(12)|	\
	 _BV(11)|_BV(10)|_BV(9)|_BV(8)|		\
	 _BV(7)|_BV(6)|_BV(5)|_BV(4)|		\
	 _BV(3)|_BV(2))
#define RMR_EL1_RES0		RMR_RES0
#define RMR_EL2_RES0		RMR_RES0
#define RMR_EL3_RES0		RMR_RES0

/* D.10.2.88 SCR_EL3, Secure Configuration Register */
#define SCR_NS			_BV(0) /* Non-secure */
#define SCR_IRQ			_BV(1) /* Physical IRQ routing */
#define SCR_FIQ			_BV(2) /* Physical FIQ routing */
#define SCR_EA			_BV(3) /* External ABT and SError IRQ routing */
#define SCR_SMD			_BV(7) /* Disable SMC */
#define SCR_HCE			_BV(8) /* Disable HVC */
#define SCR_SIF			_BV(9) /* Secure instruction fetch */
#define SCR_RW			_BV(10) /* Execution state for lower level */
#define SCR_ST			_BV(11) /* Secure timer - trap EL1 CNTPS_ */
#define SCR_TWI			_BV(12) /* Trap WFI */
#define SCR_TWE			_BV(13) /* Trap WFE */

#define SCR_EL3_RES0				\
	(_BV(31)|_BV(30)|_BV(29)|_BV(28)|	\
	 _BV(27)|_BV(26)|_BV(25)|_BV(24)|	\
	 _BV(23)|_BV(22)|_BV(21)|_BV(20)|	\
	 _BV(19)|_BV(18)|_BV(17)|_BV(16)|	\
	 _BV(15)|_BV(14)|_BV(6))
#define SCR_EL3_RES1		(_BV(5)|_BV(4))

/* D.10.2.89 SCTLR_EL1, System Control Register (EL1)
 * D.10.2.90 SCTLR_EL2, System Control Register (EL2)
 * D.10.2.91 SCTLR_EL3, System Control Register (EL3)
 */
/* EL1 only: */
#define SCTLR_SA0		_BV(4)	/* Check EL0 stack alignment */
#define SCTLR_CP16BEN		_BV(5)	/* Enable CP15 memory barrier */
#define SCTLR_IT		_BV(7)	/* Trap EL0 IT */
#define SCTLR_SED		_BV(8)	/* Trap EL0 SETEND */
#define SCTLR_UMA		_BV(9)	/* Trap EL0 DAIF access */
#define SCTLR_DZE		_BV(14)	/* Trap EL0 DC ZVA */
#define SCTLR_UCT		_BV(15)	/* Trap EL0 CTR_EL0 access */
#define SCTLR_nTWI		_BV(16)	/* Trap EL0 WFI */
#define SCTLR_nTWE		_BV(18)	/* Trap EL0 WFE */
#define SCTLR_E0E		_BV(24)	/* EL0 endianness */
#define SCTLR_UCI		_BV(26)	/* Trap EL0 cache maintenance */

#define SCTLR_EL1_RES0				\
	(_BV(31)|_BV(30)|_BV(27)|_BV(21)|	\
	 _BV(17)|_BV(13)|_BV(10)|_BV(6))
#define SCTLR_EL1_RES1				\
	(_BV(29)|_BV(28)|_BV(23)|_BV(22)|	\
	 _BV(20)|_BV(11))

/* EL3/EL2/EL1: */
#define SCTLR_M			_BV(0)	/* Enable MMU */
#define SCTLR_A			_BV(1)	/* Check alignment */
#define SCTLR_C			_BV(2)	/* Cacheability */
#define SCTLR_SA		_BV(3)	/* Check stack alignment */
#define SCTLR_I			_BV(12)	/* Instruction access cacheability */
#define SCTLR_WXN		_BV(19)	/* Write permission Execute-Never */
#define SCTLR_EE		_BV(25)	/* Endianness */
#define SCTLR_RES0				\
	(_BV(31)|_BV(30)|_BV(27)|_BV(26)|	\
	 _BV(24)|_BV(21)|_BV(20)|_BV(17)|	\
	 _BV(15)|_BV(14)|_BV(13)|_BV(10)|	\
	 _BV(9)|_BV(8)|_BV(7)|_BV(6))
#define SCTLR_RES1				\
	(_BV(29)|_BV(28)|_BV(23)|_BV(22)|	\
	 _BV(18)|_BV(16)|_BV(11)|_BV(5)|_BV(4))
#define SCTLR_EL2_RES0		SCTLR_RES0
#define SCTLR_EL2_RES1		SCTLR_RES1
#define SCTLR_EL3_RES0		SCTLR_RES0
#define SCTLR_EL3_RES1		SCTLR_RES1

/* D10.2.92 TCR_EL1, Translation Control Register (EL1)
 * D10.2.93 TCR_EL2, Translation Control Register (EL2)
 * D10.2.94 TCR_EL3, Translation Control Register (EL3)
 * TCR_EL1, TCR_EL2, or TCR_EL3
 */
#define TCR_SH0_OFFSET		12 /* Shareability for TTBR0_EL1 walk */
#define TCR_SH0_MASK		REG_2BIT_MASK
#define TCR_SH0(value)		_SET_FV(TCR_SH0, value)
#define TCR_SH_NON		0
#define TCR_SH_OUTER		2
#define TCR_SH_INNER		3
#define TCR_ORGN0_OFFSET	10 /* Outer cacheability for TTBR0_EL1 walk */
#define TCR_ORGN0_MASK		REG_2BIT_MASK
#define TCR_ORGN0(value)	_SET_FV(TCR_ORGN0, value)
#define TCR_IRGN0_OFFSET	8 /* Inner cacheability for TTBR0_EL1 walk */
#define TCR_IRGN0_MASK		REG_2BIT_MASK
#define TCR_IRGN0(value)	_SET_FV(TCR_IRGN0, value)
#define TCR_RGN_NC		0 /* Non-Cacheable */
/* Write-Back Read-Allocate Write-Allocate Cacheable */
#define TCR_RGN_WB_WA		1
/* Write-Through Read-Allocate No Write-Allocate Cacheable */
#define TCR_RGN_WT_NWA		2
/* Write-Back Read-Allocate No Write-Allocate Cacheable */
#define TCR_RGN_WB_NWA		3
/* Size offset of memory region addressed by TTBR0_EL1 */
#define TCR_T0SZ_OFFSET		0
#define TCR_T0SZ_MASK		REG_6BIT_MASK
#define TCR_T0SZ(value)		_SET_FV(TCR_T0SZ, value)

/* D10.2.92 TCR_EL1, Translation Control Register (EL1)
 * D10.2.93 TCR_EL2, Translation Control Register (EL2)
 * TCR_EL1, or TCR_EL2 when HCR_EL2.E2H=1 (EL2 Host)
 */
/* ARMv8.2+ */
/* present if Scalable Vector Extension (SVE) */
#define TCR_NFD1		_BV(54) /* Non-fault table walk Disable */
#define TCR_NFD0		_BV(53)
/* ARMv8.1+ */
/* present if Hierarchical permission disables (HPD) */
#define TCR_HPD1		_BV(42) /* Hierarchical Permisiion Disable */
#define TCR_HPD0		_BV(41)
/* present if Hardware Management of the Access flag and dirty state (TTHM) */
#define TCR_HDx			_BV(40) /* Hardware management of Dirty state */
#define TCR_HAx			_BV(39) /* Hardware Access flag update */
/* ARMv8 */
#define TCR_TBI1		_BV(38) /* Top Byte Ignored */
#define TCR_TBI0		_BV(37)
#define TCR_AS			_BV(36) /* ASID Size */
#define TCR_IPS_OFFSET		32 /* Intermediate Physical Address Size */
#define TCR_IPS_MASK		REG_3BIT_MASK
#define TCR_IPS(value)		_GET_FV(TCR_IPS, value)
#define TCR_TG1_OFFSET		30 /* Granule for TTBR1_EL1 */
#define TCR_TG1_MASK		REG_2BIT_MASK
#define TCR_TG1(value)		_SET_FV(TCR_TG1, value)
#define TCR_TG1_16KB		1
#define TCR_TG1_4KB		2
#define TCR_TG1_64KB		3
#define TCR_SH1_OFFSET		28 /* Shareability for TTBR1_EL1 walk */
#define TCR_SH1_MASK		REG_2BIT_MASK
#define TCR_SH1(value)		_SET_FV(TCR_SH1, value)
#define TCR_ORGN1_OFFSET	26 /* Outer cacheability for TTBR1_EL1 walk */
#define TCR_ORGN1_MASK		REG_2BIT_MASK
#define TCR_ORGN1(value)	_SET_FV(TCR_ORGN1, value)
#define TCR_IRGN1_OFFSET	24 /* Inner cacheability for TTBR1_EL1 walk */
#define TCR_IRGN1_MASK		REG_2BIT_MASK
#define TCR_IRGN1(value)	_SET_FV(TCR_IRGN1, value)
/* TTBR1_EL1/TTBR0_EL1 table walk disable on TLB miss */
#define TCR_EPD1		_BV(23)
#define TCR_EPD0		_BV(7)
/* Select whether TTBR0_EL1(0) or TTBR1_EL1(1) defines the ASID */
#define TCR_A1			_BV(22)
/* Size offset of memory region addressed by TTBR1_EL1 */
#define TCR_T1SZ_OFFSET		16
#define TCR_T1SZ_MASK		REG_6BIT_MASK
#define TCR_T1SZ(value)		_SET_FV(TCR_T1SZ, value)
#define TCR_TG0_OFFSET		14 /* Granule for TTBR0_EL1 */
#define TCR_TG0_MASK		REG_2BIT_MASK
#define TCR_TG0(value)		_SET_FV(TCR_TG0, value)
#define TCR_TG0_4KB		0
#define TCR_TG0_64KB		1
#define TCR_TG0_16KB		2

/* D10.2.93 TCR_EL2, Translation Control Register (EL2)
 * D10.2.94 TCR_EL3, Translation Control Register (EL3)
 * TCR_EL2 when HCR_EL2.E2H=0 (EL2 Hypervisor), or TCR_EL3
 */
/* ARMv8.1+ */
/* present if Hierarchical permission disables (HPD) */
#define TCR_HPD			_BV(24) /* Hierarchical Permisiion Disable */
/* present if Hardware Management of the Access flag and dirty state (TTHM) */
#define TCR_HD			_BV(22) /* Hardware management of Dirty state */
#define TCR_HA			_BV(21) /* Hardware Access flag update */
#define TCR_TBI			_BV(20) /* Top Byte Ignored */
#define TCR_PS_OFFSET		16 /* Physical Address Size */
#define TCR_PS_MASK		REG_3BIT_MASK
#define TCR_PS(value)		_SET_FV(TCR_PS, value)

/* =================================================================
 * D.10.3 Debug registers
 * ================================================================= */
/* D.10.3.16 MDCCSR_EL0, Monitor DCC Status Register */
#define MDCCSR_TXFULL		_BV(29)
#define MDCCSR_RXFULL		_BV(30)

/* D10.3.18 MDCR_EL3, Monitor Debug Configuration Register (EL3)
 */
#define MDCR_EPMAD		_BV(21)
#define MDCR_EDAD		_BV(20)
#define MDCR_SPME		_BV(17)
#define MDCR_SDD		_BV(16)
#define MDCR_SPD32_OFFSET	14
#define MDCR_SPD32_MASK		0x03
#define MDCR_SPD32(value)	_FV(MDCR_SPD32, value)
#define MDCR_NSPB_OFFSET	12
#define MDCR_NSPB_MASK		0x03
#define MDCR_NSPB(value)	_FV(MDCR_NSPB, value)
#define MDCR_TDOSA		_BV(10)
#define MDCR_TDA		_BV(9)
#define MDCR_TPM		_BV(6)

/* =================================================================
 * D.10.6 Generic Timer registers
 * ================================================================= */
/* D.10.6.2 CNTHCTL_EL2, Counter-timer Hypervisor Control register */
#define CNTHCTL_EL1PCTEN	_BV(0)
#define CNTHCTL_EL1PCEN		_BV(1)

#define CNTHCTL_EL2_RES0			\
	(_BV(31)|_BV(30)|_BV(29)|_BV(28)|	\
	 _BV(27)|_BV(26)|_BV(25)|_BV(24)|	\
	 _BV(23)|_BV(21)|_BV(20)|_BV(19)|	\
	 _BV(18)|_BV(17)|_BV(16)|_BV(15)|	\
	 _BV(14)|_BV(13)|_BV(12)|_BV(11)|	\
	 _BV(10)|_BV(9)_BV(8))

/* D10.6.9 CNTKCTL_EL1, Conter-timer Kernel Control register */
#define CNTKCTL_EL0PCTEN	_BV(0)
#define CNTKCTL_EL0VCTEN	_BV(1)
#define CNTKCTL_EL0VTEN		_BV(8)
#define CNTKCTL_EL0PTEN		_BV(9)

#define CNTKCTL_EL1_RES0			\
	(_BV(31)|_BV(30)|_BV(29)|_BV(28)|	\
	 _BV(27)|_BV(26)|_BV(25)|_BV(24)|	\
	 _BV(23)|_BV(21)|_BV(20)|_BV(19)|	\
	 _BV(18)|_BV(17)|_BV(16)|_BV(15)|	\
	 _BV(14)|_BV(13)|_BV(12)|_BV(11)|	\
	 _BV(10))

/* Fields common to CNTHCTL_EL2/CNTKCTL_EL1 */
#define	CNTXCTL_FV(name, value)	_FV(CNTXCTL_##name, value)

#define CNTXCTL_EVNTEN		_BV(2)
#define CNTXCTL_EVNTDIR		_BV(3)
#define CNTXCTL_EVNTI_OFFSET	4
#define CNTXCTL_EVNTI_MASK	0x0F
#define CNTXCTL_EVNTI(value)	CNTXCTL_FV(EVNTI, value)

/* D10.6.3  CNTHP_CTL_EL2,
 *          Counter-timer Hypervisor Physical Timer Control register
 * D10.6.6  CNTHV_CTL_EL2,
 *          Counter-timer Virtual Timer Control register (EL2)
 * D10.6.10 CNTP_CTL_EL0,
 *          Counter-timer Physical Timer Control register
 * D10.6.14 CNTPS_CTL_EL1,
 *          Counter-timer Physical Secure Timer Control register
 */
#define CNTX_CTL_ENABLE		_BV(0)
#define CNTX_CTL_IMASK		_BV(1)
#define CNTX_CTL_ISTATUS	_BV(2)

#endif /* __REG_ARM64_H_INCLUDE__ */
