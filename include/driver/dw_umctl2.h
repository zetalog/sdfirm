/*
 * ZETALOG's Personal COPYRIGHT
 *
 * Copyright (c) 2022
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
 * @(#)dw_umctl2.h: Synopsys DesignWare DDR uMCTL2 interface
 * $Id: dw_umctl2.h,v 1.0 2022-04-06 09:26:00 zhenglv Exp $
 */

#ifndef __DW_UMCTL2_H_INCLUDE__
#define __DW_UMCTL2_H_INCLUDE__

#include <target/arch.h>

#ifndef DW_UMCTL2_BASE
#define DW_UMCTL2_BASE(n)		(DW_UMCTL2##n##_BASE)
#endif
#ifndef DW_UMCTL2_REG
#define DW_UMCTL2_REG(n, offset)	(DW_UMCTL2_BASE(n) + (offset))
#endif
#ifndef NR_DW_UMCTL2S
#define NR_DW_UMCTL2S		1
#endif

#ifdef CONFIG_DW_UMCTL2_CID_WIDTH_0
#define DW_UMCTL2_CID_WIDTH		0
#endif
#ifdef CONFIG_DW_UMCTL2_CID_WIDTH_1
#define DW_UMCTL2_CID_WIDTH		1
#endif
#ifdef CONFIG_DW_UMCTL2_CID_WIDTH_2
#define DW_UMCTL2_CID_WIDTH		2
#endif

#ifdef CONFIG_DW_UMCTL2_RANK_1
#define DW_UMCTL2_NUM_RANKS		1
#endif
#ifdef CONFIG_DW_UMCTL2_RANK_2
#define DW_UMCTL2_NUM_RANKS		2
#endif
#ifdef CONFIG_DW_UMCTL2_RANK_4
#define DW_UMCTL2_NUM_RANKS		4
#endif
#define DW_UMCTL2_NUM_RANKS_MASK(nr)	(_BV(nr) - 1)
#define DW_UMCTL2_RANKS_MASK		\
	DW_UMCTL2_NUM_RANKS_MASK(DW_UMCTL2_NUM_RANKS)

#ifdef CONFIG_DW_UMCTL2_BURST_LENGTH_2
#define DW_UMCTL2_BURST_LENGTH		2
#endif
#ifdef CONFIG_DW_UMCTL2_BURST_LENGTH_4
#define DW_UMCTL2_BURST_LENGTH		4
#endif
#ifdef CONFIG_DW_UMCTL2_BURST_LENGTH_8
#define DW_UMCTL2_BURST_LENGTH		8
#endif
#ifdef CONFIG_DW_UMCTL2_BURST_LENGTH_16
#define DW_UMCTL2_BURST_LENGTH		16
#endif

#ifdef CONFIG_ARCH_IS_DW_UMCTL_FREQ_1
#define DW_UMCTL2_NUM_FREQS		1
#endif
#ifdef CONFIG_ARCH_IS_DW_UMCTL_FREQ_2
#define DW_UMCTL2_NUM_FREQS		2
#endif
#ifdef CONFIG_ARCH_IS_DW_UMCTL_FREQ_3
#define DW_UMCTL2_NUM_FREQS		3
#endif
#ifdef CONFIG_ARCH_IS_DW_UMCTL_FREQ_4
#define DW_UMCTL2_NUM_FREQS		4
#endif

/* MEMC_PAGE_BITS */
#ifdef CONFIG_DW_UMCTL2_DDR4
#define DW_UMCTL2_PAGE_BITS		18
#elif defined(CONFIG_DW_UMCTL2_LPDDR4)
#define DW_UMCTL2_PAGE_BITS		17
#else
#define DW_UMCTL2_PAGE_BITS		16
#endif

#define DW_UMCTL2_ROUNDUP(value, unit_shift)	\
	(round_up((value), 1 << (unit_shift)) >> (unit_shift))
#define DW_UMCTL2_ROUNDDOWN(value, unit_shift)	\
	(round_down((value), 1 << (unit_shift)) >> (unit_shift))

/* uMCTL2 DDRC Registers UMCTL2_REGS */
/* uMCTL2 DDRC DCH1 Registers UMCTL2_REGS_DCH1 (UMCTL2_CREG)
 * if UMCTL2_DUAL_DATA_CHANNEL=1
 */
#define DW_UMCTL2_CREG(n, c, offset)	\
	DW_UMCTL2_REG((n), (offset) + (0x1B00 * (n)))
/* uMCTL2 DDRC FREQ1 Registers UMCTL2_REGS_FREQ1 (UMCTL2_FREG)
 * if UMCTL2_FAST_FREQUENCY_CHANGE==1
 * uMCTL2 DDRC FERQ2 Registers UMCTL2_REGS_FREQ2 (UMCTL2_FREG)
 * if UMCTL2_FREQUENCY_NUM>2
 * uMCTL2 DDRC FREQ3 Registers UMCTL2_REGS_FREQ3 (UMCTL2_FREG)
 * if UMCTL2_FREQUENCY_NUM>3
 */
#define DW_UMCTL2_FREG(n, f, offset)	\
	DW_UMCTL2_REG((n), (offset) + (0x2000 * (f)))
/* uMCTL2 DDRC Alternative Address Map Registers UMCTL2_REGS_ADDRMAP_ALT
 * (UMCTL2_AREG)
 * if UMCTL2_HET_RANK==1
 */
#define UMCTL2_AREG(n, a, offset)	\
	DW_UMCTL2_REG((n), (offset) + (0x21F8 * (a)))

#define UMCTL2_MSTR(n)			DW_UMCTL2_REG(n, 0x000)
#define UMCTL2_STAT(n, c)		DW_UMCTL2_CREG(n, c, 0x004)
#define UMCTL2_MRCTRL0(n, c)		DW_UMCTL2_CREG(n, c, 0x010)
#define UMCTL2_MRCTRL1(n, c)		DW_UMCTL2_CREG(n, c, 0x014)
#define UMCTL2_MRSTAT(n, c)		DW_UMCTL2_CREG(n, c, 0x018)
#define UMCTL2_MSTR2(n)			DW_UMCTL2_REG(n, 0x028)
#define UMCTL2_PWRCTL(n, c)		DW_UMCTL2_CREG(n, c, 0x030)
#define UMCTL2_PWRTMG(n, f)		DW_UMCTL2_FREG(n, f, 0x034)
#define UMCTL2_RFSHCTL0(n, f)		DW_UMCTL2_FREG(n, f, 0x050)
#if DW_UMCTL2_NUM_RANKS > 1
#define UMCTL2_RFSHCTL1(n)		DW_UMCTL2_REG(n, 0x054)
#define UMCTL2_RFSHCTL2(n)		DW_UMCTL2_REG(n, 0x058)
#define UMCTL2_RFSHCTL12(n, r)		\
	REG_16BIT_ADDR(DW_UMCTL2_REG(n, 0x54), r)
#endif /* DW_UMCTL2_NUM_RANKS > 1 */
#define UMCTL2_RFSHCTL4(n)		DW_UMCTL2_REG(n, 0x05C)
#define UMCTL2_RFSHCTL3(n)		DW_UMCTL2_REG(n, 0x060)
#define UMCTL2_RFSHTMG(n, f)		DW_UMCTL2_FREG(n, f, 0x064)
#define UMCTL2_CRCPARCTL0(n, c)		DW_UMCTL2_CREG(n, c, 0x0C0)
#define UMCTL2_CRCPARCTL1(n)		DW_UMCTL2_REG(n, 0x0C4)
#define UMCTL2_CRCPARSTAT(n, c)		DW_UMCTL2_CREG(n, c, 0x0CC)
#define UMCTL2_INIT0(n)			DW_UMCTL2_REG(n, 0x0D0)
#define UMCTL2_INIT1(n)			DW_UMCTL2_REG(n, 0x0D4)
#define UMCTL2_INIT3(n, f)		DW_UMCTL2_FREG(n, f, 0x0DC)
#define UMCTL2_INIT4(n, f)		DW_UMCTL2_FREG(n, f, 0x0E0)
#define UMCTL2_DIMMCTL(n)		DW_UMCTL2_REG(n, 0x0F0)
#define UMCTL2_RANKCTL(n, f)		DW_UMCTL2_FREG(n, f, 0x0F4)
#define UMCTL2_CHCTL(n)			DW_UMCTL2_REG(n, 0x0FC)
#define UMCTL2_DRAMTMG0(n, f)		DW_UMCTL2_FREG(n, f, 0x100)
#define UMCTL2_DRAMTMG1(n, f)		DW_UMCTL2_FREG(n, f, 0x104)
#define UMCTL2_DRAMTMG2(n, f)		DW_UMCTL2_FREG(n, f, 0x108)
#define UMCTL2_DRAMTMG3(n, f)		DW_UMCTL2_FREG(n, f, 0x10C)
#define UMCTL2_DRAMTMG4(n, f)		DW_UMCTL2_FREG(n, f, 0x110)
#define UMCTL2_DRAMTMG5(n, f)		DW_UMCTL2_FREG(n, f, 0x114)
#define UMCTL2_DRAMTMG8(n, f)		DW_UMCTL2_FREG(n, f, 0x120)
#define UMCTL2_DFITMG0(n, f)		DW_UMCTL2_FREG(n, f, 0x190)
#define UMCTL2_DFITMG1(n, f)		DW_UMCTL2_FREG(n, f, 0x194)
#define UMCTL2_DFILPCFG0(n)		DW_UMCTL2_REG(n, 0x198)
#define UMCTL2_DFIUPD0(n)		DW_UMCTL2_REG(n, 0x1A0)
#define UMCTL2_DFIUPD1(n)		DW_UMCTL2_REG(n, 0x1A4)
#define UMCTL2_DFIUPD2(n)		DW_UMCTL2_REG(n, 0x1A8)
#define UMCTL2_DFIMISC(n)		DW_UMCTL2_REG(n, 0x1B0)
#define UMCTL2_DFISTAT(n, c)		DW_UMCTL2_CREG(n, c, 0x1BC)
#define UMCTL2_DFIPHYMSTR(n)		DW_UMCTL2_REG(n, 0x1C4)
#define UMCTL2_ADDRMAP0(n, a)		DW_UMCTL2_AREG(n, a, 0x200)
#define UMCTL2_ADDRMAP1(n, a)		DW_UMCTL2_AREG(n, a, 0x204)
#define UMCTL2_ADDRMAP2(n, a)		DW_UMCTL2_AREG(n, a, 0x208)
#define UMCTL2_ADDRMAP3(n, a)		DW_UMCTL2_AREG(n, a, 0x20C)
#define UMCTL2_ADDRMAP4(n, a)		DW_UMCTL2_AREG(n, a, 0x210)
#define UMCTL2_ADDRMAP5(n, a)		DW_UMCTL2_AREG(n, a, 0x214)
#define UMCTL2_ADDRMAP6(n, a)		DW_UMCTL2_AREG(n, a, 0x218)
#define UMCTL2_ADDRMAP9(n, a)		DW_UMCTL2_AREG(n, a, 0x224)
#define UMCTL2_ADDRMAP10(n, a)		DW_UMCTL2_AREG(n, a, 0x228)
#define UMCTL2_ADDRMAP11(n, a)		DW_UMCTL2_AREG(n, a, 0x22C)
#define UMCTL2_ODTCFG(n, f)		DW_UMCTL2_FREG(n, f, 0x240)
#define UMCTL2_ODTMAP(n, c)		DW_UMCTL2_CREG(n, c, 0x244)
#define UMCTL2_SCHED(n)			DW_UMCTL2_REG(n, 0x250)
#define UMCTL2_SCHED1(n)		DW_UMCTL2_REG(n, 0x254)
#define UMCTL2_PERFHPR1(n)		DW_UMCTL2_REG(n, 0x25C)
#define UMCTL2_PERFLPR1(n)		DW_UMCTL2_REG(n, 0x264)
#define UMCTL2_PERFWR1(n)		DW_UMCTL2_REG(n, 0x26C)
#define UMCTL2_DBG0(n)			DW_UMCTL2_REG(n, 0x300)
#define UMCTL2_DBG1(n, c)		DW_UMCTL2_CREG(n, c, 0x304)
#define UMCTL2_DBGCAM(n, c)		DW_UMCTL2_CREG(n, c, 0x308)
#define UMCTL2_DBGCMD(n, c)		DW_UMCTL2_CREG(n, c, 0x30C)
#define UMCTL2_DBGSTAT(n, c)		DW_UMCTL2_CREG(n, c, 0x310)
#define UMCTL2_SWCTL(n)			DW_UMCTL2_REG(n, 0x320)
#define UMCTL2_SWSTAT(n)		DW_UMCTL2_REG(n, 0x324)
#define UMCTL2_SWCTLSTATIC(n)		DW_UMCTL2_REG(n, 0x328)
#define UMCTL2_POISONCFG(n)		DW_UMCTL2_REG(n, 0x36C)
#define UMCTL2_POISONSTAT(n)		DW_UMCTL2_REG(n, 0x370)

/* DDR3/DDR4 specific registers */
#ifdef CONFIG_DW_UMCTL2_DDR4
#define UMCTL2_MRCTRL2(n, c)		DW_UMCTL2_CREG(n, c, 0x01C)
#define UMCTL2_DRAMTMG9(n, f)		DW_UMCTL2_FREG(n, f, 0x124)
#define UMCTL2_DRAMTMG10(n, f)		DW_UMCTL2_FREG(n, f, 0x128)
#define UMCTL2_DRAMTMG11(n, f)		DW_UMCTL2_FREG(n, f, 0x12C)
#define UMCTL2_DFILPCFG1(n)		DW_UMCTL2_REG(n, 0x19C)
#define UMCTL2_DFITMG3(n, f)		DW_UMCTL2_FREG(n, f, 0x1B8)
#if defined(CONFIG_DW_UMCTL2_CMD_RTN2IDLE) && \
    defined(CONFIG_DW_UMCTL2_FREQ_RATIO_2)
#endif /* CONFIG_DW_UMCTL2_CMD_RTN2IDLE && CONFIG_DW_UMCTL2_FREQ_RATIO_2 */
#define UMCTL2_ADDRMAP8(n, a)		DW_UMCTL2_AREG(n, a, 0x220)
#endif /* CONFIG_DW_UMCTL2_DDR4 */
#if defined(CONFIG_DW_UMCTL2_DDR3) || defined(CONFIG_DW_UMCTL2_LPDDR2)
#define UMCTL2_INIT5(n)			DW_UMCTL2_REG(n, 0x0E4)
#endif /* CONFIG_DW_UMCTL2_DDR3 || CONFIG_DW_UMCTL2_LPDDR2 */
#if defined(CONFIG_DW_UMCTL2_DDR4) || defined(CONFIG_DW_UMCTL2_LPDDR4)
#define UMCTL2_INIT6(n, f)		DW_UMCTL2_FREG(n, f, 0x0E8)
#define UMCTL2_INIT7(n, f)		DW_UMCTL2_FREG(n, f, 0x0EC)
#define UMCTL2_DRAMTMG12(n, f)		DW_UMCTL2_FREG(n, f, 0x130)
#define UMCTL2_DBICTL(n)		DW_UMCTL2_REG(n, 0x1C0)
#define UMCTL2_ADDRMAP7(n, a)		DW_UMCTL2_AREG(n, a, 0x21C)
#endif /* CONFIG_DW_UMCTL2_DDR4 || CONFIG_DW_UMCTL2_LPDDR4 */
#if defined(CONFIG_DW_UMCTL2_DDR3) || defined(CONFIG_DW_UMCTL2_DDR4)
#define UMCTL2_DRAMTMG15(n, f)		DW_UMCTL2_FREG(n, f, 0x13C)
#endif /* CONFIG_DW_UMCTL2_DDR3 || CONFIG_DW_UMCTL2_DDR4 */
#if defined(CONFIG_DW_UMCTL2_DDR3) || defined(CONFIG_DW_UMCTL2_DDR4) || \
    defined(CONFIG_DW_UMCTL2_LPDDR2)
#define UMCTL2_ZQCTL0(n, f)		DW_UMCTL2_FREG(n, f, 0x180)
#define UMCTL2_ZQCTL1(n)		DW_UMCTL2_REG(n, 0x184)
#endif /* CONFIG_DW_UMCTL2_DDR3 || CONFIG_DW_UMCTL2_DDR4 || CONFIG_DW_UMCTL2_LPDDR2 */

/* ECC registers */
#ifdef CONFIG_DW_UMCTL2_ECC
#define UMCTL2_ECCCFG0(n)		DW_UMCTL2_REG(n, 0x070)
#define UMCTL2_ECCCFG1(n)		DW_UMCTL2_REG(n, 0x074)
#define UMCTL2_ECCSTAT(n, c)		DW_UMCTL2_CREG(n, c, 0x078)
#define UMCTL2_ECCCTL(n, c)		DW_UMCTL2_CREG(n, c, 0x07C)
#define UMCTL2_ECCERRCNT(n, c)		DW_UMCTL2_CREG(n, c, 0x080)
#define UMCTL2_ECCCADDR0(n, c)		DW_UMCTL2_CREG(n, c, 0x084)
#define UMCTL2_ECCCADDR1(n, c)		DW_UMCTL2_CREG(n, c, 0x088)
#define UMCTL2_ECCCSYN0(n, c)		DW_UMCTL2_CREG(n, c, 0x08C)
#define UMCTL2_ECCCSYN1(n, c)		DW_UMCTL2_CREG(n, c, 0x090)
#define UMCTL2_ECCCSYN2(n, c)		DW_UMCTL2_CREG(n, c, 0x094)
#define UMCTL2_ECCBITMASK0(n, c)	DW_UMCTL2_CREG(n, c, 0x098)
#define UMCTL2_ECCBITMASK1(n, c)	DW_UMCTL2_CREG(n, c, 0x09C)
#define UMCTL2_ECCBITMASK2(n, c)	DW_UMCTL2_CREG(n, c, 0x0A0)
#define UMCTL2_ECCUADDR0(n, c)		DW_UMCTL2_CREG(n, c, 0x0A4)
#define UMCTL2_ECCUADDR1(n, c)		DW_UMCTL2_CREG(n, c, 0x0A8)
#define UMCTL2_ECCUSYN0(n, c)		DW_UMCTL2_CREG(n, c, 0x0AC)
#define UMCTL2_ECCUSYN1(n, c)		DW_UMCTL2_CREG(n, c, 0x0B0)
#define UMCTL2_ECCUSYN2(n, c)		DW_UMCTL2_CREG(n, c, 0x0B4)
#define UMCTL2_ECCPOISONADDR0(n)	DW_UMCTL2_REG(n, 0x0B8)
#define UMCTL2_ECCPOISONADDR1(n)	DW_UMCTL2_REG(n, 0x0BC)
#define UMCTL2_ADVECCINDEX(n)		DW_UMCTL2_REG(n, 0x374)
#define UMCTL2_ADVECCSTAT(n, c)		DW_UMCTL2_CREG(n, c, 0x378)
#define UMCTL2_ECCPOISONPAT0(n)		DW_UMCTL2_REG(n, 0x37C)
#define UMCTL2_ECCPOISONPAT1(n)		DW_UMCTL2_REG(n, 0x380)
#define UMCTL2_ECCPOISONPAT2(n)		DW_UMCTL2_REG(n, 0x384)
#define UMCTL2_ECCAPSTAT(n, c)		DW_UMCTL2_CREG(n, c, 0x388)
#ifdef CONFIG_DW_UMCTL2_INLINE_ECC
#define UMCTL2_DBGCAM1(n, c)		DW_UMCTL2_CREG(n, c, 0x318)
#endif /* CONFIG_DW_UMCTL2_INLINE_ECC */
#endif /* CONFIG_DW_UMCTL2_ECC */

/* LPDDR specific */
#if defined(CONFIG_DW_UMCTL2_MOBILE) || defined(CONFIG_DW_UMCTL2_LPDDR2)
#define UMCTL2_DRAMTMG6(n, f)		DW_UMCTL2_FREG(n, f, 0x118)
#define UMCTL2_DRAMTMG7(n, f)		DW_UMCTL2_FREG(n, f, 0x11C)
#define UMCTL2_DRAMTMG14(n, f)		DW_UMCTL2_FREG(n, f, 0x138)
#endif /* CONFIG_DW_UMCTL2_MOBILE || CONFIG_DW_UMCTL2_LPDDR2 */
#ifdef CONFIG_DW_UMCTL2_LPDDR2
#define UMCTL2_DERATEEN(n, f)		DW_UMCTL2_FREG(n, f, 0x020)
#define UMCTL2_DERATEINT(n, f)		DW_UMCTL2_FREG(n, f, 0x024)
#define UMCTL2_DERATECTL(n, c)		DW_UMCTL2_CREG(n, c, 0x02C)
#define UMCTL2_DERATESTAT(n, c)		DW_UMCTL2_CREG(n, c, 0x3F0)
#define UMCTL2_INIT2(n)			DW_UMCTL2_REG(n, 0x0D8)
#define UMCTL2_ZQCTL2(n, c)		DW_UMCTL2_CREG(n, c, 0x188)
#define UMCTL2_ZQSTAT(n, c)		DW_UMCTL2_CREG(n, c, 0x18C)
#endif /* CONFIG_DW_UMCTL2_LPDDR2 */
#ifdef CONFIG_DW_UMCTL2_LPDDR4
#define UMCTL2_DRAMTMG13(n, f)		DW_UMCTL2_FREG(n, f, 0x134)
#endif /* CONFIG_DW_UMCTL2_LPDDR4 */

/* DDR4 ST-MRAM specific */
#if defined(CONFIG_DW_UMCTL2_DDR4_MRAM) || \
    defined(CONFIG_DW_UMCTL2_HET_RANK_DDR34)
#define UMCTL2_MSTR1(n)			DW_UMCTL2_REG(n, 0x008)
#ifdef CONFIG_DW_UMCTL2_HET_RANK_DDR34
#define UMCTL2_RFSHTMG_HET(n, f)	DW_UMCTL2_FREG(n, f, 0x150)
#endif /* CONFIG_DW_UMCTL2_HET_RANK_DDR34 */
#ifdef CONFIG_DW_UMCTL2_DDR4_MRAM
#define UMCTL2_MRAMTMG0(n, f)		DW_UMCTL2_FREG(n, f, 0x170)
#define UMCTL2_MRAMTMG1(n, f)		DW_UMCTL2_FREG(n, f, 0x174)
#define UMCTL2_MRAMTMG4(n, f)		DW_UMCTL2_FREG(n, f, 0x178)
#define UMCTL2_MRAMTMG9(n, f)		DW_UMCTL2_FREG(n, f, 0x17C)
#endif /* CONFIG_DW_UMCTL2_DDR4_MRAM */
#endif /* CONFIG_DW_UMCTL2_DDR4_MRAM || CONFIG_DW_UMCTL2_HET_RANK_DDR34 */
/* DDR4 3DS specific */
#ifndef CONFIG_DW_UMCTL2_CID_WIDTH_0
#define UMCTL2_DRAMTMG16(n, f)		DW_UMCTL2_FREG(n, f, 0x140)
#define UMCTL2_RFSHTMG1(n, f)		DW_UMCTL2_FREG(n, f, 0x068)
#endif /* CONFIG_DW_UMCTL2_CID_WIDTH_0 */

/* Optional registers */
#ifdef CONFIG_DW_UMCTL2_DFI_DATA_CS
#define UMCTL2_DFITMG2(n, f)		DW_UMCTL2_FREG(n, f, 0x1B4)
#endif /* CONFIG_DW_UMCTL2_DFI_DATA_CS */
#ifdef CONFIG_DW_UMCTL2_ENH_CAM_PTR
#define UMCTL2_RANKCTL1(n, f)		DW_UMCTL2_FREG(n, f, 0x0F8)
#endif /* CONFIG_DW_UMCTL2_ENH_CAM_PTR */
#ifdef CONFIG_DW_UMCTL2_DYN_BSM
#define UMCTL2_SCHED2(n)		DW_UMCTL2_REG(n, 0x258)
#define UMCTL2_DYNBSMSTAT(n, c)		DW_UMCTL2_CREG(n, c, 0x3B0)
#endif /* CONFIG_DW_UMCTL2_DYN_BSM */
#ifdef CONFIG_DW_UMCTL2_ENH_RDWR_SWITCH
#define UMCTL2_SCHED3(n)		DW_UMCTL2_REG(n, 0x270)
#define UMCTL2_SCHED4(n)		DW_UMCTL2_REG(n, 0x274)
#ifdef CONFIG_DW_UMCTL2_INLINE_ECC
#define UMCTL2_SCHED5(n)		DW_UMCTL2_REG(n, 0x278)
#endif /* CONFIG_DW_UMCTL2_INLINE_ECC */
#endif /* CONFIG_DW_UMCTL2_ENH_RDWR_SWITCH */
#ifdef CONFIG_DW_UMCTL2_HWFFC
#define UMCTL2_DRAMTMG17(n, f)		DW_UMCTL2_FREG(n, f, 0x144)
#define UMCTL2_HWLPCTL(n, c)		DW_UMCTL2_CREG(n, c, 0x038)
#define UMCTL2_HWFFCCTL(n)		DW_UMCTL2_REG(n, 0x03C)
#define UMCTL2_HWFFCSTAT(n, c)		DW_UMCTL2_CREG(n, c, 0x040)
#define UMCTL2_HWFFCEX_RANK1(n, f)	DW_UMCTL2_FREG(n, f, 0x044)
#define UMCTL2_HWFFCEX_RANK2(n, f)	DW_UMCTL2_FREG(n, f, 0x048)
#define UMCTL2_HWFFCEX_RANK3(n, f)	DW_UMCTL2_FREG(n, f, 0x04C)
#endif /* CONFIG_DW_UMCTL2_HWFFC */
#ifdef CONFIG_DW_UMCTL2_CRC_PARITY_RETRY
#ifdef CONFIG_DW_UMCTL2_DDR4
#define UMCTL2_CAPARPOISONCTL(n, c)	DW_UMCTL2_CREG(n, c, 0x3A0)
#define UMCTL2_CAPARPOISONSTAT(n, c)	DW_UMCTL2_CREG(n, c, 0x3A4)
#endif /* CONFIG_DW_UMCTL2_DDR4 */
#define UMCTL2_CRCPARCTL2(n, f)		DW_UMCTL2_FREG(n, f, 0x0C8)
#define UMCTL2_CRCPARCTL3(n)		DW_UMCTL2_REG(n, 0x3B8)
#endif /* CONFIG_DW_UMCTL2_CRC_PARITY_RETRY */
#ifdef CONFIG_DW_UMCTL2_DQ_MAPPING
#define UMCTL2_DQMAP0(n)		DW_UMCTL2_REG(n, 0x280)
#define UMCTL2_DQMAP1(n)		DW_UMCTL2_REG(n, 0x284)
#define UMCTL2_DQMAP2(n)		DW_UMCTL2_REG(n, 0x288)
#define UMCTL2_DQMAP3(n)		DW_UMCTL2_REG(n, 0x28C)
#define UMCTL2_DQMAP4(n)		DW_UMCTL2_REG(n, 0x290)
#define UMCTL2_DQMAP5(n)		DW_UMCTL2_REG(n, 0x294)
#endif /* CONFIG_DW_UMCTL2_DQ_MAPPING */
#if defined(CONFIG_DW_UMCTL2_OCPAR) || defined(CONFIG_DW_UMCLT2_OCECC)
#define UMCTL2_OCPARCFG0(n, c)		DW_UMCTL2_CREG(n, c, 0x330)
#define UMCTL2_OCPARSTAT0(n)		DW_UMCTL2_REG(n, 0x338)
#ifdef CONFIG_DW_UMCTL2_OCPAR
#define UMCTL2_OCPARCFG1(n)		DW_UMCTL2_REG(n, 0x334)
#define UMCTL2_OCPARSTAT1(n)		DW_UMCTL2_REG(n, 0x33C)
#define UMCTL2_OCPARSTAT2(n, c)		DW_UMCTL2_CREG(n, c, 0x340)
#define UMCTL2_OCPARSTAT3(n)		DW_UMCTL2_REG(n, 0x344)
#define UMCTL2_OCPARSTAT4(n)		DW_UMCTL2_REG(n, 0x348)
#define UMCTL2_OCPARSTAT5(n)		DW_UMCTL2_REG(n, 0x34C)
#define UMCTL2_OCPARSTAT6(n)		DW_UMCTL2_REG(n, 0x350)
#define UMCTL2_OCPARSTAT7(n)		DW_UMCTL2_REG(n, 0x354)
#endif /* CONFIG_DW_UMCTL2_OCPAR */
#ifdef CONFIG_DW_UMCTL2_OCECC
#define UMCTL2_OCECCCFG0(n)		DW_UMCTL2_REG(n, 0x358)
#define UMCTL2_OCECCCFG1(n)		DW_UMCTL2_REG(n, 0x35C)
#define UMCTL2_OCECCSTAT0(n)		DW_UMCTL2_REG(n, 0x360)
#define UMCTL2_OCECCSTAT1(n)		DW_UMCTL2_REG(n, 0x364)
#define UMCTL2_OCECCSTAT2(n)		DW_UMCTL2_REG(n, 0x368)
#endif /* CONFIG_DW_UMCTL2_OCECC */
#endif /* CONFIG_DW_UMCTL2_OCPAR || CONFIG_DW_UMCLT2_OCECC */
#ifdef CONFIG_DW_UMCTL2_REGPAR
#define UMCTL2_REGPARCFG(n)		DW_UMCTL2_REG(n, 0x3C0)
#define UMCTL2_REGPARSTAT(n)		DW_UMCTL2_REG(n, 0x3C4)
#endif /* CONFIG_DW_UMCTL2_REGPAR */
#if defined(CONFIG_DW_UMCTL2_HWFFC) && defined(CONFIG_DW_UMCTL2_DDR4)
#define UMCTL2_RCDINIT1(n, f)		DW_UMCTL2_FREG(n, f, 0x3D0)
#define UMCTL2_RCDINIT2(n, f)		DW_UMCTL2_FREG(n, f, 0x3D4)
#define UMCTL2_RCDINIT3(n, f)		DW_UMCTL2_FREG(n, f, 0x3D8)
#define UMCTL2_RCDINIT4(n, f)		DW_UMCTL2_FREG(n, f, 0x3DC)
#endif /* CONFIG_DW_UMCTL2_HWFFC && CONFIG_DW_UMCTL2_DDR4 */
#ifdef CONFIG_DW_UMCTL2_OCCAP
#define UMCTL2_OCCAPCFG(n)		DW_UMCTL2_REG(n, 0x3E0)
#define UMCTL2_OCCAPSTAT(n)		DW_UMCTL2_REG(n, 0x3E4)
#define UMCTL2_OCCAPCFG1(n, c)		DW_UMCTL2_CREG(n, c, 0x3E8)
#define UMCTL2_OCCAPSTAT1(n, c)		DW_UMCTL2_CREG(n, c, 0x3EC)
#endif /* CONFIG_DW_UMCTL2_OCCAP */

/* uMCTL2 Multi-Port Registers UMCTL2_MP */
/* Port based register */
#define DW_UMCTL2_PREG(n, p, offset)		\
	DW_UMCTL2_REG((n), (offset) + 0xB0 * (p))
/* Port/Channel based register */
#define DW_UMCTL2_PCREG(n, p, c, offset)	\
	DW_UMCTL2_REG((n), (offset) + 0xB0 * (p) + 0x08 * (c))
/* SAR based register */
#define DW_UMCTL2_SREG(n, s, offset)		\
	DW_UMCTL2_REG((n), (offset) + 0x08 * (s))
#define UMCTL2_PSTAT(n)			DW_UMCTL2_REG(n, 0x3FC)
#define UMCTL2_PCCFG(n)			DW_UMCTL2_REG(n, 0x400)
#define UMCTL2_PCFGR(n, p)		DW_UMCTL2_PREG(n, p, 0x404)
#define UMCTL2_PCFGW(n, p)		DW_UMCTL2_PREG(n, p, 0x408)
#define UMCTL2_PCFGC(n, p)		DW_UMCTL2_PREG(n, p, 0x40C)
#define UMCTL2_PCFGIDMASKCH(n, p, c)	DW_UMCTL2_PCREG(n, p, c, 0x410)
#define UMCTL2_PCFGIDVALUECH(n, p, c)	DW_UMCTL2_PCREG(n, p, c, 0x414)
#define UMCTL2_PCTRL(n, p)		DW_UMCTL2_PREG(n, p, 0x490)
#define UMCTL2_PCFGQOS0(n, p)		DW_UMCTL2_PREG(n, p, 0x494)
#define UMCTL2_PCFGQOS1(n, p)		DW_UMCTL2_PREG(n, p, 0x498)
#define UMCTL2_PCFGWQOS0(n, p)		DW_UMCTL2_PREG(n, p, 0x49C)
#define UMCTL2_PCFGWQOS1(n, p)		DW_UMCTL2_PREG(n, p, 0x4A0)
#define UMCTL2_SARBASE(n, s)		DW_UMCTL2_SREG(n, s, 0xF04)
#define UMCTL2_SARSIZE(n, s)		DW_UMCTL2_SREG(n, s, 0xF08)
#define UMCTL2_SBRCTL(n)		DW_UMCTL2_REG(n, 0xF24)
#define UMCTL2_SBRSTAT(n)		DW_UMCTL2_REG(n, 0xF28)
#define UMCTL2_SBRWDATA0(n)		DW_UMCTL2_REG(n, 0xF2C)
#define UMCTL2_SBRWDATA1(n)		DW_UMCTL2_REG(n, 0xF30)
#define UMCTL2_PDCH(n)			DW_UMCTL2_REG(n, 0xF34)
#define UMCTL2_SBRSTART0(n)		DW_UMCTL2_REG(n, 0xF38)
#define UMCTL2_SBRSTART1(n)		DW_UMCTL2_REG(n, 0xF3C)
#define UMCTL2_SBRRANGE0(n)		DW_UMCTL2_REG(n, 0xF40)
#define UMCTL2_SBRRANGE1(n)		DW_UMCTL2_REG(n, 0xF44)
#define UMCTL2_SBRSTART0DCH1(n)		DW_UMCTL2_REG(n, 0xF48)
#define UMCTL2_SBRSTART1DCH1(n)		DW_UMCTL2_REG(n, 0xF4C)
#define UMCTL2_SBRRANGE0DCH1(n)		DW_UMCTL2_REG(n, 0xF50)
#define UMCTL2_SBRRANGE1DCH1(n)		DW_UMCTL2_REG(n, 0xF54)
#define UMCTL2_VER_NUMBER(n)		DW_UMCTL2_REG(n, 0xFF0)
#define UMCTL2_VER_TYPE(n)		DW_UMCTL2_REG(n, 0xFF4)

/* Programming Mode:
 * - Static: Can be written only when the controller is in reset.
 * - Dynamic: Can be written at any time during operation.
 * - Quasi Dynamic: Can be written when the controller is in reset and
 *   some specific conditions outside reset. There are four groups this
 *   type.
 *   Group 1: Registers that can be Written when No Read/Write Traffic is
 *            Present at the DFI
 *   Group 2: Registers that can be Written in Self-refresh, DPD and MPSM
 *            Modes
 *   Group 3: Registers that can be Written When Controller is Empty
 *   Group 4: Registers that can be Written Depending on
 *            MSTR.frequency_mode and MSTR2.target_frequency
 */
/* Static */
/* 5.1 UMCTL2_REGS Registers */
/* 5.1.1 MSTR */
#ifdef CONFIG_DW_UMCTL2_DDR4
#define MSTR_device_config_OFFSET	30
#define MSTR_device_config_MASK		REG_2BIT_MASK
#define MSTR_device_config(value)	_SET_FV(MSTR_device_config, value)
#define MSTR_device_x4			0
#define MSTR_device_x8			1
#define MSTR_device_x16			2
#define MSTR_device_x32			3
#endif
#define MSTR_active_ranks_OFFSET	24
#define MSTR_active_ranks_MASK		REG_5BIT_MASK
#define MSTR_active_ranks(value)	\
	_SET_FV(MSTR_active_ranks, _BV(value) - 1)
#ifdef CONFIG_DW_UMCTL2_FREQ_RATIO_SW
#define MSTR_frequency_ratio_OFFSET	22
#define MSTR_frequency_ratio_MASK	REG_1BIT_MASK
#define MSTR_frequency_ratio(value)	_SET_FV(MSTR_frequency_ratio, value)
#else
#define MSTR_frequency_ratio(value)	0
#endif
#define MSTR_frequency_ratio_1		1
#define MSTR_frequency_ratio_2		0
#ifdef CONFIG_DW_UMCTL2_FREQ_RATIO_HW
#ifdef CONFIG_DW_UMCTL2_FREQ_RATIO_HW_1
#define MSTR_frequency_ratio_static	\
	MSTR_frequency_ratio(MSTR_frequency_ratio_1)
#endif
#ifdef CONFIG_DW_UMCTL2_FREQ_RATIO_HW_2
#define MSTR_frequency_ratio_static	\
	MSTR_frequency_ratio(MSTR_frequency_ratio_2)
#endif
#else
#define MSTR_frequency_ratio_static	\
	MSTR_frequency_ratio(MSTR_frequency_ratio_2)
#endif
#ifndef CONFIG_DW_UMCTL2_CID_WIDTH_0
#define MSTR_active_logical_ranks_OFFSET	20
#define MSTR_active_logical_ranks_MASK		REG_2BIT_MASK
#define MSTR_active_logical_ranks(value)	\
	_SET_FV(MSTR_active_logical_ranks, value)
#define MSTR_active_logical_ranks_static	\
	MSTR_active_logical_ranks(DW_UMCTL2_CID_WIDTH)
#else
#define MSTR_active_logical_ranks_static	0
#endif
#define MSTR_burst_rdwr_OFFSET		16
#define MSTR_burst_rdwr_MASK		REG_4BIT_MASK
#define MSTR_burst_rdwr(value)		_SET_FV(MSTR_burst_rdwr, value)
#define MSTR_burst_rdwr_len(v)		((v) >> 1)
#define MSTR_burst_rdwr_static		\
	MSTR_burst_rdwr(MSTR_burst_rdwr_len(DW_UMCTL2_BURST_LENGTH))
#define MSTR_data_bus_width_OFFSET	12
#define MSTR_data_bus_width_MASK	REG_2BIT_MASK
#define MSTR_data_bus_width(value)	_SET_FV(MSTR_data_bus_width, value)
#define MSTR_data_bus_full_DQ		0
#define MSTR_data_bus_half_DQ		1
#define MSTR_data_bus_quarter_DQ	2
#ifdef CONFIG_DW_UMCTL2_QUARTER_DQ
#define MSTR_data_bus_width_static	\
	MSTR_data_bus_width(MSTR_data_bus_quarter_DQ)
#elif defined CONFIG_DW_UMCTL2_HALF_DQ
#define MSTR_data_bus_width_static	\
	MSTR_data_bus_width(MSTR_data_bus_half_DQ)
#else
#define MSTR_data_bus_width_static	\
	MSTR_data_bus_width(MSTR_data_bus_full_DQ)
#endif
#ifdef CONFIG_DW_UMCTL2_2T_TIMING
#define MSTR_en_2t_timing_mode		_BV(10)
#define MSTR_en_2t_timing_mode_static	MSTR_en_2t_timing_mode
#else
#define MSTR_en_2t_timing_mode_static	0
#endif
#if defined(CONFIG_DW_UMCTL2_DDR3) || defined(CONFIG_DW_UMCTL2_DDR4)
#define MSTR_burstchop			_BV(9)
#if defined(CONFIG_DW_UMCTL2_BURST_LENGTH_8) || \
    defined(CONFIG_DW_UMCTL2_BURST_LENGTH_4)
#define MSTR_burstchop_static		MSTR_burstchop
#else
#define MSTR_burstchop_static		0
#endif
#else
#define MSTR_burstchop_static		0
#endif
#define MSTR_burst_mode_OFFSET		8
#define MSTR_burst_mode_MASK		REG_1BIT_MASK
#define MSTR_burst_mode(value)		_SET_FV(MSTR_burst_mode, value)
#define MSTR_burst_mode_sequential	0
#if !defined(CONFIG_DW_UMCTL2_LPDDR3) && !defined(CONFIG_DW_UMCTL2_LPDDR4)
/* For LPDDr3 and LPDDR4, this must be set to 0 */
#define MSTR_burst_mode_interleaved	1
#endif
#define MSTR_burst_mode_static		\
	MSTR_burst_mode(MSTR_burst_mode_sequential)
#define MSTR_protocols_OFFSET		0
#define MSTR_protocols_MASK		REG_6BIT_MASK
#define MSTR_protocols(value)		_SET_FV(MSTR_protocols, value)
#ifdef CONFIG_DW_UMCTL2_LPDDR4
#define MSTR_lpddr4			_BV(5)
#else
#define MSTR_lpddr4			0
#endif
#ifdef CONFIG_DW_UMCTL2_DDR4
#define MSTR_ddr4			_BV(4)
#else
#define MSTR_ddr4			0
#endif
#ifdef CONFIG_DW_UMCTL2_LPDDR3
#define MSTR_lpddr3			_BV(3)
#else
#define MSTR_lpddr3			0
#endif
#ifdef CONFIG_DW_UMCTL2_LPDDR2
#define MSTR_lpddr2			_BV(2)
#else
#define MSTR_lpddr2			0
#endif
#ifdef CONFIG_DW_UMCTL2_MOBILE
#define MSTR_mobile			_BV(1)
#else
#define MSTR_mobile			0
#endif
#ifdef CONFIG_DW_UMCTL2_DDR3
#define MSTR_ddr3			_BV(0)
#else
#define MSTR_ddr3			0
#endif
#define MSTR_lpddr_protocols		\
	(MSTR_mobile | MSTR_lpddr2 | MSTR_lpddr3 | MSTR_lpddr4)
#define MSTR_ddr_protocols		\
	(MSTR_ddr3 | MSTR_ddr4 | MSTR_lpddr_protocols)

/* 5.1.2 STAT */
#define STAT_selfref_cam_not_empty	_BV(12)
#define STAT_selfref_state_OFFSET	8
#define STAT_selfref_state_MASK		REG_2BIT_MASK
#define STAT_selfref_state(value)	_GET_FV(STAT_selfref_state, value)
#define STAT_selfref_type_OFFSET	4
#define STAT_selfref_type_MASK		REG_2BIT_MASK
#define STAT_selfref_type(value)	_GET_FV(STAT_selfref_type, value)
#define STAT_operating_mode_OFFSET	0
#define STAT_operating_mode_MASK	REG_5BIT_MASK
#define STAT_operating_mode(value)	_GET_FV(STAT_operating_mode, value)
#define STAT_operating_mode_Init		0
#define STAT_operating_mode_Normal		1
#define STAT_operating_mode_Power_down		2
#define STAT_operating_mode_Self_refresh	3

/* 5.1.3 MSTR1 */
#ifdef CONFIG_DW_UMCTL2_HET_RANK_DDR34
#define MSTR1_alt_addrmap_en		_BV(16)
#define MSTR1_rfc_tmgreg_sel_OFFSET	8
#define MSTR1_rfc_tmgreg_sel_MASK	DW_UMCTL2_RANKS_MASK
#define MSTR1_rfc_tmgreg_sel(value)	_SET_FV(MSTR1_rfc_tmgreg_sel, value)
#define MSTR1_rfc_tmgreg_sel_static	\
	MSTR1_rfc_tmgreg_sel(DW_UMCTL2_RANKS_MASK)
#endif
#ifdef CONFIG_DW_UMCTL2_DDR4_MRAM
#define MSTR1_rank_tmgreg_sel_OFFSET	0
#define MSTR1_rank_tmgreg_sel_MASK	DW_UMCTL2_RANKS_MASK
#define MSTR1_rank_tmgreg_sel(value)	_SET_FV(MSTR1_rank_tmgreg_sel, value)
#define MSTR1_rank_tmgreg_sel_static	\
	MSTR1_rank_tmgreg_sel(DW_UMCTL2_RANKS_MASK)
#endif

/* 5.1.4 MRCTRL0 */
#ifndef CONFIG_DW_UMCTL2_CID_WIDTH_0
#define MRCTRL0_mr_cid_OFFSET		16
#define MRCTRL0_mr_cid_MASK		(_BV(DW_UMCTL2_CID_WIDTH) - 1)
#define MRCTRL0_mr_cid(value)		_SET_FV(MRCTRL0_mr_cid, value)
#endif /* CONFIG_DW_UMCTL2_CID_WIDTH_0 */

/* 5.1.14 HWLPCTL */
#define HWLPCTL_hw_lp_exit_idle_en	_BV(1)
#define HWLPCTL_hw_lp_idle_x32_OFFSET	16
#define HWLPCTL_hw_lp_idle_x32_MASK	_SET_FV(HWLPCTL_hw_lp_idle_x32, value)

/* 5.1.20 RFSHCTL0 */
#ifdef CONFIG_DW_UMCTL2_LPDDR2
#define RFSHCTL0_per_bank_refresh		_BV(2)
#define RFSHCTL0_per_bank_refresh_static	RFSHCTL0_per_bank_refresh
#endif

/* 5.1.23 RFSHCTL4 */
#ifndef CONFIG_DW_UMCTL2_CID_WIDTH_0
#define RFSHCTL4_refresh_timer_lr_offset_x32_OFFSET	0
#define RFSHCTL4_refresh_timer_lr_offset_x32_MASK	REG_11BIT_MASK
#define RFSHCTL4_refresh_timer_lr_offset_x32(value)	\
	_SET_FV(RFSHCTL4_refresh_timer_lr_offset_x32,	\
		DW_UMCTL2_ROUNDUP(value, 5))
#define RFSHCTL4_refresh_timer_lr_offset_x32_static	\
	RFSHCTL4_refresh_timer_lr_offset_x32(0)
#endif

/* 5.1.24 RFSHCTL3 */
#ifdef CONFIG_DW_UMCTL2_DDR4_MRAM
#define RFSHCTL3_rank_dis_refresh_OFFSET	16
#define RFSHCTL3_rank_dis_refresh_MASK		DW_UMCTL2_RANKS_MASK
#define RFSHCTL3_rank_dis_refresh(value)	\
	_SET_FV(RFSHCTL3_rank_dis_refresh, value)
#endif

/* 5.1.25 RFSHTMG */
#ifdef CONFIG_DW_UMCTL2_LPDDR3
#define RFSHTMG_lpddr3_trefbw_en		_BV(15)
#endif

/* 5.1.51 INIT0 */
#define INIT0_post_cke_x1024_OFFSET	16
#define INIT0_post_cke_x1024_MASK	REG_10BIT_MASK
#define INIT0_post_cke_x1024(value)	\
	_SET_FV(INIT0_post_cke_x1024, DW_UMCTL2_ROUNDUP(value, 10))
#define INIT0_post_cke_x1024_mask	\
	INIT0_post_cke_x1024(INIT0_post_cke_x1024_MASK)
#define INIT0_pre_cke_x1024_OFFSET	0
#define INIT0_pre_cke_x1024_MASK	REG_12BIT_MASK
#define INIT0_pre_cke_x1024(value)	\
	_SET_FV(INIT0_pre_cke_x1024, DW_UMCTL2_ROUNDUP(value, 10))
#define INIT0_pre_cke_x1024_mask	\
	INIT0_pre_cke_x1024(INIT0_pre_cke_x1024_MASK)

/* 5.1.52 INIT1 */
#define INIT1_dram_rstn_x1024_OFFSET	16
#define INIT1_dram_rstn_x1024_MASK	REG_9BIT_MASK
#define INIT1_dram_rstn_x1024(value)	\
	_SET_FV(INIT1_dram_rstn_x1024, DW_UMCTL2_ROUNDUP(value, 10))
#define INIT1_dram_rstn_x1024_mask	\
	INIT1_dram_rstn_x1024(INIT1_dram_rstn_x1024_MASK)
#define INIT1_pre_ocd_x32_OFFSET	0
#define INIT1_pre_ocd_x32_MASK		REG_4BIT_MASK
#define INIT1_pre_ocd_x32(value)	\
	_SET_FV(INIT1_pre_ocd_x32, DW_UMCTL2_ROUNDUP(value, 5))

/* 5.1.53 INIT2 */
#ifdef CONFIG_DW_UMCTL2_LPDDR2
#define INIT2_idle_after_reset_x32_OFFSET	8
#define INIT2_idle_after_reset_x32_MASK		REG_8BIT_MASK
#define INIT2_idle_after_reset_x32(value)	\
	_SET_FV(INIT2_idle_after_reset_x32, DW_UMCTL2_ROUNDUP(value, 5))
#define INIT2_min_stable_clock_x1_OFFSET	0
#define INIT2_min_stable_clock_x1_MASK		REG_4BIT_MASK
#define INIT2_min_stable_clock_x1(value)	\
	_SET_FV(INIT2_min_stable_clock_x1, value)
#endif

/* 5.1.56 INIT5 */
#if defined(CONFIG_DW_UMCTL2_DDR3) || defined(CONFIG_DW_UMCTL2_DDR4) ||	\
    defined(CONFIG_DW_UMCTL2_LPDDR2)
#define INIT5_dev_zqinit_x32_OFFSET	16
#define INIT5_dev_zqinit_x32_MASK	REG_8BIT_MASK
#define INIT5_dev_zqinit_x32(value)	\
	_SET_FV(INIT5_dev_zqinit_x32, DW_UMCTL2_ROUNDUP(value, 5))
#endif
#ifdef CONFIG_DW_UMCTL2_LPDDR
#define INIT5_max_auto_init_x1024_OFFSET	0
#define INIT5_max_auto_init_x1024_MASK		REG_10BIT_MASK
#define INIT5_max_auto_init_x1024(value)	\
	_SET_FV(INIT5_max_auto_init_x1024, DW_UMCTL2_ROUNDUP(value, 10))
#endif

/* 5.1.133 DBG1 */
#define DBG1_dis_hif			_BV(1)
#define DBG1_dis_dq			_BV(0)

/* 5.1.134 DBGCAM */
#define DBGCAM_dbg_stall_rd		_BV(31)
#define DBGCAM_dbg_stall_wr		_BV(30)
#define DBGCAM_wr_data_pipeline_empty	_BV(29)
#define DBGCAM_rd_data_pipeline_empty	_BV(28)
#define DBGCAM_dbg_wr_q_empty		_BV(26)
#define DBGCAM_dbg_rd_q_empty		_BV(25)
#define DBGCAM_dbg_stall		_BV(24)

/* Dynamic */
/* 5.1.4 MRCTRL0 */
#define MRCTRL0_mr_wr			_BV(31)
#ifdef CONFIG_DW_UMCTL2_DDR4
#define MRCTRL0_pda_mode_OFFSET		30
#define MRCTRL0_pda_mode_MASK		REG_1BIT_MASK
#define MRCTRL0_pda_mode(value)		_SET_FV(MRCTRL0_pda_mode. value)
#define MRCTRL0_pda_Per_DRAM		0
#define MRCTRL0_pda_Per_Buffer		1
#endif
#define MRCTRL0_mr_addr_OFFSET		12
#define MRCTRL0_mr_addr_MASK		REG_4BIT_MASK
#define MRCTRL0_mr_addr(value)		_SET_FV(MRCTRL0_mr_addr, value)
#define MRCTRL0_mr_rank_OFFSET		4
#define MRCTRL0_mr_rank_MASK		DW_UMCTL2_RANKS_MASK
#define MRCTRL0_mr_rank(value)		_SET_FV(MRCTRL0_mr_rank, value)
#if defined(CONFIG_DW_UMCTL2_DDR4) || defined(CONFIG_DW_UMCTL2_LPDDR4)
#define MRCTRL0_sw_init_int		_BV(3)
#endif
#ifdef CONFIG_DW_UMCTL2_DDR4
#define MRCTRL0_pda_en			_BV(2)
#define MRCTRL0_mpr_en			_BV(1)
#endif
#if defined(CONFIG_DW_UMCTL2_DDR4) || defined(CONFIG_DW_UMCTL2_LPDDR2)
#define MRCTRL0_mr_type_OFFSET		0
#define MRCTRL0_mr_type_MASK		REG_1BIT_MASK
#define MRCTRL0_mr_type(value)		_SET_FV(MRCTRL0_mr_type, value)
#define MRCTRL0_mr_Write		0
#define MRCTRL0_mr_Read			1
#endif

/* 5.1.5 MRCTRL1 */
#define MRCTRL1_mr_data_OFFSET		0
#define MRCTRL1_mr_data_MASK		(_BV(DW_UMCTL2_PAGE_BITS) - 1)
#define MRCTRL1_mr_data(value)		_SET_FV(MRCTRL1_mr_data, value)
#if defined(CONFIG_DW_UMCTL2_LPDDR2) || defined(CONFIG_DW_UMCTL2_LPDDR3) || \
    defined(CONFIG_DW_UMCTL2_LPDDR4)
#define MRCTRL1_lpddr234_data_OFFSET	0
#define MRCTRL1_lpddr234_data_MASK	REG_8BIT_MASK
#define MRCTRL1_lpddr234_data(value)	_SET_FV(MRCTRL1_lpddr234_data, value)
#define MRCTRL1_lpddr234_addr_OFFSET	8
#define MRCTRL1_lpddr234_addr_MASK	REG_8BIT_MASK
#define MRCTRL1_lpddr234_addr(value)	_SET_FV(MRCTRL1_lpddr234_addr, value)
#endif

/* 5.1.6 MRSTAT */
#define MRSTAT_mr_wr_busy		_BV(0)
#ifdef CONFIG_DW_UMCTL2_DDR4
#define MRSTAT_pda_done			_BV(8)
#endif

/* 5.1.7 MRCTRL2 */
#ifdef CONFIG_DW_UMCTL2_DDR4
#define MRCTRL2_mr_device_sel_OFFSET	0
#define MRCTRL2_mr_device_sel_MASK	REG_32BIT_MASK
#define MRCTRL2_mr_device_sel(value)	_SET_FV(MRCTRL2_mr_device_sel, value)
#endif

/* 5.1.12 PWRCTL */
#ifdef CONFIG_DW_UMCTL2_LPDDR4
#define PWRCTL_lpddr4_sr_allowed	_BV(8)
#endif
#define PWRCTL_dis_cam_drain_selfref	_BV(7)
#ifdef CONFIG_DW_UMCTL2_LPDDR4
#define PWRCTL_stay_in_selfref		_BV(6)
#endif /* CONFIG_DW_UMCTL2_LPDDR4 */
#define PWRCTL_selfref_sw		_BV(5)
#ifdef CONFIG_DW_UMCTL2_DDR4
#define PWRCTL_mpsm_en			_BV(4)
#endif /* CONFIG_DW_UMCTL2_DDR4 */
#define PWRCTL_en_dfi_dram_clk_disable	_BV(3)
#if defined(CONFIG_DW_UMCTL2_MOBILE) || defined(CONFIG_DW_UMCTL2_LPDDR2)
#define PWRCTL_deeppowerdown_en		_BV(2)
#endif /* CONFIG_DW_UMCTL2_MOBILE || CONFIG_DW_UMCTL2_LPDDR2 */
#define PWRCTL_powerdown_en		_BV(1)
#define PWRCTL_selfref_en		_BV(0)

/* 5.1.24 RFSHCTL3 */
#define RFSHCTL3_refresh_update_level	_BV(1)

/* 5.1.135 DBGCMD */
#define DBGCMD_rank_refresh(r)		_BV((r) > 4 ? (r) + 4 : (r))
#define DBGCMD_ranks_refresh(ranks)	\
	(((ranks) & REG_4BIT_MASK) |	\
	 ((((ranks) >> 4) & REG_12BIT_MASK) << 8))
#define DBGCMD_ctrlupd			_BV(5)
#if defined(CONFIG_DW_UMCTL2_DDR3) || defined(CONFIG_DW_UMCTL2_DDR4) ||	\
    defined(CONFIG_DW_UMCTL2_LPDDR2)
#define DBGCMD_zq_calib_short		_BV(4)
#endif

/* 5.1.136 DBGSTAT */
#define DBGSTAT_rank_refresh_busy(r)	_BV((r) > 4 ? (r) + 4 : (r))
#define DBGSTAT_ranks_refresh_busy(ranks)	\
	(((ranks) & REG_4BIT_MASK) |	\
	 ((((ranks) >> 4) & REG_12BIT_MASK) << 8))
#define DBGSTAT_ctrlupd_busy		_BV(5)
#if defined(CONFIG_DW_UMCTL2_DDR3) || defined(CONFIG_DW_UMCTL2_DDR4) ||	\
    defined(CONFIG_DW_UMCTL2_LPDDR2)
#define DBGCMD_zq_calib_short_busy	_BV(4)
#endif

/* Dynamic - refresh */
/* 5.1.20 RFSHCTL0 */
#define RFSHCTL0_refresh_margin_OFFSET		20
#define RFSHCTL0_refresh_margin_MASK		REG_4BIT_MASK
#define RFSHCTL0_refresh_margin(value)		\
	_SET_FV(RFSHCTL0_refresh_margin, DW_UMCTL2_ROUNDUP(value, 5))
#define RFSHCTL0_refresh_margin_dynamic		\
	RFSHCTL0_refresh_margin(64) /* default reset value */
#define RFSHCTL0_refresh_to_x1_x32_OFFSET	12
#define RFSHCTL0_refresh_to_x1_x32_MASK		REG_5BIT_MASK
#define RFSHCTL0_refresh_to_x1_x32(value)	\
	_SET_FV(RFSHCTL0_refresh_to_x1_x32, DW_UMCTL2_ROUNDUP(value, 5))
#define RFSHCTL0_refresh_to_x1_x32_dynamic	\
	RFSHCTL0_refresh_to_x1_x32(512) /* default reset value */
#define RFSHCTL0_refresh_burst_OFFSET		4
#define RFSHCTL0_refresh_burst_MASK		REG_6BIT_MASK
#define RFSHCTL0_refresh_burst(value)		\
	_SET_FV(RFSHCTL0_refresh_burst, (value) - 1)
#define RFSHCTL0_refresh_burst_dynamic		\
	RFSHCTL0_refresh_burst(1) /* default reset value */

/* 5.1.21 RFSHCTL1/RFSHCTL2 */
#define RFSHCTL12_refresh_timern_start_value_x32_OFFSET(r)	\
	REG_16BIT_OFFSET(r)
#define RFSHCTL12_refresh_timern_start_value_x32_MASK		\
	REG_12BIT_MASK
#define RFSHCTL12_refresh_timern_start_value_x32(r, value)	\
	_SET_FVn(r, RFSHCTL12_refresh_timern_start_value_x32,	\
		 DW_UMCTL2_ROUNDUP(value, 5))
#define RFSHCTL12_refresh_timern_start_value_x32_dynamic	0
#define RFSHCTL12_refresh_timern_start_value_x32_mask(r)	\
	RFSHCTL12_refresh_timern_start_value_x32(		\
		r, RFSHCTL12_refresh_timern_start_value_x32_MASK)

/* 5.1.24 RFSHCTL3 */
#define RFSHCTL3_dis_auto_refresh	_BV(0)

/* 5.1.25 RFSHTMG */
#ifdef CONFIG_DW_UMCTL2_LPDDR2
#define RFSHTMG_t_rfc_nom_x1_sel		_BV(31)
#define RFSHTMG_t_rfc_nom_x1_sel_dynamic	RFSHTMG_t_rfc_nom_x1_sel
#endif
#define RFSHTMG_t_rfc_nom_x1_x32_OFFSET		16
#define RFSHTMG_t_rfc_nom_x1_x32_MASK		REG_12BIT_MASK
#define RFSHTMG_t_rfc_nom_x1_x32(value)		\
	_SET_FV(RFSHTMG_t_rfc_nom_x1_x32, DW_UMCTL2_ROUNDUP(value, 5))
#define RFSHTMG_t_rfc_nom_x1_x32_dynamic	\
	RFSHTMG_t_rfc_nom_x1_x32(DW_UMCTL2_RANKS_MASK)
#define RFSHTMG_t_rfc_min_OFFSET		0
#define RFSHTMG_t_rfc_min_MASK			REG_10BIT_MASK
#define RFSHTMG_t_rfc_min(value)		\
	_SET_FV(RFSHTMG_t_rfc_min, value)

/* Quasi-dynamic Group 1, Group 4 */
/* 5.1.54 */
#define INIT3_mr_OFFSET		16
#define INIT3_mr_MASK		REG_16BIT_MASK
#define INIT3_mr(value)		_SET_FV(INIT3_mr, value)
#define INIT3_mr_mask		INIT3_mr(INIT3_mr_MASK)

/* Quasi-dynamic Group 2 */
/* 5.1 UMCTL2_REGS Registers */
/* 5.1.1 MSTR */
#define MSTR_frequency_mode		_BV(29)
#ifdef CONFIG_DW_UMCTL2_FAST_FREQ_CHANGE
#define MSTR_frequency_mode_quasi	MSTR_frequency_mode
#else /* CONFIG_DW_UMCTL2_FAST_FREQ_CHANGE */
#define MSTR2_target_frequency_quasi	0
#define MSTR_frequency_mode_quasi	0
#endif /* CONFIG_DW_UMCTL2_FAST_FREQ_CHANGE */
#ifdef CONFIG_DW_UMCTL2_DLL_OFF
#define MSTR_dll_off_mode		_BV(15)
#define MSTR_dll_off_mode_quasi		MSTR_dll_off_mode
#else
#define MSTR_dll_off_mode_quasi		0
#endif
#ifdef CONFIG_DW_UMCTL2_GEARDOWN
#define MSTR_geardown_mode		_BV(11)
#define MSTR_geardown_mode_quasi	MSTR_geardown_mode
#else
#define MSTR_geardown_mode_quasi	0
#endif

/* 5.1.10 MSTR2 */
#ifdef CONFIG_DW_UMCTL2_FAST_FREQ_CHANGE
#define MSTR2_target_frequency_OFFSET	0
#define MSTR2_target_frequency_MASK	REG_2BIT_MASK
#define MSTR2_target_frequency(value)	_SET_FV(MSTR2_target_frequency, value)
#define MSTR2_target_frequency_quasi	MSTR2_target_frequency(0)
#endif

/* 5.1.14 HWLPCTL */
#define HWLPCTL_hw_lp_en		_BV(0)

/* 5.1.24 RFSHCTL3 */
#ifdef CONFIG_DW_UMCTL2_DDR4
#define RFSHCTL3_refresh_mode_OFFSET	4
#define RFSHCTL3_refresh_mode_MASK	REG_3BIT_MASK
#define RFSHCTL3_refresh_mode(value)	_SET_FV(RFSHCTL3_refresh_mode, value)
#define RFSHCTL3_refresh_mode_Fixed1x		0
#define RFSHCTL3_refresh_mode_Fixed2x		1
#define RFSHCTL3_refresh_mode_Fixed4x		2
#define RFSHCTL3_refresh_mode_OnTheFly2x	5 /* not supported */
#define RFSHCTL3_refresh_mode_OnTheFly4x	6 /* not supported */
#define RFSHCTL3_refresh_mode_quasi		\
	RFSHCTL3_refresh_mode(RFSHCTL3_refresh_mode_Fixed1x)
#define RFSHCTL3_refresh_mode_mask		\
	RFSHCTL3_refresh_mode(RFSHCTL3_refresh_mode_MASK)
#else
#define RFSHCTL3_refresh_mode_quasi		0
#define RFSHCTL3_refresh_mode_mask		0
#endif

/* 5.1.51 INIT0 */
#define INIT0_skip_dram_init_OFFSET		30
#define INIT0_skip_dram_init_MASK		REG_2BIT_MASK
#define INIT0_skip_dram_init(value)		\
	_SET_FV(INIT0_skip_dram_init, value)
#define INIT0_skip_dram_init_mask		\
	INIT0_skip_dram_init(INIT0_skip_dram_init_MASK)
#define INIT0_skip_dram_init_none		0
#define INIT0_skip_dram_init_normal		1
#define INIT0_skip_dram_init_self_refresh	3
#if defined(CONFIG_DW_UMCTL2_TRAINING) || \
    defined(CONFIG_DW_UMCTL2_DRAM_INIT_ACCEL)
#define INIT0_skip_dram_init_quasi		\
	INIT0_skip_dram_init(INIT0_skip_dram_init_self_refresh)
#else
#define INIT0_skip_dram_init_quasi		\
	INIT0_skip_dram_init(INIT0_skip_dram_init_none)
#endif

/* Quasi-dynamic Group 2, Group 4 */
/* 5.1.57 INIT6 */
#if defined(CONFIG_DW_UMCTL2_DDR4) || defined(CONFIG_DW_UMCTL2_LPDDR4)
#define INIT6_mr4_OFFSET		16
#define INIT6_mr4_MASK			REG_16BIT_MASK
#define INIT6_mr4(value)		_SET_FV(INIT6_mr4, value)
#define INIT6_mr4_mask			INIT6_mr4(INIT6_mr4_MASK)
#define INIT6_mr5_OFFSET		0
#define INIT6_mr5_MASK			REG_16BIT_MASK
#define INIT6_mr5(value)		_SET_FV(INIT6_mr5, value)
#define INIT6_mr5_mask			INIT6_mr5(INIT6_mr5_MASK)
#endif

/* Quasi-dynamic Group 4 */
/* 5.1.13 PWRTMG */
#define PWRTMG_selfref_to_x32_OFFSET	16
#define PWRTMG_selfref_to_x32_MASK	REG_8BIT_MASK
#define PWRTMG_selfref_to_x32(value)	\
	_SET_FV(PWRTMG_selfref_to_x32, DW_UMCTL2_ROUNDUP(value, 5))
#define PWRTMG_selfref_to_x32_mask	\
	PWRTMG_selfref_to_x32(PWRTMG_selfref_to_x32_MASK)
#if defined(CONFIG_DW_UMCTL2_MOBILE) || defined(CONFIG_DW_UMCTL2_LPDDR2)
#define PWRTMG_t_dpd_x4096_OFFSET	8
#define PWRTMG_t_dpd_x4096_MASK		REG_8BIT_MASK
#define PWRTMG_t_dpd_x4096(value)	\
	_SET_FV(PWRTMG_t_dpd_x4096, DW_UMCTL2_ROUNDUP(value, 12))
#define PWRTMG_t_dpd_x4096_mask		\
	PWRTMG_t_dpd_x4096(PWRTMG_t_dpd_x4096_MASK)
#endif
#define PWRTMG_powerdown_to_x32_OFFSET	0
#define PWRTMG_powerdown_to_x32_MASK	REG_5BIT_MASK
#define PWRTMG_powerdown_to_x32(value)	\
	_SET_FV(PWRTMG_powerdown_to_x32, DW_UMCTL2_ROUNDUP(value, 5))
#define PWRTMG_powerdown_to_x32_mask	\
	PWRTMG_powerdown_to_x32(PWRTMG_powerdown_to_x32_MASK)

/* 5.1.54 INIT3 */
#define INIT3_emr_OFFSET		0
#define INIT3_emr_MASK			REG_16BIT_MASK
#define INIT3_emr(value)		_SET_FV(INIT3_emr, value)
#define INIT3_emr_mask			INIT3_emr(INIT3_emr_MASK)

/* 5.1.55 INIT4 */
#define INIT4_emr2_OFFSET		16
#define INIT4_emr2_MASK			REG_16BIT_MASK
#define INIT4_emr2(value)		_SET_FV(INIT4_emr2, value)
#define INIT4_emr2_mask			INIT4_emr2(INIT4_emr2_MASK)
#define INIT4_emr3_OFFSET		0
#define INIT4_emr3_MASK			REG_16BIT_MASK
#define INIT4_emr3(value)		_SET_FV(INIT4_emr3, value)
#define INIT4_emr3_mask			INIT4_emr3(INIT4_emr3_MASK)

/* 5.1.58 INIT7 */
#ifdef CONFIG_DW_UMCTL2_LPDDR4
#define INIT7_mr22_OFFSET		16
#define INIT7_mr22_MASK			REG_16BIT_MASK
#define INIT7_mr22(value)		_SET_FV(INIT7_mr22, value)
#define INIT7_mr22_mask			INIT7_mr22(INIT7_mr22_MASK)
#endif
#if defined(CONFIG_DW_UMCTL2_DDR4) || defined(CONFIG_DW_UMCTL2_LPDDR4)
#define INIT7_mr6_OFFSET		0
#define INIT7_mr6_MASK			REG_16BIT_MASK
#define INIT7_mr6(value)		_SET_FV(INIT7_mr6, value)
#define INIT7_mr6_mask			INIT7_mr6(INIT7_mr6_MASK)
#endif

/* ssi_memory_map/ssi_address_block registers */

#define CONFIG_DW_UMCTL2_DEBUG_REGS	1

#ifdef CONFIG_DW_UMCTL2_DEBUG_REGS
#define dw_umctl2_write(v, a)		\
	printf("CTL W: %016llx %08x\n", (uint64_t)(a), (unsigned int)(v))
#define dw_umctl2_read(a)		\
	(printf("CTL R: %016llx\n", (uint64_t)(a)), 0)
#else
#define dw_umctl2_write(v, a)		__raw_writel(v, a)
#define dw_umctl2_read(a)		__raw_readl(a)
#endif
#define dw_umctl2_set(v, a)				\
	do {						\
		uint32_t __v = dw_umctl2_read(a);	\
		__v |= (v);				\
		dw_umctl2_write(__v, (a));		\
	} while (0)
#define dw_umctl2_clear(v, a)				\
	do {						\
		uint32_t __v = dw_umctl2_read(a);	\
		__v &= ~(v);				\
		dw_umctl2_write(__v, (a));		\
	} while (0)
#define dw_umctl2_toggle(v, a)				\
	do {						\
		uint32_t __v = dw_umctl2_read(a);	\
		__v ^= (v);				\
		dw_umctl2_write(__v, (a));		\
	} while (0)
#define dw_umctl2_set_msk(v, m, a)			\
	do {						\
		uint32_t __v = dw_umctl2_read(a);	\
		__v &= ~(m);				\
		__v |= (v);				\
		dw_umctl2_write(__v, (a));		\
	} while (0)

/* Device Low Power Control:
 *
 * Configures how deep the device should enter when idle:
 * PD:   2.25.2.1 Precharge Power Down: command idle
 * SR:   2.25.2.2 Self-refresh: command idle
 * DPD:  2.25.2.3 Deep Power-down: transaction store is empty
 * MPSM: 2.25.2.4 Maximum Power Saving Mode: transaction store is empty
 * CS:   2.25.2.5 Assertion of dfi_dram_clk_disable (clock stop)
 * DO:   2.25.2.6 DLL-off Mode (DDR3 and DDR4)
 * The default initialization process only configures the device into idle
 * self-refresh mode, while cycles are configured to the default values.
 */
#define DW_UMCTL2_POWER_NONE		0
#define DW_UMCTL2_POWER_SR		_BV(1)
#define DW_UMCTL2_POWER_PD		_BV(2)
#if defined(CONFIG_DW_UMCTL2_MOBILE) || defined(CONFIG_DW_UMCTL2_LPDDR2)
#define DW_UMCTL2_POWER_DPD		_BV(3)
#else
#define DW_UMCTL2_POWER_DPD		DW_UMCTL2_POWER_PD
#endif
#ifdef CONFIG_DW_UMCTL2_DDR4
#define DW_UMCTL2_POWER_MPSM		_BV(3) /* Excluded with DPD */
#else
#define DW_UMCTL2_POWER_MPSM		DW_UMCTL2_POWER_PD
#endif
#define DW_UMCTL2_POWER_CS		_BV(4)
#define DW_UMCTL2_POWER_DO		_BV(5)
#ifdef CONFIG_DW_UMCTL2_TRAINING
/* Should use selfref_sw */
#define DW_UMCTL2_POWER_DEF		DW_UMCTL2_POWER_NONE
#else
#define DW_UMCTL2_POWER_DEF		DW_UMCTL2_POWER_PD
#endif
#define dw_umctl2_enable_idle_sr(n, c)			\
	dw_umctl2_set(PWRCTL_selfref_en, UMCTL2_PWRCTL(n, c))
#define dw_umctl2_disable_idle_sr(n, c)			\
	dw_umctl2_clear(PWRCTL_selfref_en, UMCTL2_PWRCTL(n, c))
void dw_umctl2_set_idle_sr(uint8_t n, uint8_t c, uint8_t f,
			   bool enable, uint16_t cycles);
#define dw_umctl2_enable_idle_pd(n, c)			\
	dw_umctl2_set(PWRCTL_powerdown_en, UMCTL2_PWRCTL(n, c))
#define dw_umctl2_disable_idle_pd(n, c)			\
	dw_umctl2_clear(PWRCTL_powerdown_en, UMCTL2_PWRCTL(n, c))
void dw_umctl2_set_idle_pd(uint8_t n, uint8_t c, uint8_t f,
			   bool enable, uint16_t cycles);
#if defined(CONFIG_DW_UMCTL2_MOBILE) || defined(CONFIG_DW_UMCTL2_LPDDR2)
#define dw_umctl2_enable_idle_dpd(n, c)			\
	dw_umctl2_set(PWRCTL_deeppowerdown_en, UMCTL2_PWRCTL(n, c))
#define dw_umctl2_disable_idle_dpd(n, c)		\
	dw_umctl2_clear(PWRCTL_deeppowerdown_en, UMCTL2_PWRCTL(n, c))
void dw_umctl2_set_idle_dpd(uint8_t n, uint8_t c, uint8_t f,
			    bool enable, uint32_t cycles);
#else
#define dw_umctl2_enable_idle_dpd(n, c)			do { } while (0)
#define dw_umctl2_disable_idle_dpd(n, c)		do { } while (0)
#define dw_umctl2_set_idle_dpd(n, c, f, en, cyc)	do { } while (0)
#endif
#ifdef CONFIG_DW_UMCTL2_DDR4
#define dw_umctl2_enable_idle_mpsm(n, c)		\
	dw_umctl2_set(PWRCTL_mpsm_en, UMCTL2_PWRCTL(n, c))
#define dw_umctl2_disable_idle_mpsm(n, c)		\
	dw_umctl2_clear(PWRCTL_mpsm_en, UMCTL2_PWRCTL(n, c))
void dw_umctl2_set_idle_mpsm(uint8_t n, uint8_t c, bool enable);
#else
#define dw_umctl2_enable_idle_mpsm(n, c)		do { } while (0)
#define dw_umctl2_disable_idle_mpsm(n, c)		do { } while (0)
#define dw_umctl2_set_idle_mpsm(n, c, en)		do { } while (0)
#endif
#define dw_umctl2_enable_idle_cs(n, c)			\
	dw_umctl2_set(PWRCTL_en_dfi_dram_clk_disable, UMCTL2_PWRCTL(n, c))
#define dw_umctl2_disable_idle_cs(n, c)			\
	dw_umctl2_clear(PWRCTL_en_dfi_dram_clk_disable, UMCTL2_PWRCTL(n, c))
void dw_umctl2_set_idle(uint8_t n, uint8_t c, uint8_t f,
			uint8_t state, uint16_t cycles_selfref,
			uint16_t cycles_powerdown);

#define dw_umctl2_entry_sr(n, c)	\
	dw_umctl2_set(PWRCTL_selfref_sw, UMCTL2_PWRCTL(n, c))
#define dw_umctl2_exit_sr(n, c)		\
	dw_umctl2_clear(PWRCTL_selfref_sw, UMCTL2_PWRCTL(n, c))

#ifdef CONFIG_DW_UMCTL2_DDR4
void dw_umctl2_ddr4_init_device(uint8_t n);
void dw_umctl2_ddr4_init_refresh(uint8_t n, uint8_t ranks);
void dw_umctl2_ddr4_config_refresh(uint8_t n, uint8_t mode,
				   uint32_t tREFI, uint32_t tRFCmin);
void dw_umctl2_ddr4_init_rstn(uint8_t n, uint32_t tRESET_n);
void dw_umctl2_ddr4_init_pre_cke(uint8_t n,
				 uint32_t tCKEactive, uint32_t tCKEstab);
void dw_umctl2_ddr4_init_post_cke(uint8_t n, uint32_t tXS);
#else
#define dw_umctl2_ddr4_init_device(n)		do { } while (0)
#define dw_umctl2_ddr4_init_refresh(n, ranks)	do { } while (0)
#define dw_umctl2_ddr4_init_rstn(n, tRESET_n)	do { } while (0)
#define dw_umctl2_ddr4_config_refresh(n, c, mode, tREFI, tRFCmin)	\
	do { } while (0)
#define dw_umctl2_ddr4_init_pre_cke(n, tCKEactive, tCKEstab)		\
	do { } while (0)
#define dw_umctl2_ddr4_init_post_cke(n, tXS)	do { } while (0)
#endif

void dw_umctl2_init(void);
void dw_umctl2_start(void);
void dw_umctl2_mr_write(uint8_t n, uint8_t c, uint8_t ranks,
			uint16_t v, uint8_t r);

/* Power-up and Initialization */
void dw_umctl2_init_mr(uint8_t n, uint16_t v, uint8_t r);

extern uint8_t dw_umctl2_f;
extern uint16_t dw_umctl2_spd;
extern uint8_t dw_umctl2_freq_ratio;

#endif /* __DW_UMCTL2_H_INCLUDE__ */
