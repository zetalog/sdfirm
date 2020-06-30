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
 * @(#)tlb.h: RISCV TLB flush interfaces
 * $Id: tlb.h,v 1.1 2019-08-14 14:20:00 zhenglv Exp $
 */

#ifndef __TLB_RISCV_H_INCLUDE__
#define __TLB_RISCV_H_INCLUDE__

#include <asm/sbi.h>

#ifndef __ASSEMBLY__
#define sfence_vma_all()			\
	asm volatile ("sfence.vma" : : : "memory")
#define sfence_vma_page(addr)			\
	asm volatile ("sfence.vma %0" : : "r" (addr) : "memory")
#define sfence_vma_asid_all(asid)		\
	asm volatile("sfence.vma x0, %0" : : "r"(asid) : "memory")
#define sfence_vma_asid_page(asid, addr)	\
	asm volatile("sfence.vma %0, %1" : : "r"(addr), "r"(asid) : "memory")
#define fence_i()				\
	asm volatile("fence.i" : : : "memory")

void local_flush_tlb_all(void);
void local_flush_tlb_page(caddr_t addr);
void local_flush_tlb_asid_all(int asid);
void local_flush_tlb_asid_page(int asid, caddr_t addr);
void local_flush_icache_all(void);

void flush_tlb_all(void);
void flush_tlb_page(int asid, caddr_t addr);
void flush_tlb_range_user(int asid, caddr_t start, caddr_t end);
void flush_tlb_range_kern(caddr_t start, caddr_t end);
void flush_icache_all(void);
#endif /* !__ASSEMBLY__ */

#endif /* __TLB_RISCV_H_INCLUDE__ */
