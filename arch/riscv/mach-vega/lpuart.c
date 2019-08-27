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
 * @(#)lpuart.c: RV32M1 (VEGA) low power UART implementation
 * $Id: lpuart.c,v 1.1 2019-08-16 23:02:00 zhenglv Exp $
 */

#include <target/uart.h>
#include <target/gpio.h>
#include <target/clk.h>

#ifdef CONFIG_LPUART_FIFO
static void lpuart_enable_fifo_irqs(uint8_t id, lpuart_irq_t irqs)
{
	uint32_t fifo1;

	fifo1 = LPUART_GET_FIFOI1(irqs);
	/* unconditionally enabled: LPUART_GET_FIFOI2(irqs) */
	__raw_writel_mask(LPUART_FIFOIE1(fifo1),
			  LPUART_FIFOIE1(LPUART_FIFOIE1_MASK),
			  LPUART_FIFO(id));
}

static void lpuart_disable_fifo_irqs(uint8_t id)
{
	__raw_writel_mask(LPUART_FIFOIE1(0),
			  LPUART_FIFOIE1(LPUART_FIFOIE1_MASK),
			  LPUART_FIFO(id));
}

static lpuart_irq_t lpuart_fifo_irqs_status(uint8_t id)
{
	uint32_t fifo1, fifo2;
	lpuart_irq_t irqs = __raw_readl(LPUART_FIFO(id));

	fifo1 = LPUART_FIFOIF1(irqs);
	fifo2 = LPUART_FIFOIF2(irqs);
	return LPUART_SET_FIFOI1(fifo1) | LPUART_SET_FIFOI2(fifo2);
}

static void lpuart_clear_fifo_irqs(uint8_t id, lpuart_irq_t irqs)
{
	caddr_t fifo = LPUART_FIFO(id);

	if (irqs & LPUART_RXUFI)
		__raw_clearl(LPUART_RXUFE, fifo);
	if (irqs & LPUART_TXOFI)
		__raw_clearl(LPUART_TXOFE, fifo);
}

static void lpuart_fifo_init(uint8_t id)
{
	lpuart_config_fifo(id, 0, 0);
	lpuart_enable_fifo(id);
}

#define lpuart_tx_fifo_empty(id) (!!(lpuart_tx_count(id) == 0))
#define lpuart_rx_fifo_empty(id) (!!(lpuart_rx_count(id) == 0))
#else
#define lpuart_fifo_init(id)			lpuart_disable_fifo(id)
#define lpuart_enable_fifo_irqs(id, irqs)	\
	do { } while (0)
#define lpuart_disable_fifo_irqs(id)		\
	do { } while (0)
#define lpuart_fifo_irqs_status(id)		0
#define lpuart_clear_fifo_irqs(id, irqs)	\
	do { } while (0)
#define lpuart_tx_fifo_empty(id)		\
	(!!(lpuart_irqs_status(id) & LPUART_TDREI))
#define lpuart_rx_fifo_empty(id)		\
	(!(lpuart_irqs_status(id) & LPUART_RDRF))
#endif

void lpuart_enable_irqs(uint8_t id, lpuart_irq_t irqs)
{
	uint32_t baud;
	uint32_t ctrl1, ctrl2, ctrl3;

	baud = LPUART_GET_BAUDI1(irqs);
	__raw_writel_mask(LPUART_BAUDIE(baud),
			  LPUART_BAUDIE(LPUART_BAUDIE_MASK),
			  LPUART_BAUD(id));
	ctrl1 = LPUART_GET_CTRLI1(irqs);
	ctrl2 = LPUART_GET_CTRLI2(irqs);
	ctrl3 = LPUART_GET_CTRLI3(irqs);
	__raw_writel_mask(LPUART_CTRLIE1(ctrl1) |
			  LPUART_CTRLIE2(ctrl2) |
			  LPUART_CTRLIE3(ctrl3),
			  LPUART_CTRLIE1(LPUART_CTRLIE1_MASK) |
			  LPUART_CTRLIE2(LPUART_CTRLIE2_MASK) |
			  LPUART_CTRLIE3(LPUART_CTRLIE3_MASK),
			  LPUART_CTRL(id));
	lpuart_enable_fifo_irqs(id, irqs);
}

void lpuart_disable_irqs(uint8_t id)
{
	__raw_writel_mask(LPUART_BAUDIE(0),
			  LPUART_BAUDIE(LPUART_BAUDIE_MASK),
			  LPUART_BAUD(id));
	__raw_writel_mask(LPUART_CTRLIE1(0) |
			  LPUART_CTRLIE2(0) |
			  LPUART_CTRLIE3(0),
			  LPUART_CTRLIE1(LPUART_CTRLIE1_MASK) |
			  LPUART_CTRLIE2(LPUART_CTRLIE2_MASK) |
			  LPUART_CTRLIE3(LPUART_CTRLIE3_MASK),
			  LPUART_CTRL(id));
	lpuart_disable_fifo_irqs(id);
}

lpuart_irq_t lpuart_irqs_status(uint8_t id)
{
	uint32_t baud;
	uint32_t ctrl1, ctrl2, ctrl3;
	lpuart_irq_t irqs = __raw_readl(LPUART_STAT(id));

	baud = LPUART_BAUDIF(irqs);
	ctrl1 = LPUART_CTRLIF1(irqs);
	ctrl2 = LPUART_CTRLIF2(irqs);
	ctrl3 = LPUART_CTRLIF3(irqs);
	return LPUART_SET_BAUDI1(baud) | LPUART_SET_CTRLI1(ctrl1) |
	       LPUART_SET_CTRLI2(ctrl2) | LPUART_SET_CTRLI3(ctrl3) |
	       lpuart_fifo_irqs_status(id);
}

void lpuart_clear_irqs(uint8_t id, lpuart_irq_t irqs)
{
	caddr_t stat = LPUART_STAT(id);

	if (irqs & LPUART_MA2I)
		__raw_clearl(LPUART_MA2F, stat);
	if (irqs & LPUART_MA1I)
		__raw_clearl(LPUART_MA1F, stat);
	if (irqs & LPUART_PEI)
		__raw_clearl(LPUART_PF, stat);
	if (irqs & LPUART_FEI)
		__raw_clearl(LPUART_FE, stat);
	if (irqs & LPUART_NEI)
		__raw_clearl(LPUART_NF, stat);
	if (irqs & LPUART_ORI)
		__raw_clearl(LPUART_OR, stat);
	if (irqs & LPUART_ILI)
		__raw_clearl(LPUART_IDLE, stat);
	if (irqs & LPUART_LBKDI)
		__raw_clearl(LPUART_LBKDIF, stat);
	if (irqs & LPUART_RXEDGI)
		__raw_clearl(LPUART_RXEDGIF, stat);
	lpuart_clear_fifo_irqs(id, irqs);
}

void lpuart_write_byte(uint8_t id, uint8_t byte)
{
	while (!lpuart_tx_fifo_empty(id));
	__raw_writel(byte, LPUART_DATA(id));
}

uint8_t lpuart_read_byte(uint8_t id)
{
	uint8_t data = __raw_readl(LPUART_DATA(id));
	if (lpuart_7bit_enabled(id))
		data &= 0x7F;
	return data;
}

bool lpuart_ctrl_poll(uint8_t id)
{
	return !lpuart_rx_fifo_empty(id);
}

void lpuart_config_baudrate(uint8_t id, uint32_t src_freq, uint32_t baud)
{
	uint32_t cfg;
	uint16_t sbr, s;
	uint32_t osr, o;
	uint32_t err_tmp, err_tmp1, err;
	uint32_t calc_baud, calc_baud1;

	/* BAUD = HZ(LPUART_CLK) / ((OSR + 1) * SBR)
	 * Calibrate best OSR.
	 * Note, OSR is typically hard-set to 16 in other LPUART
	 * instantiations loop to find the best OSR value possible, one
	 * that generates minimum err iterate through the rest of the
	 * supported values of OSR.
	 */
	err = baud;
	osr = 0;
	sbr = 0;
	for (o = 4; o <= 32; o++) {
		/* calculate the temporary sbr value   */
		s = div32u(src_freq, mul32u(baud, o));
		if (s == 0)
			calc_baud = 0;
		else
			calc_baud = div32u(src_freq, mul32u(o, s));
		calc_baud1 = div32u(src_freq, mul32u(o, s + 1));

		/* Select the better value between sbr and (sbr + 1) */
		err_tmp = baud - calc_baud;
		err_tmp1 = baud - calc_baud1;
		if (err_tmp > err_tmp1) {
			err_tmp = err_tmp1;
			s++;
		}
		if (err_tmp <= err) {
			osr = o;
			sbr = s;
			err = err_tmp;
		}
		if (err == 0)
			break;
	}

	/* error should be less < 3% */
	BUG_ON(err > mul32u(div32u(baud, 100), 3));

	cfg = LPUART_OSR(osr - 1) | LPUART_SBR(sbr);
	/* BOTHEDGE if OSR is 4x-7x */
	if ((osr > 3) && (osr < 8))
		cfg |=  LPUART_BOTHEDGE;

	__raw_writel_mask(cfg,
			  LPUART_OSR(LPUART_OSR_MASK) |
			  LPUART_SBR(LPUART_SBR_MASK) |
			  LPUART_BOTHEDGE,
			  LPUART_BAUD(id));
}

void lpuart_config_params(uint8_t id, uint8_t params)
{
	uint8_t bits = uart_bits(params);
	uint8_t parity = uart_parity(params);
	uint8_t cfg;

	if (bits == 7 && parity != UART_PARITY_NONE)
		bits++;
	if (bits >= 8)
		lpuart_disable_7bit(id);
	else
		lpuart_enable_7bit(id);
	if (bits >= 9)
		__raw_setl(LPUART_M, LPUART_CTRL(id));
	else
		__raw_clearl(LPUART_M, LPUART_CTRL(id));
	if (bits >= 10)
		__raw_setl(LPUART_M10, LPUART_BAUD(id));
	else
		__raw_clearl(LPUART_M10, LPUART_BAUD(id));

	cfg = 0;
	switch (parity) {
	case UART_PARITY_ODD:
		cfg |= LPUART_PT;
	case UART_PARITY_EVEN:
		cfg |= LPUART_PE;
		break;
	}
	__raw_writeb_mask(cfg, LPUART_PE | LPUART_PT, LPUART_CTRL(id));
	if (uart_stopb(params))
		__raw_setl(LPUART_SBNS, LPUART_BAUD(id));
}

void lpuart_ctrl_init(uint8_t id, uint32_t src_freq,
		      uint8_t params, uint32_t baud)
{
	lpuart_reset_uart(id);
	lpuart_disable_uart(id);
	lpuart_config_baudrate(id, src_freq, baud);
	lpuart_config_params(id, params);
#if 0
	/* Set bit count and parity mode. */
		 LPUART_CTRL_M_MASK | LPUART_CTRL_ILT_MASK |
		 LPUART_CTRL_IDLECFG_MASK);
	temp |= 
		LPUART_CTRL_IDLECFG(config->rxIdleConfig) |
	       	LPUART_CTRL_ILT(config->rxIdleType);
#endif
	lpuart_fifo_init(id);
	lpuart_clear_irqs(id, LPUART_IRQS_ALL);
	lpuart_enable_uart(id);
}
