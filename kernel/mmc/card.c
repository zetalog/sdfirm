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

void mmc_card_complete(mmc_rca_t rca, uint8_t op, bool result);

struct mem_card mem_cards[NR_MMC_CARDS];
mmc_card_t mmc_nr_cards = 0;

mmc_card_t mmc_rca2card(mmc_rca_t rca)
{
	mmc_card_t cid;

	for (cid = 0; cid < mmc_nr_cards; cid++) {
		if (mem_cards[cid].rca == rca)
			return cid;
	}
	return INVALID_MMC_CARD;
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
		mmc_select_card(mmc_card_complete);
	mmc_nr_cards++;
	return cid;
}

void mmc_card_select_card(mmc_rca_t rca)
{
	__unused mmc_slot_t sslot;

	sslot = mmc_slot_save(MMC_SLOT(rca));
	mmc_select_card(mmc_card_complete);
	mmc_slot_restore(sslot);
}

void mmc_card_deselect_card(mmc_rca_t rca)
{
	__unused mmc_slot_t sslot;

	sslot = mmc_slot_save(MMC_SLOT(rca));
	mmc_deselect_card(mmc_card_complete);
	mmc_slot_restore(sslot);
}

void mmc_card_read_blocks(mmc_rca_t rca, uint8_t *buf,
			  mmc_lba_t lba, size_t cnt)
{
	__unused mmc_slot_t sslot;

	sslot = mmc_slot_save(MMC_SLOT(rca));
	mmc_read_blocks(buf, lba, cnt,
			mmc_card_complete);
	mmc_slot_restore(sslot);
}

void mmc_card_start_tran(mmc_rca_t rca)
{
	mmc_card_t cid;

	cid = mmc_rca2card(rca);
	BUG_ON(cid == INVALID_MMC_CARD);

	if (mem_cards[cid].tran == MMC_OP_READ_BLOCKS)
		mmc_card_read_blocks(rca, mem_cards[cid].buf,
				     mem_cards[cid].lba,
				     mem_cards[cid].cnt);
}

void mmc_card_complete(mmc_rca_t rca, uint8_t op, bool result)
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
			mmc_card_start_tran(rca);
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

int mmc_card_read_async(mmc_rca_t rca, uint8_t *buf,
			mmc_lba_t lba, size_t cnt)
{
	__unused mmc_slot_t sslot;
	mmc_card_t cid;

	if (!buf)
		return -EINVAL;
	cid = mmc_rca2card(rca);
	if (cid == INVALID_MMC_CARD)
		return -ENODEV;
	if (mem_cards[cid].tran)
		return -EBUSY;
	mem_cards[cid].tran = MMC_OP_READ_BLOCKS;
	mem_cards[cid].buf = buf;
	mem_cards[cid].lba = lba;
	mem_cards[cid].cnt = cnt;
	mem_cards[cid].res = false;
	if (!(mmc_slot_ctrl.flags & MMC_SLOT_CARD_SELECT))
		mmc_card_select_card(rca);
	else
		mmc_card_start_tran(rca);
	return 0;
}

int mmc_card_read_sync(mmc_rca_t rca, uint8_t *buf,
		       mmc_lba_t lba, size_t cnt)
{
	int ret;
	mmc_card_t cid;

	ret = mmc_card_read_async(rca, buf, lba, cnt);
	if (ret)
		return ret;
	cid = mmc_rca2card(rca);
	BUG_ON(cid == INVALID_MMC_CARD);
	bh_sync();
	return mem_cards[cid].res ? 0 : -EINVAL;
}

static int do_card_list(int argc, char *argv[])
{
	mmc_card_t cid;

	for (cid = 0; cid < mmc_nr_cards; cid++) {
		/* cid should equals to CARD */
		printf("RCA\tSLOT\tCARD\n");
		printf("%d\t%d\t%d\n", mem_cards[cid].rca,
		       mem_cards[cid].sid, cid);
	}
	return 0;
}

__align(MMC_DATA_ALIGN) uint8_t mem_card_buf[2 * MMC_DEF_BL_LEN];
static int do_card_dump(int argc, char *argv[])
{
	mmc_rca_t rca;
	mmc_lba_t lba = 0;
	size_t cnt = 1;

	if (argc < 3)
		return -EINVAL;
	rca = strtoul(argv[2], 0, 0);
	if (argc > 3)
		lba = (mmc_lba_t)strtoul(argv[3], 0, 0);
	if (argc > 4)
		cnt = (size_t)strtoul(argv[4], 0, 0);
	if (cnt > 2)
		cnt = 2;
	if (cnt < 1)
		cnt = 1;
	memset(mem_card_buf, 0, 2 * MMC_DEF_BL_LEN);
	if (mmc_card_read_sync(rca, mem_card_buf, lba, cnt))
		printf("read_blocks %016x(%d) failure.\n", lba, cnt);
	else
		mem_print_data(0, mem_card_buf, 1,
			       cnt * MMC_DEF_BL_LEN);
	return 0;
}

static int do_card(int argc, char *argv[])
{
	if (argc < 2)
		return -EINVAL;

	if (strcmp(argv[1], "list") == 0)
		return do_card_list(argc, argv);
	if (strcmp(argv[1], "dump") == 0)
		return do_card_dump(argc, argv);
	return 0;
}

DEFINE_COMMAND(mmcsd, do_card, "MMC/SD card commands",
	"list\n"
	"    - list slots and cards\n"
	"dump rca lba cnt\n"
	"    - dump content of number (cnt) of blocks (lba)\n"
);
