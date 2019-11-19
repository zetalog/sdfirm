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
 * @(#)sdhc.c: secure digital host controller (SDHC) implementation
 * $Id: sdhc.c,v 1.1 2019-10-08 18:46:00 zhenglv Exp $
 */

#include <target/mmc.h>
#include <target/spi.h>
#include <target/efi.h>
#include <target/cmdline.h>
#include <stdio.h>

mmc_rca_t mmc_spi_rca;

void mmc_spi_select(mmc_rca_t rca)
{
	mmc_spi_rca = rca;
}

#define SD_INIT_ERROR_CMD16	5
#define SD_COPY_ERROR_CMD18	1
#define SD_COPY_ERROR_CMD18_CRC	2

/* Command frame starts by asserting low and then high for first two clock
 * edges.
 */
#define SD_CMD(cmd)			(0x40 | (cmd))

spi_t spi_card;

/* Send dummy byte (all ones).
 *
 * Used in many cases to read one byte from SD card, since SPI is a
 * full-duplex protocol and it is necessary to send a byte in order to read
 * a byte.
 */
static inline uint8_t sd_dummy(void)
{
	return spi_txrx(0xFF);
}

uint8_t __mmc_spi_send(uint8_t cmd, uint32_t arg)
{
	uint8_t crc = 0;
	uint16_t n;
	uint8_t r;

	spi_select_device(spi_card);
	sd_dummy();
	spi_txrx(SD_CMD(cmd));
	if (cmd == MMC_CMD_GO_IDLE_STATE) {
		spi_txrx(0x00);
		spi_txrx(0x00);
		spi_txrx(0x00);
		spi_txrx(0x00);
		spi_txrx(MMC_CMD0_CRC);
	} else {
		crc = mmc_crc7_update(crc, SD_CMD(cmd));
		spi_txrx(arg >> 24);
		crc = mmc_crc7_update(crc, arg >> 24);
		spi_txrx(arg >> 16);
		crc = mmc_crc7_update(crc, arg >> 16);
		spi_txrx(arg >> 8);
		crc = mmc_crc7_update(crc, arg >> 8);
		spi_txrx(arg);
		crc = mmc_crc7_update(crc, arg >> 0);
		spi_txrx(crc);
	}

	n = 1000;
	do {
		r = sd_dummy();
		if (!(r & 0x80))
			break;
	} while (--n > 0);
	return r;
}

static inline void sd_cmd_end(void)
{
	sd_dummy();
	spi_deselect_device();
}

uint8_t mmc_spi_resp;

void mmc_spi_send(uint8_t cmd, uint32_t arg)
{
	mmc_spi_resp = __mmc_spi_send(cmd, arg);
	mmc_cmd_success();
}

void mmc_spi_recv(uint8_t *resp, uint16_t len)
{
	uint8_t r;

	while (len) {
		r = sd_dummy();
		*resp = r;
		resp++;
		len--;
	}
	sd_cmd_end();
}

static int sd_cmd(uint8_t cmd, uint32_t arg)
{
	return __mmc_spi_send(cmd, arg);
}

static int sd_cmd16(void)
{
	int rc;

	rc = (sd_cmd(MMC_CMD_SET_BLOCKLEN, 0x200) != 0x00);
	sd_cmd_end();
	return rc;
}

static uint16_t crc16(uint16_t crc, uint8_t data)
{
	/* CRC polynomial 0x11021 */
	crc = (uint8_t)(crc >> 8) | (crc << 8);
	crc ^= data;
	crc ^= (uint8_t)(crc >> 4) & 0xf;
	crc ^= crc << 12;
	crc ^= (crc & 0xff) << 5;
	return crc;
}

int sd_copy(void *dst, uint32_t src_lba, size_t size)
{
	volatile uint8_t *p = dst;
	long i = size;
	int rc = 0;

	if (sd_cmd16()) return SD_INIT_ERROR_CMD16;
	if (sd_cmd(MMC_CMD_READ_MULTIPLE_BLOCK, src_lba) != 0x00) {
		sd_cmd_end();
		return SD_COPY_ERROR_CMD18;
	}
	do {
		uint16_t crc, crc_exp;
		long n;

		crc = 0;
		n = 512;
		while (sd_dummy() != SD_DATA_TOKEN);
		do {
			uint8_t x = sd_dummy();
			*p++ = x;
			crc = crc16(crc, x);
		} while (--n > 0);
		crc_exp = ((uint16_t)sd_dummy() << 8);
		crc_exp |= sd_dummy();
		if (crc != crc_exp) {
			rc = SD_COPY_ERROR_CMD18_CRC;
			break;
		}
		if ((i % 2000) == 0) {
			puts(".");
		}
	} while (--i > 0);

	sd_cmd(MMC_CMD_STOP_TRANSMISSION, 0);
	sd_cmd_end();
	return rc;
}

static void mmc_spi_mode(void)
{
	int i;

	/* Switch SDHC adapter to SPI mode:
	 * Set SD card pin DI high for 74+ cycles
	 * SD CMD pin is the same pin as SPI DI, so CMD = 0xff means assert
	 * DI high for 8 cycles.
	 */
	spi_select_device(spi_card);
	for (i = 10; i > 0; i--)
		sd_dummy();
	spi_deselect_device();
}

struct spi_device spid_mmc_spi = {
	.mode = SPI_MODE(SPI_MODE_0) | SPI_ORDER(SPI_MSB),
	.max_freq_khz = 20000,
	.chip = 0,
};

void mmc_spi_init(void)
{
	mmc_hw_spi_init();
	spi_card = spi_register_device(&spid_mmc_spi);
	mmc_spi_mode();
}

#define SD_BLOCK_SIZE		512

static int do_mmc(int argc, char *argv[])
{
	uint8_t gpt_buf[SD_BLOCK_SIZE];
	gpt_header hdr;
	uint64_t partition_entries_lba_end;
	gpt_partition_entry *gpt_entries;
	uint64_t i;
	uint32_t j;
	int err;
	uint32_t num_entries;

	if (SPI_FLASH_ID != 2) {
		printf("Only SPI2 connects to an SDCard flash!\n");
		return -EINVAL;
	}
	printf("Reading SDCard from SPI%d...\n", SPI_FLASH_ID);
	err = sd_copy(&hdr, GPT_HEADER_LBA, 1);
	if (err)
		return -EINVAL;
	mem_print_data(0, &hdr, 1, sizeof (gpt_header));
	partition_entries_lba_end = (hdr.partition_entries_lba +
		(hdr.num_partition_entries * hdr.partition_entry_size +
		 SD_BLOCK_SIZE - 1) / SD_BLOCK_SIZE);
	for (i = hdr.partition_entries_lba;
	     i < partition_entries_lba_end; i++) {
		sd_copy(gpt_buf, i, 1);
		gpt_entries = (gpt_partition_entry *)gpt_buf;
		num_entries = SD_BLOCK_SIZE / hdr.partition_entry_size;
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

DEFINE_COMMAND(mmc, do_mmc, "multimedia card commands",
	"    - MMC commands\n"
	"gpt ...\n"
	"    - print GPT entry information"
	"\n"
);
