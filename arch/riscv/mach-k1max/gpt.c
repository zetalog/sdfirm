/*
 * ZETALOG's Personal COPYRIGHT
 *
 * Copyright (c) 2022
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
 * @(#)gpt.c: K1MAX specific generic timer implementation
 * $Id: gpt.c,v 1.1 2022-10-15 14:39:00 zhenglv Exp $
 */

#include <target/timer.h>
#include <target/jiffies.h>
#include <target/smp.h>
#include <target/sbi.h>
#include <target/irq.h>

void riscv_timer(irq_t irq)
{
	irqc_clear_irq(IRQ_TIMER);
	irqc_disable_irq(IRQ_TIMER);
	tick_handler();
}

#ifdef SYS_BOOTLOAD
void gpt_hw_irq_poll(void)
{
	if (riscv_irq_raised(IRQ_TIMER))
		riscv_timer(IRQ_TIMER);
}
#endif

#ifdef CONFIG_SBI
void gpt_hw_oneshot_timeout(timeout_t tout_ms)
{
	uint64_t next = tick_get_counter() + tout_ms;

	irqc_enable_irq(IRQ_TIMER);
	sbi_set_timer(TSC_FREQ * next);
}
#else
void gpt_hw_oneshot_timeout(timeout_t tout_ms)
{
	uint64_t next = tick_get_counter() + tout_ms;

	irqc_enable_irq(IRQ_TIMER);
	clint_set_mtimecmp(smp_processor_id(), TSC_FREQ * next);
}
#endif

void gpt_hw_ctrl_init(void)
{
	irq_register_vector(IRQ_TIMER, riscv_timer);
	if (!cnt_status_gcounter())
		cnt_enable_gcounter();
}
