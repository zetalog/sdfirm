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
 * @(#)uart.c: DUOWEN machine specific UART implementation
 * $Id: uart.c,v 1.1 2020-07-01 10:32:00 zhenglv Exp $
 */

#include <target/console.h>
#include <target/paging.h>

static inline void uart_config_pad(bool rxtx, uint16_t pin,
				   pad_cfg_t pad, uint8_t func)
{
	if (rxtx) {
		gpio_config_mux(UART_CON_PORT_DATA, pin, func);
		gpio_config_pad(UART_CON_PORT_DATA, pin, pad, 8);
	} else {
		gpio_config_mux(UART_CON_PORT_CTRL, pin, func);
		gpio_config_pad(UART_CON_PORT_CTRL, pin, pad, 8);
	}
}

void uart_hw_con_init(void)
{
	uart_config_pad(true, UART_CON_PIN_RXD, GPIO_PAD_PULL_DOWN,
			TLMM_PAD_FUNCTION);
	uart_config_pad(true, UART_CON_PIN_TXD, GPIO_PAD_PULL_DOWN,
			TLMM_PAD_FUNCTION);
	uart_config_pad(false, UART_CON_PIN_CTS, GPIO_PAD_PULL_UP,
			TLMM_PAD_FUNCTION);
	uart_config_pad(false, UART_CON_PIN_RTS, GPIO_PAD_PULL_UP,
			TLMM_PAD_FUNCTION);
	uart_config_pad(false, UART_CON_PIN_DSR, GPIO_PAD_PULL_UP,
			TLMM_PAD_FUNCTION);
	uart_config_pad(false, UART_CON_PIN_DTR, GPIO_PAD_PULL_UP,
			TLMM_PAD_FUNCTION);
	uart_config_pad(false, UART_CON_PIN_RI, GPIO_PAD_PULL_UP,
			TLMM_PAD_FUNCTION);
	uart_config_pad(false, UART_CON_PIN_DCD, GPIO_PAD_PULL_UP,
			TLMM_PAD_FUNCTION);

	board_init_clock();
	clk_enable(UART_CLK_ID);
	dw_uart_ctrl_init(clk_get_frequency(UART_CLK_ID));
}

#ifdef CONFIG_MMU
void uart_hw_mmu_init(void)
{
	duowen_mmu_map_uart(UART_CON_ID);
	uart_hw_con_init();
	duowen_mmu_dump_maps();
}
#endif
