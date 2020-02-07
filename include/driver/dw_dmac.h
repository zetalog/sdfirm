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
 * @(#)dw_dma.h: Synopsys DesignWare DMA interface
 * $Id: dw_dma.h,v 1.0 2020-2-6 10:58:00 syl Exp $
 */
#ifndef _DW_DMA_H
#define _DW_DMA_H
#include <target/spinlock.h>
#include <target/types.h>

#define DMAC_MAX_CHANNELS	 8
#define DMAC_MAX_MASTERS	2
#define DMAC_MAX_BLK_SIZE	0x200000

enum irqreturn {
	IRQ_NONE		= (0 << 0),
	IRQ_HANDLED		= (1 << 0),
	IRQ_WAKE_THREAD		= (1 << 1),
};
typedef enum irqreturn irqreturn_t;
typedef caddr_t dma_addr_t;

typedef struct 
{
    int32_t alloc_index;
    int32_t allocated_num;
    int32_t free_num;
} desc_stuct_t;

typedef struct  {
	uint32_t	nr_channels;
	uint32_t	nr_masters;
	uint32_t	m_data_width;
	uint32_t	block_size[DMAC_MAX_CHANNELS];
	uint32_t	priority[DMAC_MAX_CHANNELS];
	uint32_t	rw_burst_len;
	bool	restrict_burst_len;
} dw_dma_hcfg_t;

typedef struct {
	caddr_t		sar;
	caddr_t		dar;
	uint32_t		block_ts_lo;
	uint32_t		block_ts_hi;
	caddr_t		llp;
	uint32_t		ctl_lo;
	uint32_t		ctl_hi;
	uint32_t		sstat;
	uint32_t		dstat;
	uint32_t		status_lo;
	uint32_t		ststus_hi;
	uint32_t		reserved_lo;
	uint32_t		reserved_hi;
} dma_lli_t;

typedef struct  {
	dma_lli_t		lli;
       dma_addr_t phys;
	struct dma_chan_str	*chan;
	int32_t	    desc_id;
       int32_t	    desc_status; //0:free,1:allocated
       struct list_head	desc_list;
} dma_desc_t;

typedef struct dma_chan_str{
	struct  dma_chip_str	*chip;
	caddr_t	chan_reg_base;
	uint8_t				id;
       spinlock_t lock;
       int32_t		descs_allocated;
       dma_desc_t *first;
} dma_chan_t;

typedef struct  dma_chip_str{
	int32_t			irq;
	caddr_t     reg_base;
	caddr_t  core_clk;
	caddr_t  cfgr_clk;
	dw_dma_hcfg_t	*hdata;
	dma_chan_t	*chan[DMAC_MAX_CHANNELS];
} dma_chip_t;

#define DMA_OK  0
#define DMA_ERROR  (-1)



#define BIT(nr)			(1UL << (nr))
#define le64_to_cpu(x) ((uint64_t)(x))
#define cpu_to_le64(x) ((uint64_t)(x))
#define le32_to_cpu(x)		(x)
#define cpu_to_le32(x)		(x)
#define upper_32_bits(n) ((uint32_t)(((n) >> 16) >> 16))
#define lower_32_bits(n) ((uint32_t)(n))

#define DMA_SUPPORTS_MEM_TO_MEM	BIT(0)
#define DMA_SUPPORTS_MEM_TO_DEV	BIT(1)
#define DMA_SUPPORTS_DEV_TO_MEM	BIT(2)
#define DMA_SUPPORTS_DEV_TO_DEV	BIT(3)

#define AXI_DMA_BUSWIDTHS		  \
	(DMA_SLAVE_BUSWIDTH_1_BYTE	| \
	DMA_SLAVE_BUSWIDTH_2_BYTES	| \
	DMA_SLAVE_BUSWIDTH_4_BYTES	| \
	DMA_SLAVE_BUSWIDTH_8_BYTES	| \
	DMA_SLAVE_BUSWIDTH_16_BYTES	| \
	DMA_SLAVE_BUSWIDTH_32_BYTES	| \
	DMA_SLAVE_BUSWIDTH_64_BYTES)
	
#define DW_DMA_POOL_NUM   100
#define DW_DMA_INT_NUM 3
#define DW_DMA_REG_BASE 0xf0000000
#define DW_DMA_CORE_CLK 0xf0000000
#define DW_DMA_CFGR_CLK 0xf0000100
#define DW_DMA_M_DATA_WIDTH 8
#define ALLOCATED 1


#define COMMON_REG_LEN		0x100
#define CHAN_REG_LEN		0x100

#define DMAC_ID			0x000 /* R DMAC ID */
#define DMAC_COMPVER		0x008 /* R DMAC Component Version */
#define DMAC_CFG		0x010 /* R/W DMAC Configuration */
#define DMAC_CHEN		0x018 /* R/W DMAC Channel Enable */
#define DMAC_CHEN_L		0x018 /* R/W DMAC Channel Enable 00-31 */
#define DMAC_CHEN_H		0x01C /* R/W DMAC Channel Enable 32-63 */
#define DMAC_INTSTATUS		0x030 /* R DMAC Interrupt Status */
#define DMAC_COMMON_INTCLEAR	0x038 /* W DMAC Interrupt Clear */
#define DMAC_COMMON_INTSTATUS_ENA 0x040 /* R DMAC Interrupt Status Enable */
#define DMAC_COMMON_INTSIGNAL_ENA 0x048 /* R/W DMAC Interrupt Signal Enable */
#define DMAC_COMMON_INTSTATUS	0x050 /* R DMAC Interrupt Status */
#define DMAC_RESET		0x058 /* R DMAC Reset Register1 */

#define CH_SAR			0x000 /* R/W Chan Source Address */
#define CH_DAR			0x008 /* R/W Chan Destination Address */
#define CH_BLOCK_TS		0x010 /* R/W Chan Block Transfer Size */
#define CH_CTL			0x018 /* R/W Chan Control */
#define CH_CTL_L		0x018 /* R/W Chan Control 00-31 */
#define CH_CTL_H		0x01C /* R/W Chan Control 32-63 */
#define CH_CFG			0x020 /* R/W Chan Configuration */
#define CH_CFG_L		0x020 /* R/W Chan Configuration 00-31 */
#define CH_CFG_H		0x024 /* R/W Chan Configuration 32-63 */
#define CH_LLP			0x028 /* R/W Chan Linked List Pointer */
#define CH_STATUS		0x030 /* R Chan Status */
#define CH_SWHSSRC		0x038 /* R/W Chan SW Handshake Source */
#define CH_SWHSDST		0x040 /* R/W Chan SW Handshake Destination */
#define CH_BLK_TFR_RESUMEREQ	0x048 /* W Chan Block Transfer Resume Req */
#define CH_AXI_ID		0x050 /* R/W Chan AXI ID */
#define CH_AXI_QOS		0x058 /* R/W Chan AXI QOS */
#define CH_SSTAT		0x060 /* R Chan Source Status */
#define CH_DSTAT		0x068 /* R Chan Destination Status */
#define CH_SSTATAR		0x070 /* R/W Chan Source Status Fetch Addr */
#define CH_DSTATAR		0x078 /* R/W Chan Destination Status Fetch Addr */
#define CH_INTSTATUS_ENA	0x080 /* R/W Chan Interrupt Status Enable */
#define CH_INTSTATUS		0x088 /* R/W Chan Interrupt Status */
#define CH_INTSIGNAL_ENA	0x090 /* R/W Chan Interrupt Signal Enable */
#define CH_INTCLEAR		0x098 /* W Chan Interrupt Clear */

#define DMAC_EN_POS			0
#define DMAC_EN_MASK			BIT(DMAC_EN_POS)

#define INT_EN_POS			1
#define INT_EN_MASK			BIT(INT_EN_POS)

#define DMAC_CHAN_EN_SHIFT		0
#define DMAC_CHAN_EN_WE_SHIFT		8

#define DMAC_CHAN_SUSP_SHIFT		16
#define DMAC_CHAN_SUSP_WE_SHIFT		24

#define CH_CTL_H_ARLEN_EN		BIT(6)
#define CH_CTL_H_ARLEN_POS		7
#define CH_CTL_H_AWLEN_EN		BIT(15)
#define CH_CTL_H_AWLEN_POS		16

enum {
	DW_DMAC_ARWLEN_1		= 0,
	DW_DMAC_ARWLEN_2		= 1,
	DW_DMAC_ARWLEN_4		= 3,
	DW_DMAC_ARWLEN_8		= 7,
	DW_DMAC_ARWLEN_16		= 15,
	DW_DMAC_ARWLEN_32		= 31,
	DW_DMAC_ARWLEN_64		= 63,
	DW_DMAC_ARWLEN_128		= 127,
	DW_DMAC_ARWLEN_256		= 255,
	DW_DMAC_ARWLEN_MIN		= DW_DMAC_ARWLEN_1,
	DW_DMAC_ARWLEN_MAX		= DW_DMAC_ARWLEN_256
};

#define CH_CTL_H_LLI_LAST		BIT(30)
#define CH_CTL_H_LLI_VALID		BIT(31)

/* CH_CTL_L */
#define CH_CTL_L_LAST_WRITE_EN		BIT(30)

#define CH_CTL_L_DST_MSIZE_POS		18
#define CH_CTL_L_SRC_MSIZE_POS		14

enum {
	DW_DMAC_BURST_TRANS_LEN_1	= 0,
	DW_DMAC_BURST_TRANS_LEN_4,
	DW_DMAC_BURST_TRANS_LEN_8,
	DW_DMAC_BURST_TRANS_LEN_16,
	DW_DMAC_BURST_TRANS_LEN_32,
	DW_DMAC_BURST_TRANS_LEN_64,
	DW_DMAC_BURST_TRANS_LEN_128,
	DW_DMAC_BURST_TRANS_LEN_256,
	DW_DMAC_BURST_TRANS_LEN_512,
	DW_DMAC_BURST_TRANS_LEN_1024
};

#define CH_CTL_L_DST_WIDTH_POS		11
#define CH_CTL_L_SRC_WIDTH_POS		8

#define CH_CTL_L_DST_INC_POS		6
#define CH_CTL_L_SRC_INC_POS		4
enum {
	DW_DMAC_CH_CTL_L_INC		= 0,
	DW_DMAC_CH_CTL_L_NOINC
};

#define CH_CTL_L_DST_MAST		BIT(2)
#define CH_CTL_L_SRC_MAST		BIT(0)

#define CH_CFG_H_PRIORITY_POS		17
#define CH_CFG_H_HS_SEL_DST_POS		4
#define CH_CFG_H_HS_SEL_SRC_POS		3
enum {
	DW_DMAC_HS_SEL_HW		= 0,
	DW_DMAC_HS_SEL_SW
};

#define CH_CFG_H_TT_FC_POS		0
enum {
	DW_DMAC_TT_FC_MEM_TO_MEM_DMAC	= 0,
	DW_DMAC_TT_FC_MEM_TO_PER_DMAC,
	DW_DMAC_TT_FC_PER_TO_MEM_DMAC,
	DW_DMAC_TT_FC_PER_TO_PER_DMAC,
	DW_DMAC_TT_FC_PER_TO_MEM_SRC,
	DW_DMAC_TT_FC_PER_TO_PER_SRC,
	DW_DMAC_TT_FC_MEM_TO_PER_DST,
	DW_DMAC_TT_FC_PER_TO_PER_DST
};

#define CH_CFG_L_DST_MULTBLK_TYPE_POS	2
#define CH_CFG_L_SRC_MULTBLK_TYPE_POS	0
enum {
	DW_DMAC_MBLK_TYPE_CONTIGUOUS	= 0,
	DW_DMAC_MBLK_TYPE_RELOAD,
	DW_DMAC_MBLK_TYPE_SHADOW_REG,
	DW_DMAC_MBLK_TYPE_LL
};

enum {
	DW_DMAC_IRQ_NONE		= 0,
	DW_DMAC_IRQ_BLOCK_TRF		= BIT(0),
	DW_DMAC_IRQ_DMA_TRF		= BIT(1),
	DW_DMAC_IRQ_SRC_TRAN		= BIT(3),
	DW_DMAC_IRQ_DST_TRAN		= BIT(4),
	DW_DMAC_IRQ_SRC_DEC_ERR	= BIT(5),
	DW_DMAC_IRQ_DST_DEC_ERR	= BIT(6),
	DW_DMAC_IRQ_SRC_SLV_ERR	= BIT(7),
	DW_DMAC_IRQ_DST_SLV_ERR	= BIT(8),
	DW_DMAC_IRQ_LLI_RD_DEC_ERR	= BIT(9),
	DW_DMAC_IRQ_LLI_WR_DEC_ERR	= BIT(10),
	DW_DMAC_IRQ_LLI_RD_SLV_ERR	= BIT(11),
	DW_DMAC_IRQ_LLI_WR_SLV_ERR	= BIT(12),
	DW_DMAC_IRQ_INVALID_ERR	= BIT(13),
	DW_DMAC_IRQ_MULTIBLKTYPE_ERR	= BIT(14),
	DW_DMAC_IRQ_DEC_ERR		= BIT(16),
	DW_DMAC_IRQ_WR2RO_ERR		= BIT(17),
	DW_DMAC_IRQ_RD2RWO_ERR	= BIT(18),
	DW_DMAC_IRQ_WRONCHEN_ERR	= BIT(19),
	DW_DMAC_IRQ_SHADOWREG_ERR	= BIT(20),
	DW_DMAC_IRQ_WRONHOLD_ERR	= BIT(21),
	DW_DMAC_IRQ_LOCK_CLEARED	= BIT(27),
	DW_DMAC_IRQ_SRC_SUSPENDED	= BIT(28),
	DW_DMAC_IRQ_SUSPENDED		= BIT(29),
	DW_DMAC_IRQ_DISABLED		= BIT(30),
	DW_DMAC_IRQ_ABORTED		= BIT(31),
	DW_DMAC_IRQ_ALL_ERR		= (GENMASK(21, 16) | GENMASK(14, 5)),
	DW_DMAC_IRQ_ALL		= GENMASK(31, 0)
};

enum {
	DW_DMAC_TRANS_WIDTH_8		= 0,
	DW_DMAC_TRANS_WIDTH_16,
	DW_DMAC_TRANS_WIDTH_32,
	DW_DMAC_TRANS_WIDTH_64,
	DW_DMAC_TRANS_WIDTH_128,
	DW_DMAC_TRANS_WIDTH_256,
	DW_DMAC_TRANS_WIDTH_512,
	DW_DMAC_TRANS_WIDTH_MAX	= DW_DMAC_TRANS_WIDTH_512
};

enum dma_slave_buswidth {
	DMA_SLAVE_BUSWIDTH_UNDEFINED = 0,
	DMA_SLAVE_BUSWIDTH_1_BYTE = 1,
	DMA_SLAVE_BUSWIDTH_2_BYTES = 2,
	DMA_SLAVE_BUSWIDTH_3_BYTES = 3,
	DMA_SLAVE_BUSWIDTH_4_BYTES = 4,
	DMA_SLAVE_BUSWIDTH_8_BYTES = 8,
	DMA_SLAVE_BUSWIDTH_16_BYTES = 16,
	DMA_SLAVE_BUSWIDTH_32_BYTES = 32,
	DMA_SLAVE_BUSWIDTH_64_BYTES = 64,
};
#endif /* _DW_DMA_H */
