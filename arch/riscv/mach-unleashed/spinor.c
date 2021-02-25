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
#include <target/uefi.h>
#include <target/cmdline.h>
#include <target/console.h>

#ifdef CONFIG_UNLEASHED_FLASH_QSPI0
#define SPINOR_BASE				QSPI0_FLASH_BASE
#endif
#ifdef CONFIG_UNLEASHED_FLASH_QSPI1
#define SPINOR_BASE				QSPI1_FLASH_BASE
#endif

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

void __unleashed_spinor_init(uint8_t spi)
{
	sifive_qspi_disable_spinor(spi);
	sifive_qspi_config_freq(spi, clk_get_frequency(tlclk), 10000000);
	spi_txrx(MICRON_SPI_FLASH_CMD_RESET_ENABLE);
	spi_txrx(MICRON_SPI_FLASH_CMD_MEMORY_RESET);
}

#ifdef CONFIG_UNLEASHED_SPINOR_RANDOM_ACCESS
int unleashed_spinor_init(uint8_t spi)
{
	__unleashed_spinor_init(spi);
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

int unleashed_spinor_copy(void *buf, uint32_t addr, uint32_t size)
{
	memcpy(buf, (const void *)SPINOR_BASE + addr, size);
	return 0;
}
#else
#define unleashed_spinor_init(spi)	__unleashed_spinor_init(spi)

int unleashed_spinor_copy(void *buf, uint32_t addr, uint32_t size)
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
#endif

int board_spinor_init(uint8_t spi)
{
	board_init_clock();
	spi_spinor = spi_register_device(&spid_spinor);
	unleashed_spinor_init(spi);
	return 0;
}

static int do_spinor(int argc, char *argv[])
{
	uint8_t gpt_buf[GPT_LBA_SIZE];
	gpt_header hdr;
	uint64_t partition_entries_lba_end;
	gpt_partition_entry *gpt_entries;
	uint64_t i;
	uint32_t j;
	int err;
	uint32_t num_entries;

	if (SPI_FLASH_ID == 2) {
		printf("SPI2 doesn't connect to an SPI-NOR flash!\n");
		return -EINVAL;
	}
	printf("Reading SPI-NOR from SPI%d...\n", SPI_FLASH_ID);
	err = unleashed_spinor_copy(&hdr,
		GPT_HEADER_LBA * GPT_LBA_SIZE, GPT_HEADER_BYTES);
	if (err)
		return -EINVAL;
	hexdump(0, &hdr, 1, sizeof (gpt_header));
	partition_entries_lba_end = (hdr.partition_entries_lba +
		(hdr.num_partition_entries * hdr.partition_entry_size +
		 GPT_LBA_SIZE - 1) / GPT_LBA_SIZE);
	for (i = hdr.partition_entries_lba;
	     i < partition_entries_lba_end; i++) {
		unleashed_spinor_copy(gpt_buf, i * GPT_LBA_SIZE,
				      GPT_LBA_SIZE);
		gpt_entries = (gpt_partition_entry *)gpt_buf;
		num_entries = GPT_LBA_SIZE / hdr.partition_entry_size;
		for (j = 0; j < num_entries; j++) {
			printf("%s:\n",
			       uuid_export(gpt_entries[j].partition_type_guid.u.uuid));
			printf("%016llX - %016llX \n",
			       gpt_entries[j].first_lba,
			       gpt_entries[i].last_lba);
		}
	}
	return 0;
}

DEFINE_COMMAND(spinor, do_spinor, "SiFive QSPI SPI-NOR commands",
	"    - SiFive QSPI SPI NOR flash commands\n"
	"gpt ...\n"
	"    - print GPT entry information\n"
);
