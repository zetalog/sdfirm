#ifndef __MMU_K210_H_INCLUDE__
#define __MMU_K210_H_INCLUDE__

#define FIX_CLINT		(FIX_HOLE + 1)
#define FIX_PLIC		(FIX_HOLE + 2)
#define FIX_SYSCTL		(FIX_HOLE + 3)
#define FIX_GPIOHS		(FIX_HOLE + 4)
#define FIX_UARTHS		(FIX_HOLE + 5)
#define MMU_HW_MAX_FIXMAP	(FIX_UARTHS + 1)

#endif /* __MMU_K210_H_INCLUDE__ */
