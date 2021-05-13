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
 * @(#)dw_dma.c: Synopsys DesignWare DMA interface
 * $Id: dw_dma.c,v 1.0 2020-2-6 10:58:00 syl Exp $
 */

#include <target/dma.h>
#include <target/list.h>
#include <target/console.h>
#include <target/panic.h>

#define dw_dma2dma(id)			(dw_dma_chans[(id)].dma)
#define DW_DMA_POOL_SIZE		PAGE_SIZE

struct dw_dma_chan dw_dma_chans[DW_DMA_MAX_CHANS];
struct dw_dma_chip dw_dma_chips[DW_DMA_MAX_CHIPS];
struct dw_dma_desc dw_dma_descs[DW_DMA_MAX_DESCS];
struct dw_dma_pool dw_dma_pool;

static int dma_pool_init(void)
{
	uint8_t id;

	INIT_LIST_HEAD(&dw_dma_pool.descs);
	for (id = 0; id < DW_DMA_MAX_DESCS; id++) {
		INIT_LIST_HEAD(&dw_dma_descs[id].link);
		list_add(&dw_dma_descs[id].link, &dw_dma_pool.descs);
	}
	return 0;
}

static struct dw_dma_desc *dma_pool_alloc(void)
{
	struct dw_dma_desc *desc;

	if (list_empty(&dw_dma_pool.descs))
		return NULL;
	desc = list_first_entry(&dw_dma_pool.descs,
				struct dw_dma_desc, link);
	BUG_ON(!desc);
	/* TODO: DMA alloc coherent */
	desc->phys = (phys_addr_t)&desc->lli;
	list_del_init(&desc->link);
	return desc;
}

static void dma_pool_free(struct dw_dma_desc *desc)
{ 
	list_del_init(&desc->link);
}

static struct dw_dma_desc *dw_dma_get_desc(dma_t id)
{
	struct dw_dma_desc *desc;
	__unused int chip;
	int chan;

	BUG_ON(id >= DW_DMA_MAX_CHANS);
	chip = dw_dma_chans[id].chip;
	chan = dw_dma_chans[id].chan;

	desc = dma_pool_alloc();
	if (unlikely(!desc)) {
		con_err("dw_dma(%d:%d): not enough descriptors!\n",
			chip, chan);
		return NULL;
	}
	list_add(&desc->link, &dw_dma_chans[id].descs);
	desc->dma = dw_dma2dma(id);
	return desc;
}

static void dw_dma_put_desc(struct dw_dma_desc *desc)
{
	dma_pool_free(desc);
	list_add(&desc->link, &dw_dma_pool.descs);
}

static void dw_dma_put_descs(dma_t id)
{
	struct dw_dma_desc *desc, *next;

	list_for_each_entry_safe(struct dw_dma_desc, desc, next,
				 &dw_dma_chans[id].descs, link) {
		dw_dma_put_desc(desc);
	}
}

static uint32_t chan_get_xfer_width(dma_t id, dma_addr_t src,
				    dma_addr_t dst, size_t len)
{
	__unused int chip;

	BUG_ON(id >= DW_DMA_MAX_CHANS);
	chip = dw_dma_chans[id].chip;
	return __ffs16(src | dst | len | (DW_DMA_M_DATA_WIDTH(chip) / 8));
}

static void write_chan_llp(dma_t id, dma_addr_t addr, uint32_t lms)
{
	__unused int chip;
	int chan;

	BUG_ON(id >= DW_DMA_MAX_CHANS);
	chip = dw_dma_chans[id].chip;
	chan = dw_dma_chans[id].chan;
	__raw_writeq(addr | DMA_LMS(lms), DMA_CH_LLP(chip, chan));
}

static int chan_block_xfer_start(dma_t id, struct dw_dma_desc *first,
				 bool mult)
{
	__unused int chip;
	int chan;
	uint8_t priority;
	uint32_t irqs;

	BUG_ON(id >= DW_DMA_MAX_CHANS);
	chip = dw_dma_chans[id].chip;
	chan = dw_dma_chans[id].chan;
	priority = dw_dma_chans[id].priority;

	if (unlikely(dw_dma_channel_enabled(chip, chan))) {
		con_err("dw_dma(%d:%d): transfer start on busy!\n",
			chip, chan);
		return -EBUSY;
	}

	dw_dma_enable_ctrl(chip);

	dw_dmac_config_multblk(chip, chan,
			       mult ? DMA_MULTBLK_TYPE_LINKED_LIST : 0);
	dw_dmac_config_priority(chip, chan, priority);
	dw_dmac_config_handshaking_hw(chip, chan, false);
	switch (dma_direction(dw_dma_chans[id].dma)) {
	case DMA_MEM_TO_DEV:
		if (dw_dma_chans[id].device_fc)
			dw_dmac_config_tt_fc(chip, chan,
					     DMA_TT_MEM_TO_PER_FC_DST);
		else
			dw_dmac_config_tt_fc(chip, chan,
					     DMA_TT_MEM_TO_PER_FC_DMAC);
		break;
	case DMA_DEV_TO_MEM:
		if (dw_dma_chans[id].device_fc)
			dw_dmac_config_tt_fc(chip, chan,
					     DMA_TT_PER_TO_MEM_FC_SRC);
		else
			dw_dmac_config_tt_fc(chip, chan,
					     DMA_TT_PER_TO_MEM_FC_DMAC);
		break;
	default:
		dw_dmac_config_tt_fc(chip, chan,
				     DMA_TT_MEM_TO_MEM_FC_DMAC);
		break;
	}

	write_chan_llp(id, first->phys, DMA_MS_AXI1);

	irqs = DMA_DMA_TFR_DONE_Int | DW_DMAC_IRQ_ALL_ERR;
	dw_dmac_enable_irq(chip, chan, irqs);

	/* Generate 'suspend' status but don't generate interrupt */
	dw_dmac_enable_rirq(chip, chan, irqs | DMA_CH_SUSPENDED_Int);

	dw_dma_enable_channel(chip, chan);
	return 0;
}

static int chan_start_first_queued(dma_t id)
{
	struct dw_dma_desc *desc;

	BUG_ON(id >= DW_DMA_MAX_CHANS);
	desc = list_first_entry(&dw_dma_chans[id].descs,
				struct dw_dma_desc, link);
	return chan_block_xfer_start(id, desc, true);
}

int dw_dma_memcpy_async(dma_t id, dma_addr_t dst, dma_addr_t src, size_t len)
{
	struct dw_dma_desc *desc = NULL, *prev = NULL;
	size_t block_ts, max_block_ts, xfer_len;
	uint32_t xfer_width, burstlen;
	__unused int chip, chan;
	int ret = 0;
	struct dw_dma_lli *lli;
	uint8_t dinc = DMA_INCR, sinc = DMA_INCR;

	BUG_ON(id >= DW_DMA_MAX_CHANS);
	chip = dw_dma_chans[id].chip;
	chan = dw_dma_chans[id].chan;

	if (!list_empty(&dw_dma_chans[id].descs))
		return -EBUSY;

	con_dbg("dw_dma(%d:%d): memcpy src=%016llx dst=%016llx length=%d\n",
		chip, chan, src, dst, len);

	max_block_ts = DW_DMA_C_MAX_BLOCKTS(chip, chan);
	while (len) {
		desc = dw_dma_get_desc(id);
		if (unlikely(!desc)) {
			ret = -ENOMEM;
			goto err_desc;
		}
		lli = &desc->lli;

		xfer_len = len;
		xfer_width = chan_get_xfer_width(id, src, dst, len);

		/* block_ts indicates the total number of data of width
		 * to be transferred in a DMA block transfer.
		 *
		 * BLOCK_TS register should be set to block_ts - 1.
		 */
		block_ts = xfer_len >> xfer_width;
		if (block_ts > max_block_ts) {
			block_ts = max_block_ts;
			xfer_len = max_block_ts << xfer_width;
		}

		dw_dma_lli_set_ctl(lli);
		dw_dma_lli_set_sar(lli, src);
		dw_dma_lli_set_dar(lli, dst);
		dw_dma_lli_set_block_ts(lli, block_ts - 1);

		burstlen = _BV(DW_DMA_M_BURSTLEN_WIDTH(chip));
		if (burstlen > DW_DMA_C_MAX_BURSTLEN(chip, chan))
			burstlen = DW_DMA_C_MAX_BURSTLEN(chip, chan);
		dw_dma_lli_set_arlen(lli, burstlen);
		dw_dma_lli_set_awlen(lli, burstlen);

		switch (dma_direction(dw_dma_chans[id].dma)) {
		case DMA_MEM_TO_DEV:
			dinc = DMA_FIXED;
			break;
		case DMA_DEV_TO_MEM:
			sinc = DMA_FIXED;
			break;
		case DMA_DEV_TO_DEV:
			dinc = DMA_FIXED;
			sinc = DMA_FIXED;
			break;
		}
#if 1
		dw_dma_lli_set_dst(lli, DMA_MSIZE4, xfer_width, dinc);
		dw_dma_lli_set_src(lli, DMA_MSIZE4, xfer_width, sinc);
#else
		dw_dma_lli_set_dst(lli, DW_DMA_M_MAX_MSIZE(chip, chan),
				   xfer_width, dinc);
		dw_dma_lli_set_src(lli, DW_DMA_M_MAX_MSIZE(chip, chan),
				   xfer_width, sinc);
#endif

		/* Source master always uses AXI1 master */
		dw_dma_lli_set_sms(desc, DMA_MS_AXI1);
		/* Destination master selects AXI2 master if possible */
		if (dw_dma_chips[chip].nr_masters > 1)
			dw_dma_lli_set_dms(desc, DMA_MS_AXI2);
		else
			dw_dma_lli_set_dms(desc, DMA_MS_AXI1);

		if (prev)
			dw_dma_lli_set_llp(&prev->lli, desc->phys, DMA_MS_AXI1);
		prev = desc;

		len -= xfer_len;
		dst += xfer_len;
		src += xfer_len;
	}
	if (desc)
		dw_dma_lli_set_last(desc);

err_desc:
	if (ret)
		dw_dma_put_descs(id);
	return ret;
}

static void chan_block_xfer_complete(dma_t id)
{
	__unused int chip;
	int chan;

	BUG_ON(id >= DW_DMA_MAX_CHANS);
	chip = dw_dma_chans[id].chip;
	chan = dw_dma_chans[id].chan;

	if (unlikely(dw_dma_channel_enabled(chip, chan)))
		con_err("dw_dma(%d:%d): transfer stop on idle!\n",
			chip, chan);
	do_DMA(dw_dma_chans[id].dma, true);
	dw_dma_disable_channel(chip, chan);
	dw_dma_put_descs(id);
	chan_start_first_queued(id);
}

static void chan_handle_err(dma_t id, uint32_t status)
{
	__unused int chip;
	int chan;

	BUG_ON(id >= DW_DMA_MAX_CHANS);
	chip = dw_dma_chans[id].chip;
	chan = dw_dma_chans[id].chan;

	do_DMA(dw_dma_chans[id].dma, false);
	dw_dma_disable_channel(chip, chan);
	dw_dma_put_descs(id);
	chan_start_first_queued(id);
}

irq_t dw_dma_chip_irq(dma_t id)
{
	int chip;

	BUG_ON(id >= DW_DMA_MAX_CHANS);
	chip = dw_dma_chans[id].chip;
	return dw_dma_chips[chip].irq;
}

irq_t dw_dma_chan_irq(dma_t id)
{
	return dw_dma_chans[id].irq;
}

int dw_dma_irq2chip(irq_t irq)
{
	dma_t id;

	for (id = 0; id < DW_DMA_MAX_CHANS; id++) {
		if (dw_dma_chip_irq(id) == irq ||
		    dw_dma_chan_irq(id) == irq)
			return dw_dma_chans[id].chip;
	}
	return INVALID_DW_DMA_CHIP;
}

static void dw_dma_interrupt(irq_t irq)
{
	__unused int chip, chan;
	uint32_t status;

	chip = dw_dma_irq2chip(irq);
	dw_dma_disable_irq(chip);

	/* Poll, clear and process every chanel interrupt status */
	for (chan = 0; chan < dw_dma_chips[chip].nr_channels; chan++) {
		status = dw_dmac_irq_status(chip, chan);
		dw_dmac_clear_irq(chip, chan, status);

		con_dbg("dw_dma(%d:%d): IRQ status: 0x%08x\n",
			chip, chan, status);

		if (status & DW_DMAC_IRQ_ALL_ERR)
			chan_handle_err(chan, status);
		else if (status & DMA_DMA_TFR_DONE_Int)
			chan_block_xfer_complete(chan);
	}

	dw_dma_enable_irq(chip);
}

int dw_dma_memcpy_sync(dma_t id, dma_addr_t dst, dma_addr_t src, size_t len)
{
	int ret;
	irq_flags_t flags;
	__unused int chip, chan;

	BUG_ON(id >= DW_DMA_MAX_CHANS);
	chip = dw_dma_chans[id].chip;
	chan = dw_dma_chans[id].chan;

	/* prepare data */
	ret = dw_dma_memcpy_async(id, dst, src, len);
	if (ret)
		return ret;

	/* start tx */
	(void)chan_start_first_queued(id);

	while (unlikely(dw_dma_channel_enabled(chip, chan))) {
		irq_local_save(flags);
		irq_local_enable();
		bh_sync();
		irq_local_restore(flags);
		irq_local_disable();
	}
	return 0;
}

#ifdef SYS_REALTIME
void dw_dma_irq_poll(void)
{
	int chip;

	for (chip = 0; chip < DW_DMA_MAX_CHIPS; chip++)
		dw_dma_interrupt(dw_dma_chips[chip].irq);
}

#define dw_dma_irq_ack(irq)	do { } while (0)
#else
#define dw_dma_irq_ack(irq)	irqc_ack_irq(irq);

static void dw_dma_chan_irq_handler(irq_t irq)
{
	dw_dma_interrupt(irq);
	dw_dma_irq_ack(irq);
}

static void dw_dma_chip_irq_handler(irq_t irq)
{
	dw_dma_interrupt(irq);
	dw_dma_irq_ack(irq);
}

void dw_dma_irq_init(void)
{
	irq_t irq = dma_chan.irq;

	irqc_configure_irq(irq, 0, IRQ_LEVEL_TRIGGERED);
	irq_register_vector(irq, dw_dma_irq_handler);
	irqc_enable_irq(irq);
}
#endif

void dw_dma_chan_init(dma_t id, int chip, int chan, dma_t dma, irq_t irq)
{
	dw_dma_chans[id].chip = chip;
	dw_dma_chans[id].chan = chan;
	dw_dma_chans[id].dma = dma;
	dw_dma_chans[id].irq = irq;
	dw_dmac_disable_all_rirqs(chip, chan);
	dw_dma_disable_channel(chip, chan);
	INIT_LIST_HEAD(&dw_dma_chans[id].descs);
}

void dw_dma_chip_init(int chip, irq_t irq)
{
	dw_dma_chips[chip].irq = irq;
	dw_dma_disable_irq(chip);
	dw_dma_enable_ctrl(chip);
	dw_dma_reset_ctrl(chip);
	dma_pool_init();
}
