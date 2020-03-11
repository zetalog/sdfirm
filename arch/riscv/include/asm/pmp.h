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
 * @(#)pmp.h: RISCV physical memory protection (PMP) interface
 * $Id: pmp.h,v 1.1 2020-01-02 22:48:00 zhenglv Exp $
 */

#ifndef __PMP_RISCV_H_INCLUDE__
#define __PMP_RISCV_H_INCLUDE__

#define PMP_R				0x01
#define PMP_W				0x02
#define PMP_X				0x04
#define PMP_A				0x18
#define PMP_A_TOR			0x08
#define PMP_A_NA4			0x10
#define PMP_A_NAPOT			0x18
#define PMP_L				0x80

#define PMP_SHIFT			2
#define PMP_COUNT			16

#if !defined(__ASSEMBLY__) && !defined(LINKER_SCRIPT)
#ifdef CONFIG_RISCV_PMP
int pmp_set(int n, unsigned long prot,
	    phys_addr_t addr, unsigned long log2len);
int pmp_get(int n, unsigned long *prot, phys_addr_t *addr,
	    unsigned long *log2len);
int pmp_dump(int argc, char **argv);
#else
#define pmp_set(n, prot, addr, log2len)		(-ENODEV)
#define pmp_get(n, prot, addr, log2len)		(-ENODEV)
#define pmp_dump(argc, argv)			do { } while (0)
#endif
#endif

#endif /* __PMP_RISCV_H_INCLUDE__ */
