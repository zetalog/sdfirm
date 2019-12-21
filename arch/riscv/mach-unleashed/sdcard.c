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

#include <target/mmc.h>
#include <target/delay.h>
#include <target/efi.h>
#include <target/cmdline.h>

void mmc_hw_spi_init(void)
{
	board_init_clock();
	sifive_qspi_chip_mode(SPI_FLASH_ID, QSPI_MODE_OFF);
	sifive_qspi_chip_select(SPI_FLASH_ID, 0);
	mmc_slot_ctrl.host_ocr = SD_OCR_HCS | MMC_OCR_DUAL_VOLTAGE;
}

void mmc_hw_card_detect(void)
{
	mmc_event_raise(MMC_EVENT_CARD_INSERT);
}

#ifdef SYS_REALTIME
void mmc_hw_irq_poll(void)
{
}
#else
void mmc_hw_irq_init(void)
{
}
#endif

static int unleashed_sdcard_cmd(uint8_t cmd, uint32_t arg, bool busy)
{
	mmc_slot_ctrl.rsp = MMC_R1;
	mmc_spi_send(cmd, arg);
	return mmc_slot_ctrl.r1;
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

int unleashed_sdcard_copy(void *dst, uint32_t src_lba, size_t size)
{
	volatile uint8_t *p = dst;
	long i = size;
	int rc = 0;
	uint8_t r1;

	if (size == 1)
		r1 = unleashed_sdcard_cmd(MMC_CMD_READ_SINGLE_BLOCK,
					  src_lba, false);
	else
		r1 = unleashed_sdcard_cmd(MMC_CMD_READ_MULTIPLE_BLOCK,
					  src_lba, false);
	if (r1 != 0x00)
		return -EINVAL;

	do {
		uint16_t crc, crc_exp;
		long n;

		crc = 0;
		n = MMC_DEF_BL_LEN;
		while (mmc_spi_dummy() != SD_DATA_TOKEN);
		do {
			uint8_t x = mmc_spi_dummy();
			*p++ = x;
			crc = crc16(crc, x);
		} while (--n > 0);
		crc_exp = ((uint16_t)mmc_spi_dummy() << 8);
		crc_exp |= mmc_spi_dummy();
		if (crc != crc_exp) {
			rc = -EINVAL;
			break;
		}
		if ((i % 2000) == 0) {
			puts(".");
		}
	} while (--i > 0);

	if (size > 1)
		unleashed_sdcard_cmd(MMC_CMD_STOP_TRANSMISSION, 0, true);
	return rc;
}

static int do_sdcard(int argc, char *argv[])
{
	uint8_t gpt_buf[MMC_DEF_BL_LEN];
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
	err = unleashed_sdcard_copy(&hdr, GPT_HEADER_LBA, 1);
	if (err)
		return -EINVAL;
	mem_print_data(0, &hdr, 1, sizeof (gpt_header));
	partition_entries_lba_end = (hdr.partition_entries_lba +
		(hdr.num_partition_entries * hdr.partition_entry_size +
		 MMC_DEF_BL_LEN - 1) / MMC_DEF_BL_LEN);
	for (i = hdr.partition_entries_lba;
	     i < partition_entries_lba_end; i++) {
		unleashed_sdcard_copy(gpt_buf, i, 1);
		gpt_entries = (gpt_partition_entry *)gpt_buf;
		num_entries = MMC_DEF_BL_LEN / hdr.partition_entry_size;
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

DEFINE_COMMAND(sdcard, do_sdcard, "SiFive QSPI SD card commands",
	"    - SiFive QSPI SD card flash commands\n"
	"gpt ...\n"
	"    - print GPT entry information"
	"\n"
);
