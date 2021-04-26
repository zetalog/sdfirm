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

dma_chip_t chip_var;
dma_chip_t *chip = &chip_var;
dma_chan_t chan_var[DW_DMA_MAX_CHANNELS];

dw_dma_hcfg_t  hdata_var;
dw_dma_hcfg_t *hdata = &hdata_var;

dma_desc_t dma_desc_buffer[DW_DMA_POOL_NUM];
uint8_t phys_buffer[DW_DMA_POOL_NUM][DMAC_MAX_BLK_SIZE];
desc_stuct_t  axi_desc;
desc_stuct_t *ptr_axi_desc = &axi_desc;

#define DMA_SINGLE_LLI_WIDTH	0x40
#define DW_DMA_MAX_POOLS	10
uint64_t u64LocADDR[DW_DMA_MAX_POOLS];
uint64_t u64POOL_ADDR[DW_DMA_MAX_POOLS][DMA_SINGLE_LLI_WIDTH];

static inline void
dma_reg_set32(dma_chip_t *chip, uint32_t reg, uint32_t val)
{
	__raw_writel(val, chip->reg_base + reg);
}

static inline uint32_t dma_reg_get32(dma_chip_t *chip, uint32_t reg)
{
	return __raw_readl(chip->reg_base + reg);
}

static inline void
chan_reg_set32(dma_chan_t *chan, uint32_t reg, uint32_t val)
{
	__raw_writel(val, chan->chan_reg_base + reg);
}

static inline uint32_t chan_reg_get32(dma_chan_t *chan, uint32_t reg)
{
	return __raw_readl(chan->chan_reg_base + reg);
}

static inline void
chan_reg_set64(dma_chan_t *chan, uint32_t reg, uint64_t val)
{
	__raw_writel(lower_32_bits(val), chan->chan_reg_base + reg);
	__raw_writel(upper_32_bits(val), chan->chan_reg_base + reg + 4);
}

static inline void dma_disable(dma_chip_t *chip)
{
	uint32_t val;
        
	val = dma_reg_get32(chip, DMAC_CFG);
	val &= ~DMAC_EN_MASK;
	dma_reg_set32(chip, DMAC_CFG, val);
}

static inline void dma_enable(dma_chip_t *chip)
{
	uint32_t val;
        
	val = dma_reg_get32(chip, DMAC_CFG);
	val |= DMAC_EN_MASK;
	dma_reg_set32(chip, DMAC_CFG, val);
}

static inline void dma_irq_disable(dma_chip_t *chip)
{
	uint32_t val;

	val = dma_reg_get32(chip, DMAC_CFG);
	val &= ~INT_EN_MASK;
	dma_reg_set32(chip, DMAC_CFG, val);
}

static inline void dma_irq_enable(dma_chip_t *chip)
{
	uint32_t val;

	val = dma_reg_get32(chip, DMAC_CFG);
	val |= INT_EN_MASK;
	dma_reg_set32(chip, DMAC_CFG, val);
}

static inline void chan_irq_disable(dma_chan_t *chan, uint32_t irq_mask)
{
	uint32_t val;

	if (likely(irq_mask == DW_DMAC_IRQ_ALL)) {
		chan_reg_set32(chan, CH_INTSTATUS_ENA, DW_DMAC_IRQ_NONE);
	} else {
		val = chan_reg_get32(chan, CH_INTSTATUS_ENA);
		val &= ~irq_mask;
		chan_reg_set32(chan, CH_INTSTATUS_ENA, val);
	}
}

static inline void chan_irq_set(dma_chan_t *chan, uint32_t irq_mask)
{
	chan_reg_set32(chan, CH_INTSTATUS_ENA, irq_mask);
}

static inline void chan_irq_sig_set(dma_chan_t *chan, uint32_t irq_mask)
{
	chan_reg_set32(chan, CH_INTSIGNAL_ENA, irq_mask);
}

static inline void chan_irq_clear(dma_chan_t *chan, uint32_t irq_mask)
{
	chan_reg_set32(chan, CH_INTCLEAR, irq_mask);
}

static inline uint32_t chan_irq_read(dma_chan_t *chan)
{
	return chan_reg_get32(chan, CH_INTSTATUS);
}

static inline void chan_disable(dma_chan_t *chan)
{
	uint32_t val;

	val = dma_reg_get32(chan->chip, DMAC_CHEN);
	val &= ~(_BV(chan->id) << DMAC_CHAN_EN_SHIFT);
	val |=   _BV(chan->id) << DMAC_CHAN_EN_WE_SHIFT;
	dma_reg_set32(chan->chip, DMAC_CHEN, val);
}

static inline void chan_enable(dma_chan_t *chan)
{
	uint32_t val;

	val = dma_reg_get32(chan->chip, DMAC_CHEN);
	val |= _BV(chan->id) << DMAC_CHAN_EN_SHIFT |
	       _BV(chan->id) << DMAC_CHAN_EN_WE_SHIFT;
	dma_reg_set32(chan->chip, DMAC_CHEN, val);
}

static inline bool chan_is_hw_enable(dma_chan_t *chan)
{
	uint32_t val;

	val = dma_reg_get32(chan->chip, DMAC_CHEN);

	return !!(val & (_BV(chan->id) << DMAC_CHAN_EN_SHIFT));
}

static void dma_hw_init(dma_chip_t *chip)
{
	uint32_t i;

	for (i = 0; i < chip->hdata->nr_channels; i++) {
		chan_irq_disable(chip->chan[i], DW_DMAC_IRQ_ALL);
		chan_disable(chip->chan[i]);
	}
}

static uint32_t chan_get_xfer_width(dma_chan_t *chan, dma_addr_t src,
				   dma_addr_t dst, size_t len)
{
	uint32_t max_width = chan->chip->hdata->m_data_width;

	return __ffs32(src | dst | len | _BV(max_width));
}

static int dma_pool_init(void)
{
	int8_t cnt;

	ptr_axi_desc->alloc_index = 0;
	ptr_axi_desc->allocated_num = 0;
	ptr_axi_desc->free_num = 0;

	for (cnt = 0; cnt < DW_DMA_POOL_NUM; cnt++) {
		dma_desc_buffer[cnt].desc_id = cnt;
		dma_desc_buffer[cnt].desc_status = 0;
	}
	return 0;
}

static inline dma_desc_t *dma_desc_alloc(dma_addr_t *handle)
{
	dma_desc_t *desc;
	int32_t free_index;

	if (ptr_axi_desc->allocated_num - ptr_axi_desc->free_num ==
	    DW_DMA_POOL_NUM)
		return NULL;
    
	desc = &dma_desc_buffer[ptr_axi_desc->alloc_index];
	desc->desc_status = ALLOCATED;
    
	handle = (dma_addr_t *)&phys_buffer[ptr_axi_desc->alloc_index];

	free_index = ptr_axi_desc->alloc_index++;
	free_index = free_index%DW_DMA_POOL_NUM;
	desc = &dma_desc_buffer[free_index];
    
	while (desc->desc_status == ALLOCATED) {
		free_index++;
		free_index = free_index%DW_DMA_POOL_NUM;
		desc = &dma_desc_buffer[free_index];
	}
	ptr_axi_desc->alloc_index = free_index;
	return desc;
}

static inline void dma_desc_free(dma_desc_t *desc)
{ 
	int32_t	desc_id;

	desc_id = desc->desc_id;
	memset(&dma_desc_buffer[desc_id], 0, sizeof(dma_desc_t));
	memset(phys_buffer[desc_id], 0, DMAC_MAX_BLK_SIZE);
	desc->desc_status = 0;
	desc->desc_id = desc_id;
	ptr_axi_desc->free_num ++;
}

static dma_desc_t *axi_desc_get(dma_chan_t *chan)
{
	dma_desc_t *desc;
	dma_addr_t phys_tmp = 0;

	desc = dma_desc_alloc(&phys_tmp);
	if (unlikely(!desc)) {
		con_err("dw_dma: chan %d: not enough descriptors\n",
			chan->id);
		return NULL;
	}
	if (chan->descs_allocated == 0)
		chan->first = desc;
	chan->descs_allocated++;
	INIT_LIST_HEAD(&desc->desc_list);
	desc->phys = phys_tmp;
	desc->chan = chan;
	return desc;
}

static void axi_desc_put(dma_desc_t *desc)
{
	dma_chan_t *chan = desc->chan;
	dma_desc_t *child, *_next;
	unsigned int descs_put = 0;

	list_for_each_entry_safe(dma_desc_t, child, _next,
				 &desc->desc_list, desc_list) {
		list_del(&child->desc_list);
		dma_desc_free(child);
		descs_put++;
	}

	dma_desc_free(desc);
	descs_put++;
	chan->descs_allocated = chan->descs_allocated -descs_put;
	con_log("dw_dma: chan %d: %d descs put, %d still allocated\n",
		chan->id,descs_put,chan->descs_allocated);
}

static void write_desc_llp(dma_desc_t *desc, dma_addr_t adr)
{
	desc->lli.llp = cpu_to_le64(adr);
}

static void write_chan_llp(dma_chan_t *chan, dma_addr_t adr)
{
	chan_reg_set64(chan, CH_LLP, adr);
}

static int chan_block_xfer_start(dma_chan_t *chan, dma_desc_t *first)
{
	uint32_t priority = chan->chip->hdata->priority[chan->id];
	uint32_t reg, irq_mask;
	uint8_t lms = 0;

	if (unlikely(chan_is_hw_enable(chan))) {
		con_err("dw_dma: chan %d : is non-idle!\n", chan->id);
		return DMA_ERROR;
	}

	dma_enable(chan->chip);

	reg = (DW_DMAC_MBLK_TYPE_LL << CH_CFG_L_DST_MULTBLK_TYPE_POS |
	       DW_DMAC_MBLK_TYPE_LL << CH_CFG_L_SRC_MULTBLK_TYPE_POS);
	chan_reg_set32(chan, CH_CFG_L, reg);

	reg = (DW_DMAC_TT_FC_MEM_TO_MEM_DMAC << CH_CFG_H_TT_FC_POS |
	       priority << CH_CFG_H_PRIORITY_POS |
	       DW_DMAC_HS_SEL_HW << CH_CFG_H_HS_SEL_DST_POS |
	       DW_DMAC_HS_SEL_HW << CH_CFG_H_HS_SEL_SRC_POS);
	chan_reg_set32(chan, CH_CFG_H, reg);

	write_chan_llp(chan, first->phys | lms);

	irq_mask = DW_DMAC_IRQ_DMA_TRF | DW_DMAC_IRQ_ALL_ERR;
	chan_irq_sig_set(chan, irq_mask);

	/* Generate 'suspend' status but don't generate interrupt */
	irq_mask |= DW_DMAC_IRQ_SUSPENDED;
	chan_irq_set(chan, irq_mask);

	chan_enable(chan);
	return DMA_OK;
}

static int32_t chan_block_xfer_start_single(dma_chan_t *chan,
				      dma_desc_t *first)
{
	uint32_t priority = chan->chip->hdata->priority[chan->id];
	uint32_t reg, irq_mask;
	uint8_t lms = 0;

	if (unlikely(chan_is_hw_enable(chan))) {
		con_err("dw_dma: chan %d : is non-idle!\n", chan->id);
		return DMA_ERROR;
	}

	dma_enable(chan->chip);

	reg = (0 << CH_CFG_L_DST_MULTBLK_TYPE_POS |
	       0 << CH_CFG_L_SRC_MULTBLK_TYPE_POS);
	chan_reg_set32(chan, CH_CFG_L, reg);

	reg = (DW_DMAC_TT_FC_MEM_TO_MEM_DMAC << CH_CFG_H_TT_FC_POS |
	       priority << CH_CFG_H_PRIORITY_POS |
	       DW_DMAC_HS_SEL_HW << CH_CFG_H_HS_SEL_DST_POS |
	       DW_DMAC_HS_SEL_HW << CH_CFG_H_HS_SEL_SRC_POS);
	chan_reg_set32(chan, CH_CFG_H, reg);

	write_chan_llp(chan, first->phys | lms);

	irq_mask = DW_DMAC_IRQ_DMA_TRF | DW_DMAC_IRQ_ALL_ERR;
	chan_irq_sig_set(chan, irq_mask);

	irq_mask |= DW_DMAC_IRQ_SUSPENDED;
	chan_irq_set(chan, irq_mask);

	chan_enable(chan);
	return DMA_OK;
}

static void chan_start_first_queued(uint8_t chanid)
{
	dma_desc_t *desc;
	dma_chan_t *chan = chip->chan[chanid];

	desc = chan->first;
	con_log("dw_dma: chan %d : started!\n", chan->id);
	chan_block_xfer_start(chan, desc);
}

static void set_desc_last(dma_desc_t *desc)
{
	uint32_t val;

	val = le32_to_cpu(desc->lli.ctl_hi);
	val |= CH_CTL_H_LLI_LAST;
	desc->lli.ctl_hi = cpu_to_le32(val);
}

static void write_desc_sar(dma_desc_t *desc, dma_addr_t adr)
{
	desc->lli.sar = cpu_to_le64(adr);
}

static void write_desc_dar(dma_desc_t *desc, dma_addr_t adr)
{
	desc->lli.dar = cpu_to_le64(adr);
}

static void set_desc_src_master(dma_desc_t *desc)
{
	uint32_t val;

	val = le32_to_cpu(desc->lli.ctl_lo);
	val &= ~CH_CTL_L_SRC_MAST;
	desc->lli.ctl_lo = cpu_to_le32(val);
}

static void set_desc_dest_master(dma_desc_t *desc)
{
	uint32_t val;

	val = le32_to_cpu(desc->lli.ctl_lo);
	if (desc->chan->chip->hdata->nr_masters > 1)
		val |= CH_CTL_L_DST_MAST;
	else
		val &= ~CH_CTL_L_DST_MAST;

	desc->lli.ctl_lo = cpu_to_le32(val);
}

static int32_t
dma_chan_prep_dma_memcpy(uint8_t chanid, dma_addr_t dst_adr,
			 dma_addr_t src_adr, size_t len)
{
	dma_desc_t *first = NULL, *desc = NULL, *prev = NULL;
	size_t block_ts, max_block_ts, xfer_len;
	uint32_t xfer_width, reg;
	dma_chan_t *chan = chip->chan[chanid];
	uint8_t lms = 0;
	
	con_log("dw_dma: chan %d: memcpy: src: %pad dst: %pad length: %zd \n",
		chan->id,&src_adr, &dst_adr, len);

	max_block_ts = chan->chip->hdata->block_size[chan->id];

	while (len) {
		xfer_len = len;
		xfer_width = chan_get_xfer_width(chan, src_adr, dst_adr, xfer_len);

		block_ts = xfer_len >> xfer_width;
		if (block_ts > max_block_ts) {
			block_ts = max_block_ts;
			xfer_len = max_block_ts << xfer_width;
		}

		desc = axi_desc_get(chan);
		if (unlikely(!desc))
			goto err_desc_get;

		write_desc_sar(desc, src_adr);
		write_desc_dar(desc, dst_adr);
		desc->lli.block_ts_lo = cpu_to_le32(block_ts - 1);

		reg = CH_CTL_H_LLI_VALID;
		if (chan->chip->hdata->restrict_burst_len) {
			uint32_t burst_len = chan->chip->hdata->rw_burst_len;

			reg |= (CH_CTL_H_ARLEN_EN |
				burst_len << CH_CTL_H_ARLEN_POS |
				CH_CTL_H_AWLEN_EN |
				burst_len << CH_CTL_H_AWLEN_POS);
		}
		desc->lli.ctl_hi = cpu_to_le32(reg);

		reg = (DW_DMAC_BURST_TRANS_LEN_4 << CH_CTL_L_DST_MSIZE_POS |
		       DW_DMAC_BURST_TRANS_LEN_4 << CH_CTL_L_SRC_MSIZE_POS |
		       xfer_width << CH_CTL_L_DST_WIDTH_POS |
		       xfer_width << CH_CTL_L_SRC_WIDTH_POS |
		       DW_DMAC_CH_CTL_L_INC << CH_CTL_L_DST_INC_POS |
		       DW_DMAC_CH_CTL_L_INC << CH_CTL_L_SRC_INC_POS);
		desc->lli.ctl_lo = cpu_to_le32(reg);

		set_desc_src_master(desc);
		set_desc_dest_master(desc);

		if (!first) {
			first = desc;
		} else {
			list_add_tail(&desc->desc_list, &first->desc_list);
			write_desc_llp(prev, desc->phys | lms);
		}
		prev = desc;

		len -= xfer_len;
		dst_adr += xfer_len;
		src_adr += xfer_len;
	}

	set_desc_last(desc);

	return DMA_OK;
err_desc_get:
	if (first)
		axi_desc_put(first);
	return DMA_ERROR;
}

static int32_t
dma_chan_prep_dma_memcpy_sinlge_block(uint8_t chanid, dma_addr_t dst_adr,
			 dma_addr_t src_adr, size_t len)
{
	dma_desc_t *desc = NULL;
	size_t block_ts, max_block_ts;
	uint32_t xfer_width, reg;
	dma_chan_t *chan = chip->chan[chanid];
	uint8_t lms = 0;
	
	con_log("dw_dma: chan %d: memcpy: src: %pad dst: %pad length: %zd \n",
		chan->id,&src_adr, &dst_adr, len);

	max_block_ts = chan->chip->hdata->block_size[chan->id];

	xfer_width = chan_get_xfer_width(chan, src_adr, dst_adr, len);
	block_ts = len >> xfer_width;
	if (block_ts > max_block_ts) {
		block_ts = max_block_ts;
		len = max_block_ts << xfer_width;
	}

	desc = axi_desc_get(chan);
	if (unlikely(!desc))
		goto err_desc_get;

	write_desc_sar(desc, src_adr);
	write_desc_dar(desc, dst_adr);
	desc->lli.block_ts_lo = cpu_to_le32(block_ts - 1);

	reg = CH_CTL_H_LLI_VALID;
	if (chan->chip->hdata->restrict_burst_len) {
		uint32_t burst_len = chan->chip->hdata->rw_burst_len;

		reg |= (CH_CTL_H_ARLEN_EN |
			burst_len << CH_CTL_H_ARLEN_POS |
			CH_CTL_H_AWLEN_EN |
			burst_len << CH_CTL_H_AWLEN_POS);
	}
	desc->lli.ctl_hi = cpu_to_le32(reg);

	reg = (DW_DMAC_BURST_TRANS_LEN_4 << CH_CTL_L_DST_MSIZE_POS |
	       DW_DMAC_BURST_TRANS_LEN_4 << CH_CTL_L_SRC_MSIZE_POS |
	       xfer_width << CH_CTL_L_DST_WIDTH_POS |
	       xfer_width << CH_CTL_L_SRC_WIDTH_POS |
	       DW_DMAC_CH_CTL_L_INC << CH_CTL_L_DST_INC_POS |
	       DW_DMAC_CH_CTL_L_INC << CH_CTL_L_SRC_INC_POS);
	desc->lli.ctl_lo = cpu_to_le32(reg);

	set_desc_src_master(desc);
	set_desc_dest_master(desc);

	write_desc_llp(desc, desc->phys | lms);

	return DMA_OK;
    
err_desc_get:
	axi_desc_put(desc);
	return DMA_ERROR;
}

static void chan_block_xfer_complete(dma_chan_t *chan)
{
	unsigned long flags;

	spin_lock_irqsave(&chan->lock, flags);
	if (unlikely(chan_is_hw_enable(chan))) {
		con_err("dw_dma: chan %d: caught DW_DMAC_IRQ_DMA_TRF, but channel not idle!\n",
			chan->id);
		chan_disable(chan);
	}

	chan_start_first_queued(chan->id);
	spin_unlock_irqrestore(&chan->lock, flags);
}

static void chan_handle_err(dma_chan_t *chan, uint32_t status)
{
	unsigned long flags;

	spin_lock_irqsave(&chan->lock, flags);
	chan_disable(chan);
	chan_start_first_queued(chan->id);

	spin_unlock_irqrestore(&chan->lock, flags);
}

void dw_dma_interrupt(int irq, void *dev_id)
{
	dma_chip_t *chip = dev_id;
	dma_chan_t *chan;
	uint32_t status, i;

	dma_irq_disable(chip);

	/* Poll, clear and process every chanel interrupt status */
	for (i = 0; i < chip->hdata->nr_channels; i++) {
		chan = chip->chan[i];
		status = chan_irq_read(chan);
		chan_irq_clear(chan, status);

		con_log("dw_dma: chan %d: %u IRQ status: 0x%08x\n",
			chan->id, i, status);

		if (status & DW_DMAC_IRQ_ALL_ERR)
			chan_handle_err(chan, status);
		else if (status & DW_DMAC_IRQ_DMA_TRF)
			chan_block_xfer_complete(chan);
	}

	dma_irq_enable(chip);
}

static inline void chan_resume(dma_chan_t *chan)
{
	uint32_t val;

	val = dma_reg_get32(chan->chip, DMAC_CHEN);
	val &= ~(_BV(chan->id) << DMAC_CHAN_SUSP_SHIFT);
	val |=  (_BV(chan->id) << DMAC_CHAN_SUSP_WE_SHIFT);
	dma_reg_set32(chan->chip, DMAC_CHEN, val);
}

static int32_t dma_resume(dma_chip_t *chip)
{
	dma_enable(chip);
	dma_irq_enable(chip);
	return 0;
}

static void dw_dma_init_bank(void)
{
	int i;
	uint32_t tmp;

	con_dbg("dw_dma: dw_dma_init_bank\n");

	chip->reg_base = DW_DMA_REG_BASE;

	for (i = 0; i < DW_DMA_MAX_POOLS; i++)
		u64LocADDR[i] = (uint64_t)u64POOL_ADDR[i];

	for (tmp = 0; tmp < DW_DMA_MAX_CHANNELS; tmp++)
		chip->chan[tmp] = &chan_var[tmp];

	for (i = 0; i < DW_DMA_MAX_CHANNELS; i++) {
		dma_chan_t *chan = chip->chan[i];
		chan->chip = chip;
		chan->id = i;
		chan->chan_reg_base = chip->reg_base + COMMON_REG_LEN +
			i * CHAN_REG_LEN;
	}
}

void dw_dma_init(void)
{
	uint64_t dst = 0xff05000000;
	uint64_t src = 0xff05040000;
	uint32_t len = 136;
	
	con_dbg("dw_dma: dw_dma_init\n");

	dw_dma_init_bank();

#if 0
	ret = dw_dma_single_block(dst, src, len);

	chip->irq = DW_DMA_INT_NUM;
	chip->reg_base = DW_DMA_REG_BASE;

	chip->core_clk = DW_DMA_CORE_CLK;
	chip->cfgr_clk =  DW_DMA_CORE_CLK;

        chip->hdata->nr_channels = DMAC_MAX_CHANNELS;
        chip->hdata->nr_masters = DMAC_MAX_MASTERS;
        chip->hdata->m_data_width = DW_DMA_M_DATA_WIDTH(0);

        for (tmp = 0; tmp < chip->hdata->nr_channels; tmp++) {
		chip->hdata->block_size[tmp] = DMAC_MAX_BLK_SIZE;
		chip->hdata->priority[tmp] = tmp;
	}
	chip->hdata->restrict_burst_len = true;
	chip->hdata->rw_burst_len = DW_DMAC_ARWLEN_64;
        
	for (tmp = 0; tmp < chip->hdata->nr_channels; tmp++) {
		chip->chan[tmp] = &chan_var[tmp];
	}
        
	for (i = 0; i <  chip->hdata->nr_channels; i++) {
		dma_chan_t *chan = chip->chan[i];
		chan->chip = chip;
		chan->id = i;
		chan->chan_reg_base = chip->reg_base +
			COMMON_REG_LEN + i * CHAN_REG_LEN;
	}
	dma_pool_init();    
	dma_resume(chip);
	dma_hw_init(chip);
#endif
}

int32_t dma_multi_block_memcpy(void *dst, void *src, size_t len)
{
	uint8_t chanid = 0;
	dma_chan_t *chan = chip->chan[chanid];
	int32_t ret;
	__unused uint32_t status;

	/* prepare data */
	ret = dma_chan_prep_dma_memcpy(chanid,
		(dma_addr_t)dst, (dma_addr_t)src, len);
	if (ret == DMA_ERROR) {
		con_err("dw_dma: chan %d: dma_chan_prep_dma_memcpy error \n",
			chanid);
		return DMA_ERROR;
	}

	/* start tx */
	chan_start_first_queued(chanid);

	/* check result */
	status = chan_irq_read(chan);
	while (status == 0)
		status = chan_irq_read(chan);
	chan_irq_clear(chan, status);
	if (status & DW_DMAC_IRQ_DMA_TRF) {
		chan_block_xfer_complete(chan);
		return DMA_OK;
	}
	return DMA_ERROR;
}

int32_t dma_single_block_memcpy(void *dst, void *src, size_t len)
{
	uint8_t chanid = 0;
	dma_chan_t *chan = chip->chan[chanid];
	int32_t ret;
	__unused uint32_t status;

	/* prepare data  */
	ret = dma_chan_prep_dma_memcpy_sinlge_block(chanid,
		(dma_addr_t)dst, (dma_addr_t)src, len);
	if (ret == DMA_ERROR) {
		con_err("dw_dma: chan %d: dma_chan_prep_dma_memcpy error \n",
			chanid);
		return DMA_ERROR;
	}

	/* start tx */
	chan_block_xfer_start_single(chan,chan->first);

	/* check result */
	status = chan_irq_read(chan);
	while (status == 0)
		status = chan_irq_read(chan);
	chan_irq_clear(chan, status);
	if (status & DW_DMAC_IRQ_DMA_TRF) {
		chan_block_xfer_complete(chan);
		return DMA_OK;
	}
	return DMA_ERROR;
}
