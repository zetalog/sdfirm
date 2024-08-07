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
#include <asm/mach/sysreg_cpu.h>
#define SYSREG_REG(offset)	(SYS_REG_BASE + (offset))

#define CPU_SW_RESET		SYSREG_REG(0x000)
#define CORE_SW_RESET		SYSREG_REG(0x004)
#define DDR_SW_RESET		SYSREG_REG(0x100)
#define SYS_SW_RESET		SYSREG_REG(0x200)
#define PCIE_SW_RESET		SYSREG_REG(0x300)
#define GMAC_SW_RESET		SYSREG_REG(0x400)
#define SYS_CTRL_GMAC		SYSREG_REG(0x500)
#define SYS_CTRL_MESH		SYSREG_REG(0x600)

#define EFUSE_INFO_REG0		SYSREG_REG(0x1000) /* CPU mask */
#define EFUSE_INFO_REG1		SYSREG_REG(0x1004) /* boot CPU */

#define GPIO_AUX_CFG0		SYSREG_REG(0x2000)
#define GPIO_AUX_CFG1		SYSREG_REG(0x2004)
#define GPIO_AUX_CFG2		SYSREG_REG(0x2008)
#define GPIO_AUX_CFG3		SYSREG_REG(0x200C)
#define PAD_IN_STATUS		SYSREG_REG(0x2010)

#define RMU_UART0_RESET		SYSREG_REG(0x2340)
#define RMU_UART0_RESET_BIT	_BV(0)

/* CPU_SW_RESET */
#define CPU_GLOBAL_RESET	_BV(0)

/* CORE_SW_RESET */
#define CPU_RESET(cpu)		_BV(cpu)

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

/* EFUSE_INFO_REG0 */
#define CPU_CORE_VLD(n)		_BV(n)

/* EFUSE_INFO_REG1 */
#define DIE_ID			_BV(5)
#define BOOTHART_OFFSET		0
#define BOOTHART_MASK		REG_5BIT_MASK
#define BOOTHART(value)		_GET_FV(BOOTHART, value)

/* SYS_CTRL_GMAC */
#define GMAC_RMII_EN		_BV(0)
#define GMAC_CLK_EN		_BV(1)

/* PAD_IN_STATUS */
#define PAD_BOOT_SEL_OFFSET	1
#define PAD_BOOT_SEL_MASK	REG_1BIT_MASK
#define PAD_BOOT_SEL(value)	_GET_FV(PAD_BOOT_SEL, value)
#define __PAD_BOOT_SEL		_BV(PAD_BOOT_SEL_OFFSET)
#define PAD_DIE_ID_OFFSET	0
#define PAD_DIE_ID_MASK		REG_1BIT_MASK
#define PAD_DIE_ID(value)	_GET_FV(PAD_DIE_ID, value)
#define __PAD_DIE_ID		_BV(PAD_DIE_ID_OFFSET)

#define sysreg_soft_reset()		\
	__raw_writel(CPU_GLOBAL_RESET, CPU_SW_RESET)
#define sysreg_soft_reset_cpu(cpu)	\
	__raw_setl(CPU_RESET(cpu), CORE_SW_RESET)
#define sysreg_pcie_global_reset()	\
	__raw_setl(PCIE_GLOBAL_RESET, PCIE_SW_RESET)
#define sysreg_pcie0_cold_reset()	\
	__raw_setl(PCIE0_COLD_RESET, PCIE_SW_RESET)
#define sysreg_pcie0_button_reset()	\
	__raw_setl(PCIE0_BUTTON_RESET, PCIE_SW_RESET)
#define sysreg_pcie0_warm_reset()	\
	__raw_setl(PCIE0_WARM_RESET, PCIE_SW_RESET)
#define sysreg_rmu_uart0_reset() \
	__raw_setl(RMU_UART0_RESET_BIT, RMU_UART0_RESET)
#define sysreg_boot_sel()		\
	PAD_BOOT_SEL(__raw_readl(PAD_IN_STATUS))
#define sysreg_die_id()			\
	PAD_DIE_ID(__raw_readl(PAD_IN_STATUS))
#define sysreg_boot_cpu()		\
	BOOTHART(__raw_readl(EFUSE_INFO_REG1))
#define sysreg_cpu_mask()		\
	__raw_readl(EFUSE_INFO_REG0)
#define set_cluster_core_rvba_l(cluster, core, offset, value)	\
	__raw_writel(value & 0xffffffff, CPUREG_REG(cluster,core,offset))
#define set_cluster_core_rvba_h(cluster, core, offset, value)	\
	__raw_writel(value  >> 32, CPUREG_REG(cluster,core,offset))
#define read_cluster_core(cluster, core, offset)		\
	__raw_readl(CPUREG_REG(cluster,core,offset))


#ifndef __ASSEMBLY__
void k1matrix_cpu_reset(void);
uint32_t acpu_get_cpu_map(void);
uint8_t acpu_get_cluster_map(void);
#endif

#endif /* __SYSREG_K1MATRIX_H_INCLUDE__ */
