/*
 * ZETALOG's Personal COPYRIGHT
 *
 * Copyright (c) 2020
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
 * @(#)flash.h: DPU ramdon accessible SPI flash definitions
 * $Id: flash.h,v 1.1 2020-03-26 16:49:00 zhenglv Exp $
 */

#ifndef __FLASH_DPU_H_INCLUDE__
#define __FLASH_DPU_H_INCLUDE__

#include <target/arch.h>

#define FLASH_MEM_OFFSET	0
#define FLASH_REG_OFFSET	0x01E00000
#define FLASH_CFG_OFFSET	0x01E08000
#define FLASH_REG(offset)	(FLASH_BASE + FLASH_REG_OFFSET + (offset))
#define FLASH_CFG(offset)	(FLASH_BASE + FLASH_CFG_OFFSET + (offset))

#define DPU_FLASH_FREQ		80000000 /* 80MHz */
#define DPU_FLASH_REAL_FREQ	4000000 /* 4MHz */

#define SPI_CTRL		FLASH_CFG(0x00)
#define SPI_DIVIDER		FLASH_CFG(0x04)
#define SPI_SS			FLASH_CFG(0x08)
#define SPI_COUTER		FLASH_CFG(0x0C)
#define SPI_TIMEOUT		FLASH_CFG(0x10)
#define SPI_RX_0		FLASH_CFG(0x40)
#define SPI_RX_1		FLASH_CFG(0x44)
#define SPI_RX_2		FLASH_CFG(0x48)
#define SPI_RX_3		FLASH_CFG(0x4C)
#define SPI_TX_0		FLASH_CFG(0x40)
#define SPI_TX_1		FLASH_CFG(0x44)
#define SPI_TX_2		FLASH_CFG(0x48)
#define SPI_TX_3		FLASH_CFG(0x4C)

/* SPI_CTRL */
#define SPI_CTRL_DMA			_BV(31)
#define SPI_CTRL_DUMMY_SEL		_BV(26)
#define SPI_CTRL_DUMMY_OFFSET		16
#define SPI_CTRL_DUMMY_MASK		REG_7BIT_MASK
#define SPI_CTRL_DUMMY(value)		_SET_FV(SPI_CTRL_DUMMY, value)
#define SPI_CTRL_WORD_ALIGN		_BV(15)
#define SPI_CTRL_CPOL			_BV(14)
#define SPI_CTRL_CPHA			_BV(13)
#define SPI_CTRL_IE			_BV(12)
#define SPI_CTRL_RNW			_BV(11)
#define SPI_CTRL_LSB			_BV(10)
#define SPI_CTRL_BI			_BV(9)
#define SPI_CTRL_BO			_BV(8)
#define SPI_CTRL_CHAR_LEN_SEL		_BV(7)
#define SPI_CTRL_CHAR_LEN_OFFSET	0
#define SPI_CTRL_CHAR_LEN_MASK		REG_7BIT_MASK
#define SPI_CTRL_CHAR_LEN(value)	_SET_FV(SPI_CTRL_CHAR_LEN, value)

#ifdef CONFIG_DPU_FLASH_DIVIDER_8
#define SPI_DIVIDER_MASK		REG_8BIT_MASK
#endif
#ifdef CONFIG_DPU_FLASH_DIVIDER_16
#define SPI_DIVIDER_MASK		REG_16BIT_MASK
#endif
#ifdef CONFIG_DPU_FLASH_DIVIDER_24
#define SPI_DIVIDER_MASK		REG_24BIT_MASK
#endif
#ifdef CONFIG_DPU_FLASH_DIVIDER_32
#define SPI_DIVIDER_MASK		REG_32BIT_MASK
#endif
#define SPI_DIVIDER_OFFSET		0
#define SPI_SET_DIVIDER(value)		_SET_FV(SPI_DIVIDER, value)

#ifdef CONFIG_DPU_FLASH_SS_8
#define SPI_SS_MASK			REG_8BIT_MASK
#endif
#ifdef CONFIG_DPU_FLASH_SS_16
#define SPI_SS_MASK			REG_16BIT_MASK
#endif
#ifdef CONFIG_DPU_FLASH_SS_24
#define SPI_SS_MASK			REG_24BIT_MASK
#endif
#ifdef CONFIG_DPU_FLASH_SS_32
#define SPI_SS_MASK			REG_32BIT_MASK
#endif
#define SPI_SS_OFFSET			0
#define SPI_SET_SS(value)		_SET_FV(SPI_SS, value)

#define SPI_TIMEOUT_OFFSET		0
#define SPI_TIMEOUT_MASK		REG_24BIT_MASK
#define SPI_SET_TIMEOUT(value)		_SET_FV(SPI_TIMEOUT, value)
#define SPI_COUNTER_OFFSET		0
#define SPI_COUNTER_MASK		REG_16BIT_MASK
#define SPI_GET_COUNTER(value)		_GET_FV(SPI_COUNTER, value)

#ifndef __ASSEMBLY__
void dpu_flash_set_frequency(uint32_t freq);
#endif

#endif /* __FLASH_DPU_H_INCLUDE__ */
