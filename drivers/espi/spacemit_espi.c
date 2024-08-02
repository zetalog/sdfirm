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
#include <target/espi.h>
#include <target/cmdline.h>
#include <target/irq.h>
#include <target/panic.h>

static void (*rxvw_callback)(int group, uint8_t rxvw_data);
static void (*rxoob_callback)(void *buffer, int len);
static void *rxoob_buffer;
static uint8_t spacemit_espi_rsp;

uint32_t spacemit_espi_read32(caddr_t reg)
{
	uint32_t val;

	val = __raw_readl(reg);
	con_dbg("spacemit_espi: R %016lx=%08x\n", reg, val);
	return val;
}

void spacemit_espi_write32(uint32_t val, caddr_t reg)
{
	con_dbg("spacemit_espi: W %016lx=%08x\n", reg, val);
	__raw_writel(val, reg);
}

static inline void espi_clear_upcmd_status(void)
{
	spacemit_espi_clear32(ESPI_UPCMD_STATUS, ESPI_UP_RXHDR);
}

static inline enum espi_upcmd_type espi_get_upcmd_type(void)
{
	return ESPI_UPCMD_TYPE(spacemit_espi_read32(ESPI_UP_RXHDR));
}

static inline uint32_t espi_get_up_rxdata(void)
{
	return spacemit_espi_read32(ESPI_UP_RXDATA_PORT);
}

static inline uint32_t espi_get_flash_max_size(void)
{
	return ESPI_FLASH_MAX_SIZE(spacemit_espi_read32(ESPI_MASTER_CAP));
}

static inline uint32_t espi_get_oob_max_size(void)
{
	return ESPI_OOB_MAX_SIZE(spacemit_espi_read32(ESPI_MASTER_CAP));
}

static inline uint32_t espi_get_vw_max_size(void)
{
	return ESPI_VW_MAX_SIZE(spacemit_espi_read32(ESPI_MASTER_CAP));
}

static inline uint32_t espi_get_pr_max_size(void)
{
	return ESPI_PR_MAX_SIZE(spacemit_espi_read32(ESPI_MASTER_CAP));
}

static inline void espi_wdg_enable(void)
{
	spacemit_espi_set32(ESPI_WDG_EN, ESPI_GLOBAL_CONTROL_0);
}

static inline void espi_set_wdt_count(uint32_t count)
{
	spacemit_espi_write32_mask(ESPI_WDG_CNT(count),
				   ESPI_WDG_CNT(ESPI_WDG_CNT_MASK),
				   ESPI_GLOBAL_CONTROL_0);
}

static inline void espi_set_master_wait_count(uint32_t count)
{
	spacemit_espi_write32_mask(ESPI_WAIT_CNT(count),
				   ESPI_WAIT_CNT(ESPI_WAIT_CNT_MASK),
				   ESPI_GLOBAL_CONTROL_0);
}

static inline void espi_soft_reset(void)
{
	spacemit_espi_set32(ESPI_SW_RST, ESPI_GLOBAL_CONTROL_1);
}

static inline void espi_vw_channel_slave_suspend_enter(void)
{
	spacemit_espi_set32(ESPI_SUS_STAT, ESPI_GLOBAL_CONTROL_1);
}

static inline void espi_vw_channel_slave_suspend_exit(void)
{
	spacemit_espi_clear32(ESPI_SUS_STAT, ESPI_GLOBAL_CONTROL_1);
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
	spacemit_espi_set32(decode_en, ESPI_DECODE);
}

static bool espi_is_decode_enabled(uint32_t decode)
{
	uint32_t val;

	val = spacemit_espi_read32(ESPI_DECODE);
	return !!(val & decode);
}

static int espi_find_io_window(uint16_t win_base)
{
	int i;

	for (i = 0; i < ESPI_GENERIC_IO_WIN_COUNT; i++) {
		if (!espi_is_decode_enabled(espi_decode_io_range_en_bit(i)))
			continue;

		if (spacemit_espi_read32(espi_io_range_base_reg(i)) == win_base)
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

#if 0
static void espi_clear_decodes(void)
{
	unsigned int idx;

	/* First turn off all enable bits, then zero base, range, and size registers */
	spacemit_espi_write16((spacemit_espi_read16(ESPI_DECODE) & ESPI_DECODE_IO_0x80_EN), ESPI_DECODE);

	for (idx = 0; idx < ESPI_GENERIC_IO_WIN_COUNT; idx++) {
		spacemit_espi_write16(0, espi_io_range_base_reg(idx));
		spacemit_espi_write8(0, espi_io_range_size_reg(idx));
	}
	for (idx = 0; idx < ESPI_GENERIC_MMIO_WIN_COUNT; idx++) {
		spacemit_espi_write32(0, espi_mmio_range_base_reg(idx));
		spacemit_espi_write16(0, espi_mmio_range_size_reg(idx));
	}
}
#endif

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
	return spacemit_espi_read32(espi_io_range_size_reg(idx)) + 1;
}

static void espi_write_io_window(int idx, uint16_t base, size_t size)
{
	spacemit_espi_write32(base, espi_io_range_base_reg(idx));
	spacemit_espi_write32(size - 1, espi_io_range_size_reg(idx));
}

static int espi_open_generic_io_window(uint16_t base, size_t size)
{
	size_t win_size;
	int idx;

	for (; size; size -= win_size, base += win_size) {
		win_size = min(size, ESPI_GENERIC_IO_MAX_WIN_SIZE);

		idx = espi_find_io_window(base);
		if (idx != -1) {
			size_t curr_size = espi_get_io_window_size(idx);

			if (curr_size > win_size) {
				con_err("eSPI: window too large: Base=0x%x, Req=0x%zx, Win=0x%zx\n",
				        base, win_size, curr_size);
			} else if (curr_size < win_size) {
				espi_write_io_window(idx, base, win_size);
				con_log("eSPI window at base: 0x%x resized from 0x%zx to 0x%zx\n",
				       base, curr_size, win_size);
			}

			continue;
		}

		idx = espi_get_unused_io_window();
		if (idx == -1) {
			con_err("Cannot open IO window base %x size %zx\n", base,
			       size);
			con_err("No more available IO windows!\n");
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

		if (spacemit_espi_read32(espi_mmio_range_base_reg(i)) == win_base)
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
	return spacemit_espi_read32(espi_mmio_range_size_reg(idx)) + 1;
}

static void espi_write_mmio_window(int idx, uint32_t base, size_t size)
{
	spacemit_espi_write32(base, espi_mmio_range_base_reg(idx));
	spacemit_espi_write32(size - 1, espi_mmio_range_size_reg(idx));
}

int espi_open_mmio_window(uint32_t base, size_t size)
{
	size_t win_size;
	int idx;

	for (; size; size -= win_size, base += win_size) {
		win_size = min(size, ESPI_GENERIC_MMIO_MAX_WIN_SIZE);

		idx = espi_find_mmio_window(base);
		if (idx != -1) {
			size_t curr_size = espi_get_mmio_window_size(idx);

			if (curr_size > win_size) {
				con_err("eSPI window already configured to be larger than requested! ");
				con_log("Base: 0x%x, Requested size: 0x%zx, Actual size: 0x%zx\n",
				       base, win_size, curr_size);
			} else if (curr_size < win_size) {
				espi_write_mmio_window(idx, base, win_size);
				con_log("eSPI window at base: 0x%x resized from 0x%zx to 0x%zx\n",
				       base, curr_size, win_size);
			}

			continue;
		}

		idx = espi_get_unused_mmio_window();
		if (idx == -1) {
			con_err("Cannot open IO window base %x size %zx\n", base,
			       size);
			con_err("No more available MMIO windows!\n");
			return -1;
		}

		espi_write_mmio_window(idx, base, win_size);
		espi_enable_decode(espi_decode_mmio_range_en_bit(idx));
	}

	return 0;
}
#if 0
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
#endif

/* Wait up to ESPI_CMD_TIMEOUT_US for hardware to clear DNCMD_STATUS bit. */
static int espi_wait_ready(void)
{
	int count = 1000;

	do {
		if (spacemit_espi_write_done())
			return 0;
	} while (count--);
	return -EAGAIN;
}

/* Clear interrupt status register */
static void espi_clear_status(void)
{
	uint32_t status = spacemit_espi_read32(ESPI_SLAVE0_INT_STS);
	if (status)
		spacemit_espi_write32(status, ESPI_SLAVE0_INT_STS);
}

/*
 * Wait up to ESPI_CMD_TIMEOUT_US for interrupt status register to update after sending a
 * command.
 */
static int espi_poll_status(uint32_t *status)
{
	int count = 1000;

	do {
		*status = spacemit_espi_read32(ESPI_SLAVE0_INT_STS);
		if (*status)
			return 0;
	} while (count--);

	con_err("eSPI timed out waiting for status update.\n");

	return -1;
}

static int espi_send_command(const struct espi_cmd *cmd)
{
	uint32_t status;

	con_dbg("espi: cmd0-cmd2: hdr=%08x %08x %08x data=%08x.\n",
		cmd->hdr0.val, cmd->hdr1.val,
		cmd->hdr2.val, cmd->data.val);

	if (espi_wait_ready() != 0)
		return -EBUSY;

	espi_clear_status();
	
	spacemit_espi_write32(cmd->data.val, ESPI_DN_TXDATA_PORT);

	spacemit_espi_write_txhdr(7, cmd->hdr2.hdata7);
	spacemit_espi_write_txhdr(6, cmd->hdr1.hdata6);
	spacemit_espi_write_txhdr(5, cmd->hdr1.hdata5);
	spacemit_espi_write_txhdr(4, cmd->hdr1.hdata4);
	spacemit_espi_write_txhdr(3, cmd->hdr1.hdata3);
	spacemit_espi_write_txhdr(2, cmd->hdr0.hdata2);
	spacemit_espi_write_txhdr(1, cmd->hdr0.hdata1);
	spacemit_espi_write_txhdr(0, cmd->hdr0.hdata0);

	/* Dword 0 must be last as this write triggers the transaction */
	//spacemit_espi_write32(cmd->hdr0.val, ESPI_DN_TXHDR_0);

	spacemit_espi_write_dncmd(cmd->hdr0.cmd_type, cmd->hdr0.slave_sel);

	if (espi_wait_ready() != 0)
		return -EBUSY;
	if (espi_poll_status(&status) != 0)
		return -EBUSY;

	/* If command did not complete downstream, return error. */
	if (!(status & ESPI_DNCMD_INT))
		return -EIO;

	spacemit_espi_write32(status, ESPI_SLAVE0_INT_STS);
	return 0;
}

static int espi_send_pltrst(bool assert)
{
	struct espi_cmd cmd = {
		.hdr0 = {
			.cmd_type = ESPI_DNCMD_PUT_VW,
			.cmd_sts = 1,
			.hdata0 = 0, /* 1 VW group */
		},
		.data = {
			.byte0 = ESPI_VWIRE_INDEX_SYSTEM_EVENT_3,
			.byte1 = assert ? ESPI_VWIRE_SYSTEM_EVENT_LOW(ESPI_VWIRE_SYSTEM_PLTRST)
					: ESPI_VWIRE_SYSTEM_EVENT_HIGH(ESPI_VWIRE_SYSTEM_PLTRST),
		},
	};

	if (!ESPI_VW_CHAN)
		return 0;

	return espi_send_command(&cmd);
}

static uint32_t spacemit_espi_get_configuration(uint16_t address)
{
	return 0;
}

static void spacemit_espi_set_configuration(uint16_t address, uint32_t config)
{
}

static int espi_get_general_configuration(uint32_t *config)
{
	*config = spacemit_espi_get_configuration(ESPI_SLAVE_GEN_CFG);
	espi_show_slave_general_configuration(*config);
	return 0;
}

static int espi_set_general_configuration(uint32_t slave_caps)
{
	uint32_t slave_config = 0;
	uint32_t ctrlr_config = 0;

	if (ESPI_CRC_CHECKING) {
		slave_config |= ESPI_CRC_CHECKING;
		ctrlr_config |= ESPI_CRC_CHECKING_EN;
	}

	espi_config_alert_pin(slave_caps, &slave_config, &ctrlr_config);
	espi_config_io_mode(slave_caps, &slave_config, &ctrlr_config);
	espi_config_op_freq(slave_caps, &slave_config, &ctrlr_config);

	con_log("spacemit_espi: configuration: slave=0x%x master=0x%x\n",
	        slave_config, ctrlr_config);

	espi_show_slave_general_configuration(slave_config);

	spacemit_espi_set_configuration(ESPI_SLAVE_GEN_CFG, slave_config);
	spacemit_espi_write32(ctrlr_config, ESPI_SLAVE0_CONFIG);
	return 0;
}

static int espi_wait_channel_ready(uint16_t address)
{
	uint32_t config;
	int count = ESPI_CH_READY_TIMEOUT_US;

	do {
		config = spacemit_espi_get_configuration(address);
		if (espi_slave_channel_ready(config))
			return 0;
	} while (count--);

	con_err("Channel is not ready after %d usec (slave addr: 0x%x)\n",
	       ESPI_CH_READY_TIMEOUT_US, address);
	return -1;

}

static void espi_enable_ctrlr_channel(uint32_t channel_en)
{
	uint32_t reg = spacemit_espi_read32(ESPI_SLAVE0_CONFIG);

	reg |= channel_en;

	spacemit_espi_write32(reg, ESPI_SLAVE0_CONFIG);
}

static int espi_set_channel_configuration(uint32_t slave_config,
					  uint32_t slave_reg_addr,
					  uint32_t ctrlr_enable)
{
	spacemit_espi_set_configuration(slave_reg_addr, slave_config);
	if (!(slave_config & ESPI_SLAVE_CHANNEL_ENABLE))
		return 0;

	if (espi_wait_channel_ready(slave_reg_addr) != 0)
		return -1;

	espi_enable_ctrlr_channel(ctrlr_enable);
	return 0;
}

#ifdef CONFIG_ESPI_PERI
static int espi_setup_peri_channel(uint32_t slave_caps)
{
	uint32_t slave_config;
	/* Peripheral channel requires BME bit to be set when enabling. */
	const uint32_t slave_en_mask =
		ESPI_SLAVE_CHANNEL_ENABLE | ESPI_PERI_BUS_MASTER_ENABLE;

	slave_config = spacemit_espi_get_configuration(ESPI_SLAVE_PERI_CFG);
	if (!espi_slave_peri_chan_sup(slave_caps)) {
		con_err("espi: peripheral channel not supported!\n");
		return -ENOTSUP;
	}
	slave_config |= slave_en_mask;
	espi_show_slave_peripheral_channel_configuration(slave_config);

	return espi_set_channel_configuration(slave_config, ESPI_SLAVE_PERI_CFG,
					      ESPI_PERIPH_CH_EN);
}
#else
static int espi_setup_peri_channel(uint32_t slave_caps)
{
	uint32_t slave_config;
	/* Peripheral channel requires BME bit to be set when enabling the channel. */
	const uint32_t slave_en_mask =
		ESPI_SLAVE_CHANNEL_ENABLE | ESPI_PERI_BUS_MASTER_ENABLE;

	slave_config = spacemit_espi_get_configuration(ESPI_SLAVE_PERI_CFG);
	/* Peripheral channel is the only one which is enabled on reset.
	 * So, if the mainboard wants to disable it, set configuration to
	 * disable peripheral channel. It also requires that BME bit be
	 * cleared.
	 */
	slave_config &= ~slave_en_mask;
	return espi_set_channel_configuration(slave_config, ESPI_SLAVE_PERI_CFG,
					      ESPI_PERIPH_CH_EN);
}
#endif

#ifdef CONFIG_ESPI_VWIRE
static int espi_setup_vw_channel(uint32_t slave_caps)
{
	uint32_t slave_vw_caps;
	uint32_t ctrlr_vw_caps;
	uint32_t slave_vw_count_supp;
	uint32_t ctrlr_vw_count_supp;
	uint32_t use_vw_count;
	uint32_t slave_config;

	if (!espi_slave_vw_chan_sup(slave_caps)) {
		con_err("espi: VW channel not supported: %x\n", slave_caps);
//		return -ENOTSUP;
	}

	slave_config = spacemit_espi_get_configuration(ESPI_SLAVE_VWIRE_CFG);
	ctrlr_vw_caps = spacemit_espi_read32(ESPI_MASTER_CAP);
	ctrlr_vw_count_supp = FIELD_GET(ESPI_VW_MAX_SIZE_MASK, ctrlr_vw_caps);

	slave_vw_count_supp = espi_slave_vwire_max_op_count_sup(slave_vw_caps);
	use_vw_count = min(ctrlr_vw_count_supp, slave_vw_count_supp);

	slave_config = ESPI_SLAVE_CHANNEL_ENABLE | ESPI_VWIRE_MAX_OP_COUNT_SEL(use_vw_count);
	spacemit_espi_set_configuration(ESPI_SLAVE_VWIRE_CFG, slave_config);
	spacemit_espi_write32(ESPI_ENABLE_VW_CHANNEL, ESPI_DN_TXHDRn(1));
	if (spacemit_espi_read32(ESPI_DN_TXHDRn(1)) == 0x70703)
		return 0;
	//return espi_set_channel_configuration(slave_config, ESPI_SLAVE_VWIRE_CFG, ESPI_VW_CH_EN);
	return -1;
}
#else
#define espi_setup_vw_channel(caps)		0
#endif

#ifdef CONFIG_ESPI_OOB
static int espi_setup_oob_channel(uint32_t slave_caps)
{
	uint32_t slave_config;

	if (!espi_slave_oob_chan_sup(slave_caps)) {
		con_err("espi: OOB channel not supported!\n");
		return -ENOTSUP;
	}

	slave_config = spacemit_espi_get_configuration(ESPI_SLAVE_OOB_CFG);
	slave_config |= ESPI_SLAVE_CHANNEL_ENABLE;

	return espi_set_channel_configuration(slave_config, ESPI_SLAVE_OOB_CFG,
					      ESPI_OOB_CH_EN);
}
#else
#define espi_setup_oob_channel(caps)		0
#endif

#ifdef CONFIG_ESPI_FLASH
static int espi_setup_flash_channel(uint32_t slave_caps)
{
	uint32_t slave_config;

	if (!espi_slave_flash_chan_sup(slave_caps)) {
		con_err("espi: flash channel not supported!\n");
		return -ENOTSUP;
	}

	slave_config = spacemit_espi_get_configuration(ESPI_SLAVE_FLASH_CFG);
	slave_config |= ESPI_SLAVE_CHANNEL_ENABLE;

	return espi_set_channel_configuration(slave_config, ESPI_SLAVE_FLASH_CFG,
					      ESPI_FLASH_CH_EN);
}
#else
#define espi_setup_flash_channel(caps)		0
#endif

/* mode: 1x
 * freq: 16MHz
 * slect alert mode
 */
static void espi_set_initial_config(void)
{
	uint32_t espi_initial_mode = ESPI_CLK_FREQ_SEL(ESPI_GEN_OP_FREQ_20MHZ) |
				     ESPI_IO_MODE_SEL(ESPI_GEN_IO_MODE_SINGLE);

	switch (ESPI_ALERT_PIN) {
	case ESPI_GEN_ALERT_MODE_IO1:
		break;
	case ESPI_GEN_ALERT_MODE_PIN:
		//espi_initial_mode |= ESPI_ALERT_MODE_SEL;
		break;
	default:
		BUG();
	}
	espi_initial_mode |= ESPI_CRC_CHECK_EN;
	if (CONFIG_ESPI_PERI)
		espi_initial_mode |= ESPI_PR_EN;
	if (CONFIG_ESPI_VWIRE)
		espi_initial_mode |= ESPI_VW_EN;
	if (CONFIG_ESPI_OOB)
		espi_initial_mode |= ESPI_OOB_EN;
	if (CONFIG_ESPI_FLASH)
		espi_initial_mode |= ESPI_FLASH_EN;

	spacemit_espi_write32(espi_initial_mode, ESPI_SLAVE0_CONFIG);
}

static void espi_setup_subtractive_decode(uint32_t std_io_decode_bitmap)
{
	// uint32_t global_ctrl_reg;
	// global_ctrl_reg = spacemit_espi_read32(ESPI_GLOBAL_CONTROL_1);

	// if (mb_cfg->subtractive_decode) {
	// 	global_ctrl_reg &= ~ESPI_SUB_DECODE_SLV_MASK;
	// 	global_ctrl_reg |= ESPI_SUB_DECODE_EN;
	// } else {
	// 	global_ctrl_reg &= ~ESPI_SUB_DECODE_EN;
	// }
	// spacemit_espi_write32(ESPI_GLOBAL_CONTROL_1, global_ctrl_reg);
}

int spacemit_espi_tx_vw(uint8_t *ids, uint8_t *data, int num)
{
	int i;
	uint32_t status;
	uint32_t val = 0;

	if (num > 64 || num < 1)
		return -EINVAL;

	for (i = 0; i < num; i++) {
		val |= ids[i] << ((i % 4) << 1) | data[i] << (((i % 4) << 1) + 1);
		if ((i % 2) == 1) {
			spacemit_espi_write32(val, ESPI_DN_TXDATA_PORT);
			val = 0;
		}
	}

	if ((i % 2) == 0) {
		spacemit_espi_write32(val, ESPI_DN_TXDATA_PORT);
	}

	spacemit_espi_write_dncmd(ESPI_DNCMD_PUT_VW, 0);
	spacemit_espi_write_txhdr(0, num);

	if (espi_wait_ready() != 0)
		return -EBUSY;
	if (espi_poll_status(&status) != 0)
		return -EBUSY;

	/* If command did not complete downstream, return error. */
	if (!(status & ESPI_DNCMD_INT))
		return -EIO;
	return 0;
}

int spacemit_espi_tx_oob(uint8_t *buf, int len)
{
	int i;
	uint32_t status;
	uint32_t data[16];

	memset(&data, 0, 64);
	memcpy(&data, &buf[3], len - 3);

	for (i = 0; i < (len - 3 + 3) / 4; i++) {
		spacemit_espi_write32(data[i], ESPI_DN_TXDATA_PORT);
	}

	spacemit_espi_write_dncmd(ESPI_DNCMD_PUT_OOB, 0);
	spacemit_espi_write_txhdr(0, ESPI_CYCLE_TYPE_OOB_MESSAGE);
	spacemit_espi_write_txhdr(1, ESPI_TXHDR_TAG(0) | ESPI_TXHDR_LEN(HIBYTE(len)));
	spacemit_espi_write_txhdr(2, LOBYTE(len));

	if (espi_wait_ready() != 0)
		return -EBUSY;
	if (espi_poll_status(&status) != 0)
		return -EBUSY;

	/* If command did not complete downstream, return error. */
	if (!(status & ESPI_DNCMD_INT))
		return -EIO;
	return 0;
}

int spacemit_espi_rx_oob(uint8_t *buf)
{
	int i;
	int len = 0;
	uint32_t data[16];

	for (i = 0; i < ESPI_OOB_MESSAGE_HDR_LEN; i++) {
		buf[i] = spacemit_espi_read_rxhdr(i);
	}
	len = ESPI_RXHDR_LENGTH(buf);
	for (i = 0; i < (len - 3) / 4; i++) {
		data[i] = spacemit_espi_read32(ESPI_UP_RXDATA_PORT);
	}
	memcpy(&buf[3], &data, len - 3);
	return len;
}

void spacemit_espi_irq_init(void)
{
	irqc_configure_irq(ESPI_IRQ, 0, IRQ_LEVEL_TRIGGERED);
	irq_register_vector(ESPI_IRQ, espi_handle_irq);
	irqc_enable_irq(ESPI_IRQ);
	spacemit_espi_enable_all_irqs();
}

void spacemit_espi_handle_irq(void)
{
	int int_sts;
	int len;
	uint8_t rxvw_data;

	int_sts = __raw_readl(ESPI_SLAVE0_INT_STS);
	__raw_writel(int_sts, ESPI_SLAVE0_INT_STS);

	if (int_sts & ESPI_FLASH_REQ_INT)
		con_log("spacemit_espi: ESPI_FLASH_REQ_INT\n");
	if (int_sts & ESPI_RXOOB_INT) {
		con_log("spacemit_espi: ESPI_RXOOB_INT\n");
		len = spacemit_espi_rx_oob((uint8_t *)rxoob_buffer);
		rxoob_callback(rxoob_buffer, len);
	}
	if (int_sts & ESPI_RXMSG_INT)
		con_log("spacemit_espi: ESPI_RXMSG_INT\n");
	if (int_sts & ESPI_RXVW_GRP3_INT) {
		con_log("spacemit_espi: ESPI_RXVW_GRP3_INT\n");
		rxvw_data = (spacemit_espi_read32(ESPI_SLAVE0_RXVW_DATA) & 0xFF000000U) >> 8;
		rxvw_callback(131, rxvw_data);
	}
	if (int_sts & ESPI_RXVW_GRP2_INT) {
		con_log("spacemit_espi: ESPI_RXVW_GRP2_INT\n");
		rxvw_data = (spacemit_espi_read32(ESPI_SLAVE0_RXVW_DATA) & 0xFF0000U) >> 8;
		rxvw_callback(130, rxvw_data);
	}
	if (int_sts & ESPI_RXVW_GRP1_INT) {
		con_log("spacemit_espi: ESPI_RXVW_GRP1_INT\n");
		rxvw_data = (spacemit_espi_read32(ESPI_SLAVE0_RXVW_DATA) & 0xFF00U) >> 8;
		rxvw_callback(129, rxvw_data);
	}
	if (int_sts & ESPI_RXVW_GRP0_INT) {
		con_log("spacemit_espi: ESPI_RXVW_GRP0_INT\n");
		rxvw_data = spacemit_espi_read32(ESPI_SLAVE0_RXVW_DATA) & 0xFFU;
		rxvw_callback(128, rxvw_data);
	}
	if (int_sts & ESPI_PROTOCOL_INT) {
		if (int_sts & ESPI_PROTOCOL_ERR_INT)
			con_log("spacemit_espi: ESPI_PROTOCOL_ERR_INT\n");
		if (int_sts & ESPI_RXFLASH_OFLOW_INT)
			con_log("spacemit_espi: ESPI_RXFLASH_OFLOW_INT\n");
		if (int_sts & ESPI_RXMSG_OFLOW_INT)
			con_log("spacemit_espi: ESPI_RXMSG_OFLOW_INT\n");
		if (int_sts & ESPI_RXOOB_OFLOW_INT)
			con_log("spacemit_espi: ESPI_RXOOB_OFLOW_INT\n");
		if (int_sts & ESPI_ILLEGAL_LEN_INT)
			con_log("spacemit_espi: ESPI_ILLEGAL_LEN_INT\n");
		if (int_sts & ESPI_ILLEGAL_TAG_INT)
			con_log("spacemit_espi: ESPI_ILLEGAL_TAG_INT\n");
		if (int_sts & ESPI_UNSUCSS_CPL_INT)
			con_log("spacemit_espi: ESPI_UNSUCSS_CPL_INT\n");
		if (int_sts & ESPI_INVALID_CT_RSP_INT)
			con_log("spacemit_espi: ESPI_INVALID_CP_RSP_INT\n");
		if (int_sts & ESPI_UNKNOWN_RSP_INT)
			con_log("spacemit_espi: ESPI_UNKNOWN_RSP_INT\n");
		if (int_sts & ESPI_CRC_ERR_INT)
			con_log("spacemit_espi: ESPI_CRC_ERR_INT\n");
		if (int_sts & ESPI_WAIT_TIMEOUT_INT)
			con_log("spacemit_espi: ESPI_WAIT_TIMEOUT_INT\n");
		if (int_sts & ESPI_BUS_ERR_INT)
			con_log("spacemit_espi: ESPI_BUS_ERR_INT\n");
		spacemit_espi_rsp = ESPI_RSP_FATAL_ERROR;
	}
	if (int_sts & ESPI_NON_FATAL_INT) {
		con_log("spacemit_espi: ESPI_NON_FATAL_INT\n");
		spacemit_espi_rsp = ESPI_RSP_NON_FATAL_ERROR;
	}
	if (int_sts & ESPI_FATAL_ERR_INT) {
		con_log("spacemit_espi: ESPI_FATAL_ERR_INT\n");
		spacemit_espi_rsp = ESPI_RSP_FATAL_ERROR;
	}
	if (int_sts & ESPI_NO_RSP_INT) {
		/* When performing an in-band reset the host controller
		 * and the peripheral can have mismatched IO configs.
		 *
		 * i.e., The eSPI peripheral can be in IO-4 mode while, the
		 * eSPI host will be in IO-1. This results in the
		 * peripheral getting invalid packets and thus not
		 * responding.
		 *
		 * If the peripheral is alerting when we perform an in-band
		 * reset, there is a race condition in
		 * spacemit_espi_write_cmd().
		 * 1) spacemit_espi_write_cmd() clears the interrupt
		 *    status.
		 * 2) eSPI host controller hardware notices the alert and
		 *    sends a GET_STATUS.
		 * 3) spacemit_espi_write_cmd() writes the in-band reset
		 *    command.
		 * 4) eSPI hardware enqueues the in-band reset until
		 *    GET_STATUS is complete.
		 * 5) GET_STATUS fails with NO_RSP_INT and sets the
		 *    interrupt status.
		 * 6) eSPI hardware performs in-band reset.
		 * 7) spacemit_espi_write_cmd() checks the status and sees
		 *    a NO_RSP_INT bit.
		 *
		 * As a workaround we allow the NO_RSP_INT status code
		 * when we perform an in-band reset.
		 */
		con_log("spacemit_espi: ESPI_NO_RSP_INT\n");
		spacemit_espi_rsp = ESPI_RSP_NO_RESPONSE;
	}
	if (int_sts & ESPI_DNCMD_INT) {
		con_log("spacemit_espi: ESPI_DNCMD_INT\n");
		espi_cmd_complete(spacemit_espi_rsp);
	}
}

void espi_handle_irq(irq_t irq)
{
	spacemit_espi_handle_irq();
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

uint8_t spacemit_espi_cmd2dncmd(uint8_t opcode)
{
	uint8_t dncmd;

	switch (opcode) {
	case ESPI_CMD_SET_CONFIGURATION:
		dncmd = ESPI_DNCMD_SET_CONFIGURATION;
		break;
	case ESPI_CMD_GET_CONFIGURATION:
		dncmd = ESPI_DNCMD_GET_CONFIGURATION;
		break;
	case ESPI_CMD_RESET:
	default:
		dncmd = ESPI_DNCMD_IN_BAND_RESET;
		break;
	}
	return dncmd;
}

void spacemit_espi_write_cmd(uint8_t opcode,
			     uint8_t hlen, uint8_t *hbuf,
			     uint8_t dlen, uint8_t *dbuf)
{
	uint8_t i;
	uint8_t dncmd;
	uint32_t txdata;
	uint8_t db0, db1, db2, db3;

	dncmd = spacemit_espi_cmd2dncmd(opcode);

	con_dbg("spacemit_espi: cmd: %02x, hdr=%d, dat=%d\n", dncmd, hlen, dlen);

	for (i = 0; i < hlen; i++)
		spacemit_espi_write_txhdr(i, hbuf[i]);
	for (i = 0; i < ((dlen + 3) / 4); i++) {
		uint8_t ilen = i * 4;

		if (ilen < dlen)
			db0 = dbuf[ilen];
		else
			db0 = 0x00;
		if ((ilen + 1) < dlen)
			db1 = dbuf[ilen + 1];
		else
			db1 = 0x00;
		if ((ilen + 2) < dlen)
			db2 = dbuf[ilen + 2];
		else
			db2 = 0x00;
		if ((ilen + 3) < dlen)
			db3 = dbuf[ilen + 3];
		else
			db3 = 0x00;
		txdata = MAKELONG(MAKEWORD(db0, db1),
				  MAKEWORD(db2, db3));
		spacemit_espi_write32(txdata, ESPI_DN_TXDATA_PORT);
	}
	spacemit_espi_write_dncmd(dncmd, 0);
}

uint8_t spacemit_espi_read_rsp(uint8_t opcode,
			       uint8_t hlen, uint8_t *hbuf,
			       uint8_t dlen, uint8_t *dbuf)
{
	uint8_t dncmd;

	dncmd = spacemit_espi_cmd2dncmd(opcode);

	con_dbg("spacemit_espi: cmd: %02x, hdr=%d, dat=%d\n", dncmd, hlen, dlen);

	switch (dncmd) {
	case ESPI_DNCMD_GET_CONFIGURATION:
		BUG_ON(hlen < 4);
		hbuf[0] = spacemit_espi_read_txhdr(3);
		hbuf[1] = spacemit_espi_read_txhdr(4);
		hbuf[2] = spacemit_espi_read_txhdr(5);
		hbuf[3] = spacemit_espi_read_txhdr(6);
		break;
	}
	return spacemit_espi_rsp;
}

int spacemit_espi_write_complete(void)
{
	while (!spacemit_espi_write_done());
	return 0;
}

#ifdef CONFIG_SPACEMIT_ESPI_AUTO_GATING
void spacemit_espi_config_gating(void)
{
	spacemit_espi_clear32(ESPI_MST_STOP_EN, ESPI_GLOBAL_CONTROL_0);
}
#else
#define spacemit_espi_config_gating()	do { } while (0)
#endif

void spacemit_espi_init(void)
{
	con_dbg("spacemit_espi: initializing...\n");
	spacemit_espi_config_gating();
	spacemit_espi_reset();
}

#if 0
void spacemit_espi_test(void)
{
	uint32_t slave_caps;

	// spacemit_espi_write32(ESPI_RGCMD_INT(23) | ESPI_ERR_INT_SMI, ESPI_GLOBAL_CONTROL_1);
	spacemit_espi_write32(0, ESPI_SLAVE0_INT_EN);
	espi_clear_status();
	// espi_clear_decodes();

	/* Boot sequence: Step 1
	 * Set correct initial configuration to talk to the slave:
	 * Set clock frequency to 16.7MHz and single IO mode.
	 */
	espi_set_initial_config();

	/* Boot sequence: Step 2
	 * Send in-band reset
	 * The resets affects both host and slave devices, so set initial
	 * config again.
	 */
	con_log("In-band reset success!\n");

	espi_set_initial_config();

	/* Boot sequence: Step 3
	 * Get configuration of slave device.
	 */
	if (espi_get_general_configuration(&slave_caps) != 0) {
		con_err("Slave GET_CONFIGURATION failed!\n");
		return;
	}
	con_log("Slave GET_CONFIGURATION success!\n");

	/* Boot sequence:
	 * Step 4: Write slave device general config
	 * Step 5: Set host slave config
	 */
	if (espi_set_general_configuration(slave_caps) != 0) {
		con_err("Slave SET_CONFIGURATION failed!\n");
		return;
	}
	con_log("Slave SET_CONFIGURATION success!\n");

	/* Setup polarity before enabling the VW channel so any interrupts
	 * received will have the correct polarity.
	 */
	spacemit_espi_write32(0, ESPI_SLAVE0_VW_POLARITY);

	/* Boot Sequences: Steps 6 - 9
	 * Channel setup
	 */
	/* Set up VW first so we can deassert PLTRST#. */
	if (espi_setup_vw_channel(slave_caps) != 0)
		return;
	con_log("VW Channel setup success!\n");

	/* Assert PLTRST# if VW channel is enabled by mainboard. */
	if (espi_send_pltrst(true) != 0) {
		con_err("PLTRST# assertion failed!\n");
		return;
	}
	con_log("PLTRST# assertion success!\n");

	/* De-assert PLTRST# if VW channel is enabled by mainboard. */
	if (espi_send_pltrst(false) != 0) {
		con_err("PLTRST# deassertion failed!\n");
		return;
	}
	con_log("PLTRST# deassertion success!\n");

	if (espi_setup_peri_channel(slave_caps) != 0)
		return;

	espi_setup_pr_mem_base0(SPACEMIT_ESPI_PR_MEM0);
	espi_setup_pr_mem_base1(SPACEMIT_ESPI_PR_MEM1);
	con_log("Peripheral Channel setup success!\n");

	if (espi_setup_oob_channel(slave_caps) != 0)
		return;
	con_log("OOB Channel setup success!\n");

	if (espi_setup_flash_channel(slave_caps) != 0)
		return;
	con_log("Flash Channel setup success!\n");

	// if (espi_configure_decodes(cfg) != 0) {
	// 	con_err("Configuring decodes failed!\n");
	// 	return;
	// }
	// con_err("Configuring decodes success!\n");

	/* Enable subtractive decode if configured */
	espi_setup_subtractive_decode(ESPI_DECODE_IO_0x80_EN |
				      ESPI_DECODE_IO_0X2E_0X2F_EN |
				      ESPI_DECODE_IO_0X60_0X64_EN);

	// ctrl = spacemit_espi_read32(ESPI_GLOBAL_CONTROL_1);
	// ctrl |= ESPI_BUS_MASTER_EN;

	// ctrl |= ESPI_ALERT_ENABLE;

	// spacemit_espi_write32(ctrl, ESPI_GLOBAL_CONTROL_1);


#if 0
	clk_set_frequency(espi_sclk, ESPI_OP_FREQ_66_MHZ);
	clk_enable(espi_sclk);
#endif

	con_log("espi: initialized spacemit_espi.\n");
}
#endif

#if 0
static int do_espi_read(int argc, char *argv[])
{
#if 0
	caddr_t addr;
	uint32_t val;

	if (argc < 3)
		return -EINVAL;

	if (strcmp(argv[2], "io") == 0) {
		if (argc < 4)
			return -EINVAL;
		addr = (caddr_t)strtoull(argv[4], 0, 0);
		if (strcmp(argv[3], "1") == 0) {
			val = espi_io_read8(addr);
			printf("IO: 0x%08lx=%02x\n", addr, (uint8_t)val);
		} else if (strcmp(argv[3], "2") == 0) {
			val = espi_io_read16(addr);
			printf("IO: 0x%08lx=%04x\n", addr, (uint16_t)val);
		} else if (strcmp(argv[3], "4") == 0) {
			val = espi_io_read32(addr);
			printf("IO: 0x%08lx=%08x\n", addr, val);
		} else
			return -EINVAL;
		return 0;
	} else {
		if (argc < 4)
			return -EINVAL;
		addr = (caddr_t)strtoull(argv[4], 0, 0);
		if (strcmp(argv[3], "1") == 0) {
			val = espi_mem_read8(addr);
			printf("Memory: 0x%08lx=%02x\n", addr, (uint8_t)val);
		} else if (strcmp(argv[3], "2") == 0) {
			val = espi_mem_read16(addr);
			printf("Memory: 0x%08lx=%04x\n", addr, (uint16_t)val);
		} else if (strcmp(argv[3], "4") == 0) {
			val = espi_mem_read32(addr);
			printf("Memory: 0x%08lx=%08x\n", addr, val);
		} else
			return -EINVAL;
		return 0;
	}
#endif
	return -EINVAL;
}

static int do_espi_write(int argc, char *argv[])
{
#if 0
	caddr_t addr;

	if (argc < 5)
		return -EINVAL;

	if (strcmp(argv[2], "io") == 0) {
		uint32_t v;
		int size;
		if (argc < 6)
			return -EINVAL;
		size = (uint32_t)strtoull(argv[3], 0, 0);
		v = (uint32_t)strtoull(argv[4], 0, 0);
		addr = (caddr_t)strtoull(argv[5], 0, 0);
		if (size == 1)
			espi_io_write8(v, addr);
		else if (size == 2)
			espi_io_write16(v, addr);
		else if (size == 4)
			espi_io_write32(v, addr);
		else
			return -EINVAL;
		return 0;
	} else {
		uint32_t v;
		int size;
		if (argc < 6)
			return -EINVAL;
		size = (uint32_t)strtoull(argv[3], 0, 0);
		v = (uint32_t)strtoull(argv[4], 0, 0);
		addr = (caddr_t)strtoull(argv[5], 0, 0);
		if (size == 1)
			espi_mem_write8(v, addr);
		else if (size == 2)
			espi_mem_write16(v, addr);
		else if (size == 4)
			espi_mem_write32(v, addr);
		else
			return -EINVAL;
		return 0;
	}
#endif
	return -EINVAL;
}

static int do_espi(int argc, char *argv[])
{
	if (argc < 2)
		return -EINVAL;
	if (strcmp(argv[1], "read") == 0)
		return do_espi_read(argc, argv);
	if (strcmp(argv[1], "write") == 0)
		return do_espi_write(argc, argv);
	return -EINVAL;
}

DEFINE_COMMAND(espi, do_espi, "SpacemiT enhanced SPI commands",
	"espi read io [1|2|4] <addr>\n"
	"espi read mem [1|2|4] <addr>\n"
	"    -eSPI read sequence\n"
	"espi write io [1|2|4] <value> <addr>\n"
	"espi write mem [1|2|4] <value> <addr>\n"
	"    -eSPI write sequence\n"
);

#endif
