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
 * @(#)ssi_flash.c: DUOWEN specific SSI flash implementation
 * $Id: ssi_flash.c,v 1.1 2020-12-30 17:15:00 zhenglv Exp $
 */

#include <target/spiflash.h>
#include <target/cmdline.h>
#include <target/mem.h>
#include <target/uefi.h>
#include <target/efi.h>
#include <target/irq.h>

#define FLASH_PAGE_SIZE		256
#define FLASH_TOTAL_SIZE	4000000

mtd_t board_flash = INVALID_MTD_ID;

void duowen_ssi_flash_init(void)
{
	board_flash = spiflash_register_bank(0);
}

void duowen_ssi_flash_copy(void *buf, uint32_t addr, uint32_t size)
{
	mtd_t smtd;
	int i;
	uint8_t *dst = buf;

	smtd = mtd_save_device(board_flash);
	mtd_open(OPEN_READ, addr, size);
	for (i = 0; i < size; i++)
		dst[i] = mtd_read_byte();
	mtd_close();
	mtd_restore_device(smtd);
}

static inline void duowen_ssi_flash_select(uint32_t chips)
{
	__raw_clearl(SSI_EN, SSI_SSIENR(0));
	__raw_writel(chips, SSI_SER(0));
	__raw_setl(SSI_EN, SSI_SSIENR(0));
}

static inline void duowen_ssi_flash_writeb(uint8_t byte)
{
	while (!(__raw_readl(SSI_RISR(0)) & SSI_TXEI));
	__raw_writel(byte, SSI_DR(0, 0));
}

static inline uint8_t duowen_ssi_flash_readb(void)
{
        while (!(__raw_readl(SSI_RISR(0)) & SSI_RXFI));
        return __raw_readl(SSI_DR(0, 0));
}

static inline uint8_t duowen_ssi_flash_read(uint32_t addr)
{
	uint8_t byte;

	duowen_ssi_flash_select(_BV(0));
	duowen_ssi_flash_writeb(SF_READ_DATA);
	duowen_ssi_flash_writeb((uint8_t)(addr >> 16));
	duowen_ssi_flash_writeb((uint8_t)(addr >> 8));
	duowen_ssi_flash_writeb((uint8_t)(addr >> 0));
	byte = duowen_ssi_flash_readb();
	duowen_ssi_flash_select(0);
	return byte;
}

void __duowen_ssi_flash_boot(void *boot, uint32_t addr, uint32_t size)
{
	int i;
	uint8_t *dst = boot;
	void (*boot_entry)(void) = boot;

	for (i = 0; i < size; i++, addr++)
		dst[i] = duowen_ssi_flash_read(addr);
	boot_entry();
}

void duowen_ssi_flash_boot(void *boot, uint32_t addr, uint32_t size)
{
	duowen_boot_cb boot_func;
#ifdef CONFIG_DUOWEN_BOOT_STACK
	__align(32) uint8_t boot_from_stack[256];

	boot_func = (duowen_boot_cb)boot_from_stack;
	memcpy(boot_from_stack, __duowen_ssi_flash_boot, 256);
#else
	boot_func = __duowen_ssi_flash_boot;
#endif
	boot_func(boot, addr, size);
	unreachable();
}

#define PAGES_PER_LBA (GPT_LBA_SIZE / FLASH_PAGE_SIZE)

static int do_flash_gpt(int argc, char *argv[])
{
	uint8_t gpt_buf[FLASH_PAGE_SIZE];
	gpt_header hdr;
	uint64_t partition_entries_lba_end;
	gpt_partition_entry *gpt_entries;
	uint64_t i;
	uint32_t j;
	uint32_t num_entries;
	int k;
	int i_part = 0;
	int i_page = 0;

	duowen_ssi_flash_copy(&hdr,
		GPT_HEADER_LBA * GPT_LBA_SIZE, GPT_HEADER_BYTES);
	partition_entries_lba_end = (hdr.partition_entries_lba +
		(hdr.num_partition_entries * hdr.partition_entry_size +
		 FLASH_PAGE_SIZE - 1) / FLASH_PAGE_SIZE);
	for (i = hdr.partition_entries_lba;
	     i < partition_entries_lba_end; i++) {
		for (i_page = 0; i_page < PAGES_PER_LBA; i_page++) {
			duowen_ssi_flash_copy(gpt_buf, 
					(i * PAGES_PER_LBA + i_page) * FLASH_PAGE_SIZE,
					FLASH_PAGE_SIZE);
			gpt_entries = (gpt_partition_entry *)gpt_buf;
			num_entries = FLASH_PAGE_SIZE / hdr.partition_entry_size;
			for (j = 0; j < num_entries; j++) {
				/* Stop at empty entry with UUID = 0 */
				unsigned char name_str[GPT_PART_NAME_U16_LEN + 1] = {0};
				uint32_t *uuid_u32_ptr = (uint32_t *)
						&gpt_entries[j].partition_guid.u.uuid;
				int uuid_u32_cnt = sizeof(guid_t) / sizeof(uint32_t);
				for (k = 0; k < uuid_u32_cnt; k++) {
					if (uuid_u32_ptr[k] != 0)
						break;
				}
				if (k >= uuid_u32_cnt)
					goto last_part_done;

				/* Convert partition name to C string */
				for (k = 0; k < GPT_PART_NAME_U16_LEN; k++) {
					if (gpt_entries[j].name[k] == 0)
						break;
					name_str[k] = (unsigned char)gpt_entries[j].name[k];
				}
#if 0
				/* Stop at partition with NULL name */
				if (k <= 0)
					goto last_part_done;
#endif
				i_part++;
				printf("%d", i_part);
				printf(" %lld %lld", gpt_entries[j].first_lba, gpt_entries[j].last_lba);
				printf(" %s", uuid_export(gpt_entries[j].partition_guid.u.uuid));
				printf(" %s", name_str);
				printf("\n");
			}
		}
	}
last_part_done:
	return 0;
}

static int do_flash_dump(int argc, char *argv[])
{
	uint8_t buffer[FLASH_PAGE_SIZE];
	uint32_t addr = 128;
	size_t size = 32;

	if (argc > 2)
		addr = strtoul(argv[2], NULL, 0);
	if (argc > 3)
		size = strtoul(argv[3], NULL, 0);
	if (size >= FLASH_TOTAL_SIZE) {
		printf("addr should be less than %d\n", FLASH_TOTAL_SIZE);
		return -EINVAL;
	}
	if (size > FLASH_PAGE_SIZE) {
		printf("size should be less or equal than %d\n",
		       FLASH_PAGE_SIZE);
		return -EINVAL;
	}
	duowen_ssi_flash_copy(buffer, addr, size);
	mem_print_data(0, buffer, 1, size);
	return 0;
}

#ifdef CONFIG_DUOWEN_SIM_SSI_IRQ
static uint32_t dw_ssi_irqs;

static void duowen_ssi_handler(void)
{
	uint32_t irqs = dw_ssi_irqs_status(SSI_ID);

	if ((irqs & SSI_TXEI) && !(dw_ssi_irqs & SSI_TXEI)) {
		dw_ssi_disable_irqs(SSI_ID, SSI_TXEI);
		dw_ssi_irqs |= SSI_TXEI;
		dw_ssi_write_dr(SSI_ID, SF_READ_STATUS_1);
	}
	if ((irqs & SSI_RXFI) && !(dw_ssi_irqs & SSI_RXFI)) {
		dw_ssi_disable_irqs(SSI_ID, SSI_RXFI);
		dw_ssi_irqs |= SSI_RXFI;
		(void)dw_ssi_read_dr(SSI_ID);
	}
	dw_ssi_clear_irqs(SSI_ID, irqs);
	irqc_ack_irq(IRQ_SPI);
}

void duowen_ssi_irq_init(void)
{
	irqc_configure_irq(IRQ_SPI, 0, IRQ_LEVEL_TRIGGERED);
	irq_register_vector(IRQ_SPI, duowen_ssi_handler);
}

static int do_flash_irq(int argc, char *argv[])
{
	uint32_t irqs = SSI_RXFI | SSI_TXEI;
	mtd_t smtd;

	smtd = mtd_save_device(board_flash);
	mtd_open(OPEN_READ, 0, 8);
	mtd_close();
	mtd_restore_device(smtd);
	printf("IRQ test ready.\n");

	dw_ssi_irqs = 0;

	dw_ssi_select_chip(SSI_ID, 0);
	dw_ssi_enable_irqs(SSI_ID, irqs);
	irqc_enable_irq(IRQ_SPI);
	while (dw_ssi_irqs != irqs) {
		irq_local_enable();
		irq_local_disable();
	}
	irqc_disable_irq(IRQ_SPI);
	dw_ssi_disable_irqs(SSI_ID, irqs);
	dw_ssi_deselect_chips(SSI_ID);
	printf("IRQ test passed.\n");
	return 0;
}
#else
static int do_flash_irq(int argc, char *argv[])
{
	return -EINVAL;
}
#endif

static int do_flash(int argc, char *argv[])
{
	if (argc < 2)
		return -EINVAL;

	if (strcmp(argv[1], "dump") == 0)
		return do_flash_dump(argc, argv);
	if (strcmp(argv[1], "gpt") == 0)
		return do_flash_gpt(argc, argv);
	if (strcmp(argv[1], "irq") == 0)
		return do_flash_irq(argc, argv);
	return -ENODEV;
}

DEFINE_COMMAND(flash, do_flash, "SSI flash commands",
	"dump [addr] [size]\n"
	"    - dump content of SSI flash\n"
	"      addr: default to 128\n"
	"      size: default to 32\n"
	"gpt\n"
	"    - dump GPT partitions from SSI flash\n"
	"irq\n"
	"    - testing SSI IRQ\n"
);
