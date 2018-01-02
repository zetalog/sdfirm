/*
 * ZETALOG's Personal COPYRIGHT
 *
 * Copyright (c) 2018
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
 * @(#)vic_nvic.c: nested vectored interrupt controller implementation
 * $Id: vic_nvic.c,v 1.279 2011-10-19 10:19:18 zhenglv Exp $
 */

#include <target/irq.h>

/*
 * When configuring the OFFSET field, the offset must be aligned to the
 * number of exception entries in the vector table.
 * Because there are 54 interrupts, 16 exceptions, the minimum alignment
 * is 128 words.
 */
#define __VIC_HW_ALIGN	__attribute__((aligned(128*4)))
irq_handler __VIC_HW_ALIGN __vic_hw_vector_table[NR_IRQS+NR_TRAPS];

extern irq_handler __lovec[NR_TRAPS];
extern void __bad_interrupt(void);

void vic_hw_register_trap(uint8_t nr, irq_handler h)
{
	BUG_ON(nr >= NR_TRAPS);
	__vic_hw_vector_table[nr] = h;
}

void vic_hw_register_irq(uint8_t nr, irq_handler h)
{
	BUG_ON(nr >= NR_IRQS);
	__vic_hw_vector_table[NR_TRAPS+nr] = h;
}

void vic_hw_irq_priority(uint8_t irq, uint8_t prio)
{
	BUG_ON(irq >= NR_IRQS);
	BUG_ON(prio > __VIC_HW_PRIO_MAX);
	nvic_irq_set_priority(irq, prio);
}

void vic_hw_trap_priority(uint8_t trap, uint8_t prio)
{
	BUG_ON(trap < __VIC_HW_PRIO_TRAP_MIN || trap >= NR_TRAPS);
	BUG_ON(prio > __VIC_HW_PRIO_MAX);
	nvic_trap_set_priority(trap, prio);
}

void vic_hw_vectors_init(void)
{
	uint8_t i;

	for (i = 0; i < NR_TRAPS; i++)
		__vic_hw_vector_table[i] = __lovec[i];
	for (i = 0; i < NR_IRQS; i++)
		__vic_hw_vector_table[NR_TRAPS+i] = __bad_interrupt;
	__raw_writel((unsigned long)__vic_hw_vector_table, VTABLE);

	/* disable IT folding */
	__raw_setl_atomic(DISFOLD, ACTLR);

	/* disable all IRQs */
	__raw_writel(0xFFFFFFFF, DIS0);
	__raw_writel(0xFFFFFFFF, DIS1);
}
