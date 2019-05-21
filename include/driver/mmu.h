#ifndef __MMU_DRIVER_H_INCLUDE__
#define __MMU_DRIVER_H_INCLUDE__

#ifdef CONFIG_ARCH_HAS_MMU
#include <asm/mmu.h>
#else
#define mmu_hw_ctrl_init()
#endif

#endif /* __MMU_DRIVER_H_INCLUDE__ */
