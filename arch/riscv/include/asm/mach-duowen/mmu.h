#ifndef __MMU_DUOWEN_H_INCLUDE__
#define __MMU_DUOWEN_H_INCLUDE__

#define FIX_CLINT		(FIX_HOLE + 1)
#define FIX_CRCNTL		(FIX_HOLE + 2)
#define FIX_CFAB		(FIX_HOLE + 3)
#define FIX_APC0		(FIX_HOLE + 4)
#define FIX_APC1		(FIX_HOLE + 5)
#define FIX_APC2		(FIX_HOLE + 6)
#define FIX_APC3		(FIX_HOLE + 7)
#define FIX_UART		(FIX_HOLE + 8)
#define FIX_PLIC		(FIX_HOLE + 9)
#define MMU_HW_MAX_FIXMAP	(FIX_PLIC + 1)

void duowen_mmu_dump_maps(void);
void duowen_mmu_map_uart(int n);
void duowen_mmu_map_clk(void);

#endif /* __MMU_DUOWEN_H_INCLUDE__ */
