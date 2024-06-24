#ifndef __LPC_K1MATRIX_H_INCLUDE__
#define __LPC_K1MATRIX_H_INCLUDE__

#include <target/clk.h>

#define lpc_clk                         rmu_lpc_clk
#define lpc_lclk                        rmu_lpc_lclk
#define SPACEMIT_LPC_CFG_BASE		__RMU_LPC_CFG_BASE
#define SPACEMIT_LPC_IO_BASE            __RMU_LPC_IO_BASE
#define SPACEMIT_LPC_MEM_BASE           __RMU_LPC_FLASH_BASE
#define CORELPC_BASE			__RMU_LPC_IO_BASE

#ifndef ARCH_HAVE_LPC
#define ARCH_HAVE_LPC			1
#else
#error "Multiple LPC controller defined"
#endif

#include <driver/spacemit_lpc.h>

#define lpc_hw_ctrl_init()		spacemit_lpc_init()

#endif /* __LPC_K1MATRIX_H_INCLUDE__ */
