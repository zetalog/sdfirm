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
 * @(#)sd.c: DUOWEN specific SD card implementation
 * $Id: sd.c,v 1.1 2020-12-30 17:06:00 zhenglv Exp $
 */

#include <target/mmc.h>
#include <target/uefi.h>
#include <target/efi.h>
#include <target/cmdline.h>
#include <target/mem.h>
#include <target/mtd.h>

void duowen_sd_power(void)
{
	__raw_setl(IMC_SD_HOST_REG_VOL_STABLE, SCSR_SD_STABLE);
}

void duowen_mshc_init(void)
{
	__unused mmc_slot_t sslot;

	duowen_sd_power();
	clk_enable(sd_clk);
	sslot = mmc_slot_save(0);
	sdhc_init(SD_FREQ_MIN, SD_FREQ_MAX);
	mmc_slot_restore(sslot);
}

void duowen_sd_init(void)
{
}

void duowen_sd_copy(void *buf, uint32_t addr, uint32_t size)
{
}

static inline uint8_t duowen_sd_read(uint32_t addr)
{
	uint8_t byte = 0;

	return byte;
}

void __duowen_sd_boot(void *boot, uint32_t addr, uint32_t size)
{
	int i;
	uint8_t *dst = boot;
	void (*boot_entry)(void) = boot;

	for (i = 0; i < size; i++, addr++)
		dst[i] = duowen_sd_read(addr);
	boot_entry();
}

void duowen_sd_boot(void *boot, uint32_t addr, uint32_t size)
{
	duowen_boot_cb boot_func;
#ifdef CONFIG_DUOWEN_BOOT_STACK
	__align(32) uint8_t boot_from_stack[256];

	boot_func = (duowen_boot_cb)boot_from_stack;
	memcpy(boot_from_stack, __duowen_ssi_flash_boot, 256);
#else
	boot_func = __duowen_sd_boot;
#endif
	boot_func(boot, addr, size);
	unreachable();
}

static int do_sd_gpt(int argc, char *argv[])
{
	uint8_t gpt_buf[GPT_LBA_SIZE];
	gpt_header hdr;
	uint64_t partition_entries_lba_end;
	gpt_partition_entry *gpt_entries;
	uint64_t i;
	uint32_t j;
	int err;
	uint32_t num_entries;

	err = mmc_card_read_sync(0, (uint8_t *)&hdr,
				 GPT_HEADER_LBA, 1);
	if (err)
		return -EINVAL;
	mem_print_data(0, &hdr, 1, sizeof (gpt_header));
	partition_entries_lba_end = (hdr.partition_entries_lba +
		(hdr.num_partition_entries * hdr.partition_entry_size +
		 GPT_LBA_SIZE - 1) / GPT_LBA_SIZE);
	for (i = hdr.partition_entries_lba;
	     i < partition_entries_lba_end; i++) {
		mmc_card_read_sync(0, gpt_buf, i, 1);
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

static int do_sd_status(int argc, char *argv[])
{
	uint32_t status = __raw_readl(SCSR_SD_STATUS);

	printf("Bus Power Control Interface Singnals:\n");
	printf("sd_vdd1_on: %d\n", !!(status & IMC_SD_VDD1_ON));
	switch (IMC_SD_VDD1_SEL(status)) {
	case 5:
		printf("sd_vdd1_sel: 1.8V\n");
		break;
	case 6:
		printf("sd_vdd1_sel: 3.0V\n");
		break;
	case 7:
		printf("sd_vdd1_sel: 3.3V\n");
		break;
	default:
		printf("sd_vdd1_sel: unknown\n");
		break;
	}
	printf("SD/eMMC Card Interface Singnals:\n");
	switch (IMC_SD_DATXFER_WIDTH(status)) {
	case 0:
		printf("sd_datxfer_width: 1-bit\n");
		break;
	case 1:
		printf("sd_datxfer_width: 4-bit\n");
		break;
	default:
		printf("sd_datxfer_width: 8-bit\n");
		break;
	}
	printf("Misc Singnals:\n");
	switch (IMC_SD_UHSI_DRV_STH(status)) {
	default:
	case 0:
		printf("uhs1_drv_sth: B\n");
		break;
	case 1:
		printf("uhs1_drv_sth: A\n");
		break;
	case 2:
		printf("uhs1_drv_sth: C\n");
		break;
	case 3:
		printf("uhs1_drv_sth: D\n");
		break;
	}
	printf("uhs1_swvolt_en: %s\n",
	       status & IMC_SD_UHSI_SWVOLT_EN ?
	       "1.8V" : "3.3V");

	return 0;
}

static int do_sd(int argc, char *argv[])
{
	if (argc < 2)
		return -EINVAL;

	if (strcmp(argv[1], "gpt") == 0)
		return do_sd_gpt(argc, argv);
	if (strcmp(argv[1], "status") == 0)
		return do_sd_status(argc, argv);
	return -ENODEV;
}

DEFINE_COMMAND(sd, do_sd, "SD card commands",
	"gpt ...\n"
	"    - print GPT entry information\n"
	"status...\n"
	"    - print SD power status information\n"
);
