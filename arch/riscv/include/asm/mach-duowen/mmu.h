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

#define FIX_CLINT		(FIX_HOLE + 1)
#define FIX_CRCNTL		(FIX_HOLE + 2)
#define FIX_CFAB		(FIX_HOLE + 3)
#define FIX_APC0		(FIX_HOLE + 4)
#define FIX_APC1		(FIX_HOLE + 5)
#define FIX_APC2		(FIX_HOLE + 6)
#define FIX_APC3		(FIX_HOLE + 7)
#define FIX_ETH			(FIX_HOLE + 8)
#define FIX_UART		(FIX_HOLE + 9)
#define FIX_PLIC		(FIX_HOLE + 10)
#define FIX_GPIO0		(FIX_HOLE + 11)
#define FIX_GPIO1		(FIX_HOLE + 12)
#define FIX_GPIO2		(FIX_HOLE + 13)
#define FIX_TLMM		(FIX_HOLE + 14)
#define FIX_SCSR		(FIX_HOLE + 15)
#define FIX_LCSR		(FIX_HOLE + 16)
#define MMU_HW_MAX_FIXMAP	(FIX_LCSR + 1)

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
#else
#define duowen_mmu_map_clk()	do { } while (0)
#define duowen_mmu_dump_clk()	do { } while (0)
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
