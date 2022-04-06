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
#include <target/irq.h>
#include <asm/mach/boot.h>

#define SSI_BOOT_SIZE		4096

mtd_t board_flash = INVALID_MTD_ID;

static inline void spi_config_pad(uint16_t pin, uint8_t pad, uint8_t func)
{
	gpio_config_pad(SSI_PORT, pin, pad, 8);
	gpio_config_mux(SSI_PORT, pin, func);
}

void spi_hw_ctrl_init(void)
{
	spi_config_pad(SSI_PIN_RXD, GPIO_PAD_PULL_DOWN, TLMM_PAD_FUNCTION);
	spi_config_pad(SSI_PIN_TXD, GPIO_PAD_PULL_DOWN, TLMM_PAD_FUNCTION);
	spi_config_pad(SSI_PIN_SCK, GPIO_PAD_PULL_DOWN, TLMM_PAD_FUNCTION);
	spi_config_pad(SSI_PIN_SS, GPIO_PAD_PULL_UP, TLMM_PAD_FUNCTION);
	spi_config_pad(SSI_PIN_SS_IN, GPIO_PAD_PULL_UP, TLMM_PAD_FUNCTION);

	clk_enable(DW_SSI_CLK);
	dw_ssi_init_master(SSI_ID, SSI_FRF_SPI,
			   SSI_TMOD_EEPROM_READ, 8, 8);
	dw_ssi_init_spi(SSI_ID, SSI_SPI_FRF_STD,
			8, 24, 0);
}

void duowen_ssi_init(void)
{
	board_flash = spiflash_register_bank(0);
	if (board_flash == INVALID_MTD_ID)
		bh_panic();
}

#ifdef CONFIG_DUOWEN_BOOT_PROT
typedef void (*boot_cb)(void *, uint32_t, uint32_t, bool);

static __always_inline void __duowen_ssi_flash_select(uint32_t chips)
{
	__raw_writel(chips, SSI_SER(SSI_ID));
}

static __always_inline void __duowen_ssi_flash_writeb(uint8_t byte)
{
	while (!(__raw_readl(SSI_RISR(SSI_ID)) & SSI_TXEI));
	__raw_writel(byte, SSI_DR(SSI_ID, 0));
}

static __always_inline uint8_t __duowen_ssi_flash_readb(void)
{
        while (!(__raw_readl(SSI_RISR(SSI_ID)) & SSI_RXFI));
        return __raw_readl(SSI_DR(SSI_ID, 0));
}

static __always_inline uint8_t __duowen_ssi_flash_read(uint32_t addr)
{
	uint8_t byte;

	__duowen_ssi_flash_writeb(SF_READ_DATA);
	__duowen_ssi_flash_writeb((uint8_t)(addr >> 16));
	__duowen_ssi_flash_writeb((uint8_t)(addr >> 8));
	__duowen_ssi_flash_writeb((uint8_t)(addr >> 0));
	__duowen_ssi_flash_select(_BV(0));
	byte = __duowen_ssi_flash_readb();
	__duowen_ssi_flash_select(0);
	return byte;
}

__naked __align(__WORDSIZE)
void __duowen_ssi_boot(void *boot, uint32_t addr, uint32_t size, bool jump)
{
	int i;
	uint8_t *dst = boot;

#define is_last(index, length)		(((index) + 1) == length)

	__boot_dbg('|');
	for (i = 0; i < size; i++, addr++) {
		dst[i] = __duowen_ssi_flash_read(addr);
		__boot_dump8(dst[i], is_last(i, size));
		if ((i % 0x200) == 0)
			__boot_dbg('.');
	}
	__boot_dbg('|');
	__boot_dbg('\n');
	if (jump)
		__boot_jump(boot);
}

void duowen_ssi_boot(void *boot, uint32_t addr, uint32_t size, bool jump)
{
	volatile boot_cb boot_func;
	DUOWEN_BOOT_PROT_FUNC_DEFINE(SSI_BOOT_SIZE);

	DUOWEN_BOOT_PROT_FUNC_ASSIGN(boot_cb, __duowen_ssi_boot,
				     boot_func, SSI_BOOT_SIZE);
	boot_func(boot, addr, size, jump);
}
#else
void duowen_ssi_boot(void *boot, uint32_t addr, uint32_t size, bool jump)
{
	mtd_load(board_flash, boot, addr, size);
	if (jump)
		__boot_jump(boot);
}
#endif

static int do_flash_read(int argc, char *argv[])
{
	uint8_t buffer[GPT_LBA_SIZE];
	uint32_t addr = 512;
	size_t size = 32;

	if (argc > 2)
		addr = strtoul(argv[2], NULL, 0);
	if (argc > 3)
		size = strtoul(argv[3], NULL, 0);
	if (size > GPT_LBA_SIZE) {
		printf("size should be less or equal than %d\n",
		       GPT_LBA_SIZE);
		return -EINVAL;
	}
	mtd_load(board_flash, buffer, addr, size);
	hexdump(0, buffer, 1, size);
	return 0;
}

static int do_flash_write(int argc, char *argv[])
{
	uint8_t buffer[GPT_LBA_SIZE];
	uint32_t addr = 512;
	size_t size = 32;
	uint8_t byte = 'T';

	if (argc > 2)
		addr = strtoul(argv[2], NULL, 0);
	if (argc > 3)
		size = strtoul(argv[3], NULL, 0);
	if (size > GPT_LBA_SIZE) {
		printf("size should be less or equal than %d\n",
		       GPT_LBA_SIZE);
		return -EINVAL;
	}
	if (argc > 4)
		byte = strtoul(argv[4], NULL, 0);
	memset(buffer, byte, size);
	mtd_store(board_flash, buffer, addr, size);
	return 0;
}

#ifdef CONFIG_DUOWEN_SSI_FLASH_IRQ
static uint32_t dw_ssi_irqs;

static void duowen_ssi_handler(irq_t irq)
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
	printf("IRQ test enabled.\n");

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
	printf("IRQ test disabled.\n");
	return -EINVAL;
}
#endif

DUOWEN_BOOT_PROT_TEST_FUNC(do_flash_boot, duowen_ssi_boot);

int do_flash(int argc, char *argv[])
{
	if (argc < 2)
		return -EINVAL;

	if (strcmp(argv[1], "read") == 0)
		return do_flash_read(argc, argv);
	if (strcmp(argv[1], "write") == 0)
		return do_flash_write(argc, argv);
	if (strcmp(argv[1], "gpt") == 0) {
		gpt_mtd_dump(board_flash);
		return 0;
	}
	if (strcmp(argv[1], "irq") == 0)
		return do_flash_irq(argc, argv);
	DUOWEN_BOOT_PROT_TEST_EXEC(do_flash_boot);
	return -ENODEV;
}

DEFINE_COMMAND(flash, do_flash, "SSI flash commands",
	"read [addr] [size]\n"
	"    - read content of SSI flash\n"
	"      addr: default to 512\n"
	"      size: default to 32\n"
	"write [addr] [size] [byte]\n"
	"    - write content of SSI flash\n"
	"      addr: default to 512\n"
	"      size: default to 32\n"
	"      byte: byte value\n"
	"gpt\n"
	"    - dump GPT partitions from SSI flash\n"
	"irq\n"
	"    - testing SSI IRQ\n"
	DUOWEN_BOOT_PROT_TEST_HELP
);
