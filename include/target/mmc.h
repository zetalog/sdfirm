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
 * @(#)mmc.h: multimedia card (MMC) definitions
 * $Id: mmc.h,v 1.1 2019-10-08 15:42:00 zhenglv Exp $
 */

#ifndef __MMC_H_INCLUDE__
#define __MMC_H_INCLUDE__

#include <target/generic.h>
#include <target/panic.h>
#include <target/jiffies.h>

#ifdef CONFIG_MMC_DEBUG
#define mmc_debug_state(state)					\
	do {							\
		if (state != mmc_state_get()) {			\
			mmc_debug(MMC_DEBUG_STATE, state);	\
		}						\
	} while (0)
#define mmc_debug_event(event)					\
	do {							\
		mmc_debug(MMC_DEBUG_EVENT, event);		\
	} while (0)
#define mmc_debug_error(error)					\
	do {							\
		mmc_debug(MMC_DEBUG_ERROR, error);		\
	} while (0)
#define mmc_debug_cmd(cmd)					\
	do {							\
		mmc_debug(MMC_DEBUG_CMD, cmd);			\
	} while (0)
#define mmc_debug_acmd(acmd)					\
	do {							\
		mmc_debug(MMC_DEBUG_ACMD, acmd);		\
	} while (0)
#define mmc_debug_op(op)					\
	do {							\
		mmc_debug(MMC_DEBUG_OP, op);			\
	} while (0)
#else
#define mmc_debug_state(state)
#define mmc_debug_event(event)
#define mmc_debug_error(error)
#define mmc_debug_cmd(cmd)
#define mmc_debug_acmd(acmd)
#define mmc_debug_op(op)
#endif

#ifndef NR_MMC_SLOTS
#define NR_MMC_SLOTS		CONFIG_MMC_MAX_SLOTS
#endif
#define INVALID_MMC_SLOT	NR_MMC_SLOTS
#ifndef NR_MMC_CARDS
#define NR_MMC_CARDS		CONFIG_MMC_MAX_CARDS
#endif
#define INVALID_MMC_CARD	NR_MMC_CARDS

#define MMC_RCA_DEFAULT		0

typedef uint16_t mmc_slot_t;
typedef uint16_t mmc_card_t;
typedef uint16_t mmc_rca_t;
typedef uint16_t mmc_event_t;
typedef uint32_t mmc_lba_t;
typedef uint8_t mmc_r1_t[4];
typedef uint8_t mmc_r2_t[16];
typedef uint8_t mmc_r3_t[4];

#include <driver/mmc.h>

#ifdef MMC_HW_DATA_ALIGN
#define MMC_DATA_ALIGN		MMC_HW_DATA_ALIGN
#else
#define MMC_DATA_ALIGN		4
#endif

/* This file is implemented according to the following standards:
 * JESD84-B41
 *  MultiMediaCard (MMC) Electrical Standard, Standard Capacity
 * JESD84-B42
 *  MultiMediaCard (MMC) Electrical Standard, High Capacity
 */

/* 7.8 commands */
/* command index */
#define MMC_CMD0			0
#define MMC_CMD1			1
#define MMC_CMD2			2
#define MMC_CMD3			3
#define MMC_CMD4			4
#define MMC_CMD5			5
#define MMC_CMD6			6
#define MMC_CMD7			7
#define MMC_CMD8			8
#define MMC_CMD9			9
#define MMC_CMD10			10
#define MMC_CMD11			11
#define MMC_CMD12			12
#define MMC_CMD13			13
#define MMC_CMD14			14
#define MMC_CMD15			15
#define MMC_CMD16			16
#define MMC_CMD17			17
#define MMC_CMD18			18
#define MMC_CMD19			19
#define MMC_CMD20			20
#define MMC_CMD21			21
#define MMC_CMD22			22
#define MMC_CMD23			23
#define MMC_CMD24			24
#define MMC_CMD25			25
#define MMC_CMD26			26
#define MMC_CMD27			27
#define MMC_CMD28			28
#define MMC_CMD29			29
#define MMC_CMD30			30
#define MMC_CMD31			31
#define MMC_CMD32			32
#define MMC_CMD33			33
#define MMC_CMD34			34
#define MMC_CMD35			35
#define MMC_CMD36			36
#define MMC_CMD37			37
#define MMC_CMD38			38
#define MMC_CMD39			39
#define MMC_CMD40			40
#define MMC_CMD41			41
#define MMC_CMD42			42
#define MMC_CMD43			43
#define MMC_CMD44			44
#define MMC_CMD45			45
#define MMC_CMD46			46
#define MMC_CMD47			47
#define MMC_CMD48			48
#define MMC_CMD49			49
#define MMC_CMD50			50
#define MMC_CMD51			51
#define MMC_CMD52			52
#define MMC_CMD53			53
#define MMC_CMD54			54
#define MMC_CMD55			55
#define MMC_CMD56			56
#define MMC_CMD57			57
#define MMC_CMD58			58
#define MMC_CMD59			59
#define MMC_CMD60			60
#define MMC_CMD61			61
#define MMC_CMD62			62
#define MMC_CMD63			63

#define MMC_CMD_NONE			64

/* Application commands */
#define MMC_ACMD6			6
#define MMC_ACMD13			13
#define MMC_ACMD22			22
#define MMC_ACMD23			23
#define MMC_ACMD41			41
#define MMC_ACMD42			42
#define MMC_ACMD51			51

#define MMC_ACMD_NONE			64

#ifdef CONFIG_MMC
#include <target/mmc_phy.h>
#endif
#ifdef CONFIG_SD
#include <target/sd_phy.h>
#endif

/* bc */
#define MMC_CMD_GO_IDLE_STATE		MMC_CMD0
#define MMC_CMD_SET_DSR			MMC_CMD4

/* bcr */
#define MMC_CMD_SEND_OP_COND		MMC_CMD1
#define MMC_CMD_ALL_SEND_CID		MMC_CMD2

/* ac */
#define MMC_CMD_SELECT_DESELECT_CARD	MMC_CMD7
#define MMC_CMD_SEND_CSD		MMC_CMD9
#define MMC_CMD_SEND_CID		MMC_CMD10
#define MMC_CMD_STOP_TRANSMISSION	MMC_CMD12
#define MMC_CMD_SEND_STATUS		MMC_CMD13
#define MMC_CMD_GO_INACTIVE_STATE	MMC_CMD15
#if defined(MMC_CLASS2) || defined(MMC_CLASS4)
#define MMC_CMD_SET_BLOCKLEN		MMC_CMD16
#endif
#define MMC_CMD_SET_BLOCK_COUNT		MMC_CMD23
#ifdef MMC_CLASS6
#define MMC_CMD_SET_WRITE_PROT		MMC_CMD28
#define MMC_CMD_CLR_WRITE_PROT		MMC_CMD29
#endif
#ifdef MMC_CLASS5
#define MMC_CMD_ERASE			MMC_CMD38
#endif
#ifdef MMC_CLASS8
#define MMC_CMD_GEN_CMD			MMC_CMD56
#define MMC_CMD_APP_CMD			MMC_CMD55
#endif

/* adtc */
#ifdef MMC_CLASS2
#define MMC_CMD_READ_SINGLE_BLOCK	MMC_CMD17
#define MMC_CMD_READ_MULTIPLE_BLOCK	MMC_CMD18
#endif
#ifdef MMC_CLASS4
#define MMC_CMD_WRITE_BLOCK		MMC_CMD24
#define MMC_CMD_WRITE_MULTIPLE_BLOCK	MMC_CMD25
#define MMC_CMD_PROGRAM_CSD		MMC_CMD27
#endif
#ifdef MMC_CLASS6
#define MMC_CMD_SEND_WRITE_PROT		MMC_CMD30
#endif
#ifdef MMC_CLASS7
#define MMC_CMD_LOCK_UNLOCK		MMC_CMD42
#endif

#ifdef MMC_CLASS2
#define mmc_cmd_is_read_block()				\
	(mmc_cmd_is(MMC_CMD_READ_SINGLE_BLOCK) ||	\
	 mmc_cmd_is(MMC_CMD_READ_MULTIPLE_BLOCK))
#else
#define mmc_cmd_is_read_block()				false
#endif
#ifdef MMC_CLASS4
#define mmc_cmd_is_write_block()			\
	(mmc_cmd_is(MMC_CMD_WRITE_BLOCK) ||		\
	 mmc_cmd_is(MMC_CMD_WRITE_MULTIPLE_BLOCK))
#else
#define mmc_cmd_is_write_block()			false
#endif
#if defined(MMC_CLASS2) || defined(MMC_CLASS4)
#define mmc_cmd_is_block_len()				\
	mmc_cmd_is(MMC_CMD_SET_BLOCKLEN)
#else
#define mmc_cmd_is_block_len()				false
#endif
#ifdef MMC_CLASS6
#define mmc_cmd_is_write_prot()				\
	mmc_cmd_is(MMC_CMD_SEND_WRITE_PROT)
#define mmc_cmd_is_write_prot_bit()			\
	(mmc_cmd_is(MMC_CMD_SET_WRITE_PROT) ||		\
	 mmc_cmd_is(MMC_CMD_CLR_WRITE_PROT))
#else
#define mmc_cmd_is_write_prot()				false
#define mmc_cmd_is_write_prot_bit()			false
#endif
#ifdef MMC_CLASS7
#define mmc_cmd_is_lock()				\
	mmc_cmd_is(MMC_CMD_LOCK_UNLOCK)
#else
#define mmc_cmd_is_lock()				false
#endif
#ifdef MMC_CLASS5
#define mmc_cmd_is_erase()				\
	mmc_cmd_is(MMC_CMD_ERASE)
#else
#define mmc_cmd_is_erase()				false
#endif
#ifdef MMC_CLASS8
#define mmc_cmd_is_w()		(mmc_slot_ctrl.flags & MMC_SLOT_GEN_CMD_RDWR)
#define mmc_cmd_is_r()		(!mmc_cmd_is_w())
#define mmc_cmd_is_gen_w()	(mmc_cmd_is_w() && mmc_cmd_is(MMC_CMD_GEN_CMD))
#define mmc_cmd_is_gen_r()	(mmc_cmd_is_r() && mmc_cmd_is(MMC_CMD_GEN_CMD))
#define mmc_cmd_is_app()				\
	mmc_cmd_is(MMC_CMD_APP_CMD)
#else
#define mmc_cmd_is_gen_w()	false
#define mmc_cmd_is_gen_r()	false
#define mmc_cmd_is_app()				false
#endif

/* 7.10 Responses */
#define MMC_RSP_TYPE(x)		((x) & 0x7)
#define MMC_RSP_PRESENT		_BV(3)
#define MMC_RSP_136		_BV(4)	/* 136 bit response */
#define MMC_RSP_CRC		_BV(5)	/* expect valid crc */
#define MMC_RSP_BUSY		_BV(6)	/* card may send busy */
#define MMC_RSP_OPCODE		_BV(7)	/* response contains opcode */

#define MMC_R_NC		MMC_RSP_PRESENT
#define MMC_R_C			(MMC_RSP_PRESENT|MMC_RSP_CRC)
#define MMC_R_CS		(MMC_R_C|MMC_RSP_OPCODE)
#define MMC_R_CL		(MMC_R_C|MMC_RSP_136)

#define MMC_R0		MMC_RSP_TYPE(0)
#define MMC_R1		(MMC_RSP_TYPE(1)|MMC_R_CS)
#define MMC_R1b		(MMC_RSP_TYPE(1)|MMC_RSP_BUSY|MMC_R_CS)
/* CID, CSD */
#define MMC_R2		(MMC_RSP_TYPE(2)|MMC_R_CL)
/* OCR */
#define MMC_R3		(MMC_RSP_TYPE(3)|MMC_R_NC)
/* Fast I/O */
#define MMC_R4		(MMC_RSP_TYPE(4)|MMC_R_NC)
/* IRQ */
#define MMC_R5		(MMC_RSP_TYPE(5)|MMC_R_CS)

/* R1: Card status */
#define MMC_DET_ADDRESS_OUT_OF_RANGE	_BV(31)
#define MMC_DET_ADDRESS_MISALIGN	_BV(30)
#define MMC_DET_BLOCK_LEN_ERROR		_BV(29)
#define MMC_DET_ERASE_SEQ_ERROR		_BV(28)
#define MMC_DET_ERASE_PARAM		_BV(27)
#define MMC_DET_WP_VIOLATION		_BV(26)
#define MMC_DET_CARD_IS_LOCKED		_BV(25)
#define MMC_DET_LOCK_UNLOCK_FAILED	_BV(24)
#define MMC_DET_COM_CRC_ERROR		_BV(23)
#define MMC_DET_ILLEGAL_COMMAND		_BV(22)
#define MMC_DET_CARD_ECC_FAILED		_BV(21)
#define MMC_DET_CC_ERROR		_BV(20)
#define MMC_DET_ERROR			_BV(19)
#define MMC_DET_CID_CSD_OVERWRITE	_BV(16)
#define MMC_DET_WP_ERASE_SKIP		_BV(15)
#define MMC_DET_ERASE_RESET		_BV(13)
/* Status bits */
#define MMC_DET_CURRENT_STATE_OFFSET	9
#define MMC_DET_CURRENT_STATE_MASK	REG_4BIT_MASK
#define MMC_CURRENT_STATE(value)	_SET_FV(MMC_DET_CURRENT_STATE, value)
#define mmc_current_state(value)	_GET_FV(MMC_DET_CURRENT_STATE, value)
#define MMC_DET_READY_FOR_DATA		_BV(8)
#define MMC_DET_APP_CMD			_BV(5)
/* The values can be used as the result of MMC_CURRENT_STATE,
 * and are common to SD/MMC.
 */
#define MMC_STATE_idle			0  /* idle */
#define MMC_STATE_ready			1  /* ready */
#define MMC_STATE_ident			2  /* identification */
#define MMC_STATE_stby			3  /* stand-by */
#define MMC_STATE_tran			4  /* transfer */
#define MMC_STATE_data			5  /* sending-data */
#define MMC_STATE_rcv			6  /* receive-data */
#define MMC_STATE_prg			7  /* programming */
#define MMC_STATE_dis			8  /* disconnect */
/* The values are not used as MMC_CURRENT_STATE */
#define MMC_STATE___ina			14 /* entering inactive */
#define MMC_STATE_ina			15 /* inactive */
#define NR_MMC_STATES			16

/* Card registers */
/* R3: OCR register - Operation Condition Register */
#define MMC_OCR_170_195			_BV(7)  /* VDD voltage 1.70 ~ 1.95 */
#define MMC_OCR_20_21			_BV(8)  /* VDD voltage 2.0 ~ 2.1 */
#define MMC_OCR_21_22			_BV(9)  /* VDD voltage 2.1 ~ 2.2 */
#define MMC_OCR_22_23			_BV(10) /* VDD voltage 2.2 ~ 2.3 */
#define MMC_OCR_23_24			_BV(11) /* VDD voltage 2.3 ~ 2.4 */
#define MMC_OCR_24_25			_BV(12) /* VDD voltage 2.4 ~ 2.5 */
#define MMC_OCR_25_26			_BV(13) /* VDD voltage 2.5 ~ 2.6 */
#define MMC_OCR_26_27			_BV(14) /* VDD voltage 2.6 ~ 2.7 */
#define MMC_OCR_20_26						\
	(MMC_OCR_20_21 | MMC_OCR_21_22 | MMC_OCR_22_23 |	\
	 MMC_OCR_23_24 | MMC_OCR_24_25 | MMC_OCR_25_26 |	\
	 MMC_OCR_26_27)
#define MMC_OCR_27_28			_BV(15) /* VDD voltage 2.7 ~ 2.8 */
#define MMC_OCR_28_29			_BV(16) /* VDD voltage 2.8 ~ 2.9 */
#define MMC_OCR_29_30			_BV(17)	/* VDD voltage 2.9 ~ 3.0 */
#define MMC_OCR_30_31			_BV(18) /* VDD voltage 3.0 ~ 3.1 */
#define MMC_OCR_31_32			_BV(19) /* VDD voltage 3.1 ~ 3.2 */
#define MMC_OCR_32_33			_BV(20) /* VDD voltage 3.2 ~ 3.3 */
#define MMC_OCR_33_34   		_BV(21) /* VDD voltage 3.3 ~ 3.4 */
#define MMC_OCR_34_35			_BV(22) /* VDD voltage 3.4 ~ 3.5 */
#define MMC_OCR_35_36			_BV(23) /* VDD voltage 3.5 ~ 3.6 */
#define MMC_OCR_27_36						\
	(MMC_OCR_27_28 | MMC_OCR_28_29 | MMC_OCR_29_30 |	\
	 MMC_OCR_30_31 | MMC_OCR_31_32 | MMC_OCR_32_33 |	\
	 MMC_OCR_33_34 | MMC_OCR_34_35 | MMC_OCR_35_36)
#define MMC_OCR_LOW_VOLTAGE		MMC_OCR_170_195
#define MMC_OCR_HIGH_VOLTAGE		MMC_OCR_27_36
#define MMC_OCR_DUAL_VOLTAGE		(MMC_OCR_170_195 | MMC_OCR_27_36)
#define MMC_OCR_VOLTAGE_RANGE_OFFSET	0
#define MMC_OCR_VOLTAGE_RANGE_MASK	REG_24BIT_MASK
#define MMC_OCR_VOLTAGE_RANGE(value)	_GET_FV(MMC_OCR_VOLTAGE_RANGE, value)
#define MMC_OCR_CCS			_BV(30)
#define MMC_OCR_BUSY			_BV(31)

/* 8.3 CSD register - Card Specific Data register
 *
 * The CSD is defined using the following field registers:
 * CSD3[32-bit] | CSD2[32-bit] | | CSD1[32-bit] | CSD0[32-bit]
 */
/* CSD3 */
#define MMC_CSD3_CSD_STRUCTURE_OFFSET	30
#define MMC_CSD3_CSD_STRUCTURE_MASK	REG_2BIT_MASK
#define MMC_CSD3_CSD_STRUCTURE(value)	_GET_FV(MMC_CSD3_CSD_STRUCTURE, value)
#define MMC_CSD3_CSD_VERSION_1_0	0
#define MMC_CSD3_CSD_VERSION_1_1	1
#define MMC_CSD3_CSD_VERSION_1_2	2
#define MMC_CSD3_CSD_VERSION_EXT	3
#define MMC_CSD3_SPEC_VERS_OFFSET	24
#define MMC_CSD3_SPEC_VERS_MASK		REG_4BIT_MASK
#define MMC_CSD3_SPEC_VERS(value)	_GET_FV(MMC_CSD3_SPEC_VERS, value)
#define MMC_CSD3_TAAC_OFFSET		16
#define MMC_CSD3_TAAC_MASK		REG_8BIT_MASK
#define MMC_CSD3_TAAC(value)		_GET_FV(MMC_CSD3_TAAC, value)
#define MMC_CSD3_NSAC_OFFSET		8
#define MMC_CSD3_NSAC_MASK		REG_8BIT_MASK
#define MMC_CSD3_NSAC(value)		_GET_FV(MMC_CSD3_NSAC, value)
#define MMC_CSD3_TRAN_SPEED_OFFSET	0
#define MMC_CSD3_TRAN_SPEED_MASK	REG_8BIT_MASK
#define MMC_CSD3_TRAN_SPEED(value)	_GET_FV(MMC_CSD3_TRAN_SPEED, value)
/* TRAN_SPEED fields */
#define MMC_TRAN_SPEED_UNIT_OFFSET	0
#define MMC_TRAN_SPEED_UNIT_MASK	REG_3BIT_MASK
#define MMC_TRAN_SPEED_UNIT(value)	_GET_FV(MMC_TRAN_SPEED_UNIT, value)
#define MMC_TRAN_SPEED_MULT_OFFSET	3
#define MMC_TRAN_SPEED_MULT_MASK	REG_4BIT_MASK
#define MMC_TRAN_SPEED_MULT(value)	_GET_FV(MMC_TRAN_SPEED_MULT, value)
/* CSD2 */
#define MMC_CSD2_CCC_OFFSET		20
#define MMC_CSD2_CCC_MASK		REG_12BIT_MASK
#define MMC_CSD2_CCC(value)		_GET_FV(MMC_CSD2_CCC, value)
#define MMC_CSD2_READ_BL_LEN_OFFSET	16
#define MMC_CSD2_READ_BL_LEN_MASK	REG_4BIT_MASK
#define MMC_CSD2_READ_BL_LEN(value)	_GET_FV(MMC_CSD2_READ_BL_LEN, value)
#define MMC_CSD2_READ_BL_PARTIAL	_BV(15)
#define MMC_CSD2_WRITE_BLK_MISALIGN	_BV(14)
#define MMC_CSD2_READ_BLK_MISALIGN	_BV(13)
#define MMC_CSD2_DSR_IMP		_BV(12)
#define MMC_CSD2_C_SIZE_OFFSET		0
#define MMC_CSD2_C_SIZE_MASK		REG_10BIT_MASK
#define MMC_CSD2_C_SIZE(value)		_GET_FV(MMC_CSD2_C_SIZE, value)
/* CSD1 */
#define MMC_CSD1_C_SIZE_OFFSET		30
#define MMC_CSD1_C_SIZE_MASK		REG_2BIT_MASK
#define MMC_CSD1_C_SIZE(value)		_GET_FV(MMC_CSD1_C_SIZE, value)
#define MMC_CSD1_VDD_R_CURR_MIN_OFFSET	27
#define MMC_CSD1_VDD_R_CURR_MIN_MASK	REG_3BIT_MASK
#define MMC_CSD1_VDD_R_CURR_MIN(value)	_GET_FV(MMC_CSD1_VDD_R_CURR_MIN, value)
#define MMC_CSD1_VDD_R_CURR_MAX_OFFSET	24
#define MMC_CSD1_VDD_R_CURR_MAX_MASK	REG_3BIT_MASK
#define MMC_CSD1_VDD_R_CURR_MAX(value)	_GET_FV(MMC_CSD1_VDD_R_CURR_MAX, value)
#define MMC_CSD1_VDD_W_CURR_MIN_OFFSET	21
#define MMC_CSD1_VDD_W_CURR_MIN_MASK	REG_3BIT_MASK
#define MMC_CSD1_VDD_W_CURR_MIN(value)	_GET_FV(MMC_CSD1_VDD_W_CURR_MIN, value)
#define MMC_CSD1_VDD_W_CURR_MAX_OFFSET	18
#define MMC_CSD1_VDD_W_CURR_MAX_MASK	REG_3BIT_MASK
#define MMC_CSD1_VDD_W_CURR_MAX(value)	_GET_FV(MMC_CSD1_VDD_W_CURR_MAX, value)
#define MMC_CSD1_C_SIZE_MULT_OFFSET	15
#define MMC_CSD1_C_SIZE_MULT_MASK	REG_3BIT_MASK
#define MMC_CSD1_C_SIZE_MULT(value)	_GET_FV(MMC_CSD1_C_SIZE_MULT, value)
/* CSD0 */
#define MMC_CSD0_WP_GRP_ENABLE		_BV(31)
#define MMC_CSD0_R2W_FACTOR_OFFSET	26
#define MMC_CSD0_R2W_FACTOR_MASK	REG_3BIT_MASK
#define MMC_CSD0_R2W_FACTOR(value)	_GET_FV(MMC_CSD0_R2W_FACTOR, value)
#define MMC_CSD0_WRITE_BL_LEN_OFFSET	22
#define MMC_CSD0_WRITE_BL_LEN_MASK	REG_4BIT_MASK
#define MMC_CSD0_WRITE_BL_LEN(value)	_GET_FV(MMC_CSD0_WRITE_BL_LEN, value)
#define MMC_CSD0_WRITE_BL_PARTIAL	_BV(21)
/* R/W fields */
#define MMC_CSD0_FILE_FORMAT_GRP	_BV(15)
#define MMC_CSD0_COPY			_BV(14)
#define MMC_CSD0_PERM_WRITE_PROTECT	_BV(13)
#define MMC_CSD0_TMP_WRITE_PROTECT	_BV(12)
#define MMC_CSD0_FILE_FORMAT_OFFSET	10
#define MMC_CSD0_FILE_FORMAT_MASK	REG_2BIT_MASK
#define MMC_CSD0_GET_FILE_FORMAT(value)	_GET_FV(MMC_CSD0_FILE_FORMAT, value)
#define MMC_CSD0_SET_FILE_FORMAT(value)	_GET_FV(MMC_CSD0_FILE_FORMAT, value)
#define MMC_CSD0_CRC_OFFSET		8
#define MMC_CSD0_CRC_MASK		REG_7BIT_MASK
#define MMC_CSD0_GET_CRC(value)		_GET_FV(MMC_CSD0_CRC, value)
#define MMC_CSD0_SET_CRC(value)		_SET_FV(MMC_CSD0_CRC, value)

/* 8.4 Extended CSD register */
/* CSD version 1/2 */
typedef struct {
	uint8_t csd_structure : 2;
	uint8_t spec_vers : 4;

	uint8_t taac;
	uint8_t nsac;
	uint8_t tran_speed;

	uint8_t read_bl_len : 4;
	uint8_t read_bl_partial : 1;
	uint8_t read_blk_misalign : 1;
	uint8_t read_bl_reserved : 2;

	uint8_t write_bl_len : 4;
	uint8_t write_bl_partial : 1;
	uint8_t write_blk_misalign : 1;
	uint8_t write_bl_reserved : 2;

	uint8_t sector_size : 7;
	uint8_t erase_blk_en : 1;

	uint8_t wp_grp_size : 7;
	uint8_t wp_grp_enable : 1;

	uint64_t capacity; /* from c_size/c_mult */

	uint16_t vdd_r_curr_min : 3;
	uint16_t vdd_r_curr_max : 3;
	uint16_t vdd_w_curr_min : 3;
	uint16_t vdd_w_curr_max : 3;
	uint16_t r2w_factor : 3;
	uint16_t dsr_imp : 1;

	uint16_t ccc : 12;
} __packed mmc_csd_t;
#define MMC_DEF_BL_LEN		512

/*===========================================================================
 * MMC Slot
 *===========================================================================*/
typedef void (*mmc_cmpl_cb)(mmc_rca_t rca, uint8_t op, bool result);

struct mmc_slot {
	uint8_t state;
	uint8_t cmd;
	uint8_t acmd;
	uint8_t rsp;
	uint8_t *dat;
	uint8_t err;
	uint8_t op;
	uint16_t flags;
#define MMC_SLOT_CARD_STATUS_VALID	_BV(0) /* R1 */
#define MMC_SLOT_CARD_IS_BUSY		_BV(1) /* R1b */
#define MMC_SLOT_GEN_CMD_RDWR		_BV(2)
#define MMC_SLOT_SPI_CRC_ONOFF		_BV(3)
#define MMC_SLOT_WAIT_APP_CMD		_BV(4)
#define MMC_SLOT_WAIT_APP_ACMD		_BV(5)
#define MMC_SLOT_BLOCK_READ		_BV(6)
#define MMC_SLOT_BLOCK_WRITE		_BV(7)
#define MMC_SLOT_BLOCK_DATA		\
	(MMC_SLOT_BLOCK_READ | MMC_SLOT_BLOCK_WRITE)
#define MMC_SLOT_CARD_DETECT		_BV(8)
#define MMC_SLOT_CARD_SELECT		_BV(9)
#define MMC_SLOT_TRANS_STOPPED		_BV(10)
	mmc_event_t event;
	/* SDSC Card (CCS=0) uses byte unit address and SDHC and SDXC
	 * Cards (CCS=1) use block unit address (512 Bytes unit).
	 */
	uint32_t address;
	uint16_t block_cnt;
	uint32_t block_len;
	uint8_t *block_data;
	tick_t start_tick;
	tick_t start_busy;
	/* erase/switch func */
	uint32_t func;
	/* R1 */
	uint32_t csr;
	/* R2 */
	mmc_csd_t csd;
	bool csd_valid;
	bool block_start;
	bool block_stop;
	uint32_t f_max;
	uint32_t f_min;
	/* R3 */
	uint32_t card_ocr; /* card capacity */
	uint32_t host_ocr; /* host capacity */
	bool ocr_valid;
	uint16_t dsr;
	/* XXX: support only 1 card per slot */
	mmc_rca_t rca;
	mmc_card_t mmc_cid;
	mmc_cmpl_cb op_cb;
	MMC_PHY_SLOT
	MMC_SPI_SLOT
};
#define mmc_op_is(_op)			(!!(mmc_slot_ctrl.op == (_op)))
#define mmc_cmd_is(_cmd)		(!!(mmc_slot_ctrl.cmd == (_cmd)))
#define mmc_err_is(_err)		(!!(mmc_slot_ctrl.err == (_err)))

#define MMC_BLOCK(rdwr, len, cnt)				\
	do {							\
		mmc_slot_ctrl.block_len = (len);		\
		mmc_slot_ctrl.block_cnt = (cnt);		\
		mmc_set_block_data(MMC_SLOT_BLOCK_##rdwr);	\
	} while (0)

#define MMC_EVENT_POWER_ON	_BV(0)
#define MMC_EVENT_CMD_SUCCESS	_BV(1)
#define MMC_EVENT_CMD_FAILURE	_BV(2)
#define MMC_EVENT_CARD_IRQ	_BV(3)
#define MMC_EVENT_NO_IRQ	_BV(4)
#define MMC_EVENT_OP_COMPLETE	_BV(5)
#define MMC_EVENT_TRANS_END	_BV(6)
#define MMC_EVENT_CARD_INSERT	_BV(7)
#define MMC_EVENT_CARD_REMOVE	_BV(8)
#define MMC_EVENT_CARD_BUSY	_BV(9)
#define MMC_EVENT_SELECT_CARD	_BV(10) /* SPI */
#define MMC_EVENT_DESELECT_CARD	_BV(11) /* SPI */
#define MMC_EVENT_START_TRAN	_BV(12)
#define MMC_EVENT_STOP_TRAN	_BV(13)

#define MMC_OP_NO_OP			0
#define MMC_OP_IDENTIFY_CARD		1
#define MMC_OP_SELECT_CARD		2
#define MMC_OP_DESELECT_CARD		3
#define MMC_OP_READ_BLOCKS		4
#define MMC_OP_WRITE_BLOCKS		5

#define MMC_ERR_NO_ERROR		0
#define MMC_ERR_CARD_IS_BUSY		1 /* card is busy */
#define MMC_ERR_HOST_OMIT_VOLT		2 /* host omits voltage range */
#define MMC_ERR_CARD_NON_COMP_VOLT	3 /* card with non compatible voltage range */
#define MMC_ERR_CARD_LOOSE_BUS		4 /* card looses bus */
#define MMC_ERR_ILLEGAL_COMMAND		5 /* illegal command */
#define MMC_ERR_COM_CRC_ERROR		6 /* com CRC error */
#define MMC_ERR_CHECK_PATTERN		7 /* check pattern error */
#define MMC_ERR_TIMEOUT			8 /* communication timeout */

#ifdef CONFIG_DEBUG_PRINT
#define mmc_debug(tag, val)	dbg_print((tag), (val))
#endif
#ifdef CONFIG_CONSOLE
void mmc_debug(uint8_t tag, uint32_t val);
#endif

#if NR_MMC_SLOTS > 1
mmc_slot_t mmc_slot_save(mmc_slot_t slot);
void mmc_slot_restore(mmc_slot_t slot);
extern mmc_slot_t mmc_sid;
extern struct mmc_slot mmc_slots[NR_MMC_SLOTS];
extern uint8_t mmc_slot_bufs[NR_MMC_SLOTS][MMC_DEF_BL_LEN];
#define mmc_slot_ctrl mmc_slots[mmc_sid]
#define mmc_slot_buf mmc_slot_bufs[mmc_sid]
#else
#define mmc_slot_save(sid)	0
#define mmc_slot_restore(rca)
#define mmc_sid			0
extern struct mmc_slot mmc_slot_ctrl;
extern uint8_t mmc_slot_buf[MMC_DEF_BL_LEN];
#endif

uint8_t mmc_crc7_update(uint8_t crc, uint8_t byte);
uint32_t mmc_decode_tran_speed(uint8_t tran_speed);

int mmc_start_op(uint8_t op, mmc_cmpl_cb cb);
void mmc_op_complete(bool result);
#define mmc_op_success()		mmc_op_complete(true)
#define mmc_op_failure()		mmc_op_complete(false)
#define mmc_identify_card()		mmc_start_op(MMC_OP_IDENTIFY_CARD, NULL)
#define mmc_select_card(cmpl)		mmc_start_op(MMC_OP_SELECT_CARD, cmpl)
#define mmc_deselect_card(cmpl)		mmc_start_op(MMC_OP_DESELECT_CARD, cmpl)
#define __mmc_read_blocks(cmpl)		mmc_start_op(MMC_OP_READ_BLOCKS, cmpl)
#define __mmc_write_blocks(cmpl)	mmc_start_op(MMC_OP_WRITE_BLOCKS, cmpl)
int mmc_read_blocks(uint8_t *buf, mmc_lba_t lba,
		    size_t cnt, mmc_cmpl_cb cb);

void mmc_cmd(uint8_t cmd);
void mmc_cmd_complete(uint8_t err);
#define mmc_cmd_success()	mmc_cmd_complete(MMC_ERR_NO_ERROR)
#define mmc_cmd_failure(err)	mmc_cmd_complete(err)
void mmc_rsp_complete(uint8_t err);
#define mmc_rsp_success()	mmc_rsp_complete(MMC_ERR_NO_ERROR)
#define mmc_rsp_failure(err)	mmc_rsp_complete(err)
void mmc_dat_complete(uint8_t err);
#define mmc_dat_success()	mmc_dat_complete(MMC_ERR_NO_ERROR)
#define mmc_dat_failure(err)	mmc_dat_complete(err)
void mmc_send_acmd(uint8_t acmd);
void mmc_app_cmd_complete(void);

uint8_t mmc_state_get(void);
void mmc_state_set(uint8_t state);
#define mmc_state_is(state)	(mmc_state_get() == MMC_STATE_##state)
#define mmc_state_gt(state)	(mmc_state_get() > MMC_STATE_##state && \
				 mmc_state_get() <= MMC_STATE_dis)
#define mmc_state_ge(state)	(mmc_state_get() >= MMC_STATE_##state && \
				 mmc_state_get() <= MMC_STATE_dis)
#define mmc_state_lt(state)	(mmc_state_get() < MMC_STATE_##state)
#define mmc_state_le(state)	(mmc_state_get() <= MMC_STATE_##state)
#define mmc_state_enter(state)	mmc_state_set(MMC_STATE_##state)

void mmc_event_raise(mmc_event_t event);
mmc_event_t mmc_event_save(void);
void mmc_event_restore(mmc_event_t event);
void mmc_set_block_data(uint8_t type);
uint8_t mmc_get_block_data(void);
void mmc_wait_busy(void);

mmc_card_t mmc_register_card(mmc_rca_t rca);
int mmc_card_read_async(mmc_rca_t rca, uint8_t *buf,
			mmc_lba_t lba, size_t cnt);
int mmc_card_read_sync(mmc_rca_t rca, uint8_t *buf,
		       mmc_lba_t lba, size_t cnt);

#endif /* __MMC_H_INCLUDE__ */
