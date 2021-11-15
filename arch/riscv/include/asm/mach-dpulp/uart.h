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
 * @(#)uart.h: DPU-LP specific UART controller interface
 * $Id: uart.h,v 1.1 2021-11-01 15:03:00 zhenglv Exp $
 */

#ifndef __UART_DPULP_H_INCLUDE__
#define __UART_DPULP_H_INCLUDE__

#include <target/gpio.h>
#include <target/clk.h>

#ifdef CONFIG_DPULP_UART_CON0
#define UART_CLK_ID		srst_uart
#define DW_UART_REG(n, offset)	(UART0_BASE + (offset))
#define UART_CON_ID		0
#define UART_CON_IRQ		IRQ_UART0
#endif /* CONFIG_DPULP_UART_CON0 */
#ifdef CONFIG_DPULP_UART_CON1
#define UART_CLK_ID		srst_uart
#define DW_UART_REG(n, offset)	(UART1_BASE + (offset))
#define UART_CON_ID		1
#define UART_CON_IRQ		IRQ_UART1
#endif /* CONFIG_DPULP_UART_CON1 */
#ifdef CONFIG_DPULP_UART_CON2
#define UART_CLK_ID		srst_uart
#define DW_UART_REG(n, offset)	(UART2_BASE + (offset))
#define UART_CON_ID		2
#define UART_CON_IRQ		IRQ_UART2
#endif /* CONFIG_DPULP_UART_CON2 */
#ifdef CONFIG_DPULP_UART_CON3
#define UART_CLK_ID		srst_uart
#define DW_UART_REG(n, offset)	(UART3_BASE + (offset))
#define UART_CON_ID		3
#define UART_CON_IRQ		IRQ_UART3
#endif /* CONFIG_DPULP_UART_CON3 */

#ifdef CONFIG_DW_UART
#include <driver/dw_uart.h>
#ifndef ARCH_HAVE_UART
#define ARCH_HAVE_UART		1
#else
#error "Multiple UART controller defined"
#endif
#endif

#ifdef CONFIG_DPULP_UART_VIP
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
#define uart_hw_con_init()						\
	do {								\
		board_init_clock();					\
		clk_enable(UART_CLK_ID);				\
		dw_uart_con_init(clk_get_frequency(UART_CLK_ID));	\
	} while (0)
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

#endif /* __UART_DPULP_H_INCLUDE__ */
