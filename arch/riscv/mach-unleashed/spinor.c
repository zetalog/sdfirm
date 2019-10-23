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
 * @(#)spinor.c: FU540 (unleashed) QSPI SPI-NOR implementation
 * $Id: spinor.c,v 1.1 2019-10-23 10:23:00 zhenglv Exp $
 */

#include <target/spi.h>
#include <target/barrier.h>

#define MICRON_SPI_FLASH_CMD_RESET_ENABLE	0x66
#define MICRON_SPI_FLASH_CMD_MEMORY_RESET	0x99
#define MICRON_SPI_FLASH_CMD_READ		0x03
#define MICRON_SPI_FLASH_CMD_QUAD_FAST_READ	0x6b

#ifdef CONFIG_UNLEASHED_SPINOR_SINGLE
#define SPINOR_PAD_LEN_DEFAULT		SPINOR_PAD_LEN(0)
#define SPINOR_CMD_CODE_DEFAULT		\
	SPINOR_CMD_CODE(MICRON_SPI_FLASH_CMD_READ)
#define SPINOR_WIRE			SPI_FLASH_3WIRE
#endif
#ifdef CONFIG_UNLEASHED_SPINOR_QUAD
#define SPINOR_PAD_LEN_DEFAULT		SPINOR_PAD_LEN(8)
#define SPINOR_CMD_CODE_DEFAULT		\
	SPINOR_CMD_CODE(MICRON_SPI_FLASH_CMD_QUAD_FAST_READ)
#define SPINOR_WIRE			SPI_FLASH_6WIRE
#endif
#define SPINOR_DATA_PROTO_DEFAULT	SPINOR_DATA_PROTO(SPINOR_WIRE)

struct spi_device spid_spinor = {
	.mode = SPI_MODE(SPI_MODE_0) | SPI_ORDER(SPI_MSB) |
		SPI_FLASH(SPINOR_WIRE),
	.max_freq_khz = 10000,
	.chip = 0,
};
spi_t spi_spinor;

int spi_copy(void *buf, uint32_t addr, uint32_t size)
{
	uint8_t *buf_bytes = (uint8_t *)buf;
	unsigned int i;

	spi_select_device(spi_spinor);
	spi_txrx(MICRON_SPI_FLASH_CMD_READ);
	spi_txrx((addr >> 16) & 0xff);
	spi_txrx((addr >> 8) & 0xff);
	spi_txrx(addr & 0xff);
	for (i = 0; i < size; i++) {
		*buf_bytes = spi_txrx(0);
		buf_bytes++;
	}
	spi_deselect_device();
	return 0;
}

static void board_flash_init(uint8_t spi)
{
	sifive_qspi_disable_spinor(spi);
	sifive_qspi_config_freq(spi, clk_get_frequency(tlclk), 10000000);
	spi_txrx(MICRON_SPI_FLASH_CMD_RESET_ENABLE);
	spi_txrx(MICRON_SPI_FLASH_CMD_MEMORY_RESET);
}

int board_spinor_init(uint8_t spi)
{
	board_init_clock();
	spi_spinor = spi_register_device(&spid_spinor);

	board_flash_init(spi);

	__raw_writel(SPINOR_CMD_EN | SPINOR_ADDR_LEN(3) |
		     SPINOR_PAD_LEN_DEFAULT |
		     SPINOR_CMD_PROTO(QSPI_PROTO_SINGLE) |
		     SPINOR_ADDR_PROTO(QSPI_PROTO_SINGLE) |
		     SPINOR_DATA_PROTO_DEFAULT | SPINOR_CMD_CODE_DEFAULT |
		     SPINOR_PAD_CODE(0), QSPI_FFMT(spi));
	sifive_qspi_enable_spinor(spi);
	mb();
	return 0;
}
