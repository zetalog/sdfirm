#ifndef __MMU_UNLEASHED_H_INCLUDE__
#define __MMU_UNLEASHED_H_INCLUDE__

#define FIX_CLINT		(FIX_HOLE + 1)
#define FIX_PLIC		(FIX_HOLE + 2)
#define FIX_PRCI		(FIX_HOLE + 3)
#define FIX_GPIO		(FIX_HOLE + 4)
#define FIX_PINC		(FIX_HOLE + 5)
#define FIX_UART0		(FIX_HOLE + 6)
#define FIX_UART1		(FIX_HOLE + 7)
#define FIX_L2_CC		(FIX_HOLE + 8)
#define FIX_DDRC		(FIX_HOLE + 9)
#define FIX_QSPI0		(FIX_HOLE + 10)
#define FIX_QSPI1		(FIX_HOLE + 11)
#define FIX_QSPI2		(FIX_HOLE + 12)
#define MMU_HW_MAX_FIXMAP	(FIX_QSPI2 + 1)

#endif /* __MMU_UNLEASHED_H_INCLUDE__ */
