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

#define SPACEMIT_ESPI_SLAVE_PERI_CFG		ESPI_SLAVE_PERI_CFG
#define SPACEMIT_ESPI_SLAVE_VW_CFG		ESPI_SLAVE_VWIRE_CFG
#define SPACEMIT_ESPI_SLAVE_OOB_CFG		ESPI_SLAVE_OOB_CFG
#define SPACEMIT_ESPI_SLAVE_FLASH_CFG		ESPI_SLAVE_FLASH_CFG

/* Register offset */
#define ESPI_DN_TXHDR				SPACEMIT_ESPI_REG(0x00)
#define ESPI_DN_TXHDRn(n)			SPACEMIT_ESPI_REG(0x00 + ((n) << 2))
#define ESPI_DN_TXDATA_PORT			SPACEMIT_ESPI_REG(0x0C)
#define ESPI_UP_RXHDR				SPACEMIT_ESPI_REG(0x10)
#define ESPI_UP_RXHDRn(n)			SPACEMIT_ESPI_REG(0x10 + ((n) << 2))
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
#define ESPI_SLAVE0_RXMSG_HDR0			SPACEMIT_ESPI_REG(0x74)
#define ESPI_SLAVE0_RXMSG_HDR1			SPACEMIT_ESPI_REG(0x78)
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
#define ESPI_ENABLE_VW_CHANNEL			0x70701

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
#define ESPI_WDG_CNT(n)				_SET_FV(ESPI_WDG_CNT, n)
#define ESPI_MST_IDLE_CNT_OFFSET		4
#define ESPI_MST_IDLE_CNT_MASK			REG_3BIT_MASK
#define ESPI_MST_IDLE_CNT(n)			_SET_FV(ESPI_MST_IDLE_CNT, n)
#define ESPI_MST_STOP_EN			_BV(3)
#define ESPI_WAIT_CHK_EN			_BV(1)
#define ESPI_WDG_EN				_BV(0)

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
#define ESPI_PR_INT_EN				_BV(23)
#define ESPI_PROTOCOL_ERR_INT			_BV(15)
#define ESPI_RXFLASH_OFLOW_INT			_BV(14)
#define ESPI_RXMSG_OFLOW_INT			_BV(13)
#define ESPI_RXOOB_OFLOW_INT			_BV(12)
#define ESPI_ILLEGAL_LEN_INT			_BV(11)
#define ESPI_ILLEGAL_TAG_INT			_BV(10)
#define ESPI_UNSUCSS_CPL_INT			_BV(9)
#define ESPI_INVALID_CT_RSP_INT			_BV(8)
#define ESPI_INVALID_ID_RSP_INT			_BV(7)
#define ESPI_NON_FATAL_INT			_BV(6)
#define ESPI_FATAL_ERR_INT			_BV(5)
#define ESPI_NO_RSP_INT				_BV(4)
#define ESPI_CRC_ERR_INT			_BV(2)
#define ESPI_WAIT_TIMEOUT_INT			_BV(1)
#define ESPI_BUS_ERR_INT			_BV(0)
#define ESPI_ALL_INT				\
	(ESPI_FLASH_REQ_INT |			\
	 ESPI_RXOOB_INT |			\
	 ESPI_RXMSG_INT |			\
	 ESPI_DNCMD_INT |			\
	 ESPI_RXVW_GPR3_INT |			\
	 ESPI_RXVW_GPR2_INT |			\
	 ESPI_RXVW_GPR1_INT |			\
	 ESPI_RXVW_GPR0_INT |			\
	 ESPI_PR_INT_EN |			\
	 ESPI_PROTOCOL_ERR_INT |		\
	 ESPI_RXFLASH_OFLOW_INT |		\
	 ESPI_RXMSG_OFLOW_INT |			\
	 ESPI_RXOOB_OFLOW_INT |			\
	 ESPI_ILLEGAL_LEN_INT |			\
	 ESPI_ILLEGAL_TAG_INT |			\
	 ESPI_UNSUCSS_CPL_INT |			\
	 ESPI_INVALID_CT_RSP_INT |		\
	 ESPI_INVALID_ID_RSP_INT |		\
	 ESPI_NON_FATAL_INT |			\
	 ESPI_FATAL_ERR_INT |			\
	 ESPI_NO_RSP_INT |			\
	 ESPI_CRC_ERR_INT |			\
	 ESPI_WAIT_TIMEOUT_INT |		\
	 ESPI_BUS_ERR_INT |			\
	 ESPI_BUS_ERR_INT)

/* SLAVE0_RXMSG_HDR0 */
#define SLAVE0_RXMSG_HDR0_RXMSG_HDATA2_OFFSET	24
#define SLAVE0_RXMSG_HDR0_RXMSG_HDATA2_MASK	REG_8BIT_MASK
#define SLAVE0_RXMSG_HDR0_RXMSG_HDATA2(n)	_SET_FV(SLAVE0_RXMSG_HDR0_RXMSG_HDATA2, n)
#define SLAVE0_RXMSG_HDR0_RXMSG_HDATA1_OFFSET	16
#define SLAVE0_RXMSG_HDR0_RXMSG_HDATA1_MASK	REG_8BIT_MASK
#define SLAVE0_RXMSG_HDR0_RXMSG_HDATA1(n)	_SET_FV(SLAVE0_RXMSG_HDR0_RXMSG_HDATA1, n)
#define SLAVE0_RXMSG_HDR0_RXMSG_HDATA0_OFFSET	8
#define SLAVE0_RXMSG_HDR0_RXMSG_HDATA0_MASK	REG_8BIT_MASK
#define SLAVE0_RXMSG_HDR0_RXMSG_HDATA0(n)	_SET_FV(SLAVE0_RXMSG_HDR0_RXMSG_HDATA0, n)
#define SLAVE0_RXMSG_HDR0_RXMSG_TYPE_OFFSET	0
#define SLAVE0_RXMSG_HDR0_RXMSG_TYPE_MASK	REG_8BIT_MASK
#define SLAVE0_RXMSG_HDR0_RXMSG_TYPE(n)		_SET_FV(SLAVE0_RXMSG_HDR0_RXMSG_TYPE, n)

/* SLAVE0_RXMSG_HDR1 */
#define SLAVE0_RXMSG_HDR1_SPECIFIC_HDATA3_OFFSET	24
#define SLAVE0_RXMSG_HDR1_SPECIFIC_HDATA3_MASK	REG_8BIT_MASK
#define SLAVE0_RXMSG_HDR1_SPECIFIC_HDATA3(n)	_SET_FV(SLAVE0_RXMSG_HDR1_SPECIFIC_HDATA3, n)
#define SLAVE0_RXMSG_HDR1_SPECIFIC_HDATA2_OFFSET	16
#define SLAVE0_RXMSG_HDR1_SPECIFIC_HDATA2_MASK	REG_8BIT_MASK
#define SLAVE0_RXMSG_HDR1_SPECIFIC_HDATA2(n)	_SET_FV(SLAVE0_RXMSG_HDR1_SPECIFIC_HDATA2, n)
#define SLAVE0_RXMSG_HDR1_SPECIFIC_HDATA1_OFFSET	8
#define SLAVE0_RXMSG_HDR1_SPECIFIC_HDATA1_MASK	REG_8BIT_MASK
#define SLAVE0_RXMSG_HDR1_SPECIFIC_HDATA1(n)	_SET_FV(SLAVE0_RXMSG_HDR1_SPECIFIC_HDATA1, n)
#define SLAVE0_RXMSG_HDR1_SPECIFIC_HDATA0_OFFSET	0
#define SLAVE0_RXMSG_HDR1_SPECIFIC_HDATA0_MASK	REG_8BIT_MASK
#define SLAVE0_RXMSG_HDR1_SPECIFIC_HDATA0(n)	_SET_FV(SLAVE0_RXMSG_HDR1_SPECIFIC_HDATA0, n)

/* SLAVE0_RXVW_STS */
#define SLAVE0_RXVW_STS_SYS_EVT_STS		_BV(29)
#define SLAVE0_RXVW_STS_IRQ23_STS		_BV(23)
#define SLAVE0_RXVW_STS_IRQ22_STS		_BV(22)
#define SLAVE0_RXVW_STS_IRQ21_STS		_BV(21)
#define SLAVE0_RXVW_STS_IRQ20_STS		_BV(20)
#define SLAVE0_RXVW_STS_IRQ19_STS		_BV(19)
#define SLAVE0_RXVW_STS_IRQ18_STS		_BV(18)
#define SLAVE0_RXVW_STS_IRQ17_STS		_BV(17)
#define SLAVE0_RXVW_STS_IRQ16_STS		_BV(16)
#define SLAVE0_RXVW_STS_IRQ15_STS		_BV(15)
#define SLAVE0_RXVW_STS_IRQ14_STS		_BV(14)
#define SLAVE0_RXVW_STS_IRQ13_STS		_BV(13)
#define SLAVE0_RXVW_STS_IRQ12_STS		_BV(12)
#define SLAVE0_RXVW_STS_IRQ11_STS		_BV(11)
#define SLAVE0_RXVW_STS_IRQ10_STS		_BV(10)
#define SLAVE0_RXVW_STS_IRQ9_STS		_BV(9)
#define SLAVE0_RXVW_STS_IRQ8_STS		_BV(8)
#define SLAVE0_RXVW_STS_IRQ7_STS		_BV(7)
#define SLAVE0_RXVW_STS_IRQ6_STS		_BV(6)
#define SLAVE0_RXVW_STS_IRQ5_STS		_BV(5)
#define SLAVE0_RXVW_STS_IRQ4_STS		_BV(4)
#define SLAVE0_RXVW_STS_IRQ3_STS		_BV(3)
#define SLAVE0_RXVW_STS_IRQ2_STS		_BV(2)
#define SLAVE0_RXVW_STS_IRQ1_STS		_BV(1)
#define SLAVE0_RXVW_STS_IRQ0_STS		_BV(0)

/* SLAVE0_RXVW */
#define SLAVE0_RXVW_HOST_RST_ACK		_BV(19)
#define SLAVE0_RXVW_RCIN			_BV(18)
#define SLAVE0_RXVW_SMI				_BV(17)
#define SLAVE0_RXVW_SCI				_BV(16)
#define SLAVE0_RXVW_SLAVE0_BOOT_LOAD_STS	_BV(15)
#define SLAVE0_RXVW_SLAVE0_ERROR_NONFATAL	_BV(14)
#define SLAVE0_RXVW_SLAVE0_ERROR_FATAL		_BV(13)
#define SLAVE0_RXVW_SLAVE0_BOOT_LOAD_DONE	_BV(12)
#define SLAVE0_RXVW_PME				_BV(11)
#define SLAVE0_RXVW_WAKE			_BV(10)
#define SLAVE0_RXVW_OOB_RST_ACK			_BV(8)
#define SLAVE0_RXVW_DNX_ACK			_BV(1)
#define SLAVE0_RXVW_SUS_ACK_B			_BV(0)

/* SLAVE0_RXVW_DATA */
#define SLAVE0_RXVW_DATA_GRP3_OFFSET		24
#define SLAVE0_RXVW_DATA_GRP3_MASK		REG_8BIT_MASK
#define SLAVE0_RXVW_DATA_GRP3(n)		_SET_FV(SLAVE0_RXVW_DATA_GRP3, n)
#define SLAVE0_RXVW_DATA_GRP2_OFFSET		16
#define SLAVE0_RXVW_DATA_GRP2_MASK		REG_8BIT_MASK
#define SLAVE0_RXVW_DATA_GRP2(n)		_SET_FV(SLAVE0_RXVW_DATA_GRP2, n)
#define SLAVE0_RXVW_DATA_GRP1_OFFSET		8
#define SLAVE0_RXVW_DATA_GRP1_MASK		REG_8BIT_MASK
#define SLAVE0_RXVW_DATA_GRP1(n)		_SET_FV(SLAVE0_RXVW_DATA_GRP1, n)
#define SLAVE0_RXVW_DATA_GRP0_OFFSET		0
#define SLAVE0_RXVW_DATA_GRP0_MASK		REG_8BIT_MASK
#define SLAVE0_RXVW_DATA_GRP0(n)		_SET_FV(SLAVE0_RXVW_DATA_GRP0, n)

/* SLAVE0_RXVW_INDEX */
#define SLAVE0_RXVW_INDEX_GRP3_OFFSET		24
#define SLAVE0_RXVW_INDEX_GRP3_MASK		REG_8BIT_MASK
#define SLAVE0_RXVW_INDEX_GRP3(n)		_SET_FV(SLAVE0_RXVW_INDEX_GRP3, n)
#define SLAVE0_RXVW_INDEX_GRP2_OFFSET		16
#define SLAVE0_RXVW_INDEX_GRP2_MASK		REG_8BIT_MASK
#define SLAVE0_RXVW_INDEX_GRP2(n)		_SET_FV(SLAVE0_RXVW_INDEX_GRP2, n)
#define SLAVE0_RXVW_INDEX_GRP1_OFFSET		8
#define SLAVE0_RXVW_INDEX_GRP1_MASK		REG_8BIT_MASK
#define SLAVE0_RXVW_INDEX_GRP1(n)		_SET_FV(SLAVE0_RXVW_INDEX_GRP1, n)
#define SLAVE0_RXVW_INDEX_GRP0_OFFSET		0
#define SLAVE0_RXVW_INDEX_GRP0_MASK		REG_8BIT_MASK
#define SLAVE0_RXVW_INDEX_GRP0(n)		_SET_FV(SLAVE0_RXVW_INDEX_GRP0, n)

/* SLAVE0_VW_CTL */
#define SLAVE0_VW_CTL_IRQ23_MASK		_BV(23)
#define SLAVE0_VW_CTL_IRQ22_MASK		_BV(22)
#define SLAVE0_VW_CTL_IRQ21_MASK		_BV(21)
#define SLAVE0_VW_CTL_IRQ20_MASK		_BV(20)
#define SLAVE0_VW_CTL_IRQ19_MASK		_BV(19)
#define SLAVE0_VW_CTL_IRQ18_MASK		_BV(18)
#define SLAVE0_VW_CTL_IRQ17_MASK		_BV(17)
#define SLAVE0_VW_CTL_IRQ16_MASK		_BV(16)
#define SLAVE0_VW_CTL_IRQ15_MASK		_BV(15)
#define SLAVE0_VW_CTL_IRQ14_MASK		_BV(14)
#define SLAVE0_VW_CTL_IRQ13_MASK		_BV(13)
#define SLAVE0_VW_CTL_IRQ12_MASK		_BV(12)
#define SLAVE0_VW_CTL_IRQ11_MASK		_BV(11)
#define SLAVE0_VW_CTL_IRQ10_MASK		_BV(10)
#define SLAVE0_VW_CTL_IRQ9_MASK			_BV(9)
#define SLAVE0_VW_CTL_IRQ8_MASK			_BV(8)
#define SLAVE0_VW_CTL_IRQ7_MASK			_BV(7)
#define SLAVE0_VW_CTL_IRQ6_MASK			_BV(6)
#define SLAVE0_VW_CTL_IRQ5_MASK			_BV(5)
#define SLAVE0_VW_CTL_IRQ4_MASK			_BV(4)
#define SLAVE0_VW_CTL_IRQ3_MASK			_BV(3)
#define SLAVE0_VW_CTL_IRQ2_MASK			_BV(2)
#define SLAVE0_VW_CTL_IRQ1_MASK			_BV(1)
#define SLAVE0_VW_CTL_IRQ0_MASK			_BV(0)

/* SLAVE0_VW_POLARITY */
#define SLAVE0_VW_POLARITY_IRQ23_POLARITY	_BV(23)
#define SLAVE0_VW_POLARITY_IRQ22_POLARITY	_BV(22)
#define SLAVE0_VW_POLARITY_IRQ21_POLARITY	_BV(21)
#define SLAVE0_VW_POLARITY_IRQ20_POLARITY	_BV(20)
#define SLAVE0_VW_POLARITY_IRQ19_POLARITY	_BV(19)
#define SLAVE0_VW_POLARITY_IRQ18_POLARITY	_BV(18)
#define SLAVE0_VW_POLARITY_IRQ17_POLARITY	_BV(17)
#define SLAVE0_VW_POLARITY_IRQ16_POLARITY	_BV(16)
#define SLAVE0_VW_POLARITY_IRQ15_POLARITY	_BV(15)
#define SLAVE0_VW_POLARITY_IRQ14_POLARITY	_BV(14)
#define SLAVE0_VW_POLARITY_IRQ13_POLARITY	_BV(13)
#define SLAVE0_VW_POLARITY_IRQ12_POLARITY	_BV(12)
#define SLAVE0_VW_POLARITY_IRQ11_POLARITY	_BV(11)
#define SLAVE0_VW_POLARITY_IRQ10_POLARITY	_BV(10)
#define SLAVE0_VW_POLARITY_IRQ9_POLARITY	_BV(9)
#define SLAVE0_VW_POLARITY_IRQ8_POLARITY	_BV(8)
#define SLAVE0_VW_POLARITY_IRQ7_POLARITY	_BV(7)
#define SLAVE0_VW_POLARITY_IRQ6_POLARITY	_BV(6)
#define SLAVE0_VW_POLARITY_IRQ5_POLARITY	_BV(5)
#define SLAVE0_VW_POLARITY_IRQ4_POLARITY	_BV(4)
#define SLAVE0_VW_POLARITY_IRQ3_POLARITY	_BV(3)
#define SLAVE0_VW_POLARITY_IRQ2_POLARITY	_BV(2)
#define SLAVE0_VW_POLARITY_IRQ1_POLARITY	_BV(1)
#define SLAVE0_VW_POLARITY_IRQ0_POLARITY	_BV(0)

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

#define  ESPI_CRC_CHECKING_EN			(1 << 31)
#define  ESPI_ALERT_MODE			(1 << 30)

#define  ESPI_IO_MODE_SHIFT			28
#define  ESPI_IO_MODE_MASK			(0x3 << ESPI_IO_MODE_SHIFT)
#define  ESPI_IO_MODE_VALUE(x)			((x) << ESPI_IO_MODE_SHIFT)

#define  ESPI_OP_FREQ_SHIFT			25
#define  ESPI_OP_FREQ_MASK			(0x7 << ESPI_OP_FREQ_SHIFT)
#define  ESPI_OP_FREQ_VALUE(x)			((x) << ESPI_OP_FREQ_SHIFT)

#define  ESPI_PERIPH_CH_EN			(1 << 3)
#define  ESPI_VW_CH_EN				(1 << 2)
#define  ESPI_OOB_CH_EN				(1 << 1)
#define  ESPI_FLASH_CH_EN			(1 << 0)

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

uint8_t spacemit_espi_read8(caddr_t reg);
uint16_t spacemit_espi_read16(caddr_t reg);
uint32_t spacemit_espi_read32(caddr_t reg);
void spacemit_espi_write8(uint8_t val, caddr_t reg);
void spacemit_espi_write16(uint16_t val, caddr_t reg);
void spacemit_espi_write32(uint32_t val, caddr_t reg);
#define spacemit_espi_set8(v,a)					\
	do {							\
		uint8_t __v = spacemit_espi_read8(a);		\
		__v |= (v);					\
		spacemit_espi_write8(__v, (a));			\
	} while (0)
#define spacemit_espi_clear8(v,a)				\
	do {							\
		uint8_t __v = spacemit_espi_read8(a);		\
		__v &= ~(v);					\
		spacemit_espi_write8(__v, (a));			\
	} while (0)
#define spacemit_espi_write8_mask(v,m,a)			\
	do {							\
		uint8_t __v = spacemit_espi_read8(a);		\
		__v &= ~(m);					\
		__v |= (v);					\
		spacemit_espi_write8(__v, (a));			\
	} while (0)
#define spacemit_espi_set16(v,a)				\
	do {							\
		uint16_t __v = spacemit_espi_read16(a);		\
		__v |= (v);					\
		spacemit_espi_write16(__v, (a));		\
	} while (0)
#define spacemit_espi_clear16(v,a)				\
	do {							\
		uint16_t __v = spacemit_espi_read16(a);		\
		__v &= ~(v);					\
		spacemit_espi_write16(__v, (a));		\
	} while (0)
#define spacemit_espi_write16_mask(v,m,a)			\
	do {							\
		uint16_t __v = spacemit_espi_read16(a);		\
		__v &= ~(m);					\
		__v |= (v);					\
		spacemit_espi_write16(__v, (a));		\
	} while (0)
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
	__raw_writel_mask(ESPI_DNCMD_TYPE(type) |				\
			  ESPI_DNCMD_EN |					\
			  ESPI_DNCMD_SLAVE_SEL(slave),				\
			  ESPI_DNCMD_TYPE(ESPI_DNCMD_TYPE_MASK) |		\
			  ESPI_DNCMD_EN |					\
			  ESPI_DNCMD_SLAVE_SEL(ESPI_DNCMD_SLAVE_SEL_MASK),	\
			  ESPI_DN_TXHDR)
#define spacemit_espi_read_dncmd()						\
	(__raw_readl(ESPI_DN_TXHDR) & ESPI_DNCMD_EN)
#define spacemit_espi_write_txhdr(n, byte)			\
	__raw_writel_mask(ESPI_DNCMD_HDATA(n, byte),		\
			  ESPI_DNCMD_HDATA(n, REG_8BIT_MASK),	\
			  ESPI_DN_TXHDRn(n))
#define spacemit_espi_read_rxhdr(n)				\
	((uint8_t)ESPI_UPCMD_HDATA(n, __raw_readl(ESPI_UP_RXHDRn(n))))
#define espi_setup_pr_mem_base0(base)	\
	spacemit_espi_write32(base, ESPI_PR_BASE_ADDR_MEM0)
#define espi_setup_pr_mem_base1(base)	\
	spacemit_espi_write32(base, ESPI_PR_BASE_ADDR_MEM1)

int espi_open_io_window(uint16_t base, size_t size);
int espi_open_mmio_window(uint32_t base, size_t size);

/*
 * In cases where eSPI BAR is statically provided by SoC, use that BAR instead of reading
 * SPIBASE. This is required for cases where verstage runs on PSP.
 */
void espi_update_static_bar(uintptr_t bar);

int spacemit_espi_tx_vw(uint8_t *ids, uint8_t *data, int num);
int spacemit_espi_tx_oob(uint8_t *buf, int len);
int spacemit_espi_rx_oob(uint8_t *buf);
int espi_send_oob_mctp(uint8_t *buf, int len);

void espi_register_rxvw_callback(void *callback);
void espi_register_rxoob_callback(void *callback, void *buffer);

void spacemit_espi_init(void);

#endif /* __SPACEMIT_EPSI_H_INCLUDE__ */
