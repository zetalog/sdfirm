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
 * @(#)uart.h: SUNXI UART serial definitions
 * $Id: uart.h,v 1.1 2019-11-17 13:16:00 zhenglv Exp $
 */

#ifndef __UART_SUNXI_H_INCLUDE__
#define __UART_SUNXI_H_INCLUDE__

#include <target/config.h>
#include <target/generic.h>
#include <target/gpio.h>
#include <target/clk.h>

#ifdef CONFIG_ARM_DCC
#include <asm/debug.h>
#else /* CONFIG_ARM_DCC */
#if defined(CONFIG_UART_SUNXI)
#define UART0_BASE		ULL(0x01C28000)
#define UART1_BASE		ULL(0x01C28400)
#define UART2_BASE		ULL(0x01C28800)
#define UART3_BASE		ULL(0x01C28C00)
#define R_UART_BASE		ULL(0x01F02800)

#define UART_FREQ		24000000

#define UART_BASE(n)		(UART0_BASE + (n) * 0x400)

#ifdef CONFIG_UART_SUNXI_CON0
#define UART_CON_ID		0
#endif
#ifdef CONFIG_UART_SUNXI_CON1
#define UART_CON_ID		1
#endif
#ifdef CONFIG_UART_SUNXI_CON2
#define UART_CON_ID		2
#endif
#ifdef CONFIG_UART_SUNXI_CON3
#define UART_CON_ID		3
#endif

#include <driver/ns16550.h>
#ifndef ARCH_HAVE_UART
#define ARCH_HAVE_UART		1
#else
#error "Multiple UART controller defined"
#endif
#endif

#ifdef CONFIG_DEBUG_PRINT
void uart_hw_dbg_init(void);
void uart_hw_dbg_start(void);
void uart_hw_dbg_stop(void);
void uart_hw_dbg_write(uint8_t byte);
void uart_hw_dbg_config(uint8_t params, uint32_t baudrate);
#endif

#ifdef CONFIG_CONSOLE
#define uart_hw_con_init()		ns16550_con_init()
#endif
#ifdef CONFIG_CONSOLE_OUTPUT
#define uart_hw_con_write(byte)		ns16550_con_write(byte)
#endif
#ifdef CONFIG_CONSOLE_INPUT
#define uart_hw_con_read()		ns16550_con_read()
#define uart_hw_con_poll()		ns16550_con_poll()

void uart_hw_irq_ack(void);
void uart_hw_irq_init(void);
#endif
#endif /* CONFIG_ARM_DCC */

#endif /* __UART_SUNXI_H_INCLUDE__ */
