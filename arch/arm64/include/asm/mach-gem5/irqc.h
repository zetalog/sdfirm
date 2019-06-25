#ifndef __IRQC_GEM5_H_INCLUDE__
#define __IRQC_GEM5_H_INCLUDE__

#include <target/arch.h>
#include <target/paging.h>

#define __GICD_BASE		0x2C001000
#define __GICC_BASE		0x2C002000
#ifdef CONFIG_MMU
//#if defined(CONFIG_MMU) && !defined(CONFIG_MMU_IDMAP_DEVICE)
#define GICD_BASE		fix_to_virt(FIX_GICD)
#define GICC_BASE		fix_to_virt(FIX_GICC)
#else
#define GICD_BASE		__GICD_BASE
#define GICC_BASE		__GICC_BASE
#endif
#define GIC_PRIORITY_SHIFT	0

#include <asm/gicv2.h>

#endif /* __IRQC_GEM5_H_INCLUDE__ */
