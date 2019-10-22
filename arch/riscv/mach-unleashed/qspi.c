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
 * @(#)qspi.c: FU540 (unleashed) specific QSPI implementation
 * $Id: qspi.c,v 1.1 2019-10-22 19:25:00 zhenglv Exp $
 */

#include <target/spi.h>
#include <target/delay.h>
#include <target/const.h>
#include <target/cmdline.h>
#include <target/clk.h>
#include <target/barrier.h>

#ifndef CONFIG_SIFIVE_QSPI_STATUS
uint32_t sifive_qspi_rx;
#endif

uint32_t sifive_qspi_min_div(uint32_t input_freq, uint32_t max_output_freq)
{
	/* The nearest integer solution for div requires rounding up as to
	 * not exceed max_output_freq.
	 *  div = ceil(Fin / (2 * Fsck)) - 1
	 *  i    = floor((Fin - 1 + 2 * Fsck) / (2 * Fsck)) - 1
	 * This should not overflow as long as (Fin - 1 + 2 * Fsck) does
	 * not exceed 2^32 - 1.
	 */
	uint32_t quotient = div32u(input_freq + (max_output_freq << 1) - 1,
				   max_output_freq << 1);
	/* Avoid underflow */
	return quotient ? quotient - 1 : 0;
}

void sifive_qspi_tx(uint8_t spi, uint8_t byte)
{
	while (!sifive_qspi_write_poll(spi));
	sifive_qspi_write_byte(spi, byte);
}

uint8_t sifive_qspi_rx(uint8_t spi)
{
	while (!sifive_qspi_read_poll(spi));
	return sifive_qspi_read_byte(spi);
}

#define spi_tx(byte)		spi_write_byte(byte)
#define spi_rx()		spi_read_byte()
uint8_t spi_txrx(uint8_t byte)
{
	spi_tx(byte);
	return spi_rx();
}

#ifdef CONFIG_UNLEASHED_FLASH_QSPI2
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

	spi_select_device(spi_flash);
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

struct spi_device spid_sdcard = {
	.mode = SPI_MODE_0 | SPI_MSB,
	.max_freq_khz = 20000,
	.chip = 0,
};
spi_t spi_sdcard;

int sifive_qspi_sdcard_init(uint8_t spi)
{
	int i;

	mdelay(1);
	sifive_qspi_config_mode(spi, SPI_MODE_0 | SPI_MSB);
	sifive_qspi_chip_select(spi, 0);
	sifive_qspi_config_freq(spi, clk_get_frequency(tlclk),
				UL(0x1000) * SPI_HW_MIN_FREQ);

	sifive_qspi_chip_mode(spi, QSPI_MODE_OFF);
	/* Set SD card pin DI high for 74+ cycles
	 * SD CMD pin is the same pin as SPI DI, so CMD = 0xff means assert
	 * DI high for 8 cycles.
	 */
	for (i = 10; i > 0; i--)
		sd_dummy();
	sifive_qspi_chip_mode(spi, QSPI_MODE_AUTO);
	/* Skip SD initialization commands if already done earlier and only
	 * set the clock divider for data transfer.
	 */
	if (sd_cmd0()) return SD_INIT_ERROR_CMD0;
	if (sd_cmd8()) return SD_INIT_ERROR_CMD8;
	if (sd_acmd41()) return SD_INIT_ERROR_ACMD41;
	if (sd_cmd58()) return SD_INIT_ERROR_CMD58;
	if (sd_cmd16()) return SD_INIT_ERROR_CMD16;

	/* Increase clock frequency after initialization for higher
	 * performance.
	 */
	sifive_qspi_config_freq(SPI_FLASH_ID,
				clk_get_frequency(tlclk),
				UL(1000) * spid_sdcard.max_freq_khz);
	return 0;
}
#else
struct spi_device spid_spinor = {
	.mode = SPI_MODE_0 | SPI_MSB,
	.max_freq_khz = 10000,
	.chip = 0,
};
spi_t spi_spinor;

#define MICRON_SPI_FLASH_CMD_RESET_ENABLE	0x66
#define MICRON_SPI_FLASH_CMD_MEMORY_RESET	0x99
#define MICRON_SPI_FLASH_CMD_READ		0x03
#define MICRON_SPI_FLASH_CMD_QUAD_FAST_READ	0x6b

int spi_copy(void *buf, uint32_t addr, uint32_t size)
{
	uint8_t *buf_bytes = (uint8_t *)buf;
	unsigned int i;

	spi_select_device(spi_flash);
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

static void sifive_qspi_flash_init(uint8_t spi)
{
	sifive_qspi_config_freq(spi, clk_get_frequency(tlclk), 10000000);
	sifive_qspi_disable_spinor(spi);
	spi_txrx(MICRON_SPI_FLASH_CMD_RESET_ENABLE);
	spi_txrx(MICRON_SPI_FLASH_CMD_MEMORY_RESET);
}

int sifive_qspi_spinor_init(uint8_t spi)
{
	sifive_qspi_flash_init(spi);

#ifdef CONFIG_UNLEASHED_SPINOR_SINGLE
#define SPINOR_PAD_LEN_DEFAULT		SPINOR_PAD_LEN(0)
#define SPINOR_DATA_PROTO_DEFAULT	SPINOR_DATA_PROTO(QSPI_PROTO_SINGLE)
#define SPINOR_CMD_CODE_DEFAULT		\
	SPINOR_CMD_CODE(MICRON_SPI_FLASH_CMD_READ)
#endif
#ifdef CONFIG_UNLEASHED_SPINOR_QUAD
#define SPINOR_PAD_LEN_DEFAULT		SPINOR_PAD_LEN(8)
#define SPINOR_DATA_PROTO_DEFAULT	SPINOR_DATA_PROTO(QSPI_PROTO_QUAD)
#define SPINOR_CMD_CODE_DEFAULT		\
	SPINOR_CMD_CODE(MICRON_SPI_FLASH_CMD_QUAD_FAST_READ)
#endif
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
#endif

#define GPT_GUID_SIZE		16
#define GPT_HEADER_LBA		1
#define GPT_HEADER_BYTES	92

#define SD_BLOCK_SIZE		512

typedef struct {
	uint32_t  time_low;
	uint16_t  time_mid;
	uint16_t  time_hi_and_version;
	uint8_t   clock_seq_hi_and_reserved;
	uint8_t   clock_seq_low;
	uint8_t   node[6];
} uuid_t;

typedef struct {
	union {
		uint8_t bytes[GPT_GUID_SIZE];
		uuid_t uuid;
	} u;
} gpt_guid;

#define UUID_LEN_STR    36

const char *uuid_export(uuid_t *u)
{
	static char s[UUID_LEN_STR+1];

	if (!u) return 0;
	if (snprintf(s, UUID_LEN_STR+1,
		     "%08lx-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x",
		     (unsigned long)u->time_low,
		     (unsigned int)u->time_mid,
		     (unsigned int)u->time_hi_and_version,
		     (unsigned int)u->clock_seq_hi_and_reserved,
		     (unsigned int)u->clock_seq_low,
		     (unsigned int)u->node[0],
		     (unsigned int)u->node[1],
		     (unsigned int)u->node[2],
		     (unsigned int)u->node[3],
		     (unsigned int)u->node[4],
		     (unsigned int)u->node[5]) != UUID_LEN_STR) {
		return 0;
	}
	return s;
}

#if 0
bool guid_equal(const gpt_guid *a, const gpt_guid *b)
{
	int i;

	for (i = 0; i < GPT_GUID_SIZE; i++) {
		if (a->u.bytes[i] != b->u.bytes[i]) {
			return false;
		}
	}
	return true;
}
#endif

typedef struct {
	uint64_t signature;
	uint32_t revision;
	uint32_t header_size;
	uint32_t header_crc;
	uint32_t reserved;
	uint64_t current_lba;
	uint64_t backup_lba;
	uint64_t first_usable_lba;
	uint64_t last_usable_lba;
	gpt_guid disk_guid;
	uint64_t partition_entries_lba;
	uint32_t num_partition_entries;
	uint32_t partition_entry_size;
	uint32_t partition_array_crc;
	/* gcc will pad this struct to an alignment the matches the
	 * alignment of the maximum member size, i.e. an 8-byte alignment.
	 */
	uint32_t padding;
} gpt_header;

typedef struct {
	uint64_t first_lba;
	uint64_t last_lba;	/* Inclusive */
} gpt_partition_range;

typedef struct {
	gpt_guid partition_type_guid;
	gpt_guid partition_guid;
	uint64_t first_lba;
	uint64_t last_lba;
	uint64_t attributes;
	uint16_t name[36];  /* UTF-16 */
} gpt_partition_entry;

static int do_qspi(int argc, char *argv[])
{
	uint8_t gpt_buf[SD_BLOCK_SIZE];
	gpt_header hdr;
	uint64_t partition_entries_lba_end;
	gpt_partition_entry *gpt_entries;
	uint64_t i;
	uint32_t j;
	int err;
	uint32_t num_entries;

	err = spi_flash_copy(&hdr, GPT_HEADER_LBA, 1);
	if (err)
		return -EINVAL;
	mem_print_data(0, &hdr, 1, sizeof (gpt_header));
	partition_entries_lba_end = (hdr.partition_entries_lba +
		(hdr.num_partition_entries * hdr.partition_entry_size +
		 SD_BLOCK_SIZE - 1) / SD_BLOCK_SIZE);
	for (i = hdr.partition_entries_lba;
	     i < partition_entries_lba_end; i++) {
		spi_flash_copy(gpt_buf, i, 1);
		gpt_entries = (gpt_partition_entry *)gpt_buf;
		num_entries = SD_BLOCK_SIZE / hdr.partition_entry_size;
		for (j = 0; j < num_entries; j++) {
			printf("%s:\n",
			       uuid_export(&gpt_entries[j].partition_type_guid.u.uuid));
			printf("%016llX - %016llX \n",
			       gpt_entries[j].first_lba,
			       gpt_entries[i].last_lba);
		}
	}
	return 0;
}

DEFINE_COMMAND(qspi, do_qspi, "SiFive QSPI test commands",
	"    - print brief description of all commands\n"
	"help command ...\n"
	"    - print detailed usage of 'command'"
	"\n"
);
