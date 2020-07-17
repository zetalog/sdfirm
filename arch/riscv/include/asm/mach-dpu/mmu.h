#ifndef __MMU_DPU_H_INCLUDE__
#define __MMU_DPU_H_INCLUDE__

#define FIX_CLINT		(FIX_HOLE + 1)
#define FIX_UART		(FIX_HOLE + 2)
#define FIX_GPIO		(FIX_HOLE + 3)
#define FIX_PLL			(FIX_HOLE + 4)
/* Need to be ordered reversely */
#define FIX_PLIC_EN_0_31	(FIX_HOLE + 5)
#define FIX_PLIC_PEND		(FIX_HOLE + 6)
#define FIX_PLIC_PRIO		(FIX_HOLE + 7)
/* Context base, ordered reversely */
#define FIX_PLIC_CTX_1		(FIX_HOLE + 8)
#define FIX_PLIC_CTX_0		(FIX_HOLE + 9)
#define MMU_HW_MAX_FIXMAP	(FIX_PLIC_CTX_1 + 1)

#endif /* __MMU_DPU_H_INCLUDE__ */
