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
 * @(#)timer.c: SUNXI systick timer implementation
 * $Id: timer.c,v 1.1 2019-11-17 13:25:00 zhenglv Exp $
 */

#include <target/gpt.h>
#include <target/tsc.h>
#include <target/jiffies.h>
#include <target/irq.h>
#include <target/panic.h>

bool gblct_initialized = false;

void gblct_init(void)
{
	if (!gblct_initialized) {
		gblct_initialized = true;
		__systick_init();
	}
}

tsc_count_t tsc_hw_read_counter(void)
{
	return (tsc_count_t)__systick_read();
}

void tsc_hw_ctrl_init(void)
{
	gblct_init();
}

void gpt_hw_oneshot_timeout(timeout_t tout_ms)
{
	BUG_ON(tout_ms > GPT_MAX_TIMEOUT);

	__systick_set_timeout(tout_ms);
	__systick_unmask_irq();
}

static void gpt_hw_handle_irq(void)
{
	if (__systick_poll()) {
		__systick_mask_irq();
		tick_handler();
	}
}

#ifdef SYS_BOOTLOAD
void gpt_hw_irq_poll(void)
{
	gpt_hw_handle_irq();
}

#define gpt_hw_irq_enable()
#define gpt_hw_irq_init()
#else
#define gpt_hw_irq_enable()	__systick_unmask_irq()

void gpt_hw_irq_init(void)
{
	irqc_configure_irq(IRQ_TIMER, 0, IRQ_LEVEL_TRIGGERED);
	irq_register_vector(IRQ_TIMER, gpt_hw_handle_irq);
	irqc_enable_irq(IRQ_TIMER);
}
#endif

void gpt_hw_ctrl_init(void)
{
	gblct_init();
}
