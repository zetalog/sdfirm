/*
 * ZETALOG's Personal COPYRIGHT
 *
 * Copyright (c) 2021
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
 * @(#)spi.h: DPU-LP serial peripheral interface (SPI) definitions
 * $Id: spi.h,v 1.1 2021-11-01 15:16:00 zhenglv Exp $
 */

#ifndef __SPI_DPULP_H_INCLUDE__
#define __SPI_DPULP_H_INCLUDE__

#include <target/gpio.h>
#include <target/clk.h>
#include <target/mtd.h>

#define DW_SSI_CLK		ssi_reset
#define DW_SSI_BASE(n)		SSI_BASE
#define SSI_ID			0

#ifdef CONFIG_DW_SSI
#include <driver/dw_ssi.h>
#ifndef ARCH_HAVE_SPI
#define ARCH_HAVE_SPI		1
#else
#error "Multiple SPI controller defined"
#endif
#endif

#define DW_SSI_CLK_FREQ			(APB_CLK_FREQ) /* Hz */

#ifdef CONFIG_DW_SSI
#define spi_hw_config_mode(mode)	dw_ssi_config_mode(SSI_ID, mode)
#define spi_hw_config_freq(khz)		dw_ssi_config_freq(SSI_ID, khz)
#define spi_hw_read_byte()		dw_ssi_read_byte(SSI_ID)
#define spi_hw_write_byte(byte)		dw_ssi_write_byte(SSI_ID, byte)
#define spi_hw_chip_select(chip)	dw_ssi_select_chip(SSI_ID, chip)
#define spi_hw_deselect_chips()		dw_ssi_deselect_chips(SSI_ID)
void spi_hw_ctrl_init(void);
#define spi_hw_ctrl_start()		dw_ssi_enable_ctrl(SSI_ID)
#define spi_hw_ctrl_stop()		dw_ssi_disable_ctrl(SSI_ID)
#endif

#ifdef CONFIG_DPULP_SSI_FLASH
void dpulp_ssi_init(void);
void dpulp_ssi_boot(void *boot, uint32_t addr, uint32_t size, bool jump);
int do_flash(int argc, char *argv[]);

extern mtd_t board_flash;
#else
#define dpulp_ssi_init()			do { } while (0)
#define dpulp_ssi_boot(boot, addr, size, jump)	do { } while (0)

#define board_flash				INVALID_MTD_ID
#endif

#endif /* __SPI_DPULP_H_INCLUDE__ */
