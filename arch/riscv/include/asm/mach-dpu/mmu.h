#ifndef __MMU_DPU_H_INCLUDE__
#define __MMU_DPU_H_INCLUDE__

#define FIX_CLINT		(FIX_HOLE + 1)
#define FIX_UART		(FIX_HOLE + 2)
#define FIX_GPIO		(FIX_HOLE + 3)
#define FIX_PLL			(FIX_HOLE + 4)
#define FIX_TCSR		(FIX_HOLE + 5)
/* Need to be ordered reversely */
#define FIX_PLIC_EN_0_31	(FIX_HOLE + 6)
#define FIX_PLIC_PEND		(FIX_HOLE + 7)
#define FIX_PLIC_PRIO		(FIX_HOLE + 8)
/* Context base, ordered reversely */
#define FIX_PLIC_CTX_1		(FIX_HOLE + 9)
#define FIX_PLIC_CTX_0		(FIX_HOLE + 10)
/* Peripherals */
#define FIX_DDR0_CTRL		(FIX_HOLE + 11)
#define FIX_PE_DMA0		(FIX_HOLE + 12)
#define FIX_PCIEx_SUBSYS_CUST	(FIX_HOLE + 13)
#define MMU_HW_MAX_FIXMAP	(FIX_PCIEx_SUBSYS_CUST + 1)

#endif /* __MMU_DPU_H_INCLUDE__ */
