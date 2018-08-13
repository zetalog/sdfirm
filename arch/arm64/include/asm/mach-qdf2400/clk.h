/*
 * ZETALOG's Personal COPYRIGHT
 *
 * Copyright (c) 2018
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
 * @(#)clk.h: qualcomm generic clock controller definitions
 * $Id: clk.h,v 1.279 2018-09-20 10:19:18 zhenglv Exp $
 */

#ifndef __CLK_QDF2400_H_INCLUDE__
#define __CLK_QDF2400_H_INCLUDE__

#include <target/config.h>
#include <target/types.h>

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
 */
#define RCG_MND_OUT_HZ(input_hz, n, m)	\
	(((input_hz) / n) * m)
#define RCG_HID_OUT_HZ(input_hz, hid)	\
	((hid) ? (input_hz) * 2 / ((hid) + 1) : (input_hz))

#define RCG_HID_DIV_MAX			16

/* CLK_OUT = CLK_IN * M / N */
/* M */
#define RCG_M_OFFSET			0
#define RCG_M_MASK			0xFFFF
#define RCG_M(value)			_SET_FV(RCG_M, value)
#define RCG_M_MAX			RCG_M_MASK
/* N */
#define RCG_N_OFFSET			0
#define RCG_N_MASK			0xFFFF
#define RCG_N(value)			_SET_FV(RCG_N, value)
#define RCG_N_MAX			RCG_N_MASK
/* D */
#define RCG_D_OFFSET			0
#define RCG_D_MASK			0xFFFF
#define RCG_D(value)			_SET_FV(RCG_D, value)
#define RCG_D_MAX			RCG_D_MASK

#define RCG_MND_DIV_MAX			65535

#define __clk_enable_branch(cbcr)	__raw_setl(CBC_CLK_ENABLE, (cbcr))
#define __clk_update_root(cmd_rcgr)	__raw_setl(RCG_UPDATE, (cmd_rcgr))
#define __clk_enable_root(cmd_rcgr)	__raw_setl(RCG_ROOT_EN, (cmd_rcgr))
#define __clk_disable_root(cmd_rcgr)	__raw_clearl(RCG_ROOT_EN, (cmd_rcgr))

void __clk_config_root(caddr_t cmd_rcgr, uint8_t mode, uint8_t clk_sel,
		       uint8_t hid, uint16_t m, uint16_t n);
void __clk_generate_root(caddr_t cmd_rcgr, uint8_t mode, uint8_t clk_sel,
			 uint32_t input_hz, uint32_t output_hz);

/* Peripherals are controlled by clk */
#include <asm/mach/pcc.h>

#endif /* __CLK_QDF2400_H_INCLUDE__ */
