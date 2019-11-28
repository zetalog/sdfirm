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
#include <errno.h>

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
#define mmc_debug_op(op)					\
	do {							\
		mmc_debug(MMC_DEBUG_OP, op);			\
	} while (0)
#else
#define mmc_debug_state(state)
#define mmc_debug_event(event)
#define mmc_debug_error(error)
#define mmc_debug_cmd(cmd)
#define mmc_debug_op(op)
#endif

#ifndef NR_MMC_SLOTS
#define NR_MMC_SLOTS		CONFIG_MMC_MAX_SLOTS
#endif
#define INVALID_MMC_SID		NR_MMC_SLOTS
#ifndef NR_MMC_CARDS
#define NR_MMC_CARDS		CONFIG_MMC_MAX_CARDS
#endif
#define INVALID_MMC_CID		NR_MMC_CARDS
#define INVALID_MMC_RCA		MMC_RCA(INVALID_MMC_CID, INVALID_MMC_SID)

#define MMC_RCA(slot, card)	MAKEWORD(card, slot)
#define MMC_SLOT(rca)		HIBYTE(rca)
#define MMC_CARD(rca)		LOBYTE(rca)

typedef uint8_t mmc_slot_t;
typedef uint8_t mmc_card_t;
typedef uint16_t mmc_rca_t;
typedef uint16_t mmc_event_t;

#include <driver/mmc.h>

/* This file is implemented according to the following standards:
 * JESD84-B41
 *  MultiMediaCard (MMC) Electrical Standard, Standard Capacity
 * JESD84-B42
 *  MultiMediaCard (MMC) Electrical Standard, High Capacity
 */

/* Table 5 — Card states and modes */
/* The values can be used as the result of MMC_CURRENT_STATE */
#define MMC_STATE_idle			0  /* idle */
#define MMC_STATE_ready			1  /* ready */
#define MMC_STATE_ident			2  /* identification */
#define MMC_STATE_stby			3  /* stand-by */
#define MMC_STATE_tran			4  /* transfer */
#define MMC_STATE_data			5  /* sending-data */
#define MMC_STATE_rcv			6  /* receive-data */
#define MMC_STATE_prg			7  /* programming */
#define MMC_STATE_dis			8  /* disconnect */
#define MMC_STATE_btst			9  /* bus test */
/* The values cannot be used as the result of MMC_CURRENT_STATE */
#define MMC_STATE_irq			10 /* wait-irq */
#define MMC_STATE_ina			11 /* inactive */

/* 7.8 commands */
/* command index */
#define MMC_CMD0			0
#define MMC_CMD1			1
#define MMC_CMD2			2
#define MMC_CMD3			3
#define MMC_CMD4			4
#define MMC_CMD6			6
#define MMC_CMD7			7
#define MMC_CMD8			8
#define MMC_CMD9			9
#define MMC_CMD10			10
#ifdef MMC_CLASS1
#define MMC_CMD11			11
#endif
#define MMC_CMD12			12
#define MMC_CMD13			13
#define MMC_CMD14			14
#define MMC_CMD15			15
#if defined(MMC_CLASS2) || defined(MMC_CLASS4)
#define MMC_CMD16			16
#endif
#ifdef MMC_CLASS2
#define MMC_CMD17			17
#define MMC_CMD18			18
#endif
#define MMC_CMD19			19
#ifdef MMC_CLASS3
#define MMC_CMD20			20
#endif
#define MMC_CMD23			23
#ifdef MMC_CLASS4
#define MMC_CMD24			24
#define MMC_CMD25			25
#define MMC_CMD26			26
#define MMC_CMD27			27
#endif
#ifdef MMC_CLASS6
#define MMC_CMD28			28
#define MMC_CMD29			29
#define MMC_CMD30			30
#endif
#ifdef MMC_CLASS5
#define MMC_CMD35			35
#define MMC_CMD36			36
#define MMC_CMD38			38
#endif
#ifdef MMC_CLASS9
#define MMC_CMD39			39
#define MMC_CMD40			40
#endif
#ifdef MMC_CLASS7
#define MMC_CMD42			42
#endif
#ifdef MMC_CLASS8
#define MMC_CMD55			55
#define MMC_CMD56			56
#endif

/* bc */
#define MMC_CMD_GO_IDLE_STATE		MMC_CMD0
#define MMC_CMD_SET_DSR			MMC_CMD4

/* bcr */
#define MMC_CMD_SEND_OP_COND		MMC_CMD1
#define MMC_CMD_ALL_SEND_CID		MMC_CMD2
#ifdef MMC_CLASS9
#define MMC_CMD_GO_IRQ_STATE		MMC_CMD40
#endif

/* ac */
#define MMC_CMD_SET_RELATIVE_ADDR	MMC_CMD3
#define MMC_CMD_SWITCH			MMC_CMD6
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
#define MMC_CMD_ERASE_GROUP_START	MMC_CMD35
#define MMC_CMD_ERASE_GROUP_END		MMC_CMD36
#define MMC_CMD_ERASE			MMC_CMD38
#endif
#ifdef MMC_CLASS9
#define MMC_CMD_FAST_IO			MMC_CMD39
#endif
#ifdef MMC_CLASS8
#define MMC_CMD_APP_CMD			MMC_CMD55
#endif

/* adtc */
#define MMC_CMD_SEND_EXT_CSD		MMC_CMD8
#ifdef MMC_CLASS1
#define MMC_CMD_READ_DAT_UNTIL_STOP	MMC_CMD11
#endif
#define MMC_CMD_BUSTEST_R		MMC_CMD14
#define MMC_CMD_BUSTEST_W		MMC_CMD19
#ifdef MMC_CLASS2
#define MMC_CMD_READ_SINGLE_BLOCK	MMC_CMD17
#define MMC_CMD_READ_MULTIPLE_BLOCK	MMC_CMD18
#endif
#ifdef MMC_CLASS3
#define MMC_CMD_WRITE_DATA_UNTIL_STOP	MMC_CMD20
#endif
#ifdef MMC_CLASS4
#define MMC_CMD_WRITE_BLOCK		MMC_CMD24
#define MMC_CMD_WRITE_MULTIPLE_BLOCK	MMC_CMD25
#define MMC_CMD_PROGRAM_CID		MMC_CMD26
#define MMC_CMD_PROGRAM_CSD		MMC_CMD27
#endif
#ifdef MMC_CLASS6
#define MMC_CMD_SEND_WRITE_PROT		MMC_CMD30
#endif
#ifdef MMC_CLASS7
#define MMC_CMD_LOCK_UNLOCK		MMC_CMD42
#endif
#ifdef MMC_CLASS8
#define MMC_CMD_GEN_CMD			MMC_CMD56
#endif

#define MMC_CMD_NONE			64
#define MMC_CMD_ARCH			65

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
#define mmc_cmd_is_program()				\
	(mmc_cmd_is(MMC_CMD_PROGRAM_CID) ||		\
	 mmc_cmd_is(MMC_CMD_PROGRAM_CSD))
#else
#define mmc_cmd_is_write_block()			false
#define mmc_cmd_is_program()				false
#endif

#if defined(MMC_CLASS2) || defined(MMC_CLASS4)
#define mmc_cmd_is_block_len()				\
	mmc_cmd_is(MMC_CMD_SET_BLOCKLEN)
#else
#define mmc_cmd_is_block_len()				false
#endif

#ifdef MMC_CLASS5
#define mmc_cmd_is_erase_group()			\
	(mmc_cmd_is(MMC_CMD_ERASE_GROUP_START) ||	\
	 mmc_cmd_is(MMC_CMD_ERASE_GROUP_END))
#define mmc_cmd_is_erase()				\
	mmc_cmd_is(MMC_CMD_ERASE)
#else
#define mmc_cmd_is_erase_group()			false
#define mmc_cmd_is_erase()				false
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
#define mmc_cmd_is_lock()	mmc_cmd_is(MMC_CMD_LOCK_UNLOCK)
#else
#define mmc_cmd_is_lock()	false
#endif

#ifdef MMC_CLASS9
#define mmc_cmd_is_fio()	mmc_cmd_is(MMC_CMD_FAST_IO)
#define mmc_cmd_is_irq()	mmc_cmd_is(MMC_CMD_GO_IRQ_STATE)
#else
#define mmc_cmd_is_fio()	false
#define mmc_cmd_is_irq()	false
#endif
#ifdef MMC_CLASS8
#define mmc_cmd_is_w()		(mmc_slot_ctrl.flags & MMC_SLOT_GEN_CMD_RDWR)
#define mmc_cmd_is_r()		(!mmc_cmd_is_w())
#define mmc_cmd_is_app()	mmc_cmd_is(MMC_CMD_APP_CMD)
#define mmc_cmd_is_gen_w()	(mmc_cmd_is_w() && mmc_cmd_is(MMC_CMD_GEN_CMD))
#define mmc_cmd_is_gen_r()	(mmc_cmd_is_r() && mmc_cmd_is(MMC_CMD_GEN_CMD))
#else
#define mmc_cmd_is_app()	false
#define mmc_cmd_is_gen_w()	false
#define mmc_cmd_is_gen_r()	false
#endif

/* 7.10 Responses */
#define MMC_R1				0
#define MMC_R1b				1
#define MMC_R2				2 /* CID, CSD */
#define MMC_R3				3 /* OCR */
#define MMC_R4				4 /* Fast I/O */
#define MMC_R5				5 /* Interrupt request */

/* 7.11 Card status
 * The response format R1 contains a 32-bit field named card status.
 */
/* Table 25 — Detection mode bit descriptions */
/* Error bits */
#define MMC_DET_ADDRESS_OUT_OF_RANGE	_BV(31)
#define MMC_DET_ADDRESS_MISALIGN	_BV(30)
#define MMC_DET_BLOCK_LEN_ERROR		_BV(29)
#define MMC_DET_ERASE_SEQ_ERROR		_BV(28)
#define MMC_DET_ERASE_PARAM		_BV(27)
#define MMC_DET_WP_VIOLATION		_BV(26)
#define MMC_DET_LOCK_UNLOCK_FAILED	_BV(24)
#define MMC_DET_COM_CRC_ERROR		_BV(23)
#define MMC_DET_ILLEGAL_COMMAND		_BV(22)
#define MMC_DET_CARD_ECC_FAILED		_BV(21)
#define MMC_DET_CC_ERROR		_BV(20)
#define MMC_DET_ERROR			_BV(19)
#define MMC_DET_UNDERRUN		_BV(18)
#define MMC_DET_OVERRUN			_BV(17)
#define MMC_DET_CID_CSD_OVERWRITE	_BV(16)
#define MMC_DET_WP_ERASE_SKIP		_BV(15)
#define MMC_DET_ERASE_RESET		_BV(13)
#define MMC_DET_SWITCH_ERROR		_BV(7)
/* Status bits */
#define MMC_DET_CARD_IS_LOCKED		_BV(25)
#define MMC_DET_CURRENT_STATE_OFFSET	9
#define MMC_DET_CURRENT_STATE_MASK	REG_4BIT_MASK
#define MMC_DET_CURRENT_STATE(value)	_GET_FV(MMC_DET_CURRENT_STATE, value)
#define MMC_DET_READY_FOR_DATA		_BV(8)
#define MMC_DET_APP_CMD			_BV(5)

/* 8 Card registers */
/* 8.1 OCR register - Operation Condition Register */
#define MMC_OCR_170_195			_BV(7)
#define MMC_OCR_20_26_OFFSET		8
#define MMC_OCR_20_26_MASK		REG_7BIT_MASK
#define MMC_OCR_GET_20_26(value)	_GET_FV(MMC_OCR_20_26, value)
#define MMC_OCR_SET_20_26(value)	_SET_FV(MMC_OCR_20_26, value)
#define MMC_OCR_27_36_OFFSET		15
#define MMC_OCR_27_36_MASK		REG_9BIT_MASK
#define MMC_OCR_GET_27_36(value)	_GET_FV(MMC_OCR_27_36, value)
#define MMC_OCR_SET_27_36(value)	_SET_FV(MMC_OCR_27_36, value)
#define MMC_OCR_HIGH_VOLTAGE		MMC_OCR_27_36_MASK
#define MMC_OCR_DUAL_VOLTAGE		(MMC_OCR_170_195 | MMC_OCR_27_36_MASK)
#define MMC_OCR_165_195	MMC_OCR_170_195	     /* VDD voltage 1.65 - 1.95 */
#define MMC_OCR_20_21	MMC_OCR_SET_20_26(0) /* VDD voltage 2.0 ~ 2.1 */
#define MMC_OCR_21_22	MMC_OCR_SET_20_26(1) /* VDD voltage 2.1 ~ 2.2 */
#define MMC_OCR_22_23	MMC_OCR_SET_20_26(2) /* VDD voltage 2.2 ~ 2.3 */
#define MMC_OCR_23_24	MMC_OCR_SET_20_26(3) /* VDD voltage 2.3 ~ 2.4 */
#define MMC_OCR_24_25	MMC_OCR_SET_20_26(4) /* VDD voltage 2.4 ~ 2.5 */
#define MMC_OCR_25_26	MMC_OCR_SET_20_26(5) /* VDD voltage 2.5 ~ 2.6 */
#define MMC_OCR_26_27	MMC_OCR_SET_20_26(6) /* VDD voltage 2.6 ~ 2.7 */
#define MMC_OCR_27_28	MMC_OCR_SET_27_36(0) /* VDD voltage 2.7 ~ 2.8 */
#define MMC_OCR_28_29	MMC_OCR_SET_27_36(1) /* VDD voltage 2.8 ~ 2.9 */
#define MMC_OCR_29_30	MMC_OCR_SET_27_36(2) /* VDD voltage 2.9 ~ 3.0 */
#define MMC_OCR_30_31	MMC_OCR_SET_27_36(3) /* VDD voltage 3.0 ~ 3.1 */
#define MMC_OCR_31_32	MMC_OCR_SET_27_36(4) /* VDD voltage 3.1 ~ 3.2 */
#define MMC_OCR_32_33	MMC_OCR_SET_27_36(5) /* VDD voltage 3.2 ~ 3.3 */
#define MMC_OCR_33_34   MMC_OCR_SET_27_36(6) /* VDD voltage 3.3 ~ 3.4 */
#define MMC_OCR_34_35	MMC_OCR_SET_27_36(7) /* VDD voltage 3.4 ~ 3.5 */
#define MMC_OCR_35_36	MMC_OCR_SET_27_36(8) /* VDD voltage 3.5 ~ 3.6 */

#define MMC_OCR_ACCESS_MODE_OFFSET	29
#define MMC_OCR_ACCESS_MODE_MASK	REG_2BIT_MASK
#define MMC_OCR_GET_ACCESS_MODE(value)	_GET_FV(MMC_OCR_ACCESS_MODE, value)
#define MMC_OCR_SET_ACCESS_MODE(value)	_SET_FV(MMC_OCR_ACCESS_MODE, value)
#define MMC_OCR_BYTE_MODE		0
#define MMC_OCR_SECTOR_MODE		2
#define MMC_OCR_BUSY			_BV(31)

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

/* 8.3 CSD register - Card Specific Data register
 *
 * The CSD is defined using the following field registers:
 * CSD3[32-bit] | CSD21[64-bit] | CSD0[32-bit]
 */
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
#define MMC_CSD21_CCC_OFFSET		52
#define MMC_CSD21_CCC_MASK		REG_12BIT_MASK
#define MMC_CSD21_CCC(value)		_GET_FV_ULL(MMC_CSD21_CCC, value)
#define MMC_CSD21_READ_BL_LEN_OFFSET	48
#define MMC_CSD21_READ_BL_LEN_MASK	REG_4BIT_MASK
#define MMC_CSD21_READ_BL_LEN(value)	_GET_FV_ULL(MMC_CSD21_READ_BL_LEN, value)
#define MMC_CSD21_READ_BL_PARTIAL	_BV_ULL(47)
#define MMC_CSD21_WRITE_BLK_MISALIGN	_BV_ULL(46)
#define MMC_CSD21_READ_BLK_MISALIGN	_BV_ULL(45)
#define MMC_CSD21_DSR_IMP		_BV_ULL(44)
#define MMC_CSD21_C_SIZE_OFFSET		30
#define MMC_CSD21_C_SIZE_MASK		REG_12BIT_MASK
#define MMC_CSD21_C_SIZE(value)		_GET_FV_ULL(MMC_CSD21_C_SIZE, value)
#define MMC_CSD21_VDD_R_CURR_MIN_OFFSET	27
#define MMC_CSD21_VDD_R_CURR_MIN_MASK	REG_3BIT_MASK
#define MMC_CSD21_VDD_R_CURR_MIN(value)	_GET_FV_ULL(MMC_CSD21_VDD_R_CURR_MIN, value)
#define MMC_CSD21_VDD_R_CURR_MAX_OFFSET	24
#define MMC_CSD21_VDD_R_CURR_MAX_MASK	REG_3BIT_MASK
#define MMC_CSD21_VDD_R_CURR_MAX(value)	_GET_FV_ULL(MMC_CSD21_VDD_R_CURR_MAX, value)
#define MMC_CSD21_VDD_W_CURR_MIN_OFFSET	21
#define MMC_CSD21_VDD_W_CURR_MIN_MASK	REG_3BIT_MASK
#define MMC_CSD21_VDD_W_CURR_MIN(value)	_GET_FV_ULL(MMC_CSD21_VDD_W_CURR_MIN, value)
#define MMC_CSD21_VDD_W_CURR_MAX_OFFSET	18
#define MMC_CSD21_VDD_W_CURR_MAX_MASK	REG_3BIT_MASK
#define MMC_CSD21_VDD_W_CURR_MAX(value)	_GET_FV_ULL(MMC_CSD21_VDD_W_CURR_MAX, value)
#define MMC_CSD21_C_SIZE_MULT_OFFSET	15
#define MMC_CSD21_C_SIZE_MULT_MASK	REG_3BIT_MASK
#define MMC_CSD21_C_SIZE_MULT(value)	_GET_FV_ULL(MMC_CSD21_C_SIZE_MULT, value)
#define MMC_CSD21_ERASE_GRP_SIZE_OFFSET	10
#define MMC_CSD21_ERASE_GRP_SIZE_MASK	REG_5BIT_MASK
#define MMC_CSD21_ERASE_GRP_SIZE(value)	_GET_FV_ULL(MMC_CSD21_ERASE_GRP_SIZE, value)
#define MMC_CSD21_ERASE_GRP_MULT_OFFSET	5
#define MMC_CSD21_ERASE_GRP_MULT_MASK	REG_5BIT_MASK
#define MMC_CSD21_ERASE_GRP_MULT(value)	_GET_FV_ULL(MMC_CSD21_ERASE_GRP_MULT, value)
#define MMC_CSD21_WP_GRP_SIZE_OFFSET	0
#define MMC_CSD21_WP_GRP_SIZE_MASK	REG_5BIT_MASK
#define MMC_CSD21_WP_GRP_SIZE(value)	_GET_FV_ULL(MMC_CSD21_WP_GRP_SIZE, value)
#define MMC_CSD0_WP_GRP_ENABLE		_BV(31)
#define MMC_CSD0_DEFAULT_ECC_OFFSET	29
#define MMC_CSD0_DEFAULT_ECC_MASK	REG_2BIT_MASK
#define MMC_CSD0_DEFAULT_ECC(value)	_GET_FV(MMC_CSD0_DEFAULT_ECC, value)
#define MMC_CSD0_R2W_FACTOR_OFFSET	26
#define MMC_CSD0_R2W_FACTOR_MASK	REG_3BIT_MASK
#define MMC_CSD0_R2W_FACTOR(value)	_GET_FV(MMC_CSD0_R2W_FACTOR, value)
#define MMC_CSD0_WRITE_BL_LEN_OFFSET	22
#define MMC_CSD0_WRITE_BL_LEN_MASK	REG_4BIT_MASK
#define MMC_CSD0_WRITE_BL_LEN(value)	_GET_FV(MMC_CSD0_WRITE_BL_LEN, value)
#define MMC_CSD0_WRITE_BL_PARTIAL	_BV(21)
/* R/W fields */
#define MMC_CSD0_CONTENT_PROT_APP	_BV(16)
#define MMC_CSD0_FILE_FORMAT_GRP	_BV(15)
#define MMC_CSD0_COPY			_BV(14)
#define MMC_CSD0_PERM_WRITE_PROTECT	_BV(13)
#define MMC_CSD0_TMP_WRITE_PROTECT	_BV(12)
#define MMC_CSD0_FILE_FORMAT_OFFSET	10
#define MMC_CSD0_FILE_FORMAT_MASK	REG_2BIT_MASK
#define MMC_CSD0_GET_FILE_FORMAT(value)	_GET_FV(MMC_CSD0_FILE_FORMAT, value)
#define MMC_CSD0_SET_FILE_FORMAT(value)	_GET_FV(MMC_CSD0_FILE_FORMAT, value)
#define MMC_CSD0_ECC_OFFSET		8
#define MMC_CSD0_ECC_MASK		REG_2BIT_MASK
#define MMC_CSD0_GET_ECC(value)		_GET_FV(MMC_CSD0_ECC, value)
#define MMC_CSD0_SET_ECC(value)		_SET_FV(MMC_CSD0_ECC, value)
#define MMC_CSD0_CRC_OFFSET		8
#define MMC_CSD0_CRC_MASK		REG_7BIT_MASK
#define MMC_CSD0_GET_CRC(value)		_GET_FV(MMC_CSD0_CRC, value)
#define MMC_CSD0_SET_CRC(value)		_SET_FV(MMC_CSD0_CRC, value)

typedef struct {
	uint32_t csd3;
	uint64_t csd21;
	uint32_t csd0;
} __packed mmc_csd_t;

typedef uint8_t mmc_r1_t[4];
typedef uint8_t mmc_r2_t[16];
typedef uint8_t mmc_r3_t[4];

/* 8.4 Extended CSD register */

/*===========================================================================
 * MMC Slot
 *===========================================================================*/
typedef void (*mmc_cmpl_cb)(bool result);

struct mmc_slot {
	uint8_t state;
	uint8_t cmd;
	uint8_t err;
	uint8_t op;
	uint8_t flags;
#define MMC_SLOT_CARD_STATUS_VALID	_BV(0) /* R1 */
#define MMC_SLOT_CARD_IS_BUSY		_BV(1) /* R1b */
#define MMC_SLOT_GEN_CMD_RDWR		_BV(2)
	mmc_event_t event;
	uint16_t block_cnt;
	uint32_t block_len;
	uint32_t address;
	/* R1 */
	uint32_t card_status;
	/* R2 */
	mmc_cid_t cid;
	mmc_csd_t csd;
	/* R3 */
	uint32_t ocr;
	uint16_t dsr;
	mmc_cmpl_cb op_cb;
};
#define mmc_op_is(_op)			(!!(mmc_slot_ctrl.op == (_op)))
#define mmc_cmd_is(_cmd)		(!!(mmc_slot_ctrl.cmd == (_cmd)))
#define mmc_err_is(_err)		(!!(mmc_slot_ctrl.err == (_err)))

#define MMC_OP_NO_OP			0
#define MMC_OP_IDENTIFY_CARD		1
#define MMC_OP_READ_BLOCKS		2
#define MMC_OP_WRITE_BLOCKS		3

#define MMC_ERR_NO_ERROR		0
#define MMC_ERR_CARD_IS_BUSY		1 /* card is busy */
#define MMC_ERR_HOST_OMIT_VOLT		2 /* host omits voltage range */
#define MMC_ERR_CARD_NON_COMP_VOLT	3 /* card with non compatible voltage range */
#define MMC_ERR_CARD_LOOSE_BUS		4 /* card looses bus */

#ifdef CONFIG_DEBUG_PRINT
#define mmc_debug(tag, val)	dbg_print((tag), (val))
#endif
#ifdef CONFIG_CONSOLE
void mmc_debug(uint8_t tag, uint32_t val);
#endif

#if NR_MMC_SLOTS > 1
mmc_rca_t mmc_rca_save(mmc_rca_t rca);
void mmc_rca_restore(mmc_rca_t rca);
extern mmc_rca_t mmc_rca;
extern struct mmc_slot mmc_slots[NR_MMC_SLOTS];
#define mmc_slot_ctrl mmc_slots[mmc_rca]
#else
#define mmc_rca_save(sid)	0
#define mmc_rca_restore(rca)
#define mmc_rca			0
extern struct mmc_slot mmc_slot_ctrl;
#endif

uint8_t mmc_crc7_update(uint8_t crc, uint8_t byte);

int mmc_start_op(uint8_t op, mmc_cmpl_cb cb);
void mmc_op_complete(bool result);
#define mmc_op_success()	mmc_op_complete(true)
#define mmc_op_failure()	mmc_op_complete(false)
#define mmc_identify_card()	mmc_start_op(MMC_OP_IDENTIFY_CARD, NULL)

void mmc_send_cmd(uint8_t cmd);
void mmc_cmd_complete(uint8_t err);
#define mmc_cmd_success()	mmc_cmd_complete(MMC_ERR_NO_ERROR)
#define mmc_cmd_failure(err)	mmc_cmd_complete(err)

uint8_t mmc_state_get(void);
void mmc_state_set(uint8_t state);
#define mmc_state_is(state)	(mmc_state_get() == MMC_STATE_##state)
#define mmc_state_gt(state)	(mmc_state_get() > MMC_STATE_##state)
#define mmc_state_ge(state)	(mmc_state_get() >= MMC_STATE_##state)
#define mmc_state_lt(state)	(mmc_state_get() < MMC_STATE_##state)
#define mmc_state_le(state)	(mmc_state_get() <= MMC_STATE_##state)
#define mmc_state_enter(state)	mmc_state_set(MMC_STATE_##state)

#endif /* __MMC_H_INCLUDE__ */
