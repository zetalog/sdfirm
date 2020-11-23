#ifndef __IOMMU_DRIVER_H_INCLUDE__
#define __IOMMU_DRIVER_H_INCLUDE__

#ifdef CONFIG_ARCH_HAS_IOMMU
#include <asm/mach/iommu.h>
#endif

#ifndef ARCH_HAVE_IOMMU
#define iommu_hw_ctrl_init()		do { } while (0)
#define iommu_hw_group_select()		do { } while (0)
#define iommu_hw_domain_select()	do { } while (0)
#define iommu_hw_alloc_master(iommu)	INVALID_IOMMU_GRP
#endif

#endif /* __IOMMU_DRIVER_H_INCLUDE__ */
