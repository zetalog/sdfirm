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
 * @(#)tmr.h: DPU specific timer controller (TMR) driver
 * $Id: tmr.h,v 1.1 2020-13-48 13:48:00 zhenglv Exp $
 */

#ifndef __TMR_DPU_H_INCLUDE__
#define __TMR_DPU_H_INCLUDE__

#include <driver/dw_timers.h>

#define TMR_BASE		TIMER_BASE
#define TMR_REG(offset)		(TMR_BASE + (offset))

#define TMR_CNT_CTRL		TMR_REG(0x00)
#define TMR_CMP_CTRL(n)		REG_1BIT_ADDR(TMR_REG(0x04), n)
#define TMR_INTR_EN(n)		REG_1BIT_ADDR(TMR_REG(0x10), n)
#define TMR_INTR_STATUS(n)	REG_1BIT_ADDR(TMR_REG(0x14), n)
#define TMR_CNT_LO		TMR_REG(0x40)
#define TMR_CNT_HI		TMR_REG(0x44)
#define TMR_CMP_LO(n)		TMR_REG(0x200 + ((n) << 4))
#define TMR_CMP_HI(n)		TMR_REG(0x204 + ((n) << 4))
#define TMR_VAL(n)		TMR_REG(0x208 + ((n) << 4))

/* TMR_CNT_CTRL */
#define TMR_EN			_BV(0)
#define TMR_HALT_ON_DEBUG	_BV(1)

#define tmr_enable_irq(id)	__raw_setl(_BV(id), TMR_INTR_EN(id))
#define tmr_disable_irq(id)	__raw_clearl(_BV(id), TMR_INTR_EN(id))
#define tmr_irq_status(id)	(__raw_readl(TMR_INTR_STATUS(id)) & _BV(id))
#define tmr_irq_clear(id)	__raw_clearl(_BV(id), TMR_INTR_STATUS(id))

#ifndef __ASSEMBLY__
uint64_t tmr_read_counter(void);
void tmr_write_compare(uint8_t id, uint64_t count);
void tmr_ctrl_init(void);
#endif

#endif /* __TMR_DPU_H_INCLUDE__ */
