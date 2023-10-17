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

#define BUF_LEN 1024
static uint8_t src_buf[BUF_LEN];
static uint8_t dst_buf[BUF_LEN];
volatile static int ch2_done = 0;
static uint64_t base_addr = 0;

#define SYS_DMAC_BADDR 0x100000

#define DMAC_IDREG_BLK0_OFFSET 0x0
#define DMAC_COMPVERREG_BLK0_OFFSET 0x8
#define DMAC_CFGREG_BLK0_OFFSET 0x10
#define DMAC_CHENREG_BLK0_OFFSET 0x18
#define DMAC_INTSTATUSREG_BLK0_OFFSET 0x30
#define DMAC_COMMONREG_INTCLEARREG_BLK0_OFFSET 0x38
#define DMAC_COMMONREG_INTSTATUS_ENABLEREG_BLK0_OFFSET 0x40
#define DMAC_COMMONREG_INTSIGNAL_ENABLEREG_BLK0_OFFSET 0x48
#define DMAC_COMMONREG_INTSTATUSREG_BLK0_OFFSET 0x50
#define DMAC_RESETREG_BLK0_OFFSET 0x58
#define DMAC_LOWPOWER_CFGREG_BLK0_OFFSET 0x60
#define CH1_SAR_BLK1_OFFSET 0x100
#define CH1_DAR_BLK1_OFFSET 0x108
#define CH1_BLOCK_TS_BLK1_OFFSET 0x110
#define CH1_CTL_BLK1_OFFSET 0x118
#define CH1_CFG_BLK1_OFFSET 0x120
#define CH1_LLP_BLK1_OFFSET 0x128
#define CH1_STATUSREG_BLK1_OFFSET 0x130
#define CH1_SWHSSRCREG_BLK1_OFFSET 0x138
#define CH1_SWHSDSTREG_BLK1_OFFSET 0x140
#define CH1_BLK_TFR_RESUMEREQREG BLK1 OFFSET 0x148
#define CH1_AXI_IDREG_BLK1 OFFSET 0x150
#define CH1_AXI_QOSREG_BLK1_OFFSEI 0x158
#define CH1_INTSTATUS_ENABLEREG_BLK1_OFFSET 0x180
#define CH1_INTSTATUS_BLK1_OFFSET 0x188
#define CH1_INTSIGNAL_ENABLEREG_BLK1_OFFSET 0x190
#define CH1_INTCLEARREG_BLK1_OFFSET 0x198
#define CH2_SAR_BLK2_OFFSET 0x200
#define CH2_DAR_BLK2_OFFSET 0x208
#define CH2_BLOCK_TS_BLK2_OFFSET 0x210
#define CH2_CTL_BLK2_OFFSET 0x218
#define CH2_CFG_BLK2_OFFSET 0x220
#define CH2_LLP_BLK2_OFFSET 0x228
#define CH2_STATUSREG_BLK2_OFFSET 0x230
#define CH2_SWHSSRCREG_BLK2_OFFSET 0x238
#define CH2_SWHSDSTREG_BLK2_OFFSET 0x240
#define CH2_BLK_TFR RESUMEREQREG_BLK2_OFFSET 0x248
#define CH2_AXI_IDREG_BLK2_OFFSET 0x250
#define CH2_AXI_QOSREG_BLK2_OFFSET 0x258
#define CH2_INTSTATUS_ENABLEREG_BLK2_OFFSET 0x280
#define CH2_INTSTATUS_BLK2_OFFSET 0x288
#define CH2_INTSIGNAL_ENABLEREG_BLK2_OFFSET 0x290
#define CH2_INTCLEARREG_BLK2_OFFSET 0x298

static __inline void writel(uint32_t v, caddr_t a)
{
	sbi_printf("wr32 addr:0x%lx, val:%lx\n", a, v);
	asm volatile("sw %0, 0(%1)" : : "r" (v), "r" (a));
}

volatile int g_wr_cnt;
static __inline void writeq(uint64_t v, caddr_t a)
{
	sbi_printf("wr64(%d) addr:0x%lx val:0x%llx\n", g_wr_cnt++, a, v);
	asm volatile("sd %0, 0(%1)" : : "r" (v), "r" (a));
}

static __inline uint32_t readl(const caddr_t a)
{
	uint32_t v;

	asm volatile("lw %0, 0(%1)" : "=r" (v) : "r" (a));
	return v;
}

static __inline uint64_t readq(const caddr_t a)
{
	uint64_t v;

	asm volatile("ld %0, 0(%1)" : "=r" (v) : "r" (a));
	return v;
}

#define wr64(addr, val) writeq(val, addr)
#define wr(addr, val) writel(val, addr)
#define rd64(addr) readl(addr)

typedef union {
	struct {
		uint64_t SRC_MULTBLK_TYPE  : 2;
		uint64_t DST_MULTBLK_TYPE  : 2;
		uint64_t RSVD_DMAC_CHx_CFG_4to31 : 28;
		uint64_t TT_FC : 3;
		uint64_t HS_SEL_SRC  : 1;
		uint64_t HS_SEL_DST  : 1;
		uint64_t SRC_HWHS_POL  : 1;
		uint64_t DST_HWHS_POL : 1;
		uint64_t SRC_PER  : 2;
		uint64_t RSVD_DMAC_CHx_CFG_42_39  : 2;
		uint64_t RSVD_DMAC_CHx_CFG_43  : 1;
		uint64_t DST_PER  : 2;
		uint64_t RSVD_DMAC_CHx_CFG_47_44  : 2;
		uint64_t RSVD_DMAC_CHx_CFG_48  : 1;
		uint64_t CH_PRIOR  : 3;
		uint64_t L0CK_CH  : 1;
		uint64_t LOCK_CH_L  : 2;
		uint64_t SRC_OSR_LMT  : 4;
		uint64_t DST_OSR_LMT  : 4;
		uint64_t RSVD_DMAC_CHx_CFG_63  : 1;
	}bits;
	uint64_t value;
} DW_AXI_DMAC_CH2_CFG_BLK2;

typedef union {
	struct {
		uint64_t SMS : 1;
		uint64_t RSVD_DMAC_CHx_CTL_1 : 1;
		uint64_t DMS : 1;
		uint64_t RSVD_DMAC_CHx_CTL_3 : 1;
		uint64_t SINC : 1;
		uint64_t RSVD_DMAC_CHx_CTL_5 : 1;
		uint64_t DINC : 1;
		uint64_t RSVD_DMAC_CHx_CTL_7 : 1;
		uint64_t SRC_TR_WIDTH : 3;
		uint64_t DST_TR_WIDTH : 3;
		uint64_t SRC_MSIZE : 4;
		uint64_t DST_MSIZE : 4;
		uint64_t AR_CACHE : 4;
		uint64_t AW_CACHE : 4;
		uint64_t NonPosted_LastWrite_En : 1;
		uint64_t RSVD_DMAC_CHx_CTL_31 : 1;
		uint64_t AR_PROT : 3;
		uint64_t AW_PROT : 3;
		uint64_t ARLEN_EN : 1;
		uint64_t ARLEN : 8;
		uint64_t AWLEN_EN : 1;
		uint64_t AWLEN : 8;
		uint64_t SRC_STAT_EN : 1;
		uint64_t DST_STAT_EN : 1;
		uint64_t IOC_BlkTfr : 1;
		uint64_t RSVD_DMAC_CHx_CTL_59to61 : 3;
		uint64_t SHADOWREG_OR_LLI_LAST : 1;
		uint64_t SHADOWREG_OR_LLI_VALID  : 1;
	} bits;
	uint64_t value;
} DW_AXI_DMAC_CH2_CTL_BLK2;

typedef union {
	struct {
		uint64_t BLOCK_TS : 22;
		uint64_t RSVD_DMAC_CHx_BLOCK_TSREG_63to22 : 42;
	} bits;
	uint64_t value;
} DW_AXI_DMAC_CH2_BLOCK_TS_BLK2;

static void dw_dma_irq_handler(void)
{
	int state = __raw_readl(base_addr + SYS_DMAC_BADDR + DMAC_INTSTATUSREG_BLK0_OFFSET);

	if (state & 0x02 == 0x02) {
		int ch_int_stat = __raw_readl(base_addr + SYS_DMAC_BADDR + CH2_INTSTATUS_BLK2_OFFSET);
		__raw_writel(ch_int_stat, base_addr + SYS_DMAC_BADDR + CH2_INTCLEARREG_BLK2_OFFSET);
	}

	__raw_writel(state, base_addr + SYS_DMAC_BADDR + DMAC_COMMONREG_INTCLEARREG_BLK0_OFFSET);

	if((state & 0x2)==0x2){
		ch2_done = 0x1;
	}
}

static void dma_trans_ch(uint64_t src_addr, uint64_t dst_addr, int trans_len,int ch)
{
	DW_AXI_DMAC_CH2_CFG_BLK2 ch_cfg;
	DW_AXI_DMAC_CH2_CTL_BLK2 ch_ctl;
	DW_AXI_DMAC_CH2_BLOCK_TS_BLK2 blk_ts;
	uint64_t offset;
	printf("ch%d:src_addr:%llx,dst_addr:%llx\n",ch, src_addr, dst_addr);
	if(ch == 1)
		offset = CH1_CTL_BLK1_OFFSET;
	else if(ch == 2)
		offset = CH2_CTL_BLK2_OFFSET;

	ch_ctl.value = rd64(base_addr + SYS_DMAC_BADDR + offset);
	ch_ctl.bits.AWLEN = 16;
	ch_ctl.bits.AWLEN_EN = 1;
	ch_ctl.bits.ARLEN = 16;
	ch_ctl.bits.ARLEN_EN = 1;
	ch_ctl.bits.AR_PROT = 0x0;
	ch_ctl.bits.AW_PROT = 0x0;
	ch_ctl.bits.SRC_TR_WIDTH = 0x4;//axsize
	ch_ctl.bits.DST_TR_WIDTH = 0x4;
	ch_ctl.bits.SRC_STAT_EN = 0x1;
	ch_ctl.bits.DST_STAT_EN = 0x1;
	ch_ctl.bits.SRC_MSIZE = 0x3;//16
	ch_ctl.bits.DST_MSIZE = 0x3;
	ch_ctl.bits.DMS = 0x0;
	ch_ctl.bits.SMS = 0x0;
	wr64(base_addr + SYS_DMAC_BADDR + offset, ch_ctl.value);
	
	if(ch == 1)
		offset = CH1_CFG_BLK1_OFFSET;
	else if(ch == 2)
		offset = CH2_CFG_BLK2_OFFSET;
	
	ch_cfg.value = rd64(base_addr + SYS_DMAC_BADDR +offset);
	ch_cfg.bits.TT_FC = 0;
	ch_cfg.bits.HS_SEL_SRC = 0;
	ch_cfg.bits.HS_SEL_DST = 0;
	ch_cfg.bits.SRC_HWHS_POL = 0;
	ch_cfg.bits.DST_HWHS_POL = 0;
	ch_cfg.bits.SRC_OSR_LMT = 0x4;
	ch_cfg.bits.DST_OSR_LMT = 0x4;
	ch_cfg.bits.CH_PRIOR = 0x7;
	wr64(base_addr+ SYS_DMAC_BADDR + offset, ch_cfg.value);
	if(ch == 1)
		offset = CH1_BLOCK_TS_BLK1_OFFSET;
	else if(ch == 2)
		offset = CH2_BLOCK_TS_BLK2_OFFSET;

	blk_ts.value = rd64(base_addr + SYS_DMAC_BADDR + offset);
	blk_ts.bits.BLOCK_TS = trans_len;
	// wr64(base_addr + SYS_DMAC_BADDR + offset, blk_ts.value);
	wr64(base_addr + SYS_DMAC_BADDR + offset, 0x100);

	//Config DSP DMA, CHANNEL2--------- DRAM --> Shared RAM
	printf("Configure DMA CH%d\n",ch);

	if (ch == 1){ 
		wr(base_addr + SYS_DMAC_BADDR + CH1_SAR_BLK1_OFFSET, src_addr);
		wr(base_addr + SYS_DMAC_BADDR + CH1_SAR_BLK1_OFFSET +0x4, src_addr >> 32);
		wr(base_addr + SYS_DMAC_BADDR + CH1_DAR_BLK1_OFFSET, dst_addr);
		wr(base_addr + SYS_DMAC_BADDR + CH1_DAR_BLK1_OFFSET +0x4, dst_addr >> 32);
	} else if (ch == 2) {
		wr(base_addr + SYS_DMAC_BADDR + CH2_SAR_BLK2_OFFSET, src_addr);
		wr(base_addr + SYS_DMAC_BADDR + CH2_SAR_BLK2_OFFSET +0x4, src_addr >> 32);
		wr(base_addr + SYS_DMAC_BADDR + CH2_DAR_BLK2_OFFSET, dst_addr);
		wr(base_addr + SYS_DMAC_BADDR + CH2_DAR_BLK2_OFFSET +0x4, dst_addr >> 32);		
	}
}

static void dma_test(void)
{
	int i , j;
	uint64_t dst_base_addr, src_addr, dst_addr;

	sbi_printf("1111111111111\n");
	dma_master_init();
	sbi_printf("222222222222\n");

	memset(src_buf, 0xa5, BUF_LEN);
	memset(dst_buf, 0x00, BUF_LEN);

	src_addr = src_buf;
	dst_addr = dst_buf;

	irqc_configure_irq(26, 0, IRQ_LEVEL_TRIGGERED);
	irq_register_vector(26, dw_dma_irq_handler);
	irqc_enable_irq(26);

	dma_trans_ch(src_addr, dst_addr, BUF_LEN, 0x02);

	// __raw_writel(0x202, base_addr + SYS_DMAC_BADDR + DMAC_COMMONREG_INTCLEARREG_BLK0_OFFSET);
	// __raw_writel(0x3, base_addr + SYS_DMAC_BADDR+DMAC_CFGREG_BLK0_OFFSET); // [0]:dma_en [1]:int_en

	// __raw_writel(0x202, DMAC_CHENREG_BLK0_0FFSET);

	//Enable DMA0 and block interrupt at LAST!!!
	wr(base_addr + SYS_DMAC_BADDR + DMAC_COMMONREG_INTSTATUS_ENABLEREG_BLK0_OFFSET, 0x0202);
	wr(base_addr + SYS_DMAC_BADDR + DMAC_CFGREG_BLK0_OFFSET, 0x3);//[1]:int_en [0]:dma_en
	//enable dma ch2
	wr(base_addr + SYS_DMAC_BADDR + DMAC_CHENREG_BLK0_OFFSET,0x000000202);

	while(!ch2_done) ;
	sbi_printf("!!!!!!!!!!");
}

void dw_dma_init(void)
{
	dma_test();
}

static int do_dmatest(int argc, char *argv[])
{
	dw_dma_init();
}

DEFINE_COMMAND(dmatest, do_dmatest, "K1Matrix SoC global commands",
	"dmatest\n"
	"    -simulation/boot mode information\n"
);
