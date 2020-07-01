#ifndef __MMU_DUOWEN_H_INCLUDE__
#define __MMU_DUOWEN_H_INCLUDE__

#define FIX_CLINT		(FIX_HOLE + 1)
#define FIX_PLIC		(FIX_HOLE + 2)
#define FIX_UART		(FIX_HOLE + 3)
#define MMU_HW_MAX_FIXMAP	(FIX_UART + 1)

void duowen_mmu_dump_maps(void);
void duowen_mmu_map_uart(int n);

#endif /* __MMU_DUOWEN_H_INCLUDE__ */
