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
 * @(#)mmu.h: DUOWEN specific MMU device mapping definitions
 * $Id: mmu.h,v 1.1 2020-09-24 13:29:00 zhenglv Exp $
 */

#ifndef __MMU_DUOWEN_H_INCLUDE__
#define __MMU_DUOWEN_H_INCLUDE__

#define __FIX_PLIC		1
#define __FIX_CRCNTL		2
#define __FIX_CFAB		3
#define __FIX_APC0		4
#define __FIX_APC1		5
#define __FIX_APC2		6
#define __FIX_APC3		7
#define __FIX_ETH		8
#define __FIX_UART		9
#define __FIX_GPIO0		10
#define __FIX_GPIO1		11
#define __FIX_GPIO2		12
#define __FIX_TLMM		13
#define __FIX_SCSR		14
#define __FIX_LCSR		15
#define __FIX_MAX		(__FIX_LCSR + 1)
#define __MMU_HW_MAX_FIXMAP	(FIX_HOLE + __FIX_MAX)
#ifdef CONFIG_DUOWEN_BBL_DUAL
#define __FIX_HOLE(soc)		(FIX_HOLE + ((soc) ? __FIX_MAX : 0))
#define FIX_PLIC(soc)		(__FIX_HOLE(soc) + __FIX_PLIC)
#define FIX_CRCNTL(soc)		(__FIX_HOLE(soc) + __FIX_CRCNTL)
#define FIX_CFAB(soc)		(__FIX_HOLE(soc) + __FIX_CFAB)
#define FIX_APC0(soc)		(__FIX_HOLE(soc) + __FIX_APC0)
#define FIX_APC1(soc)		(__FIX_HOLE(soc) + __FIX_APC1)
#define FIX_APC2(soc)		(__FIX_HOLE(soc) + __FIX_APC2)
#define FIX_APC3(soc)		(__FIX_HOLE(soc) + __FIX_APC3)
#define FIX_ETH(soc)		(__FIX_HOLE(soc) + __FIX_ETH)
#define FIX_UART(soc)		(__FIX_HOLE(soc) + __FIX_UART)
#define FIX_GPIO0(soc)		(__FIX_HOLE(soc) + __FIX_GPIO0)
#define FIX_GPIO1(soc)		(__FIX_HOLE(soc) + __FIX_GPIO1)
#define FIX_GPIO2(soc)		(__FIX_HOLE(soc) + __FIX_GPIO2)
#define FIX_TLMM(soc)		(__FIX_HOLE(soc) + __FIX_TLMM)
#define FIX_SCSR(soc)		(__FIX_HOLE(soc) + __FIX_SCSR)
#define FIX_LCSR(soc)		(__FIX_HOLE(soc) + __FIX_LCSR)
#define MMU_HW_MAX_FIXMAP	(__FIX_HOLE(1) + __FIX_MAX)
#else /* CONFIG_DUOWEN_BBL_DUAL */
#define FIX_PLIC(soc)		(FIX_HOLE + __FIX_PLIC)
#define FIX_CRCNTL(soc)		(FIX_HOLE + __FIX_CRCNTL)
#define FIX_CFAB(soc)		(FIX_HOLE + __FIX_CFAB)
#define FIX_APC0(soc)		(FIX_HOLE + __FIX_APC0)
#define FIX_APC1(soc)		(FIX_HOLE + __FIX_APC1)
#define FIX_APC2(soc)		(FIX_HOLE + __FIX_APC2)
#define FIX_APC3(soc)		(FIX_HOLE + __FIX_APC3)
#define FIX_ETH(soc)		(FIX_HOLE + __FIX_ETH)
#define FIX_UART(soc)		(FIX_HOLE + __FIX_UART)
#define FIX_GPIO0(soc)		(FIX_HOLE + __FIX_GPIO0)
#define FIX_GPIO1(soc)		(FIX_HOLE + __FIX_GPIO1)
#define FIX_GPIO2(soc)		(FIX_HOLE + __FIX_GPIO2)
#define FIX_TLMM(soc)		(FIX_HOLE + __FIX_TLMM)
#define FIX_SCSR(soc)		(FIX_HOLE + __FIX_SCSR)
#define FIX_LCSR(soc)		(FIX_HOLE + __FIX_LCSR)
#define MMU_HW_MAX_FIXMAP	(FIX_HOLE + __FIX_MAX)
#endif /* CONFIG_DUOWEN_BBL_DUAL */

void duowen_mmu_dump_maps(void);
#ifdef CONFIG_DUOWEN_UART
extern caddr_t duowen_uart_reg_base;
void duowen_mmu_map_uart(int n);
void duowen_mmu_dump_uart(void);
#else
#define duowen_mmu_map_uart(n)	do { } while (0)
#define duowen_mmu_dump_uart()	do { } while (0)
#endif
#ifdef CONFIG_CRCNTL
void duowen_mmu_map_clk(void);
void duowen_mmu_dump_clk(void);
void duowen_mmu_init_clk(void);
#else
#define duowen_mmu_map_clk()	do { } while (0)
#define duowen_mmu_dump_clk()	do { } while (0)
#define duowen_mmu_init_clk()	do { } while (0)
#endif
#ifdef CONFIG_DUOWEN_GPIO
extern caddr_t duowen_gpio_reg_base[];
extern caddr_t duowen_tlmm_reg_base;
void duowen_mmu_map_gpio(void);
void duowen_mmu_dump_gpio(void);
#else
#define duowen_mmu_map_gpio()	do { } while (0)
#define duowen_mmu_dump_gpio()	do { } while (0)
#endif
#ifdef CONFIG_DUOWEN_SCSR
extern caddr_t duowen_scsr_reg_base;
extern caddr_t duowen_lcsr_reg_base;
void duowen_mmu_map_scsr(void);
void duowen_mmu_dump_scsr(void);
#else
#define duowen_mmu_map_scsr()	do { } while (0)
#define duowen_mmu_dump_scsr()	do { } while (0)
#endif

#endif /* __MMU_DUOWEN_H_INCLUDE__ */
