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
 * @(#)card.c: MMC/SD memory card implementation
 * $Id: card.c,v 1.1 2019-12-16 21:25:00 zhenglv Exp $
 */

#include <target/bh.h>
#include <target/mmc.h>
#include <target/page.h>
#include <target/cmdline.h>
#include <target/mem.h>
#include <target/irq.h>

#ifdef CONFIG_MMC_SELECTED_CARD
#define MMC_SELECTED_CARD	CONFIG_MMC_SELECTED_CARD
#else
#define MMC_SELECTED_CARD	-1
#endif

struct mem_card {
	mmc_slot_t sid;
	mmc_rca_t rca;

	uint8_t *buf;
	mmc_lba_t lba;
	size_t cnt;
	bool res;

	uint8_t tran;
};

void mmc_slot_complete(mmc_rca_t rca, uint8_t op, bool result);

struct mem_card mem_cards[NR_MMC_CARDS];
mmc_card_t mmc_nr_cards = 0;

mmc_card_t mmc_rca2card(mmc_rca_t rca)
{
	mmc_card_t cid;

	for (cid = 0; cid < mmc_nr_cards; cid++) {
		if (mmc_sid == mem_cards[cid].sid &&
		    mem_cards[cid].rca == rca)
			return cid;
	}
	return INVALID_MMC_CARD;
}

mmc_slot_t mmc_card_slot(mmc_card_t card)
{
	if (card >= mmc_nr_cards)
		return INVALID_MMC_SLOT;
	return mem_cards[card].sid;
}

bool mmc_card_capacity(mmc_card_t cid, uint16_t *blk_len, uint64_t *blk_cnt)
{
	__unused mmc_slot_t sslot;
	bool result = false;

	sslot = mmc_slot_save(mmc_card_slot(cid));
	if (mmc_slot_ctrl.capacity_cnt) {
		*blk_len = mmc_slot_ctrl.capacity_len;
		*blk_cnt = mmc_slot_ctrl.capacity_cnt;
		result = true;
	}
	mmc_slot_restore(sslot);
	return result;
}

mmc_card_t mmc_register_card(mmc_rca_t rca)
{
	mmc_card_t cid;

	BUG_ON(mmc_nr_cards >= INVALID_MMC_CARD);
	cid = mmc_nr_cards;
	mem_cards[cid].sid = mmc_sid;
	mem_cards[cid].rca = rca;
	mem_cards[cid].tran = 0;
	if (mmc_nr_cards == MMC_SELECTED_CARD)
		mmc_select_card(NULL);
	mmc_nr_cards++;
	return cid;
}

void mmc_slot_start_tran(mmc_rca_t rca)
{
	mmc_card_t cid;

	cid = mmc_rca2card(rca);
	BUG_ON(cid == INVALID_MMC_CARD);

	/* TODO: multi-cards support, pass rca to mmc_read_blocks() */
	BUG_ON(mmc_slot_ctrl.rca != rca);
	if (mem_cards[cid].tran == MMC_OP_READ_BLOCKS)
		mmc_read_blocks(mem_cards[cid].buf, mem_cards[cid].lba,
				mem_cards[cid].cnt, mmc_slot_complete);
	else if (mem_cards[cid].tran == MMC_OP_WRITE_BLOCKS)
		mmc_write_blocks(mem_cards[cid].buf, mem_cards[cid].lba,
				mem_cards[cid].cnt, mmc_slot_complete);
}

void mmc_slot_complete(mmc_rca_t rca, uint8_t op, bool result)
{
	mmc_card_t cid;

	cid = mmc_rca2card(rca);
	if (cid == INVALID_MMC_CARD)
		return;
	if (!result)
		goto err_exit;

	switch (op) {
	case MMC_OP_SELECT_CARD:
		if (mem_cards[cid].tran != 0) {
			mmc_slot_start_tran(rca);
			return;
		}
		break;
	case MMC_OP_READ_BLOCKS:
	default:
		break;
	}

err_exit:
	mem_cards[cid].res = result;
	mem_cards[cid].tran = 0;
}

int mmc_card_read_async(mmc_card_t cid, uint8_t *buf,
			mmc_lba_t lba, mmc_lba_t cnt)
{
	__unused mmc_slot_t sslot;

	if (!buf)
		return -EINVAL;
	if (cid >= mmc_nr_cards)
		return -ENODEV;
	if (mem_cards[cid].tran)
		return -EBUSY;
	mem_cards[cid].tran = MMC_OP_READ_BLOCKS;
	mem_cards[cid].buf = buf;
	mem_cards[cid].lba = lba;
	mem_cards[cid].cnt = cnt;
	mem_cards[cid].res = false;
	sslot = mmc_slot_save(mmc_card_slot(cid));
	mmc_slot_start_tran(mem_cards[cid].rca);
	mmc_slot_restore(sslot);
	return 0;
}

int mmc_card_write_async(mmc_card_t cid, uint8_t *buf,
			 mmc_lba_t lba, mmc_lba_t cnt)
{
	__unused mmc_slot_t sslot;

	if (!buf)
		return -EINVAL;
	if (cid >= mmc_nr_cards)
		return -ENODEV;
	if (mem_cards[cid].tran)
		return -EBUSY;
	mem_cards[cid].tran = MMC_OP_WRITE_BLOCKS;
	mem_cards[cid].buf = buf;
	mem_cards[cid].lba = lba;
	mem_cards[cid].cnt = cnt;
	mem_cards[cid].res = false;
	sslot = mmc_slot_save(mmc_card_slot(cid));
	mmc_slot_start_tran(mem_cards[cid].rca);
	mmc_slot_restore(sslot);
	return 0;
}

bool mmc_card_busy(mmc_card_t cid)
{
	__unused mmc_slot_t sslot;
	bool busy;

	sslot = mmc_slot_save(mmc_card_slot(cid));
	busy = mmc_op_busy();
	mmc_slot_restore(sslot);
	return busy;
}

int mmc_card_read_sync(mmc_card_t cid, uint8_t *buf,
		       mmc_lba_t lba, mmc_lba_t cnt)
{
	int ret;
	irq_flags_t flags;

	ret = mmc_card_read_async(cid, buf, lba, cnt);
	if (ret)
		return ret;
	while (mmc_card_busy(cid)) {
		irq_local_save(flags);
		irq_local_enable();
		bh_sync();
		irq_local_disable();
		irq_local_restore(flags);
	}
	return mem_cards[cid].res ? 0 : -EINVAL;
}

int mmc_card_write_sync(mmc_card_t cid, uint8_t *buf,
			mmc_lba_t lba, mmc_lba_t cnt)
{
	int ret;
	irq_flags_t flags;

	ret = mmc_card_write_async(cid, buf, lba, cnt);
	if (ret)
		return ret;
	while (mmc_card_busy(cid)) {
		irq_local_save(flags);
		irq_local_enable();
		bh_sync();
		irq_local_disable();
		irq_local_restore(flags);
	}
	return mem_cards[cid].res ? 0 : -EINVAL;
}

static int do_card_list(int argc, char *argv[])
{
	mmc_card_t cid;

	for (cid = 0; cid < mmc_nr_cards; cid++) {
		/* cid should equals to CARD */
		printf("CARD\tSLOT\tRCA\n");
		printf("%d\t%d\t%d\n", cid, mem_cards[cid].sid,
		       mem_cards[cid].rca);
	}
	return 0;
}

__align(MMC_DATA_ALIGN) uint8_t mem_card_buf[2 * MMC_DEF_BL_LEN];

static int do_card_read(int argc, char *argv[])
{
	mmc_card_t cid;
	mmc_lba_t lba = 0;
	size_t cnt = 1;

	if (argc < 3)
		return -EINVAL;
	cid = strtoul(argv[2], 0, 0);
	if (argc > 3)
		lba = (mmc_lba_t)strtoul(argv[3], 0, 0);
	if (argc > 4)
		cnt = (size_t)strtoul(argv[4], 0, 0);
	if (cnt > 2)
		cnt = 2;
	if (cnt < 1)
		cnt = 1;
	memset(mem_card_buf, 0, 2 * MMC_DEF_BL_LEN);
	if (mmc_card_read_sync(cid, mem_card_buf, lba, cnt))
		printf("read_blocks %016x(%d) failure.\n", lba, cnt);
	else
		hexdump(0, mem_card_buf, 1, cnt * MMC_DEF_BL_LEN);
	return 0;
}

static int do_card_write(int argc, char *argv[])
{
	mmc_card_t cid;
	mmc_lba_t lba = 0;
	size_t cnt = 1;
	uint8_t byte = 'T';

	if (argc < 3)
		return -EINVAL;
	cid = strtoul(argv[2], 0, 0);
	if (argc > 3)
		lba = (mmc_lba_t)strtoul(argv[3], 0, 0);
	if (argc > 4)
		cnt = (size_t)strtoul(argv[4], 0, 0);
	if (cnt > 2)
		cnt = 2;
	if (cnt < 1)
		cnt = 1;
	if (argc > 5)
		byte = (uint8_t)strtoul(argv[5], 0, 0);
	memset(mem_card_buf, byte, 2 * MMC_DEF_BL_LEN);
	for (int i = 0; i < (2*MMC_DEF_BL_LEN); i += 16)
		*((uint8_t *)mem_card_buf + i) = byte + (uint8_t)i;
	if (mmc_card_write_sync(cid, mem_card_buf, lba, cnt))
		printf("write_blocks %016x(%d) failure.\n", lba, cnt);
	return 0;
}

static int do_card(int argc, char *argv[])
{
	if (argc < 2)
		return -EINVAL;

	if (strcmp(argv[1], "list") == 0)
		return do_card_list(argc, argv);
	if (strcmp(argv[1], "read") == 0)
		return do_card_read(argc, argv);
	if (strcmp(argv[1], "write") == 0)
		return do_card_write(argc, argv);
	return 0;
}

DEFINE_COMMAND(mmcsd, do_card, "MMC/SD card commands",
	"list\n"
	"    - list slots and cards\n"
	"read card lba cnt\n"
	"    - read content of number (cnt) of blocks (lba)\n"
	"write card lba cnt byte\n"
	"    - write content of number (cnt) of blocks (lba)\n"
);
