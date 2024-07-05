#ifndef __ESPI_K1MATRIX_H_INCLUDE__
#define __ESPI_K1MATRIX_H_INCLUDE__

#include <target/clk.h>

#define espi_mclk			rmu_espi_clk
#define espi_sclk			rmu_espi_clk
#define espi_mrst			rmu_espi_rst
#define espi_srst			rmu_espi_rst

#define SPACEMIT_ESPI_BASE		__RMU_ESPI_CFG_BASE
#define SPACEMIT_ESPI_IO_BASE		__RMU_ESPI_IO_BASE
#define SPACEMIT_ESPI_FLASH_BASE	__RMU_ESPI_FLASH_BASE
#define SPACEMIT_ESPI_PR_MEM0		(0x22000000U)
#define SPACEMIT_ESPI_PR_MEM1		(0x23000000U)

#ifndef ARCH_HAVE_ESPI
#define ARCH_HAVE_ESPI			1
#else
#error "Multiple eSPI controller defined"
#endif

#endif /* __ESPI_K1MATRIX_H_INCLUDE__ */