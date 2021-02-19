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
 * @(#)mmc_core.c: MMC/SD/SDIO core implementation
 * $Id: mmc_core.c,v 1.1 2019-10-08 18:24:00 zhenglv Exp $
 */

#include <target/mmc.h>
#include <target/bh.h>
#include <target/irq.h>

bh_t mmc_bh = INVALID_BH;

#if NR_IFD_SLOTS > 1
mmc_slot_t mmc_sid = INVALID_MMC_SID;
struct mmc_slot mmc_slots[NR_MMC_SLOTS];
__align(MMC_DATA_ALIGN) uint8_t mmc_slot_bufs[NR_MMC_SLOTS][MMC_DEF_BL_LEN];
#else
struct mmc_slot mmc_slot_ctrl;
__align(MMC_DATA_ALIGN) uint8_t mmc_slot_buf[MMC_DEF_BL_LEN];
#endif

#ifdef SYS_REALTIME
#define mmc_irq_init_noirq()	irq_register_poller(mmc_bh)
#define mmc_irq_init_irq()	do { } while (0)
#define mmc_irq_poll(event)	mmc_hw_irq_poll()
#else
#define mmc_irq_init_noirq()	do { } while (0)
#define mmc_irq_init_irq()	mmc_hw_irq_init()
#define mmc_irq_poll(event)	do { } while (0)
#endif

#if defined(CONFIG_CONSOLE) && defined(CONFIG_MMC_DEBUG)
#define mmc_state_names	mmc_phy_state_names
#define mmc_cmd_names	mmc_phy_cmd_names
#define mmc_acmd_names	mmc_phy_acmd_names

const char *mmc_state_name(uint32_t val)
{
	if (val >= ARRAY_SIZE(mmc_state_names))
		return "unk";
	return mmc_state_names[val];
}

const char *mmc_cmd_name(uint8_t cmd_index)
{
	if (cmd_index >= ARRAY_SIZE(mmc_cmd_names))
		return "UNDEF";
	if (mmc_cmd_names[cmd_index])
		return mmc_cmd_names[cmd_index];
	else
		return "NULL";
}

const char *mmc_acmd_name(uint8_t cmd_index)
{
	if (cmd_index >= ARRAY_SIZE(mmc_acmd_names))
		return "UNDEF";
	if (mmc_acmd_names[cmd_index])
		return mmc_acmd_names[cmd_index];
	else
		return "NULL";
}

const char *mmc_event_names[] = {
	"POWER_ON",
	"CMD_SUCCESS",
	"CMD_FAILURE",
	"CARD_IRQ",
	"NO_IRQ",
	"OP_COMPLETE",
	"TRANS_END",
	"CARD_INSERT",
	"CARD_REMOVE",
	"CARD_BUSY",
	"SELECT_CARD",
	"DESELECT_CARD",
	"START_TRAN",
	"STOP_TRAN",
};

const char *mmc_event_name(mmc_event_t event)
{
	int i, nr_evts = 0;

	for (i = 0; i < ARRAY_SIZE(mmc_event_names); i++) {
		if (_BV(i) & event) {
			nr_evts++;
			return mmc_event_names[i];
		}
	}
	return "NONE";
}

const char *mmc_op_names[] = {
	"NO_OP",
	"IDENTIFY_CARD",
	"SELECT_CARD",
	"DESELECT_CARD",
	"READ_BLOCKS",
	"WRITE_BLOCKS",
};

const char *mmc_op_name(uint8_t op)
{
	if (op >= ARRAY_SIZE(mmc_op_names))
		return "NO-OP";
	return mmc_op_names[op];
}

const char *mmc_error_names[] = {
	"no error",
	"card is busy",
	"host omits voltage range",
	"card is with non compatible voltage range",
	"card looses bus",
	"illegal command",
	"COM CRC error",
	"check pattern error",
	"timeout",
};

const char *mmc_error_name(uint8_t error)
{
	if (error >= ARRAY_SIZE(mmc_error_names))
		return "no error";
	return mmc_error_names[error];
}

void mmc_debug(uint8_t tag, uint32_t val)
{
	switch (tag) {
	case MMC_DEBUG_STATE:
		printf("state %s\n", mmc_state_name(val));
		break;
	case MMC_DEBUG_EVENT:
		printf("event %s\n", mmc_event_name(val));
		break;
	case MMC_DEBUG_ERROR:
		printf("\nerror %s\n", mmc_error_name(val));
		break;
	case MMC_DEBUG_CMD:
		printf("cmd%d %s\n", val, mmc_cmd_name(val));
		break;
	case MMC_DEBUG_OP:
		printf("op %s\n", mmc_op_name(val));
		break;
	case MMC_DEBUG_ACMD:
		printf("acmd%d %s\n", val, mmc_acmd_name(val));
		break;
	default:
		BUG();
		break;
	}
}
#endif

void mmc_event_raise(mmc_event_t event)
{
	mmc_debug_event(event);
	mmc_slot_ctrl.event |= event;
	bh_resume(mmc_bh);
}

mmc_event_t mmc_event_save(void)
{
	mmc_event_t events;
	events = mmc_slot_ctrl.event;
	mmc_slot_ctrl.event = 0;
	return events;
}

void mmc_event_restore(mmc_event_t event)
{
	mmc_slot_ctrl.event |= event;
}

uint8_t mmc_state_get(void)
{
	return mmc_slot_ctrl.state;
}

void mmc_state_set(uint8_t state)
{
	mmc_debug_state(state);
	mmc_slot_ctrl.state = state;
	mmc_phy_handle_seq();
}

#if NR_MMC_SLOTS > 1
void mmc_slot_restore(mmc_slot_t slot)
{
	mmc_sid = slot;
	mmc_hw_slot_select(slot);
}

mmc_slot_t mmc_slot_save(mmc_slot_t slot)
{
	mmc_slot_t osid = mmc_sid;
	mmc_slot_restore(slot);
	return osid;
}
#endif

static const struct mmc_mode mmc_modes[] = {
#ifdef CONFIG_MMC_UHSI
	{
		.mode = UHS_SDR104,
		.widths = MMC_MODE_4BIT | MMC_MODE_1BIT,
#ifdef CONFIG_MMC_TUNING
		.tuning = MMC_CMD_SEND_TUNING_BLOCK,
#endif
		.freq = 208000000,
	},
	{
		.mode = UHS_SDR50,
		.widths = MMC_MODE_4BIT | MMC_MODE_1BIT,
		.freq = 100000000,
	},
	{
		.mode = UHS_DDR50,
		.widths = MMC_MODE_4BIT | MMC_MODE_1BIT,
		.freq = 50000000,
		.is_ddr = true,
	},
	{
		.mode = UHS_SDR25,
		.widths = MMC_MODE_4BIT | MMC_MODE_1BIT,
		.freq = 50000000,
	},
	{
		.mode = UHS_SDR12,
		.widths = MMC_MODE_4BIT | MMC_MODE_1BIT,
		.freq = 25000000,
	},
#endif
	{
		.mode = SD_HS,
		.widths = MMC_MODE_4BIT | MMC_MODE_1BIT,
		.freq = 50000000,
	},
	{
		.mode = MMC_HS,
		.widths = MMC_MODE_4BIT | MMC_MODE_1BIT,
		.freq = 26000000,
	},
	{
		.mode = MMC_HS_52,
		.widths = MMC_MODE_4BIT | MMC_MODE_1BIT,
		.freq = 52000000,
	},
	{
		.mode = MMC_DDR_52,
		.widths = MMC_MODE_4BIT | MMC_MODE_1BIT,
		.freq = 52000000,
		.is_ddr = true,
	},
	{
		.mode = MMC_HS_200,
		.widths = MMC_MODE_4BIT | MMC_MODE_1BIT,
		.freq = 200000000,
	},
	{
		.mode = MMC_HS_400,
		.widths = MMC_MODE_8BIT | MMC_MODE_4BIT | MMC_MODE_1BIT,
		.freq = 200000000,
		.is_ddr = true,
	},
	{
		.mode = MMC_HS_400_ES,
		.widths = MMC_MODE_8BIT | MMC_MODE_4BIT | MMC_MODE_1BIT,
		.freq = 200000000,
		.is_ddr = true,
	},
	{
		.mode = MMC_LEGACY,
		.widths = MMC_MODE_4BIT | MMC_MODE_1BIT,
		.freq = 25000000,
	},
	{
		.mode = MMC_IDENT,
		.widths = MMC_MODE_1BIT,
		.freq = MMC_FREQ_OD,
	},
};

static const struct mmc_mode *mmc_mode_find(enum mmc_bus_mode mode)
{
	int i;
	const struct mmc_mode *m;

	for (i = 0; i < ARRAY_SIZE(mmc_modes); i++) {
		m = &(mmc_modes[i]);
		if (m->mode == mode)
			return m;
	}
	return NULL;
}

uint32_t mmc_mode_speed(enum mmc_bus_mode mode)
{
	const struct mmc_mode *m = mmc_mode_find(mode);

	if (!m)
		return 0;
	return m->freq;
}

uint32_t mmc_mode_width(enum mmc_bus_mode mode)
{
	const struct mmc_mode *m = mmc_mode_find(mode);

	if (!m)
		return 0;
	return m->widths;
}

bool mmc_mode_isddr(enum mmc_bus_mode mode)
{
	const struct mmc_mode *m = mmc_mode_find(mode);

	if (!m)
		return 0;
	return m->is_ddr;
}

void mmc_config_mode(enum mmc_bus_mode mode)
{
	mmc_slot_ctrl.mode = mode;
}

void mmc_select_modes(enum mmc_bus_mode mode, uint32_t card_widths)
{
	uint32_t speed;
	uint32_t mode_widths;

	mmc_slot_ctrl.select_mode = mode;
	if (mode == MMC_LEGACY)
		speed = mmc_slot_ctrl.default_speed;
	else
		speed = mmc_mode_speed(mode);
	if (speed != mmc_slot_ctrl.config_speed)
		mmc_config_clock(speed);
	mode_widths = mmc_mode_width(mode);
	if (mode_widths & card_widths & MMC_MODE_8BIT)
		mmc_slot_ctrl.expect_width = 8;
	else if (mode_widths & card_widths & MMC_MODE_4BIT)
		mmc_slot_ctrl.expect_width = 4;
	else
		mmc_slot_ctrl.expect_width = 1;
}

uint8_t mmc_crc7_update(uint8_t crc, uint8_t data)
{
	int i;

	crc ^= data;
	for (i = 0; i < 8; i++) {
		if (crc & 0x80)
			crc = (crc << 1) ^ 0x12;  /* 0x12 = 0x09<<(8-7) */
		else
			crc <<= 1;
	}
	return crc;
}

static void mmc_async_handler(void)
{
	mmc_slot_t slot;
	__unused mmc_slot_t sslot;

	for (slot = 0; slot < NR_MMC_SLOTS; slot++) {
		sslot = mmc_slot_save(slot);
		mmc_phy_handle_stm();
		mmc_slot_restore(sslot);
	}
}

static void mmc_handler(uint8_t event)
{
	if (event == BH_POLLIRQ)
		mmc_irq_poll(event);
	else {
		switch (event) {
		case BH_WAKEUP:
			mmc_async_handler();
			break;
		default:
			BUG();
		}
	}
}

void mmc_dat_complete(uint8_t err)
{
	mmc_slot_ctrl.err = err;
	if (err != MMC_ERR_NO_ERROR) {
		mmc_debug_error(err);
		mmc_event_raise(MMC_EVENT_CMD_FAILURE);
		return;
	}
	mmc_phy_stop_dat();
	mmc_set_block_data(0);
	mmc_event_raise(MMC_EVENT_CMD_SUCCESS);
}

bool mmc_blk_success(void)
{
	mmc_slot_ctrl.block_cnt--;
	if (mmc_slot_ctrl.block_cnt == 0) {
		mmc_dat_success();
		return true;
	} else
		mmc_slot_ctrl.block_data += mmc_slot_ctrl.block_len;
	return false;
}

void mmc_rsp_complete(uint8_t err)
{
	uint8_t type = mmc_get_block_data();

	mmc_slot_ctrl.err = err;
	if (err != MMC_ERR_NO_ERROR) {
		mmc_debug_error(err);
		mmc_event_raise(MMC_EVENT_CMD_FAILURE);
		unraise_bits(mmc_slot_ctrl.flags, MMC_SLOT_WAIT_APP_ACMD);
		return;
	}
	unraise_bits(mmc_slot_ctrl.flags, MMC_SLOT_WAIT_APP_ACMD);
	if (!type) {
		mmc_event_raise(MMC_EVENT_CMD_SUCCESS);
		return;
	}
	mmc_phy_start_dat();
}

void mmc_cmd_complete(uint8_t err)
{
	mmc_slot_ctrl.err = err;
	if (err != MMC_ERR_NO_ERROR) {
		mmc_debug_error(err);
		mmc_event_raise(MMC_EVENT_CMD_FAILURE);
		return;
	}
	mmc_phy_recv_rsp();
}

void mmc_cmd(uint8_t cmd)
{
	mmc_slot_ctrl.cmd = cmd;
	mmc_slot_ctrl.err = MMC_ERR_NO_ERROR;
	mmc_debug_cmd(cmd);
	mmc_phy_send_cmd();
}

void mmc_wait_busy(void)
{
	mmc_slot_ctrl.start_busy = tick_get_counter();
	raise_bits(mmc_slot_ctrl.flags, MMC_SLOT_CARD_IS_BUSY);
}

void mmc_send_acmd(uint8_t acmd)
{
	mmc_slot_ctrl.acmd = acmd;
	raise_bits(mmc_slot_ctrl.flags, MMC_SLOT_WAIT_APP_CMD);
	mmc_cmd(MMC_CMD_APP_CMD);
}

void mmc_app_cmd_complete(void)
{
	mmc_debug_acmd(mmc_slot_ctrl.acmd);
	unraise_bits(mmc_slot_ctrl.flags, MMC_SLOT_WAIT_APP_CMD);
	raise_bits(mmc_slot_ctrl.flags, MMC_SLOT_WAIT_APP_ACMD);
	mmc_phy_send_cmd();
}

void mmc_op_complete(bool result)
{
	uint8_t op = mmc_slot_ctrl.op;

	mmc_cmpl_cb cb = mmc_slot_ctrl.op_cb;

	mmc_slot_ctrl.op = MMC_OP_NO_OP;
	mmc_slot_ctrl.op_cb = NULL;
	mmc_slot_ctrl.cmd = MMC_CMD_NONE;

	if (cb)
		cb(mmc_slot_ctrl.rca, op, result);
}

bool mmc_op_busy(void)
{
	return !!(mmc_slot_ctrl.op != MMC_OP_NO_OP);
}

int mmc_start_op(uint8_t op, mmc_cmpl_cb cb)
{
	if (mmc_op_busy())
		return -EBUSY;

	mmc_slot_ctrl.op = op;
	mmc_slot_ctrl.op_cb = cb;
	mmc_slot_ctrl.cmd = MMC_CMD_NONE;
	mmc_debug_op(op);

	mmc_phy_handle_seq();
	return 0;
}

int mmc_read_blocks(uint8_t *buf, mmc_lba_t lba,
		    size_t cnt, mmc_cmpl_cb cb)
{
	BUG_ON(!IS_ALIGNED(lba, MMC_DATA_ALIGN));

	if (mmc_slot_ctrl.card_ocr & MMC_OCR_CCS) {
		mmc_slot_ctrl.address = lba;
		mmc_slot_ctrl.trans_len = MMC_DEF_BL_LEN;
		mmc_slot_ctrl.trans_cnt = cnt;
	} else {
		uint16_t len = 1 << mmc_slot_ctrl.csd.read_bl_len;

		mmc_slot_ctrl.address = lba * len;
		mmc_slot_ctrl.trans_len = len;
		mmc_slot_ctrl.trans_cnt = cnt * (len / MMC_DEF_BL_LEN);
	}
	mmc_slot_ctrl.trans_buf = buf;
	__mmc_read_blocks(cb);
	return 0;
}

void mmc_config_width(uint8_t width)
{
	mmc_slot_ctrl.config_width = width;
	mmc_hw_set_width(mmc_slot_ctrl.config_width);
}

void mmc_config_clock(uint32_t speed)
{
	mmc_slot_ctrl.config_speed = speed;
	mmc_hw_set_clock(mmc_slot_ctrl.config_speed);
}

void mmc_reset_slot(void)
{
	mmc_slot_ctrl.op = MMC_OP_NO_OP;
	mmc_slot_ctrl.cmd = MMC_CMD_NONE;
	mmc_slot_ctrl.dat = mmc_slot_buf;
	mmc_slot_ctrl.state = MMC_STATE_idle;
	mmc_slot_ctrl.rca = MMC_RCA_DEFAULT;
	mmc_slot_ctrl.mmc_cid = INVALID_MMC_CARD;
	mmc_slot_ctrl.event = 0;
	mmc_slot_ctrl.flags = 0;
	mmc_phy_reset_slot();
	/* Reset modes */
	mmc_config_mode(MMC_IDENT);
	mmc_config_width(1);
	mmc_config_clock(MMC_FREQ_OD);
	mmc_select_modes(MMC_IDENT, MMC_MODE_1BIT);
	mmc_hw_card_detect();
}

void mmc_set_block_data(uint8_t type)
{
	unraise_bits(mmc_slot_ctrl.flags, MMC_SLOT_BLOCK_DATA);
	raise_bits(mmc_slot_ctrl.flags, type);
}

uint8_t mmc_get_block_data(void)
{
	return (mmc_slot_ctrl.flags & MMC_SLOT_BLOCK_DATA);
}

/* Divide 10 to make sure fmult(s) are integers */
static const int funit[] = {
	10000,
	100000,
	1000000,
	10000000,
};
static const uint8_t fmult[] = {
	0,	/* reserved */
	10,
	12,
	13,
	15,
	20,
	25,
	30,
	35,
	40,
	45,
	50,
	55,
	60,
	70,
	80,
};

uint32_t mmc_decode_tran_speed(uint8_t tran_speed)
{
	return funit[MMC_TRAN_SPEED_UNIT(tran_speed)] *
	       fmult[MMC_TRAN_SPEED_MULT(tran_speed)];
}

void mmcsd_init(void)
{
	mmc_slot_t slot;
	__unused mmc_slot_t sslot;

	DEVICE_INTF(DEVICE_INTF_MMC);
	mmc_bh = bh_register_handler(mmc_handler);
	mmc_hw_ctrl_init();
	for (slot = 0; slot < NR_MMC_SLOTS; slot++) {
		sslot = mmc_slot_save(slot);
		mmc_reset_slot();
		mmc_irq_init_irq();
		mmc_slot_restore(sslot);
	}
	mmc_irq_init_noirq();
}
