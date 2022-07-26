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
 * @(#)uart.h: GUANGMU specific UART (16550) interface
 * $Id: uart.h,v 1.1 2019-09-26 10:40:00 zhenglv Exp $
 */

#ifndef __UART_GUANGMU_H_INCLUDE__
#define __UART_GUANGMU_H_INCLUDE__

#include <target/arch.h>
#include <target/paging.h>
#include <target/gpio.h>
#include <target/clk.h>

#define __GM_UART_BASE		UART0_BASE
#ifdef CONFIG_MMU
#define GM_UART_BASE			gm_uart_reg_base
extern caddr_t gm_uart_reg_base;
#else
#define GM_UART_BASE			__GM_UART_BASE
#endif

#define UART_CON_ID			0
#define UART_BASE(n)			VIRT_UART_BASE

#ifndef ARCH_HAVE_UART
#define ARCH_HAVE_UART
#else
#error "Multiple UART controller defined"
#endif

#ifdef CONFIG_MMU
void uart_hw_mmu_init(void);
#endif

#ifdef CONFIG_CONSOLE
#define uart_hw_con_init()	dw_uart_init()
#endif
#ifdef CONFIG_CONSOLE_OUTPUT
#define uart_hw_con_write(byte)	console_putc(byte)
#endif
#ifdef CONFIG_CONSOLE_INPUT
void uart_hw_irq_ack(void);
void uart_hw_irq_init(void);
#endif

#endif /* __UART_GUANGMU_H_INCLUDE__ */
