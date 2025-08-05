/*
 * ZETALOG's Personal COPYRIGHT
 *
 * Copyright (c) 2020
 *    ZETALOG - "Lv ZHENG".  All rights reserved.
 *    Author: Lv "Zetalog" Zheng
 *    Internet: zhenglv@hotmail.com
 *
 * This COPYRIGHT used to protect Personal Intelligence Rights.
 * Redistribution and use in source and binary forms with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *    This product includes software developed by the Lv "Zetalog" ZHENG.
 * 3. Neither the name of this software nor the names of its developers may
 *    be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 * 4. Permission of redistribution and/or reuse of souce code partially only
 *    granted to the developer(s) in the companies ZETALOG worked.
 * 5. Any modification of this software should be published to ZETALOG unless
 *    the above copyright notice is no longer declaimed.
 *
 * THIS SOFTWARE IS PROVIDED BY THE ZETALOG AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE ZETALOG OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * @(#)spi.h: DPU serial peripheral interface (SPI) definitions
 * $Id: spi.h,v 1.1 2020-04-14 13:16:00 zhenglv Exp $
 */

#ifndef __SPI_DPU_H_INCLUDE__
#define __SPI_DPU_H_INCLUDE__

#include <target/gpio.h>
#include <target/clk.h>
#include <target/mtd.h>

#define DW_SSI_CLK		srst_spi
#define DW_SSI_BASE(n)		SSI_BASE
#define SSI_ID			0

#ifdef CONFIG_DW_SSI
#ifdef CONFIG_DPU_BOOT_ROM
/* Considering the delay, 12.5MHz SPI clock should work. However, this is
 * left here for customization rather than using the default max frequency
 * definition in dw_ssi.h.
 */
#define SPI_HW_MAX_FREQ			(DW_SSI_CLK_FREQ / 2000) /* kHz */
#else /* CONFIG_DPU_BOOT_ROM */
/* In the mindelay simulation, it founds a 10.4ns delay from master to the
 * slave and back when SSI clocks at 100MHz rate. Thus when slave sends
 * data using delayed clock, master samples wrong data with un-delayed
 * clock.
 */
#define SPI_HW_MAX_FREQ			(DW_SSI_CLK_FREQ / 4000) /* kHz */
#endif /* CONFIG_DPU_BOOT_ROM */
#include <driver/dw_ssi.h>
#ifndef ARCH_HAVE_SPI
#define ARCH_HAVE_SPI		1
#else
#error "Multiple SPI controller defined"
#endif
#endif

#define DW_SSI_CLK_FREQ			(APB_CLK_FREQ) /* Hz */
#ifdef CONFIG_SPIFLASH_ADDR_4BYTE
#define DW_SSI_ADDR_LEN			32
#else
#define DW_SSI_ADDR_LEN			24
#endif

#ifdef CONFIG_DW_SSI
#define spi_hw_config_mode(mode)	dw_ssi_config_mode(SSI_ID, mode)
#define spi_hw_config_freq(khz)		dw_ssi_config_freq(SSI_ID, khz)
#define spi_hw_read_byte()		dw_ssi_read_byte(SSI_ID)
#define spi_hw_write_byte(byte)		dw_ssi_write_byte(SSI_ID, byte)
#define spi_hw_chip_select(chip)	dw_ssi_select_chip(SSI_ID, chip)
#define spi_hw_deselect_chips()		dw_ssi_deselect_chips(SSI_ID)
#define spi_hw_ctrl_init()					\
	do {							\
		clk_enable(DW_SSI_CLK);				\
		dw_ssi_init_master(SSI_ID, SSI_SPI_FRF_STD,	\
				   SSI_TMOD_EEPROM_READ, 8, 8);	\
		dw_ssi_init_spi(SSI_ID, SSI_SPI_FRF_STD,	\
				8, DW_SSI_ADDR_LEN, 0);		\
	} while (0)
#define spi_hw_irq_init()		dw_ssi_irq_init()
#define spi_hw_handle_irq()		dw_ssi_handle_irq()
#define spi_hw_ctrl_start()		dw_ssi_start_ctrl(SSI_ID)
#define spi_hw_ctrl_stop()		dw_ssi_stop_ctrl(SSI_ID)
#endif

#ifdef CONFIG_DPU_SSI_FLASH
void dpu_ssi_flash_init(void);
void dpu_ssi_flash_boot(void *boot, uint32_t addr, uint32_t size);

extern mtd_t board_flash;
#else
#define dpu_ssi_flash_init()			do { } while (0)
#define dpu_ssi_flash_boot(boot, addr, size)	do { } while (0)
#endif

#ifdef CONFIG_DPU_SIM_SSI_IRQ
void dpu_ssi_irq_init(void);
#else
#define dpu_ssi_irq_init()		do { } while (0)
#endif

#endif /* __SPI_DPU_H_INCLUDE__ */
