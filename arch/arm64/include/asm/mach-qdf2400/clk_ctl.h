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
 * clk_ctl.h: clock controller interfaces
 * $Id: clk_ctl.h,v 1.279 2019-04-14 10:19:18 zhenglv Exp $
 */

#ifndef __QDF2400_CLK_CTL_H_INCLUDE__
#define __QDF2400_CLK_CTL_H_INCLUDE__

#include <asm/io.h>

/* CC: Clock controller block definitions */

/* CBC: Clock Branch Controller */
/* CBCR */
#define CBC_CLK_OFF			_BV(31)
#define CBC_CLK_ENABLE			_BV(0)

/* RCG: Root Clock Generator */
#define RCG_REG_OFFSET_CMD_RCGR		0x00
#define RCG_REG_OFFSET_CFG_RCGR		0x04
#define RCG_REG_OFFSET_M		0x08
#define RCG_REG_OFFSET_N		0x0C
#define RCG_REG_OFFSET_D		0x10

/* CMD_RCGR */
#define RCG_ROOT_OFF			_BV(31)
#define RCG_DIRTY_D			_BV(7)
#define RCG_DIRTY_M			_BV(6)
#define RCG_DIRTY_N			_BV(5)
#define RCG_DIRTY_CFG_RCFR		_BV(4)
#define RCG_ROOT_EN			_BV(1)
#define RCG_UPDATE			_BV(0)

/* CFG_RCGR */
#define RCG_MODE_OFFSET			12
#define RCG_MODE_MASK			0x03
#define RCG_MODE(value)			_SET_FV(RCG_MODE, value)
/* MODE field is only used for MND clock divider function
 * (__clock_generate_mnd()). However, a non RCG_MODE_BYPASS value can be
 * used to tell an HID divider function (__clock_generate_hid()) to use a
 * non bypass mode HID value.
 */
#define RCG_MODE_BYPASS			0
#define RCG_MODE_SWALLOW		1
#define RCG_MODE_DUAL_EDGE		2
#define RCG_MODE_SINGLE_EDGE		3
#define RCG_SRC_SEL_OFFSET		8
#define RCG_SRC_SEL_MASK		0x07
#define RCG_SRC_SEL(value)		_SET_FV(RCG_SRC_SEL, value)
/* Half integer divider */
#define RCG_SRC_DIV_OFFSET		0
#define RCG_SRC_DIV_MASK		0x1F
#define RCG_SRC_DIV(value)		_SET_FV(RCG_SRC_DIV, value)

/* RCG can have HID and MND divider:
 * HID is configured via SRC_DIV field of CFG_RCGR registers
 * MND is configured via M/N/D registers
 * MND algorithm:
 *  CLK_OUT = CLK_IN * M / N
 *  REG_M = M, REG_N = ~(N-M), REG_D = ~N
 */
#define RCG_MND_OUT_HZ(input_hz, n, m)	\
	(((input_hz) / n) * m)
#define RCG_HID_OUT_HZ(input_hz, hid)	\
	((hid) ? (input_hz) * 2 / ((hid) + 1) : (input_hz))

#define RCG_HID_DIV_MAX			16

#define __clk_rcgr_reversed(cmd_rcgr)	\
	(!!((cmd_rcgr) == PCC_I2C_CMD_RCGR(PCC_BLSP1_QUP0)))

struct clk_cbc {
	caddr_t base;
	clk_t input_clk;
	uint8_t refcnt;
};

struct clk_rcg {
	caddr_t base;
	uint8_t nr_input_clks;
	clk_t *input_clks;
	uint32_t def_output_hz[NR_FREQPLANS];
	uint8_t src_sel;
	uint32_t output_hz;
	uint32_t flags;
	uint8_t refcnt;
};

/* RCG flags */
#define CLK_MND_MASK_OFFSET	0
#define CLK_MND_MASK_MASK	0xFFFF
#define CLK_MND_MASK(value)	_SET_FV(CLK_MND_MASK, value)
#define CLK_MND_MODE_OFFSET	16
#define CLK_MND_MODE_MASK	0xFF
#define CLK_MND_MODE(value)	_SET_FV(CLK_MND_MODE, value)

#define RCG_CMD_RCGR(cmd_rcgr)				\
	(__clk_rcgr_reversed(cmd_rcgr) ?		\
	 ((cmd_rcgr) + RCG_REG_OFFSET_CFG_RCGR) :	\
	 (cmd_rcgr))
#define RCG_CFG_RCGR(cmd_rcgr)				\
	(__clk_rcgr_reversed(cmd_rcgr) ?		\
	 (cmd_rcgr) :					\
	 ((cmd_rcgr) + RCG_REG_OFFSET_CFG_RCGR))
#define RCG_M(cmd_rcgr)		((cmd_rcgr) + RCG_REG_OFFSET_M)
#define RCG_N(cmd_rcgr) 	((cmd_rcgr) + RCG_REG_OFFSET_N)
#define RCG_D(cmd_rcgr) 	((cmd_rcgr) + RCG_REG_OFFSET_D)

#define __clk_is_branch_off(cbcr)	\
	(__raw_readl(cbcr) & CBC_CLK_OFF)
#define __clk_enable_branch(cbcr)	\
	__raw_setl(CBC_CLK_ENABLE, (cbcr))
#define __clk_disable_branch(cbcr)	\
	__raw_clearl(CBC_CLK_ENABLE, (cbcr))
#define __clk_update_root(cmd_rcgr)	\
	__raw_setl(RCG_UPDATE, RCG_CMD_RCGR(cmd_rcgr))
#define __clk_enable_root(cmd_rcgr)	\
	__raw_setl(RCG_ROOT_EN, RCG_CMD_RCGR(cmd_rcgr))
#define __clk_disable_root(cmd_rcgr)	\
	__raw_clearl(RCG_ROOT_EN, RCG_CMD_RCGR(cmd_rcgr))

void __clk_config_root(caddr_t cmd_rcgr, uint8_t clk_sel, uint8_t hid,
		       uint32_t m, uint32_t n, uint8_t mode);
void __clk_generate_mnd(caddr_t cmd_rcgr, uint8_t clk_sel,
			uint32_t input_hz, uint32_t output_hz,
			uint8_t mode, uint16_t mnd_mask);
void __clk_generate_hid(caddr_t cmd_rcgr, uint8_t clk_sel,
			uint32_t input_hz, uint32_t output_hz,
			uint8_t mode);

typedef struct clk_ctl {
	uint8_t src_sel;
	uint32_t input_hz;
	uint32_t output_hz;
	caddr_t cmd_rcgr;
	uint32_t flags;
} __packed clk_ctl_cfg;

void clock_config_mux(const clk_ctl_cfg *cfg);
void clock_toggle_clk(caddr_t cbcr, bool enable);

#endif /* __QDF2400_CLK_CTL_H_INCLUDE__ */
