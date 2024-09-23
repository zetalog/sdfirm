#ifndef __SMMU_PMCG_H_INCLUDE__
#define __SMMU_PMCG_H_INCLUDE__

#include <target/iommu.h>

#ifdef CONFIG_64BIT
#define PMCG_STRIDE			8
#define __pmcg_read(addr)		__raw_readq(addr)
#define __pmcg_write(v, addr)		__raw_writeq(v, addr)
#else /* CONFIG_64BIT */
#define PMCG_STRIDE			4
#define __pmcg_read(addr)		__raw_readl(addr)
#define __pmcg_write(v, addr)		__raw_writel(v, addr)
#endif /* CONFIG_64BIT */

#define SMMU_PMCG_BASE			0x2000

#define TCU_PMCG_PAGE0_REG(smmu, offset)	TCU_PAGE0_REG(smmu, SMMU_PMCG_BASE + (offset))
#define TCU_PMCG_PAGE1_REG(smmu, offset)	TCU_PAGE2_REG(smmu, SMMU_PMCG_BASE + (offset))
#define TBU_PMCG_PAGE0_REG(smmu, tbu, offset)	TBU_PAGE0_REG(smmu, tbu, SMMU_PMCG_BASE + (offset))
#define TBU_PMCG_PAGE1_REG(smmu, tbu, offset)	TBU_PAGE1_REG(smmu, tbu, SMMU_PMCG_BASE + (offset))

#define TCU_PMCG_EVCNTR(smmu, n)	(TCU_PMCG_PAGE0_REG(smmu, 0x0 + SMMU_PMCG_RELOC_BASE(smmu)) + ((n) * PMCG_STRIDE))
#define TCU_PMCG_EVTYPER(smmu, n)	(TCU_PMCG_PAGE0_REG(smmu, 0x400) + ((n) * 4))
#define TCU_PMCG_SVR(smmu, n)		(TCU_PMCG_PAGE0_REG(smmu, 0x600 + SMMU_PMCG_RELOC_BASE(smmu)) + ((n) * PMCG_STRIDE))
#define TCU_PMCG_SMR(smmu, n)		(TCU_PMCG_PAGE0_REG(smmu, 0xA00) + ((n) * 4))
#define TCU_PMCG_CNTENSET0(smmu)	TCU_PMCG_PAGE0_REG(smmu, 0xC00)
#define TCU_PMCG_CNTENCLR0(smmu)	TCU_PMCG_PAGE0_REG(smmu, 0xC20)
#define TCU_PMCG_INTENSET0(smmu)	TCU_PMCG_PAGE0_REG(smmu, 0xC40)
#define TCU_PMCG_INTENCLR0(smmu)	TCU_PMCG_PAGE0_REG(smmu, 0xC60)
#define TCU_PMCG_OVSCLR0(smmu)		TCU_PMCG_PAGE0_REG(smmu, 0xC80 + SMMU_PMCG_RELOC_BASE(smmu))
#define TCU_PMCG_OVSSET0(smmu)		TCU_PMCG_PAGE0_REG(smmu, 0xCC0 + SMMU_PMCG_RELOC_BASE(smmu))
#define TCU_PMCG_CAPR(smmu)		TCU_PMCG_PAGE0_REG(smmu, 0xD88i + SMMU_PMCG_RELOC_BASE(smmu))
#define TCU_PMCG_SCR(smmu)		TCU_PMCG_PAGE0_REG(smmu, 0xDF8)
#define TCU_PMCG_CFGR(smmu)		TCU_PMCG_PAGE0_REG(smmu, 0xE00)
#define TCU_PMCG_CR(smmu)		TCU_PMCG_PAGE0_REG(smmu, 0xE04)
#define TCU_PMCG_CEID(smmu, n)		(TCU_PMCG_PAGE0_REG(smmu, 0xE20) + (REG64_1BIT_INDEX(n) << 3))
#define TCU_PMCG_IRQ_CTRL(smmu)		TCU_PMCG_PAGE0_REG(smmu, 0xE50)
#define TCU_PMCG_IRQ_CTRLACK(smmu)	TCU_PMCG_PAGE0_REG(smmu, 0xE54)
#ifdef CONFIG_SMMU_PMCG_MSI
#define TCU_PMCG_IRQ_CFG0(smmu)		TCU_PMCG_PAGE0_REG(smmu, 0xE58)
#define TCU_PMCG_IRQ_CFG1(smmu)		TCU_PMCG_PAGE0_REG(smmu, 0xE60)
#define TCU_PMCG_IRQ_CFG2(smmu)		TCU_PMCG_PAGE0_REG(smmu, 0xE64)
#define TCU_PMCG_IRQ_STATUS(smmu)	TCU_PMCG_PAGE0_REG(smmu, 0xE68)
#endif
#define TCU_PMCG_AIDR(smmu)		TCU_PMCG_PAGE0_REG(smmu, 0xE70)
#define TCU_PMCG_CIDR0(smmu)		TCU_PMCG_PAGE0_REG(smmu, 0xFF0)
#define TCU_PMCG_CIDR1(smmu)		TCU_PMCG_PAGE0_REG(smmu, 0xFF4)
#define TCU_PMCG_CIDR2(smmu)		TCU_PMCG_PAGE0_REG(smmu, 0xFF8)
#define TCU_PMCG_CIDR3(smmu)		TCU_PMCG_PAGE0_REG(smmu, 0xFFC)
#define TCU_PMCG_PIDR0(smmu)		TCU_PMCG_PAGE0_REG(smmu, 0xFE0)
#define TCU_PMCG_PIDR1(smmu)		TCU_PMCG_PAGE0_REG(smmu, 0xFE4)
#define TCU_PMCG_PIDR2(smmu)		TCU_PMCG_PAGE0_REG(smmu, 0xFE8)
#define TCU_PMCG_PIDR3(smmu)		TCU_PMCG_PAGE0_REG(smmu, 0xFEC)
#define TCU_PMCG_PIDR4(smmu)		TCU_PMCG_PAGE0_REG(smmu, 0xFD0)
#define TCU_PMCG_PIDR5(smmu)		TCU_PMCG_PAGE0_REG(smmu, 0xFD4)
#define TCU_PMCG_PIDR6(smmu)		TCU_PMCG_PAGE0_REG(smmu, 0xFD8)
#define TCU_PMCG_PIDR7(smmu)		TCU_PMCG_PAGE0_REG(smmu, 0xFDC)
#define TCU_PMCG_PMAUTHSTATUS(smmu)	TCU_PMCG_PAGE0_REG(smmu, 0xFB8)
#define TCU_PMCG_PMDARCH(smmu)		TCU_PMCG_PAGE0_REG(smmu, 0xFBC)
#define TCU_PMCG_PMDTYPE(smmu)		TCU_PMCG_PAGE0_REG(smmu, 0xFCC)

#define TBU_PMCG_EVCNTR(smmu, tbu, n)	(TBU_PMCG_PAGE0_REG(smmu, tbu, 0x0)+ ((n) * PMCG_STRIDE))
#define TBU_PMCG_EVTYPER(smmu, tbu, n)	(TBU_PMCG_PAGE0_REG(smmu, tbu, 0x400)+ ((n) * 4))
#define TBU_PMCG_SVR(smmu, tbu, n)	(TBU_PMCG_PAGE0_REG(smmu, tbu, 0x600)+ ((n) * PMCG_STRIDE))
#define TBU_PMCG_SMR(smmu, tbu, n)	(TBU_PMCG_PAGE0_REG(smmu, tbu, 0xA00)+ ((n) * 4))
#define TBU_PMCG_CNTENSET0(smmu, tbu)	TBU_PMCG_PAGE0_REG(smmu, tbu, 0xC00)
#define TBU_PMCG_CNTENCLR0(smmu, tbu)	TBU_PMCG_PAGE0_REG(smmu, tbu, 0xC20)
#define TBU_PMCG_INTENSET0(smmu, tbu)	TBU_PMCG_PAGE0_REG(smmu, tbu, 0xC40)
#define TBU_PMCG_INTENCLR0(smmu, tbu)	TBU_PMCG_PAGE0_REG(smmu, tbu, 0xC60)
#define TBU_PMCG_OVSCLR0(smmu, tbu, n)	TBU_PMCG_PAGE0_REG(smmu, tbu, 0xC80)
#define TBU_PMCG_OVSSET0(smmu, tbu, n)	TBU_PMCG_PAGE0_REG(smmu, tbu, 0xCC0)
#define TBU_PMCG_CAPR(smmu, tbu)	TBU_PMCG_PAGE0_REG(smmu, tbu, 0xD88)
#define TBU_PMCG_SCR(smmu, tbu)		TBU_PMCG_PAGE0_REG(smmu, tbu, 0xDF8)
#define TBU_PMCG_CFGR(smmu, tbu)	TBU_PMCG_PAGE0_REG(smmu, tbu, 0xE00)
#define TBU_PMCG_CR(smmu, tbu)		TBU_PMCG_PAGE0_REG(smmu, tbu, 0xE04)
#define TBU_PMCG_CEID(smmu, tbu, n)	(TBU_PMCG_PAGE0_REG(smmu, tbu, 0xE20) + (REG64_1BIT_INDEX(n) << 3))
#define TBU_PMCG_IRQ_CTRL(smmu)		TBU_PMCG_PAGE0_REG(smmu, tbu, 0xE50)
#define TBU_PMCG_IRQ_CTRLACK(smmu)	TBU_PMCG_PAGE0_REG(smmu, tbu, 0xE54)
#ifdef CONFIG_SMMU_PMCG_MSI
#define TBU_PMCG_IRQ_CFG0(smmu, tbu)	TBU_PMCG_PAGE0_REG(smmu, tbu, 0xE58)
#define TBU_PMCG_IRQ_CFG1(smmu, tbu)	TBU_PMCG_PAGE0_REG(smmu, tbu, 0xE60)
#define TBU_PMCG_IRQ_CFG2(smmu, tbu)	TBU_PMCG_PAGE0_REG(smmu, tbu, 0xE64)
#define TBU_PMCG_IRQ_STATUS(smmu, tbu)	TBU_PMCG_PAGE0_REG(smmu, tbu, 0xE68)
#endif
#define TBU_PMCG_AIDR(smmu, tbu)	TBU_PMCG_PAGE0_REG(smmu, tbu, 0xE70)
#define TBU_PMCG_CIDR0(smmu, tbu)	TBU_PMCG_PAGE0_REG(smmu, tbu, 0xFF0)
#define TBU_PMCG_CIDR1(smmu, tbu)	TBU_PMCG_PAGE0_REG(smmu, tbu, 0xFF4)
#define TBU_PMCG_CIDR2(smmu, tbu)	TBU_PMCG_PAGE0_REG(smmu, tbu, 0xFF8)
#define TBU_PMCG_CIDR3(smmu, tbu)	TBU_PMCG_PAGE0_REG(smmu, tbu, 0xFFC)
#define TBU_PMCG_PIDR0(smmu, tbu)	TBU_PMCG_PAGE0_REG(smmu, tbu, 0xFE0)
#define TBU_PMCG_PIDR1(smmu, tbu)	TBU_PMCG_PAGE0_REG(smmu, tbu, 0xFE4)
#define TBU_PMCG_PIDR2(smmu, tbu)	TBU_PMCG_PAGE0_REG(smmu, tbu, 0xFE8)
#define TBU_PMCG_PIDR3(smmu, tbu)	TBU_PMCG_PAGE0_REG(smmu, tbu, 0xFEC)
#define TBU_PMCG_PIDR4(smmu, tbu)	TBU_PMCG_PAGE0_REG(smmu, tbu, 0xFD0)
#define TBU_PMCG_PIDR5(smmu, tbu)	TBU_PMCG_PAGE0_REG(smmu, tbu, 0xFD4)
#define TBU_PMCG_PIDR6(smmu, tbu)	TBU_PMCG_PAGE0_REG(smmu, tbu, 0xFD8)
#define TBU_PMCG_PIDR7(smmu, tbu)	TBU_PMCG_PAGE0_REG(smmu, tbu, 0xFDC)
#define TBU_PMCG_PMAUTHSTATUS(smmu,tbu)	TBU_PMCG_PAGE0_REG(smmu, tbu, 0xFB8)
#define TBU_PMCG_PMDARCH(smmu, tbu)	TBU_PMCG_PAGE0_REG(smmu, tbu, 0xFBC)
#define TBU_PMCG_PMDTYPE(smmu, tbu)	TBU_PMCG_PAGE0_REG(smmu, tbu, 0xFCC)

/* SMMU_PMCG_CEID */
#define PMCG_CEID(eid)			_BV(REG64_1BIT_OFFSET(eid))
#define PMCG_CNT(c)			_BV(REG64_1BIT_OFFSET(c))

#define SMMU_PMCG_SID_SPAN_SHIFT        29
#define SMMU_PMCG_CFGR_SID_FILTER_TYPE  _BV(23)
#define SMMU_PMCG_CFGR_MSI              _BV(21)
#define SMMU_PMCG_CFGR_RELOC_CTRS       _BV(20)
#define SMMU_PMCG_CFGR_SIZE             GENMASK(13, 8)
#define SMMU_PMCG_CFGR_NCTR             GENMASK(5, 0)
#define SMMU_PMCG_CR_ENABLE             _BV(0)
#define SMMU_PMCG_IIDR_PRODUCTID        GENMASK(31, 20)
#define SMMU_PMCG_IIDR_VARIANT          GENMASK(19, 16)
#define SMMU_PMCG_IIDR_REVISION         GENMASK(15, 12)
#define SMMU_PMCG_IIDR_IMPLEMENTER      GENMASK(11, 0)
#define SMMU_PMCG_IRQ_CTRL_IRQEN        _BV(0)

/* IMP-DEF ID registers */
#define SMMU_PMCG_PIDR0_PART_0          GENMASK(7, 0)
#define SMMU_PMCG_PIDR1_DES_0           GENMASK(7, 4)
#define SMMU_PMCG_PIDR1_PART_1          GENMASK(3, 0)
#define SMMU_PMCG_PIDR2_REVISION        GENMASK(7, 4)
#define SMMU_PMCG_PIDR2_DES_1           GENMASK(2, 0)
#define SMMU_PMCG_PIDR3_REVAND          GENMASK(7, 4)
#define SMMU_PMCG_PIDR4_DES_2           GENMASK(3, 0)

/* MSI config fields */
#define MSI_CFG0_ADDR_MASK              GENMASK_ULL(51, 2)
#define MSI_CFG2_MEMATTR_DEVICE_nGnRE   0x1

#define SMMU_PMCG_DEFAULT_FILTER_SPAN   1
#define SMMU_PMCG_DEFAULT_FILTER_SID    GENMASK(31, 0)

#define SMMU_PMCG_MAX_COUNTERS          64
#define SMMU_PMCG_ARCH_MAX_EVENTS       128

#define SMMU_PMCG_PA_SHIFT              12

#define SMMU_PMCG_EVCNTR_RDONLY         _BV(0)
#define SMMU_PMCG_HARDEN_DISABLE        _BV(1)

#define smmu_pmcg_disable()					\
	do {							\
		__raw_writel(0, TCU_PMCG_CR(iommu_dev));	\
		__raw_writel(0, TCU_PMCG_IRQ_CTRL(iommu_dev));	\
	} while (0)
#define smmu_pmcg_enable()					\
	do {							\
		__raw_writel(SMMU_PMCG_IRQ_CTRL_IRQEN,		\
			     TCU_PMCG_IRQ_CTRL(iommu_dev));	\
		__raw_writel(SMMU_PMCG_CR_ENABLE,		\
			     TCU_PMCG_CR(iommu_dev));		\
	} while (0)
#define smmu_pmcg_counter_set_value(idx, value)			\
	__pmcg_write(value, TCU_PMCG_EVCNTR(iommu_dev, idx))
#define smmu_pmcg_counter_get_value(idx)			\
	__pmcg_read(TCU_PMCG_EVCNTR(iommu_dev, idx))
#define smmu_pmcg_counter_enable(idx)				\
	__raw_writeq(_BV_ULL(idx), TCU_PMCG_CNTENSET0(iommu_dev))
#define smmu_pmcg_counter_disable(idx)				\
	__raw_writeq(_BV_ULL(idx), TCU_PMCG_CNTENCLR0(iommu_dev))
#define smmu_pmcg_interrupt_enable(idx)				\
	__raw_writeq(_BV_ULL(idx), TCU_PMCG_INTENSET0(iommu_dev))
#define smmu_pmcg_interrupt_disable(idx)			\
	__raw_writeq(_BV_ULL(idx), TCU_PMCG_INTENCLR0(iommu_dev))
#define smmu_pmcg_set_evtyper(idx, val)				\
	__raw_writel(val, TCU_PMCG_EVTYPER(iommu_dev, idx))
#define smmu_pmcg_set_smr(idx, val)				\
	__raw_writel(val, TCU_PMCG_SMR(iommu_dev, idx))

struct smmu_perf_event {
	uint8_t state;
	struct list_head sibling_list;
#define PMCG_STOPPED	_BV(0)
#define PMCG_UPTODATE	_BV(1)
	perf_cnt_t count;
	uint32_t idx;
	uint64_t prev_count;
	/* Configure EVTYPERn.EVENT */
	uint16_t event;
	bool filter;
	/* Configure EVTYPERn.FILTER_SID_SPAN */
	bool filter_sid_span;
	/* Configure SMRn */
	uint32_t streamid;
	struct smmu_perf_event *group_leader;
};

struct smmu_pmu {
	struct smmu_perf_event events[SMMU_PMCG_MAX_COUNTERS];
	DECLARE_BITMAP(used_counters, SMMU_PMCG_MAX_COUNTERS);
	DECLARE_BITMAP(supported_events, SMMU_PMCG_ARCH_MAX_EVENTS);
	unsigned int irq;
	unsigned int on_cpu;
	unsigned int num_counters;
	caddr_t reloc_base;
	uint64_t counter_mask;
	uint32_t options;
	uint32_t iidr;
	bool global_filter;
};

#if NR_SMMU_STREAMS > 1
extern struct smmu_pmu smmu_pmus[];
#define smmu_pmu_ctrl		smmu_pmus[iommu_dev]
#define SMMU_PMCG_RELOC_BASE(i)	(smmu_pmus[i].reloc_base)
#else
extern struct smmu_pmu smmu_pmu_ctrl;
#define SMMU_PMCG_RELOC_BASE(i)	smmu_pmu_ctrl.reloc_base
#endif

#ifdef CONFIG_SMMU_PMCG
void smmu_pmcg_init(void);
void smmu_pmcg_start(void);
void smmu_pmcg_stop(void);
#else
#define smmu_pmcg_init()	do { } while (0)
#define smmu_pmcg_start()	do { } while (0)
#define smmu_pmcg_stop()	do { } while (0)
#endif

#endif /* __SMMU_PMCG_H_INCLUDE__ */
