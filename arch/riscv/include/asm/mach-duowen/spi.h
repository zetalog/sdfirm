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
 * @(#)spi.h: DUOWEN serial peripheral interface (SPI) definitions
 * $Id: spi.h,v 1.1 2020-04-14 13:16:00 zhenglv Exp $
 */

#ifndef __SPI_DUOWEN_H_INCLUDE__
#define __SPI_DUOWEN_H_INCLUDE__

#include <target/gpio.h>
#include <target/clk.h>
#include <target/mtd.h>

#ifdef CONFIG_DUOWEN_SSI_FLASH_SPI0
#define DW_SSI_CLK(n)		spi0_clk
#define DW_SSI_BASE(n)		SPI0_BASE
#define SSI_ID			0
#define SSI_PORT		GPIO1B
#define SSI_PIN_RXD		pad_gpio_48
#define SSI_PIN_TXD		pad_gpio_49
#define SSI_PIN_SCK		pad_gpio_50
#define SSI_PIN_SS		pad_gpio_51
#define SSI_PIN_SS_IN		pad_gpio_52
#define IRQ_SPI			IRQ_SPI_MST0
#endif
#ifdef CONFIG_DUOWEN_SSI_FLASH_SPI1
#define DW_SSI_CLK(n)		spi1_clk
#define DW_SSI_BASE(n)		SPI1_BASE
#define SSI_ID			1
#define SSI_PORT		GPIO1B
#define SSI_PIN_RXD		pad_gpio_56
#define SSI_PIN_TXD		pad_gpio_57
#define SSI_PIN_SCK		pad_gpio_58
#define SSI_PIN_SS		pad_gpio_59
#define SSI_PIN_SS_IN		pad_gpio_60
#define IRQ_SPI			IRQ_SPI_MST1
#endif
#ifdef CONFIG_DUOWEN_SSI_FLASH_SPI2
#define DW_SSI_CLK(n)		spi2_clk
#define DW_SSI_BASE(n)		SPI2_BASE
#define SSI_ID			2
#define SSI_PORT		GPIO1C
#define SSI_PIN_RXD		pad_gpio_64
#define SSI_PIN_TXD		pad_gpio_65
#define SSI_PIN_SCK		pad_gpio_66
#define SSI_PIN_SS		pad_gpio_67
#define SSI_PIN_SS_IN		pad_gpio_68
#define IRQ_SPI			IRQ_SPI_MST2
#endif
#ifdef CONFIG_DUOWEN_SSI_FLASH_SPI3
#define DW_SSI_CLK(n)		spi3_clk
#define DW_SSI_BASE(n)		SPI3_BASE
#define SSI_ID			3
#define SSI_PORT		GPIO1D
#define SSI_PIN_RXD		pad_gpio_72
#define SSI_PIN_TXD		pad_gpio_73
#define SSI_PIN_SCK		pad_gpio_74
#define SSI_PIN_SS		pad_gpio_75
#define SSI_PIN_SS_IN		pad_gpio_76
#define IRQ_SPI			IRQ_SPI_MST3
#endif

#ifdef CONFIG_DW_SSI
/* In the mindelay simulation, it founds a 10.4ns delay from master to the
 * slave and back when SSI clocks at 100MHz rate. Thus when slave sends
 * data using delayed clock, master samples wrong data with un-delayed
 * clock.
 */
#define SPI_HW_MAX_FREQ			(DW_SSI_CLK_FREQ / 4000) /* kHz */
#include <driver/dw_ssi.h>
#ifndef ARCH_HAVE_SPI
#define ARCH_HAVE_SPI		1
#else
#error "Multiple SPI controller defined"
#endif
#endif

#define DW_SSI_CLK_FREQ			(APB_CLK_FREQ) /* Hz */

#ifdef CONFIG_DW_SSI
#define spi_hw_master_select(spi)	dw_ssi_master_select(spi)
#define spi_hw_config_mode(mode)	dw_ssi_config_mode(mode)
#define spi_hw_config_freq(khz)		dw_ssi_config_freq(khz)
#define spi_hw_read_byte()		dw_ssi_read_byte()
#define spi_hw_write_byte(byte)		dw_ssi_write_byte(byte)
#define spi_hw_chip_select(chip)	dw_ssi_select_chip(chip)
#define spi_hw_deselect_chips()		dw_ssi_deselect_chips()
void spi_hw_ctrl_init(void);
#define spi_hw_irq_init()		dw_ssi_irq_init()
#define spi_hw_handle_irq()		dw_ssi_handle_irq()
#define spi_hw_ctrl_start()		dw_ssi_start_ctrl()
#define spi_hw_ctrl_stop()		dw_ssi_stop_ctrl()
#endif

#ifdef CONFIG_DUOWEN_SSI_FLASH
void duowen_ssi_init(void);
void duowen_gpt_load(uint8_t *dst, uint32_t addr, uint32_t size);
void duowen_ssi_boot(void *boot, uint32_t addr, uint32_t size, bool jump);
/* Test command */
int do_flash(int argc, char *argv[]);

extern mtd_t board_flash;
#else
#define duowen_ssi_init()			do { } while (0)
#define duowen_ssi_boot(boot, addr, size, jump)	do { } while (0)
#define duowen_gpt_load(dst, addr, size)	do { } while (0)

#define board_flash			INVALID_MTD_ID
#endif

#ifdef CONFIG_DUOWEN_SSI_FLASH_IRQ
void duowen_ssi_irq_init(void);
#else
#define duowen_ssi_irq_init()		do { } while (0)
#endif

#endif /* __SPI_DUOWEN_H_INCLUDE__ */
