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
 * @(#)rio.h: DPU-LP specific RapidIO interconnect interface
 * $Id: rio.h,v 1.1 2021-11-02 15:47:00 zhenglv Exp $
 */

#ifndef __RIO_DPULP_H_INCLUDE__
#define __RIO_DPULP_H_INCLUDE__

#include <target/arch.h>
#include <target/clk.h>

#define RAB_BASE		RAB0_CTRL_BASE

#define DW_E16MP_BASE(n)	(RAB0_PHY_BASE + ((n) << 24))
#define RAB_CUST_REG(n, offset)	\
	((RAB0_PHY_BASE + ((n) << 24)) + 0x20000 + (offset))

#ifdef CONFIG_DPULP_RIO
#include <driver/mobiveil_rab.h>
#include <driver/dw_e16mp_tsmc12ffc.h>
#ifndef ARCH_HAVE_RIO
#define ARCH_HAVE_RIO		1
#else
#error "Multiple RIO controller defined"
#endif
#endif /* CONFIG_DPULP_RIO */

#define PHY_RESET(n)			RAB_CUST_REG(n, 0x00)
#define SC_AICAR_AR(n)			RAB_CUST_REG(n, 0x02)
#define SC_AIDCAR_AI(n)			RAB_CUST_REG(n, 0x04)
#define SC_AIDCAR_AVI(n)		RAB_CUST_REG(n, 0x06)
#define SC_DICAR_DR_LOW(n)		RAB_CUST_REG(n, 0x08)
#define SC_DICAR_DR_HI(n)		RAB_CUST_REG(n, 0x0A)
#define SC_DIDCAR_DI(n)			RAB_CUST_REG(n, 0x0C)
#define SC_DIDCAR_DVI(n)		RAB_CUST_REG(n, 0x0E)
#define SC_MISC(n)			RAB_CUST_REG(n, 0x10)
#define SC_PRESCALE_VALUE(n)		RAB_CUST_REG(n, 0x12)
#define CLOCK_SEL(n)			RAB_CUST_REG(n, 0x14)
#define SRAM_BYPASS(n)			RAB_CUST_REG(n, 0x16)
#define SRAM_EXT_LD_DONE(n)		RAB_CUST_REG(n, 0x18)
#define SRAM_INIT_DONE(n)		RAB_CUST_REG(n, 0x1a)

/* PHY_RESET */
#define RAB_phy_reset			_BV(0)
#define RAB_tx_reset			_BV(1)
#define RAB_rx_reset			_BV(2)
#define RAB_ref_clk_en			_BV(3)

#define RAB_SRIO_LANES			4

/* SC_MISC */
#define RAB_sc_pefcar_ctls		_BV(0)
#define RAB_sc_srio_mode_OFFSET		1
#define RAB_sc_srio_mode_MASK		REG_2BIT_MASK
#define RAB_sc_srio_mode(value)		_SET_FV(RAB_sc_srio_mode, value)
#define RAB_sc_rio_version		_BV(3)
#define RAB_sc_2x_force			_BV(4)
#define RAB_sc_1x_force			_BV(5)
#define RAB_sc_port_number_OFFSET	6
#define RAB_sc_port_number_MASK		REG_4BIT_MASK
#define RAB_sc_port_number(value)	_SET_FV(RAB_sc_port_number, value)

/* SC_PRESCALE_VALUE */
#define RAB_sc_prescale_value_OFFSET	0
#define RAB_sc_prescale_value_MASK	REG_7BIT_MASK
#define RAB_sc_prescale_value(value)	_SET_FV(RAB_sc_prescale_value, value)

/* CLOCK_SEL */
#define RAB_clock_sel_OFFSET		0
#define RAB_clock_sel_MASK		REG_2BIT_MASK
#define RAB_clock_sel(value)		_SET_FV(RAB_clock_sel, value)

/* SRAM_BYPASS */
#define RAB_sram_bypass			_BV(0)

/* SRAM_EXT_LD_DONE */
#define RAB_sram_ext_ld_done		_BV(0)

/* SRAM_INIT_DONE */
#define RAB_sram_init_done		_BV(0)

/* IP integration */
#ifdef CONFIG_DPULP_RIO_62500KHZ
#define RAB_SDS_FREQ			62500
#endif /* CONFIG_DPULP_RIO_62500KHZ */
#ifdef CONFIG_DPULP_RIO_78125KHZ
#define RAB_SDS_FREQ			78125
#endif /* CONFIG_DPULP_RIO_78125KHZ */
#ifdef CONFIG_DPULP_RIO_125000KHZ
#define RAB_SDS_FREQ			125000
#endif /* CONFIG_DPULP_RIO_125000KHZ */
#ifdef CONFIG_DPULP_RIO_156250KHZ
#define RAB_SDS_FREQ			156250
#endif /* CONFIG_DPULP_RIO_156250KHZ */

#ifdef CONFIG_MOBIVEIL_RAB
#define rio_hw_ctrl_init()		dpulp_rio_init(0)
#endif /* CONFIG_MOBIVEIL_RAB */

#ifdef CONFIG_DPULP_RIO
void dpulp_rio_init(int n);
#else /* CONFIG_DPULP_RIO */
#define dpulp_rio_init(n)		do { } while (0)
#endif /* CONFIG_DPULP_RIO */

#endif /* __RIO_DPULP_H_INCLUDE__ */
