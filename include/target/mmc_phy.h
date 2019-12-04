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
 * @(#)mmc_phy.h: multimedia card (MMC) electrical definitions
 * $Id: mmc_phy.h,v 1.1 2019-10-28 11:20:00 zhenglv Exp $
 */

#ifndef __MMC_PHY_H_INCLUDE__
#define __MMC_PHY_H_INCLUDE__

/* ac */
#define MMC_CMD_SET_RELATIVE_ADDR	MMC_CMD3
#define MMC_CMD_SWITCH			MMC_CMD6
#ifdef MMC_CLASS5
#define MMC_CMD_ERASE_GROUP_START	MMC_CMD35
#define MMC_CMD_ERASE_GROUP_END		MMC_CMD36
#endif
#ifdef MMC_CLASS9
#define MMC_CMD_FAST_IO			MMC_CMD39
#define MMC_CMD_GO_IRQ_STATE		MMC_CMD40
#endif

/* adtc */
#define MMC_CMD_SEND_EXT_CSD		MMC_CMD8
#ifdef MMC_CLASS1
#define MMC_CMD_READ_DAT_UNTIL_STOP	MMC_CMD11
#endif
#define MMC_CMD_BUSTEST_R		MMC_CMD14
#define MMC_CMD_BUSTEST_W		MMC_CMD19
#ifdef MMC_CLASS3
#define MMC_CMD_WRITE_DATA_UNTIL_STOP	MMC_CMD20
#endif
#ifdef MMC_CLASS4
#define MMC_CMD_PROGRAM_CID		MMC_CMD26
#endif

#ifdef MMC_CLASS1
#define mmc_cmd_is_read_stream()			\
	mmc_cmd_is(MMC_CMD_READ_DAT_UNTIL_STOP)
#else
#define mmc_cmd_is_read_stream()			false
#endif
#ifdef MMC_CLASS3
#define mmc_cmd_is_write_stream()			\
	mmc_cmd_is(MMC_CMD_WRITE_DATA_UNTIL_STOP)
#else
#define mmc_cmd_is_write_stream()			false
#endif
#ifdef MMC_CLASS5
#define mmc_cmd_is_erase_group()			\
	(mmc_cmd_is(MMC_CMD_ERASE_GROUP_START) ||	\
	 mmc_cmd_is(MMC_CMD_ERASE_GROUP_END))
#else
#define mmc_cmd_is_erase_group()			false
#endif
#ifdef MMC_CLASS4
#define mmc_cmd_is_program()				\
	(mmc_cmd_is(MMC_CMD_PROGRAM_CID) ||		\
	 mmc_cmd_is(MMC_CMD_PROGRAM_CSD))
#else
#define mmc_cmd_is_program()				false
#endif
#ifdef MMC_CLASS9
#define mmc_cmd_is_fio()				\
	mmc_cmd_is(MMC_CMD_FAST_IO)
#define mmc_cmd_is_irq()				\
	mmc_cmd_is(MMC_CMD_GO_IRQ_STATE)
#else
#define mmc_cmd_is_fio()				false
#define mmc_cmd_is_irq()				false
#endif

/* 7.11 Card status
 * The response format R1 contains a 32-bit field named card status.
 */
/* Table 25 — Detection mode bit descriptions */
/* Error bits */
#define MMC_DET_UNDERRUN		_BV(18)
#define MMC_DET_OVERRUN			_BV(17)
#define MMC_DET_SWITCH_ERROR		_BV(7)
/* Table 5 — Card states and modes */
#define MMC_STATE_btst			9  /* bus test */
/* The values cannot be used as the result of MMC_CURRENT_STATE */
#define MMC_STATE_irq			10 /* wait-irq */

/* 8.1 OCR register - Operation Conditions register */
#define MMC_OCR_ACCESS_MODE_OFFSET	29
#define MMC_OCR_ACCESS_MODE_MASK	REG_2BIT_MASK
#define MMC_OCR_GET_ACCESS_MODE(value)	_GET_FV(MMC_OCR_ACCESS_MODE, value)
#define MMC_OCR_SET_ACCESS_MODE(value)	_SET_FV(MMC_OCR_ACCESS_MODE, value)
#define MMC_OCR_BYTE_MODE		0
#define MMC_OCR_SECTOR_MODE		2

/* 8.2 CID register - Card IDentification register */
typedef struct {
	uint8_t mid;	/* manuafacturer ID */
	uint16_t oid;	/* OEM/application ID */
#ifdef CONFIG_MMC_CID_ALL_FIELDS
	uint8_t pnm[6];	/* product name */
	uint8_t prv;	/* product revision */
	uint32_t psn;	/* product serial number */
	uint8_t mdt;	/* manuafacturing date */
	uint8_t crc;
#endif
} __packed mmc_cid_t;

/* 8.3 CSD register - Card-Specific Data */
/* CSD1 */
#define MMC_CSD1_ERASE_GRP_SIZE_OFFSET	10
#define MMC_CSD1_ERASE_GRP_SIZE_MASK	REG_5BIT_MASK
#define MMC_CSD1_ERASE_GRP_SIZE(value)	_GET_FV(MMC_CSD1_ERASE_GRP_SIZE, value)
#define MMC_CSD1_ERASE_GRP_MULT_OFFSET	5
#define MMC_CSD1_ERASE_GRP_MULT_MASK	REG_5BIT_MASK
#define MMC_CSD1_ERASE_GRP_MULT(value)	_GET_FV(MMC_CSD1_ERASE_GRP_MULT, value)
#define MMC_CSD1_WP_GRP_SIZE_OFFSET	0
#define MMC_CSD1_WP_GRP_SIZE_MASK	REG_5BIT_MASK
#define MMC_CSD1_WP_GRP_SIZE(value)	_GET_FV(MMC_CSD1_WP_GRP_SIZE, value)
/* CSD0 */
#define MMC_CSD0_DEFAULT_ECC_OFFSET	29
#define MMC_CSD0_DEFAULT_ECC_MASK	REG_2BIT_MASK
#define MMC_CSD0_DEFAULT_ECC(value)	_GET_FV(MMC_CSD0_DEFAULT_ECC, value)
#define MMC_CSD0_CONTENT_PROT_APP	_BV(16)
#define MMC_CSD0_ECC_OFFSET		8
#define MMC_CSD0_ECC_MASK		REG_2BIT_MASK
#define MMC_CSD0_GET_ECC(value)		_GET_FV(MMC_CSD0_ECC, value)
#define MMC_CSD0_SET_ECC(value)		_SET_FV(MMC_CSD0_ECC, value)

#define MMC_PHY_SLOT		\
	/* R2 */		\
	mmc_cid_t cid;

#include <target/mmc_spi.h>

#if defined(CONFIG_CONSOLE) && defined(CONFIG_MMC_DEBUG)
extern const char *mmc_phy_state_names[16];
extern const char *mmc_phy_cmd_names[64];
extern const char *mmc_phy_acmd_names[64];
#endif

#define mmc_phy_reset_slot()	do { } while (0)
void mmc_phy_handle_seq(void);
void mmc_phy_handle_stm(void);
void mmc_phy_recv_rsp(void);
void mmc_phy_send_cmd(void);

#endif /* __MMC_PHY_H_INCLUDE__ */
