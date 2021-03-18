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

#include <target/uefi.h>
#include <target/cmdline.h>
#include <target/mem.h>
#include <target/mmcard.h>
#include <asm/mach/boot.h>

mtd_t board_sdcard = INVALID_MTD_ID;

void duowen_sd_power(void)
{
	__raw_setl(IMC_SD_HOST_REG_VOL_STABLE, SCSR_SD_STABLE);
}

static inline void duowen_sd_gpio(uint16_t pin, uint8_t pad, uint8_t mux)
{
	gpio_config_pad(GPIO1A, pin, pad, 8);
	gpio_config_mux(GPIO1A, pin, mux);
}

void duowen_mshc_init(void)
{
	__unused mmc_slot_t sslot;

	duowen_sd_gpio(pad_gpio_32, GPIO_PAD_PULL_UP, TLMM_PAD_FUNCTION);
	duowen_sd_gpio(pad_gpio_33, GPIO_PAD_PULL_DOWN, TLMM_PAD_FUNCTION);
	duowen_sd_gpio(pad_gpio_34, GPIO_PAD_PULL_DOWN, TLMM_PAD_FUNCTION);
	duowen_sd_gpio(pad_gpio_35, GPIO_PAD_PULL_DOWN, TLMM_PAD_FUNCTION);
	duowen_sd_gpio(pad_gpio_36, GPIO_PAD_PULL_DOWN, TLMM_PAD_FUNCTION);
	duowen_sd_gpio(pad_gpio_37, GPIO_PAD_PULL_DOWN, TLMM_PAD_FUNCTION);
	duowen_sd_gpio(pad_gpio_38, GPIO_PAD_PULL_DOWN, TLMM_PAD_FUNCTION);
	duowen_sd_gpio(pad_gpio_39, GPIO_PAD_PULL_DOWN, TLMM_PAD_FUNCTION);
	duowen_sd_gpio(pad_gpio_40, GPIO_PAD_PULL_DOWN, TLMM_PAD_FUNCTION);

	clk_enable(sd_clk);
	sslot = mmc_slot_save(0);
	sdhc_init(0, 0, IRQ_SD);
	/* SoC power stable PIN assignment */
	duowen_sd_power();
	mmc_slot_restore(sslot);
}

void duowen_sd_init(void)
{
	__unused mmc_slot_t sslot;

	if (!mmc_slot_wait_state(DUOWEN_SD_SLOT, MMC_STATE_stby))
		return;

	sslot = mmc_slot_save(DUOWEN_SD_SLOT);
	mmc_select_mode(NULL);
	mmc_slot_restore(sslot);

	if (!mmc_slot_wait_state(DUOWEN_SD_SLOT, MMC_STATE_tran))
		return;

	board_sdcard = mmcard_register_card(DUOWEN_SD_CARD);
	if (board_sdcard == INVALID_MTD_ID)
		bh_panic();
}

#ifdef CONFIG_DUOWEN_BOOT_PROT
typedef void (*boot_cb)(void *, bool, uint16_t, uint32_t, uint32_t, bool);

static __always_inline bool __sdhc_xfer_dat(uint8_t *block, uint16_t blk_len,
					    uint16_t off, uint16_t len,
					    bool last_block)
{
	uint32_t dat;
	uint32_t *dwdat = (uint32_t *)block;
	uint16_t dwoff = off / 4;
	uint16_t dwlen = blk_len / 4, dwlength = len / 4;
	uint16_t dwoffset = 0;
	uint32_t irqs;
	bool result = true;

	/* ==================================================
	 * Recv data
	 * ================================================== */
	do {
		irqs = sdhc_irq_status(0);
		if (irqs & SDHC_ERROR_INTERRUPT_MASK) {
			result = false;
			goto exit_xfer;
		}
		if (sdhc_state_present(0, SDHC_BUFFER_READ_ENABLE) &&
		    irqs & SDHC_BUFFER_READ_READY) {
			sdhc_clear_irq(0, SDHC_BUFFER_READ_READY);
			break;
		}
	} while (1);

	while (dwlen) {
		dat = __raw_readl(SDHC_BUFFER_DATA_PORT(0));
		if (dwoffset == dwoff) {
			if (dwlength) {
				*dwdat = dat;
				dwdat++;
				dwlength--;
				__boot_dump32(dat, !dwlength);
			}
		} else
			dwoffset++;
		dwlen--;
	}

	if (!last_block)
		return true;

exit_xfer:
	/* ==================================================
	 * Stop transfer
	 * ================================================== */
	sdhc_disable_irq(0, SDHC_COMMAND_MASK | SDHC_TRANSFER_MASK);
	sdhc_clear_all_irqs(0);
	sdhc_software_reset(0, SDHC_SOFTWARE_RESET_FOR_CMD_LINE);
	sdhc_software_reset(0, SDHC_SOFTWARE_RESET_FOR_DAT_LINE);
	return result;
}

/* TODO: This function is endianess related. */
static __always_inline void __sdhc_decode_reg(uint8_t *resp,
					      uint8_t size, uint32_t reg)
{
	if (size > 0)
		resp[0] = HIBYTE(HIWORD(reg));
	if (size > 1)
		resp[1] = LOBYTE(HIWORD(reg));
	if (size > 2)
		resp[2] = HIBYTE(LOWORD(reg));
	if (size > 3)
		resp[3] = LOBYTE(LOWORD(reg));
}

static __always_inline bool __sdhc_xfer_cmd(uint8_t cmd, uint32_t arg,
					    uint32_t block_cnt,
					    uint16_t block_len)
{
	bool type = !!(cmd != MMC_CMD_STOP_TRANSMISSION);
	uint8_t rsp = cmd == MMC_CMD_STOP_TRANSMISSION ? MMC_R1b : MMC_R1;
	__unused size_t trans_bytes;
	uint32_t mask, flags, mode, cmpl_mask;
	uint32_t irqs;
	int i;
	uint32_t reg;
	bool result = true;
	uint8_t len;
	mmc_r1_t r1;
	uint8_t *resp = r1;
	uint8_t size = 4;

	/* ==================================================
	 * Start transfer
	 * ================================================== */
	while (sdhc_state_present(0, SDHC_COMMAND_INHIBIT));
	sdhc_clear_all_irqs(0);
	sdhc_enable_irq(0, SDHC_COMMAND_MASK | SDHC_TRANSFER_MASK);

	/* ==================================================
	 * Send command
	 * ================================================== */
	mask = SDHC_COMMAND_COMPLETE;
	if (!(rsp & MMC_RSP_PRESENT))
		flags = SDHC_RESPONSE_TYPE_SELECT(SDHC_NO_RESPONSE);
	else if (rsp & MMC_RSP_136)
		flags = SDHC_RESPONSE_TYPE_SELECT(SDHC_RESPONSE_LENGTH_136);
	else if (rsp & MMC_RSP_BUSY) {
		flags = SDHC_RESPONSE_TYPE_SELECT(SDHC_RESPONSE_LENGTH_48_BUSY);
		if (type)
			mask |= SDHC_TRANSFER_COMPLETE;
	} else
		flags = SDHC_RESPONSE_TYPE_SELECT(SDHC_RESPONSE_LENGTH_48);

	if (rsp & MMC_RSP_CRC)
		flags |= SDHC_COMMAND_CRC_CHECK_ENABLE;
	if (rsp & MMC_RSP_OPCODE)
		flags |= SDHC_COMMAND_INDEX_CHECK_ENABLE;

	if (type)
		flags |= SDHC_DATA_PRESENT_SELECT;

	/* Set Transfer mode regarding to data flag */
	if (type) {
		__raw_writeb(0xE, SDHC_TIMEOUT_CONTROL(0));
		mode = SDHC_BLOCK_COUNT_ENABLE;
		trans_bytes = block_cnt * block_len;
		if (block_cnt > 1)
			mode |= SDHC_MULTI_SINGLE_BLOCK_SELECT;
		if (type)
			mode |= SDHC_DATA_TRANSFER_DIRECTION_SELECT;

		__raw_writew(SDHC_TRANSFER_BLOCK_SIZE(block_len),
			     SDHC_BLOCK_SIZE(0));
		__raw_writew(block_cnt,
			     SDHC_16BIT_BLOCK_COUNT(0));
		__raw_writew(mode, SDHC_TRANSFER_MODE(0));
	} else if (rsp & MMC_RSP_BUSY) {
		__raw_writeb(0xE, SDHC_TIMEOUT_CONTROL(0));
	}

	__raw_writel(arg, SDHC_ARGUMENT(0));
	__raw_writew(SDHC_CMD(cmd, flags), SDHC_COMMAND(0));

	/* ==================================================
	 * Recv response
	 * ================================================== */
	do {
		irqs = sdhc_irq_status(0);

		if (irqs & SDHC_ERROR_INTERRUPT_MASK) {
			result = false;
			goto exit_xfer;
		}

		cmpl_mask = irqs & mask;
		if (cmpl_mask) {
			sdhc_clear_irq(0, cmpl_mask);
			unraise_bits(mask, cmpl_mask);
		}
		if (!mask)
			break;
	} while (1);

	if (rsp & MMC_RSP_136) {
		/* CRC is stripped so we need to do some shifting. */
		len = 0;
		for (i = 0; i < 4; i++) {
			reg = __raw_readl(
				SDHC_RESPONSE32(0, 3-i)) << 8;
			if (i != 3)
				reg |= __raw_readb(
					SDHC_RESPONSE8(0, (3-i)*4-1));
			__sdhc_decode_reg(resp + len,
					  size >= len ? 4 : 0, reg);
			len += 4;
		}
	} else {
		reg = __raw_readl(SDHC_RESPONSE32(0, 0));
		__sdhc_decode_reg(resp, size, reg);
	}

	if (type)
		return true;

exit_xfer:
	/* ==================================================
	 * Stop transfer
	 * ================================================== */
	sdhc_disable_irq(0, SDHC_COMMAND_MASK | SDHC_TRANSFER_MASK);
	sdhc_clear_all_irqs(0);
	sdhc_software_reset(0, SDHC_SOFTWARE_RESET_FOR_CMD_LINE);
	sdhc_software_reset(0, SDHC_SOFTWARE_RESET_FOR_DAT_LINE);
	return result;
}

__align(4)
void __sdhc_boot(void *boot, bool block_ccs, uint16_t block_len,
		 uint32_t addr, uint32_t size, bool jump)
{
	uint8_t *dst = boot;
	uint32_t block_cnt, cnt;
	uint32_t address;
	uint16_t offset, length;
	uint8_t cmd;
	uint32_t total_size;

#define is_first_blk(cnt)		((cnt) == 0)
#define is_last_blk(cnt, block_cnt)	(((cnt) + 1) == block_cnt)

	address = ALIGN_DOWN(addr, block_len);
	total_size = addr - address + size;
	block_cnt = ALIGN_UP(total_size, block_len) / block_len;
	cmd = block_cnt > 1 ?
	      MMC_CMD_READ_MULTIPLE_BLOCK : MMC_CMD_READ_SINGLE_BLOCK;
	if (!__sdhc_xfer_cmd(cmd, block_ccs ? address / block_len : address,
			     block_cnt, block_len))
		BUG();
	cnt = 0;
	offset = addr - address;
	do {
		length = 0;
		if (is_last_blk(cnt, block_cnt))
			length = total_size & (block_len - 1);
		if (!length)
			length = block_len;
		if (is_first_blk(cnt))
			length -= offset;
		if (!__sdhc_xfer_dat(dst, block_len, offset, length,
				     is_last_blk(cnt, block_cnt)))
			BUG();
		cnt++;
		dst += length;
		offset = 0;
	} while (cnt < block_cnt);
	cmd = MMC_CMD_STOP_TRANSMISSION;
	if (!__sdhc_xfer_cmd(cmd, 0, 0, 0))
		BUG();
	if (jump)
		__boot_jump(boot);
}

void duowen_sd_boot(void *boot, uint32_t addr, uint32_t size, bool jump)
{
	boot_cb boot_func;
	bool block_ccs = true;
	uint16_t block_len;
	__unused mmc_slot_t sslot;
	DUOWEN_BOOT_PROT_FUNC_DEFINE(2048);

	sslot = mmc_slot_save(DUOWEN_SD_SLOT);
	block_len = mmc_slot_ctrl.capacity_len;
	block_ccs = mmc_slot_ctrl.high_capacity;
	mmc_slot_restore(sslot);

	DUOWEN_BOOT_PROT_FUNC_ASSIGN(boot_cb, __sdhc_boot, boot_func);
	boot_func(boot, block_ccs, block_len, addr, ALIGN_UP(size, 4), jump);
}
#else
void duowen_sd_boot(void *boot, uint32_t addr, uint32_t size, bool jump)
{
	mtd_load(board_sdcard, boot, addr, size);
	if (jump)
		__boot_jump(boot);
}
#endif

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

DUOWEN_BOOT_PROT_TEST_FUNC(do_sd_boot, duowen_sd_boot);

static int do_sd(int argc, char *argv[])
{
	if (argc < 2)
		return -EINVAL;

	if (strcmp(argv[1], "gpt") == 0) {
		gpt_mtd_dump(board_sdcard);
		return 0;
	}
	if (strcmp(argv[1], "status") == 0)
		return do_sd_status(argc, argv);
	DUOWEN_BOOT_PROT_TEST_EXEC(do_sd_boot);
	return -ENODEV;
}

DEFINE_COMMAND(sd, do_sd, "SD card commands",
	"gpt\n"
	"    - print GPT entry information\n"
	"status\n"
	"    - print SD power status information\n"
	DUOWEN_BOOT_PROT_TEST_HELP
);
