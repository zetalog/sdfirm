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
 * @(#)sdcard.c: FU540 (unleashed) QSPI SDCard adapter implementation
 * $Id: sdcard.c,v 1.1 2019-10-23 10:21:00 zhenglv Exp $
 */

#include <target/spi.h>
#include <target/delay.h>
#include <target/efi.h>
#include <target/cmdline.h>

#define SD_BLOCK_SIZE		512

#define SD_INIT_ERROR_CMD0	1
#define SD_INIT_ERROR_CMD8	2
#define SD_INIT_ERROR_ACMD41	3
#define SD_INIT_ERROR_CMD58	4
#define SD_INIT_ERROR_CMD16	5

#define SD_COPY_ERROR_CMD18	1
#define SD_COPY_ERROR_CMD18_CRC	2

#define SD_CMD_GO_IDLE_STATE		0
#define SD_CMD_SEND_IF_COND		8
#define SD_CMD_STOP_TRANSMISSION	12
#define SD_CMD_SET_BLOCKLEN		16
#define SD_CMD_READ_BLOCK_MULTIPLE	18
#define SD_CMD_APP_SEND_OP_COND		41
#define SD_CMD_APP_CMD			55
#define SD_CMD_READ_OCR			58
#define SD_RESPONSE_IDLE		0x1
/* Data token for commands 17, 18, 24 */
#define SD_DATA_TOKEN			0xfe

/* Command frame starts by asserting low and then high for first two clock
 * edges.
 */
#define SD_CMD(cmd)			(0x40 | (cmd))

struct spi_device spid_sdcard = {
	.mode = SPI_MODE(SPI_MODE_0) | SPI_ORDER(SPI_MSB),
	.max_freq_khz = 20000,
	.chip = 0,
};
spi_t spi_sdcard;

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

static int sd_cmd(uint8_t cmd, uint32_t arg, uint8_t crc)
{
	unsigned long n;
	uint8_t r;

	spi_select_device(spi_sdcard);
	sd_dummy();
	spi_txrx(cmd);
	spi_txrx(arg >> 24);
	spi_txrx(arg >> 16);
	spi_txrx(arg >> 8);
	spi_txrx(arg);
	spi_txrx(crc);

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

/* Reset card */
static int sd_cmd0(void)
{
	int rc;

	rc = (sd_cmd(SD_CMD(SD_CMD_GO_IDLE_STATE), 0, 0x95) != SD_RESPONSE_IDLE);
	sd_cmd_end();
	return rc;
}

/* Check for SD version and supported voltages */
static int sd_cmd8(void)
{
	/* Check for high capacity cards
	 * Fail if card does not support SDHC
	 */
	int rc;

	rc = (sd_cmd(SD_CMD(SD_CMD_SEND_IF_COND), 0x000001AA, 0x87) != SD_RESPONSE_IDLE);
	sd_dummy(); /* command version; reserved */
	sd_dummy(); /* reserved */
	rc |= ((sd_dummy() & 0xF) != 0x1); /* voltage */
	rc |= (sd_dummy() != 0xAA); /* check pattern */
	sd_cmd_end();
	return rc;
}

/* Send app command. Used as prefix to app commands (ACMD) */
static void sd_cmd55(void)
{
	sd_cmd(SD_CMD(SD_CMD_APP_CMD), 0, 0x65);
	sd_cmd_end();
}

/* Start SDC initialization process */
static int sd_acmd41(void)
{
	uint8_t r;

	do {
		sd_cmd55();
		r = sd_cmd(SD_CMD(SD_CMD_APP_SEND_OP_COND), 0x40000000, 0x77); /* HCS = 1 */
		sd_cmd_end();
	} while (r == SD_RESPONSE_IDLE);
	return (r != 0x00);
}

/* Read operation conditions register (OCR) to check for availability of
 * block addressing mode.
 */
#if 0
static int sd_cmd58(void)
{
	int rc;

	rc = (sd_cmd(SD_CMD(SD_CMD_READ_OCR), 0, 0xFD) != 0x00);
	rc |= ((sd_dummy() & 0x80) != 0x80); /* Power up status */
	sd_dummy(); /* Supported voltages */
	sd_dummy(); /* Supported voltages */
	sd_dummy(); /* Supported voltages */
	sd_cmd_end();
	return rc;
}
#else
static int sd_cmd58(void)
{
	/* HACK: Disabled due to bugs. It is not strictly necessary to
	 * issue this command if we only support SD cards that support
	 * SDHC mode.
	 */
	return 0;
}
#endif

/* Set block addressing mode */
static int sd_cmd16(void)
{
	int rc;

	rc = (sd_cmd(SD_CMD(SD_CMD_SET_BLOCKLEN), 0x200, 0x15) != 0x00);
	sd_cmd_end();
	return rc;
}

static uint8_t crc7(uint8_t prev, uint8_t in)
{
	/* CRC polynomial 0x89 */
	uint8_t remainder = prev & in;

	remainder ^= (remainder >> 4) ^ (remainder >> 7);
	remainder ^= remainder << 4;
	return remainder & 0x7f;
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
	uint8_t crc = 0;

	crc = crc7(crc, SD_CMD(SD_CMD_READ_BLOCK_MULTIPLE));
	crc = crc7(crc, src_lba >> 24);
	crc = crc7(crc, (src_lba >> 16) & 0xff);
	crc = crc7(crc, (src_lba >> 8) & 0xff);
	crc = crc7(crc, src_lba & 0xff);
	crc = (crc << 1) | 1;
	if (sd_cmd(SD_CMD(SD_CMD_READ_BLOCK_MULTIPLE),
		   src_lba, crc) != 0x00) {
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
		if ((i % 2000) == 0){ 
			puts(".");
		}
	} while (--i > 0);

	sd_cmd(SD_CMD(SD_CMD_STOP_TRANSMISSION), 0, 0x01);
	sd_cmd_end();
	return rc;
}

int board_sdcard_init(uint8_t spi)
{
	int i;

	board_init_clock();
	spi_sdcard = spi_register_device(&spid_sdcard);
	mdelay(1);
	sifive_qspi_chip_mode(spi, QSPI_MODE_OFF);
	sifive_qspi_chip_select(spi, 0);

	/* Switch SDHC adapter to SPI mode:
	 * Set SD card pin DI high for 74+ cycles
	 * SD CMD pin is the same pin as SPI DI, so CMD = 0xff means assert
	 * DI high for 8 cycles.
	 */
	spi_select_device(spi_sdcard);
	for (i = 10; i > 0; i--)
		sd_dummy();
	spi_deselect_device();

	/* Initialize SDCard */
	if (sd_cmd0()) return SD_INIT_ERROR_CMD0;
	if (sd_cmd8()) return SD_INIT_ERROR_CMD8;
	if (sd_acmd41()) return SD_INIT_ERROR_ACMD41;
	if (sd_cmd58()) return SD_INIT_ERROR_CMD58;
	if (sd_cmd16()) return SD_INIT_ERROR_CMD16;
	return 0;
}

static int do_sdcard(int argc, char *argv[])
{
	uint8_t gpt_buf[SD_BLOCK_SIZE];
	gpt_header hdr;
	uint64_t partition_entries_lba_end;
	gpt_partition_entry *gpt_entries;
	uint64_t i;
	uint32_t j;
	int err;
	uint32_t num_entries;

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

DEFINE_COMMAND(sdcard, do_sdcard, "SiFive QSPI SDCard commands",
	"    - SiFive QSPI SDCard adapter commands\n"
	"gpt ...\n"
	"    - print GPT entry information"
	"\n"
);
