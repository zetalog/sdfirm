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
 * @(#)plic.c: DPU specific PLIC implementation
 * $Id: plic.c,v 1.1 2020-07-03 10:01:00 zhenglv Exp $
 */

#include <target/irq.h>
#include <target/paging.h>

#ifdef CONFIG_MMU
#define PLIC_CTX_REG_BASE(n)		\
	(PLIC_REG_BASE + PLIC_CONTEXT_BASE + (PLIC_BLOCK_SIZE * (n)))

caddr_t dpu_plic_reg_base = PLIC_REG_BASE;
caddr_t dpu_plic_ctx_base[2] = {
	PLIC_CTX_REG_BASE(0),
	PLIC_CTX_REG_BASE(1),
};

void plic_hw_mmu_init(void)
{
	if (dpu_plic_reg_base == PLIC_REG_BASE) {
		set_fixmap_io(FIX_PLIC_EN_0_31, PLIC_ENABLER(0, 0) & PAGE_MASK);
		set_fixmap_io(FIX_PLIC_PEND, PLIC_PENDINGR(0, 0) & PAGE_MASK);
		set_fixmap_io(FIX_PLIC_PRIO, PLIC_PRIORITYR(0, 0) & PAGE_MASK);
		dpu_plic_reg_base = fix_to_virt(FIX_PLIC_PRIO);
		printf("FIXMAP: %016llx -> %016llx %016llx %016llx: PLIC\n",
		       PLIC_REG_BASE, fix_to_virt(FIX_PLIC_PRIO),
		       fix_to_virt(FIX_PLIC_PEND),
		       fix_to_virt(FIX_PLIC_EN_0_31));
	}
	if (dpu_plic_ctx_base[1] == PLIC_CTX_REG_BASE(1)) {
		set_fixmap_io(FIX_PLIC_CTX_1, PLIC_CTX_REG_BASE(1) & PAGE_MASK);
		dpu_plic_ctx_base[1] = fix_to_virt(FIX_PLIC_CTX_1);
		printf("FIXMAP: %016llx -> %016llx: PLIC CTX 1\n",
		       PLIC_CTX_REG_BASE(1), fix_to_virt(FIX_PLIC_CTX_1));
	}
	if (dpu_plic_ctx_base[0] == PLIC_CTX_REG_BASE(0)) {
		set_fixmap_io(FIX_PLIC_CTX_0, PLIC_CTX_REG_BASE(0) & PAGE_MASK);
		dpu_plic_ctx_base[0] = fix_to_virt(FIX_PLIC_CTX_0);
		printf("FIXMAP: %016llx -> %016llx: PLIC CTX 0\n",
		       PLIC_CTX_REG_BASE(0), fix_to_virt(FIX_PLIC_CTX_0));
	}
}
#endif
