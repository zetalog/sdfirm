/*
 * ZETALOG's Personal COPYRIGHT
 *
 * Copyright (c) 2023
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
 * @(#)sysreg.h: K1Matrix system registers definitions
 * $Id: sysreg.h,v 1.1 2023-09-06 18:48:00 zhenglv Exp $
 */

#ifndef __SYSREG_K1MATRIX_H_INCLUDE__
#define __SYSREG_K1MATRIX_H_INCLUDE__

#include <asm/mach/reg.h>

#define SYSREG_REG(offset)	(SYSREG_BASE + (offset))

#define CPU_SW_RESET		SYSREG_REG(0x000)
#define DDR_SW_RESET		SYSREG_REG(0x100)
#define SYS_SW_RESET		SYSREG_REG(0x200)
#define PCIE_SW_RESET		SYSREG_REG(0x300)
#define GMAC_SW_RESET		SYSREG_REG(0x400)
#define SYS_CTRL_GMAC		SYSREG_REG(0x500)
#define SYS_CTRL_MESH		SYSREG_REG(0x600)

#define GPIO_AUX_CFG0		SYSREG_REG(0x2000)
#define GPIO_AUX_CFG1		SYSREG_REG(0x2004)
#define GPIO_AUX_CFG2		SYSREG_REG(0x2008)
#define GPIO_AUX_CFG3		SYSREG_REG(0x200C)
#define PAD_IN_STATUS		SYSREG_REG(0x2010)

/* CPU_SW_RESET */
#define CPU_GLOBAL_RESET	_BV(0)
#define CPU_RESET(cpu)		_BV(1+(cpu))

/* DDR_SW_RESET */
#define DDR_GLOBAL_RESET	_BV(0)

/* SYS_SW_RESET */
#define SYS_GLOBAL_RESET	_BV(0)

/* PCIE_SW_RESET */
#define PCIE_GLOBAL_RESET	_BV(0)
#define PCIE0_COLD_RESET	_BV(1)
#define PCIE0_BUTTON_RESET	_BV(2)
#define PCIE0_WARM_RESET	_BV(3)
#define PCIE1_COLD_RESET	_BV(4)
#define PCIE1_BUTTON_RESET	_BV(5)
#define PCIE1_WARM_RESET	_BV(6)

/* GMAC_SW_RESET */
#define GMAC_GLOBAL_RESET	_BV(0)
#define GMAC_CSR_RESET		_BV(1)
#define GMAC_DMA_RESET		_BV(2)

/* SYS_CTRL_GMAC */
#define GMAC_RMII_EN		_BV(0)
#define GMAC_CLK_EN		_BV(1)

/* SYS_CTRL_MESH */
#define MESH_NID8_SYSCOREQ	_BV(0)
#define MESH_NID40_SYSCOREQ	_BV(1)
#define MESH_NID8_SYSCOACK	_BV(8)
#define MESH_NID40_SYSCOACK	_BV(9)

#define sysreg_soft_reset()		\
	__raw_writel(CPU_GLOBAL_RESET, CPU_SW_RESET)
#define sysreg_soft_reset_cpu(cpu)	\
	__raw_setl(CPU_RESET(cpu), CPU_SW_RESET)

#define k1matrix_cpu_reset()	do { } while (0)

#endif /* __SYSREG_K1MATRIX_H_INCLUDE__ */