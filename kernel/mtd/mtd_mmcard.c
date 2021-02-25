/*
 * ZETALOG's Personal COPYRIGHT
 *
 * Copyright (c) 2021
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
 * @(#)mmcard.c: MTD implementation for MMC/SD cards
 * $Id: mmcard.c,v 1.1 2021-02-24 15:46:00 zhenglv Exp $
 */

#include <target/mmc.h>
#include <target/mtd.h>

struct mmcard_info {
	mmc_card_t cid;
	mtd_t mtd;
	mtd_addr_t offset;
	mtd_size_t length;
	uint64_t blk_cnt;
	uint16_t blk_len;
	bool registered;
	bool buffered;
	uint8_t buffer[MMC_DEF_BL_LEN];
	mtd_addr_t buffered_address;
	uint16_t nr_buffered_blks;
};

struct mtd_info mmcard_infos[NR_MMC_CARDS];
struct mmcard_info mmcard_privs[NR_MMC_CARDS];
mtd_cid_t mmcard_cid = INVALID_MTD_CID;

static mmc_card_t mmcard_mtd2cid(mtd_t mtd)
{
	mmc_card_t cid = 0;

	for (cid = 0; cid < NR_MMC_CARDS; cid++) {
		if (mmcard_privs[cid].registered &&
		    mtd == mmcard_privs[cid].mtd)
			return cid;
	}
	return INVALID_MMC_CARD;
}

static bool mmcard_buffered(mtd_addr_t addr)
{
	mtd_addr_t blk_off;
	uint16_t blk_cnt;
	int result;

	blk_off = ALIGN_DOWN(mmcard_privs[mmcard_cid].offset,
			     MMC_DEF_BL_LEN);
	if (mmcard_privs[mmcard_cid].buffered &&
	    blk_off == mmcard_privs[mmcard_cid].buffered_address)
		return true;
	blk_cnt = MMC_DEF_BL_LEN / mmcard_privs[mmcard_cid].blk_len;

	result = mmc_card_read_sync(mmcard_cid,
				    mmcard_privs[mmcard_cid].buffer,
				    blk_off, blk_cnt);
	if (result != 0) {
		con_printf("Failed to read %08lx\n", blk_off);
		return false;
	}
	mmcard_privs[mmcard_cid].buffered_address = blk_off;
	mmcard_privs[mmcard_cid].nr_buffered_blks = blk_cnt;
	mmcard_privs[mmcard_cid].buffered = true;
	return true;
}

static uint8_t mmcard_read(void)
{
	uint8_t byte = 0;
	mtd_addr_t offset;

	if (mmcard_buffered(mmcard_privs[mmcard_cid].offset)) {
		offset = mmcard_privs[mmcard_cid].offset -
			 mmcard_privs[mmcard_cid].buffered_address;
		if (offset >= MMC_DEF_BL_LEN)
			con_printf("Wrong offset %016llx\n", offset);
		else
			byte = mmcard_privs[mmcard_cid].buffer[offset];
		mmcard_privs[mmcard_cid].offset++;
		mmcard_privs[mmcard_cid].length--;
	} else {
		con_printf("Failed to buffer offset %016llx\n",
			   mmcard_privs[mmcard_cid].offset);
	}
	return byte;
}

static void mmcard_erase(mtd_t mtd, mtd_addr_t addr, mtd_size_t size)
{
	/* NYI */
}

static void mmcard_write(uint8_t byte)
{
	/* NYI */
}

static boolean mmcard_open(uint8_t mode, mtd_addr_t addr, mtd_size_t size)
{
	mmc_card_t cid = mmcard_mtd2cid(mtd_id);

	if (cid >= NR_MMC_CARDS || !mmcard_privs[cid].registered)
		return false;

	mmcard_cid = cid;
	mmcard_privs[cid].offset = addr;
	mmcard_privs[cid].length = size;
	return true;
}

static void mmcard_close(void)
{
	mmcard_privs[mmcard_cid].buffered = false;
}

mtd_chip_t mmcard_chip = {
	mmcard_erase,
	mmcard_open,
	mmcard_close,
	mmcard_read,
	mmcard_write,
};

mtd_t mmcard_register_card(mmc_card_t cid)
{
	uint16_t blk_len;
	uint64_t blk_cnt;
	
	/* Already registered */
	if (mmcard_privs[cid].registered)
		return INVALID_MTD_ID;

	if (!mmc_card_capacity(cid, &blk_len, &blk_cnt))
		return INVALID_MTD_ID;

	if (mmcard_cid == INVALID_MTD_CID) {
		mmcard_cid = mtd_register_chip(&mmcard_chip);
		BUG_ON(mmcard_cid == INVALID_MTD_CID);
	}

	/* register MTD device */
	mmcard_infos[cid].cid = mmcard_cid;
	mmcard_infos[cid].pageorder = __fls16(MMC_DEF_BL_LEN);
	mmcard_infos[cid].eraseorder = __fls16(MMC_DEF_BL_LEN);
	mmcard_infos[cid].nr_pages = blk_cnt * (blk_len / MMC_DEF_BL_LEN);

	mmcard_privs[cid].mtd = mtd_add_device(&mmcard_infos[cid]);
	mmcard_privs[cid].cid = cid;
	mmcard_privs[cid].blk_len = blk_len;
	mmcard_privs[cid].blk_cnt = blk_cnt;
	mmcard_privs[cid].registered = true;
	return mmcard_privs[cid].mtd;
}
