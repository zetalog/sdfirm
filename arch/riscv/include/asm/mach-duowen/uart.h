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
 * @(#)dw_uart.h: Synopsys DesignWare UART interface
 * $Id: dw_uart.h,v 1.1 2019-09-26 10:40:00 zhenglv Exp $
 */

#ifndef __UART_DUOWEN_H_INCLUDE__
#define __UART_DUOWEN_H_INCLUDE__

#include <target/paging.h>
#include <target/gpio.h>
#include <target/clk.h>

#ifdef CONFIG_DUOWEN_UART_CON0
#define __DUOWEN_UART_BASE	UART0_BASE
#define UART_CLK_ID		uart0_clk
#define UART_CON_ID		0
#define UART_CON_IRQ		IRQ_UART0
#define UART_CON_PORT_IO	GPIO1B
#define UART_CON_PORT_MODEM	GPIO2A
#define UART_CON_PIN_SIN	pad_gpio_54
#define UART_CON_PIN_SOUT	pad_gpio_55
#define UART_CON_PIN_CTS	pad_gpio_80
#define UART_CON_PIN_RTS	pad_gpio_81
#define UART_CON_PIN_DSR	pad_gpio_82
#define UART_CON_PIN_DTR	pad_gpio_83
#define UART_CON_PIN_RI		pad_gpio_84
#define UART_CON_PIN_DCD	pad_gpio_85
#endif
#ifdef CONFIG_DUOWEN_UART_CON1
#define __DUOWEN_UART_BASE	UART1_BASE
#define UART_CLK_ID		uart1_clk
#define UART_CON_ID		1
#define UART_CON_IRQ		IRQ_UART1
#define UART_CON_PORT_IO	GPIO1B
#define UART_CON_PORT_MODEM	GPIO2A
#define UART_CON_PIN_SIN	pad_gpio_62
#define UART_CON_PIN_SOUT	pad_gpio_63
#define UART_CON_PIN_CTS	pad_gpio_88
#define UART_CON_PIN_RTS	pad_gpio_89
#define UART_CON_PIN_DSR	pad_gpio_90
#define UART_CON_PIN_DTR	pad_gpio_91
#define UART_CON_PIN_RI		pad_gpio_92
#define UART_CON_PIN_DCD	pad_gpio_93
#endif
#ifdef CONFIG_DUOWEN_UART_CON2
#define __DUOWEN_UART_BASE	UART2_BASE
#define UART_CLK_ID		uart2_clk
#define UART_CON_ID		2
#define UART_CON_IRQ		IRQ_UART2
#define UART_CON_PORT_IO	GPIO1C
#define UART_CON_PORT_MODEM	GPIO2A
#define UART_CON_PIN_SIN	pad_gpio_70
#define UART_CON_PIN_SOUT	pad_gpio_71
#define UART_CON_PIN_CTS	pad_gpio_96
#define UART_CON_PIN_RTS	pad_gpio_97
#define UART_CON_PIN_DSR	pad_gpio_98
#define UART_CON_PIN_DTR	pad_gpio_99
#define UART_CON_PIN_RI		pad_gpio_100
#define UART_CON_PIN_DCD	pad_gpio_101
#endif
#ifdef CONFIG_DUOWEN_UART_CON3
#define __DUOWEN_UART_BASE	UART3_BASE
#define UART_CLK_ID		uart3_clk
#define UART_CON_ID		3
#define UART_CON_IRQ		IRQ_UART3
#define UART_CON_PORT_IO	GPIO1D
#define UART_CON_PORT_MODEM	GPIO2A
#define UART_CON_PIN_SIN	pad_gpio_78
#define UART_CON_PIN_SOUT	pad_gpio_79
#define UART_CON_PIN_CTS	pad_gpio_104
#define UART_CON_PIN_RTS	pad_gpio_105
#define UART_CON_PIN_DSR	pad_gpio_106
#define UART_CON_PIN_DTR	pad_gpio_107
#define UART_CON_PIN_RI		pad_gpio_108
#define UART_CON_PIN_DCD	pad_gpio_109
#endif

#ifdef CONFIG_MMU
#define DUOWEN_UART_BASE	duowen_uart_reg_base
#else
#define DUOWEN_UART_BASE	__DUOWEN_UART_BASE
#endif
#define DW_UART_REG(n, offset)	(DUOWEN_UART_BASE + (offset))

#if defined(CONFIG_DW_UART)
#include <driver/dw_uart.h>
#ifndef ARCH_HAVE_UART
#define ARCH_HAVE_UART		1
#else
#error "Multiple UART controller defined"
#endif
#endif

#ifdef CONFIG_DUOWEN_UART_ACCEL
#define UART_CON_BAUDRATE		(APB_CLK_FREQ/16)
#endif

#ifdef CONFIG_DEBUG_PRINT
void uart_hw_dbg_init(void);
void uart_hw_dbg_start(void);
void uart_hw_dbg_stop(void);
void uart_hw_dbg_write(uint8_t byte);
void uart_hw_dbg_config(uint8_t params, uint32_t baudrate);
#endif

#ifdef CONFIG_CONSOLE
#ifdef CONFIG_CLK
void uart_hw_con_init(void);
#else
#define uart_hw_con_init()	do { } while (0)
#endif
#endif
#ifdef CONFIG_CONSOLE_OUTPUT
#define uart_hw_con_write(byte)	dw_uart_con_write(byte)
#endif
#ifdef CONFIG_CONSOLE_INPUT
#define uart_hw_con_read()	dw_uart_con_read()
#define uart_hw_con_poll()	dw_uart_con_poll()
#ifndef CONFIG_SYS_NOIRQ
#define uart_hw_irq_init()	dw_uart_irq_init()
#define uart_hw_irq_ack()	dw_uart_irq_ack()
#endif
#endif
#ifndef CONFIG_SYS_NOIRQ
#define uart_hw_irq_init()	dw_uart_irq_init()
#define uart_hw_irq_ack()	dw_uart_irq_ack()
#endif

#ifdef CONFIG_MMU
void uart_hw_mmu_init(void);
#endif

#endif /* __UART_DUOWEN_H_INCLUDE__ */
