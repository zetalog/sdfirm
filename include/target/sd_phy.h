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
 * @(#)sd_phy.h: secure digital (SD) physical layer definitions
 * $Id: sd_phy.h,v 1.1 2019-10-25 13:40:00 zhenglv Exp $
 */

#ifndef __SD_PHY_H_INCLUDE__
#define __SD_PHY_H_INCLUDE__

#ifdef SD_CLASS2
#define MMC_CLASS2	1
#endif
#ifdef SD_CLASS7
#define MMC_CLASS4	1
#endif
#ifdef SD_CLASS5
#define MMC_CLASS5	1
#endif
#ifdef SD_CLASS6
#define MMC_CLASS6	1
#endif
#ifdef SD_CLASS7
#define MMC_CLASS7	1
#endif
#ifdef SD_CLASS8
#define MMC_CLASS8	1
#endif

#define SD_PHY_VERSION_10		10
#define SD_PHY_VERSION_20		20
#define SD_PHY_VERSION_30		30
#define SD_PHY_VERSION_40		40
#ifndef SD_PHY_VERSION
#define SD_PHY_VERSION			(CONFIG_SD_PHY_VERSION * 10)
#endif
#define SD_PHY_VERSION_UNKNOWN		0

#if defined(CONFIG_SD_SDHC) || defined(CONFIG_SD_SDXC)
#define SD_HCS				1
#else
#define SD_HCS				0
#endif

/* Bus speed mode */
#define SD_SPEED_DEFAULT		0 /* 3.3V 25MHz 12.5Mbps */
#define SD_SPEED_HIGH			1 /* 3.3V 50MHz 25Mbps */
#define SD_SPEED_SDR12			2 /* UHS-I 1.8V 25MHz 12.5Mbps */
#define SD_SPEED_SDR25			3 /* UHS-I 1.8V 50MHz 25Mbps */
#define SD_SPEED_SDR50			4 /* UHS-I 1.8V 100MHz 50Mbps */
#define SD_SPEED_SDR104			5 /* UHS-I 1.8V 208MHz 104Mbps */
#define SD_SPEED_DDR50			6 /* UHS-I 1.8V 50MHz 50Mbps */
#define SD_SPEED_UHS156			7 /* UHS-II 26-52MHz 1.56Gbps */
#define SD_SPEED_UHS624			8 /* UHS-II 26-52MHz 6.24Gbps */

/* Used for SD_CMD_SEND_IF_COND */
#define SD_CHECK_PATTERN		0xAA

/* bc */
#define SD_CMD_SEND_IF_COND		MMC_CMD8

/* bcr */
#define SD_CMD_SEND_RELATIVE_ADDR	MMC_CMD3

/* ac */
#define SD_CMD_VOLTAGE_SWITCH		MMC_CMD11
#if defined(SD_CLASS2) || defined(SD_CLASS4)
#define SD_CMD_SPEED_CLASS_CONTROL	MMC_CMD20
#endif
#ifdef SD_CLASS5
#define SD_CMD_ERASE_WR_BLK_START	MMC_CMD32
#define SD_CMD_ERASE_WR_BLK_END		MMC_CMD33
#endif
#ifdef SD_CLASS1
#define SD_CMD_Q_MANAGEMENT		MMC_CMD43
#define SD_CMD_Q_TASK_INFO_A		MMC_CMD44
#define SD_CMD_Q_TASK_INFO_B		MMC_CMD45
#endif

/* adtc */
#ifdef SD_CLASS2
#define SD_CMD_SEND_TUNING_BLOCK	MMC_CMD19
#endif
#ifdef SD_CLASS10
#define SD_CMD_SWITCH_FUNC		MMC_CMD6  /* 1.10 */
#endif
#ifdef CONFIG_SD_SPI
#define SD_CMD_READ_OCR			MMC_CMD58
#else /* CONFIG_SD_SPI */
#ifdef SD_CLASS11
#define SD_CMD_READ_EXTRA_SINGLE	MMC_CMD48
#define SD_CMD_WRITE_EXTRA_SINGLE	MMC_CMD49
#define SD_CMD_READ_EXTRA_MULTI		MMC_CMD58
#define SD_CMD_WRITE_EXTRA_MULTI	MMC_CMD59
#endif
#endif /* CONFIG_SD_SPI */
#ifdef SD_CLASS1
#define SD_CMD_Q_RD_TASK		MMC_CMD46
#define SD_CMD_Q_WR_TASK		MMC_CMD47
#endif

/* bcr */
#define SD_ACMD_SEND_OP_COND		MMC_ACMD41

/* ac */
#define SD_ACMD_SET_BUS_WIDTH		MMC_ACMD6
#define SD_ACMD_SET_WR_BLK_ERASE_COUNT	MMC_ACMD23
#define SD_ACMD_SET_CLR_CARD_DETECT	MMC_ACMD42

/* adtc */
#define SD_ACMD_SD_STATUS		MMC_ACMD13
#define SD_ACMD_SEND_NUM_WR_BLOCKS	MMC_ACMD22
#define SD_ACMD_SEND_SCR		MMC_ACMD51

/* SD specific response types */
#define SD_R6		(MMC_RSP_TYPE(6)|MMC_R_CS)
#define SD_R7		(MMC_RSP_TYPE(7)|MMC_R_CS)

/* SD specific states */
#define SD_STATE_ver		9  /* After CMD8, SPEC version identified */

typedef uint8_t sd_r6_t[4];
typedef uint8_t sd_r7_t[4];

/* 4.10.1 CSR register - Card Status register */
#define SD_DET_CARD_ECC_DISABLED	_BV(14)
#define SD_DET_FX_EVENT			_BV(6)
#define SD_DET_AKE_SEQ_ERROR		_BV(3)

/* 4.10.2 SSR register - SD Status */
/* SSR15 */
#define SD_SSR15_DATA_BUS_WIDTH_OFFSET	30
#define SD_SSR15_DATA_BUS_WIDTH_MASK	REG_2BIT_MASK
#define SD_SSR15_DATA_BUS_WIDTH(value)	_GET_FV(SD_SSR15_DATA_BUS_WIDTH, value)
#define SD_SSR15_SECURE_MODE		_BV(29)
#define SD_SSR15_SD_CARD_TYPE_OFFSET	0
#define SD_SSR15_SD_CARD_TYPE_MASK	REG_16BIT_MASK
#define SD_SSR15_SD_CARD_TYPE(value)	_GET_FV(SD_SSR15_SD_CARD_TYPE, value)
/* SSR14 is SIZE_OFF_PROTECTED_AREA */
/* SSR13 */
#define SD_SSR13_SPEED_CLASS_OFFSET	24
#define SD_SSR13_SPEED_CLASS_MASK	REG_8BIT_MASK
#define SD_SSR13_SPEED_CLASS(value)	_GET_FV(SD_SSR13_SPEED_CLASS, value)
#define SD_SSR13_PERFORMANCE_MOVE_OFFSET	16
#define SD_SSR13_PERFORMANCE_MOVE_MASK		REG_8BIT_MASK
#define SD_SSR13_PERFORMANCE_MOVE(value)	\
	_GET_FV(SD_SSR13_PERFORMANCE_MOVE, value)
#define SD_SSR13_ERASE_SIZE_HI_OFFSET	0
#define SD_SSR13_ERASE_SIZE_HI_MASK	REG_8BIT_MASK
#define SD_SSR13_ERASE_SIZE_HI(value)	_GET_FV(SD_SSR13_ERASE_SIZE_HI, value)
/* SSR12 */
#define SD_SSR12_ERASE_SIZE_LO_OFFSET	24
#define SD_SSR12_ERASE_SIZE_LO_MASK	REG_8BIT_MASK
#define SD_SSR12_ERASE_SIZE_LO(value)	_GET_FV(SD_SSR12_ERASE_SIZE_LO, value)
#define SD_SSR12_ERASE_TIMEOUT_OFFSET	18
#define SD_SSR12_ERASE_TIMEOUT_MASK	REG_6BIT_MASK
#define SD_SSR12_ERASE_TIMEOUT(value)	_GET_FV(SD_SSR12_ERASE_TIMEOUT, value)
#define SD_SSR12_ERASE_OFFSET_OFFSET	16
#define SD_SSR12_ERASE_OFFSET_MASK	REG_2BIT_MASK
#define SD_SSR12_ERASE_OFFSET(value)	_GET_FV(SD_SSR12_ERASE_OFFSET, value)
#define SD_SSR12_UHS_SPEED_GRADE_OFFSET	12
#define SD_SSR12_UHS_SPEED_GRADE_MASK	REG_4BIT_MASK
#define SD_SSR12_UHS_SPEED_GRADE(value)	_GET_FV(SD_SSR12_UHS_SPEED_GRADE, value)
#define SD_SSR12_UHS_AU_SIZE_OFFSET	8
#define SD_SSR12_UHS_AU_SIZE_MASK	REG_4BIT_MASK
#define SD_SSR12_UHS_AU_SIZE(value)	_GET_FV(SD_SSR12_UHS_AU_SIZE, value)
#define SD_SSR12_VIDEO_SPEED_CLASS_OFFSET	\
#define SD_SSR12_VIDEO_SPEED_CLASS_MASK		REG_8BIT_MASK
#define SD_SSR12_VIDEO_SPEED_CLASS(value)	\
	_GET_FV(SD_SSR12_VIDEO_SPEED_CLASS, value)
/* SSR11 */
#define SD_SSR11_VSC_AU_SIZE_OFFSET	16
#define SD_SSR11_VSC_AU_SIZE_MASK	REG_10BIT_MASK
#define SD_SSR11_VSC_AU_SIZE(value)	_GET_FV(SD_SSR11_VSC_AU_SIZE, value)
#define SD_SSR11_SUS_ADDR_HI_OFFSET	0
#define SD_SSR11_SUS_ADDR_HI_MASK	REG_16BIT_MASK
#define SD_SSR11_SUS_ADDR_HI(value)	_GET_FV(SD_SSR11_SUS_ADDR_HI, value)
/* SSR10 */
#define SD_SSR10_SUS_ADDR_LO_OFFSET	26
#define SD_SSR10_SUS_ADDR_LO_MASK	REG_6BIT_MASK
#define SD_SSR10_SUS_ADDR_LO(value)	_GET_FV(SD_SSR10_SUS_ADDR_LO, value)
#define SD_SSR10_APP_PERF_CLASS_OFFSET	16
#define SD_SSR10_APP_PERF_CLASS_MASK	REG_4BIT_MASK
#define SD_SSR10_APP_PERF_CLASS(value)	_GET_FV(SD_SSR10_APP_PERF_CLASS, value)
#define SD_SSR10_PERFORMANCE_ENHANCE_OFFSET	8
#define SD_SSR10_PERFORMANCE_ENHANCE_MASK	REG_8BIT_MASK
#define SD_SSR10_PERFORMANCE_ENHANCE(value)	\
	_GET(SD_SSR10_PERFORMANCE_ENHANCE, value)
/* SSR9 */
#define SD_SSR9_DISCARD_SUPPORT		_BV(25)
#define SD_SSR9_FULE_SUPPORT		_BV(24)

typedef struct {
	uint16_t sd_card_type;
	uint8_t speed_class;
	uint8_t dat_bus_width : 2;
	uint8_t au_size : 4;
	uint32_t size_of_protected_area;
	uint16_t erase_size;
	uint8_t erase_timeout : 6;
	uint8_t erase_offset : 2;
	uint8_t uhs_speed_grade : 4;
	uint8_t uhs_au_size : 4;
	uint8_t performance_move;
	uint8_t performance_enhance;
	uint8_t app_perf_class : 4;
	uint8_t secure_mode : 1;
	uint8_t discard_support : 1;
	uint8_t fule_support : 1;
} sd_ssr_t;

/* 5.1 OCR register - Operation Conditions register */
#define SD_OCR_S18A			_BV(24)
#define SD_OCR_UHSCS			_BV(29) /* UHS-II Card Status */
#define SD_OCR_CCS			_BV(30) /* Card Capacity Status */

/* 5.2 CID register - Card IDentification register */
typedef struct {
	uint8_t mid;	/* manuafacturer ID */
	uint16_t oid;	/* OEM/application ID */
#ifdef CONFIG_MMC_CID_ALL_FIELDS
	uint8_t pnm[5];	/* product name */
	uint8_t prv;	/* product revision */
	uint32_t psn;	/* product serial number */
	uint16_t mdt;	/* manuafacturing date */
	uint8_t crc;
#endif
} __packed sd_cid_t;

/* 5.3.3 CSD Register (CSD Version 2.0) */
#define SD_CSD3_CSD_VERSION_1_0		0
#define SD_CSD3_CSD_VERSION_2_0		1
/* CSD2 */
#define SD_CSD20_2_C_SIZE_OFFSET	0
#define SD_CSD20_2_C_SIZE_MASK		REG_6BIT_MASK
#define SD_CSD20_2_C_SIZE(value)	_GET_FV(SD_CSD20_2_C_SIZE, value)
/* CSD1 */
#define SD_CSD1_ERASE_BLK_EN		_BV(14)
#define SD_CSD1_SECTOR_SIZE_OFFSET	7
#define SD_CSD1_SECTOR_SIZE_MASK	REG_7BIT_MASK
#define SD_CSD1_SECTOR_SIZE(value)	_GET_FV(SD_CSD1_SECTOR_SIZE, value)
#define SD_CSD1_WP_GRP_SIZE_OFFSET	0
#define SD_CSD1_WP_GRP_SIZE_MASK	REG_7BIT_MASK
#define SD_CSD20_1_C_SIZE_OFFSET	16
#define SD_CSD20_1_C_SIZE_MASK		REG_16BIT_MASK
#define SD_CSD20_1_C_SIZE(value)	_GET_FV(SD_CSD20_1_C_SIZE, value)

/* 5.6 SCR register (SD Card Configuration)
 *
 * The SCR is defined using the following field registers:
 * | SCR1[32-bit] | SCR0[32-bit]
 */
#define SD_SCR1_SCR_STRUCTURE_OFFSET	28
#define SD_SCR1_SCR_STRUCTURE_MASK	REG_4BIT_MASK
#define SD_SCR1_SCR_STRUCTURE(value)	_GET_FV(SD_SCR1_SCR_STRUCTURE, value)
#define SD_SCR1_SD_SPEC_OFFSET		24
#define SD_SCR1_SD_SPEC_MASK		REG_4BIT_MASK
#define SD_SCR1_SD_SPEC(value)		_GET_FV(SD_SCR1_SD_SPEC, value)
#define SD_SCR1_DATA_STAT_AFTER_ERASE	_BV(23)
#define SD_SCR1_SD_SECURITY_OFFSET	20
#define SD_SCR1_SD_SECURITY_MASK	REG_3BIT_MASK
#define SD_SCR1_SD_SECURITY(value)	_GET_FV(SD_SCR1_SD_SECURITY, value)
#define SD_SCR1_SD_BUS_WIDTHS_OFFSET	16
#define SD_SCR1_SD_BUS_WIDTHS_MASK	REG_4BIT_MASK
#define SD_SCR1_SD_BUS_WIDTHS(value)	_GET_FV(SD_SCR1_SD_BUS_WIDTHS, value)
#define SD_BUS_WIDTH_1BIT_SUPPORT	_BV(0)
#define SD_BUS_WIDTH_4BIT_SUPPORT	_BV(2)
#define SD_SCR1_SD_SPEC3		_BV(15)
#define SD_SCR1_EX_SECURITY_OFFSET	11
#define SD_SCR1_EX_SECURITY_MASK	REG_4BIT_MASK
#define SD_SCR1_EX_SECURITY(value)	_GET_FV(SD_SCR1_EX_SECURITY, value)
#define SD_SCR1_SD_SPEC4		_BV(10)
#define SD_SCR1_SD_SPECX_OFFSET		6
#define SD_SCR1_SD_SPECX_MASK		REG_4BIT_MASK
#define SD_SCR1_SD_SPECX(value)		_GET_FV(SD_SCR1_SD_SPECX, value)
#define SD_SCR1_CMD_SUPPORT_OFFSET	0
#define SD_SCR1_CMD_SUPPORT_MASK	REG_4BIT_MASK
#define SD_SCR1_CMD_SUPPORT(value)	_GET_FV(SD_SCR1_CMD_SUPPORT, value)

#define SD_VERSION_1		0
#define SD_VERSION_1_1		1
#define SD_VERSION_2		2
#define SD_VERSION_3		3
#define SD_VERSION_4		4
#define SD_VERSION_5		5
#define SD_VERSION_6		6

#define SD_FREQ_PP		MMC_FREQ_PP
#define SD_FREQ_HS		50000000

typedef struct {
	uint8_t scr_structure : 4;
	uint8_t version : 4;
	uint8_t bus_widths : 4;
	uint8_t cmd_support : 4;
	uint8_t sd_security : 3;
	uint8_t ex_security : 4;
	uint8_t data_stat_after_erase : 1;
} sd_scr_t;

/* 4.13.2.8 Speed Class Control Command (CMD20) */
#define SD_SPEED_CLASS_OFFSET	0
#define SD_SPEED_CLASS_MASK	REG_28BIT_MASK
#define SD_SPEED_CLASS(value)	_SET_FV(SD_SPEED_CLASS, value)
#define SD_SPEED_CONTROL_OFFSET	28
#define SD_SPEED_CONTROL_MASK	REG_4BIT_MASK
#define SD_SPEED_CONTROL(value)	_SET_FV(SD_SPEED_CONTROL, value)

/* CMD_SPEED_CLASS_CONTROL */
typedef struct {
	uint32_t speed_class : 28;
	uint32_t speed_control: 4;
} __packed sd_speed_t;

/* CMD_SEND_IF_COND */
/* 4.3.13 Send Interface Condition Command (CMD8)
 * 4.9.6 R1 (Card interface condition)
 */
#define SD_R7_VHS_OFFSET		8
#define SD_R7_VHS_MASK			REG_4BIT_MASK
#define SD_R7_VHS(value)		_SET_FV(SD_R7_VHS, value)
#define sd_r7_vhs(value)		_GET_FV(SD_R7_VHS, value)
#define SD_R7_CHECK_PATTERN_OFFSET	0
#define SD_R7_CHECK_PATTERN_MASK	REG_8BIT_MASK
#define SD_R7_CHECK_PATTERN(value)	_SET_FV(SD_R7_CHECK_PATTERN, value)
#define sd_r7_check_pattern(value)	_GET_FV(SD_R7_CHECK_PATTERN, value)
/* host supply voltage */
#define SD_VOLTAGE_UNDEF		0
#define SD_VOLTAGE_HIGH			1
#define SD_VOLTAGE_LOW			2

/* ACMD_SET_BUS_WIDTH */
#define SD_BUS_WIDTH_OFFSET		0
#define SD_BUS_WIDTH_MASK		REG_2BIT_MASK
#define SD_BUS_WIDTH_1BIT		0
#define SD_BUS_WIDTH_4BIT		2
#define SD_BUS_WIDTH(value)		_SET_FV(SD_BUS_WIDTH, value)

/* ACMD_SET_WR_BLK_ERASE_COUNT */
#define SD_WR_BLK_ERASE_COUNT_OFFSET	0
#define SD_WR_BLK_ERASE_COUNT_MASK	REG_23BIT_MASK
#define sd_wr_blk_erase_count(value)	_GET_FV(SD_WR_BLK_ERASE_COUNT, value)

/* ACMD_SD_SEND_OP_COND, R3 */
#define SD_OCR_S18R			_BV(24)
#define SD_OCR_XPC			_BV(28)
#define SD_OCR_HCS_OFFSET		30
#define SD_OCR_HCS_MASK			REG_1BIT_MASK
#define SD_OCR_HCS			_SET_FV(SD_OCR_HCS, SD_HCS)

/* SD frequencies and voltages */
#define SD_25MHZ		0x00
#define SD_50MHZ		0x01
#define SD_100MHZ		0x02
#define SD_208MHZ		0x03
#define SD_52MHZ		0x04
#define SD_3_3V			0x00
#define SD_1_8V			0x01
#define SD_FD_B			0x02
#define SD_HD_B			0x03
#define SD_FD_C			0x04
#define SD_FD_D			0x05
#define SD_OP_MODE(freq, volt)	MAKEWORD(freq, volt)
#define SD_IF_MODE(freq, range)	MAKEWORD(freq, range)

#define UHSI_DS		SD_OP_MODE(SD_25MHZ, SD_3_3V)
#define UHSI_HS		SD_OP_MODE(SD_50MHZ, SD_3_3V)

#ifndef CONFIG_SD_SDSC
/* Ultra High Speed Phase I (UHS-I) Card */
/* UHS-I card operation modes */
#define UHSI_SDR12	SD_OP_MODE(SD_25MHZ, SD_1_8V)
#define UHSI_SDR25	SD_OP_MODE(SD_50MHZ, SD_1_8V)
#define UHSI_SDR50	SD_OP_MODE(SD_100MHZ, SD_1_8V)
#define UHSI_SDR104	SD_OP_MODE(SD_208MHZ, SD_1_8V)
#define UHSI_DDR50	SD_OP_MODE(SD_50MHZ, SD_1_8V)

/* UHS-I card types */
#define UHS50		UHSI_SDR50
#define UHS104		UHSI_SDR104

/* UHS-I host types */
#define UHS_HOST_SDR_FD	UHSI_SDR50	/* SDR, fixed delay */
#define UHS_HOST_SDR_VD	UHSI_SDR104	/* SDR, variable delay */
#define UHS_HOST_DDR	UHSI_DDR50	/* DDR */

/* Ultra High Speed Phase II (UHS-II) Card */
/* UHS-II interface modes */
#define UHSII_FD156	SD_IF_MODE(SD_52MHZ, SD_FD_B)
#define UHSII_HD312	SD_IF_MODE(SD_52MHZ, SD_HD_B)
#define UHSII_FD312	SD_IF_MODE(SD_52MHZ, SD_FD_C)
#define UHSII_FD624	SD_IF_MODE(SD_52MHZ, SD_FD_D)

/* UHS-II card types */
#define UHS156		UHSII_FD156
#define UHS624		UHSII_FD624
#endif /* !CONFIG_SD_SDSC */

/* Bus speed mode option/mandatory
 * +-----------+------+------------------+-----------------------+
 * |           |NonUHS|UHS-I             |UHS-II                 |
 * |           |      |                  +-----------+-----+-----+
 * |           |      |                  |B          |C    |D    |
 * +-----------+--+---+-----+------+-----+-----+-----+-----+-----+
 * |Class      |DS|HS |SDR50|SDR104|DDR50|FD156|HD312|FD312|FD624|
 * +-----------+--+---+-----+------+-----+-----+-----+-----+-----+
 * |SDSC       |M |O  |N/A  |N/A   |N/A  |N/A  |N/A  |N/A  |N/A  |
 * +----+------+--+---+-----+------+-----+-----+-----+-----+-----+
 * |SDHC|NonUHS|M |O  |N/A  |N/A   |N/A  |N/A  |N/A  |N/A  |N/A  |
 * |    +------+--+---+-----+------+-----+-----+-----+-----+-----+
 * |    |UHS50 |M |M  |M    |N/A   |(1)  |N/A  |N/A  |N/A  |N/A  |
 * |    +------+--+---+-----+------+-----+-----+-----+-----+-----+
 * |    |UHS104|M |M  |M    |M     |(1)  |N/A  |N/A  |N/A  |N/A  |
 * |    +------+--+---+-----+------+-----+-----+-----+-----+-----+
 * |    |UHS156|M |M  |M    |O     |(1)  |M    |O    |N/A  |N/A  |
 * |    +------+--+---+-----+------+-----+-----+-----+-----+-----+
 * |    |UHS624|M |M  |M    |O     |(1)  |M    |O    |M    |M    |
 * +----+------+--+---+-----+------+-----+-----+-----+-----+-----+
 * |SDXC|NonUHS|M |O  |N/A  |N/A   |N/A  |N/A  |N/A  |N/A  |N/A  |
 * |    +------+--+---+-----+------+-----+-----+-----+-----+-----+
 * |    |UHS50 |M |M  |M    |N/A   |(1)  |N/A  |N/A  |N/A  |N/A  |
 * |    +------+--+---+-----+------+-----+-----+-----+-----+-----+
 * |    |UHS104|M |M  |M    |M     |(1)  |N/A  |N/A  |N/A  |N/A  |
 * |    +------+--+---+-----+------+-----+-----+-----+-----+-----+
 * |    |UHS156|M |M  |M    |O     |(1)  |M    |O    |N/A  |N/A  |
 * |    +------+--+---+-----+------+-----+-----+-----+-----+-----+
 * |    |UHS624|M |M  |M    |O     |(1)  |M    |O    |M    |M    |
 * +----+------+--+---+-----+------+-----+-----+-----+-----+-----+
 * (1): O (SD) and M (uSD).
 */

#define sd_state_is(state)	(mmc_state_get() == SD_STATE_##state)
#define sd_state_gt(state)	(mmc_state_get() > SD_STATE_##state)
#define sd_state_ge(state)	(mmc_state_get() >= SD_STATE_##state)
#define sd_state_lt(state)	(mmc_state_get() < SD_STATE_##state)
#define sd_state_le(state)	(mmc_state_get() <= SD_STATE_##state)
#define sd_state_enter(state)	mmc_state_set(SD_STATE_##state)

#define SD_SD_STATUS_DATA_LEN		64
#define SD_SCR_DATA_LEN			8
#define SD_MAX_DATA_LEN			SD_SD_STATUS_DATA_LEN

#define MMC_PHY_SLOT				\
	sd_cid_t cid;				\
	sd_scr_t host_scr;			\
	sd_scr_t card_scr;			\
	sd_ssr_t sd_status;			\
	bool scr_valid;				\
	bool ssr_valid;				\
	bool voltage_ready;			\
	uint8_t card_version;			\
	uint32_t wr_blk_erase_count : 23;	\
	sd_speed_t speed;

#ifdef CONFIG_SD_SPI
#define MMC_SPI_SLOT				\
	uint8_t r1;				\
	bool op_cond_sent;
#else
#define MMC_SPI_SLOT
#define sd_spi_send_cmd()	do { } while (0)
#define sd_spi_recv_rsp()	do { } while (0)
#define sd_spi_enter_ver()	do { } while (0)
#define sd_spi_handle_ver()	do { } while (0)
#endif

#if defined(CONFIG_CONSOLE) && defined(CONFIG_MMC_DEBUG)
extern const char *mmc_phy_state_names[16];
extern const char *mmc_phy_cmd_names[64];
extern const char *mmc_phy_acmd_names[64];
#endif

extern bool sd_spi_mode;

uint32_t sd_encode_if_cond(void);
uint32_t sd_decode_ocr(mmc_r3_t raw_ocr);
uint8_t sd_get_vhs(void);

void mmc_phy_reset_slot(void);
void mmc_phy_handle_seq(void);
void mmc_phy_handle_stm(void);
void mmc_phy_send_cmd(void);
void mmc_phy_recv_rsp(void);
void mmc_phy_start_dat(void);
void mmc_phy_stop_dat(void);

#endif /* __SD_PHY_H_INCLUDE__ */
