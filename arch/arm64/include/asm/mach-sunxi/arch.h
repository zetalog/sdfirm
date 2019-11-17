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
 * @(#)arch.h: Allwinnder SUNXI architecture definitions
 * $Id: arch.h,v 1.1 2019-11-17 11:47:00 zhenglv Exp $
 */

#ifndef __ARCH_SUNXI_H_INCLUDE__
#define __ARCH_SUNXI_H_INCLUDE__

#if defined(__ASSEMBLY__) && !defined(LINKER_SCRIPT)
#ifdef CONFIG_SUNXI_BOOT0_RMR
.macro boot0_hook
	tst	x0, x0
	b	8007f
	.space	0x7c
	.word   0xe59f1024      // ldr     r1, [pc, #36] ; 0x170000a0
	.word   0xe59f0024      // ldr     r0, [pc, #36] ; CONFIG_BOOT_BASE
	.word   0xe5810000      // str     r0, [r1]
	.word   0xf57ff04f      // dsb     sy
	.word   0xf57ff06f      // isb     sy
	.word   0xee1c0f50      // mrc     15, 0, r0, cr12, cr0, {2} ; RMR
	.word   0xe3800003      // orr     r0, r0, #3
	.word   0xee0c0f50      // mcr     15, 0, r0, cr12, cr0, {2} ; RMR
	.word   0xf57ff06f      // isb     sy
	.word   0xe320f003      // wfi
	.word   0xeafffffd      // b       @wfi
	.word   0x017000a0      // writeable RVBAR mapping address
        .word   CONFIG_BOOT_BASE
	8007:
.endm
#endif /* CONFIG_SUNXI_BOOT0_RMR */
#endif /* __ASSEMBLY__ && !LINKER_SCRIPT */

#endif /* __SUNXI_ARCH_H_INCLUDE__ */
