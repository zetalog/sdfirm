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

#ifndef __DW_DMA_H_INCLUDE__
#define __DW_DMA_H_INCLUDE__

#include <target/generic.h>
#include <target/spinlock.h>

#ifdef CONFIG_DW_DMA_CHANNEL8
#define DW_DMA_MAX_CHANNELS		8 /* dma-channels */
#endif
#ifdef CONFIG_DW_DMA_CHANNEL16
#define DW_DMA_MAX_CHANNELS		16 /* dma-channels */
#endif
#ifdef CONFIG_DW_DMA_MSTIF2
#define DW_DMA_MAX_MASTERS(n)		2 /* snps,dma-masters */
#endif
#ifdef CONFIG_DW_DMA_MSTIF1
#define DW_DMA_MAX_MASTERS(n)		1 /* snps,dma-masters */
#endif
#ifndef DW_DMA_M_ADDR_WIDTH
#define DW_DMA_M_ADDR_WIDTH(n)		32
#endif
#ifndef DW_DMA_M_DATA_WIDTH
#define DW_DMA_M_DATA_WIDTH(n)		32 /* snps,data-width */
#endif
#ifndef DW_DMA_M_BURSTLEN_WIDTH
#define DW_DMA_M_BURSTLEN_WIDTH(n)	4 /* snps,axi-max-burst-len */
#endif
#ifndef DW_DMA_S_DATA_WIDTH
#define DW_DMA_S_DATA_WIDTH(n)		32
#endif

#ifdef CONFIG_DW_DMC_MAX_BLK_SIZE
#define DMAC_MAX_BLK_SIZE	CONFIG_DW_DMC_MAX_BLK_SIZE
#else
#define DMAC_MAX_BLK_SIZE	0x200000
#endif

#ifndef DW_DMA_BASE
#define DW_DMA_BASE(n)		(DW_DMA##n##_BASE)
#endif
#ifndef DW_DMA_REG
#define DW_DMA_REG(n, offset)	(DW_DMA_BASE(n) + (offset))
#endif
#ifndef NR_DW_DMAS
#define NR_DW_DMAS		1
#endif
/* x is 0...DMAC_MAX_ChANNELS */
#define DW_DMA_CHREG(n, x, offset)		\
	DW_DMA_REG((n), 0x100 + ((x) << 8) + (offset))

#define DMAC_IDREG(n)				DW_DMA_REG(n, 0x00)
#define DMAC_COMPVERREG(n)			DW_DMA_REG(n, 0x08)
#define DMAC_CFGREG(n)				DW_DMA_REG(n, 0x10)
#define DMAC_CHENREG(n)				DW_DMA_REG(n, 0x18)
#define DMAC_CHENREG_LO(n)			DW_DMA_REG(n, 0x18)
#define DMAC_CHENREG_HI(n)			DW_DMA_REG(n, 0x1C)
#define DMAC_INTSTATUSREG(n)			DW_DMA_REG(n, 0x30)
#define DMAC_COMMONREG_INTCLEARREG(n)		DW_DMA_REG(n, 0x38)
#define DMAC_COMMONREG_INTSTATUS_ENABLEREG(n)	DW_DMA_REG(n, 0x40)
#define DMAC_COMMONREG_INTSIGNAL_ENABLEREG(n)	DW_DMA_REG(n, 0x48)
#define DMAC_COMMONREG_INTSTATUSREG(n)		DW_DMA_REG(n, 0x50)
#define DMAC_RESETREG				DW_DMA_REG(n, 0x58)

/* 5.1.2 DMAC_COMPVERREG */
#define DMA_DMAC_COMPVER_OFFSET		0
#define DMA_DMAC_COMPVER_MASK		REG_32BIT_MASK
#define DMA_DMAC_COMPVER(value)		_GET_FV(DMA_DMAC_COMPVER, value)

/* 5.1.3 DMAC_CFGREG */
#define DMA_DMAC_EN			_BV(0)
#define DMA_INT_EN			_BV(0)

/* 5.1.4 DMAC_CHENREG */
/* DMAC_CHENREG_LO */
#define DMA_CH_EN(n)			REG32_1BIT_OFFSET(n)
#define DMA_CH_EN_WE(n)			REG32_1BIT_OFFSET((n) + DW_DMA_MAX_CHANNELS)
#define DMA_CH_SUSP(n)			REG32_1BIT_OFFSET((n) + 2 * DW_DMA_MAX_CHANNELS)
#define DMA_CH_SUSP_WE(n)		REG32_1BIT_OFFSET((n) + 3 * DW_DMA_MAX_CHANNELS)
/* DMAC_CHENREG_HI */
#define DMA_CH_ABORT(n)			REG32_1BIT_OFFSET(n)
#define DMA_CH_ABORT_WE(n)		REG32_1BIT_OFFSET((n) + DW_DMA_MAX_CHANNELS)

/* 5.1.8 DMAC_INTSTATUSREG */
#define DMA_CommonReg_IntStat		_BV(16)
#define DMA_CH_IntStat(n)		REG32_1BIT_OFFSET(n)

/* 5.1.10 DMAC_COMMONREG_INTCLEARREG
 * 5.1.11 DMAC_COMMONREG_INTSTATUS_ENABLEREG
 * 5.1.12 DMAC_COMMONREG_INTSIGNAL_ENABLEREG
 * 5.1.13 DMAC_COMMONREG_INTSTATUSREG
 */
#define DMA_SLVIF_UndefinedReg_DEC_ERR_Int	_BV(8)
#define DMA_SLVIF_CommonReg_WrOnHold_ERR_Int	_BV(3)
#define DMA_SLVIF_CommonReg_RD2WO_ERR_Int	_BV(2)
#define DMA_SLVIF_CommonReg_WR2RO_ERR_Int	_BV(1)
#define DMA_SLVIF_CommonReg_DEC_ERR_Int		_BV(0)

#define DW_DMA_IRQ_ALL				\
	(DMA_SLVIF_UndefinedReg_DEC_ERR_Int |	\
	 DMA_SLVIF_CommonReg_WrOnHold_ERR_Int |	\
	 DMA_SLVIF_CommonReg_RD2WO_ERR_Int |	\
	 DMA_SLVIF_CommonReg_WR2RO_ERR_Int |	\
	 DMA_SLVIF_CommonReg_DEC_ERR_Int)
#define DW_DMA_IRQ_NONE				0

/* 5.1.14 DMAC_RESETREG */
#define DMA_DMAC_RST				_BV(0)

/* 5.1.15 DMAC_LOWPOWER_CFGREG */
/* DMAC_LOWPOWER_CFGREG_LO */
#define DMA_GBL_CSLP_EN				_BV(0)
#define DMA_CHNL_CSLP_EN			_BV(1)
#define DMA_SBIU_CSLP_EN			_BV(2)
#define DMA_MXIF_CSLP_EN			_BV(3)
/* DMAC_LOWPOWER_CFGREG_HI */
#define DMA_GLCH_LPDLY_OFFSET			0
#define DMA_GLCH_LPDLY_MASK			REG_8BIT_MASK
#define DMA_GLCH_LPDLY(value)			_SET_FV(DMA_GLCH_LPDLY, value)
#define DMA_SBIU_LPDLY_OFFSET			8
#define DMA_SBIU_LPDLY_MASK			REG_8BIT_MASK
#define DMA_SBIU_LPDLY(value)			_SET_FV(DMA_SBIU_LPDLY, value)
#define DMA_MXIF_LPDLY_OFFSET			16
#define DMA_MXIF_LPDLY_MASK			REG_8BIT_MASK
#define DMA_MXIF_LPDLY(value)			_SET_FV(DMA_MXIF_LPDLY, value)

#define dw_dma_disable_ctrl(n)	__raw_clearl(DMA_DMAC_EN, DMAC_CFGREG(n))
#define dw_dma_enable_ctrl(n)	__raw_setl(DMA_DMAC_EN, DMAC_CFGREG(n))
#define dw_dma_disable_irq(n)	__raw_clearl(DMA_INT_EN, DMAC_CFGREG(n))
#define dw_dma_enable_irq(n)	__raw_setl(DMA_INT_EN, DMAC_CFGREG(n))
#define dw_dma_reset_ctrl(n)						\
	do {								\
		__raw_setl(DMA_DMAC_RST, DMAC_RESETREG(n));		\
		while (__raw_readl(DMAC_RESETREG(n)) & DMA_DMAC_RST);	\
	} while (0)

#define dw_dma_disable_channel(n, c)				\
	do {							\
		uint32_t v = __raw_readl(DMAC_CHENREG_LO(n));	\
		v &= ~DMA_CH_EN(c);				\
		v |= DMA_CH_EN_WE(c);				\
		__raw_writel(v, DMAC_CHENREG_LO(n));		\
	} while (0)
#define dw_dma_enable_channel(n, c)				\
	do {							\
		uint32_t v = __raw_readl(DMAC_CHENREG_LO(n));	\
		v |= DMA_CH_EN(c);				\
		v |= DMA_CH_EN_WE(c);				\
		__raw_writel(v, DMAC_CHENREG_LO(n));		\
	} while (0)
#define dw_dma_channel_enabled(n, c)				\
	(!!(__raw_readl(DMAC_CHENREG_LO(n)) & DMA_CH_EN(c)))
#define dw_dma_resume_channel(n, c)				\
	do {							\
		uint32_t v = __raw_readl(DMAC_CHENREG_LO(n));	\
		v &= ~DMA_CH_SUSP(c);				\
		v |= DMA_CH_SUSP_WE(c);				\
		__raw_writel(v, DMAC_CHENREG_LO(n));		\
	} while (0)
#define dw_dma_suspend_channel(n, c)				\
	do {							\
		uint32_t v = __raw_readl(DMAC_CHENREG_LO(n));	\
		v |= DMA_CH_SUSP(c);				\
		v |= DMA_CH_SUSP_WE(c);				\
		__raw_writel(v, DMAC_CHENREG_LO(n));		\
	} while (0)
#define dw_dma_channel_suspended(n, c)				\
	(!!(__raw_readl(DMAC_CHENREG_LO(n)) & DMA_CH_SUSP(c)))
#define dw_dma_abort_channel(n, c)				\
	do {							\
		uint32_t v = __raw_readl(DMAC_CHENREG_HI(n));	\
		v |= DMA_CH_ABORT(c);				\
		v |= DMA_CH_ABORT_WE(c);			\
		__raw_writel(v, DMAC_CHENREG_HI(n));		\
	} while (0)
#define dw_dma_channel_aborted(n, c)				\
	(!!(__raw_readl(DMAC_CHENREG_HI(n)) & DMA_CH_ABORT(c)))

#define DMAC_CH_SAR(n, x)			DW_DMA_CHREG(n, x, 0x00)
#define DMAC_CH_DAR(n, x)			DW_DMA_CHREG(n, x, 0x08)
#define DMAC_CH_BLOCK_TS(n, x)			DW_DMA_CHREG(n, x, 0x10)
#define DMAC_CH_CTL(n, x)			DW_DMA_CHREG(n, x, 0x18)
#define DMAC_CH_CTL_LO(n, x)			DW_DMA_CHREG(n, x, 0x18)
#define DMAC_CH_CTL_HI(n, x)			DW_DMA_CHREG(n, x, 0x1C)
#define DMAC_CH_CFG(n, x)			DW_DMA_CHREG(n, x, 0x20)
#define DMAC_CH_CFG2(n, x)			DW_DMA_CHREG(n, x, 0x20)
#define DMAC_CH_LLP(n, x)			DW_DMA_CHREG(n, x, 0x28)
#define DMAC_CH_STATUSREG(n, x)			DW_DMA_CHREG(n, x, 0x30)
#define DMAC_CH_SWHSSRCREG(n, x)		DW_DMA_CHREG(n, x, 0x38)
#define DMAC_CH_SWHSDSTREG(n, x)		DW_DMA_CHREG(n, x, 0x40)
#define DMAC_CH_BLK_TFR_RESUMEREQREG(n, x)	DW_DMA_CHREG(n, x, 0x48)
#define DMAC_CH_AXI_IDREG(n, x)			DW_DMA_CHREG(n, x, 0x50)
#define DMAC_CH_AXI_QOSREG(n, x)		DW_DMA_CHREG(n, x, 0x58)
#define DMAC_CH_SSTAT(n, x)			DW_DMA_CHREG(n, x, 0x60)
#define DMAC_CH_DSTAT(n, x)			DW_DMA_CHREG(n, x, 0x68)
#define DMAC_CH_SSTATAR(n, x)			DW_DMA_CHREG(n, x, 0x70)
#define DMAC_CH_DSTATAR(n, x)			DW_DMA_CHREG(n, x, 0x78)
#define DMAC_CH_INTSTATUS_ENABLEREG(n, x)	DW_DMA_CHREG(n, x, 0x80)
#define DMAC_CH_INTSTATUS(n, x)			DW_DMA_CHREG(n, x, 0x88)
#define DMAC_CH_INTSIGNAL_ENABLEREG(n, x)	DW_DMA_CHREG(n, x, 0x90)
#define DMAC_CH_INTCLEARREG(n, x)		DW_DMA_CHREG(n, x, 0x98)

/* 5.2.4 CHx_CTL */
/* CHx_CTL_LO */
#define DMAC_NonPosted_LastWrite_En	_BV(30)
#define DMAC_AW_CACHE_OFFSET		26
#define DMAC_AW_CACHE_MASK		REG_4BIT_MASK
#define DMAC_AW_CACHE(value)		_SET_FV(DMAC_AW_CACHE, value)
#define DMAC_AR_CACHE_OFFSET		22
#define DMAC_AR_CACHE_MASK		REG_4BIT_MASK
#define DMAC_AR_CACHE(value)		_SET_FV(DMAC_AR_CACHE, value)
#define DMAC_DST_MSIZE_OFFSET		18
#define DMAC_DST_MSIZE_MASK		REG_4BIT_MASK
#define DMAC_DST_MSIZE(value)		_SET_FV(DMAC_DST_MSIZE, value)
#define DMAC_SRC_MSIZE_OFFSET		14
#define DMAC_SRC_MSIZE_MASK		REG_4BIT_MASK
#define DMAC_SRC_MSIZE(value)		_SET_FV(DMAC_SRC_MSIZE, value)
#define DMAC_DST_TR_WIDTH_OFFSET	11
#define DMAC_DST_TR_WIDTH_MASK		REG_3BIT_MASK
#define DMAC_DST_TR_WIDTH(value)	_SET_FV(DMAC_DST_TR_WIDTH, value)
#define DMAC_SRC_TR_WIDTH_OFFSET	8
#define DMAC_SRC_TR_WIDTH_MASK		REG_3BIT_MASK
#define DMAC_SRC_TR_WIDTH(value)	_SET_FV(DMAC_SRC_TR_WIDTH, value)
#define DMAC_DINC			_BV(6)
#define DMAC_SINC			_BV(4)
#define DMAC_DMS			_BV(2)
#define DMAC_SMS			_BV(0)
/* CHx_CTL_HI */
#define DMAC_SHADOWREG_OR_LLI_VALID	_BV(31)
#define DMAC_SHADOWREG_OR_LLI_LAST	_BV(30)
#define DMAC_IOC_BlkTfr			_BV(26)
#define DMAC_DST_STAT_EN		_BV(25)
#define DMAC_SRC_STAT_EN		_BV(24)
#define DMAC_AWLEN_OFFSET		16
#define DMAC_AWLEN_MASK			REG_8BIT_MASK
#define DMAC_AWLEN(value)		_SET_FV(DMAC_AWLEN, value)
#define DMAC_AWLEN_EN			_BV(15)
#define DMAC_ARLEN_OFFSET		7
#define DMAC_ARLEN_MASK			REG_8BIT_MASK
#define DMAC_ARLEN(value)		_SET_FV(DMAC_ARLEN, value)
#define DMAC_ARLEN_EN			_BV(6)
#define DMAC_AW_PROT_OFFSET		3
#define DMAC_AW_PROT_MASK		REG_3BIT_MASK
#define DMAC_AW_PROT(value)		_SET_FV(DMAC_AW_PROT, value)
#define DMAC_AR_PROT_OFFSET		0
#define DMAC_AR_PROT_MASK		REG_3BIT_MASK
#define DMAC_AR_PROT(value)		_SET_FV(DMAC_AR_PROT, value)

/* 5.2.5 CHx_CFG */
/* CHx_CFG_LO */
#ifdef CONFIG_DW_DMA_CH_MULTI_BLK
#define DMAC_DST_MULTBLK_TYPE_OFFSET	2
#define DMAC_DST_MULTBLK_TYPE_MASK	REG_2BIT_MASK
#define DMAC_DST_MULTBLK_TYPE(value)	_SET_FV(DMAC_DST_MULTBLK_TYPE, value)
#define DMAC_SRC_MULTBLK_TYPE_OFFSET	0
#define DMAC_SRC_MULTBLK_TYPE_MASK	REG_2BIT_MASK
#define DMAC_SRC_MULTBLK_TYPE(value)	_SET_FV(DMAC_SRC_MULTBLK_TYPE, value)
#define DMAC_MULTBLK_TYPE_CONTIGUOUS		0
#define DMAC_MULTBLK_TYPE_RELOAD		1
#define DMAC_MULTBLK_TYPE_SHADOW_REGISTER	2
#define DMAC_MULTBLK_TYPE_LINKED_LIST		3
#endif

/* 5.2.18 CHx_INTSTATUS_ENABLEREG
 * 5.2.19 CHx_INTSTATUS
 * 5.2.20 CHx_INTSIGNAL_ENABLEREG
 * 5.2.21 CHx_INTCLEARREG
 */
#define DMA_CH_ABORTED_Int			_BV(31)
#define DMA_CH_DISABLED_Int			_BV(30)
#define DMA_CH_SUSPENDED_Int			_BV(29)
#define DMA_CH_SRC_SUSPENDED_Int		_BV(28)
#define DMA_CH_LOCK_CLEARED_Int			_BV(27)
#define DMA_SLVIF_WRONGHOLD_ERR_Int		_BV(21)
#define DMA_SLVIF_SHADOWREG_WRON_VALID_ERR_Int	_BV(20)
#define DMA_SLVIF_WRONCHEN_ERR_Int		_BV(19)
#define DMA_SLVIF_RD2RWO_ERR_Int		_BV(18)
#define DMA_SLVIF_WR2RO_ERR_Int			_BV(17)
#define DMA_SLVIF_DEC_ERR_Int			_BV(16)
#define DMA_SLVIF_MULTIBLKTYPE_ERR_Int		_BV(14)
#define DMA_SHADOWREG_OR_LLI_INVALID_ERR_Int	_BV(13)
#define DMA_LLI_WR_SLV_ERR_Int			_BV(12)
#define DMA_LLI_RD_SLV_ERR_Int			_BV(11)
#define DMA_LLI_WR_DEC_ERR_Int			_BV(10)
#define DMA_LLI_RD_DEC_ERR_Int			_BV(9)
#define DMA_DST_SLV_ERR_Int			_BV(8)
#define DMA_SRC_SLV_ERR_Int			_BV(7)
#define DMA_DST_DEC_ERR_Int			_BV(6)
#define DMA_SRC_DEC_ERR_Int			_BV(5)
#define DMA_DST_TRANSCOMP_Int			_BV(4)
#define DMA_SRC_TRANSCOMP_Int			_BV(3)
#define DMA_DMA_TFR_DONE_Int			_BV(1)
#define DMA_BLOCK_TFR_DONE_Int			_BV(0)

#define DW_DMAC_IRQ_ALL					\
	(DMA_CH_ABORTED_Int |				\
	 DMA_CH_DISABLED_Int |				\
	 DMA_CH_SUSPENDED_Int |				\
	 DMA_CH_SRC_SUSPENDED_Int |			\
	 DMA_CH_LOCK_CLEARED_Int |			\
	 DMA_SLVIF_WRONGHOLD_ERR_Int |			\
	 DMA_SLVIF_SHADOWREG_WRON_VALID_ERR_Int |	\
	 DMA_SLVIF_WRONCHEN_ERR_Int |			\
	 DMA_SLVIF_RD2RWO_ERR_Int |			\
	 DMA_SLVIF_WR2RO_ERR_Int |			\
	 DMA_SLVIF_DEC_ERR_Int |			\
	 DMA_SLVIF_MULTIBLKTYPE_ERR_Int |		\
	 DMA_SHADOWREG_OR_LLI_INVALID_ERR_Int |		\
	 DMA_LLI_WR_SLV_ERR_Int |			\
	 DMA_LLI_RD_SLV_ERR_Int |			\
	 DMA_LLI_WR_DEC_ERR_Int |			\
	 DMA_LLI_RD_DEC_ERR_Int |			\
	 DMA_DST_SLV_ERR_Int |				\
	 DMA_SRC_SLV_ERR_Int |				\
	 DMA_DST_DEC_ERR_Int |				\
	 DMA_SRC_DEC_ERR_Int |				\
	 DMA_DST_TRANSCOMP_Int |			\
	 DMA_SRC_TRANSCOMP_Int |			\
	 DMA_DMA_TFR_DONE_Int |				\
	 DMA_BLOCK_TFR_DONE_Int)
#define DW_DMAC_IRQ_ALL_ERR		(GENMASK(21, 16) | GENMASK(14, 5))

#define dw_dmac_disable_irq(n, x, irqs)				\
	__raw_clearl(irqs, DMAC_CH_INTSTATUS_ENABLEREG(n, x))
#define dw_dmac_enable_irq(n, x, irqs)				\
	__raw_setl(irqs, DMAC_CH_INTSTATUS_ENABLEREG(n, x))
#define dw_dmac_trigger_irq(n, x, irqs)				\
	__raw_setl(irqs, DMAC_CH_INTSIGNAL_ENABLEREG(n, x))
#define dw_dmac_clear_irq(n, x, irqs)				\
	__raw_setl(irqs, DMAC_CH_INTCLEARREG(n, x))
#define dw_dmac_irq_status(n, x)				\
	__raw_readl(DMAC_CH_INTSTATUS(n, x))

typedef struct {
	int32_t alloc_index;
	int32_t allocated_num;
	int32_t free_num;
} desc_stuct_t;

typedef struct dw_dma_ctx {
	uint32_t nr_channels;
	uint32_t nr_masters;
	uint32_t m_data_width;
	uint32_t block_size[DW_DMA_MAX_CHANNELS];
	uint32_t priority[DW_DMA_MAX_CHANNELS];
	uint32_t rw_burst_len;
	bool restrict_burst_len;
} dw_dma_hcfg_t;

/* Figure 2-32 DW_axi_dmac Linked List Item (Descriptor) */
typedef struct {
	uint64_t sar;
	uint64_t dar;
	uint32_t block_ts_lo;
	uint32_t reserved1;
	uint64_t llp;
	uint32_t ctl_lo;
	uint32_t ctl_hi;
	uint32_t sstat;
	uint32_t dstat;
	uint32_t status_lo;
	uint32_t ststus_hi;
	uint64_t reserved2;
} dma_lli_t;

typedef struct  {
	dma_lli_t lli;
	dma_addr_t phys;
	struct dma_chan_str *chan;
	int32_t	desc_id;
	int32_t desc_status; /* 0:free, 1:allocated */
	struct list_head desc_list;
} dma_desc_t;

typedef struct dma_chan_str {
	struct dma_chip_str *chip;
	caddr_t	chan_reg_base;
	uint8_t id;
	spinlock_t lock;
	int32_t descs_allocated;
	dma_desc_t *first;
} dma_chan_t;

typedef struct dma_chip_str {
	int32_t irq;
	caddr_t reg_base;
	caddr_t core_clk;
	caddr_t cfgr_clk;
	dw_dma_hcfg_t *hdata;
	dma_chan_t *chan[DW_DMA_MAX_CHANNELS];
} dma_chip_t;

#define DMA_OK  0
#define DMA_ERROR  (-1)

#define le64_to_cpu(x)		((uint64_t)(x))
#define cpu_to_le64(x)		((uint64_t)(x))
#define le32_to_cpu(x)		(x)
#define cpu_to_le32(x)		(x)
#define upper_32_bits(n)	((uint32_t)(((n) >> 16) >> 16))
#define lower_32_bits(n)	((uint32_t)(n))

#define DMA_SUPPORTS_MEM_TO_MEM	_BV(0)
#define DMA_SUPPORTS_MEM_TO_DEV	_BV(1)
#define DMA_SUPPORTS_DEV_TO_MEM	_BV(2)
#define DMA_SUPPORTS_DEV_TO_DEV	_BV(3)

#define AXI_DMA_BUSWIDTHS		  \
	(DMA_SLAVE_BUSWIDTH_1_BYTE	| \
	DMA_SLAVE_BUSWIDTH_2_BYTES	| \
	DMA_SLAVE_BUSWIDTH_4_BYTES	| \
	DMA_SLAVE_BUSWIDTH_8_BYTES	| \
	DMA_SLAVE_BUSWIDTH_16_BYTES	| \
	DMA_SLAVE_BUSWIDTH_32_BYTES	| \
	DMA_SLAVE_BUSWIDTH_64_BYTES)
	
#ifdef CONFIG_DW_DMA_MAX_POOLS
#define DW_DMA_POOL_NUM		CONFIG_DW_DMA_MAX_POOLS
#else
#define DW_DMA_POOL_NUM		100
#endif
#define DW_DMA_INT_NUM		3
#define DW_DMA_REG_BASE		0xf0000000
#define DW_DMA_CORE_CLK		0xf0000000
#define DW_DMA_CFGR_CLK		0xf0000100
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
#define DMAC_EN_MASK			_BV(DMAC_EN_POS)

#define INT_EN_POS			1
#define INT_EN_MASK			_BV(INT_EN_POS)

#define DMAC_CHAN_EN_SHIFT		0
#define DMAC_CHAN_EN_WE_SHIFT		8

#define DMAC_CHAN_SUSP_SHIFT		16
#define DMAC_CHAN_SUSP_WE_SHIFT		24

#define CH_CTL_H_ARLEN_EN		_BV(6)
#define CH_CTL_H_ARLEN_POS		7
#define CH_CTL_H_AWLEN_EN		_BV(15)
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

#define CH_CTL_H_LLI_LAST		_BV(30)
#define CH_CTL_H_LLI_VALID		_BV(31)

/* CH_CTL_L */
#define CH_CTL_L_LAST_WRITE_EN		_BV(30)

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

#define CH_CTL_L_DST_MAST		_BV(2)
#define CH_CTL_L_SRC_MAST		_BV(0)

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
	DW_DMAC_IRQ_BLOCK_TRF		= _BV(0),
	DW_DMAC_IRQ_DMA_TRF		= _BV(1),
	DW_DMAC_IRQ_SRC_TRAN		= _BV(3),
	DW_DMAC_IRQ_DST_TRAN		= _BV(4),
	DW_DMAC_IRQ_SRC_DEC_ERR	= _BV(5),
	DW_DMAC_IRQ_DST_DEC_ERR	= _BV(6),
	DW_DMAC_IRQ_SRC_SLV_ERR	= _BV(7),
	DW_DMAC_IRQ_DST_SLV_ERR	= _BV(8),
	DW_DMAC_IRQ_LLI_RD_DEC_ERR	= _BV(9),
	DW_DMAC_IRQ_LLI_WR_DEC_ERR	= _BV(10),
	DW_DMAC_IRQ_LLI_RD_SLV_ERR	= _BV(11),
	DW_DMAC_IRQ_LLI_WR_SLV_ERR	= _BV(12),
	DW_DMAC_IRQ_INVALID_ERR	= _BV(13),
	DW_DMAC_IRQ_MULTIBLKTYPE_ERR	= _BV(14),
	DW_DMAC_IRQ_DEC_ERR		= _BV(16),
	DW_DMAC_IRQ_WR2RO_ERR		= _BV(17),
	DW_DMAC_IRQ_RD2RWO_ERR	= _BV(18),
	DW_DMAC_IRQ_WRONCHEN_ERR	= _BV(19),
	DW_DMAC_IRQ_SHADOWREG_ERR	= _BV(20),
	DW_DMAC_IRQ_WRONHOLD_ERR	= _BV(21),
	DW_DMAC_IRQ_LOCK_CLEARED	= _BV(27),
	DW_DMAC_IRQ_SRC_SUSPENDED	= _BV(28),
	DW_DMAC_IRQ_SUSPENDED		= _BV(29),
	DW_DMAC_IRQ_DISABLED		= _BV(30),
	DW_DMAC_IRQ_ABORTED		= _BV(31),
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

void dw_dma_init(void);
void dw_dma_interrupt(int irq, void *dev_id);

#endif /* __DW_DMA_H_INCLUDE__ */
