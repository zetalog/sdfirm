#include <target/init.h>
#include <target/delay.h>
#include <target/iommu.h>
#include <target/console.h>
#include <target/spinlock.h>

#define TLB_LOOP_TIMEOUT		1000000	/* 1s! */
#define TLB_SPIN_COUNT			10

/* Apparently, some Qualcomm arm64 platforms which appear to expose their
 * SMMU global register space are still, in fact, using a hypervisor to
 * mediate it by trapping and emulating register accesses. Sadly, some
 * deployed versions of said trapping code have bugs wherein they go
 * horribly wrong for stores using r31 (i.e. XZR/WZR) as the source
 * register.
 */
#define QCOM_DUMMY_VAL			-1

struct arm_smmu_dev smmu_devs[SMMU_HW_MAX_CTRLS];
DEFINE_SPINLOCK(smmu_global_lock);
DEFINE_SPINLOCK(smmu_context_lock);

/* Wait for any pending TLB invalidations to complete */
static void __arm_smmu_tlb_sync(caddr_t sync, caddr_t status)
{
	unsigned int spin_cnt, delay;
	uint32_t reg;

	__raw_writel(QCOM_DUMMY_VAL, sync);
	for (delay = 1; delay < TLB_LOOP_TIMEOUT; delay *= 2) {
		for (spin_cnt = TLB_SPIN_COUNT; spin_cnt > 0; spin_cnt--) {
			reg = __raw_readl(status);
			if (!(reg & SMMU_SACTIVE))
				return;
			cpu_relax();
		}
		udelay(delay);
	}
	con_printf("TLB sync timed out -- SMMU may be deadlocked\n");
}

void arm_smmu_tlb_sync_context(int smmu, int cbndx)
{
	irq_flags_t flags;

	spin_lock_irqsave(&smmu_context_lock, flags);
	__arm_smmu_tlb_sync(SMMU_CB_TLBSYNC(smmu, cbndx),
			    SMMU_CB_TLBSTATUS(smmu, cbndx));
	spin_unlock_irqrestore(&smmu_context_lock, flags);
}

void arm_smmu_tlb_sync_global(int smmu)
{
	irq_flags_t flags;

	spin_lock_irqsave(&smmu_global_lock, flags);
	__arm_smmu_tlb_sync(SMMU_sTLBGSYNC(smmu), SMMU_sTLBGSTATUS(smmu));
	spin_unlock_irqrestore(&smmu_global_lock, flags);
}

void arm_smmu_tlb_inv_context_s1(int smmu, int cbndx, uint16_t asid)
{
	/* The TLBI write may be relaxed, so ensure that PTEs cleared by the
	 * current CPU are visible beforehand.
	 */
	wmb();
	__raw_writel(SMMU_ASID(asid), SMMU_CB_TLBIASID(smmu, cbndx));
	arm_smmu_tlb_sync_context(smmu, cbndx);
}

void arm_smmu_tlb_inv_context_s2(int smmu, int cbndx, uint16_t vmid)
{
	/* See above */
	wmb();
	__raw_writel(SMMU_VMID(vmid), SMMU_TLBIVMID(smmu));
	arm_smmu_tlb_sync_global(smmu);
}

void arm_smmu_tlb_inv_walk_s1(int smmu, int cbndx,
			      uint16_t asid, unsigned long iova,
			      size_t size, size_t granule)
{
	arm_smmu_tlb_inv_range_s1(smmu, cbndx, asid, iova,
				  size, granule, SMMU_CB_TLBIVA);
	arm_smmu_tlb_sync_context(smmu, cbndx);
}

void arm_smmu_tlb_inv_leaf_s1(int smmu, int cbndx,
			      uint16_t asid, unsigned long iova,
			      size_t size, size_t granule)
{
	arm_smmu_tlb_inv_range_s1(smmu, cbndx, asid, iova,
				  size, granule, SMMU_CB_TLBIVAL);
	arm_smmu_tlb_sync_context(smmu, cbndx);
}

void arm_smmu_tlb_add_page_s1(int smmu, int cbndx,
			      uint16_t asid, unsigned long iova,
			      size_t granule)
{
	arm_smmu_tlb_inv_range_s1(smmu, cbndx, asid, iova,
				  granule, granule, SMMU_CB_TLBIVAL);
}

#if 0
void arm_smmu_tlb_inv_walk_s2(int smmu, int cbndx, unsigned long iova,
			      size_t size, size_t granule)
{
	arm_smmu_tlb_inv_range_s2(smmu, cbndx, iova, size, granule,
				  SMMU_CB_S2_TLBIIPAS2);
	arm_smmu_tlb_sync_context(smmu, cbndx);
}

void arm_smmu_tlb_inv_leaf_s2(int smmu, int cbndx, unsigned long iova,
			      size_t size, size_t granule)
{
	arm_smmu_tlb_inv_range_s2(smmu, cbndx, iova, size, granule,
				  SMMU_CB_S2_TLBIIPAS2L);
	arm_smmu_tlb_sync_context(smmu, cbndx);
}

void arm_smmu_tlb_add_page_s2(int smmu, int cbndx,
			      unsigned long iova, size_t granule)
{
	arm_smmu_tlb_inv_range_s2(smmu, cbndx, iova, granule, granule,
				  SMMU_CB_S2_TLBIIPAS2L);
}
#endif

void arm_smmu_tlb_inv_any_s2_v1(int smmu, int cbndx,
				uint16_t vmid, unsigned long iova,
				size_t size, size_t granule)
{
	arm_smmu_tlb_inv_context_s2(smmu, cbndx, vmid);
}

/* On MMU-401 at least, the cost of firing off multiple TLBIVMIDs appears
 * almost negligible, but the benefit of getting the first one in as far
 * ahead of the sync as possible is significant, hence we don't just make
 * this a no-op and call arm_smmu_tlb_inv_context_s2() from .iotlb_sync as
 * you might think.
 */
void arm_smmu_tlb_add_page_s2_v1(int smmu, int cbndx,
				 uint16_t vmid, unsigned long iova,
				 size_t granule)
{
	if (smmu_devs[smmu].features & SMMU_FEAT_COHERENT_WALK)
		wmb();
	__raw_writel(vmid, SMMU_TLBIVMID(smmu));
}

#ifdef CONFIG_SMMU_SMRG
__init void smmu_smrg_init(int smmu)
{
	int i;

	/* Reset stream mapping groups: Initial values mark all SMRn as
	 * invalid and all S2CRn as bypass unless overridden.
	 */
	for (i = 0; i < smmu->num_mapping_groups; ++i)
		arm_smmu_write_sme(smmu, i);
}

#define arm_smmu_max_smrs(dev, id)					\
	do {								\
		(dev)->streamid_mask = (dev)->max_streams - 1;		\
		if ((id) & SMMU_SMS) {					\
			(dev)->features |= SMMU_FEAT_SMRG;		\
			(dev)->max_streams = SMMU_NUMSMRG(id);		\
		}							\
	} while (0)

/* The width of SMR's mask field depends on sCR0_EXIDENABLE, so this function
 * should be called after sCR0 is written.
 */
static void arm_smmu_test_smr_masks(int smmu)
{
	uint32_t smr;
	struct arm_smmu_dev *dev = &arm_smmu_devs[smmu];

	/* SMR.ID bits may not be preserved if the corresponding MASK bits
	 * are set, so check each one separately. We can reject masters
	 * later if they try to claim IDs outside these masks.
	 */
	smr = SMMU_SMR_ID(dev->streamid_mask);
	__raw_writel(smr, SMMU_SMR(smmu, 0));
	smr = __raw_readl(SMMU_SMR(smmu, 0));
	dev->streamid_mask = _GET_FV(SMMU_SMR_ID, smr);

	smr = SMMU_SMR_MASK(dev->streamid_mask);
	__raw_writel(smr, SMMU_SMR(smmu, 0));
	smr = __raw_readl(SMMU_SMR(smmu, 0));
	dev->smr_mask_mask = _GET_FV(SMMU_SMR_MASK, smr);
}
#else
#define arm_smmu_max_smrs(dev, id)		do { } while (0)
#define arm_smmu_test_smr_masks(smmu)		do { } while (0)
#endif

#define arm_smmu_page_sizes(dev)					\
	do {								\
		if ((dev)->features & SMMU_FEAT_PTFS_ARCH32_S)		\
			(dev)->pgsize_bitmap |=				\
				SZ_4K | SZ_64K | SZ_1M | SZ_16M;	\
		if ((dev)->features & (SMMU_FEAT_PTFS_ARCH32_L |	\
				       SMMU_FEAT_PTFS_ARCH64_4K))	\
			(dev)->pgsize_bitmap |= SZ_4K | SZ_2M | SZ_1G;	\
		if ((dev)->features & SMMU_FEAT_PTFS_ARCH64_16K)	\
			(dev)->pgsize_bitmap |= SZ_16K | SZ_32M;	\
		if ((dev)->features & SMMU_FEAT_PTFS_ARCH64_64K)	\
			(dev)->pgsize_bitmap |= SZ_64K | SZ_512M;	\
	} while (0)

void arm_smmu_probe_ids(int smmu)
{
	uint32_t id;
	struct arm_smmu_dev *dev = &smmu_devs[smmu];

	id = __raw_readl(SMMU_IDR0(smmu));
	arm_smmu_trans_regimes(dev, id);
	arm_smmu_arch32_ptfs(dev, id);
	arm_smmu_max_streams(dev, id);
	arm_smmu_max_smrs(dev, id);
	arm_smmu_probe_cttw(dev, id);

	id = __raw_readl(SMMU_IDR1(smmu));
	arm_smmu_max_pages(dev, id);
	arm_smmu_max_cbs(dev, id);

	id = __raw_readl(SMMU_IDR2(smmu));
	arm_smmu_pa_sizes(dev, id);
	arm_smmu_arch64_ptfs(dev, id);

	arm_smmu_page_sizes(dev);
}

static void arm_smmu_write_context_bank(int smmu, int idx,
					struct arm_smmu_cb *cb,
					uint8_t cbar, bool stage1,
					uint16_t asid, uint16_t vmid,
					uint8_t irptndx)
{
	uint32_t reg;

	/* Unassigned context banks only need disabling */
	if (!cb) {
		__raw_writel(0, SMMU_CB_SCTLR(smmu, idx));
		return;
	}

	/* CBA2R */
#ifdef CONFIG_ARM_SMMUv2
#ifdef CONFIG_SMMU_ARCH64
	reg = SMMU_CBA2R_VA64;
#else
	reg = 0;
#endif
	/* 16-bit VMIDs live in CBA2R */
	if (smmu_devs[smmu].features & SMMU_FEAT_VMID16)
		reg |= SMMU_CBA2R_VMID16(vmid);
	__raw_writel(reg, SMMU_CBA2R(smmu, idx));
#endif

	/* CBAR */
	reg = SMMU_CBAR_TYPE(cbar);
#ifdef CONFIG_ARM_SMMUv1
	reg |= SMMU_CBAR_IRPTNDX(irptndx);
#endif

	/* Use the weakest shareability/memory types, so they are
	 * overridden by the ttbcr/pte.
	 */
	if (stage1) {
		reg |= SMMU_CBAR_BPSHCFG(SMMU_NON_SHAREABLE) |
		       SMMU_CBAR_MEMATTR(SMMU_MEM_NORMAL_WB);
	} else if (!(smmu_devs[smmu].features & SMMU_FEAT_VMID16)) {
		/* 8-bit VMIDs live in CBAR */
		reg |= SMMU_CBAR_VMID(vmid);
	}
	__raw_writel(reg, SMMU_CBAR(smmu, idx));

	/* TCR
	 * We must write this before the TTBRs, since it determines the
	 * access behaviour of some fields (in particular, ASID[15:8]).
	 */
#ifdef CONFIG_ARM_SMMUv1
	if (stage1)
		__raw_writel(cb->tcr[1], SMMU_CB_TCR2(smmu, idx));
#endif
	__raw_writel(cb->tcr[0], SMMU_CB_TCR(smmu, idx));

	/* TTBRs */
#ifdef CONFIG_SMMU_ARCH32_S
	__raw_writel(asid, SMMU_CB_CONTEXTIDR(smmu, idx));
	__raw_writel(cb->ttbr[0], SMMU_CB_TTBR0(smmu, idx));
	__raw_writel(cb->ttbr[1], SMMU_CB_TTBR1(smmu, idx));
#else
	__raw_writel(cb->ttbr[0], SMMU_CB_TTBR0(smmu, idx));
	if (stage1)
		__raw_writel(cb->ttbr[1], SMMU_CB_TTBR1(smmu, idx));
#endif

	/* MAIRs (stage-1 only) */
	if (stage1) {
		__raw_writel(cb->mair[0], SMMU_CB_MAIR(smmu, idx, 0));
		__raw_writel(cb->mair[1], SMMU_CB_MAIR(smmu, idx, 1));
	}

	/* SCTLR */
	reg = SMMU_C_CFIE | SMMU_C_CFRE | SMMU_C_AFE |
	      SMMU_C_TRE | SMMU_C_M;
	if (stage1)
		reg |= SMMU_C_ASIDPNE;
#ifdef CONFIG_SMMU_BIG_ENDIAN
	reg |= SMMU_C_E;
#endif
	__raw_writel(reg, SMMU_CB_SCTLR(smmu, idx));
}

void arm_smmu_exit(int smmu)
{
	arm_smmu_disable(smmu);
}

__init void arm_smmu_init(int smmu)
{
	uint32_t reg;
	int i;
	struct arm_smmu_dev *dev = &smmu_devs[smmu];

	arm_smmu_probe_ids(smmu);

	/* Clear global FSR */
	arm_smmu_clear_global_fault(smmu);

	for (i = 0; i < dev->max_streams; i++) {
		arm_smmu_write_s2cr(smmu, i, SMMU_S2CR_TYPE_INIT,
				    0, 0, false);
		arm_smmu_write_smr(smmu, i, 0, 0, false);
	}

	/* Make sure all context banks are disabled and clear CB_FSR  */
	for (i = 0; i < dev->max_s1_cbs; i++) {
		arm_smmu_write_context_bank(smmu, i, NULL,
					    SMMU_CBAR_TYPE_S2_TRANS,
					    false, 0, 0, 0);
		__raw_writel(SMMU_FSR_FAULT, SMMU_CB_FSR(smmu, i));
	}

	/* Invalidate the TLB, just in case */
	__raw_writel(QCOM_DUMMY_VAL, SMMU_TLBIALLH(smmu));
	__raw_writel(QCOM_DUMMY_VAL, SMMU_TLBIALLNSNH(smmu));

	reg = __raw_readl(SMMU_sCR0(smmu));

	/* Enable fault reporting */
	reg |= (SMMU_GFRE | SMMU_GFIE | SMMU_GCFGFRE | SMMU_GCFGFIE);

	/* Disable TLB broadcasting. */
	reg |= (SMMU_VMIDPNE | SMMU_PTM);

	/* Enable client access, handling unmatched streams as appropriate */
	reg &= ~SMMU_CLIENTPD;
#ifdef CONFIG_SMMU_DISABLE_BYPASS
	reg |= SMMU_USFCFG;
#else
	reg &= ~SMMU_USFCFG;
#endif

	/* Disable forced broadcasting */
	reg &= ~SMMU_FB;

	/* Don't upgrade barriers */
	reg &= ~(SMMU_BSU(SMMU_BSU_MASK));

	if (dev->features & SMMU_FEAT_VMID16)
		reg |= SMMU_VMID16EN;

	if (dev->features & SMMU_FEAT_EXIDS)
		reg |= SMMU_EXIDENABLE;

	arm_smmu_tlb_sync_global(smmu);
	__raw_writel(reg, SMMU_sCR0(smmu));

	arm_smmu_test_smr_masks(smmu);
}
