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
 * @(#)sd.h: secure digital (SD) definitions
 * $Id: sd.h,v 1.1 2019-10-25 13:40:00 zhenglv Exp $
 */

#ifndef __SD_H_INCLUDE__
#define __SD_H_INCLUDE__

#include <target/config.h>
#include <target/compiler.h>
#include <target/generic.h>
#include <errno.h>
#include <target/mmc.h>

/* 7.8 commands */
/* command index */
#define SD_CMD0				0
#define SD_CMD1				1
#define SD_CMD2				2
#define SD_CMD3				3
#define SD_CMD4				4
#define SD_CMD6				6
#define SD_CMD7				7
#define SD_CMD8				8
#define SD_CMD9				9
#define SD_CMD10			10
#define SD_CMD11			11
#define SD_CMD12			12
#define SD_CMD13			13
#define SD_CMD14			14
#define SD_CMD15			15
#define SD_CMD16			16
#define SD_CMD17			17
#define SD_CMD18			18
#define SD_CMD19			19
#define SD_CMD20			20
#define SD_CMD23			23
#define SD_CMD24			24
#define SD_CMD25			25
#define SD_CMD26			26
#define SD_CMD27			27
#define SD_CMD28			28
#define SD_CMD29			29
#define SD_CMD30			30
#define SD_CMD35			35
#define SD_CMD36			36
#define SD_CMD38			38
#define SD_CMD39			39
#define SD_CMD40			40
#define SD_CMD42			42
#define SD_CMD55			55
#define SD_CMD56			56

/* bc */
#define SD_CMD_GO_IDLE_STATE		SD_CMD0  /* MMC */
#define SD_CMD_SET_DSR			SD_CMD4  /* MMC */
#define SD_CMD_SEND_IF_COND		SD_CMD8

/* bcr */
#define SD_CMD_ALL_SEND_CID		SD_CMD2  /* MMC */
#define SD_CMD_SEND_RELATIVE_ADDR	SD_CMD3

#define SD_CMD_GO_IRQ_STATE		SD_CMD40

/* ac */
#define SD_CMD_SELECT_DESELECT_CARD	SD_CMD7  /* MMC */
#define SD_CMD_SEND_CSD			SD_CMD9  /* MMC */
#define SD_CMD_SEND_CID			SD_CMD10 /* MMC */
#define SD_CMD_VOLTAGE_SWITCH		SD_CMD11
#define SD_CMD_STOP_TRANSMISSION	SD_CMD12 /* MMC */
#define SD_CMD_SEND_STATUS		SD_CMD13 /* MMC */
#define SD_CMD_GO_INACTIVE_STATE	SD_CMD15 /* MMC */
#define SD_CMD_SET_BLOCKLEN		SD_CMD16
#define SD_CMD_SPEED_CLASS_CONTROL	SD_CMD20
#define SD_CMD_SET_BLOCK_COUNT		SD_CMD23 /* MMC */
#define SD_CMD_SET_WRITE_PROT		SD_CMD28 /* MMC */
#define SD_CMD_CLR_WRITE_PROT		SD_CMD29 /* MMC */
#define SD_CMD_ERASE_WR_BLK_START	SD_CMD32
#define SD_CMD_ERASE_WR_BLK_END		SD_CMD33
#define SD_CMD_ERASE			SD_CMD38 /* MMC */
#define SD_CMD_APP_CMD			SD_CMD55 /* MMC */
#ifdef SD_CLASS1
#define SD_CMD_Q_MANAGEMENT		SD_CMD43
#define SD_CMD_Q_TASK_INFO_A		SD_CMD44
#define SD_CMD_Q_TASK_INFO_B		SD_CMD45
#endif

/* adtc */
#define SD_CMD_READ_SINGLE_BLOCK	SD_CMD17 /* MMC */
#define SD_CMD_READ_MULTIPLE_BLOCK	SD_CMD18 /* MMC */
#define SD_CMD_SEND_TUNING_BLOCK	SD_CMD19
#define SD_CMD_WRITE_BLOCK		SD_CMD24 /* MMC */
#define SD_CMD_WRITE_MULTIPLE_BLOCK	SD_CMD25 /* MMC */
#define SD_CMD_PROGRAM_CSD		SD_CMD27
#define SD_CMD_SEND_WRITE_PROT		SD_CMD30 /* MMC */
#define SD_CMD_GEN_CMD			SD_CMD56 /* MMC */
#ifdef SD_CLASS10
#define SD_CMD_SWITCH_FUNC		SD_CMD6  /* 1.10 */
#endif
#ifdef SD_CLASS11
#define SD_CMD_READ_EXTRA_SINGLE	SD_CMD48
#define SD_CMD_WRITE_EXTRA_SINGLE	SD_CMD49
#define SD_CMD_READ_EXTRA_MULTI		SD_CMD58
#define SD_CMD_WRITE_EXTRA_MULTI	SD_CMD59
#endif
#ifdef SD_CLASS1
#define SD_CMD_Q_RD_TASK		SD_CMD46
#define SD_CMD_Q_WR_TASK		SD_CMD47
#endif

#define SD_CMD_NONE			64
#define SD_CMD_ARCH			65

/* Application commands */
#define SD_ACMD6			6
#define SD_ACMD13			13
#define SD_ACMD22			22
#define SD_ACMD23			23
#define SD_ACMD41			41
#define SD_ACMD42			42
#define SD_ACMD51			51

/* bcr */
#define SD_ACMD_SEND_OP_COND		SD_ACMD41

/* ac */
#define SD_ACMD_SET_BUS_WIDTH		SD_ACMD6
#define SD_ACMD_SET_WR_BLK_ERASE_COUNT	SD_ACMD23
#define SD_ACMD_SET_CLR_CARD_DETECT	SD_ACMD42

/* adtc */
#define SD_ACMD_SD_STATUS		SD_ACMD13
#define SD_ACMD_SEND_NUM_WR_BLOCKS	SD_ACMD22
#define SD_ACMD_SEND_SCR		SD_ACMD51

#endif /* __SD_H_INCLUDE__ */
