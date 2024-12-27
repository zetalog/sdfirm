/*
 * Author: Lv Zheng <lv.zheng@hxt-semitech.com>
 */
#ifndef __CORESIGHT_ARM64_H_INCLUDE__
#define __CORESIGHT_ARM64_H_INCLUDE__

#include <stdio.h>
#include <target/generic.h>
#include <target/jep106.h>
#include <asm/io.h>

/* JEP106 Identification, used in PIDR, DEVARCH */
#define CORESIGHT_JEP106_IDENT_OFFSET	0
#define CORESIGHT_JEP106_IDENT_MASK	0x7F
#define CORESIGHT_JEP106_IDENT(value)	_SET_FV(CORESIGHT_JEP106_IDENT, value)
#define CORESIGHT_JEP106_CONT_OFFSET	7
#define CORESIGHT_JEP106_CONT_MASK	0x0F
#define CORESIGHT_JEP106_CONT(value)	_SET_FV(CORESIGHT_JEP106_CONT, value)
#define CORESIGHT_JEP106(ident, cont)	((cont) << 7 | (ident))

#define CORESIGHT_JEP106_ARM	CORESIGHT_JEP106(JEP106_IDENT_ARM, 4)
#define CORESIGHT_JEP106_QCOM	CORESIGHT_JEP106(JEP106_IDENT_QUALCOMM, 0)
#define CORESIGHT_JEP106_HXT	CORESIGHT_JEP106(JEP106_IDENT_HXT, 9)

/* Device Architecture ID, used in DEVARCH */
#define CORESIGHT_ARCH_RAS		0x0A00
#define CORESIGHT_ARCH_ITM		0x1A01 /* Instrumentation Trace Macrocell */
#define CORESIGHT_ARCH_DWT		0x1A02
#define CORESIGHT_ARCH_FPB		0x1A03 /* Flash Patch and Breakpoint */
#define CORESIGHT_ARCH_DBG_CPUv8M	0x2A04 /* Processor debug */
#define CORESIGHT_ARCH_DBG_CPUv8R	0x6A05 /* Processor debug */
#define CORESIGHT_ARCH_PC_PROFILING	0x0A10 /* PC sample-based profiling */
#define CORESIGHT_ARCH_ETM		0x4A13 /* Embedded Trace Macrocell */
#define CORESIGHT_ARCH_CTI		0x1A14 /* Cross Trigger Interface */
#define CORESIGHT_ARCH_DBG_CPUv80A	0x6A15 /* Processor debug */
#define CORESIGHT_ARCH_DBG_CPUv81A	0x7A15 /* Processor debug */
#define CORESIGHT_ARCH_DBG_CPUv82A	0x8A15 /* Processor debug */
#define CORESIGHT_ARCH_PMU		0x2A16 /* Processor Performance Monitor */
#define CORESIGHT_ARCH_MEM_AP		0x0A17 /* Memory Access Port v2 */
#define CORESIGHT_ARCH_JTAG_AP		0x0A27 /* JTAG Access Port v2 */
#define CORESIGHT_ARCH_BTR		0x0A31 /* Base Trace Router */
#define CORESIGHT_ARCH_POWER_REQ	0x0A37 /* Power requestor */
#define CORESIGHT_ARCH_UNKNOWN_AP	0x0A47 /* Unknown Access Port v2 */
#define CORESIGHT_ARCH_HSSTP		0x0A50 /* HSSTP */
#define CORESIGHT_ARCH_STM		0x0A63 /* System Trace Macrocell */
#define CORESIGHT_ARCH_ELA		0x0A75 /* ELA */
#define CORESIGHT_ARCH_ROM		0x0AF7 /* ROM */

/* Device Type (MAJOR/SUB), used in DEVTYPE */
#define CORESIGHT_TYPE_MISC		0x00
#define CORESIGHT_TYPE_TRACE_SINK	0x01
#define CORESIGHT_TYPE_TRACE_LINK	0x02
#define CORESIGHT_TYPE_TRACE_SOURCE	0x03
#define CORESIGHT_TYPE_DEBUG_CONTROL	0x04
#define CORESIGHT_TYPE_DEBUG_LOGIC	0x05
#define CORESIGHT_TYPE_PERF_MONITOR	0x06

#define CORESIGHT_TYPE(major, sub)	((major) << 4 | (sub))

#define CORESIGHT_TYPE_VALIDATION		\
	CORESIGHT_TYPE(CORESIGHT_TYPE_MISC, 0x04)
#define CORESIGHT_TYPE_TPIU			\
	CORESIGHT_TYPE(CORESIGHT_TYPE_TRACE_SINK, 0x01)
#define CORESIGHT_TYPE_ETB			\
	CORESIGHT_TYPE(CORESIGHT_TYPE_TRACE_SINK, 0x02)
#define CORESIGHT_TYPE_ETR			\
	CORESIGHT_TYPE(CORESIGHT_TYPE_TRACE_SINK, 0x02)
#define CORESIGHT_TYPE_BTR			\
	CORESIGHT_TYPE(CORESIGHT_TYPE_TRACE_SINK, 0x03)
#define CORESIGHT_TYPE_FUNNEL			\
	CORESIGHT_TYPE(CORESIGHT_TYPE_TRACE_LINK, 0x01)
#define CORESIGHT_TYPE_REPLICATOR		\
	CORESIGHT_TYPE(CORESIGHT_TYPE_TRACE_LINK, 0x02)
#define CORESIGHT_TYPE_TMC			\
	CORESIGHT_TYPE(CORESIGHT_TYPE_TRACE_LINK, 0x03)
#define CORESIGHT_TYPE_TRACE_CPU		\
	CORESIGHT_TYPE(CORESIGHT_TYPE_TRACE_SOURCE, 0x01)
#define CORESIGHT_TYPE_TRACE_DSP		\
	CORESIGHT_TYPE(CORESIGHT_TYPE_TRACE_SOURCE, 0x02)
#define CORESIGHT_TYPE_TRACE_CP			\
	CORESIGHT_TYPE(CORESIGHT_TYPE_TRACE_SOURCE, 0x03)
#define CORESIGHT_TYPE_TRACE_BUS		\
	CORESIGHT_TYPE(CORESIGHT_TYPE_TRACE_SOURCE, 0x04)
#define CORESIGHT_TYPE_TRACE_SYS		\
	CORESIGHT_TYPE(CORESIGHT_TYPE_TRACE_SOURCE, 0x06)
#define CORESIGHT_TYPE_ECT			\
	CORESIGHT_TYPE(CORESIGHT_TYPE_DEBUG_CONTROL, 0x01)
#define CORESIGHT_TYPE_DEBUG_AUTH		\
	CORESIGHT_TYPE(CORESIGHT_TYPE_DEBUG_CONTROL, 0x02)
#define CORESIGHT_TYPE_DEBUG_CPU		\
	CORESIGHT_TYPE(CORESIGHT_TYPE_DEBUG_LOGIC, 0x01)
#define CORESIGHT_TYPE_DEBUG_DSP		\
	CORESIGHT_TYPE(CORESIGHT_TYPE_DEBUG_LOGIC, 0x02)
#define CORESIGHT_TYPE_DEBUG_CP			\
	CORESIGHT_TYPE(CORESIGHT_TYPE_DEBUG_LOGIC, 0x03)
#define CORESIGHT_TYPE_DEBUG_BUS		\
	CORESIGHT_TYPE(CORESIGHT_TYPE_DEBUG_LOGIC, 0x04)
#define CORESIGHT_TYPE_DEBUG_BIST		\
	CORESIGHT_TYPE(CORESIGHT_TYPE_DEBUG_LOGIC, 0x05)
#define CORESIGHT_TYPE_MONITOR_CPU		\
	CORESIGHT_TYPE(CORESIGHT_TYPE_PERF_MONITOR, 0x01)
#define CORESIGHT_TYPE_MONITOR_DSP		\
	CORESIGHT_TYPE(CORESIGHT_TYPE_PERF_MONITOR, 0x02)
#define CORESIGHT_TYPE_MONITOR_CP		\
	CORESIGHT_TYPE(CORESIGHT_TYPE_PERF_MONITOR, 0x03)
#define CORESIGHT_TYPE_MONITOR_BUS		\
	CORESIGHT_TYPE(CORESIGHT_TYPE_PERF_MONITOR, 0x04)
#define CORESIGHT_TYPE_MONITOR_MMU		\
	CORESIGHT_TYPE(CORESIGHT_TYPE_PERF_MONITOR, 0x05)

/* ======================================================================
 * Component and Peripheral Identification Registers
 * ====================================================================== */
#define CORESIGHT_REG(base, offset)		(base + offset)
#define CORESIGHT_REGn(base, offset, n)		\
	CORESIGHT_REG(base, (offset) + (n) * 4)
#define CORESIGHT_LREGn(base, offset, n)	\
	CORESIGHT_REG(base, (offset) + (n) * 8)

#define ITCTRL(comp)		CORESIGHT_REG(comp, 0xF00)
#define CLAIMSET(comp)		CORESIGHT_REG(comp, 0xFA0)
#define CLAIMCLR(comp)		CORESIGHT_REG(comp, 0xFA4)
#define DEVAFF0(comp)		CORESIGHT_REG(comp, 0xFA8)
#define DEVAFF1(comp)		CORESIGHT_REG(comp, 0xFAC)
#define LAR(comp)		CORESIGHT_REG(comp, 0xFB0)
#define LSR(comp)		CORESIGHT_REG(comp, 0xFB4)
#define AUTHSTATUS(comp)	CORESIGHT_REG(comp, 0xFB8)
#define DEVARCH(comp)		CORESIGHT_REG(comp, 0xFBC)
#define DEVID2(comp)		CORESIGHT_REG(comp, 0xFC0)
#define DEVID1(comp)		CORESIGHT_REG(comp, 0xFC4)
#define DEVID(comp)		CORESIGHT_REG(comp, 0xFC8)
#define DEVTYPE(comp)		CORESIGHT_REG(comp, 0xFCC)
#define PIDR4(comp)		CORESIGHT_REG(comp, 0xFD0)
#define PIDR5(comp)		CORESIGHT_REG(comp, 0xFD4)
#define PIDR6(comp)		CORESIGHT_REG(comp, 0xFD8)
#define PIDR7(comp)		CORESIGHT_REG(comp, 0xFDC)
#define PIDR0(comp)		CORESIGHT_REG(comp, 0xFE0)
#define PIDR1(comp)		CORESIGHT_REG(comp, 0xFE4)
#define PIDR2(comp)		CORESIGHT_REG(comp, 0xFE8)
#define PIDR3(comp)		CORESIGHT_REG(comp, 0xFEC)
#define CIDR0(comp)		CORESIGHT_REG(comp, 0xFF0)
#define CIDR1(comp)		CORESIGHT_REG(comp, 0xFF4)
#define CIDR2(comp)		CORESIGHT_REG(comp, 0xFF8)
#define CIDR3(comp)		CORESIGHT_REG(comp, 0xFFC)

/* B2.2.1 CIDR0-CIDR3, Component Identification Registers */
#define CIDR0_PRMBL0_OFFSET	0
#define CIDR0_PRMBL0_MASK	0xFF
#define CIDR0_PRMBL0(value)	_GET_FV(CIDR0_PRMBL0, value)
#define CIDR0_PRMBL0_DEF	0x0D
#define CIDR1_PRMBL1_OFFSET	0
#define CIDR1_PRMBL1_MASK	0xF
#define CIDR1_PRMBL1(value)	_GET_FV(CIDR1_PRMBL1, value)
#define CIDR1_PRMBL1_DEF	0x00
#define CIDR1_CLASS_OFFSET	4
#define CIDR1_CLASS_MASK	0xF
#define CIDR1_CLASS(value)	_GET_FV(CIDR1_CLASS, value)
#define CIDR1_CLASS_GENERIC_VERIFICATION_COMPONENT	0x0
#define CIDR1_CLASS_ROM_TABLE				0x1
#define CIDR1_CLASS_CORESIGHT_COMPONENT			0x9
#define CIDR1_CLASS_PERIPHERAL_TEST_BLOCK		0xB
#define CIDR1_CLASS_GENERIC_IP_COMPONENT		0xE
#define CIDR1_CLASS_SYSTEM_COMPONENT			0xF
#define CIDR2_PRMBL2_OFFSET	0
#define CIDR2_PRMBL2_MASK	0xFF
#define CIDR2_PRMBL2(value)	_GET_FV(CIDR2_PRMBL2, value)
#define CIDR2_PRMBL2_DEF	0x05
#define CIDR3_PRMBL3_OFFSET	0
#define CIDR3_PRMBL3_MASK	0xFF
#define CIDR3_PRMBL3(value)	_GET_FV(CIDR3_PRMBL3, value)
#define CIDR3_PRMBL3_DEF	0xB1

/* B2.2.2 PIDR0-PIDR7, Peripheral Identification Registers */
#define PIDR0_PART0_OFFSET	0
#define PIDR0_PART0_MASK	0xFF
#define PIDR0_PART0(value)	_GET_FV(PIDR0_PART0, value)
#define PIDR1_PART1_OFFSET	0
#define PIDR1_PART1_MASK	0xF
#define PIDR1_PART1(value)	_GET_FV(PIDR1_PART1, value)
#define PIDR1_DES0_OFFSET	4
#define PIDR1_DES0_MASK		0xF
#define PIDR1_DES0(value)	_GET_FV(PIDR1_DES0, value)
#define PIDR2_DES1_OFFSET	0
#define PIDR2_DES1_MASK		0x7
#define PIDR2_DES1(value)	_GET_FV(PIDR2_DES1, value)
#define PIDR2_JEDEC		_BV(3)
#define PIDR2_REVISION_OFFSET	4
#define PIDR2_REVISION_MASK	0xF
#define PIDR2_REVISION(value)	_GET_FV(PIDR2_REVISION, value)
#define PIDR3_CMOD_OFFSET	0
#define PIDR3_CMOD_MASK		0xF
#define PIDR3_CMOD(value)	_GET_FV(PIDR3_CMOD, value)
#define PIDR3_REVAND_OFFSET	4
#define PIDR3_REVAND_MASK	0xF
#define PIDR3_REVAND(value)	_GET_FV(PIDR3_REVAND, value)
#define PIDR4_DES2_OFFSET	0
#define PIDR4_DES2_MASK		0xF
#define PIDR4_DES2(value)	_GET_FV(PIDR4_DES2, value)
#define PIDR4_SIZE_OFFSET	4
#define PIDR4_SIZE_MASK		0xF
#define PIDR4_SIZE(value)	_GET_FV(PIDR4_SIZE, value)

/* JEP106 Identification Code */
#define PIDR_JEP106_IDENT(pidr1, pidr2)	\
	(PIDR1_DES0(pidr1) | (PIDR2_DES1(pidr2) << 4))
/* JEP106 Continuation Code */
#define PIDR_JEP106_CONT(pidr4)		\
	PIDR4_DES2(pidr4)
#define PIDR_PART(pidr0, pidr1)		\
	(PIDR0_PART0(pidr0) | (PIDR1_PART1(pidr1) << 8))

#define coresight_get_class(base)	\
	CIDR1_CLASS(__raw_readl(CIDR1(base)))
#define coresight_get_part(base)	\
	PIDR_PART(__raw_readl(PIDR0(base)), __raw_readl(PIDR1(base)))
#define pidr_jep106_ident(base)		\
	PIDR_JEP106_IDENT(__raw_readl(PIDR1(base)), __raw_readl(PIDR2(base)))
#define pidr_jep106_cont(base)		\
	PIDR_JEP106_CONT(__raw_readl(PIDR4(base)))

/* B2.3.4 DEVARCH, Device Architecture Register */
#define DEVARCH_ARCHID_OFFSET		0
#define DEVARCH_ARCHID_MASK		0xFFFF
#define DEVARCH_ARCHID(value)		_GET_FV(DEVARCH_ARCHID, value)
#define DEVARCH_REVISION_OFFSET		16
#define DEVARCH_REVISION_MASK		0x0F
#define DEVARCH_REVISION(value)		_GET_FV(DEVARCH_REVISION, value)
#define DEVARCH_PRESENT			_BV(20)
#define DEVARCH_ARCHITECT_OFFSET	21
#define DEVARCH_ARCHITECT_MASK		0x7FF
#define DEVARCH_ARCHITECT(value)	_GET_FV(DEVARCH_ARCHITECT, value)

/* B2.3.8 DEVTYPE, Device Type Identifier Register */
#define DEVTYPE_MAJOR_OFFSET		0
#define DEVTYPE_MAJOR_MASK		0x0F
#define DEVTYPE_MAJOR(value)		_GET_FV(DEVTYPE_MAJOR, value)
#define DEVTYPE_SUB_OFFSET		4
#define DEVTYPE_SUB_MASK		0x0F
#define DEVTYPE_SUB(value)		_GET_FV(DEVTYPE_SUB, value)

/* DEVID register */
#define DEVID_FORMAT_OFFSET		0
#define DEVID_FORMAT_MASK		REG_3BIT_MASK
#define DEVID_FORMAT(value)		_GET_FV(DEVID_FORMAT, value)
#define DEVID_PRR			_BV(5)

/* ======================================================================
 * CLASS 0x1 ROM Tables
 * ====================================================================== */
#define ROMENTRYn(base, n)		CORESIGHT_REGn(base, 0, n)
#define MEMTYPE(base)			CORESIGHT_REG(base, 0xFCC)
#define LROMENTRYn(base, n)		CORESIGHT_LREGn(base, 0, n)

/* D6.4.4 ROMENTRY<n>, Class 0x1 ROM Table entries */
#define ROMENTRY_PRESENT		_BV(0)
#define ROMENTRY_FORMAT			_BV(1)
#define ROMENTRY_POWERIDVALID		_BV(2)
#define ROMENTRY_POWERID_OFFSET		4
#define ROMENTRY_POWERID_MASK		0x1F
#define ROMENTRY_POWERID(value)		_GET_FV(ROMENTRY_POWERID, value)
#define ROMENTRY_OFFSET_OFFSET		12
#define ROMENTRY_OFFSET_MASK		0xFFFFF
#define ROMENTRY_OFFSET(value)		_GET_FV(ROMENTRY_OFFSET, value)
#define LROMENTRY_OFFSET_OFFSET		12
#define LROMENTRY_OFFSET_MASK		ULL(0xFFFFFFFFFFFFF)
#define LROMENTRY_OFFSET(value)		_GET_FV_ULL(LROMENTRY_OFFSET, value)

/* D6.4.2 MEMTYPE, Memory Type Register */
#define MEMTYPE_SYSMEM			_BV(0)

/* ======================================================================
 * CLASS 0x9 ROM Tables
 * ====================================================================== */

/* ======================================================================
 * MEM AP Registers
 * ====================================================================== */

/* Main control and status registers */
#define CORESIGHT_MEM_AP_CSW(base)	CORESIGHT_REG(base, 0xD00)
#define CORESIGHT_MEM_AP_TAR(base)	CORESIGHT_REG(base, 0xD04)
#define CORESIGHT_MEM_AP_DRW(base)	CORESIGHT_REG(base, 0xD0C)
#define CORESIGHT_MEM_AP_BD(base, n)	CORESIGHT_REG(base, (0xD10 + (n << 2)))
#define CORESIGHT_MEM_AP_MBT(base)	CORESIGHT_REG(base, 0xD20)
#define CORESIGHT_MEM_AP_TRR(base)	CORESIGHT_REG(base, 0xD24)
#define CORESIGHT_MEM_AP_T0TR(base)	CORESIGHT_REG(base, 0xD30)
#define CORESIGHT_MEM_AP_MECID(base)	CORESIGHT_REG(base, 0xDDC)
#define CORESIGHT_MEM_AP_CFG1(base)	CORESIGHT_REG(base, 0xDE0)
#define CORESIGHT_MEM_AP_BASE1(base)	CORESIGHT_REG(base, 0xDF0)
#define CORESIGHT_MEM_AP_CFG(base)	CORESIGHT_REG(base, 0xDF4)
#define CORESIGHT_MEM_AP_BASE(base)	CORESIGHT_REG(base, 0xDF8)
#define CORESIGHT_MEM_AP_IDR(base)	CORESIGHT_REG(base, 0xDFC)

/* MEM_AP BASE Register
 * 32-bit: BASEADDR[31:12] @ MEM_AP_BASE
 * 64-bit: BASEADDR[31:12] @ MEM_AP_BASE
 *       & BASEADDR[63:32] @ MEM_AP_BASE1 */
#define MEM_AP_BASEADDR_OFFSET		12
#define MEM_AP_BASEADDR_MASK		REG_20BIT_MASK
#define MEM_AP_BASEADDR			_GET_FV(MEM_AP_BASEADDR_OFFSET, value)

/* MEM_AP CFG Register */
#define MEM_AP_CFG_BE			_BV(0)
#define MEM_AP_CFG_LA			_BV(1)
#define MEM_AP_CFG_LD			_BV(2)
#define MEM_AP_CFG_RME			_BV(3)
#define MEM_AP_CFG_DARSIZE_OFFSET	4
#define MEM_AP_CFG_DARSIZE_MASK		REG_4BIT_MASK
#define MEM_AP_CFG_DARSIZE(value)	_GET_FV(MEM_AP_CFG_DARSIZE_OFFSET, value)
#define MEM_AP_CFG_TARINC_OFFSET	16
#define MEM_AP_CFG_TARINC_MASK		REG_4BIT_MASK
#define MEM_AP_CFG_TARINC(value)	_GET_FV(MEM_AP_CFG_TARINC_OFFSET, value)
#define MEM_AP_CFG_MECIDWIDTH_OFFSET	20
#define MEM_AP_CFG_MECIDWIDTH_MASK	REG_6BIT_MASK
#define MEM_AP_CFG_MECIDWIDTH(value)	_GET_FV(MEM_AP_CFG_MECIDWIDTH_OFFSET, value)

/* MEM_AP CFG1 Register */
#define MEM_AP_CFG1_TAG0SIZE_OFFSET	0
#define MEM_AP_CFG1_TAG0SIZE_MASK	REG_4BIT_MASK
#define MEM_AP_CFG1_TAG0SIZE(value)	_GET_FV(MEM_AP_CFG1_TAG0SIZE_OFFSET, value)
#define MEM_AP_CFG1_TAG0GRAN_OFFSET	4
#define MEM_AP_CFG1_TAG0GRAN_MASK	REG_5BIT_MASK
#define MEM_AP_CFG1_TAG0GRAN(value)	_GET_FV(MEM_AP_CFG1_TAG0GRAN_OFFSET, value)

/* MEM_AP CSW Register */
#define MEM_AP_CSW_SIZE_OFFSET		0
#define MEM_AP_CSW_SIZE_MASK		REG_3BIT_MASK
#define MEM_AP_CSW_SIZE(value)		_GET_FV(MEM_AP_CSW_SIZE_OFFSET, value)
#define MEM_AP_CSW_ADDRINC_OFFSET	4
#define MEM_AP_CSW_ADDRINC_MASK		REG_2BIT_MASK
#define MEM_AP_CSW_ADDRINC(value)	_GET_FV(MEM_AP_CSW_ADDRINC_OFFSET, value)
#define MEM_AP_CSW_DEVICEEN		_BV(6)
#define MEM_AP_CSW_TRINPROG		_BV(7)
#define MEM_AP_CSW_MODE_OFFSET		8
#define MEM_AP_CSW_MODE_MASK		REG_4BIT_MASK
#define MEM_AP_CSW_MODE(value)		_GET_FV(MEM_AP_CSW_MODE_OFFSET, value)
#define MEM_AP_CSW_TYPE_OFFSET		8
#define MEM_AP_CSW_TYPE_MASK		REG_3BIT_MASK
#define MEM_AP_CSW_TYPE(value)		_GET_FV(MEM_AP_CSW_TYPE_OFFSET, value)
#define MEM_AP_CSW_MTE			_BV(15)
#define MEM_AP_CSW_ERRNPASS		_BV(16)
#define MEM_AP_CSW_ERRSTOP		_BV(17)
#define MEM_AP_CSW_RMEEN_OFFSET		21
#define MEM_AP_CSW_RMEEN_MASK		REG_2BIT_MASK
#define MEM_AP_CSW_RMEEN(value)		_GET_FV(MEM_AP_CSW_RMEEN_OFFSET, value)
#define MEM_AP_CSW_SDEVICEEN		_BV(23)
#define MEM_AP_CSW_PROT_OFFSET		24
#define MEM_AP_CSW_PROT_MASK		REG_7BIT_MASK
#define MEM_AP_CSW_PROT(value)		_GET_FV(MEM_AP_CSW_PROT_OFFSET, value)
#define MEM_AP_CSW_DBGSWENABLE		_BV(31)

/* MEM_AP IDR Register */
#define MEM_AP_IDR_TYPE_OFFSET		0
#define MEM_AP_IDR_TYPE_MASK		REG_4BIT_MASK
#define MEM_AP_IDR_TYPE(value)		_GET_FV(MEM_AP_IDR_TYPE_OFFSET, value)
#define MEM_AP_IDR_VARIANT_OFFSET	4
#define MEM_AP_IDR_VARIANT_MASK		REG_4BIT_MASK
#define MEM_AP_IDR_VARIANT(value)	_GET_FV(MEM_AP_IDR_VARIANT_OFFSET, value)
#define MEM_AP_IDR_CLASS_OFFSET		13
#define MEM_AP_IDR_CLASS_MASK		REG_4BIT_MASK
#define MEM_AP_IDR_CLASS(value)		_GET_FV(MEM_AP_IDR_CLASS_OFFSET, value)
#define MEM_AP_IDR_DESIGNER_OFFSET	13
#define MEM_AP_IDR_DESIGNER_MASK	REG_11BIT_MASK
#define MEM_AP_IDR_DESIGNER(value)	_GET_FV(MEM_AP_IDR_DESIGNER_OFFSET, value)
#define MEM_AP_IDR_REVISION_OFFSET	28
#define MEM_AP_IDR_REVISION_MASK	REG_4BIT_MASK
#define MEM_AP_IDR_REVISION(value)	_GET_FV(MEM_AP_IDR_REVISION_OFFSET, value)

/* ======================================================================
 * CTI Registers
 * ====================================================================== */
#define CTI_CONTROL(base)		CORESIGHT_REG(base, 0x000)
#define CTI_INTACK(base)		CORESIGHT_REG(base, 0x010)
#define CTI_APPSET(base)		CORESIGHT_REG(base, 0x014)
#define CTI_APPCLEAR(base)		CORESIGHT_REG(base, 0x018)
#define CTI_APPPULSE(base)		CORESIGHT_REG(base, 0x01C)
#define CTI_INEN(base, n)		CORESIGHT_REG(base, 0x020 + (n << 2))
#define CTI_OUTEN(base, n)		CORESIGHT_REG(base, 0x0A0 + (n << 2))
#define CTI_TRIGINSTATUS(base)		CORESIGHT_REG(base, 0x130)
#define CTI_TRIGOUTSTATUS(base)		CORESIGHT_REG(base, 0x134)
#define CTI_CHINSTATUS(base)		CORESIGHT_REG(base, 0x138)
#define CTI_CHOUTSTATUS(base)		CORESIGHT_REG(base, 0x13C)
#define CTI_GATE(base)			CORESIGHT_REG(base, 0x140)
#define CTI_ASICCTRL(base)		CORESIGHT_REG(base, 0x144)
#define CTI_ITCHOUT(base)		CORESIGHT_REG(base, 0xEE4)
#define CTI_ITTRIGOUT(base)		CORESIGHT_REG(base, 0xEE8)
#define CTI_ITCHIN(base)		CORESIGHT_REG(base, 0xEF4)
#define CTI_ITTRIGIN(base)		CORESIGHT_REG(base, 0xEF8)

/* CTI_CONTROL */
#define CTI_EN				_BV(0)

/* CTI_APPSET */
#define APPSET_OFFSET			0
#define APPSET_MASK			REG_16BIT_MASK
#define APPSET(value)			_SET_FV(APPSET, value)

/* CTI_APPCLEAR */
#define APPCLEAR_OFFSET			0
#define APPCLEAR_MASK			REG_16BIT_MASK
#define APPCLEAR(value)			_SET_FV(APPCLEAR, value)

/* CTI_APPPULSE */
#define APPPULSE_OFFSET			0
#define APPPULSE_MASK			REG_16BIT_MASK
#define APPPULSE(value)			_SET_FV(APPPULSE, value)

/* CTI_INEN */
#define TRIGINEN_OFFSET			0
#define TRIGINEN_MASK			REG_16BIT_MASK
#define TRIGINEN(value)			_SET_FV(TRIGINEN, value)

/* CTI_OUTEN */
#define TRIGOUTEN_OFFSET		0
#define TRIGOUTEN_MASK			REG_16BIT_MASK
#define TRIGOUTEN(value)		_SET_FV(TRIGOUTEN, value)

/* CTI_CHINSTATUS */
#define CHINSTATUS_OFFSET		0
#define CHINSTATUS_MASK			REG_16BIT_MASK
#define CHINSTATUS(value)		_SET_FV(CHINSTATUS, value)

/* CTI_CHOUTSTATUS */
#define CHOUTSTATUS_OFFSET		0
#define CHOUTSTATUS_MASK		REG_16BIT_MASK
#define CHOUTSTATUS(value)		_SET_FV(CHOUTSTATUS, value)

/* CTI_GATE */
#define CTIGATE_OFFSET			0
#define CTIGATE_MASK			REG_16BIT_MASK
#define CTIGATE(value)			_SET_FV(CTIGATE, value)

/* CTI_ASICCTRL */
#define ASICCTRL_OFFSET			0
#define ASICCTRL_MASK			REG_8BIT_MASK
#define ASICCTRL(value)			_SET_FV(ASICCTRL, value)

/* ======================================================================
 * ETR Registers
 * ====================================================================== */
#define ETR_RSZ(base)			CORESIGHT_REG(base, 0x004)
#define ETR_STS(base)			CORESIGHT_REG(base, 0x00C)
#define ETR_RRD(base)			CORESIGHT_REG(base, 0x010)
#define ETR_RRP(base)			CORESIGHT_REG(base, 0x014)
#define ETR_RWP(base)			CORESIGHT_REG(base, 0x018)
#define ETR_TRG(base)			CORESIGHT_REG(base, 0x01C)
#define ETR_CTL(base)			CORESIGHT_REG(base, 0x020)
#define ETR_RWD(base)			CORESIGHT_REG(base, 0x024)
#define ETR_MODE(base)			CORESIGHT_REG(base, 0x028)
#define ETR_LBUFLEVEL(base)		CORESIGHT_REG(base, 0x02C)
#define ETR_CBUFLEVEL(base)		CORESIGHT_REG(base, 0x030)
#define ETR_BUFWM(base)			CORESIGHT_REG(base, 0x034)
#define ETR_RRPHI(base)			CORESIGHT_REG(base, 0x038)
#define ETR_RWPHI(base)			CORESIGHT_REG(base, 0x03C)
#define ETR_AXICTL(base)		CORESIGHT_REG(base, 0x110)
#define ETR_DBALO(base)			CORESIGHT_REG(base, 0x118)
#define ETR_DBAHI(base)			CORESIGHT_REG(base, 0x11C)
#define ETR_RURP(base)			CORESIGHT_REG(base, 0x120)
#define ETR_FFSR(base)			CORESIGHT_REG(base, 0x300)
#define ETR_FFCR(base)			CORESIGHT_REG(base, 0x304)
#define ETR_PSCR(base)			CORESIGHT_REG(base, 0x308)
#define ETR_AXICTL1(base)		CORESIGHT_REG(base, 0xED0)
#define ETR_ITEVTINTR(base)		CORESIGHT_REG(base, 0xEE0)
#define ETR_ITTRFLIN(base)		CORESIGHT_REG(base, 0xEE8)
#define ETR_ITATBDATA0(base)		CORESIGHT_REG(base, 0xEEC)
#define ETR_ITATBCTR2(base)		CORESIGHT_REG(base, 0xEF0)
#define ETR_ITATBCTR1(base)		CORESIGHT_REG(base, 0xEF4)
#define ETR_ITATBCTR0(base)		CORESIGHT_REG(base, 0xEF8)

/* ETR_STS */
#define ETR_STS_MEMERR			_BV(5)
#define ETR_STS_EMPTY			_BV(4)
#define ETR_STS_FTEMPTY			_BV(3)
#define ETR_STS_TMCREADY		_BV(2)
#define ETR_STS_TRIGGERED		_BV(1)
#define ETR_STS_FULL			_BV(0)

/* ETR MODE */
#define ETR_MODE_SOS			_BV(4)
#define ETR_MODE_CFG_OFFSET		0
#define ETR_MODE_CFG_MASK		REG_2BIT_MASK
#define ETR_MODE_CFG(value)		_SET_FV(ETR_MODE_CFG, value)
#define ETR_MODE_CB			0
#define ETR_MODE_SWF1			1
#define ETR_MODE_SWF2			3

/* ETR CTL */
#define ETR_CTL_TRACECAPTEN		_BV(0)

/* ETR AXICTL */
#define ETR_AXICTL_RCACHE_OFFSET	16
#define ETR_AXICTL_RCACHE_MASK		REG_4BIT_MASK
#define ETR_AXICTL_RCACHE(value)	_SET_FV(ETR_AXICTL_RCACHE, value)
#define ETR_AXICTL_WRBURSTLEN_OFFSET	8
#define ETR_AXICTL_WRBURSTLEN_MASK	REG_4BIT_MASK
#define ETR_AXICTL_WRBURSTLEN(value)	_SET_FV(ETR_AXICTL_WRBURSTLEN, value)
#define ETR_AXICTL_WCACHE_OFFSET	2
#define ETR_AXICTL_WCACHE_MASK		REG_4BIT_MASK
#define ETR_AXICTL_WCACHE(value)	_SET_FV(ETR_AXICTL_WCACHE, value)
#define ETR_AXICTL_PROTCTRLBIT1		_BV(1)
#define ETR_AXICTL_PROTCTRLBIT0		_BV(0)

/* ETR FFSR */
#define ETR_FFSR_FTSTOPPED		_BV(1)
#define ETR_FFSR_FLINPROG		_BV(0)

/* ETR FFCR */
#define ETR_FFCR_EMBEDFLUSH		_BV(15)
#define ETR_FFCR_STOPONTRIGEVT		_BV(13)
#define ETR_FFCR_STOPONF1		_BV(12)
#define ETR_FFCR_TRIGONF1		_BV(10)
#define ETR_FFCR_TRIGONTRIGEVT		_BV(9)
#define ETR_FFCR_TRIGONTRIGIN		_BV(8)
#define ETR_FFCR_FLUSHMAN		_BV(6)
#define ETR_FFCR_FONTRIGEVENT		_BV(5)
#define ETR_FFCR_FONFLIN		_BV(4)
#define ETR_FFCR_ENTL			_BV(1)
#define ETR_FFCR_ENFT			_BV(0)

/* ETR PSCR */
#define ETR_PSCR_EMBEDSYNC		_BV(5)
#define ETR_PSCR_PSCOUNT_OFFSET		0
#define ETR_PSCR_PSCOUNT_MASK		REG_5BIT_MASK
#define ETR_PSCR_PSCOUNT(value)		_SET_FV(ETR_PSCR_PSCOUNT, value)

/* ETR AXICTL1 */
#define ETR_AXICTL1_TLBXLATESLOTS_OFFSET	16
#define ETR_AXICTL1_TLBXLATESLOTS_MASK		REG_12BIT_MASK
#define ETR_AXICTL1_TLBXLATESLOTS(value)	_SET_FV(ETR_AXICTL1_TLBXLATESLOTS, value)
#define ETR_AXICTL1_TLBLIMIT_OFFSET		0
#define ETR_AXICTL1_TLBLIMIT_MASK		REG_5BIT_MASK
#define ETR_AXICTL1_TLBLIMIT(value)		_SET_FV(ETR_AXICTL1_TLBLIMIT, value)

/* ETR ITEVINTR */
#define ETR_ITEVINTR_BUFINTR		_BV(3)
#define ETR_ITEVINTR_FLUSHCOMP		_BV(2)
#define ETR_ITEVINTR_FULL		_BV(1)
#define ETR_ITEVINTR_ACQCOMP		_BV(0)

/* ETR ITTRFLIN */
#define ETR_ITTRFLIN_FLUSHIN		_BV(1)
#define ETR_ITTRFLIN_TRIGIN		_BV(0)

/* ETR ITATBDATA0 */
#define ETR_ATDATASBIT127		_BV(16)
#define ETR_ATDATASBIT119		_BV(15)
#define ETR_ATDATASBIT111		_BV(14)
#define ETR_ATDATASBIT103		_BV(13)
#define ETR_ATDATASBIT95		_BV(12)
#define ETR_ATDATASBIT87		_BV(11)
#define ETR_ATDATASBIT79		_BV(10)
#define ETR_ATDATASBIT71		_BV(9)
#define ETR_ATDATASBIT63		_BV(8)
#define ETR_ATDATASBIT55		_BV(7)
#define ETR_ATDATASBIT47		_BV(6)
#define ETR_ATDATASBIT39		_BV(5)
#define ETR_ATDATASBIT31		_BV(4)
#define ETR_ATDATASBIT23		_BV(3)
#define ETR_ATDATASBIT15		_BV(2)
#define ETR_ATDATASBIT7			_BV(1)
#define ETR_ATDATASBIT0			_BV(0)

/* ETR ITATBCTR2 */
#define ETR_ITATBCTR2_SYNCREQS		_BV(2)
#define ETR_ITATBCTR2_AFVALIDS		_BV(1)
#define ETR_ITATBCTR2_ATREADYS		_BV(0)

/* ETR ITATBCTR1 */
#define ETR_ITATBCTR1_ATIDS_OFFSET	0
#define ETR_ITATBCTR1_ATIDS_MASK	REG_7BIT_MASK
#define ETR_ITATBCTR1_ATIDS(value)	_SET_FV(ETR_ITATBCTR1_ATIDS, value)

/* ETR ITATBCTR0 */
#define ETR_ITATBCTR0_ATBYTESS_OFFSET	8
#define ETR_ITATBCTR0_ATBYTESS_MASK	REG_4BIT_MASK
#define ETR_ITATBCTR0_ATBYTESS(value)	_SET_FV(ETR_ITATBCTR0_ATBYTESS, value)
#define ETR_ITATBCTR0_ATWAKEUPS		_BV(2)
#define ETR_ITATBCTR0_AFREADYS		_BV(1)
#define ETR_ITATBCTR0_ATVALIDS		_BV(0)

/* ======================================================================
 * Interfaces
 * ====================================================================== */

struct coresight_rom_table {
	caddr_t base;
	uint8_t clazz;

	uint16_t jep106_ident;
	uint16_t part;
};

struct coresight_rom_device {
	uintptr_t base;

	uint16_t jep106_ident;
	uint16_t arch_id;
	uint16_t part_no;
	uint8_t revision;
};

typedef int (*coresight_table_handler)(struct coresight_rom_table *table);
typedef int (*coresight_device_handler)(struct coresight_rom_device *device);

struct coresight_table {
	const char *name;
	uint8_t clazz;
	coresight_table_handler handler;
};

#define CORESIGHT_FLAGS_ENUMERATOR	0x01

struct coresight_device {
	const char *name;
	uint8_t flags;
	uint16_t jep106_ident;
	uint16_t arch_id;
	uint16_t part_no;
	int nr_devs;
	coresight_device_handler handler;
};

#define MAX_CORESIGHT_TABLES	CORESIGHT_HW_MAX_TABLES
#define MAX_CORESIGHT_DEVICES	CORESIGHT_HW_MAX_DEVICES

#ifdef CONFIG_CORESIGHT_VERBOSE
#define coresight_log(...)	printf(__VA_ARGS__)
#else
#define coresight_log(...)
#endif

#ifdef CONFIG_CORESIGHT
int coresight_init(caddr_t *rom_table_base, uint32_t rom_table_num, caddr_t *blacklist);
#else
#define coresight_init(*rom_table_base, rom_table_num, blacklist)	do { } while (0)
#endif
int coresight_register_table(struct coresight_table *table);
int coresight_register_device(struct coresight_device *device);
int coresight_visit_table(uintptr_t rom_table_base);
int coresight_visit_device(uintptr_t rom_table_base);
int __coresight_visit_device(struct coresight_rom_device *device);

int coresight_mem_ap_init(void);
uint32_t mem_ap_read(uint32_t address);
void mem_ap_write(uint32_t data, uint32_t addr);

#ifdef CONFIG_CORESIGHT_MEM_AP
#define coresight_read(addr)				mem_ap_read(addr)
#define coresight_write(data, addr)			mem_ap_write(data, addt)
#else
#define coresight_read(addr)				__raw_readl(addr)
#define coresight_write(data, addr)			__raw_writel(data, addr)
#endif

#ifdef CONFIG_CORESIGHT_ETM
int coresight_etm_init(void);
#else
static inline int coresight_etm_init(void)
{
	return 0;
}
#endif
#ifdef CONFIG_CORESIGHT_STM
int coresight_stm_init(void);
#else
static inline int coresight_stm_init(void)
{
	return 0;
}
#endif
#ifdef CONFIG_CORESIGHT_PMU
int coresight_pmu_init(void);
#else
static inline int coresight_pmu_init(void)
{
	return 0;
}
#endif
#ifdef CONFIG_CORESIGHT_CTI
int coresight_cti_init(void);
#else
static inline int coresight_cti_init(void)
{
	return 0;
}
#endif
#ifdef CONFIG_CORESIGHT_DBG_CPUv80A
int coresight_dbg_cpuv80a_init(void);
#else
static inline int coresight_dbg_cpuv80a_init(void)
{
	return 0;
}
#endif
#ifdef CONFIG_CORESIGHT_TPIU
int coresight_tpiu_init(void);
#else
static inline int coresight_tpiu_init(void)
{
	return 0;
}
#endif
#ifdef CONFIG_CORESIGHT_ETB
int coresight_etb_init(void);
#else
static inline int coresight_etb_init(void)
{
	return 0;
}
#endif
#ifdef CONFIG_CORESIGHT_ETR
int coresight_etr_init(void);
#else
static inline int coresight_etr_init(void)
{
	return 0;
}
#endif
#ifdef CONFIG_CORESIGHT_FUNNEL
int coresight_funnel_init(void);
#else
static inline int coresight_funnel_init(void)
{
	return 0;
}
#endif
#ifdef CONFIG_CORESIGHT_REPLICATOR
int coresight_replicator_init(void);
#else
static inline int coresight_replicator_init(void)
{
	return 0;
}
#endif
#ifdef CONFIG_CORESIGHT_TMC
int coresight_tmc_init(void);
#else
static inline int coresight_tmc_init(void)
{
	return 0;
}
#endif

#endif /*__CORESIGHT_ARM64_H_INCLUDE__ */
