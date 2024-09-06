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
 * @(#)spacemit_espi.c: SpacemiT eSPI implementation
 * $Id: spacemit_espi.c,v 1.1 2023-11-27 16:50:00 zhenglv Exp $
 */
#include <target/espi.h>
#include <target/irq.h>
#include <target/panic.h>
#include <target/cmdline.h>

static uint8_t spacemit_espi_oob_buffer[128];
static uint8_t spacemit_espi_rsp;

uint32_t spacemit_espi_read32(caddr_t reg)
{
	uint32_t val;

	val = __raw_readl(reg);
	con_dbg("spacemit_espi: R %016lx=%08x\n", reg, val);
	return val;
}

void spacemit_espi_write32(uint32_t val, caddr_t reg)
{
	con_dbg("spacemit_espi: W %016lx=%08x\n", reg, val);
	__raw_writel(val, reg);
}

uint8_t spacemit_espi_read8(caddr_t reg)
{
	uint8_t val;

	val = __raw_readb(reg);
	con_dbg("spacemit_espi: R %016lx=%02x\n", reg, val);
	return val;
}

void spacemit_espi_write8(uint8_t val, caddr_t reg)
{
	con_dbg("spacemit_espi: W %016lx=%02x\n", reg, val);
	__raw_writeb(val, reg);
}

static inline void espi_soft_reset(void)
{
	spacemit_espi_set32(ESPI_SW_RST, ESPI_GLOBAL_CONTROL_1);
}

static inline void espi_vw_channel_slave_suspend_enter(void)
{
	spacemit_espi_set32(ESPI_SUS_STAT, ESPI_GLOBAL_CONTROL_1);
}

static inline void espi_vw_channel_slave_suspend_exit(void)
{
	spacemit_espi_clear32(ESPI_SUS_STAT, ESPI_GLOBAL_CONTROL_1);
}

static void spacemit_espi_handle_gpio(uint8_t index)
{
	uint8_t vwire;
	uint8_t grp, dat;
	uint8_t valid, level;
	uint16_t gpio;

	grp = spacemit_espi_vwgpio_index(index);
	dat = spacemit_espi_vwgpio_data(index);
	printf("spacemit_espi: GPIO%d=0x%02x\n", grp, dat);

	valid = HIHALF(dat);
	level = LOHALF(dat);
	for (vwire = 0; vwire < 4; vwire++) {
		gpio = ESPI_VWIRE_GPIO_EXPANDER(grp, vwire);
		if (valid & _BV(vwire))
			espi_set_gpio_expander(gpio, !!(level >> vwire));
	}
}

void spacemit_espi_handle_conirq(void)
{
	int int_sts;
	uint8_t rsp = ESPI_RSP_ACCEPT(0);

	int_sts = __raw_readl(ESPI_SLAVE0_INT_STS);
	__raw_writel(int_sts, ESPI_SLAVE0_INT_STS);

	if (int_sts & ESPI_FLASH_REQ_INT) {
		con_log("spacemit_espi: ESPI_FLASH_REQ_INT\n");
		espi_get_flash();
	}
	if (int_sts & ESPI_RXOOB_INT) {
		con_log("spacemit_espi: ESPI_RXOOB_INT\n");
		espi_get_oob();
	}
	if (int_sts & ESPI_RXMSG_INT) {
		con_log("spacemit_espi: ESPI_RXMSG_INT\n");
		espi_get_msg();
	}
	if (int_sts & ESPI_RXVW_GRP3_INT) {
		con_log("spacemit_espi: ESPI_RXVW_GRP3_INT\n");
		spacemit_espi_handle_gpio(3);
	}
	if (int_sts & ESPI_RXVW_GRP2_INT) {
		con_log("spacemit_espi: ESPI_RXVW_GRP2_INT\n");
		spacemit_espi_handle_gpio(2);
	}
	if (int_sts & ESPI_RXVW_GRP1_INT) {
		con_log("spacemit_espi: ESPI_RXVW_GRP1_INT\n");
		spacemit_espi_handle_gpio(1);
	}
	if (int_sts & ESPI_RXVW_GRP0_INT) {
		con_log("spacemit_espi: ESPI_RXVW_GRP0_INT\n");
		spacemit_espi_handle_gpio(0);
	}
	if (int_sts & ESPI_PROTOCOL_INT) {
		if (int_sts & ESPI_PROTOCOL_ERR_INT)
			con_log("spacemit_espi: ESPI_PROTOCOL_ERR_INT\n");
		if (int_sts & ESPI_RXFLASH_OFLOW_INT)
			con_log("spacemit_espi: ESPI_RXFLASH_OFLOW_INT\n");
		if (int_sts & ESPI_RXMSG_OFLOW_INT)
			con_log("spacemit_espi: ESPI_RXMSG_OFLOW_INT\n");
		if (int_sts & ESPI_RXOOB_OFLOW_INT)
			con_log("spacemit_espi: ESPI_RXOOB_OFLOW_INT\n");
		if (int_sts & ESPI_ILLEGAL_LEN_INT)
			con_log("spacemit_espi: ESPI_ILLEGAL_LEN_INT\n");
		if (int_sts & ESPI_ILLEGAL_TAG_INT)
			con_log("spacemit_espi: ESPI_ILLEGAL_TAG_INT\n");
		if (int_sts & ESPI_UNSUCSS_CPL_INT)
			con_log("spacemit_espi: ESPI_UNSUCSS_CPL_INT\n");
		if (int_sts & ESPI_INVALID_CT_RSP_INT)
			con_log("spacemit_espi: ESPI_INVALID_CP_RSP_INT\n");
		if (int_sts & ESPI_UNKNOWN_RSP_INT)
			con_log("spacemit_espi: ESPI_UNKNOWN_RSP_INT\n");
		if (int_sts & ESPI_CRC_ERR_INT)
			con_log("spacemit_espi: ESPI_CRC_ERR_INT\n");
		if (int_sts & ESPI_WAIT_TIMEOUT_INT)
			con_log("spacemit_espi: ESPI_WAIT_TIMEOUT_INT\n");
		if (int_sts & ESPI_BUS_ERR_INT)
			con_log("spacemit_espi: ESPI_BUS_ERR_INT\n");
		rsp = ESPI_RSP_FATAL_ERROR;
	}
	if (int_sts & ESPI_NON_FATAL_INT) {
		con_log("spacemit_espi: ESPI_NON_FATAL_INT\n");
		rsp = ESPI_RSP_NON_FATAL_ERROR;
	}
	if (int_sts & ESPI_FATAL_ERR_INT) {
		con_log("spacemit_espi: ESPI_FATAL_ERR_INT\n");
		rsp = ESPI_RSP_FATAL_ERROR;
	}
	if (int_sts & ESPI_NO_RSP_INT) {
		con_log("spacemit_espi: ESPI_NO_RSP_INT\n");
		rsp = ESPI_RSP_NO_RESPONSE;
	}
	if (int_sts & ESPI_DNCMD_INT) {
		con_log("spacemit_espi: ESPI_DNCMD_INT\n");
		spacemit_espi_rsp = rsp;
		espi_cmd_complete(rsp);
	}
}

void spacemit_espi_handle_vwirq(void)
{
	uint32_t sts, sys;
	uint8_t irq;

	sts = __raw_readl(ESPI_SLAVE0_RXVW_STS);
	if (sts & SLAVE0_RXVW_STS_SYS_EVT_STS) {
		sys = __raw_readl(ESPI_SLAVE0_RXVW);
		if (sys & SLAVE0_RXVW_HOST_RST_ACK)
			espi_set_sys_event(ESPI_VWIRE_SYSTEM_HOST_RST_ACK);
		else
			espi_clear_sys_event(ESPI_VWIRE_SYSTEM_HOST_RST_ACK);
		if (sys & SLAVE0_RXVW_RCIN)
			espi_set_sys_event(ESPI_VWIRE_SYSTEM_RCIN);
		else
			espi_clear_sys_event(ESPI_VWIRE_SYSTEM_RCIN);
		if (sys & SLAVE0_RXVW_SMI)
			espi_set_sys_event(ESPI_VWIRE_SYSTEM_SMI);
		else
			espi_clear_sys_event(ESPI_VWIRE_SYSTEM_SMI);
		if (sys & SLAVE0_RXVW_SCI)
			espi_set_sys_event(ESPI_VWIRE_SYSTEM_SCI);
		else
			espi_clear_sys_event(ESPI_VWIRE_SYSTEM_SCI);
		if (sys & SLAVE0_RXVW_SLAVE_BOOT_LOAD_STS)
			espi_set_sys_event(ESPI_VWIRE_SYSTEM_SLV_BOOT_LOAD_STATUS);
		else
			espi_clear_sys_event(ESPI_VWIRE_SYSTEM_SLV_BOOT_LOAD_STATUS);
		if (sys & SLAVE0_RXVW_SLAVE_ERROR_NONFATAL)
			espi_set_sys_event(ESPI_VWIRE_SYSTEM_ERROR_NONFATAL);
		else
			espi_clear_sys_event(ESPI_VWIRE_SYSTEM_ERROR_NONFATAL);
		if (sys & SLAVE0_RXVW_SLAVE_ERROR_FATAL)
			espi_set_sys_event(ESPI_VWIRE_SYSTEM_ERROR_FATAL);
		else
			espi_clear_sys_event(ESPI_VWIRE_SYSTEM_ERROR_FATAL);
		if (sys & SLAVE0_RXVW_SLAVE_BOOT_LOAD_DONE)
			espi_set_sys_event(ESPI_VWIRE_SYSTEM_SLV_BOOT_LOAD_DONE);
		else
			espi_clear_sys_event(ESPI_VWIRE_SYSTEM_SLV_BOOT_LOAD_DONE);
		if (sys & SLAVE0_RXVW_PME)
			espi_set_sys_event(ESPI_VWIRE_SYSTEM_PME);
		else
			espi_clear_sys_event(ESPI_VWIRE_SYSTEM_PME);
		if (sys & SLAVE0_RXVW_WAKE)
			espi_set_sys_event(ESPI_VWIRE_SYSTEM_WAKE);
		else
			espi_clear_sys_event(ESPI_VWIRE_SYSTEM_WAKE);
		if (sys & SLAVE0_RXVW_OOB_RST_ACK)
			espi_set_sys_event(ESPI_VWIRE_SYSTEM_OOB_RST_ACK);
		else
			espi_clear_sys_event(ESPI_VWIRE_SYSTEM_OOB_RST_ACK);
		if (sys & SLAVE0_RXVW_DNX_ACK)
			con_log("spacemit_espi: VW_SYS: PWRDN_ACK\n");
		if (sys & SLAVE0_RXVW_SUS_ACK_B)
			con_log("spacemit_espi: VW_SYS: SUS_ACK\n");
	}
	for (irq = 0; irq < 24; irq++) {
		if (spacemit_espi_get_vwirq(irq)) {
			con_log("spacemit_espi: VW_IRQ: %d\n", irq);
			spacemit_espi_ack_vwirq(irq);
		}
	}
}

void spacemit_espi_irq_init(void)
{
	irqc_configure_irq(ESPI_IRQ, 0, IRQ_LEVEL_TRIGGERED);
	irq_register_vector(ESPI_IRQ, spacemit_espi_handle_conirq);
	irqc_enable_irq(ESPI_IRQ);
	spacemit_espi_enable_all_irqs();

	irqc_configure_irq(VWIRE_IRQ, 0, IRQ_LEVEL_TRIGGERED);
	irq_register_vector(VWIRE_IRQ, spacemit_espi_handle_vwirq);
	irqc_enable_irq(VWIRE_IRQ);
	//spacemit_espi_enable_all_vwirqs();
}

void spacemit_espi_handle_irq(void)
{
	spacemit_espi_handle_conirq();
	spacemit_espi_handle_vwirq();
}

void espi_handle_conirq(irq_t irq)
{
	spacemit_espi_handle_conirq();
}

void espi_handle_vwirq(irq_t irq)
{
	spacemit_espi_handle_vwirq();
}

uint8_t spacemit_espi_cmd2dncmd(uint8_t opcode)
{
	uint8_t dncmd;

	switch (opcode) {
	case ESPI_CMD_SET_CONFIGURATION:
		dncmd = ESPI_DNCMD_SET_CONFIGURATION;
		break;
	case ESPI_CMD_GET_CONFIGURATION:
		dncmd = ESPI_DNCMD_GET_CONFIGURATION;
		break;
	case ESPI_CMD_PUT_PC:
		dncmd = ESPI_DNCMD_PR_MESSAGE;
		break;
	case ESPI_CMD_PUT_VWIRE:
		dncmd = ESPI_DNCMD_PUT_VW;
		break;
	case ESPI_CMD_PUT_OOB:
	case ESPI_CMD_GET_OOB:
		dncmd = ESPI_DNCMD_PUT_OOB;
		break;
	case ESPI_CMD_PUT_FLASH_C:
	case ESPI_CMD_GET_FLASH_NP:
		dncmd = ESPI_DNCMD_PUT_FLASH_C;
		break;
	case ESPI_CMD_RESET:
		dncmd = ESPI_DNCMD_IN_BAND_RESET;
		break;
	default:
		dncmd = ESPI_DNCMD_PR_MESSAGE;
		break;
	}
	return dncmd;
}

void spacemit_espi_write_cmd(uint8_t opcode,
			     uint8_t hlen, uint8_t *hbuf,
			     uint16_t dlen, uint8_t *dbuf)
{
	uint8_t i;
	uint8_t dncmd;
	uint32_t txdata;
	uint8_t db0, db1, db2, db3;
	uint16_t len = dlen;
	uint8_t *buf = dbuf;

	dncmd = spacemit_espi_cmd2dncmd(opcode);

	con_dbg("spacemit_espi: cmd: %02x\n", dncmd);
	con_dbg("spacemit_espi: hdr=%d", hlen);
	for (i = 0; i < hlen; i++) {
		if (i == 0)
			con_dbg(",");
		con_dbg("%02x ", ((uint8_t *)hbuf)[i]);
	}
	con_dbg("\n");
	for (i = 0; i < hlen; i++)
		spacemit_espi_write_txhdr(i, hbuf[i]);

	switch (dncmd) {
	case ESPI_DNCMD_PUT_OOB:
		if (dlen < 3) {
			con_err("spacemit_espi: OOB illegal %d\n", dlen);
			return;
		}
		spacemit_espi_write_txhdr(3, dbuf[0]);
		spacemit_espi_write_txhdr(4, dbuf[1]);
		spacemit_espi_write_txhdr(5, dbuf[2]);
		buf = dbuf + 3;
		len = dlen - 3;
		break;
	}

	con_dbg("spacemit_espi: dat=%d", dlen);
	for (i = 0; i < dlen; i++) {
		if (i == 0)
			con_dbg(",");
		con_dbg("%02x ", ((uint8_t *)dbuf)[i]);
	}
	con_dbg("\n");
	for (i = 0; i < ((len + 3) / 4); i++) {
		uint8_t ilen = i * 4;

		if (ilen < len)
			db0 = buf[ilen];
		else
			db0 = 0x00;
		if ((ilen + 1) < len)
			db1 = buf[ilen + 1];
		else
			db1 = 0x00;
		if ((ilen + 2) < len)
			db2 = buf[ilen + 2];
		else
			db2 = 0x00;
		if ((ilen + 3) < len)
			db3 = buf[ilen + 3];
		else
			db3 = 0x00;
		txdata = MAKELONG(MAKEWORD(db0, db1),
				  MAKEWORD(db2, db3));
		spacemit_espi_write32(txdata, ESPI_DN_TXDATA_PORT);
	}
	spacemit_espi_write_dncmd(dncmd, 0);
}

uint8_t spacemit_espi_read_rsp(uint8_t opcode,
			       uint8_t hlen, uint8_t *hbuf,
			       uint16_t dlen, uint8_t *dbuf)
{
	uint8_t dncmd;
	uint32_t i;
	uint32_t len = 0;
	uint8_t *buf = dbuf;

	dncmd = spacemit_espi_cmd2dncmd(opcode);

	con_dbg("spacemit_espi: cmd: %02x, hdr=%d, dat=%d\n", dncmd, hlen, dlen);

	switch (dncmd) {
	case ESPI_DNCMD_GET_CONFIGURATION:
		BUG_ON(hlen < 4);
		hbuf[0] = spacemit_espi_read_txhdr(3);
		hbuf[1] = spacemit_espi_read_txhdr(4);
		hbuf[2] = spacemit_espi_read_txhdr(5);
		hbuf[3] = spacemit_espi_read_txhdr(6);
		return spacemit_espi_rsp;

	case ESPI_DNCMD_PR_MESSAGE:
		BUG_ON(hlen < ESPI_PERI_MESSAGE_HDR_LEN);
		hbuf[0] = spacemit_espi_read_rxmsg(0);
		hbuf[1] = spacemit_espi_read_rxmsg(1);
		hbuf[2] = spacemit_espi_read_rxmsg(2);
		hbuf[3] = spacemit_espi_read_rxmsg(3);
		hbuf[4] = spacemit_espi_read_rxmsg(4);
		hbuf[5] = spacemit_espi_read_rxmsg(5);
		hbuf[6] = spacemit_espi_read_rxmsg(6);
		hbuf[7] = spacemit_espi_read_rxmsg(7);
		len = ESPI_RXHDR_LENGTH(hbuf);
		if (len > dlen) {
			con_err("spacemit_espi: PR oversized %d > %d\n",
				len, dlen);
			spacemit_espi_rsp = ESPI_RSP_NON_FATAL_ERROR;
			goto err_exit;
		}
		dlen = len;
		break;

	case ESPI_DNCMD_PUT_OOB:
		BUG_ON(hlen < ESPI_OOB_MESSAGE_HDR_LEN);
		hbuf[0] = spacemit_espi_read_rxhdr(0);
		hbuf[1] = spacemit_espi_read_rxhdr(1);
		hbuf[2] = spacemit_espi_read_rxhdr(2);
		len = ESPI_RXHDR_LENGTH(hbuf);
		if (len > dlen) {
			con_err("spacemit_espi: OOB oversized %d > %d\n",
				len, dlen);
			spacemit_espi_rsp = ESPI_RSP_NON_FATAL_ERROR;
			goto err_exit;
		}
		if (len < 3) {
			con_err("spacemit_espi: OOB illegal %d < 3\n",
				len);
			spacemit_espi_rsp = ESPI_RSP_NON_FATAL_ERROR;
			goto err_exit;
		}
		dlen = len;
		dbuf[0] = spacemit_espi_read_rxhdr(3);
		dbuf[1] = spacemit_espi_read_rxhdr(4);
		dbuf[2] = spacemit_espi_read_rxhdr(5);
		len = dlen - 3;
		buf = dbuf + 3;
		break;

	case ESPI_CMD_PUT_FLASH_C:
		BUG_ON(hlen < ESPI_FLASH_ACCESS_REQUEST_HDR_LEN);
		hbuf[0] = spacemit_espi_read_rxhdr(0);
		hbuf[1] = spacemit_espi_read_rxhdr(1);
		hbuf[2] = spacemit_espi_read_rxhdr(2);
		hbuf[3] = spacemit_espi_read_rxhdr(3);
		hbuf[4] = spacemit_espi_read_rxhdr(4);
		hbuf[5] = spacemit_espi_read_rxhdr(5);
		hbuf[6] = spacemit_espi_read_rxhdr(6);
		len = ESPI_RXHDR_LENGTH(hbuf);
		if (len > dlen) {
			con_err("spacemit_espi: flash access oversized %d > %d\n",
				len, dlen);
			spacemit_espi_rsp = ESPI_RSP_NON_FATAL_ERROR;
			goto err_exit;
		}
		dlen = len;
		break;
	}
	con_dbg("spacemit_espi: hdr=%d", hlen);
	for (i = 0; i < hlen; i++) {
		if (i == 0)
			con_dbg(",");
		con_dbg("%02x ", ((uint8_t *)hbuf)[i]);
	}
	con_dbg("\n");
	con_dbg("spacemit_espi: dat=%d\n", dlen);
	for (i = 0; i < ((len + 3) / 4); i++) {
		uint8_t ilen = i * 4;
		uint32_t rxdata;

		if (dncmd == ESPI_DNCMD_PR_MESSAGE)
			rxdata = spacemit_espi_read32(ESPI_SLAVE0_RXMSG_DATA_PORT);
		else
			rxdata = spacemit_espi_read32(ESPI_UP_RXDATA_PORT);
		if (ilen < dlen)
			buf[ilen] = LOBYTE(LOWORD(rxdata));
		if ((ilen + 1) < dlen)
			buf[ilen + 1] = HIBYTE(LOWORD(rxdata));
		if ((ilen + 2) < dlen)
			buf[ilen + 2] = LOBYTE(HIWORD(rxdata));
		if ((ilen + 3) < dlen)
			buf[ilen + 3] = HIBYTE(HIWORD(rxdata));
	}

err_exit:
	return spacemit_espi_rsp;
}

void spacemit_espi_set_cfg(uint32_t address, uint32_t config)
{
	switch (address) {
	case ESPI_SLAVE_GEN_CFG:
		if (config & ESPI_GEN_CRC_ENABLE)
			spacemit_espi_set32(ESPI_CRC_CHECK_EN, ESPI_SLAVE0_CONFIG);
		spacemit_espi_config_io_mode(ESPI_GEN_IO_MODE_SEL(config));
		spacemit_espi_config_clk_freq(ESPI_GEN_OP_FREQ_SEL(config));
		if (config & ESPI_GEN_ALERT_MODE_PIN)
			spacemit_espi_set32(ESPI_ALERT_MODE_SEL, ESPI_SLAVE0_CONFIG);
		if (config & ESPI_GEN_PERI_CHAN_SUP)
			spacemit_espi_set32(ESPI_PR_EN, ESPI_SLAVE0_CONFIG);
		if (config & ESPI_GEN_VWIRE_CHAN_SUP)
			spacemit_espi_set32(ESPI_VW_EN, ESPI_SLAVE0_CONFIG);
		if (config & ESPI_GEN_OOB_CHAN_SUP)
			spacemit_espi_set32(ESPI_OOB_EN, ESPI_SLAVE0_CONFIG);
		if (config & ESPI_GEN_FLASH_CHAN_SUP)
			spacemit_espi_set32(ESPI_FLASH_EN, ESPI_SLAVE0_CONFIG);
		break;
	}
}

#ifdef CONFIG_SPACEMIT_ESPI_AUTO_GATING
void spacemit_espi_config_gating(void)
{
	spacemit_espi_clear32(ESPI_MST_STOP_EN, ESPI_GLOBAL_CONTROL_0);
}
#else
#define spacemit_espi_config_gating()	do { } while (0)
#endif

void spacemit_espi_init(void)
{
	spacemit_espi_config_wdg(ESPI_WDG_CNT_MAX);
	spacemit_espi_enable_wdg();
	spacemit_espi_soft_reset();
	spacemit_espi_config_gating();
	spacemit_espi_config_vwgpio(0, 128);
	spacemit_espi_config_vwgpio(1, 129);
	spacemit_espi_config_vwgpio(2, 130);
	spacemit_espi_config_vwgpio(3, 131);
}

static int do_espi_mem(int argc, char *argv[])
{
	uint32_t address0, address1;

	if (argc < 3)
		return -EINVAL;
	address0 = (uint32_t)strtoull(argv[2], 0, 0);
	if (argc > 3)
		address1 = (uint32_t)strtoull(argv[3], 0, 0);
	else
		address1 = address0 + ESPI_MEM_SIZE;
	spacemit_espi_mem_cfg(address0, address1);
	return 0;
}

static int do_espi_wait(int argc, char *argv[])
{
	if (argc < 3)
		return -EINVAL;
	if (!strcmp(argv[2], "on"))
		spacemit_espi_enable_wait();
	else if (!strcmp(argv[2], "off"))
		spacemit_espi_disable_wait();
	else
		return -EINVAL;
	return 0;
}

static int do_espi_wdg(int argc, char *argv[])
{
	if (argc < 3)
		return -EINVAL;
	if (!strcmp(argv[2], "on"))
		spacemit_espi_enable_wdg();
	else if (!strcmp(argv[2], "off"))
		spacemit_espi_disable_wdg();
	else
		return -EINVAL;
	return 0;
}

static int do_espi_irq(int argc, char *argv[])
{
	uint8_t irq;

	if (argc < 4)
		return -EINVAL;
	irq = (uint8_t)strtoull(argv[2], 0, 0);
	if (!strcmp(argv[3], "high"))
		spacemit_espi_vwirq_polarity(irq, 1);
	else if (!strcmp(argv[3], "low"))
		spacemit_espi_vwirq_polarity(irq, 0);
	else
		return -EINVAL;
	return 0;
}

static int do_espi_ltr(int argc, char *argv[])
{
	bool rq;
	uint8_t ls;
	uint16_t lv;

	if (argc < 6)
		return -EINVAL;
	rq = (bool)strtoull(argv[3], 0, 0);
	ls = (uint8_t)strtoull(argv[4], 0, 0);
	lv = (uint16_t)strtoull(argv[5], 0, 0);
	espi_put_ltr(rq, ls, lv);
	return 0;
}

static int do_espi_msg(int argc, char *argv[])
{
	if (argc < 3)
		return -EINVAL;
	if (!strcmp(argv[2], "ltr"))
		return do_espi_ltr(argc, argv);
	return -EINVAL;
}

static int do_espi_oob(int argc, char *argv[])
{
	uint16_t len, i;

	if (argc < 3)
		return -EINVAL;

	len = (uint16_t)strtoull(argv[2], 0, 0);
	if (len > ESPI_HW_OOB_SIZE) {
		printf("put length too long!\n");
		return -EINVAL;
	}
	if (len > argc - 3) {
		printf("put length not match!\n");
		return -EINVAL;
	}
	for (i = 0; i < len; i++) {
		spacemit_espi_oob_buffer[i] = (uint8_t)strtoull(argv[i + 3], 0, 0);
	}
	espi_put_oob(len, spacemit_espi_oob_buffer);
	return 0;
}

static int do_espi_flash(int argc, char *argv[])
{
	uint16_t len, i;
	bool type;
	uint8_t code;

	if (argc < 5)
		return -EINVAL;

	type = (bool)strtoull(argv[2], 0, 0);
	code = (uint8_t)strtoull(argv[3], 0, 0);
	len = (uint16_t)strtoull(argv[4], 0, 0);
	if (len > ESPI_HW_FLASH_SIZE) {
		printf("put length too long!\n");
		return -EINVAL;
	}
	if (len > argc - 5) {
		printf("put length not match!\n");
		return -EINVAL;
	}
	for (i = 0; i < len; i++) {
		spacemit_espi_oob_buffer[i] = (uint8_t)strtoull(argv[i + 5], 0, 0);
	}
	espi_put_flash(type, code, len, spacemit_espi_oob_buffer);
	return 0;
}

static int do_espi(int argc, char *argv[])
{
	if (argc < 2)
		return -EINVAL;
	if (strcmp(argv[1], "mem") == 0)
		return do_espi_mem(argc, argv);
	else if (strcmp(argv[1], "wdg") == 0)
		return do_espi_wdg(argc, argv);
	else if (strcmp(argv[1], "wait") == 0)
		return do_espi_wait(argc, argv);
	else if (strcmp(argv[1], "irq") == 0)
		return do_espi_irq(argc, argv);
	else if (strcmp(argv[1], "msg") == 0)
		return do_espi_msg(argc, argv);
	else if (strcmp(argv[1], "oob") == 0)
		return do_espi_oob(argc, argv);
	else if (strcmp(argv[1], "flash") == 0)
		return do_espi_flash(argc, argv);
	return -EINVAL;
}

DEFINE_COMMAND(spacemit_espi, do_espi, "SpacemiT enhanced SPI commands",
	"spacemit_espi mem <address0> [address1]\n"
	"    -config eSPI memory translation\n"
	"spacemit_espi wdg <on|off>\n"
	"    -enable/disable watchdog counter\n"
	"spacemit_espi wait <on|off>\n"
	"    -enable/disable wait counter\n"
	"spacemit_espi irq <irq> <high|low>\n"
	"    -config irq polarity\n"
	"spacemit_espi msg ltr <rq> <ls> <lv>...\n"
	"    -put LTR message\n"
	"    -rq: service requirment:\n"
	"         0 - not required\n"
	"         1 - required\n"
	"    -ls: latency scale:\n"
	"         0 - 1ns\n"
	"         1 - 32ns\n"
	"         2 - 1024ns\n"
	"         3 - 32768ns\n"
	"         4 - 1048576ns\n"
	"         5 - 33554432ns\n"
	"    -lv: latency value\n"
	"spacemit_espi oob <len> [byte1] [byte2] ...\n"
	"    -put OOB message\n"
	"spacemit_espi flash <type> <code> <len> [byte1] [byte2] ...\n"
	"    -put flash message\n"
	"    -type: 0 - successful, 1 - unsuccessful\n"
	"    -code: 0 - middle, 1 - first, 2 - last, 3 - only\n"
);
