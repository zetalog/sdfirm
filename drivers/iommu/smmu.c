#include <target/init.h>
#include <target/delay.h>
#include <target/iommu.h>
#include <target/console.h>
#include <target/spinlock.h>
#include <target/panic.h>

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

DEFINE_SPINLOCK(smmu_global_lock);
DEFINE_SPINLOCK(smmu_context_lock);

/* Wait for any pending TLB invalidations to complete */
static void __smmu_tlb_sync(caddr_t sync, caddr_t status)
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
	con_err("TLB sync timed out -- SMMU may be deadlocked\n");
}

void smmu_tlb_sync_context(void)
{
	irq_flags_t flags;

	spin_lock_irqsave(&smmu_context_lock, flags);
	__smmu_tlb_sync(SMMU_CB_TLBSYNC(iommu_dev, smmu_cb),
			SMMU_CB_TLBSTATUS(iommu_dev, smmu_cb));
	spin_unlock_irqrestore(&smmu_context_lock, flags);
}

void smmu_tlb_sync_global(void)
{
	irq_flags_t flags;

	spin_lock_irqsave(&smmu_global_lock, flags);
	__smmu_tlb_sync(SMMU_sTLBGSYNC(iommu_dev),
			SMMU_sTLBGSTATUS(iommu_dev));
	spin_unlock_irqrestore(&smmu_global_lock, flags);
}

void smmu_tlb_inv_context_s1(void)
{
	/* The TLBI write may be relaxed, so ensure that PTEs cleared by the
	 * current CPU are visible beforehand.
	 */
	wmb();
	__raw_writel(SMMU_ASID(smmu_domain_ctrl.asid),
		     SMMU_CB_TLBIASID(iommu_dev, smmu_cb));
	smmu_tlb_sync_context();
}

static void smmu_tlb_inv_range_s1(unsigned long iova, size_t size,
				  size_t granule, caddr_t reg)
{
	if (smmu_device_ctrl.features & SMMU_FEAT_COHERENT_WALK)
		wmb();

	if (smmu_domain_ctrl.fmt == SMMU_CONTEXT_AARCH64) {
		iova >>= 12;
		iova |= (uint64_t)smmu_domain_ctrl.asid << 48;
		do {
			__raw_writeq(iova, reg);
			iova += granule >> 12;
		} while (size -= granule);
	} else {
		iova = (iova >> 12) << 12;
		iova |= smmu_domain_ctrl.asid;
		do {
			__raw_writel(iova, reg);
			iova += granule;
		} while (size -= granule);
	}
}

void smmu_tlb_inv_walk_s1(unsigned long iova, size_t size, size_t granule)
{
	smmu_tlb_inv_range_s1(iova, size, granule,
			      SMMU_CB_TLBIVA(iommu_dev, smmu_cb));
	smmu_tlb_sync_context();
}

void smmu_tlb_inv_leaf_s1(unsigned long iova, size_t size, size_t granule)
{
	smmu_tlb_inv_range_s1(iova, size, granule,
			      SMMU_CB_TLBIVAL(iommu_dev, smmu_cb));
	smmu_tlb_sync_context();
}

void smmu_tlb_add_page_s1(unsigned long iova, size_t granule)
{
	smmu_tlb_inv_range_s1(iova, granule, granule,
			      SMMU_CB_TLBIVAL(iommu_dev, smmu_cb));
}

#ifdef CONFIG_ARCH_HAS_SMMU_S2
void smmu_tlb_inv_context_s2(void)
{
	/* The TLBI write may be relaxed, so ensure that PTEs cleared by the
	 * current CPU are visible beforehand.
	 */
	wmb();
	__raw_writel(SMMU_VMID(smmu_domain_ctrl.vmid),
		     SMMU_TLBIVMID(iommu_dev));
	smmu_tlb_sync_global();
}

static void smmu_tlb_inv_range_s2(unsigned long iova, size_t size,
				  size_t granule, caddr_t reg)
{
	if (smmu_device_ctrl.features & SMMU_FEAT_COHERENT_WALK)
		wmb();

	iova >>= 12;
	do {
		if (smmu_domain_ctrl.fmt == SMMU_CONTEXT_AARCH64)
			__raw_writeq(iova, reg);
		else
			__raw_writel(iova, reg);
		iova += granule >> 12;
	} while (size -= granule);
}

void smmu_tlb_inv_walk_s2(unsigned long iova, size_t size, size_t granule)
{
	smmu_tlb_inv_range_s2(iova, size, granule,
			      SMMU_CB_S2_TLBIIPAS2(iommu_dev, smmu_cb));
	smmu_tlb_sync_context();
}

void smmu_tlb_inv_leaf_s2(unsigned long iova, size_t size, size_t granule)
{
	smmu_tlb_inv_range_s2(iova, size, granule,
			      SMMU_CB_S2_TLBIIPAS2L(iommu_dev, smmu_cb));
	smmu_tlb_sync_context();
}

void smmu_tlb_add_page_s2(unsigned long iova, size_t granule)
{
	smmu_tlb_inv_range_s2(iova, granule, granule,
			      SMMU_CB_S2_TLBIIPAS2L(iommu_dev, smmu_cb));
}

void smmu_tlb_inv_any_s2_v1(unsigned long iova, size_t size, size_t granule)
{
	smmu_tlb_inv_range_s2(iova, granule, granule,
			      SMMU_CB_S2_TLBIIPAS2L(iommu_dev, smmu_cb));
}

void smmu_tlb_add_page_s2_v1(unsigned long iova, size_t granule)
{
	smmu_tlb_inv_context_s2();
}
#endif

smmu_cb_t smmu_alloc_cb(smmu_cb_t start, smmu_cb_t end)
{
	smmu_cb_t cb;

	cb = find_next_clear_bit(smmu_device_ctrl.context_map,
				 end, start);
	if (cb == end)
		return INVALID_SMMU_CB;
	test_bit(cb, smmu_device_ctrl.context_map);
	return cb;
}

void smmu_free_cb(smmu_cb_t cb)
{
	clear_bit(cb, smmu_device_ctrl.context_map);
}

void smmu_init_context_bank(iommu_cfg_t *cfg)
{
}

int smmu_init_domain_context(void)
{
	int start;
	unsigned long ias, oas;
	iommu_cfg_t pgtbl_cfg;
	iommu_fmt_t fmt = ARM_64_LPAE_S1;

	if (iommu_domain_ctrl.type == IOMMU_DOMAIN_IDENTITY) {
		smmu_domain_ctrl.stage = SMMU_DOMAIN_BYPASS;
		return 0;
	}

	if (!(smmu_device_ctrl.features & SMMU_FEAT_TRANS_S1))
		smmu_domain_ctrl.stage = SMMU_DOMAIN_S2;
	if (!(smmu_device_ctrl.features & SMMU_FEAT_TRANS_S2))
		smmu_domain_ctrl.stage = SMMU_DOMAIN_S1;

#ifdef CONFIG_ARM
	if (smmu_device_ctrl.features & SMMU_FEAT_PTFS_AARCH32_L)
		smmu_domain_ctrl.fmt = SMMU_CONTEXT_AARCH32_L;
	else if (smmu_device_ctrl.features & SMMU_FEAT_PTFS_AARCH32_S)
		smmu_domain_ctrl.fmt = SMMU_CONTEXT_AARCH32_S;
	else
		smmu_domain_ctrl.fmt = SMMU_CONTEXT_NONE;
#endif
#ifdef CONFIG_ARM64
	if (smmu_device_ctrl.features & (SMMU_FEAT_PTFS_AARCH64_64K |
					 SMMU_FEAT_PTFS_AARCH64_16K |
					 SMMU_FEAT_PTFS_AARCH64_4K))
		smmu_domain_ctrl.fmt = SMMU_CONTEXT_AARCH64;
	else
		smmu_domain_ctrl.fmt = SMMU_CONTEXT_NONE;
#endif
#ifdef CONFIG_RISCV
	if (smmu_device_ctrl.features & SMMU_FEAT_PTFS_RISCV_SV39)
		smmu_domain_ctrl.fmt = SMMU_CONTEXT_RISCV_SV39;
	else
		smmu_domain_ctrl.fmt = SMMU_CONTEXT_NONE;
#endif

	if (smmu_domain_ctrl.fmt == SMMU_CONTEXT_NONE)
		return 0;

	switch (smmu_domain_ctrl.stage) {
	case SMMU_DOMAIN_S1:
		smmu_domain_ctrl.type = SMMU_CBAR_TYPE_S1_TRANS_S2_BYPASS;
		start = smmu_device_ctrl.max_s2_cbs;
		ias = smmu_device_ctrl.va_size;
		oas = smmu_device_ctrl.ipa_size;
#ifdef CONFIG_ARM64
		if (smmu_domain_ctrl.fmt == SMMU_CONTEXT_AARCH64)
			fmt = ARM_64_LPAE_S1;
		else if (smmu_domain_ctrl.fmt == SMMU_CONTEXT_AARCH32_L) {
			fmt = ARM_64_LPAE_S1;
			ias = min(ias, UL(32));
			oas = min(oas, UL(40));
		} else {
			fmt = ARM_V7S;
			ias = min(ias, UL(32));
			oas = min(oas, UL(32));
		}
#endif
#ifdef CONFIG_RISCV
		if (smmu_domain_ctrl.fmt == SMMU_CONTEXT_RISCV_SV39) {
			fmt = RISCV_64_SV39_S1;
			ias = min(ias, UL(39));
			oas = min(oas, UL(55));
		} else if (smmu_domain_ctrl.fmt == SMMU_CONTEXT_RISCV_SV48) {
			fmt = RISCV_64_SV48_S1;
			ias = min(ias, UL(48));
			oas = min(oas, UL(55));
		} else {
			fmt = RISCV_32_SV32_S1;
			ias = min(ias, UL(32));
			oas = min(oas, UL(32));
		}
#endif
		break;
#ifdef CONFIG_ARCH_HAS_SMMU_S2
	case SMMU_DOMAIN_NESTED:
	case SMMU_DOMAIN_S2:
		break;
#endif
	default:
		return -EINVAL;
	}

	smmu_domain_ctrl.cb = smmu_alloc_cb(start,
		smmu_device_ctrl.max_s1_cbs);
	smmu_domain_ctrl.irpt = smmu_domain_ctrl.cb;

#ifdef CONFIG_ARCH_HAS_SMMU_S2
	if (smmu_domain_ctrl.stage == SMMU_DOMAIN_S2)
		smmu_domain_ctrl.vmid = smmu_domain_ctrl.cb + 1;
	else
#endif
		smmu_domain_ctrl.asid = smmu_domain_ctrl.cb;

	pgtbl_cfg.fmt = fmt;
	pgtbl_cfg.pgsize_bitmap = iommu_device_ctrl.pgsize_bitmap;
	pgtbl_cfg.ias = ias;
	pgtbl_cfg.oas = oas;
	pgtbl_cfg.coherent_walk = \
		smmu_device_ctrl.features & SMMU_FEAT_COHERENT_WALK;

	if (!iommu_pgtable_alloc(&pgtbl_cfg))
		return -ENOMEM;
	iommu_domain_ctrl.fmt = fmt;
	iommu_domain_ctrl.pgsize_bitmap = pgtbl_cfg.pgsize_bitmap;

	smmu_init_context_bank(&pgtbl_cfg);
#if 0
	smmu_write_context_back(...);
#endif

	/* TODO: register IRQs */
	return 0;
}

void smmu_probe_ids(void)
{
	__unused uint32_t id;

	printf("Probing IDs...\n");
	id = __raw_readl(SMMU_IDR0(iommu_dev));
	smmu_trans_regimes(id);
	smmu_trans_ops(id);
	smmu_ptfs_arch32(id);
	smmu_probe_cttw(id);
	smmu_max_streams(id);
	smmu_max_smrgs(id);

	id = __raw_readl(SMMU_IDR1(iommu_dev));
	smmu_max_pages(id);
	smmu_probe_hafdbs(id);
	smmu_probe_smcd(id);
	smmu_max_cbs(id);

	id = __raw_readl(SMMU_IDR2(iommu_dev));
	smmu_pa_sizes(id);
	smmu_va_size(id);
	smmu_ptfs_arch64(id);
	smmu_probe_vmid(id);

	if (smmu_device_ctrl.features & SMMU_FEAT_PTFS_ARCH32_S)
		iommu_device_ctrl.pgsize_bitmap |=
			SZ_4K | SZ_64K | SZ_1M | SZ_16M;
	if (smmu_device_ctrl.features &
	    (SMMU_FEAT_PTFS_ARCH32_L | SMMU_FEAT_PTFS_ARCH64_4K))
		iommu_device_ctrl.pgsize_bitmap |= SZ_4K | SZ_2M | SZ_1G;
	if (smmu_device_ctrl.features &	SMMU_FEAT_PTFS_ARCH64_16K)
		iommu_device_ctrl.pgsize_bitmap |= SZ_16K | SZ_32M;
	if (smmu_device_ctrl.features & SMMU_FEAT_PTFS_ARCH64_64K)
		iommu_device_ctrl.pgsize_bitmap |= SZ_64K | SZ_512M;
}

static void smmu_write_context_bank(struct arm_smmu_cb *cb,
				    uint8_t cbar, bool stage1,
				    uint16_t asid, uint16_t vmid,
				    uint8_t irptndx)
{
	uint32_t reg;

	/* Unassigned context banks only need disabling */
	if (!cb) {
		__raw_writel(0, SMMU_CB_SCTLR(iommu_dev, smmu_cb));
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
	if (smmu_devices[iommu_dev].features & SMMU_FEAT_VMID16)
		reg |= SMMU_CBA2R_VMID16(vmid);
	__raw_writel(reg, SMMU_CBA2R(iommu_dev, smmu_cb));
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
	} else if (!(smmu_device_ctrl.features & SMMU_FEAT_VMID16)) {
		/* 8-bit VMIDs live in CBAR */
		reg |= SMMU_CBAR_VMID(vmid);
	}
	__raw_writel(reg, SMMU_CBAR(iommu_dev, smmu_cb));

	/* TCR
	 * We must write this before the TTBRs, since it determines the
	 * access behaviour of some fields (in particular, ASID[15:8]).
	 */
#ifdef CONFIG_ARM_SMMUv1
	if (stage1)
		__raw_writel(cb->tcr[1], SMMU_CB_TCR2(iommu_dev, smmu_cb));
#endif
	__raw_writel(cb->tcr[0], SMMU_CB_TCR(iommu_dev, smmu_cb));

	/* TTBRs */
#ifdef CONFIG_SMMU_ARCH32_S
	__raw_writel(asid, SMMU_CB_CONTEXTIDR(iommu_dev, smmu_cb));
	__raw_writel(cb->ttbr[0], SMMU_CB_TTBR0(iommu_dev, smmu_cb));
	__raw_writel(cb->ttbr[1], SMMU_CB_TTBR1(iommu_dev, smmu_cb));
#else
	__raw_writel(cb->ttbr[0], SMMU_CB_TTBR0(iommu_dev, smmu_cb));
	if (stage1)
		__raw_writel(cb->ttbr[1], SMMU_CB_TTBR1(iommu_dev, smmu_cb));
#endif

	/* MAIRs (stage-1 only) */
	if (stage1) {
		__raw_writel(cb->mair[0],
			     SMMU_CB_MAIR(iommu_dev, smmu_cb, 0));
		__raw_writel(cb->mair[1],
			     SMMU_CB_MAIR(iommu_dev, smmu_cb, 1));
	}

	/* SCTLR */
	reg = SMMU_C_CFIE | SMMU_C_CFRE | SMMU_C_AFE |
	      SMMU_C_TRE | SMMU_C_M;
	if (stage1)
		reg |= SMMU_C_ASIDPNE;
#ifdef ENDIAN_BE
	reg |= SMMU_C_E;
#endif
	__raw_writel(reg, SMMU_CB_SCTLR(iommu_dev, smmu_cb));
}

#if 0
void arm_smmu_iotlb_sync(struct iommu_iotlb_gather *gather)
{
	if (smmu_device_ctrl.version == ARM_SMMU_V2 ||
	    smmu_domain_ctrl.stage == ARM_SMMU_DOMAIN_S1)
		smmu_tlb_sync_context();
	else
		smmu_tlb_sync_global();
}
#endif

static void smmu_device_reset(void)
{
	uint32_t reg;
	int i;
	__unused smmu_gr_t sgr;
	__unused smmu_cb_t scb;

	/* Clear global FSR */
	printf("Clearing global faults...\n");
	smmu_clear_global_fault();

	/* Reset stream mapping groups */
	printf("Resetting %d (fw reported) stream mapping groups...\n",
	       NR_IOMMU_GROUPS);
	for (i = 0; i < SMMU_HW_NUMSMES(iommu_dev); i++) {
		sgr = smmu_stream_save(i);
		smmu_stream_ctrl.count = 0;
		smmu_stream_ctrl.sme = INVALID_IOMMU_MAP;
		printf("Writing group %d S2CR...\n", i);
		smmu_write_s2cr(SMMU_S2CR_TYPE_INIT, 0, 0, false);
		printf("Writing group %d SMR...\n", i);
		smmu_write_smr(0, 0, false);
		smmu_stream_restore(sgr);
	}

	/* Make sure all context banks are disabled and clear CB_FSR  */
	printf("Resetting %d (hw reported) context banks...\n",
	       smmu_device_ctrl.max_s1_cbs);
	for (i = 0; i < smmu_device_ctrl.max_s1_cbs; i++) {
		scb = smmu_domain_save(i);
		printf("Writing context bank %d...\n", i);
		smmu_write_context_bank(NULL, SMMU_CBAR_TYPE_S2_TRANS,
					false, 0, 0, 0);
		__raw_writel(SMMU_FSR_FAULT, SMMU_CB_FSR(iommu_dev, smmu_cb));
		smmu_domain_restore(scb);
	}

	/* Invalidate the TLB, just in case */
	printf("Invalidating TLBs...\n");
	__raw_writel(QCOM_DUMMY_VAL, SMMU_TLBIALLH(iommu_dev));
	__raw_writel(QCOM_DUMMY_VAL, SMMU_TLBIALLNSNH(iommu_dev));

	printf("Configuring sCR0...\n");
	reg = __raw_readl(SMMU_sCR0(iommu_dev));

	/* Enable fault reporting */
	reg |= (SMMU_GFRE | SMMU_GFIE | SMMU_GCFGFRE | SMMU_GCFGFIE);

	/* Disable TLB broadcasting. */
	reg |= (SMMU_VMIDPNE | SMMU_PTM);

#ifdef CONFIG_SMMU_DISABLE_BYPASS
	reg |= SMMU_USFCFG;
#else
	reg &= ~SMMU_USFCFG;
#endif

	/* Disable forced broadcasting */
	reg &= ~SMMU_FB;

	/* Don't upgrade barriers */
	reg &= ~(SMMU_BSU(SMMU_BSU_MASK));

	if (smmu_device_ctrl.features & SMMU_FEAT_VMID16)
		reg |= SMMU_VMID16EN;

	if (smmu_device_ctrl.features & SMMU_FEAT_EXIDS)
		reg |= SMMU_EXIDENABLE;

	reg = smmu_hw_ctrl_reset(reg);

	printf("Synchronizing global TLBs...\n");
	smmu_tlb_sync_global();
	__raw_writel(reg, SMMU_sCR0(iommu_dev));

	printf("Enabling client accesses...\n");
	/* Enable client access, handling unmatched streams as appropriate */
	smmu_enable(iommu_dev);
}

#if 0
void smmu_stream_uninstall(void)
{
	smmu_write_s2cr(SMMU_S2CR_TYPE_INIT, 0, 0, false);
	smmu_write_smr(0, 0, false);
}
}
#endif

void smmu_master_init(void)
{
	int i;
	__unused smmu_gr_t gr, sm;

	/* Check the SIDs are in range of the SMMU and our stream table */
	for (i = 0; i < iommu_group_ctrl.nr_iommus; i++) {
		iommu_map_t map = iommu_group_ctrl.iommus[i];
		gr = IOMMU_MAP_BASE(map);
		sm = IOMMU_MAP_MASK(map);
		smmu_write_smr(gr, sm, true);
	}
}

void smmu_master_attach(void)
{
}

void smmu_device_exit(void)
{
	smmu_disable(iommu_dev);
}

void smmu_device_init(void)
{
	smmu_probe_ids();
	smmu_device_reset();
	smmu_test_smr_masks();
}

#ifdef SYS_REALTIME
void smmu_poll_irqs(void)
{
}
#endif
