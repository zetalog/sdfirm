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
#include <target/cmdline.h>
#include <target/panic.h>
#include <target/heap.h>
#include <target/spinlock.h>
#include <driver/axi_dw_dma.h>

typedef struct axi_dma {
	phys_addr_t regs;
	uint8_t chan_num;
	uint16_t busrt_len;
	bool reg_map_8_channels;
	uint32_t inited;
	struct dw_axi_dma_hcfg	hdata;
	struct axi_dma_chan	*chan;
	bool	use_cfg2;
} axi_dma_t;

static axi_dma_t g_axi_dma;

#define AXI_DMA_BUSWIDTHS		  \
	(DMA_SLAVE_BUSWIDTH_1_BYTE	| \
	DMA_SLAVE_BUSWIDTH_2_BYTES	| \
	DMA_SLAVE_BUSWIDTH_4_BYTES	| \
	DMA_SLAVE_BUSWIDTH_8_BYTES	| \
	DMA_SLAVE_BUSWIDTH_16_BYTES	| \
	DMA_SLAVE_BUSWIDTH_32_BYTES	| \
	DMA_SLAVE_BUSWIDTH_64_BYTES)

#define AXI_DMA_FLAG_HAS_APB_REGS	BIT(0)
#define AXI_DMA_FLAG_HAS_RESETS		BIT(1)
#define AXI_DMA_FLAG_USE_CFG2		BIT(2)



static axi_dma_chan_t *chan_to_axi_dma_chan(axi_dma_t *axi_dma, struct dma_channel *chan);



static inline void
axi_dma_iowrite32(axi_dma_t *axi_dma, uint32_t reg, uint32_t val)
{
	__raw_writel(val, axi_dma->regs + reg);
}

static inline uint32_t axi_dma_ioread32(axi_dma_t *axi_dma, uint32_t reg)
{
	return __raw_readl(axi_dma->regs + reg);
}

static inline void
axi_dma_iowrite64(axi_dma_t *axi_dma, uint32_t reg, uint64_t val)
{
	__raw_writeq(val, axi_dma->regs + reg);
}

static inline uint64_t axi_dma_ioread64(axi_dma_t *axi_dma, uint32_t reg)
{
	return __raw_readq(axi_dma->regs + reg);
}

static inline void
axi_chan_iowrite32(struct axi_dma_chan *chan, uint32_t reg, uint32_t val)
{
	__raw_writel(val, chan->chan_regs + reg);
}

static inline uint32_t axi_chan_ioread32(struct axi_dma_chan *chan, uint32_t reg)
{
	return __raw_readl(chan->chan_regs + reg);
}

static inline void
axi_chan_iowrite64(struct axi_dma_chan *chan, uint32_t reg, uint64_t val)
{
	/*
	 * We split one 64 bit write for two 32 bit write as some HW doesn't
	 * support 64 bit access.
	 */
	__raw_writeq(val, chan->chan_regs + reg);
}


static inline void axi_dma_disable(axi_dma_t *chip)
{
	uint32_t val;

	val = axi_dma_ioread32(chip, DMAC_CFG);
	val &= ~DMAC_EN_MASK;
	axi_dma_iowrite32(chip, DMAC_CFG, val);
}

static inline void axi_dma_enable(axi_dma_t *chip)
{
	uint32_t val;

	val = axi_dma_ioread32(chip, DMAC_CFG);
	val |= DMAC_EN_MASK;
	axi_dma_iowrite32(chip, DMAC_CFG, val);
}

static inline void axi_dma_irq_disable(axi_dma_t *chip)
{
	uint32_t val;

	val = axi_dma_ioread32(chip, DMAC_CFG);
	val &= ~INT_EN_MASK;
	axi_dma_iowrite32(chip, DMAC_CFG, val);
}

static inline void axi_dma_irq_enable(axi_dma_t *chip)
{
	uint32_t val;

	val = axi_dma_ioread32(chip, DMAC_CFG);
	val |= INT_EN_MASK;
	axi_dma_iowrite32(chip, DMAC_CFG, val);
}

static inline void axi_chan_irq_disable(struct axi_dma_chan *chan, uint32_t irq_mask)
{
	uint32_t val;

	if (likely(irq_mask == DWAXIDMAC_IRQ_ALL)) {
		axi_chan_iowrite32(chan, CH_INTSTATUS_ENA, DWAXIDMAC_IRQ_NONE);
	} else {
		val = axi_chan_ioread32(chan, CH_INTSTATUS_ENA);
		val &= ~irq_mask;
		axi_chan_iowrite32(chan, CH_INTSTATUS_ENA, val);
	}
}

static inline void axi_chan_disable(struct axi_dma_chan *chan)
{
	uint64_t val;

	if (chan->axi_dma->chan_num >= DMAC_CHAN_16) {
		val = axi_dma_ioread64(chan->axi_dma, DMAC_CHEN);
		if (chan->id >= DMAC_CHAN_16) {
			val &= ~((uint64_t)(BIT(chan->id) >> DMAC_CHAN_16)
				<< (DMAC_CHAN_EN_SHIFT + DMAC_CHAN_BLOCK_SHIFT));
			val |=   (uint64_t)(BIT(chan->id) >> DMAC_CHAN_16)
				<< (DMAC_CHAN_EN2_WE_SHIFT + DMAC_CHAN_BLOCK_SHIFT);
		} else {
			val &= ~(BIT(chan->id) << DMAC_CHAN_EN_SHIFT);
			val |=   BIT(chan->id) << DMAC_CHAN_EN2_WE_SHIFT;
		}
		axi_dma_iowrite64(chan->axi_dma, DMAC_CHEN, val);
	} else {
		val = axi_dma_ioread32(chan->axi_dma, DMAC_CHEN);
		val &= ~(BIT(chan->id) << DMAC_CHAN_EN_SHIFT);
		if (chan->axi_dma->reg_map_8_channels)
			val |=   BIT(chan->id) << DMAC_CHAN_EN_WE_SHIFT;
		else
			val |=   BIT(chan->id) << DMAC_CHAN_EN2_WE_SHIFT;
		axi_dma_iowrite32(chan->axi_dma, DMAC_CHEN, (uint32_t)val);
	}
}

static void axi_dma_hw_init(axi_dma_t *chip)
{
	uint32_t i;

	for (i = 0; i < chip->chan_num; i++) {
		axi_chan_irq_disable(&chip->chan[i], DWAXIDMAC_IRQ_ALL);
		axi_chan_disable(&chip->chan[i]);
	}
}
static inline bool axi_chan_is_hw_enable(struct axi_dma_chan *chan)
{
	uint64_t val;

	if (chan->axi_dma->chan_num >= DMAC_CHAN_16)
		val = axi_dma_ioread64(chan->axi_dma, DMAC_CHEN);
	else
		val = axi_dma_ioread32(chan->axi_dma, DMAC_CHEN);

	if (chan->id >= DMAC_CHAN_16)
		return !!(val & ((uint64_t)(BIT(chan->id) >> DMAC_CHAN_16) << DMAC_CHAN_BLOCK_SHIFT));
	else
		return !!(val & (BIT(chan->id) << DMAC_CHAN_EN_SHIFT));
}

static inline void axi_chan_config_write(struct axi_dma_chan *chan,
					 struct axi_dma_chan_config *config)
{
	uint32_t cfg_lo, cfg_hi;

	cfg_lo = (config->dst_multblk_type << CH_CFG_L_DST_MULTBLK_TYPE_POS |
		  config->src_multblk_type << CH_CFG_L_SRC_MULTBLK_TYPE_POS);
	if (chan->axi_dma->reg_map_8_channels &&
	    !chan->axi_dma->use_cfg2) {
		cfg_hi = config->tt_fc << CH_CFG_H_TT_FC_POS |
			 config->hs_sel_src << CH_CFG_H_HS_SEL_SRC_POS |
			 config->hs_sel_dst << CH_CFG_H_HS_SEL_DST_POS |
			 config->src_per << CH_CFG_H_SRC_PER_POS |
			 config->dst_per << CH_CFG_H_DST_PER_POS |
			 config->prior << CH_CFG_H_PRIORITY_POS;
	} else {
		cfg_lo |= config->src_per << CH_CFG2_L_SRC_PER_POS |
			  config->dst_per << CH_CFG2_L_DST_PER_POS;
		cfg_hi = config->tt_fc << CH_CFG2_H_TT_FC_POS |
			 config->hs_sel_src << CH_CFG2_H_HS_SEL_SRC_POS |
			 config->hs_sel_dst << CH_CFG2_H_HS_SEL_DST_POS |
			 config->prior << CH_CFG2_H_PRIORITY_POS;
	}
	axi_chan_iowrite32(chan, CH_CFG_L, cfg_lo);
	axi_chan_iowrite32(chan, CH_CFG_H, cfg_hi);
}

static void write_chan_llp(struct axi_dma_chan *chan, dma_addr_t adr)
{
	axi_chan_iowrite64(chan, CH_LLP, adr);
}

static inline void axi_chan_irq_sig_set(struct axi_dma_chan *chan, uint32_t irq_mask)
{
	axi_chan_iowrite32(chan, CH_INTSIGNAL_ENA, irq_mask);
}

static inline void axi_chan_irq_set(struct axi_dma_chan *chan, uint32_t irq_mask)
{
	axi_chan_iowrite32(chan, CH_INTSTATUS_ENA, irq_mask);
}

static inline void axi_chan_enable(struct axi_dma_chan *chan)
{
	uint64_t val;

	if (chan->axi_dma->chan_num >= DMAC_CHAN_16) {
		val = axi_dma_ioread64(chan->axi_dma, DMAC_CHEN);
		if (chan->id >= DMAC_CHAN_16) {
			val |= (uint64_t)(BIT(chan->id) >> DMAC_CHAN_16)
				<< (DMAC_CHAN_EN_SHIFT + DMAC_CHAN_BLOCK_SHIFT) |
				(uint64_t)(BIT(chan->id) >> DMAC_CHAN_16)
				<< (DMAC_CHAN_EN2_WE_SHIFT + DMAC_CHAN_BLOCK_SHIFT);
		} else {
			val |= BIT(chan->id) << DMAC_CHAN_EN_SHIFT |
			BIT(chan->id) << DMAC_CHAN_EN2_WE_SHIFT;
		}
		axi_dma_iowrite64(chan->axi_dma, DMAC_CHEN, val);
	} else {
		val = axi_dma_ioread32(chan->axi_dma, DMAC_CHEN);
		if (chan->axi_dma->reg_map_8_channels) {
			val |= BIT(chan->id) << DMAC_CHAN_EN_SHIFT |
			BIT(chan->id) << DMAC_CHAN_EN_WE_SHIFT;
		} else {
			val |= BIT(chan->id) << DMAC_CHAN_EN_SHIFT |
				BIT(chan->id) << DMAC_CHAN_EN2_WE_SHIFT;
		}
		axi_dma_iowrite32(chan->axi_dma, DMAC_CHEN, (uint32_t)val);
	}
}
static void dw_axi_dma_set_byte_halfword(struct axi_dma_chan *chan, bool set)
{
	return;
	// uint32_t offset = DMAC_APB_BYTE_WR_CH_EN;
	// uint32_t reg_width, val;

	// if (!chan->axi_dma->apb_regs) {
	// 	dev_dbg(chan->axi_dma->dev, "apb_regs not initialized\n");
	// 	return;
	// }

	// reg_width = __ffs(chan->config.dst_addr_width);
	// if (reg_width == DWAXIDMAC_TRANS_WIDTH_16)
	// 	offset = DMAC_APB_HALFWORD_WR_CH_EN;

	// val = ioread32(chan->axi_dma->apb_regs + offset);

	// if (set)
	// 	val |= BIT(chan->id);
	// else
	// 	val &= ~BIT(chan->id);

	// iowrite32(val, chan->axi_dma->apb_regs + offset);
}

static void axi_chan_block_xfer_start(struct axi_dma_chan *chan,
				      struct axi_dma_desc *first)
{
	uint32_t priority = chan->priority[chan->id];
	struct axi_dma_chan_config config = {};
	uint32_t irq_mask;
	uint8_t lms = 0; /* Select AXI0 master for LLI fetching */

	if (unlikely(axi_chan_is_hw_enable(chan))) {
		con_err("channel%d is non-idle!\n", chan->id);
		return;
	}

	axi_dma_enable(chan->axi_dma);

	config.dst_multblk_type = DWAXIDMAC_MBLK_TYPE_LL;
	config.src_multblk_type = DWAXIDMAC_MBLK_TYPE_LL;
	config.tt_fc = DWAXIDMAC_TT_FC_MEM_TO_MEM_DMAC;
	config.prior = priority;
	config.hs_sel_dst = DWAXIDMAC_HS_SEL_HW;
	config.hs_sel_src = DWAXIDMAC_HS_SEL_HW;
	switch (chan->direction) {
	case DMA_MEM_TO_DEV:
		dw_axi_dma_set_byte_halfword(chan, true);
		// config.tt_fc = chan->config.device_fc ?
		// 		DWAXIDMAC_TT_FC_MEM_TO_PER_DST :
		// 		DWAXIDMAC_TT_FC_MEM_TO_PER_DMAC;
		// if (chan->axi_dma->apb_regs)
		// 	config.dst_per = chan->id;
		// else
			// config.dst_per = chan->hw_handshake_num;
		break;
	case DMA_DEV_TO_MEM:
		// config.tt_fc = chan->config.device_fc ?
		// 		DWAXIDMAC_TT_FC_PER_TO_MEM_SRC :
		// 		DWAXIDMAC_TT_FC_PER_TO_MEM_DMAC;
		// if (chan->axi_dma->apb_regs)
			// config.src_per = chan->id;
		// else
			// config.src_per = chan->hw_handshake_num;
		break;
	default:
		break;
	}
	axi_chan_config_write(chan, &config);

	write_chan_llp(chan, first->hw_desc[0].llp | lms);

	irq_mask = DWAXIDMAC_IRQ_DMA_TRF | DWAXIDMAC_IRQ_ALL_ERR;
	axi_chan_irq_sig_set(chan, irq_mask);

	/* Generate 'suspend' status but don't generate interrupt */
	irq_mask |= DWAXIDMAC_IRQ_SUSPENDED;
	axi_chan_irq_set(chan, irq_mask);

	axi_chan_enable(chan);
}

static void axi_chan_start_first_queued(struct axi_dma_chan *chan)
{
	axi_chan_block_xfer_start(chan, chan->desc);
}

static void dma_chan_issue_pending(struct dma_channel *dchan)
{
	struct axi_dma_chan *chan = chan_to_axi_dma_chan(&g_axi_dma, dchan);

	axi_chan_start_first_queued(chan);
}

static void set_desc_src_master(struct axi_dma_hw_desc *desc)
{
	uint32_t val;

	/* Select AXI0 for source master */
	val = le32_to_cpu(desc->lli->ctl_lo);
	val &= ~CH_CTL_L_SRC_MAST;
	desc->lli->ctl_lo = cpu_to_le32(val);
}

static void set_desc_dest_master(struct axi_dma_hw_desc *hw_desc,
				 struct axi_dma_desc *desc)
{
	uint32_t val;

	/* Select AXI1 for source master if available */
	val = le32_to_cpu(hw_desc->lli->ctl_lo);
	if (desc->chan->nr_masters > 1) {
		val |= CH_CTL_L_DST_MAST;
	}
	else {
		val &= ~CH_CTL_L_DST_MAST;
	}

	hw_desc->lli->ctl_lo = cpu_to_le32(val);
}

static axi_dma_chan_t *chan_to_axi_dma_chan(axi_dma_t *axi_dma, struct dma_channel *chan)
{
	for (int i = 0; i < axi_dma->chan_num; i++) {
		if (chan->id == i) {
			return &axi_dma->chan[i];
		}
	}
	return NULL;
}

static struct axi_dma_desc *axi_desc_alloc(uint32_t num)
{
	struct axi_dma_desc *desc;

	desc = (struct axi_dma_desc*)heap_calloc(sizeof(*desc));
	if (!desc)
		return NULL;

	desc->hw_desc = (struct axi_dma_hw_desc	*)heap_calloc(num * sizeof(*desc->hw_desc));
	if (!desc->hw_desc) {
		heap_free((caddr_t)desc);
		return NULL;
	}
	desc->nr_hw_descs = num;

	return desc;
}

static uint32_t axi_chan_get_xfer_width(struct axi_dma_chan *chan, dma_addr_t src,
				   dma_addr_t dst, size_t len)
{
// fixme
	uint32_t max_width = chan->m_data_width;

	// return __ffs_unit(src | dst | len | BIT(max_width));

	return max_width;
}

static struct axi_dma_lli *axi_desc_get(struct axi_dma_chan *chan, dma_addr_t *addr)
{
	struct axi_dma_lli *lli;

	lli = (struct axi_dma_lli *)page_alloc_pages(1);
	if (unlikely(!lli)) {
		con_err("channel%d: not enough descriptors available\n", chan->id);
		return NULL;
	}

	*addr = __pa(lli);

	return lli;
}

static void write_desc_sar(struct axi_dma_hw_desc *desc, dma_addr_t adr)
{
	desc->lli->sar = cpu_to_le64(adr);
}

static void write_desc_dar(struct axi_dma_hw_desc *desc, dma_addr_t adr)
{
	desc->lli->dar = cpu_to_le64(adr);
}

static void set_desc_last(struct axi_dma_hw_desc *desc)
{
	uint32_t val;

	val = le32_to_cpu(desc->lli->ctl_hi);
	val |= CH_CTL_H_LLI_LAST;
	desc->lli->ctl_hi = cpu_to_le32(val);
}

static void write_desc_llp(struct axi_dma_hw_desc *desc, dma_addr_t adr)
{
	desc->lli->llp = cpu_to_le64(adr);
}

static void axi_chan_dump_lli(struct axi_dma_hw_desc *desc)
{
	printf("SAR: 0x%llx DAR: 0x%llx LLP: 0x%llx BTS 0x%x CTL: 0x%x:%08x\n",
		le64_to_cpu(desc->lli->sar),
		le64_to_cpu(desc->lli->dar),
		le64_to_cpu(desc->lli->llp),
		le32_to_cpu(desc->lli->block_ts_lo),
		le32_to_cpu(desc->lli->ctl_hi),
		le32_to_cpu(desc->lli->ctl_lo));
}
int axi_dma_prep_memcpy(struct dma_channel *dchan, dma_addr_t dst_adr,
			 dma_addr_t src_adr, size_t len, unsigned long flags)
{
	struct axi_dma_chan *chan = chan_to_axi_dma_chan(&g_axi_dma, dchan);
	size_t block_ts, max_block_ts, xfer_len;
	struct axi_dma_hw_desc *hw_desc = NULL;
	struct axi_dma_desc *desc = NULL;
	uint32_t xfer_width, reg, num;
	uint64_t llp = 0;
	uint8_t lms = 0;

	max_block_ts = chan->block_size[chan->id];
	xfer_width = axi_chan_get_xfer_width(chan, src_adr, dst_adr, len);
	num = DIV_ROUND_UP(len, max_block_ts << xfer_width);
// printf("max_block_ts: %d  xfer_width: %d  num: %d\n", max_block_ts, xfer_width, num);
	desc = axi_desc_alloc(num);
	if (unlikely(!desc))
		goto err_desc_get;
	
	chan->desc = desc;
	num = 0;
	desc->length = 0;
	while (len) {
		xfer_len = len;

		hw_desc = &desc->hw_desc[num];
		/*
		 * Take care for the alignment.
		 * Actually source and destination widths can be different, but
		 * make them same to be simpler.
		 */
		xfer_width = axi_chan_get_xfer_width(chan, src_adr, dst_adr, xfer_len);

		desc->chan = chan;
		/*
		 * block_ts indicates the total number of data of width
		 * to be transferred in a DMA block transfer.
		 * BLOCK_TS register should be set to block_ts - 1
		 */
		block_ts = xfer_len >> xfer_width;
		if (block_ts > max_block_ts) {
			block_ts = max_block_ts;
			xfer_len = max_block_ts << xfer_width;
		}

		hw_desc->lli = axi_desc_get(chan, &hw_desc->llp);
		if (unlikely(!hw_desc->lli))
			goto err_desc_get;

		write_desc_sar(hw_desc, src_adr);
		write_desc_dar(hw_desc, dst_adr);
		hw_desc->lli->block_ts_lo = cpu_to_le32(block_ts - 1);

		reg = CH_CTL_H_LLI_VALID;
		if (chan->restrict_axi_burst_len) {
			uint32_t burst_len = chan->axi_rw_burst_len;

			reg |= (CH_CTL_H_ARLEN_EN |
				burst_len << CH_CTL_H_ARLEN_POS |
				CH_CTL_H_AWLEN_EN |
				burst_len << CH_CTL_H_AWLEN_POS);
		}
		hw_desc->lli->ctl_hi = cpu_to_le32(reg);
		reg = (DWAXIDMAC_BURST_TRANS_LEN_4 << CH_CTL_L_DST_MSIZE_POS |
		       DWAXIDMAC_BURST_TRANS_LEN_4 << CH_CTL_L_SRC_MSIZE_POS |
		       xfer_width << CH_CTL_L_DST_WIDTH_POS |
		       xfer_width << CH_CTL_L_SRC_WIDTH_POS |
		       DWAXIDMAC_CH_CTL_L_INC << CH_CTL_L_DST_INC_POS |
		       DWAXIDMAC_CH_CTL_L_INC << CH_CTL_L_SRC_INC_POS);
		hw_desc->lli->ctl_lo = cpu_to_le32(reg);

		set_desc_src_master(hw_desc);
		set_desc_dest_master(hw_desc, desc);

		hw_desc->len = xfer_len;
		desc->length += hw_desc->len;
		/* update the length and addresses for the next loop cycle */
		len -= xfer_len;
		dst_adr += xfer_len;
		src_adr += xfer_len;
		num++;
	}
	/* Set end-of-link to the last link descriptor of list */
	set_desc_last(&desc->hw_desc[num - 1]);
	chan->descs_allocated = num;
	/* Managed transfer list */
	do {
		hw_desc = &desc->hw_desc[--num];
		write_desc_llp(hw_desc, llp | lms);
		llp = hw_desc->llp;
	} while (num);
	

	return 0;
err_desc_get:
	// if (desc)
		// axi_desc_put(desc);
	return -1;
}

static inline void axi_chan_irq_clear(struct axi_dma_chan *chan, uint32_t irq_mask)
{
	axi_chan_iowrite32(chan, CH_INTCLEAR, irq_mask);
}

static inline uint32_t axi_chan_irq_read(struct axi_dma_chan *chan)
{
	return axi_chan_ioread32(chan, CH_INTSTATUS);
}

static void axi_chan_list_dump_lli(struct axi_dma_chan *chan,
				   struct axi_dma_desc *desc_head)
{
	int count = chan->descs_allocated;
	int i;

	for (i = 0; i < count; i++)
		axi_chan_dump_lli(&desc_head->hw_desc[i]);
}

static void axi_chan_handle_err(struct axi_dma_chan *chan, uint32_t status)
{
	axi_chan_disable(chan);

	/* WARN about bad descriptor */
	con_err("Bad descriptor submitted for chan%d, irq: 0x%08x\n",
		chan->id, status);
	axi_chan_list_dump_lli(chan, chan->desc);
}

static void axi_chan_block_xfer_complete(struct axi_dma_chan *chan)
{
	con_dbg("xfer complete submitted for chan%d\n", chan->id);		
}
static int axi_dma_chan_poll(struct dma_channel *dchan)
{
	struct axi_dma_chan *chan = chan_to_axi_dma_chan(&g_axi_dma, dchan);

	while (1) {
		
		uint32_t status = axi_chan_irq_read(chan);
		axi_chan_irq_clear(chan, status);

		if (status & DWAXIDMAC_IRQ_ALL_ERR) {
			axi_chan_handle_err(chan, status);
			return -1;
		}
		else if (status & DWAXIDMAC_IRQ_DMA_TRF) {
			axi_chan_block_xfer_complete(chan);
			return 0;
		}
	}
}
int dma_hw_memcpy_sync(void *chan, dma_addr_t dst, dma_addr_t src, size_t len, unsigned long flags)
{
	axi_dma_prep_memcpy((struct dma_channel *)chan, dst, src, len, flags);
	dma_chan_issue_pending((struct dma_channel *)chan);
	axi_dma_chan_poll((struct dma_channel *)chan);

	return 0;
}

static int axi_dma_init2(axi_dma_t *axi_dma, phys_addr_t base, uint8_t chan_num, uint16_t busrt_len)
{
	if (axi_dma->inited) return 0;

	axi_dma->regs = base;
	axi_dma->chan_num = chan_num;
	axi_dma->busrt_len = busrt_len;

	axi_dma->chan = (struct axi_dma_chan*)heap_calloc(sizeof(*axi_dma->chan) * chan_num);
	if (!axi_dma->chan) {
		return -1;
	}
	for (int i = 0; i < chan_num; i++) {
		axi_dma_chan_t *chan = &axi_dma->chan[i];
		chan->id = i;
		chan->chan_regs = axi_dma->regs + COMMON_REG_LEN + i * CHAN_REG_LEN;
		chan->axi_dma = axi_dma;
		chan->block_size[chan->id] = 65536;
		chan->m_data_width = 4; //snps,data-width
		chan->axi_rw_burst_len = 127; // snps,axi-max-burst-len
		chan->restrict_axi_burst_len = true;
		chan->nr_masters = 1; // snps,dma-masters
	}

	axi_dma->inited = 1;
	axi_dma->use_cfg2 = 0;
	if (chan_num <= DMA_REG_MAP_CH_REF)
		axi_dma->reg_map_8_channels = true;
	axi_dma_enable(axi_dma);
	axi_dma_irq_enable(axi_dma);
	axi_dma_hw_init(axi_dma);

	return 0;
}

int axi_dma_init(phys_addr_t base, uint8_t chan_num, uint16_t busrt_len)
{
	return axi_dma_init2(&g_axi_dma, base, chan_num, busrt_len);
}