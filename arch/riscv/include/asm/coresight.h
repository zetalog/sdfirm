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
 * Interfaces
 * ====================================================================== */

struct coresight_rom_table {
	uintptr_t base;
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

int coresight_init(uintptr_t rom_table_base, uintptr_t *blacklists);
int coresight_register_table(struct coresight_table *table);
int coresight_register_device(struct coresight_device *device);
int coresight_visit_table(uintptr_t rom_table_base);
int coresight_visit_device(uintptr_t rom_table_base);
int __coresight_visit_device(struct coresight_rom_device *device);

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
