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
 * @(#)spacemit_espi.h: SpacemiT eSPI interface
 * $Id: spacemit_espi.h,v 1.1 2023-11-27 16:50:00 zhenglv Exp $
 */

#ifndef __SPACEMIT_ESPI_H_INCLUDE__
#define __SPACEMIT_ESPI_H_INCLUDE__

#include <target/arch.h>
#include <asm/mach/espi.h>

#ifndef SPACEMIT_ESPI_REG
#define SPACEMIT_ESPI_REG(offset)		(SPACEMIT_ESPI_BASE + (offset))
#endif

/* Register offset */
#define ESPI_DN_TXHDR				SPACEMIT_ESPI_REG(0x00)
#define ESPI_DN_TXHDRn(n)			SPACEMIT_ESPI_REG(0x00 + (REG32_8BIT_INDEX((n) + 1) << 2))
#define ESPI_DN_TXDATA_PORT			SPACEMIT_ESPI_REG(0x0C)
#define ESPI_UP_RXHDR				SPACEMIT_ESPI_REG(0x10)
#define ESPI_UP_RXHDRn(n)			SPACEMIT_ESPI_REG(0x10 + (REG32_8BIT_INDEX((n) + 1) << 2))
#define ESPI_UP_RXDATA_PORT			SPACEMIT_ESPI_REG(0x18)
#define ESPI_MASTER_CAP				SPACEMIT_ESPI_REG(0x2C)
#define ESPI_GLOBAL_CONTROL_0			SPACEMIT_ESPI_REG(0x30)
#define ESPI_GLOBAL_CONTROL_1			SPACEMIT_ESPI_REG(0x34)
#define ESPI_PR_BASE_ADDR_MEM0			SPACEMIT_ESPI_REG(0x38)
#define ESPI_PR_BASE_ADDR_MEM1			SPACEMIT_ESPI_REG(0x3C)
#define ESPI_SLAVE0_STS_SHADOW			SPACEMIT_ESPI_REG(0x44)
#define ESPI_SLAVE0_CONFIG			SPACEMIT_ESPI_REG(0x68)
#define ESPI_SLAVE0_INT_EN			SPACEMIT_ESPI_REG(0x6C)
#define ESPI_SLAVE0_INT_STS			SPACEMIT_ESPI_REG(0x70)
#define ESPI_SLAVE0_RXMSG_HDRn(n)		SPACEMIT_ESPI_REG(0x74 + (REG32_8BIT_INDEX((n)) << 2))
#define ESPI_SLAVE0_RXMSG_DATA_PORT		SPACEMIT_ESPI_REG(0x7C)
#define ESPI_SLAVE0_RXVW_STS			SPACEMIT_ESPI_REG(0x98)
#define ESPI_SLAVE0_RXVW			SPACEMIT_ESPI_REG(0x9C)
#define ESPI_SLAVE0_RXVW_DATA			SPACEMIT_ESPI_REG(0xA0)
#define ESPI_SLAVE0_RXVW_INDEX			SPACEMIT_ESPI_REG(0xA4)
#define ESPI_SLAVE0_VW_CTL			SPACEMIT_ESPI_REG(0xA8)
#define ESPI_SLAVE0_VW_POLARITY			SPACEMIT_ESPI_REG(0xAC)

/* DN_TXHDR_0
 * DN_TXHDR_1
 * DN_TXHDR_2
 */
#define ESPI_DNCMD_HDATA_OFFSET(n)		REG32_8BIT_OFFSET((n) + 1)
#define ESPI_DNCMD_HDATA_MASK			REG_8BIT_MASK
#define ESPI_DNCMD_HDATA(n, value)		_SET_FVn(n, ESPI_DNCMD_HDATA, value)
#define espi_dncmd_hdata(n, value)		_GET_FVn(n, ESPI_DNCMD_HDATA, value)
/* DN_TXHDR_0 */
#define ESPI_DNCMD_SLAVE_SEL_OFFSET		4
#define ESPI_DNCMD_SLAVE_SEL_MASK		REG_2BIT_MASK
#define ESPI_DNCMD_SLAVE_SEL(value)		_SET_FV(ESPI_DNCMD_SLAVE_SEL, value)
#define ESPI_DNCMD_EN				_BV(3)
#define ESPI_DNCMD_TYPE_OFFSET			0
#define ESPI_DNCMD_TYPE_MASK			REG_3BIT_MASK
#define ESPI_DNCMD_TYPE(value)			_SET_FV(ESPI_DNCMD_TYPE, value)
#define ESPI_DNCMD_SET_CONFIGURATION		0x00
#define ESPI_DNCMD_GET_CONFIGURATION		0x01
#define ESPI_DNCMD_IN_BAND_RESET		0x02
#define ESPI_DNCMD_PR_MESSAGE			0x04
#define ESPI_DNCMD_PUT_VW			0x05
#define ESPI_DNCMD_PUT_OOB			0x06
#define ESPI_DNCMD_PUT_FLASH_C			0x07

/* UP_RXHDR_0
 * UP_RXHDR_1
 */
#define ESPI_UPCMD_HDATA_OFFSET(n)		REG32_8BIT_OFFSET((n) + 1)
#define ESPI_UPCMD_HDATA_MASK			REG_8BIT_MASK
#define ESPI_UPCMD_HDATA(n, value)		_GET_FVn(n, ESPI_UPCMD_HDATA, value)
/* UP_RXHDR_0 */
#define ESPI_UPCMD_SLAVE_SEL_OFFSET		4
#define ESPI_UPCMD_SLAVE_SEL_MASK		REG_2BIT_MASK
#define ESPI_UPCMD_SLAVE_SEL(value)		_GET_FV(ESPI_UPCMD_SLAVE_SEL, value)
#define ESPI_UPCMD_STATUS			_BV(3)
#define ESPI_UPCMD_TYPE_OFFSET			0
#define ESPI_UPCMD_TYPE_MASK			REG_3BIT_MASK
#define ESPI_UPCMD_TYPE(value)			_GET_FV(ESPI_UPCMD_TYPE, value)

/* SLAVE0_RXMSG_HDR0
 * SLAVE0_RXMSG_HDR1
 */
#define ESPI_RXMSG_HDATA_OFFSET(n)		REG32_8BIT_OFFSET(n)
#define ESPI_RXMSG_HDATA_MASK			REG_8BIT_MASK
#define ESPI_RXMSG_HDATA(n, value)		_SET_FVn(n, ESPI_RXMSG_HDATA, value)
#define espi_rxmsg_hdata(n, value)		_GET_FVn(n, ESPI_RXMSG_HDATA, value)

/* MASTER_CAP */
#define ESPI_CRC_CHECK_SUPPORT			_BV(31)
#define ESPI_ALERT_MODE_SUPPORT			_BV(30)
#define ESPI_IO_MODE_SUPPORT_OFFSET		28
#define ESPI_IO_MODE_SUPPORT_MASK		REG_2BIT_MASK
#define ESPI_IO_MODE_SUPPORT(n)			_SET_FV(ESPI_IO_MODE_SUPPORT, n)
#define ESPI_CLK_FREQ_SUPPORT_OFFSET		25
#define ESPI_CLK_FREQ_SUPPORT_MASK		REG_3BIT_MASK
#define ESPI_CLK_FREQ_SUPPORT(n)		_SET_FV(ESPI_CLK_FREQ_SUPPORT, n)
#define ESPI_SLAVE_NUM_OFFSET			22
#define ESPI_SLAVE_NUM_MASK			REG_3BIT_MASK
#define ESPI_SLAVE_NUM(n)			_SET_FV(ESPI_SLAVE_NUM, n)
#define ESPI_PR_MAX_SIZE_OFFSET			19
#define ESPI_PR_MAX_SIZE_MASK			REG_3BIT_MASK
#define ESPI_PR_MAX_SIZE(n)			_SET_FV(ESPI_PR_MAX_SIZE, n)
#define ESPI_VW_MAX_SIZE_OFFSET			13
#define ESPI_VW_MAX_SIZE_MASK			REG_6BIT_MASK
#define ESPI_VW_MAX_SIZE(n)			_SET_FV(ESPI_VW_MAX_SIZE, n)
#define ESPI_OOB_MAX_SIZE_OFFSET		10
#define ESPI_OOB_MAX_SIZE_MASK			REG_3BIT_MASK
#define ESPI_OOB_MAX_SIZE(n)			_SET_FV(ESPI_OOB_MAX_SIZE, n)
#define ESPI_FLASH_MAX_SIZE_OFFSET		7
#define ESPI_FLASH_MAX_SIZE_MASK		REG_3BIT_MASK
#define ESPI_FLASH_MAX_SIZE(n)			_SET_FV(ESPI_FLASH_MAX_SIZE, n)
#define ESPI_ESPI_VERSION_OFFSET		4
#define ESPI_ESPI_VERSION_MASK			REG_3BIT_MASK
#define ESPI_ESPI_VERSION(n)			_SET_FV(ESPI_ESPI_VERSION, n)
#define ESPI_PR_SUPPORT				_BV(3)
#define ESPI_VW_SUPPORT				_BV(2)
#define ESPI_OOB_SUPPORT			_BV(1)
#define ESPI_FLASH_SUPPORT			_BV(0)

/* GLOBAL_CONTROL_0 */
#define ESPI_WAIT_CNT_OFFSET			24
#define ESPI_WAIT_CNT_MASK			REG_6BIT_MASK
#define ESPI_WAIT_CNT(n)			_SET_FV(ESPI_WAIT_CNT, n)
#define ESPI_WDG_CNT_OFFSET			8
#define ESPI_WDG_CNT_MASK			REG_16BIT_MASK
#define ESPI_WDG_CNT_MAX			REG_16BIT_MASK
#define ESPI_WDG_CNT(n)				_SET_FV(ESPI_WDG_CNT, n)
#define ESPI_MST_IDLE_CNT_OFFSET		4
#define ESPI_MST_IDLE_CNT_MASK			REG_3BIT_MASK
#define ESPI_MST_IDLE_CNT(n)			_SET_FV(ESPI_MST_IDLE_CNT, n)
#define ESPI_MST_STOP_EN			_BV(3)
#define ESPI_WAIT_CHK_EN			_BV(1)
#define ESPI_WDG_EN				_BV(0)

#define spacemit_espi_config_wait(cnt)			\
	spacemit_espi_write32_mask(ESPI_WAIT_CNT(cnt),	\
		ESPI_WAIT_CNT(ESPI_WAIT_CNT_MASK),	\
		ESPI_GLOBAL_CONTROL_0)
#define spacemit_espi_enable_wait()			\
	spacemit_espi_set32(ESPI_WAIT_CHK_EN, ESPI_GLOBAL_CONTROL_0)
#define spacemit_espi_disable_wait()			\
	spacemit_espi_clear32(ESPI_WAIT_CHK_EN, ESPI_GLOBAL_CONTROL_0)
#define spacemit_espi_config_wdg(cnt)			\
	spacemit_espi_write32_mask(ESPI_WDG_CNT(cnt),	\
		ESPI_WDG_CNT(ESPI_WDG_CNT_MASK),	\
		ESPI_GLOBAL_CONTROL_0)
#define spacemit_espi_enable_wdg()			\
	spacemit_espi_set32(ESPI_WDG_EN, ESPI_GLOBAL_CONTROL_0)
#define spacemit_espi_disable_wdg()			\
	spacemit_espi_clear32(ESPI_WDG_EN, ESPI_GLOBAL_CONTROL_0)

/* GLOBAL_CONTROL_1 */
#define ESPI_NP_FORCE_EN			_BV(7)
#define ESPI_SUS_STAT				_BV(6)
#define ESPI_ESPI_RSTN				_BV(5)
#define ESPI_SAMPLE_SEL_OFFSET			3
#define ESPI_SAMPLE_SEL_MASK			REG_2BIT_MASK
#define ESPI_SAMPLE_SEL(value)			_SET_FV(ESPI_SAMPLE_SEL, value)
#define ESPI_SW_RST				_BV(0)

/* SLAVE0_STS_SHADOW */
#define ESPI_STS_SHADOW_OFFSET			16
#define ESPI_STS_SHADOW_MASK			REG_16BIT_MASK
#define ESPI_STS_SHADOW(n)			_SET_FV(ESPI_STS_SHADOW, n)
#define ESPI_STS_SHADOW_EN			_BV(0)

/* SLAVE0_CONFIG */
#define ESPI_CRC_CHECK_EN			_BV(31)
#define ESPI_ALERT_MODE_SEL			_BV(30)
#define ESPI_IO_MODE_SEL_OFFSET			28
#define ESPI_IO_MODE_SEL_MASK			REG_2BIT_MASK
#define ESPI_IO_MODE_SEL(n)			_SET_FV(ESPI_IO_MODE_SEL, n)
#define ESPI_CLK_FREQ_SEL_OFFSET		25
#define ESPI_CLK_FREQ_SEL_MASK			REG_3BIT_MASK
#define ESPI_CLK_FREQ_SEL(n)			_SET_FV(ESPI_CLK_FREQ_SEL, n)
#define ESPI_PR_EN				_BV(3)
#define ESPI_VW_EN				_BV(2)
#define ESPI_OOB_EN				_BV(1)
#define ESPI_FLASH_EN				_BV(0)

/* PR_CONFIG */
#define PR_CONFIG_PR_SWITCH			_BV(31)
#define PR_CONFIG_PR_BUSY			_BV(30)
#define PR_CONFIG_PR_WR_BUSY			_BV(29)
#define PR_CONFIG_PR_RD_BUSY			_BV(28)
#define PR_CONFIG_RX_OVERFLOW			_BV(25)
#define PR_CONFIG_RX_DATA_NUM_OFFSET		21
#define PR_CONFIG_RX_DATA_NUM_MASK		REG_4BIT_MASK
#define PR_CONFIG_RX_DATA_NUM(n)		_SET_FV(PR_CONFIG_RX_DATA_NUM, n)
#define PR_CONFIG_TX_OVERFLOW			_BV(20)
#define PR_CONFIG_TX_DATA_NUM_OFFSET		16
#define PR_CONFIG_TX_DATA_NUM_MASK		REG_4BIT_MASK
#define PR_CONFIG_TX_DATA_NUM(n)		_SET_FV(PR_CONFIG_TX_DATA_NUM, n)
#define PR_CONFIG_PR_CNT_OFFSET			8
#define PR_CONFIG_PR_CNT_MASK			REG_7BIT_MASK
#define PR_CONFIG_PR_CNT(n)			_SET_FV(PR_CONFIG_PR_CNT, n)
#define PR_CONFIG_PR_IO_MEM_SEL			_BV(2)
#define PR_CONFIG_PR_WR_RD			_BV(1)
#define PR_CONFIG_PR_TRANS_EN			_BV(0)

/* SLAVE0_INT_EN
 * SLAVE0_INT_STS
 */
#define ESPI_FLASH_REQ_INT			_BV(31)
#define ESPI_RXOOB_INT				_BV(30)
#define ESPI_RXMSG_INT				_BV(29)
#define ESPI_DNCMD_INT				_BV(28)
#define ESPI_RXVW_GRP3_INT			_BV(27)
#define ESPI_RXVW_GRP2_INT			_BV(26)
#define ESPI_RXVW_GRP1_INT			_BV(25)
#define ESPI_RXVW_GRP0_INT			_BV(24)
#define ESPI_CALLBACK_INT			\
	(ESPI_FLASH_REQ_INT |			\
	 ESPI_RXOOB_INT |			\
	 ESPI_RXMSG_INT |			\
	 ESPI_DNCMD_INT |			\
	 ESPI_RXVW_GRP3_INT |			\
	 ESPI_RXVW_GRP2_INT |			\
	 ESPI_RXVW_GRP1_INT |			\
	 ESPI_RXVW_GRP0_INT)
#define ESPI_PROTOCOL_ERR_INT			_BV(15)
#define ESPI_RXFLASH_OFLOW_INT			_BV(14)
#define ESPI_RXMSG_OFLOW_INT			_BV(13)
#define ESPI_RXOOB_OFLOW_INT			_BV(12)
#define ESPI_ILLEGAL_LEN_INT			_BV(11)
#define ESPI_ILLEGAL_TAG_INT			_BV(10)
#define ESPI_UNSUCSS_CPL_INT			_BV(9)
#define ESPI_INVALID_CT_RSP_INT			_BV(8)
#define ESPI_UNKNOWN_RSP_INT			_BV(7)
#define ESPI_CRC_ERR_INT			_BV(2)
#define ESPI_WAIT_TIMEOUT_INT			_BV(1)
#define ESPI_BUS_ERR_INT			_BV(0)
#define ESPI_PROTOCOL_INT			\
	(ESPI_PROTOCOL_ERR_INT |		\
	 ESPI_RXFLASH_OFLOW_INT |		\
	 ESPI_RXMSG_OFLOW_INT |			\
	 ESPI_RXOOB_OFLOW_INT |			\
	 ESPI_ILLEGAL_LEN_INT |			\
	 ESPI_ILLEGAL_TAG_INT |			\
	 ESPI_UNSUCSS_CPL_INT |			\
	 ESPI_INVALID_CT_RSP_INT |		\
	 ESPI_UNKNOWN_RSP_INT |			\
	 ESPI_CRC_ERR_INT |			\
	 ESPI_WAIT_TIMEOUT_INT |		\
	 ESPI_BUS_ERR_INT)
#define ESPI_NON_FATAL_INT			_BV(6)
#define ESPI_FATAL_ERR_INT			_BV(5)
#define ESPI_NO_RSP_INT				_BV(4)
#define ESPI_RESPONSE_INT			\
	(ESPI_NON_FATAL_INT |			\
	 ESPI_FATAL_ERR_INT |			\
	 ESPI_NO_RSP_INT)
#define ESPI_ALL_INT				\
	(ESPI_CALLBACK_INT |			\
	 ESPI_PROTOCOL_INT |			\
	 ESPI_RESPONSE_INT)

/* SLAVE0_RXVW_STS */
#define SLAVE0_RXVW_STS_SYS_EVT_STS		_BV(29)
#define SLAVE0_RXVW_STS_IRQ_STS(n)		_BV(n)

/* SLAVE0_RXVW */
#define SLAVE0_RXVW_HOST_RST_ACK		_BV(19)
#define SLAVE0_RXVW_RCIN			_BV(18)
#define SLAVE0_RXVW_SMI				_BV(17)
#define SLAVE0_RXVW_SCI				_BV(16)
#define SLAVE0_RXVW_SLAVE_BOOT_LOAD_STS		_BV(15)
#define SLAVE0_RXVW_SLAVE_ERROR_NONFATAL	_BV(14)
#define SLAVE0_RXVW_SLAVE_ERROR_FATAL		_BV(13)
#define SLAVE0_RXVW_SLAVE_BOOT_LOAD_DONE	_BV(12)
#define SLAVE0_RXVW_PME				_BV(11)
#define SLAVE0_RXVW_WAKE			_BV(10)
#define SLAVE0_RXVW_OOB_RST_ACK			_BV(8)
#define SLAVE0_RXVW_DNX_ACK			_BV(1)
#define SLAVE0_RXVW_SUS_ACK_B			_BV(0)

/* SLAVE0_RXVW_INDEX
 * SLAVE0_RXVW_DATA
 */
#define SLAVE0_RXVW_GRP_OFFSET(n)		REG32_8BIT_OFFSET(n)
#define SLAVE0_RXVW_GRP_MASK			REG_8BIT_MASK
#define SLAVE0_RXVW_GRP(n, value)		_SET_FVn(n, SLAVE0_RXVW_GRP, value)
#define slave0_rxvw_grp(n, value)		_GET_FVn(n, SLAVE0_RXVW_GRP, value)

/* SLAVE0_VW_CTL */
#define SLAVE0_VW_CTL_IRQ_MASK(n)		_BV(n + 8)
#define SLAVE0_VW_CTRL_ALL_IRQS			(REG_24BIT_MASK << 8)
#define SLAVE0_VW_CTL_SUS_STAT_VMEN		_BV(4)
#define SLAVE0_VW_CTL_GRP_EN(n)			_BV(n)
#define SLAVE0_VW_CTRL_ALL_GRPS			REG_4BIT_MASK
#define VW_ALL_IRQS				\
	(SLAVE0_VW_CTL_SUS_STAT_VMEN |		\
	 SLAVE0_VW_CTRL_ALL_GRPS |		\
	 SLAVE0_VW_CTRL_ALL_IRQS)


/* SLAVE0_VW_POLARITY */
#define SLAVE0_VW_POLARITY_IRQ_POLARITY(n)	_BV(n)

#define ESPI_DECODE_RANGES_PER_REG_GROUP	4

/* eSPI MMIO base lives at an offset of 0x10000 from the address in SPI BAR. */
#define ESPI_OFFSET_FROM_BAR			0x10000

#define ESPI_DECODE				0x40

#define  ESPI_DECODE_IO_0x80_EN			(1 << 2)
#define  ESPI_DECODE_IO_0X60_0X64_EN		(1 << 1)
#define  ESPI_DECODE_IO_0X2E_0X2F_EN		(1 << 0)

#define ESPI_GENERIC_IO_WIN_COUNT		4
#define ESPI_GENERIC_MMIO_WIN_COUNT		4

#define ESPI_GENERIC_IO_MAX_WIN_SIZE		0x100
#define ESPI_GENERIC_MMIO_MAX_WIN_SIZE		0x10000

#define ESPI_CYCLE_TYPE_OOB_MESSAGE		0x21

/*
 * Internally the SoC uses active low signals for the IRQs. This means what when
 * the eSPI controller comes out of reset, it's driving its IRQ lines high.
 * In order to avoid any spurious interrupts the IO-APIC must be configured to
 * trigger on active low signals. The PIC is only capable of triggering on
 * active high signals so the hardware has an inverter that converts the signals
 * before they feed into the PIC.
 *
 * +----------+         Active Low
 * |          |             |            +--------+
 * | IO-APIC  <------+      |      +-----+ LPC    |
 * |          |      |  <---+--->  |     +--------+
 * +----------+      |             |
 *                   |   +-----+   |     +--------+
 *                   +---+ AND <---+-----+ eSPI   |
 *                   |   +-----+   |     +--------+
 * +----------+      |             |
 * |          |   +--v--+          |     +--------+
 * |   PIC    <---+ NOT |          +-----+ PIR    |
 * |          | ^ +-----+                +--------+
 * +----------+ |
 *              | Active High
 *
 * The eSPI controller has an inverter that is applied to incoming Virtual Wire
 * IRQ messages. This allows eSPI peripherals to use active high signaling.
 * If the peripheral uses active low signaling like the SoC does internally, the
 * inverter can be disabled.
 *
 * The polarity register has the following behavior:
 *   0: Invert the incoming VW IRQ before outputting to the AND gates.
 *   1: Do not invert the incoming VW IRQ, but route it directly to the AND
 *      gates.
 */
#define ESPI_VW_IRQ_LEVEL_HIGH(x)		(0 << (x))
#define ESPI_VW_IRQ_LEVEL_LOW(x)		(1 << (x))
#define ESPI_VW_IRQ_EDGE_HIGH(x)		(0 << (x))
#define ESPI_VW_IRQ_EDGE_LOW(x)			(1 << (x))

/* Capabilities of the channels */
#define SPACEMIT_ESPI_VW_COUNT			16
#define SPACEMIT_ESPI_OOB_SIZE			128
#define SPACEMIT_ESPI_FLASH_SIZE		128
#define SPACEMIT_ESPI_PR_SIZE			64
#define SPACEMIT_ESPI_GPIO_GROUPS		4

enum espi_upcmd_type {
	ESPI_UPCMD_FLASH_CHANNEL = 0,
	ESPI_UPCMD_OOB_CHANNEL = 1,
};

struct espi_config {
	/* Bitmap for standard IO decodes. Use ESPI_DECODE_IO_* above. */
	uint32_t std_io_decode_bitmap;

	struct {
		uint16_t base;
		size_t size;
	} generic_io_range[ESPI_GENERIC_IO_WIN_COUNT];

	uint32_t subtractive_decode:1;

	/* Use ESPI_VW_IRQ_* above */
	uint32_t vw_irq_polarity;
};

uint32_t spacemit_espi_read32(caddr_t reg);
void spacemit_espi_write32(uint32_t val, caddr_t reg);
uint8_t spacemit_espi_read8(caddr_t reg);
void spacemit_espi_write8(uint8_t val, caddr_t reg);
#define spacemit_espi_set32(v,a)				\
	do {							\
		uint32_t __v = spacemit_espi_read32(a);		\
		__v |= (v);					\
		spacemit_espi_write32(__v, (a));		\
	} while (0)
#define spacemit_espi_clear32(v,a)				\
	do {							\
		uint32_t __v = spacemit_espi_read32(a);		\
		__v &= ~(v);					\
		spacemit_espi_write32(__v, (a));		\
	} while (0)
#define spacemit_espi_write32_mask(v,m,a)			\
	do {							\
		uint32_t __v = spacemit_espi_read32(a);		\
		__v &= ~(m);					\
		__v |= (v);					\
		spacemit_espi_write32(__v, (a));		\
	} while (0)

#define ESPI_DNCMD_SLAVE_SEL_OFFSET		4
#define ESPI_DNCMD_SLAVE_SEL_MASK		REG_2BIT_MASK
#define ESPI_DNCMD_SLAVE_SEL(value)		_SET_FV(ESPI_DNCMD_SLAVE_SEL, value)
#define ESPI_DNCMD_EN				_BV(3)
#define ESPI_DNCMD_TYPE_OFFSET			0
#define ESPI_DNCMD_TYPE_MASK			REG_3BIT_MASK
#define ESPI_DNCMD_TYPE(value)			_SET_FV(ESPI_DNCMD_TYPE, value)
#define spacemit_espi_write_dncmd(type, slave)				\
	spacemit_espi_write32_mask(ESPI_DNCMD_TYPE(type) |		\
		ESPI_DNCMD_EN | ESPI_DNCMD_SLAVE_SEL(slave),		\
		ESPI_DNCMD_TYPE(ESPI_DNCMD_TYPE_MASK) | ESPI_DNCMD_EN |	\
		ESPI_DNCMD_SLAVE_SEL(ESPI_DNCMD_SLAVE_SEL_MASK),	\
		ESPI_DN_TXHDR)
#define spacemit_espi_write_txhdr(n, byte)			\
	spacemit_espi_write32_mask(ESPI_DNCMD_HDATA(n, byte),	\
		ESPI_DNCMD_HDATA(n, REG_8BIT_MASK),		\
		ESPI_DN_TXHDRn(n))
#define spacemit_espi_read_txhdr(n)				\
	((uint8_t)espi_dncmd_hdata(n,				\
	       spacemit_espi_read32(ESPI_DN_TXHDRn(n))))
#define spacemit_espi_write_done()				\
	(!(spacemit_espi_read32(ESPI_DN_TXHDR) & ESPI_DNCMD_EN))
#define spacemit_espi_read_rxhdr(n)				\
	((uint8_t)ESPI_UPCMD_HDATA(n,				\
	       spacemit_espi_read32(ESPI_UP_RXHDRn(n))))
#define spacemit_espi_read_rxmsg(n)				\
	((uint8_t)espi_rxmsg_hdata(n,				\
	       spacemit_espi_read32(ESPI_SLAVE0_RXMSG_HDRn(n))))

#define spacemit_espi_config_io_mode(mode)			\
	spacemit_espi_write32_mask(ESPI_IO_MODE_SEL(mode),	\
		ESPI_IO_MODE_SEL(ESPI_IO_MODE_SEL_MASK),	\
		ESPI_SLAVE0_CONFIG)
#define spacemit_espi_config_clk_freq(freq)			\
	spacemit_espi_write32_mask(ESPI_CLK_FREQ_SEL(freq),	\
		ESPI_CLK_FREQ_SEL(ESPI_CLK_FREQ_SEL_MASK),	\
		ESPI_SLAVE0_CONFIG)

#define espi_setup_pr_mem_base0(base)	\
	spacemit_espi_write32(base, ESPI_PR_BASE_ADDR_MEM0)
#define espi_setup_pr_mem_base1(base)	\
	spacemit_espi_write32(base, ESPI_PR_BASE_ADDR_MEM1)
#define spacemit_espi_enable_all_irqs()	\
	spacemit_espi_write32(ESPI_ALL_INT, ESPI_SLAVE0_INT_EN)

#define spacemit_espi_hard_reset()				\
	do {							\
		spacemit_espi_clear32(ESPI_ESPI_RSTN,		\
				      ESPI_GLOBAL_CONTROL_1);	\
		spacemit_espi_set32(ESPI_ESPI_RSTN,		\
			            ESPI_GLOBAL_CONTROL_1);	\
	} while (0)
#define spacemit_espi_soft_reset()					\
	do {								\
		spacemit_espi_set32(ESPI_SW_RST, ESPI_GLOBAL_CONTROL_1);\
		while (spacemit_espi_read32(ESPI_GLOBAL_CONTROL_1) &	\
		       ESPI_SW_RST);					\
	} while (0);

#define spacemit_espi_flash_max_size()				\
	ESPI_FLASH_MAX_SIZE(spacemit_espi_read32(ESPI_MASTER_CAP))
#define spacemit_espi_oob_max_size()				\
	ESPI_OOB_MAX_SIZE(spacemit_espi_read32(ESPI_MASTER_CAP))
#define spacemit_espi_vw_max_count()				\
	ESPI_VW_MAX_SIZE(spacemit_espi_read32(ESPI_MASTER_CAP))
#define spacemit_espi_pr_max_size()				\
	ESPI_PR_MAX_SIZE(spacemit_espi_read32(ESPI_MASTER_CAP))

#define spacemit_espi_get_vwirq(irq)				(__raw_readl(ESPI_SLAVE0_RXVW_STS) & _BV(irq))
#define spacemit_espi_ack_vwirq(irq)				__raw_writel(_BV(irq), ESPI_SLAVE0_RXVW_STS)
#define spacemit_espi_mask_vwirq(irq)				spacemit_espi_set32(SLAVE0_VW_CTL_IRQ_MASK(irq), ESPI_SLAVE0_VW_CTL)
#define spacemit_espi_unmask_vwirq(irq)				spacemit_espi_clear32(SLAVE0_VW_CTL_IRQ_MASK(irq), ESPI_SLAVE0_VW_CTL)
#define spacemit_espi_enable_all_vwirqs()			\
	spacemit_espi_write32((uint32_t)VW_ALL_IRQS, ESPI_SLAVE0_VW_CTL)
/* polarity = 1 means HIGH */
#define spacemit_espi_vwirq_polarity(irq, polarity)		\
	do {							\
		if (polarity)					\
			spacemit_espi_set32(_BV(irq),		\
				ESPI_SLAVE0_VW_POLARITY);	\
		else						\
			spacemit_espi_clear32(_BV(irq),		\
				ESPI_SLAVE0_VW_POLARITY);	\
	} while (0)
#define spacemit_espi_enable_vwgpio(group)			spacemit_espi_set32(SLAVE0_VW_CTL_GRP_EN(group), ESPI_SLAVE0_VW_CTL)
#define spacemit_espi_disable_vwgpio(group)			spacemit_espi_clear32(SLAVE0_VW_CTL_GRP_EN(group), ESPI_SLAVE0_VW_CTL)
#define spacemit_espi_config_vwgpio(group, index)				\
	do {									\
		spacemit_espi_write32_mask(SLAVE0_RXVW_GRP(group, index),	\
			SLAVE0_RXVW_GRP(group, SLAVE0_RXVW_GRP_MASK), 		\
			ESPI_SLAVE0_RXVW_INDEX);				\
		spacemit_espi_enable_vwgpio(group);				\
	} while (0)
#ifdef CONFIG_SPACEMIT_ESPI_DEBUG_GPIO
#define spacemit_espi_vwgpio_index(group)			\
	(slave0_rxvw_grp(group, spacemit_espi_read32(ESPI_SLAVE0_RXVW_INDEX)))
#define spacemit_espi_vwgpio_data(group)			\
	(slave0_rxvw_grp(group, spacemit_espi_read32(ESPI_SLAVE0_RXVW_DATA)))
#else
#define spacemit_espi_vwgpio_index(group)			\
	(slave0_rxvw_grp(group, __raw_readl(ESPI_SLAVE0_RXVW_INDEX)))
#define spacemit_espi_vwgpio_data(group)			\
	(slave0_rxvw_grp(group, __raw_readl(ESPI_SLAVE0_RXVW_DATA)))
#endif

#define ESPI_MEM_SIZE				SZ_16M
#ifdef CONFIG_SPACEMIT_ESPI_DEBUG_IO
#define spacemit_espi_io_read8(a)		spacemit_espi_read8(SPACEMIT_ESPI_IO_BASE + (a))
#define spacemit_espi_io_write8(v, a)		spacemit_espi_write8(v, SPACEMIT_ESPI_IO_BASE + (a))
#else
#define spacemit_espi_io_read8(a)		__raw_readb(SPACEMIT_ESPI_IO_BASE + (a))
#define spacemit_espi_io_write8(v, a)		__raw_writeb(v, SPACEMIT_ESPI_IO_BASE + (a))
#endif
#define spacemit_espi_mem_read8(a)		__raw_readb(SPACEMIT_ESPI_MEM_BASE + (a))
#define spacemit_espi_mem_write8(v, a)		__raw_writeb(v, SPACEMIT_ESPI_MEM_BASE + (a))
#define spacemit_espi_mem_read16(a)		__raw_readw(SPACEMIT_ESPI_MEM_BASE + (a))
#define spacemit_espi_mem_write16(v, a)		__raw_writew(v, SPACEMIT_ESPI_MEM_BASE + (a))
#define spacemit_espi_mem_read32(a)		__raw_readl(SPACEMIT_ESPI_MEM_BASE + (a))
#define spacemit_espi_mem_write32(v, a)		__raw_writel(v, SPACEMIT_ESPI_MEM_BASE + (a))
#define spacemit_espi_mem_cfg(addr0, addr1)	\
	do {					\
		espi_setup_pr_mem_base0(addr0);	\
		espi_setup_pr_mem_base1(addr1);	\
	} while (0)

void spacemit_espi_irq_init(void);
void spacemit_espi_handle_irq(void);
void spacemit_espi_write_cmd(uint8_t opcode,
			     uint8_t hlen, uint8_t *hbuf,
			     uint16_t dlen, uint8_t *dbuf);
uint8_t spacemit_espi_read_rsp(uint8_t opcod,
			       uint8_t hlen, uint8_t *hbuf,
			       uint16_t dlen, uint8_t *dbuf);
void spacemit_espi_set_cfg(uint32_t address, uint32_t config);
void spacemit_espi_init(void);

#endif /* __SPACEMIT_EPSI_H_INCLUDE__ */
