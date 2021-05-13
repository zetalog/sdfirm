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
 * @(#)dw_dma.h: Synopsys DesignWare AXI DMAC interface
 * $Id: dw_dma.h,v 1.0 2020-2-6 10:58:00 syl Exp $
 */

#ifndef __DW_DMA_H_INCLUDE__
#define __DW_DMA_H_INCLUDE__

#include <target/generic.h>
#include <target/spinlock.h>
#include <target/amba.h>

#ifdef CONFIG_DW_DMA_MAX_DESCS
#define DW_DMA_MAX_DESCS		CONFIG_DW_DMA_MAX_DESCS
#endif
#ifndef DW_DMA_MAX_DESCS
#define DW_DMA_MAX_DESCS		DW_DMA_MAX_CHANS
#endif

/* Table 3-1 Top Level Parameters */
#ifndef DW_DMA_MAX_CHANNELS
#ifdef CONFIG_DW_DMA_CHANNEL8
#define DW_DMA_MAX_CHANNELS		8 /* dma-channels */
#endif
#ifdef CONFIG_DW_DMA_CHANNEL16
#define DW_DMA_MAX_CHANNELS		16 /* dma-channels */
#endif
#endif /* DW_DMA_MAX_CHANNELS */
#ifndef DW_DMA_MAX_HSIFS
#ifdef CONFIG_DW_DMA_HSIF0
#define DW_DMA_MAX_HSIFS		0
#endif
#ifdef CONFIG_DW_DMA_HSIF1
#define DW_DMA_MAX_HSIFS		1
#endif
#ifdef CONFIG_DW_DMA_HSIF2
#define DW_DMA_MAX_HSIFS		2
#endif
#ifdef CONFIG_DW_DMA_HSIF4
#define DW_DMA_MAX_HSIFS		4
#endif
#ifdef CONFIG_DW_DMA_HSIF8
#define DW_DMA_MAX_HSIFS		8
#endif
#ifdef CONFIG_DW_DMA_HSIF16
#define DW_DMA_MAX_HSIFS		16
#endif
#ifdef CONFIG_DW_DMA_HSIF32
#define DW_DMA_MAX_HSIFS		32
#endif
#endif /* DW_DMA_MAX_HSIFS */

/* Table 3-2 Master Interface Configuration Parameters */
#ifndef DW_DMA_MAX_MASTERS
#ifdef CONFIG_DW_DMA_MSTIF2
#define DW_DMA_MAX_MASTERS(n)		2 /* snps,dma-masters */
#endif
#ifdef CONFIG_DW_DMA_MSTIF1
#define DW_DMA_MAX_MASTERS(n)		1 /* snps,dma-masters */
#endif
#endif /* DW_DMA_MAX_MASTERS */
#ifndef DW_DMA_M_ADDR_WIDTH
#ifdef CONFIG_DW_DMA_M_ADDR32
#define DW_DMA_M_ADDR_WIDTH(n)		32
#endif
#ifdef CONFIG_DW_DMA_M_ADDR48
#define DW_DMA_M_ADDR_WIDTH(n)		48
#endif
#ifdef CONFIG_DW_DMA_M_ADDR49
#define DW_DMA_M_ADDR_WIDTH(n)		49
#endif
#ifdef CONFIG_DW_DMA_M_ADDR50
#define DW_DMA_M_ADDR_WIDTH(n)		50
#endif
#ifdef CONFIG_DW_DMA_M_ADDR51
#define DW_DMA_M_ADDR_WIDTH(n)		51
#endif
#ifdef CONFIG_DW_DMA_M_ADDR52
#define DW_DMA_M_ADDR_WIDTH(n)		52
#endif
#ifdef CONFIG_DW_DMA_M_ADDR53
#define DW_DMA_M_ADDR_WIDTH(n)		53
#endif
#ifdef CONFIG_DW_DMA_M_ADDR54
#define DW_DMA_M_ADDR_WIDTH(n)		54
#endif
#ifdef CONFIG_DW_DMA_M_ADDR55
#define DW_DMA_M_ADDR_WIDTH(n)		55
#endif
#ifdef CONFIG_DW_DMA_M_ADDR56
#define DW_DMA_M_ADDR_WIDTH(n)		56
#endif
#ifdef CONFIG_DW_DMA_M_ADDR57
#define DW_DMA_M_ADDR_WIDTH(n)		57
#endif
#ifdef CONFIG_DW_DMA_M_ADDR58
#define DW_DMA_M_ADDR_WIDTH(n)		58
#endif
#ifdef CONFIG_DW_DMA_M_ADDR59
#define DW_DMA_M_ADDR_WIDTH(n)		59
#endif
#ifdef CONFIG_DW_DMA_M_ADDR60
#define DW_DMA_M_ADDR_WIDTH(n)		60
#endif
#ifdef CONFIG_DW_DMA_M_ADDR61
#define DW_DMA_M_ADDR_WIDTH(n)		61
#endif
#ifdef CONFIG_DW_DMA_M_ADDR62
#define DW_DMA_M_ADDR_WIDTH(n)		62
#endif
#ifdef CONFIG_DW_DMA_M_ADDR63
#define DW_DMA_M_ADDR_WIDTH(n)		63
#endif
#ifdef CONFIG_DW_DMA_M_ADDR64
#define DW_DMA_M_ADDR_WIDTH(n)		64
#endif
#endif /* DW_DMA_M_ADDR_WIDTH */
#ifndef DW_DMA_M_DATA_WIDTH
#ifdef CONFIG_DW_DMA_M_DATA32
#define DW_DMA_M_DATA_WIDTH(n)		32 /* snps,data-width */
#endif
#ifdef CONFIG_DW_DMA_M_DATA64
#define DW_DMA_M_DATA_WIDTH(n)		64 /* snps,data-width */
#endif
#ifdef CONFIG_DW_DMA_M_DATA128
#define DW_DMA_M_DATA_WIDTH(n)		128 /* snps,data-width */
#endif
#ifdef CONFIG_DW_DMA_M_DATA256
#define DW_DMA_M_DATA_WIDTH(n)		256 /* snps,data-width */
#endif
#ifdef CONFIG_DW_DMA_M_DATA512
#define DW_DMA_M_DATA_WIDTH(n)		512 /* snps,data-width */
#endif
#endif /* DW_DMA_M_DATA_WIDTH */
#ifndef DW_DMA_M_BURSTLEN_WIDTH
#ifdef CONFIG_DW_DMA_M_BURSTLEN4
#define DW_DMA_M_BURSTLEN_WIDTH(n)	4 /* snps,axi-max-burst-len */
#endif
#ifdef CONFIG_DW_DMA_M_BURSTLEN5
#define DW_DMA_M_BURSTLEN_WIDTH(n)	5 /* snps,axi-max-burst-len */
#endif
#ifdef CONFIG_DW_DMA_M_BURSTLEN6
#define DW_DMA_M_BURSTLEN_WIDTH(n)	6 /* snps,axi-max-burst-len */
#endif
#ifdef CONFIG_DW_DMA_M_BURSTLEN7
#define DW_DMA_M_BURSTLEN_WIDTH(n)	7 /* snps,axi-max-burst-len */
#endif
#ifdef CONFIG_DW_DMA_M_BURSTLEN8
#define DW_DMA_M_BURSTLEN_WIDTH(n)	8 /* snps,axi-max-burst-len */
#endif
#endif /* DW_DMA_M_BURSTLEN_WIDTH */

/* Table 3-3 Slave Interface Configuration Parameters */
#ifndef DW_DMA_S_DATA_WIDTH
#ifdef CONFIG_DW_DMA_S_DATA32
#define DW_DMA_S_DATA_WIDTH(n)		32
#endif
#ifdef CONFIG_DW_DMA_S_DATA64
#define DW_DMA_S_DATA_WIDTH(n)		64
#endif
#endif /* DW_DMA_S_DATA_WIDTH */

/* Table 3-6 Channel x Configuration Parameters */
#ifndef DW_DMA_C_MAX_MSIZE
#ifdef CONFIG_DW_DMA_C_MSIZE1
#define DW_DMA_C_MAX_MSIZE(n, x)	DMA_MSIZE1
#endif
#ifdef CONFIG_DW_DMA_C_MSIZE4
#define DW_DMA_C_MAX_MSIZE(n, x)	DMA_MSIZE4
#endif
#ifdef CONFIG_DW_DMA_C_MSIZE8
#define DW_DMA_C_MAX_MSIZE(n, x)	DMA_MSIZE8
#endif
#ifdef CONFIG_DW_DMA_C_MSIZE16
#define DW_DMA_C_MAX_MSIZE(n, x)	DMA_MSIZE16
#endif
#ifdef CONFIG_DW_DMA_C_MSIZE32
#define DW_DMA_C_MAX_MSIZE(n, x)	DMA_MSIZE32
#endif
#ifdef CONFIG_DW_DMA_C_MSIZE64
#define DW_DMA_C_MAX_MSIZE(n, x)	DMA_MSIZE64
#endif
#ifdef CONFIG_DW_DMA_C_MSIZE128
#define DW_DMA_C_MAX_MSIZE(n, x)	DMA_MSIZE128
#endif
#ifdef CONFIG_DW_DMA_C_MSIZE256
#define DW_DMA_C_MAX_MSIZE(n, x)	DMA_MSIZE256
#endif
#ifdef CONFIG_DW_DMA_C_MSIZE512
#define DW_DMA_C_MAX_MSIZE(n, x)	DMA_MIZE512
#endif
#ifdef CONFIG_DW_DMA_C_MSIZE1024
#define DW_DMA_C_MAX_MSIZE(n, x)	DMA_MSIZE1024
#endif
#endif /* DW_DMA_C_MAX_MSIZE */
#ifndef DW_DMA_C_MAX_BLOCKTS
#ifdef CONFIG_DW_DMA_C_BLOCKTS2
#define DW_DMA_C_MAX_BLOCKTS(n, x)	_BV(2)
#endif
#ifdef CONFIG_DW_DMA_C_BLOCKTS3
#define DW_DMA_C_MAX_BLOCKTS(n, x)	_BV(3)
#endif
#ifdef CONFIG_DW_DMA_C_BLOCKTS4
#define DW_DMA_C_MAX_BLOCKTS(n, x)	_BV(4)
#endif
#ifdef CONFIG_DW_DMA_C_BLOCKTS5
#define DW_DMA_C_MAX_BLOCKTS(n, x)	_BV(5)
#endif
#ifdef CONFIG_DW_DMA_C_BLOCKTS6
#define DW_DMA_C_MAX_BLOCKTS(n, x)	_BV(6)
#endif
#ifdef CONFIG_DW_DMA_C_BLOCKTS7
#define DW_DMA_C_MAX_BLOCKTS(n, x)	_BV(7)
#endif
#ifdef CONFIG_DW_DMA_C_BLOCKTS8
#define DW_DMA_C_MAX_BLOCKTS(n, x)	_BV(8)
#endif
#ifdef CONFIG_DW_DMA_C_BLOCKTS9
#define DW_DMA_C_MAX_BLOCKTS(n, x)	_BV(9)
#endif
#ifdef CONFIG_DW_DMA_C_BLOCKTS10
#define DW_DMA_C_MAX_BLOCKTS(n, x)	_BV(10)
#endif
#ifdef CONFIG_DW_DMA_C_BLOCKTS11
#define DW_DMA_C_MAX_BLOCKTS(n, x)	_BV(11)
#endif
#ifdef CONFIG_DW_DMA_C_BLOCKTS12
#define DW_DMA_C_MAX_BLOCKTS(n, x)	_BV(12)
#endif
#ifdef CONFIG_DW_DMA_C_BLOCKTS13
#define DW_DMA_C_MAX_BLOCKTS(n, x)	_BV(13)
#endif
#ifdef CONFIG_DW_DMA_C_BLOCKTS14
#define DW_DMA_C_MAX_BLOCKTS(n, x)	_BV(14)
#endif
#ifdef CONFIG_DW_DMA_C_BLOCKTS15
#define DW_DMA_C_MAX_BLOCKTS(n, x)	_BV(15)
#endif
#ifdef CONFIG_DW_DMA_C_BLOCKTS16
#define DW_DMA_C_MAX_BLOCKTS(n, x)	_BV(16)
#endif
#ifdef CONFIG_DW_DMA_C_BLOCKTS17
#define DW_DMA_C_MAX_BLOCKTS(n, x)	_BV(17)
#endif
#ifdef CONFIG_DW_DMA_C_BLOCKTS18
#define DW_DMA_C_MAX_BLOCKTS(n, x)	_BV(18)
#endif
#ifdef CONFIG_DW_DMA_C_BLOCKTS19
#define DW_DMA_C_MAX_BLOCKTS(n, x)	_BV(19)
#endif
#ifdef CONFIG_DW_DMA_C_BLOCKTS20
#define DW_DMA_C_MAX_BLOCKTS(n, x)	_BV(20)
#endif
#ifdef CONFIG_DW_DMA_C_BLOCKTS21
#define DW_DMA_C_MAX_BLOCKTS(n, x)	_BV(21)
#endif
#ifdef CONFIG_DW_DMA_C_BLOCKTS22
#define DW_DMA_C_MAX_BLOCKTS(n, x)	_BV(22)
#endif
#endif /* DW_DMA_C_MAX_BLOCKTS */
#ifndef DW_DMA_C_MAX_BURSTLEN
#ifdef CONFIG_DW_DMA_C_BURSTLEN1
#define DW_DMA_MAX_C_BURSTLEN(n, x)	1
#endif
#ifdef CONFIG_DW_DMA_C_BURSTLEN4
#define DW_DMA_C_MAX_BURSTLEN(n, x)	4
#endif
#ifdef CONFIG_DW_DMA_C_BURSTLEN8
#define DW_DMA_C_MAX_BURSTLEN(n, x)	8
#endif
#ifdef CONFIG_DW_DMA_C_BURSTLEN16
#define DW_DMA_C_MAX_BURSTLEN(n, x)	16
#endif
#ifdef CONFIG_DW_DMA_C_BURSTLEN32
#define DW_DMA_C_MAX_BURSTLEN(n, x)	32
#endif
#ifdef CONFIG_DW_DMA_C_BURSTLEN64
#define DW_DMA_C_MAX_BURSTLEN(n, x)	64
#endif
#ifdef CONFIG_DW_DMA_C_BURSTLEN128
#define DW_DMA_C_MAX_BURSTLEN(n, x)	128
#endif
#ifdef CONFIG_DW_DMA_C_BURSTLEN256
#define DW_DMA_C_MAX_BURSTLEN(n, x)	256
#endif
#endif /* DW_DMA_C_MAX_BURSTLEN */

#ifndef DW_DMA_BASE
#define DW_DMA_BASE(n)		(DW_DMA##n##_BASE)
#endif
#ifndef DW_DMA_REG
#define DW_DMA_REG(n, offset)	(DW_DMA_BASE(n) + (offset))
#endif
#ifndef NR_DW_DMAS
#define NR_DW_DMAS		1
#endif

#define INVALID_DW_DMA_CHIP	DW_DMA_MAX_CHIPS
#define INVALID_DW_DMA_CHAN	DW_DMA_MAX_CHANS

/* x is 0...DMAC_MAX_ChANNELS */
#define DW_DMA_CHREG(n, x, offset)		\
	DW_DMA_REG((n), 0x100 + ((x) << 8) + (offset))
#define DW_DMA_LLI(lli, offset)			(((caddr_t)(lli)) + (offset))

#define DMAC_IDREG(n)				DW_DMA_REG(n, 0x00)
#define DMAC_COMPVERREG(n)			DW_DMA_REG(n, 0x08)
#define DMAC_CFGREG(n)				DW_DMA_REG(n, 0x10)
#if DW_DMA_MAX_CHANNELS <= 8
#define DMAC_CHENREG(n)				DW_DMA_REG(n, 0x18)
#define DMAC_CHENREG_LO(n)			DW_DMA_REG(n, 0x18)
#define DMAC_CHENREG_HI(n)			DW_DMA_REG(n, 0x1C)
#define DMAC_INTSTATUSREG(n)			DW_DMA_REG(n, 0x30)
#else /* DW_DMA_MAX_CHANNELS > 8 */
#define DMAC_CHENREG2(n)			DW_DMA_REG(n, 0x18)
#define DMAC_CHENREG2_LO(n)			DW_DMA_REG(n, 0x18)
#define DMAC_CHENREG2_HI(n)			DW_DMA_REG(n, 0x1C)
#define DMAC_CHSUSPREG(n)			DW_DMA_REG(n, 0x20)
#define DMAC_CHABORTREG(n)			DW_DMA_REG(n, 0x28)
#define DMAC_INTSTATUSREG2(n)			DW_DMA_REG(n, 0x30)
#define DMAC_INTSTATUSREG2_LO(n)		DW_DMA_REG(n, 0x30)
#define DMAC_INTSTATUSREG2_HI(n)		DW_DMA_REG(n, 0x34)
#endif /* DW_DMA_MAX_CHANNELS */
#define DMAC_COMMONREG_INTCLEARREG(n)		DW_DMA_REG(n, 0x38)
#define DMAC_COMMONREG_INTSTATUS_ENABLEREG(n)	DW_DMA_REG(n, 0x40)
#define DMAC_COMMONREG_INTSIGNAL_ENABLEREG(n)	DW_DMA_REG(n, 0x48)
#define DMAC_COMMONREG_INTSTATUSREG(n)		DW_DMA_REG(n, 0x50)
#define DMAC_RESETREG(n)			DW_DMA_REG(n, 0x58)

/* 5.1.2 DMAC_COMPVERREG */
#define DMA_DMAC_COMPVER_OFFSET		0
#define DMA_DMAC_COMPVER_MASK		REG_32BIT_MASK
#define DMA_DMAC_COMPVER(value)		_GET_FV(DMA_DMAC_COMPVER, value)

/* 5.1.3 DMAC_CFGREG */
#define DMA_DMAC_EN			_BV(0)
#define DMA_INT_EN			_BV(0)

#if DW_DMA_MAX_CHANNELS <= 8
/* 5.1.4 DMAC_CHENREG */
/* DMAC_CHENREG_LO */
#define DMA_CH_EN(n)			REG32_1BIT_OFFSET(n)
#define DMA_CH_EN_WE(n)			REG32_1BIT_OFFSET((n) + 8)
#define DMA_CH_SUSP(n)			REG32_1BIT_OFFSET((n) + 16)
#define DMA_CH_SUSP_WE(n)		REG32_1BIT_OFFSET((n) + 24)
/* DMAC_CHENREG_HI */
#define DMA_CH_ABORT(n)			REG32_1BIT_OFFSET(n)
#define DMA_CH_ABORT_WE(n)		REG32_1BIT_OFFSET((n) + 8)

/* 5.1.8 DMAC_INTSTATUSREG */
#define DMA_CommonReg_IntStat		_BV(16)
#define DMA_CH_IntStat(n)		REG32_1BIT_OFFSET(n)
#else /* DW_MAX_CHANNELS > 8 */
/* 5.1.9 DMAC_INTSTATUSREG2 */
/* DMAC_INTSTATUSREG2_LO */
#define DMA_CH_IntStat(n)		REG32_1BIT_OFFSET(n)
/* DMAC_INTSTATUSREG2_HI */
#define DMA_CommonReg_IntStat		_BV(0)
#endif

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

/* 5.2 DW_axi_dmac_mem_map/Channelx_Registers_Address_Block Registers */
#define DMA_CH_SAR(n, x)			DW_DMA_CHREG(n, x, 0x00)
#define DMA_CH_DAR(n, x)			DW_DMA_CHREG(n, x, 0x08)
#define DMA_CH_BLOCK_TS(n, x)			DW_DMA_CHREG(n, x, 0x10)
#define DMA_CH_CTL(n, x)			DW_DMA_CHREG(n, x, 0x18)
#define DMA_CH_CTL_LO(n, x)			DW_DMA_CHREG(n, x, 0x18)
#define DMA_CH_CTL_HI(n, x)			DW_DMA_CHREG(n, x, 0x1C)
#if DW_DMA_MAX_CHANNELS <= 8 && DW_DMA_MAX_HSIFS <= 16
#define DMA_CH_CFG(n, x)			DW_DMA_CHREG(n, x, 0x20)
#define DMA_CH_CFG_LO(n, x)			DW_DMA_CHREG(n, x, 0x20)
#define DMA_CH_CFG_HI(n, x)			DW_DMA_CHREG(n, x, 0x24)
#else /* DW_DMA_MAX_CHANNELS > 8 */
#define DMA_CH_CFG2(n, x)			DW_DMA_CHREG(n, x, 0x20)
#endif /* DW_DMA_MAX_CHANNELS */
#define DMA_CH_LLP(n, x)			DW_DMA_CHREG(n, x, 0x28)
#define DMA_CH_STATUSREG(n, x)			DW_DMA_CHREG(n, x, 0x30)
#define DMA_CH_STATUSREG_LO(n, x)		DW_DMA_CHREG(n, x, 0x30)
#define DMA_CH_STATUSREG_HI(n, x)		DW_DMA_CHREG(n, x, 0x34)
#define DMA_CH_SWHSSRCREG(n, x)			DW_DMA_CHREG(n, x, 0x38)
#define DMA_CH_SWHSDSTREG(n, x)			DW_DMA_CHREG(n, x, 0x40)
#define DMA_CH_BLK_TFR_RESUMEREQREG(n, x)	DW_DMA_CHREG(n, x, 0x48)
#define DMA_CH_AXI_IDREG(n, x)			DW_DMA_CHREG(n, x, 0x50)
#define DMA_CH_AXI_QOSREG(n, x)			DW_DMA_CHREG(n, x, 0x58)
#define DMA_CH_SSTAT(n, x)			DW_DMA_CHREG(n, x, 0x60)
#define DMA_CH_DSTAT(n, x)			DW_DMA_CHREG(n, x, 0x68)
#define DMA_CH_SSTATAR(n, x)			DW_DMA_CHREG(n, x, 0x70)
#define DMA_CH_DSTATAR(n, x)			DW_DMA_CHREG(n, x, 0x78)
#define DMA_CH_INTSTATUS_ENABLEREG(n, x)	DW_DMA_CHREG(n, x, 0x80)
#define DMA_CH_INTSTATUS(n, x)			DW_DMA_CHREG(n, x, 0x88)
#define DMA_CH_INTSIGNAL_ENABLEREG(n, x)	DW_DMA_CHREG(n, x, 0x90)
#define DMA_CH_INTCLEARREG(n, x)		DW_DMA_CHREG(n, x, 0x98)

/* 5.2.4 CHx_CTL */
/* CHx_CTL_LO */
#define DMA_NonPosted_LastWrite_En	_BV(30)
#define DMA_AW_CACHE_OFFSET		26
#define DMA_AW_CACHE_MASK		REG_4BIT_MASK
#define DMA_AW_CACHE(value)		_SET_FV(DMA_AW_CACHE, value)
#define DMA_AR_CACHE_OFFSET		22
#define DMA_AR_CACHE_MASK		REG_4BIT_MASK
#define DMA_AR_CACHE(value)		_SET_FV(DMA_AR_CACHE, value)
#define DMA_DST_MSIZE_OFFSET		18
#define DMA_DST_MSIZE_MASK		REG_4BIT_MASK
#define DMA_DST_MSIZE(value)		_SET_FV(DMA_DST_MSIZE, value)
#define DMA_SRC_MSIZE_OFFSET		14
#define DMA_SRC_MSIZE_MASK		REG_4BIT_MASK
#define DMA_SRC_MSIZE(value)		_SET_FV(DMA_SRC_MSIZE, value)
#define DMA_MSIZE1			0
#define DMA_MSIZE4			1
#define DMA_MSIZE8			2
#define DMA_MSIZE16			3
#define DMA_MSIZE32			4
#define DMA_MSIZE64			5
#define DMA_MSIZE128			6
#define DMA_MSIZE256			7
#define DMA_MSIZE512			8
#define DMA_MSIZE1024			9
#define DMA_DST_TR_WIDTH_OFFSET		11
#define DMA_DST_TR_WIDTH_MASK		REG_3BIT_MASK
#define DMA_DST_TR_WIDTH(value)		_SET_FV(DMA_DST_TR_WIDTH, value)
#define DMA_SRC_TR_WIDTH_OFFSET		8
#define DMA_SRC_TR_WIDTH_MASK		REG_3BIT_MASK
#define DMA_SRC_TR_WIDTH(value)		_SET_FV(DMA_SRC_TR_WIDTH, value)
#define DMA_DINC_OFFSET			6
#define DMA_DINC_MASK			REG_1BIT_MASK
#define DMA_DINC(value)			_SET_FV(DMA_DINC, value)
#define DMA_SINC_OFFSET			4
#define DMA_SINC_MASK			REG_1BIT_MASK
#define DMA_SINC(value)			_SET_FV(DMA_SINC, value)
#define DMA_INCR			0
#define DMA_FIXED			1
#define DMA_DMS_OFFSET			2
#define DMA_DMS_MASK			REG_1BIT_MASK
#define DMA_DMS(value)			_SET_FV(DMA_DMS, value)
#define DMA_SMS_OFFSET			0
#define DMA_SMS_MASK			REG_1BIT_MASK
#define DMA_SMS(value)			_SET_FV(DMA_DMS, value)
/* CHx_CTL_HI */
#define DMA_SHADOWREG_OR_LLI_VALID	_BV(31)
#define DMA_SHADOWREG_OR_LLI_LAST	_BV(30)
#define DMA_IOC_BlkTfr			_BV(26)
#define DMA_DST_STAT_EN			_BV(25)
#define DMA_SRC_STAT_EN			_BV(24)
#define DMA_AWLEN_OFFSET		16
#define DMA_AWLEN_MASK			REG_8BIT_MASK
#define DMA_AWLEN(value)		_SET_FV(DMA_AWLEN, value)
#define DMA_AWLEN_EN			_BV(15)
#define DMA_ARLEN_OFFSET		7
#define DMA_ARLEN_MASK			REG_8BIT_MASK
#define DMA_ARLEN(value)		_SET_FV(DMA_ARLEN, value)
#define DMA_ARLEN_EN			_BV(6)
#define DMA_MAX_AMBA_BURSTLEN		256
#define DMA_AW_PROT_OFFSET		3
#define DMA_AW_PROT_MASK		REG_3BIT_MASK
#define DMA_AW_PROT(value)		_SET_FV(DMA_AW_PROT, value)
#define DMA_AR_PROT_OFFSET		0
#define DMA_AR_PROT_MASK		REG_3BIT_MASK
#define DMA_AR_PROT(value)		_SET_FV(DMA_AR_PROT, value)

#if DW_DMA_MAX_CHANNELS <= 8
/* 5.2.5 CHx_CFG */
/* CHx_CFG_LO */
#define DMA_DST_MULTBLK_TYPE_OFFSET	2
#define DMA_DST_MULTBLK_TYPE_MASK	REG_2BIT_MASK
#define DMA_DST_MULTBLK_TYPE(value)	_SET_FV(DMA_DST_MULTBLK_TYPE, value)
#define DMA_SRC_MULTBLK_TYPE_OFFSET	0
#define DMA_SRC_MULTBLK_TYPE_MASK	REG_2BIT_MASK
#define DMA_SRC_MULTBLK_TYPE(value)	_SET_FV(DMA_SRC_MULTBLK_TYPE, value)
#define DMA_MULTBLK_TYPE_CONTIGUOUS		0
#define DMA_MULTBLK_TYPE_RELOAD			1
#define DMA_MULTBLK_TYPE_SHADOW_REGISTER	2
#define DMA_MULTBLK_TYPE_LINKED_LIST		3
/* CHx_CFG_HI */
#define DMA_TT_FC_OFFSET		0
#define DMA_TT_FC_MASK			REG_3BIT_MASK
#define DMA_TT_FC(value)		_SET_FV(DMA_TT_FC, value)
#define DMA_TT_MEM_TO_MEM_FC_DMAC	0
#define DMA_TT_MEM_TO_PER_FC_DMAC	1
#define DMA_TT_PER_TO_MEM_FC_DMAC	2
#define DMA_TT_PER_TO_PER_FC_DMAC	3
#define DMA_TT_PER_TO_MEM_FC_SRC	4
#define DMA_TT_PER_TO_PER_FC_SRC	5
#define DMA_TT_MEM_TO_PER_FC_DST	6
#define DMA_TT_PER_TO_PER_FC_DST	7
#define DMA_HS_SEL_SRC			_BV(3)
#define DMA_HS_SEL_DST			_BV(4)
#define DMA_SRC_HWHS_POL		_BV(5)
#define DMA_DST_HWHS_POL		_BV(6)
#define DMA_SRC_PER(n)			REG32_1BIT_OFFSET((n) + 7)
#define DMA_DST_PER(n)			REG32_1BIT_OFFSET((n) + 12)
#define DMA_CH_PRIOR_OFFSET		17
#define DMA_CH_PRIOR_MASK		REG_3BIT_MASK
#define DMA_CH_PRIOR(value)		_SET_FV(DMA_CH_PRIOR, value)
#define DMA_LOCK_CH			_BV(20)
#define DMA_LOCK_CH_L_OFFSET		21
#define DMA_LOCK_CH_L_MASK		REG_2BIT_MASK
#define DMA_LOCK_CH_L(value)		_SET_FV(DMA_LOCK_CH_L, value)
#define DMA_SRC_OSR_LMT_OFFSET		23
#define DMA_SRC_OSR_LMT_MASK		REG_4BIT_MASK
#define DMA_SRC_OSR_LMT(value)		_SET_FV(DMA_SRC_OSR_LMT, value)
#define DMA_DST_OSR_LMT_OFFSET		27
#define DMA_DST_OSR_LMT_MASK		REG_4BIT_MASK
#define DMA_DST_OSR_LMT(value)		_SET_FV(DMA_DST_OSR_LMT, value)
#endif /* DW_DMA_MAX_CHANNELS <= 8 */

/* 5.2.7 CHx_LLP */
#define DMA_LMS_OFFSET			0
#define DMA_LMS_MASK			REG_1BIT_MASK
#define DMA_LMS(value)			_SET_FV(DMA_LMS, value)

/* DMA_SMS/DMA_DMS/DMA_LMS */
#define DMA_MS_AXI1			0
#define DMA_MS_AXI2			1

/* 5.2.8 CHx_STATUSREG */
/* CHx_STATUSREG_LO */
#define DMA_CMPLTD_BLK_TFR_SIZE_OFFSET	0
#define DMA_CMPLTD_BLK_TFR_SIZE_MASK	REG_22BIT_MASK
#define DMA_CMPLTD_BLK_TFR_SIZE(value)	_GET_FV(DMA_CMPLTD_BLK_TFR_SIZE, value)
/* CHx_STATUSREG_HI */
#define DMA_DATA_LEFT_IN_FIFO_OFFSET	0
#define DMA_DATA_LEFT_IN_FIFO_MASK	REG_15BIT_MASK
#define DMA_DATA_LEFT_IN_FIFO(value)	_GET_FV(DMA_DATA_LEFT_IN_FIFO, value)

/* 5.2.9 CHx_SWHSSRCREG */
#define DMA_SWHS_REQ_SRC		_BV(0)
#define DMA_SWHS_REQ_SRC_WE		_BV(1)
#define DMA_SWHS_SGLREQ_SRC		_BV(2)
#define DMA_SWHS_SGLREQ_SRC_WE		_BV(3)

/* 5.2.10 CHx_SWHSDSTREG */
#define DMA_SWHS_REQ_DST		_BV(0)
#define DMA_SWHS_REQ_DST_WE		_BV(1)
#define DMA_SWHS_SGLREQ_DST		_BV(2)
#define DMA_SWHS_SGLREQ_DST_WE		_BV(3)
#define DMA_SWHS_LST_DST		_BV(4)
#define DMA_SWHS_LST_DST_WE		_BV(5)

/* 5.2.11 CHx_BLK_TFR_RESUMEREQREG */
#define DMA_BLK_TFR_RESUMEREQ		_BV(0)

/* 5.2.14 CHx_SSTAT */
#define DMA_SSTAT(n)			REG_1BIT_OFFSET(n)

/* 5.2.15 CHx_DSTAT */
#define DMA_DSTAT(n)			REG_1BIT_OFFSET(n)

/* 5.2.16 CHx_SSTATAR */
#define DMA_SSTATAR(n)			REG_1BIT_OFFSET(n)

/* 5.2.17 CHx_DSTATAR */
#define DMA_DSTATAR(n)			REG_1BIT_OFFSET(n)

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

/* 2.13.2.4 Linked List */
#define DMA_LLI_CH_SAR(lli)			DW_DMA_LLI(lli, 0x00)
#define DMA_LLI_CH_SAR_LO(lli)			DW_DMA_LLI(lli, 0x00)
#define DMA_LLI_CH_SAR_HI(lli)			DW_DMA_LLI(lli, 0x04)
#define DMA_LLI_CH_DAR(lli)			DW_DMA_LLI(lli, 0x08)
#define DMA_LLI_CH_DAR_LO(lli)			DW_DMA_LLI(lli, 0x08)
#define DMA_LLI_CH_DAR_HI(lli)			DW_DMA_LLI(lli, 0x0C)
#define DMA_LLI_CH_BLOCK_TS(lli)		DW_DMA_LLI(lli, 0x10)
#define DMA_LLI_CH_BLOCK_TS_LO(lli)		DW_DMA_LLI(lli, 0x10)
#define DMA_LLI_CH_BLOCK_TS_HI(lli)		DW_DMA_LLI(lli, 0x14)
#define DMA_LLI_CH_LLP(lli)			DW_DMA_LLI(lli, 0x18)
#define DMA_LLI_CH_LLP_LO(lli)			DW_DMA_LLI(lli, 0x18)
#define DMA_LLI_CH_LLP_HI(lli)			DW_DMA_LLI(lli, 0x1C)
#define DMA_LLI_CH_CTL(lli)			DW_DMA_LLI(lli, 0x20)
#define DMA_LLI_CH_CTL_LO(lli)			DW_DMA_LLI(lli, 0x20)
#define DMA_LLI_CH_CTL_HI(lli)			DW_DMA_LLI(lli, 0x24)
#define DMA_LLI_CH_SSTAT(lli)			DW_DMA_LLI(lli, 0x28)
#define DMA_LLI_CH_DSTAT(lli)			DW_DMA_LLI(lli, 0x2C)
#define DMA_LLI_CH_LLP_STATUS(lli)		DW_DMA_LLI(lli, 0x30)
#define DMA_LLI_CH_LLP_STATUS_LO(lli)		DW_DMA_LLI(lli, 0x30)
#define DMA_LLI_CH_LLP_STATUS_HI(lli)		DW_DMA_LLI(lli, 0x34)
#define DMA_LLI_DESC_SIZE			0x40

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

#define dw_dmac_disable_rirq(n, x, irqs)			\
	__raw_clearl(irqs, DMA_CH_INTSTATUS_ENABLEREG(n, x))
#define dw_dmac_disable_all_rirqs(n, x)				\
	__raw_writel(0, DMA_CH_INTSTATUS_ENABLEREG(n, x))
#define dw_dmac_enable_rirq(n, x, irqs)				\
	__raw_setl(irqs, DMA_CH_INTSTATUS_ENABLEREG(n, x))
#define dw_dmac_enable_irq(n, x, irqs)				\
	__raw_setl(irqs, DMA_CH_INTSIGNAL_ENABLEREG(n, x))
#define dw_dmac_disable_irq(n, x, irqs)				\
	__raw_clearl(irqs, DMA_CH_INTSIGNAL_ENABLEREG(n, x))
#define dw_dmac_clear_irq(n, x, irqs)				\
	__raw_setl(irqs, DMA_CH_INTCLEARREG(n, x))
#define dw_dmac_irq_status(n, x)				\
	__raw_readl(DMA_CH_INTSTATUS(n, x))

#define dw_dmac_config_multblk(n, x, type)			\
	__raw_writel_mask(DMA_SRC_MULTBLK_TYPE(type) | 		\
		DMA_DST_MULTBLK_TYPE(type),			\
		DMA_SRC_MULTBLK_TYPE(DMA_SRC_MULTBLK_TYPE_MASK)|\
		DMA_DST_MULTBLK_TYPE(DMA_DST_MULTBLK_TYPE_MASK),\
		DMA_CH_CFG_LO(n, x))
#define dw_dmac_config_tt_fc(n, x, tt_fc)			\
	__raw_writel_mask(DMA_TT_FC(tt_fc),			\
			  DMA_TT_FC(DMA_TT_FC_MASK),		\
			  DMA_CH_CFG_HI(n, x))
#define dw_dmac_config_priority(n, x, prior)			\
	__raw_writel_mask(DMA_CH_PRIOR(prior),			\
			  DMA_CH_PRIOR(DMA_CH_PRIOR_MASK),	\
			  DMA_CH_CFG_HI(n, x))
#define DMA_HWHS_POL_MASK		(DMA_SRC_HWHS_POL | DMA_DST_HWHS_POL)
#define DMA_HS_SEL_MASK			(DMA_HS_SEL_SRC | DMA_HS_SEL_DST)
#define DMA_HWHS_POL_ACTIVE_HIGH	0
#define DMA_HWHS_POL_ACTIVE_LOW		DMA_HWHS_POL_MASK
#define dw_dmac_config_handshaking_hw(n, x, low)		\
	__raw_writel_mask((low) ? DMA_HWHS_POL_ACTIVE_LOW :	\
			  DMA_HWHS_POL_ACTIVE_HIGH,		\
			  DMA_HS_SEL_MASK | DMA_HWHS_POL_MASK,	\
			  DMA_CH_CFG_HI(n, x))
#define dw_dmac_config_handshaking_sw(n, x)			\
	__raw_writel_mask(DMA_HS_SEL_SRC | DMA_HS_SEL_DST,	\
		          DMA_HS_SEL_MASK,			\
			  DMA_CH_CFG_HI(n, x))

/* Figure 2-32 DW_axi_dmac Linked List Item (Descriptor) */
struct dw_dma_lli {
	uint64_t sar;
	uint64_t dar;
	uint64_t block_ts;
	uint64_t llp;
	uint32_t ctl_lo;
	uint32_t ctl_hi;
	uint32_t sstat;
	uint32_t dstat;
	uint32_t status_lo;
	uint32_t status_hi;
	uint64_t reserved1;
} __packed __align(8);

/*===========================================================================
 * LLI_SAR
 *===========================================================================*/
#define dw_dma_lli_set_sar(lli, addr)			\
	__raw_writeq_le(addr, DMA_LLI_CH_SAR(lli))
/*===========================================================================
 * LLI_DAR
 *===========================================================================*/
#define dw_dma_lli_set_dar(lli, addr)			\
	__raw_writeq_le(addr, DMA_LLI_CH_DAR(lli))
/*===========================================================================
 * LLI_BLOCK_TS
 *===========================================================================*/
#define dw_dma_lli_set_block_ts(lli, block_ts)		\
	__raw_writeq_le(block_ts, DMA_LLI_CH_BLOCK_TS(lli))
/*===========================================================================
 * LLI_LLP
 *===========================================================================*/
#define dw_dma_lli_set_llp(lli, addr, lms)		\
	__raw_writeq_le(addr | DMA_LMS(lms), DMA_LLI_CH_LLP(lli))
/*===========================================================================
 * LLI_CTL
 *===========================================================================*/
#define dw_dma_lli_set_ctl(lli)				\
	__raw_writeq_le(DMA_SHADOWREG_OR_LLI_VALID,	\
			DMA_LLI_CH_CTL_HI(lli))
#define dw_dma_lli_set_last(lli)			\
	__raw_setl_le(DMA_SHADOWREG_OR_LLI_LAST,	\
		      DMA_LLI_CH_CTL_HI(lli))
#define dw_dma_lli_set_sms(lli, ms)			\
	__raw_clearl_le(DMA_SMS(ms), DMA_LLI_CH_CTL_LO(lli))
#define dw_dma_lli_set_dms(lli, ms)			\
	__raw_clearl_le(DMA_DMS(ms), DMA_LLI_CH_CTL_LO(lli))
#define dw_dma_lli_set_arlen(lli, len)			\
	__raw_setl_le(DMA_ARLEN(len) | DMA_ARLEN_EN,	\
		      DMA_LLI_CH_CTL_HI(lli))
#define dw_dma_lli_set_awlen(lli, len)			\
	__raw_setl_le(DMA_AWLEN(len) | DMA_AWLEN_EN,	\
		      DMA_LLI_CH_CTL_HI(lli))
#define dw_dma_lli_set_src(lli, msize, tr_width, incr)	\
	__raw_setl_le(DMA_SRC_MSIZE(msize) | 		\
		      DMA_SRC_TR_WIDTH(tr_width) |	\
		      DMA_SINC(incr), DMA_LLI_CH_CTL_LO(lli))
#define dw_dma_lli_set_dst(lli, msize, tr_width, incr)	\
	__raw_setl_le(DMA_DST_MSIZE(msize) | 		\
		      DMA_DST_TR_WIDTH(tr_width) |	\
		      DMA_DINC(incr), DMA_LLI_CH_CTL_LO(lli))

struct dw_dma_desc {
	struct dw_dma_lli lli;
	dma_t dma;
	phys_addr_t phys;
	struct list_head link;
};

struct dw_dma_pool {
	struct list_head descs;
};

struct dw_dma_chan {
	uint8_t chip;
	uint8_t chan;
	dma_t dma;
	irq_t irq;
	uint32_t priority;
	bool device_fc;
	struct list_head descs;
};

struct dw_dma_chip {
	irq_t irq;
	uint32_t nr_channels;
	uint32_t nr_masters;
};

void dw_dma_chip_init(int chip, irq_t irq);
void dw_dma_chan_init(dma_t id, int chip, int chan,
		      dma_t dma, irq_t irq);
void dw_dma_irq_handle(int chip, int chan);
int dw_dma_memcpy_async(dma_t id, dma_addr_t dst, dma_addr_t src, size_t len);
int dw_dma_memcpy_sync(dma_t id, dma_addr_t dst, dma_addr_t src, size_t len);

#endif /* __DW_DMA_H_INCLUDE__ */
