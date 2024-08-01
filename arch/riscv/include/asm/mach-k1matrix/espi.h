#ifndef __ESPI_K1MATRIX_H_INCLUDE__
#define __ESPI_K1MATRIX_H_INCLUDE__

#include <target/clk.h>

#define espi_mclk			rmu_espi_clk
#define espi_sclk			rmu_espi_clk
#define espi_mrst			rmu_espi_rst
#define espi_srst			rmu_espi_rst

#define ESPI_IRQ			IRQ_ESPI_CON

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

#ifdef CONFIG_K1MATRIX_ESPI
#include <driver/spacemit_espi.h>
#endif

#define espi_hw_ctrl_init()		k1matrix_espi_init()
#define espi_hw_get_cfg()		spacemit_espi_get_cfg()
#define espi_hw_set_cfg(cfgs)		spacemit_espi_set_cfg(cfgs)
#define espi_hw_write_cmd(opcode, hlen, hbuf, dlen, dbuf)	\
	spacemit_espi_write_cmd(opcode, hlen, hbuf, dlen, dbuf)
#define espi_hw_irq_init()		spacemit_espi_irq_init()
#define espi_hw_handle_irq()		spacemit_espi_handle_irq()

void k1matrix_espi_init(void);

#endif /* __ESPI_K1MATRIX_H_INCLUDE__ */
