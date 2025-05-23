/*
 * ZETALOG's Personal COPYRIGHT
 *
 * Copyright (c) 2019
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
 * @(#)spi.h: FU540 (unleashed) specific SPI defintions
 * $Id: spi.h,v 1.1 2019-10-22 13:27:00 zhenglv Exp $
 */

#ifndef __SPI_UNLEASHED_H_INCLUDE__
#define __SPI_UNLEASHED_H_INCLUDE__

#include <target/clk.h>

/*===========================================================================
 * SiFive QSPI driver
 *===========================================================================*/

#ifndef ARCH_HAVE_SPI
#define ARCH_HAVE_SPI		1
#else
#error "Multiple SPI controller defined"
#endif

#ifdef CONFIG_UNLEASHED_FLASH_QSPI0
#define SPI_FLASH_ID			0
#endif
#ifdef CONFIG_UNLEASHED_FLASH_QSPI1
#define SPI_FLASH_ID			1
#endif
#ifdef CONFIG_UNLEASHED_FLASH_QSPI2
#define SPI_FLASH_ID			2
#endif

#define SPI_HW_MAX_FREQ	(TLCLK_FREQ_FINAL / 2000) /* tlclk/2 in KHz */

#define QSPI_BASE(n)		\
	(QSPI0_BASE + (0x400 * ((n) << (n)) * ((n) << (n))))

#include <asm/mach/qspi.h>

#define spi_hw_write_byte(byte)		sifive_qspi_tx(SPI_FLASH_ID, (byte))
#define spi_hw_read_byte()		sifive_qspi_rx(SPI_FLASH_ID)
#define spi_hw_ctrl_init()		board_init_clock()
#define spi_hw_irq_init()		do { } while (0)
#define spi_hw_handle_irq()		do { } while (0)

/* select/deselect: should be implemented in vise versa way? */
#define spi_hw_chip_select(chip)				\
	sifive_qspi_chip_mode(SPI_FLASH_ID, QSPI_MODE_HOLD)
#define spi_hw_deselect_chips()					\
	sifive_qspi_chip_mode(SPI_FLASH_ID, QSPI_MODE_AUTO)

#define spi_hw_config_mode(mode)				\
	sifive_qspi_config_mode(SPI_FLASH_ID, mode)
#define spi_hw_config_freq(khz)					\
	sifive_qspi_config_freq(SPI_FLASH_ID,			\
				clk_get_frequency(tlclk),	\
				UL(1000) * khz)
#define spi_hw_ctrl_start()					\
	sifive_qspi_chip_mode(SPI_FLASH_ID, QSPI_MODE_AUTO)
#define spi_hw_ctrl_stop()					\
	sifive_qspi_chip_mode(SPI_FLASH_ID, QSPI_MODE_OFF)

#endif /* __SPI_UNLEASHED_H_INCLUDE__ */
