#include <target/iommu.h>

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

void duowen_smmu_early_init(void)
{
	smmu_riscv_enable(IOMMU_DMAC);
	smmu_riscv_enable(IOMMU_PCIE);
}
