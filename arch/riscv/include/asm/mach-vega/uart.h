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
 * @(#)uart.h: RV32M1 (VEGA) specific UART defintions
 * $Id: uart.h,v 1.1 2019-08-16 09:46:00 zhenglv Exp $
 */

#ifndef __UART_VEGA_H_INCLUDE__
#define __UART_VEGA_H_INCLUDE__

#include <target/config.h>
#include <target/generic.h>
#include <target/bitops.h>
#include <target/gpio.h>
#include <target/clk.h>

#ifdef CONFIG_VEGA_RI5CY
#define LPUART0_BASE		UL(0x40042000)
#define LPUART1_BASE		UL(0x40043000)
#define LPUART2_BASE		UL(0x40044000)
#define LPUART_BASE(n)		(LPUART0_BASE + (n) * 0x1000)
#endif
#ifdef CONFIG_VEGA_0RISCY
#define LPUART3_BASE		UL(0x41036000)
#define LPUART_BASE(n)		(LPUART3_BASE + ((n) - 3) * 0x1000)
#endif

#include <asm/mach/lpuart.h>
#ifndef ARCH_HAVE_UART
#define ARCH_HAVE_UART		1
#else
#error "Multiple UART controller defined"
#endif

#define UART_PIN(port, pin)	MAKEWORD(port, pin)
#define UART_PIN_PORT(pin)	LOBYTE(pin)
#define UART_PIN_PIN(pin)	HIBYTE(pin)

#ifdef CONFIG_LPUART0_PTA_2
#define uart0_clks	{porta_clk, invalid_clk, lpuart0_clk}
#define UART0_PIN_CTS	UART_PIN(GPIO_PORTA, 1)
#define UART0_PIN_RTS	UART_PIN(GPIO_PORTA, 4)
#define UART0_PIN_RX	UART_PIN(GPIO_PORTA, 2)
#define UART0_PIN_TX	UART_PIN(GPIO_PORTA, 3)
#define UART0_MUX	2
#endif /* CONFIG_LPUART0_PTA_2 */
#ifdef CONFIG_LPUART0_PTB_3
#define uart0_clks	{portb_clk, invalid_clk, lpuart0_clk}
#define UART0_PIN_CTS	UART_PIN(GPIO_PORTB, 22)
#define UART0_PIN_RTS	UART_PIN(GPIO_PORTB, 24)
#define UART0_PIN_RX	UART_PIN(GPIO_PORTB, 25)
#define UART0_PIN_TX	UART_PIN(GPIO_PORTB, 26)
#define UART0_MUX	3
#endif /* CONFIG_LPUART0_PTB_3 */
#ifdef CONFIG_LPUART0_PTC_3
#define uart0_clks	{portc_clk, invalid_clk, lpuart0_clk}
#define UART0_PIN_CTS	UART_PIN(GPIO_PORTC, 9)
#define UART0_PIN_RTS	UART_PIN(GPIO_PORTC, 10)
#define UART0_PIN_RX	UART_PIN(GPIO_PORTC, 7)
#define UART0_PIN_TX	UART_PIN(GPIO_PORTC, 8)
#define UART0_MUX	3
#endif /* CONFIG_LPUART0_PTB_3 */
#ifdef CONFIG_LPUART1_PTA_4
#define uart1_clks	{porta_clk, invalid_clk, lpuart1_clk}
#define UART1_PIN_CTS	UART_PIN(GPIO_PORTA, 1)
#define UART1_PIN_RTS	UART_PIN(GPIO_PORTA, 4)
#define UART1_PIN_RX	UART_PIN(GPIO_PORTA, 2)
#define UART1_PIN_TX	UART_PIN(GPIO_PORTA, 3)
#define UART1_MUX	4
#endif /* CONFIG_LPUART1_PTA_4 */
#ifdef CONFIG_LPUART1_PTA_2
#define uart1_clks	{porta_clk, invalid_clk, lpuart1_clk}
#define UART1_PIN_CTS	UART_PIN(GPIO_PORTA, 27)
#define UART1_PIN_RTS	UART_PIN(GPIO_PORTA, 28)
#define UART1_PIN_RX	UART_PIN(GPIO_PORTA, 25)
#define UART1_PIN_TX	UART_PIN(GPIO_PORTA, 26)
#define UART1_MUX	2
#endif /* CONFIG_LPUART1_PTA_2 */
#ifdef CONFIG_LPUART1_PTB_3
#define uart1_clks	{portb_clk, invalid_clk, lpuart1_clk}
#define UART1_PIN_CTS	UART_PIN(GPIO_PORTB, 4)
#define UART1_PIN_RTS	UART_PIN(GPIO_PORTB, 5)
#define UART1_PIN_RX	UART_PIN(GPIO_PORTB, 2)
#define UART1_PIN_TX	UART_PIN(GPIO_PORTB, 3)
#define UART1_MUX	3
#endif /* CONFIG_LPUART1_PTB_3 */
#ifdef CONFIG_LPUART1_PTC_2
#define uart1_clks	{portd_clk, portc_clk, lpuart1_clk}
#define UART1_PIN_CTS	UART_PIN(GPIO_PORTD, 0)
#define UART1_PIN_RTS	UART_PIN(GPIO_PORTD, 1)
#define UART1_PIN_RX	UART_PIN(GPIO_PORTC, 29)
#define UART1_PIN_TX	UART_PIN(GPIO_PORTC, 30)
#define UART1_MUX	2
#endif /* CONFIG_LPUART1_PTC_2 */
#ifdef CONFIG_LPUART2_PTA_2
#define uart2_clks	{porta_clk, portb_clk, lpuart2_clk}
#define UART2_PIN_CTS	UART_PIN(GPIO_PORTA, 30)
#define UART2_PIN_RTS	UART_PIN(GPIO_PORTA, 31)
#define UART2_PIN_RX	UART_PIN(GPIO_PORTB, 1)
#define UART2_PIN_TX	UART_PIN(GPIO_PORTB, 0)
#define UART2_MUX	2
#endif /* CONFIG_LPUART2_PTA_2 */
#ifdef CONFIG_LPUART2_PTB_2
#define uart2_clks	{portb_clk, invalid_clk, lpuart2_clk}
#define UART2_PIN_CTS	UART_PIN(GPIO_PORTB, 13)
#define UART2_PIN_RTS	UART_PIN(GPIO_PORTB, 14)
#define UART2_PIN_RX	UART_PIN(GPIO_PORTB, 11)
#define UART2_PIN_TX	UART_PIN(GPIO_PORTB, 12)
#define UART2_MUX	2
#endif /* CONFIG_LPUART2_PTB_2 */
#ifdef CONFIG_LPUART2_PTB_3
#define uart2_clks	{portb_clk, invalid_clk, lpuart2_clk}
#define UART2_PIN_CTS	UART_PIN(GPIO_PORTB, 20)
#define UART2_PIN_RTS	UART_PIN(GPIO_PORTB, 21)
#define UART2_PIN_RX	UART_PIN(GPIO_PORTB, 18)
#define UART2_PIN_TX	UART_PIN(GPIO_PORTB, 19)
#define UART2_MUX	3
#endif /* CONFIG_LPUART2_PTB_3 */
/* NOTE: LPUART3_PTB_3 won't work for RI5CY/0RISCY as:
 *       1. GPIOB is not accessible for 0RISCY;
 *       2. UART3 is not accessible for RI5CY.
 */
#ifdef CONFIG_LPUART3_PTB_3
#define uart3_clks	{portb_clk, invalid_clk, lpuart3_clk}
#define UART3_PIN_CTS	UART_PIN(GPIO_PORTB, 16)
#define UART3_PIN_RTS	UART_PIN(GPIO_PORTB, 17)
#define UART3_PIN_RX	UART_PIN(GPIO_PORTB, 28)
#define UART3_PIN_TX	UART_PIN(GPIO_PORTB, 29)
#define UART3_MUX	3
#endif /* CONFIG_LPUART3_PTB_3 */
#ifdef CONFIG_LPUART3_PTE_3
#define uart3_clks	{porte_clk, invalid_clk, lpuart3_clk}
#define UART3_PIN_CTS	UART_PIN(GPIO_PORTE, 10)
#define UART3_PIN_RTS	UART_PIN(GPIO_PORTE, 11)
#define UART3_PIN_RX	UART_PIN(GPIO_PORTE, 8)
#define UART3_PIN_TX	UART_PIN(GPIO_PORTE, 9)
#define UART3_MUX	3
#endif /* CONFIG_LPUART3_PTE_3 */
#ifdef CONFIG_LPUART3_PTE_2
#define uart3_clks	{porte_clk, invalid_clk, lpuart3_clk}
#define UART3_PIN_CTS	UART_PIN(GPIO_PORTE, 27)
#define UART3_PIN_RTS	UART_PIN(GPIO_PORTE, 28)
#define UART3_PIN_RX	UART_PIN(GPIO_PORTE, 29)
#define UART3_PIN_TX	UART_PIN(GPIO_PORTE, 30)
#define UART3_MUX	2
#endif /* CONFIG_LPUART3_PTE_2 */

#ifdef CONFIG_LPUART_CON_0
#define UART_CON_ID	0
#define uart_clks	uart0_clks
#define UART_MUX	UART0_MUX
#define UART_PIN_CTS	UART0_PIN_CTS
#define UART_PIN_RTS	UART0_PIN_RTS
#define UART_PIN_RX	UART0_PIN_RX
#define UART_PIN_TX	UART0_PIN_TX
#endif
#ifdef CONFIG_LPUART_CON_1
#define UART_CON_ID	1
#define uart_clks	uart1_clks
#define UART_MUX	UART1_MUX
#define UART_PIN_CTS	UART1_PIN_CTS
#define UART_PIN_RTS	UART1_PIN_RTS
#define UART_PIN_RX	UART1_PIN_RX
#define UART_PIN_TX	UART1_PIN_TX
#endif
#ifdef CONFIG_LPUART_CON_2
#define UART_CON_ID	2
#define uart_clks	uart2_clks
#define UART_MUX	UART2_MUX
#define UART_PIN_CTS	UART2_PIN_CTS
#define UART_PIN_RTS	UART2_PIN_RTS
#define UART_PIN_RX	UART2_PIN_RX
#define UART_PIN_TX	UART2_PIN_TX
#endif
#ifdef CONFIG_LPUART_CON_3
#define UART_CON_ID	3
#define uart_clks	uart3_clks
#define UART_MUX	UART3_MUX
#define UART_PIN_CTS	UART3_PIN_CTS
#define UART_PIN_RTS	UART3_PIN_RTS
#define UART_PIN_RX	UART3_PIN_RX
#define UART_PIN_TX	UART3_PIN_TX
#endif
#define NR_UART_CLKS	3

#ifdef CONFIG_DEBUG_PRINT
void uart_hw_dbg_init(void);
void uart_hw_dbg_start(void);
void uart_hw_dbg_stop(void);
void uart_hw_dbg_write(uint8_t byte);
void uart_hw_dbg_config(uint8_t params, uint32_t baudrate);
#endif

#ifdef CONFIG_CONSOLE
void uart_hw_con_init(void);
#endif
#ifdef CONFIG_CONSOLE_OUTPUT
#define uart_hw_con_write(byte)	lpuart_write_byte(UART_CON_ID, byte)
#endif
#ifdef CONFIG_CONSOLE_INPUT
#define uart_hw_con_read()	lpuart_read_byte(UART_CON_ID)
#define uart_hw_con_poll()	lpuart_ctrl_poll(UART_CON_ID)
void uart_hw_irq_ack(void);
void uart_hw_irq_init(void);
#else
#define uart_hw_con_read()	-1
#define uart_hw_con_poll()	false
#endif

#endif /* __UART_VEGA_H_INCLUDE__ */
