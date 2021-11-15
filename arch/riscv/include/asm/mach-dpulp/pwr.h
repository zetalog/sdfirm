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
 * @(#)pwr.h: DPU-LP subsystem power controller definitions
 * $Id: pwr.h,v 1.1 2021-11-15 14:34:00 zhenglv Exp $
 */

#ifndef __PWR_DPULP_H_INCLUDE__
#define __PWR_DPULP_H_INCLUDE__

#define PWR_SHUT_DN			CRU_REG(0x000)
#define PWR_PS_HOLD			CRU_REG(0x004)
#define PWR_GLOBAL_RESET		CRU_REG(0x008)
#define PWR_SUB_PWR_CTRL(n)		CRU_REG(0x040 + ((n) << 3))
#define PWR_SUB_PWR_CFG(n)		CRU_REG(0x044 + ((n) << 3))

/* 3.4.1 SHUT_DN */
#define PWR_SHUTDN			_BV(0)
/* 3.4.2 PS_HOLD */
#define PWR_PSHOLD			_BV(0)
/* 3.4.3 GLOBAL_RESET */
#define PWR_RESET			_BV(0)
/* 3.4.4 SUB_PWR_CTRL */
#define PWR_DN_ACK			_BV(9)
#define PWR_UP_ACK			_BV(8)
#define PWR_DN				_BV(5)
#define PWR_UP				_BV(4)
#define PWR_SW_HANDSHAKE_OFFSET		0
#define PWR_SW_HANDSHAKE_MASK		REG_1BIT_MASK
#define PWR_HANDSHAKE_SOFTWARE		0x01
#define PWR_HANDSHAKE_HARDWARE		0x00
/* 3.4.5 SUB_PWR_CFG */
#define PWR_UP_DELAY_OFFSET		0
#define PWR_UP_DELAY_MASK		REG_16BIT_MASK
#define PWR_UP_DELAY(value)		_SET_FV(PWR_UP_DELAY, value)
#define PWR_DN_DELAY_OFFSET		16
#define PWR_DN_DELAY_MASK		REG_16BIT_MASK
#define PWR_DN_DELAY(value)		_SET_FV(PWR_DN_DELAY, value)

#define PWR_VDD_DPU			0
#define PWR_VDD_VPU			1
#define PWR_VDD_PCIE			2
#define PWR_VDD_RAB0			3
#define PWR_VDD_RAB1			4
#define PWR_VDD_ETH0			5
#define PWR_VDD_ETH1			6
#define PWR_VDD_DDR_PHY0		7
#define PWR_VDD_DDR_PHY1		8

#define pwr_shutdown()			__raw_setl(PWR_SHUTDN, PWR_SHUT_DN)
#define pwr_reset()			__raw_setl(PWR_RESET, PWR_GLOBAL_RESET)
#define pwr_get_ps_hold()		(__raw_readl(PWR_PSHOLD) & PWR_PS_HOLD)
#define pwr_set_ps_hold()		__raw_setl(PWR_PSHOLD, PWR_PS_HOLD)
#define pwr_cfg_handshake(n, type)					\
	__raw_writel_mask(PWR_SW_HANDSHAKE(type),			\
			  PWR_SW_HANDSHAKE(PWR_SW_HANDSHAKE_MASK),	\
			  PWR_SUB_PWR_CTRL(n))
#define pwr_cfg_delay(n, up_delay, dn_delay)				\
	__raw_writel(PWR_UP_DELAY(up_delay) | PWR_DN_DELAY(dn_delay),	\
		     PWR_SUB_PWR_CFG(n))

#endif /* __PWR_DPULP_H_INCLUDE__ */
