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
 * @(#)spacemit_espi.c: SpacemiT eSPI implementation
 * $Id: spacemit_espi.c,v 1.1 2023-11-27 16:50:00 zhenglv Exp $
 */
#include <target/console.h>
#include <target/barrier.h>
#include <target/irq.h>
#include <target/stream.h>
#include <target/cmdline.h>
#include <target/bh.h>

#include <driver/espi.h>
#include <driver/espi_protocol.h>
#include <driver/spacemit_espi.h>

static bh_t espi_bh;
static void (*rxvw_callback)(int group, uint8_t rxvw_data);
static void (*rxoob_callback)(void *buffer, int len);
static void *rxoob_buffer;
int espi_bh_create(void);

static uint32_t espi_read32(unsigned long reg)
{
	return __raw_readl(reg);
}

static void espi_write32(uint32_t val, unsigned long reg)
{
	__raw_writel(val, reg);
}

static uint16_t espi_read16(unsigned long reg)
{
	return __raw_readw(reg);
}

static void espi_write16(uint16_t val, unsigned long reg)
{
	__raw_writew(val, reg);
}

static uint8_t espi_read8(unsigned long reg)
{
	return __raw_readb(reg);
}

static void espi_write8(uint8_t val, unsigned long reg)
{
	__raw_writeb(val, reg);
}

static inline void espi_clear_upcmd_status(void)
{
	espi_write32(UP_RXHDR_0_UPCMD_STATUS, ESPI_UP_RXHDR_0);
}

static inline enum espi_upcmd_type espi_get_upcmd_type(void)
{
	return espi_read32(ESPI_UP_RXHDR_0) & UP_RXHDR_0_UPCMD_TYPE_MASK;
}

static inline uint32_t espi_get_up_rxdata(void)
{
	return espi_read32(ESPI_UP_RXDATA_PORT);
}

static inline uint32_t espi_get_flash_max_size(void)
{
	return FIELD_GET(MASTER_CAP_FLASH_MAX_SIZE_MASK, espi_read32(ESPI_MASTER_CAP));
}

static inline uint32_t espi_get_oob_max_size(void)
{
	return FIELD_GET(MASTER_CAP_OOB_MAX_SIZE_MASK, espi_read32(ESPI_MASTER_CAP));
}

static inline uint32_t espi_get_vw_max_size(void)
{
	return FIELD_GET(MASTER_CAP_VW_MAX_SIZE_MASK, espi_read32(ESPI_MASTER_CAP));
}

static inline uint32_t espi_get_pr_max_size(void)
{
	return FIELD_GET(MASTER_CAP_PR_MAX_SIZE_MASK, espi_read32(ESPI_MASTER_CAP));
}

static inline void espi_wdg_enable(void)
{
	espi_write32(espi_read32(ESPI_GLOBAL_CONTROL_0) | GLOBAL_CONTROL_0_WDG_EN_MASK,
		ESPI_GLOBAL_CONTROL_0);
}

static inline void espi_set_wdt_count(uint32_t count)
{
	espi_write32(espi_read32(ESPI_GLOBAL_CONTROL_0) |
		GLOBAL_CONTROL_0_WDG_CNT(count), ESPI_GLOBAL_CONTROL_0);
}

static inline void espi_set_master_wait_count(uint32_t count)
{
	espi_write32(espi_read32(ESPI_GLOBAL_CONTROL_0) |
		GLOBAL_CONTROL_0_WAIT_CNT(count), ESPI_GLOBAL_CONTROL_0);
}

static inline void espi_soft_reset(void)
{
	espi_write32(GLOBAL_CONTROL_1_SW_RST, ESPI_GLOBAL_CONTROL_1);
}

static inline void espi_vw_channel_slave_suspend_enter(void)
{
	espi_write32(espi_read32(ESPI_GLOBAL_CONTROL_1) |
		GLOBAL_CONTROL_1_SUS_STAT, ESPI_GLOBAL_CONTROL_1);
}

static inline void espi_vw_channel_slave_suspend_exit(void)
{
	espi_write32(espi_read32(ESPI_GLOBAL_CONTROL_1) &
		~GLOBAL_CONTROL_1_SUS_STAT, ESPI_GLOBAL_CONTROL_1);
}

static inline uint32_t espi_decode_io_range_en_bit(unsigned int idx)
{
	asm("j .");
	return 0;
//	if (ESPI_DECODE_RANGE_TO_REG_GROUP(idx) == 0)
//		return ESPI_DECODE_IO_RANGE_EN(idx);
//	else
//		return ESPI_DECODE_IO_RANGE_EXT_EN(idx - ESPI_DECODE_RANGES_PER_REG_GROUP);
}

static inline uint32_t espi_decode_mmio_range_en_bit(unsigned int idx)
{
	asm("j .");
	return 0;
//	if (ESPI_DECODE_RANGE_TO_REG_GROUP(idx) == 0)
//		return ESPI_DECODE_MMIO_RANGE_EN(idx);
//	else
//		return ESPI_DECODE_MMIO_RANGE_EXT_EN(idx - ESPI_DECODE_RANGES_PER_REG_GROUP);
}

static inline unsigned int espi_io_range_base_reg(unsigned int idx)
{
	asm("j .");
	return 0;
//	unsigned int reg_base;
//	switch (ESPI_DECODE_RANGE_TO_REG_GROUP(idx)) {
//	case 0:
//		reg_base = ESPI_IO_BASE_REG0;
//		break;
//	case 1:
//		reg_base = ESPI_IO_BASE_REG2;
//		break;
//	case 2:
//		reg_base = ESPI_IO_BASE_REG4;
//		break;
//	default: /* case 3 */
//		reg_base = ESPI_IO_BASE_REG6;
//		break;
//	}
//	return ESPI_IO_RANGE_BASE_REG(reg_base, ESPI_DECODE_RANGE_TO_REG_OFFSET(idx));
}

static inline unsigned int espi_io_range_size_reg(unsigned int idx)
{
	asm("j .");
	return 0;
//	unsigned int reg_base;
//	switch (ESPI_DECODE_RANGE_TO_REG_GROUP(idx)) {
//	case 0:
//		reg_base = ESPI_IO_SIZE0;
//		break;
//	case 1:
//		reg_base = ESPI_IO_SIZE1;
//		break;
//	case 2:
//		reg_base = ESPI_IO_SIZE2;
//		break;
//	default: /* case 3 */
//		reg_base = ESPI_IO_SIZE3;
//		break;
//	}
//	return ESPI_IO_RANGE_SIZE_REG(reg_base, ESPI_DECODE_RANGE_TO_REG_OFFSET(idx));
}

static inline unsigned int espi_mmio_range_base_reg(unsigned int idx)
{
	asm("j .");
	return 0;
//	unsigned int reg_base;
//	if (ESPI_DECODE_RANGE_TO_REG_GROUP(idx) == 0)
//		reg_base = ESPI_MMIO_BASE_REG0;
//	else
//		reg_base = ESPI_MMIO_BASE_REG4;
//
//	return ESPI_MMIO_RANGE_BASE_REG(reg_base, ESPI_DECODE_RANGE_TO_REG_OFFSET(idx));
}

static inline unsigned int espi_mmio_range_size_reg(unsigned int idx)
{
	asm("j .");
	return 0;
//	unsigned int reg_base;
//	if (ESPI_DECODE_RANGE_TO_REG_GROUP(idx) == 0)
//		reg_base = ESPI_MMIO_SIZE_REG0;
//	else
//		reg_base = ESPI_MMIO_SIZE_REG2;
//
//	return ESPI_MMIO_RANGE_SIZE_REG(reg_base, ESPI_DECODE_RANGE_TO_REG_OFFSET(idx));
}

static void espi_enable_decode(uint32_t decode_en)
{
	uint32_t val;

	val = espi_read32(ESPI_DECODE);
	val |= decode_en;
	espi_write32(val, ESPI_DECODE);
}

static bool espi_is_decode_enabled(uint32_t decode)
{
	uint32_t val;

	val = espi_read32(ESPI_DECODE);
	return !!(val & decode);
}

static int espi_find_io_window(uint16_t win_base)
{
	int i;

	for (i = 0; i < ESPI_GENERIC_IO_WIN_COUNT; i++) {
		if (!espi_is_decode_enabled(espi_decode_io_range_en_bit(i)))
			continue;

		if (espi_read16(espi_io_range_base_reg(i)) == win_base)
			return i;
	}

	return -1;
}

static int espi_get_unused_io_window(void)
{
	int i;

	for (i = 0; i < ESPI_GENERIC_IO_WIN_COUNT; i++) {
		if (!espi_is_decode_enabled(espi_decode_io_range_en_bit(i)))
			return i;
	}

	return -1;
}

static void espi_clear_decodes(void)
{
	unsigned int idx;

	/* First turn off all enable bits, then zero base, range, and size registers */
	espi_write16((espi_read16(ESPI_DECODE) & ESPI_DECODE_IO_0x80_EN), ESPI_DECODE);

	for (idx = 0; idx < ESPI_GENERIC_IO_WIN_COUNT; idx++) {
		espi_write16(0, espi_io_range_base_reg(idx));
		espi_write8(0, espi_io_range_size_reg(idx));
	}
	for (idx = 0; idx < ESPI_GENERIC_MMIO_WIN_COUNT; idx++) {
		espi_write32(0, espi_mmio_range_base_reg(idx));
		espi_write16(0, espi_mmio_range_size_reg(idx));
	}
}

/*
 * Returns decode enable bits for standard IO port addresses. If port address is not supported
 * by standard decode or if the size of window is not 1, then it returns -1.
 */
static int espi_std_io_decode(uint16_t base, size_t size)
{
	if (size == 2 && base == 0x2e)
		return ESPI_DECODE_IO_0X2E_0X2F_EN;

	if (size != 1)
		return -1;

	switch (base) {
	case 0x80:
		return ESPI_DECODE_IO_0x80_EN;
	case 0x60:
	case 0x64:
		return ESPI_DECODE_IO_0X60_0X64_EN;
	case 0x2e:
	case 0x2f:
		return ESPI_DECODE_IO_0X2E_0X2F_EN;
	default:
		return -1;
	}
}

static size_t espi_get_io_window_size(int idx)
{
	return espi_read8(espi_io_range_size_reg(idx)) + 1;
}

static void espi_write_io_window(int idx, uint16_t base, size_t size)
{
	espi_write16(base, espi_io_range_base_reg(idx));
	espi_write8(size - 1, espi_io_range_size_reg(idx));
}

static int espi_open_generic_io_window(uint16_t base, size_t size)
{
	size_t win_size;
	int idx;

	for (; size; size -= win_size, base += win_size) {
		win_size = MIN(size, ESPI_GENERIC_IO_MAX_WIN_SIZE);

		idx = espi_find_io_window(base);
		if (idx != -1) {
			size_t curr_size = espi_get_io_window_size(idx);

			if (curr_size > win_size) {
				printf("eSPI window already configured to be larger than requested! ");
				printf("Base: 0x%x, Requested size: 0x%zx, Actual size: 0x%zx\n",
				       base, win_size, curr_size);
			} else if (curr_size < win_size) {
				espi_write_io_window(idx, base, win_size);
				printf("eSPI window at base: 0x%x resized from 0x%zx to 0x%zx\n",
				       base, curr_size, win_size);
			}

			continue;
		}

		idx = espi_get_unused_io_window();
		if (idx == -1) {
			printf("Cannot open IO window base %x size %zx\n", base,
			       size);
			printf("No more available IO windows!\n");
			return -1;
		}

		espi_write_io_window(idx, base, win_size);
		espi_enable_decode(espi_decode_io_range_en_bit(idx));
	}

	return 0;
}

int espi_open_io_window(uint16_t base, size_t size)
{
	int std_io;

	std_io = espi_std_io_decode(base, size);
	if (std_io != -1) {
		espi_enable_decode(std_io);
		return 0;
	} else {
		return espi_open_generic_io_window(base, size);
	}
}

static int espi_find_mmio_window(uint32_t win_base)
{
	int i;

	for (i = 0; i < ESPI_GENERIC_MMIO_WIN_COUNT; i++) {
		if (!espi_is_decode_enabled(espi_decode_mmio_range_en_bit(i)))
			continue;

		if (espi_read32(espi_mmio_range_base_reg(i)) == win_base)
			return i;
	}

	return -1;
}

static int espi_get_unused_mmio_window(void)
{
	int i;

	for (i = 0; i < ESPI_GENERIC_MMIO_WIN_COUNT; i++) {
		if (!espi_is_decode_enabled(espi_decode_mmio_range_en_bit(i)))
			return i;
	}

	return -1;

}

static size_t espi_get_mmio_window_size(int idx)
{
	return espi_read16(espi_mmio_range_size_reg(idx)) + 1;
}

static void espi_write_mmio_window(int idx, uint32_t base, size_t size)
{
	espi_write32(base, espi_mmio_range_base_reg(idx));
	espi_write16(size - 1, espi_mmio_range_size_reg(idx));
}

int espi_open_mmio_window(uint32_t base, size_t size)
{
	size_t win_size;
	int idx;

	for (; size; size -= win_size, base += win_size) {
		win_size = MIN(size, ESPI_GENERIC_MMIO_MAX_WIN_SIZE);

		idx = espi_find_mmio_window(base);
		if (idx != -1) {
			size_t curr_size = espi_get_mmio_window_size(idx);

			if (curr_size > win_size) {
				printf("eSPI window already configured to be larger than requested! ");
				printf("Base: 0x%x, Requested size: 0x%zx, Actual size: 0x%zx\n",
				       base, win_size, curr_size);
			} else if (curr_size < win_size) {
				espi_write_mmio_window(idx, base, win_size);
				printf("eSPI window at base: 0x%x resized from 0x%zx to 0x%zx\n",
				       base, curr_size, win_size);
			}

			continue;
		}

		idx = espi_get_unused_mmio_window();
		if (idx == -1) {
			printf("Cannot open IO window base %x size %zx\n", base,
			       size);
			printf("No more available MMIO windows!\n");
			return -1;
		}

		espi_write_mmio_window(idx, base, win_size);
		espi_enable_decode(espi_decode_mmio_range_en_bit(idx));
	}

	return 0;
}

static int espi_configure_decodes(const struct espi_config *cfg)
{
	int i;

	espi_enable_decode(cfg->std_io_decode_bitmap);

	for (i = 0; i < ESPI_GENERIC_IO_WIN_COUNT; i++) {
		if (cfg->generic_io_range[i].size == 0)
			continue;
		if (espi_open_generic_io_window(cfg->generic_io_range[i].base,
						cfg->generic_io_range[i].size) != 0)
			return -1;
	}

	return 0;
}

enum espi_cmd_type {
	CMD_TYPE_SET_CONFIGURATION = 0,
	CMD_TYPE_GET_CONFIGURATION = 1,
	CMD_TYPE_IN_BAND_RESET = 2,
	CMD_TYPE_PERIPHERAL = 4,
	CMD_TYPE_VW = 5,
	CMD_TYPE_OOB = 6,
	CMD_TYPE_FLASH = 7,
};

#define ESPI_CMD_TIMEOUT_US			100
#define ESPI_CH_READY_TIMEOUT_US		10000

union espi_txhdr0 {
	uint32_t val;
	struct  {
		uint32_t cmd_type:3;
		uint32_t cmd_sts:1;
		uint32_t slave_sel:2;
		uint32_t rsvd:2;
		uint32_t hdata0:8;
		uint32_t hdata1:8;
		uint32_t hdata2:8;
	};
} __packed;

union espi_txhdr1 {
	uint32_t val;
	struct {
		uint32_t hdata3:8;
		uint32_t hdata4:8;
		uint32_t hdata5:8;
		uint32_t hdata6:8;
	};
} __packed;

union espi_txhdr2 {
	uint32_t val;
	struct {
		uint32_t hdata7:8;
		uint32_t rsvd:24;
	};
} __packed;

union espi_txdata {
	uint32_t val;
	struct {
		uint32_t byte0:8;
		uint32_t byte1:8;
		uint32_t byte2:8;
		uint32_t byte3:8;
	};
} __packed;

struct espi_cmd {
	union espi_txhdr0 hdr0;
	union espi_txhdr1 hdr1;
	union espi_txhdr2 hdr2;
	union espi_txdata data;
	uint32_t expected_status_codes;
} __packed;

/* Wait up to ESPI_CMD_TIMEOUT_US for hardware to clear DNCMD_STATUS bit. */
static int espi_wait_ready(void)
{
	union espi_txhdr0 hdr0;
	int count = 1000;

	do {
		hdr0.val = espi_read32(ESPI_DN_TXHDR_0);
		if (!hdr0.cmd_sts)
			return 0;
	} while (count--);

	return -1;
}

/* Clear interrupt status register */
static void espi_clear_status(void)
{
	uint32_t status = espi_read32(ESPI_SLAVE0_INT_STS);
	if (status)
		espi_write32(status, ESPI_SLAVE0_INT_STS);
}

/*
 * Wait up to ESPI_CMD_TIMEOUT_US for interrupt status register to update after sending a
 * command.
 */
static int espi_poll_status(uint32_t *status)
{
	int count = 1000;

	do {
		*status = espi_read32(ESPI_SLAVE0_INT_STS);
		if (*status)
			return 0;
	} while (count--);

	printf("eSPI timed out waiting for status update.\n");

	return -1;
}

static void espi_show_failure(const struct espi_cmd *cmd, const char *str, uint32_t status)
{
	printf("eSPI cmd0-cmd2: %08x %08x %08x data: %08x.\n",
	       cmd->hdr0.val, cmd->hdr1.val, cmd->hdr2.val, cmd->data.val);
	printf("%s (Status = 0x%x)\n", str, status);
}

static int espi_send_command(const struct espi_cmd *cmd)
{
	uint32_t status;

#ifdef CONFIG_ESPI_DEBUG
	printf("eSPI cmd0-cmd2: %08x %08x %08x data: %08x.\n",
	       cmd->hdr0.val, cmd->hdr1.val, cmd->hdr2.val, cmd->data.val);
#endif

	if (espi_wait_ready() != 0) {
		espi_show_failure(cmd, "Error: eSPI was not ready to accept a command", 0);
		return -1;
	}

	espi_clear_status();

	espi_write32(cmd->hdr1.val, ESPI_DN_TXHDR_1);
	espi_write32(cmd->hdr2.val, ESPI_DN_TXHDR_2);
	espi_write32(cmd->data.val, ESPI_DN_TXDATA_PORT);

	/* Dword 0 must be last as this write triggers the transaction */
	espi_write32(cmd->hdr0.val, ESPI_DN_TXHDR_0);

	if (espi_wait_ready() != 0) {
		espi_show_failure(cmd,
				  "Error: eSPI timed out waiting for command to complete", 0);
		return -1;
	}

	if (espi_poll_status(&status) != 0) {
		espi_show_failure(cmd, "Error: eSPI poll status failed", 0);
		return -1;
	}

	/* If command did not complete downstream, return error. */
	if (!(status & SLAVE0_INT_STS_DNCMD_INT)) {
		espi_show_failure(cmd, "Error: eSPI downstream command completion failure",
				  status);
		return -1;
	}

	if (status & ~(SLAVE0_INT_STS_DNCMD_INT | cmd->expected_status_codes)) {
		espi_show_failure(cmd, "Error: unexpected eSPI status register bits set",
				  status);
		return -1;
	}

	espi_write32(status, ESPI_SLAVE0_INT_STS);

	return 0;
}

static int espi_send_reset(void)
{
	struct espi_cmd cmd = {
		.hdr0 = {
			.cmd_type = CMD_TYPE_IN_BAND_RESET,
			.cmd_sts = 1,
		},

		/*
		 * When performing an in-band reset the host controller and the
		 * peripheral can have mismatched IO configs.
		 *
		 * i.e., The eSPI peripheral can be in IO-4 mode while, the
		 * eSPI host will be in IO-1. This results in the peripheral
		 * getting invalid packets and thus not responding.
		 *
		 * If the peripheral is alerting when we perform an in-band
		 * reset, there is a race condition in espi_send_command.
		 * 1) espi_send_command clears the interrupt status.
		 * 2) eSPI host controller hardware notices the alert and sends
		 *    a GET_STATUS.
		 * 3) espi_send_command writes the in-band reset command.
		 * 4) eSPI hardware enqueues the in-band reset until GET_STATUS
		 *    is complete.
		 * 5) GET_STATUS fails with NO_RESPONSE and sets the interrupt
		 *    status.
		 * 6) eSPI hardware performs in-band reset.
		 * 7) espi_send_command checks the status and sees a
		 *    NO_RESPONSE bit.
		 *
		 * As a workaround we allow the NO_RESPONSE status code when
		 * we perform an in-band reset.
		 */
		.expected_status_codes = SLAVE0_INT_STS_NO_RSP_INT,
	};

	return espi_send_command(&cmd);
}

static int espi_send_pltrst(const struct espi_config *mb_cfg, bool assert)
{
	struct espi_cmd cmd = {
		.hdr0 = {
			.cmd_type = CMD_TYPE_VW,
			.cmd_sts = 1,
			.hdata0 = 0, /* 1 VW group */
		},
		.data = {
			.byte0 = ESPI_VW_INDEX_SYSTEM_EVENT_3,
			.byte1 = assert ? ESPI_VW_SIGNAL_LOW(ESPI_VW_PLTRST)
					: ESPI_VW_SIGNAL_HIGH(ESPI_VW_PLTRST),
		},
	};

	if (!mb_cfg->vw_ch_en)
		return 0;

	return espi_send_command(&cmd);
}

/*
 * In case of get configuration command, hdata0 contains bits 15:8 of the slave register address
 * and hdata1 contains bits 7:0 of the slave register address.
 */
#define ESPI_CONFIGURATION_HDATA0(a)		(((a) >> 8) & 0xff)
#define ESPI_CONFIGURATION_HDATA1(a)		((a) & 0xff)

static int espi_get_configuration(uint16_t slave_reg_addr, uint32_t *config)
{
	struct espi_cmd cmd = {
		.hdr0 = {
			.cmd_type = CMD_TYPE_GET_CONFIGURATION,
			.cmd_sts = 1,
			.hdata0 = ESPI_CONFIGURATION_HDATA0(slave_reg_addr),
			.hdata1 = ESPI_CONFIGURATION_HDATA1(slave_reg_addr),
		},
	};

	*config = 0;

	if (espi_send_command(&cmd) != 0)
		return -1;

	*config = espi_read32(ESPI_DN_TXHDR_1);

#ifdef CONFIG_ESPI_DEBUG
	printf("Get configuration for slave register(0x%x): 0x%x\n",
	       slave_reg_addr, *config);
#endif

	return 0;
}

static int espi_set_configuration(uint16_t slave_reg_addr, uint32_t config)
{
	struct espi_cmd cmd = {
		.hdr0 = {
			.cmd_type = CMD_TYPE_SET_CONFIGURATION,
			.cmd_sts = 1,
			.hdata0 = ESPI_CONFIGURATION_HDATA0(slave_reg_addr),
			.hdata1 = ESPI_CONFIGURATION_HDATA1(slave_reg_addr),
		},
		.hdr1 = {
			 .val = config,
		},
	};

	return espi_send_command(&cmd);
}

static int espi_get_general_configuration(uint32_t *config)
{
	if (espi_get_configuration(ESPI_SLAVE_GENERAL_CFG, config) != 0)
		return -1;

	espi_show_slave_general_configuration(*config);
	return 0;
}

static int espi_set_io_mode_cfg(enum espi_io_mode mb_io_mode, uint32_t slave_caps,
					uint32_t *slave_config, uint32_t *ctrlr_config)
{
	switch (mb_io_mode) {
	case ESPI_IO_MODE_QUAD:
		if (espi_slave_supports_quad_io(slave_caps)) {
			*slave_config |= ESPI_SLAVE_IO_MODE_SEL_QUAD;
			*ctrlr_config |= ESPI_IO_MODE_QUAD;
			break;
		}
		printf("eSPI Quad I/O not supported. Dropping to dual mode.\n");
	case ESPI_IO_MODE_DUAL:
		if (espi_slave_supports_dual_io(slave_caps)) {
			*slave_config |= ESPI_SLAVE_IO_MODE_SEL_DUAL;
			*ctrlr_config |= ESPI_IO_MODE_DUAL;
			break;
		}
		printf("eSPI Dual I/O not supported. Dropping to single mode.\n");
	case ESPI_IO_MODE_SINGLE:
		/* Single I/O mode is always supported. */
		*slave_config |= ESPI_SLAVE_IO_MODE_SEL_SINGLE;
		*ctrlr_config |= ESPI_IO_MODE_SINGLE;
		break;
	default:
		printf("No supported eSPI I/O modes!\n");
		return -1;
	}
	return 0;
}

static int espi_set_op_freq_cfg(enum espi_op_freq mb_op_freq, uint32_t slave_caps,
					uint32_t *slave_config, uint32_t *ctrlr_config)
{
	int slave_max_speed_mhz = espi_slave_max_speed_mhz_supported(slave_caps);

	switch (mb_op_freq) {
	case ESPI_OP_FREQ_66_MHZ:
		if (slave_max_speed_mhz >= 66) {
			*slave_config |= ESPI_SLAVE_OP_FREQ_SEL_66_MHZ;
			*ctrlr_config |= ESPI_OP_FREQ_66_MHZ;
			break;
		}
	case ESPI_OP_FREQ_50_MHZ:
		if (slave_max_speed_mhz >= 50) {
			*slave_config |= ESPI_SLAVE_OP_FREQ_SEL_50_MHZ;
			*ctrlr_config |= ESPI_OP_FREQ_50_MHZ;
			break;
		}
	case ESPI_OP_FREQ_33_MHZ:
		if (slave_max_speed_mhz >= 33) {
			*slave_config |= ESPI_SLAVE_OP_FREQ_SEL_33_MHZ;
			*ctrlr_config |= ESPI_OP_FREQ_33_MHZ;
			break;
		}
	case ESPI_OP_FREQ_25_MHZ:
		if (slave_max_speed_mhz > 0) {
			*slave_config |= ESPI_SLAVE_OP_FREQ_SEL_25_MHZ;
			*ctrlr_config |= ESPI_OP_FREQ_25_MHZ;
			break;
		}
	case ESPI_OP_FREQ_20_MHZ:
		if (slave_max_speed_mhz > 0) {
			*slave_config |= ESPI_SLAVE_OP_FREQ_SEL_20_MHZ;
			*ctrlr_config |= ESPI_OP_FREQ_20_MHZ;
			break;
		}
	default:
		printf("No supported eSPI Operating Frequency!\n");
		return -1;
	}
	return 0;
}

static int espi_set_alert_pin_cfg(enum espi_alert_pin alert_pin, uint32_t slave_caps,
					  uint32_t *slave_config, uint32_t *ctrlr_config)
{
	switch (alert_pin) {
	case ESPI_ALERT_PIN_IN_BAND:
		*slave_config |= ESPI_SLAVE_ALERT_MODE_IO1;
		return 0;
	case ESPI_ALERT_PIN_PUSH_PULL:
		*slave_config |= ESPI_SLAVE_ALERT_MODE_PIN | ESPI_SLAVE_PUSH_PULL_ALERT_SEL;
		*ctrlr_config |= ESPI_ALERT_MODE;
		return 0;
	case ESPI_ALERT_PIN_OPEN_DRAIN:
		if (!(slave_caps & ESPI_SLAVE_OPEN_DRAIN_ALERT_SUPP)) {
			printf("eSPI peripheral does not support open drain alert!");
			return -1;
		}

		*slave_config |= ESPI_SLAVE_ALERT_MODE_PIN | ESPI_SLAVE_OPEN_DRAIN_ALERT_SEL;
		*ctrlr_config |= ESPI_ALERT_MODE;
		return 0;
	default:
		printf("Unknown espi alert config: %u!\n", alert_pin);
		return -1;
	}
}

static int espi_set_general_configuration(const struct espi_config *mb_cfg,
						  uint32_t slave_caps)
{
	uint32_t slave_config = 0;
	uint32_t ctrlr_config = 0;

	if (mb_cfg->crc_check_en) {
		slave_config |= ESPI_SLAVE_CRC_ENABLE;
		ctrlr_config |= ESPI_CRC_CHECKING_EN;
	}

	if (espi_set_alert_pin_cfg(mb_cfg->alert_pin, slave_caps, &slave_config, &ctrlr_config)
			!= 0)
		return -1;
	if (espi_set_io_mode_cfg(mb_cfg->io_mode, slave_caps, &slave_config, &ctrlr_config)
			!= 0)
		return -1;
	if (espi_set_op_freq_cfg(mb_cfg->op_freq_mhz, slave_caps, &slave_config, &ctrlr_config)
			!= 0)
		return -1;

#ifdef CONFIG_ESPI_DEBUG
	printf("Setting general configuration: slave: 0x%x controller: 0x%x\n",
	       slave_config, ctrlr_config);
#endif

	espi_show_slave_general_configuration(slave_config);

	if (espi_set_configuration(ESPI_SLAVE_GENERAL_CFG, slave_config) != 0)
		return -1;

	espi_write32(ctrlr_config, ESPI_SLAVE0_CONFIG);
	return 0;
}

static int espi_wait_channel_ready(uint16_t slave_reg_addr)
{
	uint32_t config;
	int count = 1000;

	do {
		if (espi_get_configuration(slave_reg_addr, &config) != 0)
			return -1;
		if (espi_slave_is_channel_ready(config))
			return 0;
	} while (count--);

	printf("Channel is not ready after %d usec (slave addr: 0x%x)\n",
	       ESPI_CH_READY_TIMEOUT_US, slave_reg_addr);
	return -1;

}

static void espi_enable_ctrlr_channel(uint32_t channel_en)
{
	uint32_t reg = espi_read32(ESPI_SLAVE0_CONFIG);

	reg |= channel_en;

	espi_write32(reg, ESPI_SLAVE0_CONFIG);
}

static int espi_set_channel_configuration(uint32_t slave_config,
						  uint32_t slave_reg_addr,
						  uint32_t ctrlr_enable)
{
	if (espi_set_configuration(slave_reg_addr, slave_config) != 0)
		return -1;

	if (!(slave_config & ESPI_SLAVE_CHANNEL_ENABLE))
		return 0;

	if (espi_wait_channel_ready(slave_reg_addr) != 0)
		return -1;

	espi_enable_ctrlr_channel(ctrlr_enable);
	return 0;
}

static int espi_setup_vw_channel(const struct espi_config *mb_cfg, uint32_t slave_caps)
{
	uint32_t slave_vw_caps;
	uint32_t ctrlr_vw_caps;
	uint32_t slave_vw_count_supp;
	uint32_t ctrlr_vw_count_supp;
	uint32_t use_vw_count;
	uint32_t slave_config;

	if (!mb_cfg->vw_ch_en)
		return 0;

	if (!espi_slave_supports_vw_channel(slave_caps)) {
		printf("eSPI slave doesn't support VW channel!\n");
		return -1;
	}

	if (espi_get_configuration(ESPI_SLAVE_VW_CFG, &slave_vw_caps) != 0)
		return -1;

	ctrlr_vw_caps = espi_read32(ESPI_MASTER_CAP);
	ctrlr_vw_count_supp = FIELD_GET(MASTER_CAP_VW_MAX_SIZE_MASK, ctrlr_vw_caps);

	slave_vw_count_supp = espi_slave_get_vw_count_supp(slave_vw_caps);
	use_vw_count = MIN(ctrlr_vw_count_supp, slave_vw_count_supp);

	slave_config = ESPI_SLAVE_CHANNEL_ENABLE | ESPI_SLAVE_VW_COUNT_SEL_VAL(use_vw_count);
	return espi_set_channel_configuration(slave_config, ESPI_SLAVE_VW_CFG, ESPI_VW_CH_EN);
}

static int espi_setup_periph_channel(const struct espi_config *mb_cfg,
					     uint32_t slave_caps)
{
	uint32_t slave_config;
	/* Peripheral channel requires BME bit to be set when enabling the channel. */
	const uint32_t slave_en_mask =
		ESPI_SLAVE_CHANNEL_ENABLE | ESPI_SLAVE_PERIPH_BUS_MASTER_ENABLE;

	if (espi_get_configuration(ESPI_SLAVE_PERIPH_CFG, &slave_config) != 0)
		return -1;

	/*
	 * Peripheral channel is the only one which is enabled on reset. So, if the mainboard
	 * wants to disable it, set configuration to disable peripheral channel. It also
	 * requires that BME bit be cleared.
	 */
	if (mb_cfg->periph_ch_en) {
		if (!espi_slave_supports_periph_channel(slave_caps)) {
			printf("eSPI slave doesn't support periph channel!\n");
			return -1;
		}
		slave_config |= slave_en_mask;
	} else {
		slave_config &= ~slave_en_mask;
	}

	espi_show_slave_peripheral_channel_configuration(slave_config);

	return espi_set_channel_configuration(slave_config, ESPI_SLAVE_PERIPH_CFG,
					      ESPI_PERIPH_CH_EN);
}

static int espi_setup_pr_base_addr_mem0(uint32_t base)
{
	espi_write32(base, ESPI_PR_BASE_ADDR_MEM0);

	return 0;
}

static int espi_setup_pr_base_addr_mem1(uint32_t base)
{
	espi_write32(base, ESPI_PR_BASE_ADDR_MEM1);

	return 0;
}

static int espi_setup_oob_channel(const struct espi_config *mb_cfg, uint32_t slave_caps)
{
	uint32_t slave_config;

	if (!mb_cfg->oob_ch_en)
		return 0;

	if (!espi_slave_supports_oob_channel(slave_caps)) {
		printf("eSPI slave doesn't support OOB channel!\n");
		return -1;
	}

	if (espi_get_configuration(ESPI_SLAVE_OOB_CFG, &slave_config) != 0)
		return -1;

	slave_config |= ESPI_SLAVE_CHANNEL_ENABLE;

	return espi_set_channel_configuration(slave_config, ESPI_SLAVE_OOB_CFG,
					      ESPI_OOB_CH_EN);
}

static int espi_setup_flash_channel(const struct espi_config *mb_cfg,
					    uint32_t slave_caps)
{
	uint32_t slave_config;

	if (!mb_cfg->flash_ch_en)
		return 0;

	if (!espi_slave_supports_flash_channel(slave_caps)) {
		printf("eSPI slave doesn't support flash channel!\n");
		return -1;
	}

	if (espi_get_configuration(ESPI_SLAVE_FLASH_CFG, &slave_config) != 0)
		return -1;

	slave_config |= ESPI_SLAVE_CHANNEL_ENABLE;

	return espi_set_channel_configuration(slave_config, ESPI_SLAVE_FLASH_CFG,
					      ESPI_FLASH_CH_EN);
}

/*
 * mode: 1x
 * freq: 16MHz
 * slect alert mode
 */
static int espi_set_initial_config(const struct espi_config *mb_cfg)
{
	uint32_t io_mode = SLAVE0_CONFIG_IO_MODE_SEL(ESPI_IO_MODE_SINGLE);
	uint32_t freq = SLAVE0_CONFIG_CLK_FREQ_SEL(ESPI_OP_FREQ_20_MHZ);
	uint32_t espi_initial_mode = freq | io_mode;

	switch (mb_cfg->alert_pin) {
	case ESPI_ALERT_PIN_IN_BAND:
		break;
	case ESPI_ALERT_PIN_PUSH_PULL:
	case ESPI_ALERT_PIN_OPEN_DRAIN:
		espi_initial_mode |= SLAVE0_CONFIG_ALERT_MODE_SEL;
		break;
	default:
		printf("Unknown espi alert config: %u!\n", mb_cfg->alert_pin);
		return -1;
	}

	espi_write32(espi_initial_mode, ESPI_SLAVE0_CONFIG);
	return 0;
}

static void espi_setup_subtractive_decode(const struct espi_config *mb_cfg)
{
//	uint32_t global_ctrl_reg;
//	global_ctrl_reg = espi_read32(ESPI_GLOBAL_CONTROL_1);
//
//	if (mb_cfg->subtractive_decode) {
//		global_ctrl_reg &= ~ESPI_SUB_DECODE_SLV_MASK;
//		global_ctrl_reg |= ESPI_SUB_DECODE_EN;
//	} else {
//		global_ctrl_reg &= ~ESPI_SUB_DECODE_EN;
//	}
//	espi_write32(ESPI_GLOBAL_CONTROL_1, global_ctrl_reg);
}

static void espi_enable_all_irqs(void)
{
	espi_write32(SLAVE0_CONFIG_FLASH_REQ_INT_EN |
		     SLAVE0_INT_EN_RXOOB_INT_EN |
		     SLAVE0_INT_EN_RXMSG_INT_EN |
		     SLAVE0_INT_EN_DNCMD_INT_EN |
		     SLAVE0_INT_EN_RXVW_GPR3_INT_EN |
		     SLAVE0_INT_EN_RXVW_GPR2_INT_EN |
		     SLAVE0_INT_EN_RXVW_GPR1_INT_EN |
		     SLAVE0_INT_EN_RXVW_GPR0_INT_EN |
		     SLAVE0_INT_EN_PR_INT_EN |
		     SLAVE0_INT_EN_PROTOCOL_ERR_INT_EN |
		     SLAVE0_INT_EN_RXFLASH_OFLOW_INT_EN |
		     SLAVE0_INT_EN_RXMSG_OFLOW_INT_EN |
		     SLAVE0_INT_EN_RXOOB_OFLOW_INT_EN |
		     SLAVE0_INT_EN_ILLEGAL_LEN_INT_EN |
		     SLAVE0_INT_EN_ILLEGAL_TAG_INT_EN |
		     SLAVE0_INT_EN_UNSUCSS_CPL_INT_EN |
		     SLAVE0_INT_EN_INVALID_CT_RSP_INT_EN |
		     SLAVE0_INT_EN_INVALID_ID_RSP_INT_EN |
		     SLAVE0_INT_EN_NON_FATAL_INT_EN |
		     SLAVE0_INT_EN_FATAL_ERR_INT_EN |
		     SLAVE0_INT_EN_NO_RSP_INT_EN |
		     SLAVE0_INT_EN_CRC_ERR_INT_EN |
		     SLAVE0_INT_EN_WAIT_TIMEOUT_INT_EN |
		     SLAVE0_INT_EN_BUS_ERR_INT_EN,
		ESPI_SLAVE0_INT_EN
	);
}

static void espi_irq_init(void)
{
	irqc_configure_irq(100, 0, IRQ_LEVEL_TRIGGERED);
	irq_register_vector(100, espi_handle_irq);
	irqc_enable_irq(100);
	espi_enable_all_irqs();
}

int espi_hw_ctrl_init(struct espi_config *cfg)
{
	uint32_t slave_caps;
	struct espi_config def_cfg;

	printf("Initializing eSPI.\n");

	if (cfg == NULL) {
		def_cfg.std_io_decode_bitmap = ESPI_DECODE_IO_0x80_EN | ESPI_DECODE_IO_0X2E_0X2F_EN | ESPI_DECODE_IO_0X60_0X64_EN,
		def_cfg.io_mode = ESPI_IO_MODE_QUAD,
		def_cfg.op_freq_mhz = ESPI_OP_FREQ_20_MHZ,
		def_cfg.crc_check_en = 1,
		def_cfg.alert_pin = ESPI_ALERT_PIN_PUSH_PULL,
		def_cfg.periph_ch_en = 1,
		def_cfg.vw_ch_en = 1,
		def_cfg.oob_ch_en = 1,
		def_cfg.flash_ch_en = 0,

		cfg = &def_cfg;
	}

	espi_write32(GLOBAL_CONTROL_0_MST_STOP_EN, ESPI_GLOBAL_CONTROL_0);
//	espi_write32(ESPI_RGCMD_INT(23) | ESPI_ERR_INT_SMI, ESPI_GLOBAL_CONTROL_1);
	espi_write32(0, ESPI_SLAVE0_INT_EN);
	espi_clear_status();
	espi_clear_decodes();

	/*
	 * Boot sequence: Step 1
	 * Set correct initial configuration to talk to the slave:
	 * Set clock frequency to 16.7MHz and single IO mode.
	 */
	if (espi_set_initial_config(cfg) != 0)
		return -1;

	/*
	 * Boot sequence: Step 2
	 * Send in-band reset
	 * The resets affects both host and slave devices, so set initial config again.
	 */
	if (espi_send_reset() != 0) {
		printf("In-band reset failed!\n");
		return -1;
	}

	if (espi_set_initial_config(cfg) != 0)
		return -1;

	/*
	 * Boot sequence: Step 3
	 * Get configuration of slave device.
	 */
	if (espi_get_general_configuration(&slave_caps) != 0) {
		printf("Slave GET_CONFIGURATION failed!\n");
		return -1;
	}

	/*
	 * Boot sequence:
	 * Step 4: Write slave device general config
	 * Step 5: Set host slave config
	 */
	if (espi_set_general_configuration(cfg, slave_caps) != 0) {
		printf("Slave SET_CONFIGURATION failed!\n");
		return -1;
	}

	/*
	 * Setup polarity before enabling the VW channel so any interrupts
	 * received will have the correct polarity.
	 */
	espi_write32(cfg->vw_irq_polarity, ESPI_SLAVE0_VW_POLARITY);

	/*
	 * Boot Sequences: Steps 6 - 9
	 * Channel setup
	 */
	/* Set up VW first so we can deassert PLTRST#. */
	if (espi_setup_vw_channel(cfg, slave_caps) != 0) {
		printf("Setup VW channel failed!\n");
		return -1;
	}

	/* Assert PLTRST# if VW channel is enabled by mainboard. */
	if (espi_send_pltrst(cfg, true) != 0) {
		printf("PLTRST# assertion failed!\n");
		return -1;
	}

	/* De-assert PLTRST# if VW channel is enabled by mainboard. */
	if (espi_send_pltrst(cfg, false) != 0) {
		printf("PLTRST# deassertion failed!\n");
		return -1;
	}

	if (espi_setup_periph_channel(cfg, slave_caps) != 0) {
		printf("Setup Periph channel failed!\n");
		return -1;
	}

	if (espi_setup_pr_base_addr_mem0(SPACEMIT_ESPI_PR_MEM0)) {
		printf("Setup Periph channel mem0 failed!\n");
		return -1;
	}

	if (espi_setup_pr_base_addr_mem1(SPACEMIT_ESPI_PR_MEM1)) {
		printf("Setup Periph channel mem1 failed!\n");
		return -1;
	}

	if (espi_setup_oob_channel(cfg, slave_caps) != 0) {
		printf("Setup OOB channel failed!\n");
		return -1;
	}

	if (espi_setup_flash_channel(cfg, slave_caps) != 0) {
		printf("Setup Flash channel failed!\n");
		return -1;
	}

	if (espi_configure_decodes(cfg) != 0) {
		printf("Configuring decodes failed!\n");
		return -1;
	}

	/* Enable subtractive decode if configured */
	espi_setup_subtractive_decode(cfg);

//	ctrl = espi_read32(ESPI_GLOBAL_CONTROL_1);
//	ctrl |= ESPI_BUS_MASTER_EN;
//
//	ctrl |= ESPI_ALERT_ENABLE;
//
//	espi_write32(ctrl, ESPI_GLOBAL_CONTROL_1);

	espi_irq_init();

	espi_bh_create();

	printf("Finished initializing eSPI.\n");

	return 0;
}

int espi_send_vw(uint8_t *ids, uint8_t *data, int num)
{
	int i;
	uint32_t status;
	uint32_t val = 0;

	if (num > 64 || num < 1) {
		return -1;
	}

	for (i = 0; i < num; i++) {
		val |= ids[i] << ((i % 4) << 1) | data[i] << (((i % 4) << 1) + 1);
		if ((i % 2) == 1) {
			espi_write32(val, ESPI_DN_TXDATA_PORT);
			val = 0;
		}
	}

	if ((i % 2) == 0) {
		espi_write32(val, ESPI_DN_TXDATA_PORT);
	}

	espi_write32(DN_TXHDR_0_DNCMD_TYPE(CMD_TYPE_VW) |
		DN_TXHDR_0_DNCMD_EN | DN_TXHDR_0_DN_TXHDR_0_SLAVE_SEL(0) |
		DN_TXHDR_0_DNCMD_HDATA0(num), ESPI_DN_TXHDR_0);

	if (espi_wait_ready() != 0) {
		return -1;
	}

	if (espi_poll_status(&status) != 0) {
		return -1;
	}

	/* If command did not complete downstream, return error. */
	if (!(status & SLAVE0_INT_STS_DNCMD_INT)) {
		return -1;
	}

//	if (status & ~(SLAVE0_INT_STS_DNCMD_INT | cmd->expected_status_codes)) {
//		return -1;
//	}

	return 0;
}

/* eSPI tag + len[11:8] field */
#define ESPI_TAG_LEN_FIELD(tag, len) \
		((((tag) & 0xF) << 4) | (((len) >> 8) & 0xF))
int espi_send_oob_smbus(uint8_t *buf, int len)
{
	uint32_t status;
	uint32_t data[16];

	uint32_t val = *(uint32_t *)buf | 0x00FFFFFFU;

	espi_write32(val, ESPI_DN_TXHDR_1);

	memset(&data, 0, 64);
	memcpy(&data, &buf[3], len - 3);

	for (int i = 0; i < (len - 3 + 3) / 4; i++) {
		espi_write32(data[i], ESPI_DN_TXDATA_PORT);
	}

	espi_write32(DN_TXHDR_0_DNCMD_TYPE(CMD_TYPE_OOB) |
		DN_TXHDR_0_DNCMD_EN | DN_TXHDR_0_DN_TXHDR_0_SLAVE_SEL(0) |
		DN_TXHDR_0_DNCMD_HDATA0(ESPI_CYCLE_TYPE_OOB_MESSAGE) |
		DN_TXHDR_0_DNCMD_HDATA1(ESPI_TAG_LEN_FIELD(0, len)) |
		DN_TXHDR_0_DNCMD_HDATA2(len & 0xFFU), ESPI_DN_TXHDR_0);

	if (espi_wait_ready() != 0) {
		return -1;
	}

	if (espi_poll_status(&status) != 0) {
		return -1;
	}

	/* If command did not complete downstream, return error. */
	if (!(status & SLAVE0_INT_STS_DNCMD_INT)) {
		return -1;
	}

//	if (status & ~(SLAVE0_INT_STS_DNCMD_INT | cmd->expected_status_codes)) {
//		return -1;
//	}

	return 0;
}

static int espi_receive_oob_smbus(uint8_t *buf)
{
	int len = 0;
	uint32_t data[16];
	uint32_t rxhdr0 = espi_read32(ESPI_UP_RXHDR_0);

	len = FIELD_GET(UP_RXHDR_0_UPCMD_HDATA2_MASK, rxhdr0);

	*(uint32_t *)buf = espi_read32(ESPI_UP_RXHDR_1) & 0x00FFFFFFU;

	for (int i = 0; i < (len - 3) / 4; i++) {
		data[i] = espi_read32(ESPI_UP_RXDATA_PORT);
	}

	memcpy(&buf[3], &data, len - 3);

	return len;
}

void espi_handle_irq(irq_t irq)
{
	int int_sts;
	uint8_t rxvw_data;

	int_sts = espi_read32(ESPI_SLAVE0_INT_STS);

	espi_write32(int_sts, ESPI_SLAVE0_INT_STS);

	switch (int_sts) {
	case SLAVE0_INT_STS_RXVW_GRP0_INT:
		rxvw_data = espi_read32(ESPI_SLAVE0_RXVW_DATA) & 0xFFU;
		rxvw_callback(128, rxvw_data);
		break;
	case SLAVE0_INT_STS_RXVW_GRP1_INT:
		rxvw_data = (espi_read32(ESPI_SLAVE0_RXVW_DATA) & 0xFF00U) >> 8;
		rxvw_callback(129, rxvw_data);
		break;
	case SLAVE0_INT_STS_RXVW_GRP2_INT:
		rxvw_data = (espi_read32(ESPI_SLAVE0_RXVW_DATA) & 0xFF0000U) >> 8;
		rxvw_callback(130, rxvw_data);
		break;
	case SLAVE0_INT_STS_RXVW_GRP3_INT:
		rxvw_data = (espi_read32(ESPI_SLAVE0_RXVW_DATA) & 0xFF000000U) >> 8;
		rxvw_callback(131, rxvw_data);
		break;
	case SLAVE0_INT_STS_RXOOB_INT:
		int len = espi_receive_oob_smbus((uint8_t *)rxoob_buffer);
		rxoob_callback(rxoob_buffer, len);
	default:
		printf("espi irq error\n");
		break;
	}
}

void espi_register_rxvw_callback(void *callback)
{
	rxvw_callback = callback;
}

void espi_register_rxoob_callbcak(void *callback, void *buffer)
{
	rxoob_callback = callback;
	rxoob_buffer = buffer;
}

static void espi_hb_handler(uint8_t event)
{
	espi_handle_irq(0);
}

int espi_hb_create(void)
{
	espi_bh = bh_register_handler(espi_hb_handler);
	irq_register_poller(espi_bh);

	return 0;
}
