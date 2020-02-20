#ifndef __IOMMU_DUOWEN_H_INCLUDE__
#define __IOMMU_DUOWEN_H_INCLUDE__

#include <target/arch.h>

#define SMMU_BASE(smmu)		(DMA_SMMU_BASE + ((smmu) << 22))
#define SMMU_HW_MAX_CTRLS	2
#define SMMU_HW_PAGESIZE	0x1000
#define SMMU_HW_NUMSIDB		15
#define SMMU_HW_NUMPAGENDXB	3
#define SMMU_HW_NUMCB		16

#if defined(CONFIG_SMMU_MMU500)
#include <driver/arm_smmuv2.h>
#ifndef ARCH_HAVE_IOMMU
#define ARCH_HAVE_IOMMU		1
#else
#error "Multiple IOMMU controller defined"
#endif
#endif

#endif /* __IOMMU_DUOWEN_H_INCLUDE__ */
