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
 * @(#)spacemit_stm.h: SpacemiT STM interface
 * $Id: spacemit_stm.h,v 1.1 2023-11-27 16:50:00 zhenglv Exp $
 */

#ifndef __SPACEMIT_STM_H_INCLUDE__
#define __SPACEMIT_STM_H_INCLUDE__

#define SPACEMIT_STM_BASE			__STM_BASE

#ifndef SPACEMIT_STM_REG
#define SPACEMIT_STM_REG(offset)		(SPACEMIT_STM_BASE + (offset))
#endif

/* Register offset */
#define STM_TSP_L				SPACEMIT_STM_REG(0x00)
#define STM_TSP_H				SPACEMIT_STM_REG(0x04)
#define STM_TSP_SS_L				SPACEMIT_STM_REG(0x08)
#define STM_TSP_SS_H				SPACEMIT_STM_REG(0x0C)
#define STM_SS_CFG				SPACEMIT_STM_REG(0x10)
#define STM_SS_CTRL				SPACEMIT_STM_REG(0x14)
#define STM_SS_PRD				SPACEMIT_STM_REG(0x18)
#define STM_TSP_TRIM_L				SPACEMIT_STM_REG(0x1C)
#define STM_TSP_TRIM_H				SPACEMIT_STM_REG(0x20)
#define STM_INT_EN				SPACEMIT_STM_REG(0x24)
#define STM_INT_FLAG				SPACEMIT_STM_REG(0x28)
#define STM_DEBOUNCE				SPACEMIT_STM_REG(0x2C)

/* STM_SS_CFG */
#define STM_SS_CFG_BIT0_EN			_BV(0)
#define STM_SS_CFG_BIT1_EN			_BV(1)
#define STM_SS_CFG_BIT2_EN			_BV(2)

/* STM_SS_CTRL */
#define STM_SS_CTRL_BIT0_EN			_BV(0)
#define STM_SS_CTRL_BIT1_EN			_BV(1)

/* STM_TSP_TRIM_H */
#define STM_TSP_TRIM_H_BIT31_EN			_BV(31)
#define STM_TSP_TRIM_H_BIT0_15_MASK		GENMASK(30, 0)
#define STM_TSP_TRIM_H_BIT0_15(n)		FIELD_PREP(STM_TSP_TRIM_H_BIT0_15_MASK, n)

/* STM_INT_EN */
#define STM_INT_EN_BIT0_EN			_BV(0)

/* STM_INT_FLAG */
#define STM_INT_FLAG_BIT0_EN			_BV(0)

/* STM_DEBOUNCE */
#define STM_DEBOUNCE_BIT0_15_MASK		GENMASK(15, 0)
#define STM_DEBOUNCE_BIT0_15(n)			FIELD_PREP(STM_DEBOUNCE_BIT0_15_MASK, n)

void stm_sync_2dies(void);
void stm_hw_ctrl_init(void);

#endif /* __SPACEMIT_STM_H_INCLUDE__ */
