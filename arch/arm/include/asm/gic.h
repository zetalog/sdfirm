#ifndef __GIC_ARM_H_INCLUDE__
#define __GIC_ARM_H_INCLUDE__

#include <target/config.h>
#include <target/generic.h>
#include <asm/reg.h>

/* ARM Generic Interrupt Controller Architecture Specification v2 */

/* base address and bit offset generator for bits-registers */
#define GIC_1BIT_MASK			0x001
#define GIC_2BIT_MASK			0x003
#define GIC_3BIT_MASK			0x007
#define GIC_4BIT_MASK			0x00F
#define GIC_5BIT_MASK			0x01F
#define GIC_6BIT_MASK			0x03F
#define GIC_7BIT_MASK			0x07F
#define GIC_8BIT_MASK			0x0FF
#define GIC_9BIT_MASK			0x1FF
#define GIC_10BIT_MASK			0x3FF
#define GIC_11BIT_MASK			0x7FF
#define GIC_12BIT_MASK			0xFFF

#define GIC_1BIT_REG(base, n)		((base)+(((n) & (-31)) >> 3))
#define GIC_2BIT_REG(base, n)		((base)+(((n) & (-15)) >> 2))
#define GIC_4BIT_REG(base, n)		((base)+(((n) & (-7 )) >> 1))
#define GIC_8BIT_REG(base, n)		((base)+(((n) & (-3 )) >> 0))

#define GIC_1BIT_OFFSET(n)		(((n) & 31) << 0)
#define GIC_2BIT_OFFSET(n)		(((n) & 15) << 1)
#define GIC_4BIT_OFFSET(n)		(((n) &  7) << 2)
#define GIC_8BIT_OFFSET(n)		(((n) &  3) << 3)

#define GIC_FV(name, value)		\
	(((value) & (name##_MASK)) << (name##_OFFSET))
#define GICn_FV(n, name, value)		\
	(((value) & (name##_MASK)) << (name##_OFFSET(n)))

/* Distributor register map */
#define GICD_REG(offset)		(GICD_BASE+(offset))
#define GICD_FV(name, value)		GIC_FV(GICD_##name, value)
#define GICDn_FV(n, name, value)	GICn_FV(n, GICD_##name, value)

#define GICD_CTLR			GICD_REG(0x000)
#define GICD_TYPER			GICD_REG(0x004)
#define GICD_IIDR			GICD_REG(0x004)

#define GICD_IGROUPR(n)			GIC_1BIT_REG(0x080, n)
#define GICD_ISENABLER(n)		GIC_1BIT_REG(0x100, n)
#define GICD_ICENABLER(n)		GIC_1BIT_REG(0x180, n)
#define GICD_ISPENDR(n)			GIC_1BIT_REG(0x200, n)
#define GICD_ICPENDR(n)			GIC_1BIT_REG(0x280, n)
#define GICD_ISACTIVER(n)		GIC_1BIT_REG(0x300, n)
#define GICD_ICACTIVER(n)		GIC_1BIT_REG(0x380, n)
#define GICD_IPRIORITYR(n)		GIC_8BIT_REG(0x400, n)
#define GICD_ITARGETSR(n)		GIC_8BIT_REG(0x800, n)
#define GICD_ICFGR(n)			GIC_2BIT_REG(0xC00, n)
#define GICD_NSACR(n)			GIC_2BIT_REG(0xE00, n)

#define GICD_SGIR			GICD_REG(0xF00)

#define GICD_CPENDSGIR(n)		GIC_8BIT_REG(0xF10, n)
#define GICD_SPENDSGIR(n)		GIC_8BIT_REG(0xF20, n)

#define GICD_ICPIDR2			GICD_REG(0xFE8) /* Peripheral ID2 */

/* GICD_CTLR */
#define GICD_ENABLE_GRP0		_BV(0)
#define GICD_ENABLE_GRP1		_BV(1)

/* GICD_TYPER */
#define GICD_IT_LINES_NUMBER_OFFSET	0
#define GICD_IT_LINES_NUMBER_MASK	GIC_5BIT_MASK
#define GICD_IT_LINES_NUMBER(value)	GICD_FV(IT_LINES_NUMBER, value)
#define GICD_CPU_NUMBER_OFFSET		5
#define GICD_CPU_NUMBER_MASK		GIC_3BIT_MASK
#define GICD_CPU_NUMBER(value)		GICD_FV(CPU_NUMBER, value)
#define GICD_SECURITY_EXTN		_BV(10)
#define GICD_LSPI_OFFSET		11
#define GICD_LSPI_MASK			GIC_5BIT_MASK
#define GICD_LSPI(value)		GICD_FV(LSPI, value)

/* GICD_IIDR */
#define GICD_IMPLEMENTER_OFFSET		0
#define GICD_IMPLEMENTER_MASK		GIC_12BIT_MASK
#define GICD_IMPLEMENTER(value)		GICD_FV(IMPLEMENTER, value)
#define GICD_REVISION_OFFSET		12
#define GICD_REVISION_MASK		GIC_4BIT_MASK
#define GICD_REVISION(value)		GICD_FV(REVISION, value)
#define GICD_VARIANT_OFFSET		16
#define GICD_VARIANT_MASK		GIC_4BIT_MASK
#define GICD_VARIANT(value)		GICD_FV(VARIANT, value)
#define GICD_PRODUCT_ID_OFFSET		24
#define GICD_PRODUCT_ID_MASK		GIC_8BIT_MASK
#define GICD_PRODUCT_ID(value)		GICD_FV(PRODUCT_ID, value)

/* GICD_IGROUPR */
#define GICD_GROUP_STATUS(n)		_BV(GIC_1BIT_OFFSET(n))

/* GICD_ISENABLER/GICD_ICENABLER */
#define GICD_ENABLE(n)			_BV(GIC_1BIT_OFFSET(n))

/* GICD_ISPENDR/GICD_ICPENDR */
#define GICD_PENDING(n)			_BV(GIC_1BIT_OFFSET(n))

/* GICD_ISACTIVER/GICD_ICACTIVER */
#define GICD_ACTIVE(n)			_BV(GIC_1BIT_OFFSET(n))

/* GICD_IPRORITYR */
#define GICD_PRIORITY_OFFSET(n)		GIC_8BIT_OFFSET(n)
#define GICD_PRIORITY_MASK		GIC_8BIT_MASK
#define GICD_PRIORITY(n, value)		GICDn_FV(n, PRIORITY, value)

/* GICD_ITARGETSR */
#define GICD_CPU_TARGETS_OFFSET(n)	GIC_8BIT_OFFSET(n)
#define GICD_CPU_TARGETS_MASK		GIC_8BIT_MASK
#define GICD_CPU_TARGETS(n, value)	GICDn_FV(n, CPU_TARGETS, value)
#define GICD_CPU_TARGETS_MAX		0xFF

/* GICD_ICFGR */
#define GICD_INT_CONFIG_OFFSET(n)	GIC_2BIT_OFFSET(n)
#define GICD_INT_CONFIG_MASK		GIC_2BIT_MASK
#define GICD_INT_CONFIG(n, value)	GICDn_FV(n, INT_CONFIG, value)
#define GICD_TRIGGER(value)		((value & 1) << 0)
#define GICD_TRIGGER_LEVEL		0
#define GICD_TRIGGER_EDGE		1
#define GICD_MODEL(value)		((value & 1) << 1)
#define GICD_MODEL_N_N			0
#define GICD_MODEL_1_N			1

/* GICD_NSACR */
#define GICD_NS_ACCESS_OFFSET(n)	GIC_2BIT_OFFSET(n)
#define GICD_NS_ACCESS_MASK		GIC_2BIT_MASK
#define GICD_NS_ACCESS(n, value)	GICDn_FV(n, NS_ACCESS, value)
#define GICD_NSAC_ALL			0
#define GICD_NSAC_WSPEND_WSGI		1
#define GICD_NSAC_WCPEND_RXACTIVE	2
#define GICD_NSAC_RWTARGETS		3

/* GICD_SGIR */
#define GICD_SGIINTID_OFFSET		0
#define GICD_SGIINTID_MASK		GIC_4BIT_MASK
#define GICD_SGIINTID(value)		GICD_FV(SGIINTID, value)
#define GICD_NSATT			_BV(15)
#define GICD_CPU_TARGET_LIST_OFFSET	16
#define GICD_CPU_TARGET_LIST_MASK	GIC_8BIT_MASK
#define GICD_CPU_TARGET_LIST(value)	GICD_FV(CPU_TARGET_LIST, value)
#define GICD_TARGET_LIST_FILTER_OFFSET	24
#define GICD_TARGET_LIST_FILTER_MASK	GIC_2BIT_MASK
#define GICD_TARGET_LIST_FILTER(value)	GICD_FV(TARGET_LIST_FILTER, value)

/* GICD_CPENDSGIR/GICD_SPENDSGIR */
#define GICD_SGI_PENDING_OFFSET(n)	GIC_8BIT_OFFSET(n)
#define GICD_SGI_PENDING_MASK		GIC_8BIT_MASK
#define GICD_SGI_PENDING(n, value)	GICDn_FV(n, SGI_PENDING, value)

/* GICD_ICPIDR2 */
#define GICD_ARCH_REV_OFFSET		4
#define GICD_ARCH_REV_MASK		GIC_4BIT_MASK
#define GICD_ARCH_REV(value)		GICD_FV(ARCH_REV, value)

/* CPU interface register map */
#define GICC_OFFSET(offset)		(GICC_BASE+(offset))
#define GICC_FV(name, value)		GIC_FV(GICC_##name, value)
#define GICCn_FV(n, name, value)	GICn_FV(n, GICC_##name, value)

#define GICC_CTLR			GICC_OFFSET(0x000)
#define GICC_PMR			GICC_OFFSET(0x004)
#define GICC_BPR			GICC_OFFSET(0x008)
#define GICC_IAR			GICC_OFFSET(0x00C)
#define GICC_EOIR			GICC_OFFSET(0x010)
#define GICC_RPR			GICC_OFFSET(0x014)
#define GICC_HPPIR			GICC_OFFSET(0x018)
#define GICC_ABPR			GICC_OFFSET(0x01C)
#define GICC_AIAR			GICC_OFFSET(0x020)
#define GICC_AEOIR			GICC_OFFSET(0x024)
#define GICC_AHPPIR			GICC_OFFSET(0x028)
#define GICC_APR(n)			GIC_1BIT_REG(0x0D0, n)
#define GICC_NSAPR(n)			GIC_1BIT_REG(0x0E0, n)
#define GICC_IIDR			GICC_OFFSET(0x0FC)
#define GICC_DIR			GICC_OFFSET(0x100)

/* GICC_CTLR */
#define GICC_ENABLE			0

/* GICC_PMR/GICC_RPR */
#define GICC_PRIORITY_OFFSET		0
#define GICC_PRIORITY_MASK		GIC_8BIT_MASK
#define GICC_PRIORITY(value)		GICC_FV(PRIORITY, value)

/* GICC_BPR/GICC_ABPR */
#define GICC_BINARY_POINT_OFFSET	0
#define GICC_BINARY_POINT_MASK		GIC_3BIT_MASK
#define GICC_BINARY_POINT(value)	GICC_FV(BINARY_POINT, value)

/* GICC_IAR/GICC_EOIR/GICC_HPPIR/GICC_AIAR/GICC_AEOIR/GICC_AHPIR/GICC_DIR */
#define GICC_INTERRUPT_ID_OFFSET	0
#define GICC_INTERRUPT_ID_MASK		GIC_10BIT_MASK
#define GICC_INTERRUPT_ID(value)	GICC_FV(INTERRUPT_ID, value)
#define GICC_CPUID_OFFSET		10
#define GICC_CPUID_MASK			GIC_3BIT_MASK
#define GICC_CPUID(value)		GICC_FV(CPUID, value)

/* GICC_APR/GICC_NSAPR */
#define GICC_ACTIVE_PRIORITY_OFFSET(n)	GIC_1BIT_OFFSET(n)
#define GICC_ACTIVE_PRIORITY_MASK	GIC_1BIT_MASK
#define GICC_ACTIVE_PRIORITY(n, value)	GICCn_FV(n, ACTIVE_PRIORITY, value)

/* GICC_IIDR */
#define GICC_IMPLEMENTER_OFFSET		0
#define GICC_IMPLEMENTER_MASK		GIC_12BIT_MASK
#define GICC_IMPLEMENTER(value)		GICC_FV(IMPLEMENTER, value)
#define GICC_REVISION_OFFSET		12
#define GICC_REVISION_MASK		GIC_4BIT_MASK
#define GICC_REVISION(value)		GICC_FV(REVISION, value)
#define GICC_ARCH_REV_OFFSET		16
#define GICC_ARCH_REV_MASK		GIC_4BIT_MASK
#define GICC_ARCH_REV(value)		GICC_FV(ARCH_REV, value)
#define GICC_PRODUCT_ID_OFFSET		20
#define GICC_PRODUCT_ID_MASK		GIC_12BIT_MASK
#define GICC_PRODUCT_ID(value)		GICC_FV(PRODUCT_ID, value)

/* Generic values */
#define GIC_PRIORITY_MAX		0xFF

#endif /* __GIC_ARM_H_INCLUDE__ */
