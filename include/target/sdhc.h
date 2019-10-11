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
 * @(#)sdhc.h: secure digital host controller definitions
 * $Id: sdhc.h,v 1.1 2019-10-08 09:11:00 zhenglv Exp $
 */

#ifndef __SDHC_H_INCLUDE__
#define __SDHC_H_INCLUDE__

#include <target/config.h>

#ifndef SDHC_REG
#define SDHC_REG(n, offset)		(SDHC##n##_BASE + (offset))
#endif

/* 2.1.1 SD Host Cotrol Register */
/*===========================================================================
 * SD registers
 *===========================================================================*/
/* 64-bit registers */
#define SDHC_CAPABILITIES(n)		SDHC_REG(n, 0x040)
#define SDHC_ADMA_SYSTEM_ADDRESS(n)	SDHC_REG(n, 0x058)
#define SDHC_ADMA3_ID_ADDRESS(n)	SDHC_REG(n, 0x078)

/* 32-bit registers */
#define SDHC_32BIT_BLOCK_COUNT(n)	SDHC_REG(n, 0x000)
#define SDHC_SDMA_SYSTEM_ADDRESS(n)	SDHC_REG(n, 0x000)
#define SDHC_ARGUMENT(n)		SDHC_REG(n, 0x008)
#define SDHC_PRESENT_STATE(n)		SDHC_REG(n, 0x024)
#define SDHC_MAXIMUM_CURRENT_CAPABILITIES(n)		\
	SDHC_REG(n, 0x048)

/* 16-bit registers */
#define SDHC_BLOCK_SIZE(n)		SDHC_REG(n, 0x004)
#define SDHC_16BIT_BLOCK_COUNT(n)	SDHC_REG(n, 0x006)
#define SDHC_TRANSFER_MODE(n)		SDHC_REG(n, 0x00C)
#define SDHC_COMMAND(n)			SDHC_REG(n, 0x00E)
#define SDHC_RESPONSE0(n)		SDHC_REG(n, 0x010)
#define SDHC_RESPONSE1(n)		SDHC_REG(n, 0x012)
#define SDHC_RESPONSE2(n)		SDHC_REG(n, 0x014)
#define SDHC_RESPONSE3(n)		SDHC_REG(n, 0x016)
#define SDHC_RESPONSE4(n)		SDHC_REG(n, 0x018)
#define SDHC_RESPONSE5(n)		SDHC_REG(n, 0x01A)
#define SDHC_RESPONSE6(n)		SDHC_REG(n, 0x01C)
#define SDHC_RESPONSE7(n)		SDHC_REG(n, 0x01E)
#define SDHC_BUFFER_DATA_PORT0(n)	SDHC_REG(n, 0x020)
#define SDHC_BUFFER_DATA_PORT1(n)	SDHC_REG(n, 0x022)
#define SDHC_CLOCK_CONTROL(n)		SDHC_REG(n, 0x02C)
#define SDHC_NORMAL_INTERRUPT_STATUS(n)	SDHC_REG(n, 0x030)
#define SDHC_ERROR_INTERRUPT_STATUS(n)	SDHC_REG(n, 0x032)
#define SDHC_NORMAL_INTERRUPT_ENABLE(n)	SDHC_REG(n, 0x034)
#define SDHC_ERROR_INTERRUPT_ENABLE(n)	SDHC_REG(n, 0x036)
#define SDHC_NORMAL_INTERRUPT_SIGNAL(n)	SDHC_REG(n, 0x038)
#define SDHC_ERROR_INTERRUPT_SIGNAL(n)	SDHC_REG(n, 0x03A)
#define SDHC_AUTO_CMD_ERROR_STATUS(n)	SDHC_REG(n, 0x03C)
#define SDHC_HOST_CONTROL2(n)		SDHC_REG(n, 0x03E)
#define SDHC_FORCE_EVENT_FOR_AUTO_CMD_ERROR_STATUS(n)	\
	SDHC_REG(n, 0x050)
#define SDHC_FORCE_EVENT_FOR_ERROR_INTERRUPT_STATUS(n)	\
	SDHC_REG(n, 0x052)

#define SDHC_SLOT_INTERRUPT_STATUS(n)	SDHC_REG(n, 0x0FC)
#define SDHD_HOST_CONTROLLER_VERSION(n)	SDHC_REG(n, 0x0FE)

/* 8-bit registers */
#define SDHC_HOST_CONTROL1(n)		SDHC_REG(n, 0x028)
#define SDHC_POWER_CONTROL(n)		SDHC_REG(n, 0x029)
#define SDHC_BLOCK_GAP_CONTROL(n)	SDHC_REG(n, 0x02A)
#define SDHC_WAKEUP_CONTROL(n)		SDHC_REG(n, 0x02B)
#define SDHC_TIMEOUT_CONTROL(n)		SDHC_REG(n, 0x02E)
#define SDHC_SOFTWARE_RESET(n)		SDHC_REG(n, 0x02F)
#define SDHC_ADMA_ERROR_STATUS(n)	SDHC_REG(n, 0x054)

/*===========================================================================
 * pointer registers
 *===========================================================================*/
/* 16-bit registers */
#define SDHC_PTR_UHSII_SETTINGS(n)	SDHC_REG(n, 0x0E0)
#define SDHC_PTR_UHSII_HOST_CAPABILITIES		\
	SDHC_REG(n, 0x0E2)
#define SDHC_PTR_UHSII_TEST(n)		SDHC_REG(n, 0x0E4)
#define SDHC_PTR_EMBEDDED_CONTROL(n)	SDHC_REG(n, 0x0E6)
#define SDHC_PTR_VENDOR_SPECIFIC(n)	SDHC_REG(n, 0x0E8)
#define SDHC_PTR_SPECIFIC_CONTROL(n)	SDHC_REG(n, 0x0EA)

/*===========================================================================
 * UHS-II registers
 *===========================================================================*/
/* 32-bit registers */
#define UHSII_BLOCK_COUNT(n)		SDHC_REG(n, 0x084)
#define UHSII_MSG(n)			SDHC_REG(n, 0x0B8)
#define UHSII_ERROR_INTERRUPT_STATUS(n)	SDHC_REG(n, 0x0C4)
#define UHSII_ERROR_INTERRUPT_ENABLE(n)	SDHC_REG(n, 0x0C8)
#define UHSII_ERROR_INTERRUPT_SIGNAL(n)	SDHC_REG(n, 0x0CC)

/* 16-bit registers */
#define UHSII_BLOCK_SIZE(n)		SDHC_REG(n, 0x080)
#define UHSII_TRANSFER_MODE(n)		SDHC_REG(n, 0x09C)
#define UHSII_COMMAND(n)		SDHC_REG(n, 0x09E)
#define UHSII_DEVICE_INTERRUPT_STATUS(n)			\
	SDHC_REG(n, 0x0BC)
#define UHSII_SOFTWARE_RESET(n)		SDHC_REG(n, 0x0C0)
#define UHSII_TIMER_CONTROL(n)		SDHC_REG(n, 0x0C2)

/* 8-bit registers */
#define UHSII_COMMAND_PACKET(n, byte)	SDHC_REG(n, 0x088 + (byte))
#define UHSII_RESPONSE(n, byte)		SDHC_REG(n, 0x0A0 + (byte))
#define UHSII_MSG_SELECT(n)		SDHC_REG(n, 0x0B4)
#define UHSII_DEVICE_SELECT(n)		SDHC_REG(n, 0x0BE)
#define UHSII_DEVICE_INTERRUPT_CODE(n)	SDHC_REG(n, 0x0BF)

#ifdef CONFIG_MMC
#define mmc_hw_ctrl_init()		sdhc_init()
#define mmc_hw_slot_select(sid)		sdhc_select(sid)
#endif

#include <driver/sdhc.h>

void sdhc_init(void);
void sdhc_select(mmc_sid_t sid);

#endif /* __SDHC_H_INCLUDE__ */
