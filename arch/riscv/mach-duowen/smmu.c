#include <target/iommu.h>

#ifdef CONFIG_DUOWEN_SMMU_SV
smmu_gr_t smmu_num_sms[2] = {
	NR_DMA_IOMMUS,
	NR_PCIE_IOMMUS,
};

smmu_sme_t smmu_dma_smes[NR_DMA_IOMMUS] = {
	SMMU_SME_DMA_TBU0,
	SMMU_SME_DMA_TBU1,
	SMMU_SME_DMA_TBU2,
	SMMU_SME_DMA_TBU3,
	SMMU_SME_DMA_TBU4,
	SMMU_SME_DMA_TBU5,
	SMMU_SME_DMA_TBU6,
	SMMU_SME_DMA_TBU7,
};

smmu_sme_t smu_pcie_smes[NR_PCIE_IOMMUS] = {
	SMMU_SME_PCIE_TBU0,
	SMMU_SME_PCIE_TBU1,
	SMMU_SME_PCIE_TBU2,
	SMMU_SME_PCIE_TBU3,
};
#endif /* CONFIG_DUOWEN_SMMU_SV */

/* Enable DMA Direct Mapping
 *
 * RISCV SMMU mode utilizes SoC PMA to distinguish memory and device
 * address spaces. It's required in RISCV environment to support DMA
 * direct mapping functionality.
 */
void duowen_smmu_early_init(void)
{
	smmu_enable_global_bypass(IOMMU_DMAC);
	smmu_enable_global_bypass(IOMMU_PCIE);
}
