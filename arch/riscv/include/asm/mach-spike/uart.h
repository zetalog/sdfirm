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
 * @(#)uart.h: SPIKE specific uart controller definitions
 * $Id: uart.h,v 1.1 2020-01-15 15:50:00 zhenglv Exp $
 */

#ifndef __UART_SPIKE_H_INCLUDE__
#define __UART_SPIKE_H_INCLUDE__

#include <target/arch.h>

#define UART_CON_ID			0

#ifdef CONFIG_SPIKE_UART
#ifndef ARCH_HAVE_UART
#define ARCH_HAVE_UART		1
#else
#error "Multiple UART controller defined"
#endif
#endif

#ifdef CONFIG_SPIKE_HTIF
#define uart_hw_con_write(byte)	htif_console_write(byte)
#define uart_hw_con_read()	htif_console_read()
#define uart_hw_con_poll()	htif_console_poll()
#define uart_hw_con_init()	do { } while (0)
#ifdef CONFIG_MMU
#define uart_hw_mmu_init()	do { } while (0)
#endif /* CONFIG_MMU */
#ifndef CONFIG_SYS_NOIRQ
#define uart_hw_irq_init()	do { } while (0)
#define uart_hw_irq_ack()	do { } while (0)
#endif
#endif /* CONFIG_SPIKE_HTIF */

#ifdef CONFIG_SPIKE_8250
#define NS16550_CLK		10000000
#define UART_BASE(n)		__UART_BASE
#include <driver/ns16550.h>

#ifdef CONFIG_CONSOLE
#define uart_hw_con_init()              \
        do {                            \
                ns16550_con_init();     \
        } while (0)
#endif
#ifdef CONFIG_CONSOLE_OUTPUT
#define uart_hw_con_write(byte)		ns16550_con_write(byte)
#endif
#ifdef CONFIG_CONSOLE_INPUT
#define uart_hw_con_read()		ns16550_con_read()
#define uart_hw_con_poll()		ns16550_con_poll()
#endif
#endif /* CONFIG_SPIKE_8250 */

#endif /* __UART_SPIKE_H_INCLUDE__ */
