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
 * @(#)cache.S: ARM64 cache flush implementation
 * $Id: cache.S,v 1.279 2019-04-14 10:19:18 zhenglv Exp $
 */

#include <target/linkage.h>

       .macro  dcache_line_size, reg, tmp
       mrs     \tmp, ctr_el0
       ubfm    \tmp, \tmp, #16, #19
       mov     \reg, #4
       lsl     \reg, \reg, \tmp
       .endm

       .macro  dcache_by_line_op op, domain, kaddr, size, tmp1, tmp2
       dcache_line_size \tmp1, \tmp2
       add     \size, \kaddr, \size
       sub     \tmp2, \tmp1, #1
       bic     \kaddr, \kaddr, \tmp2
loop_\op:
       dc      \op, \kaddr
       add     \kaddr, \kaddr, \tmp1
       cmp     \kaddr, \size
       b.lo    loop_\op
       dsb     \domain
       .endm

/* __flush_dcache_area(addr, size)
 * Ensure that any D-cache lines for the interval [addr, addr+size)
 * are cleaned and invalidated to the PoC.
 * - addr    - address
 * - size    - size in question
 */
ENTRY(__flush_dcache_area)
	dcache_by_line_op civac, sy, x0, x1, x2, x3
	ret
ENDPIPROC(__flush_dcache_area)

/* __clean_dcache_area(addr, size)
 * Ensure that any D-cache lines for the interval [addr, addr+size)
 * are cleaned to the PoC.
 * - addr    - address
 * - size    - size in question
 */
ENTRY(__clean_dcache_area)
	dcache_by_line_op cvac, sy, x0, x1, x2, x3
	ret
ENDPIPROC(__clean_dcache_area)

/* __inval_dcache_area(addr, size)
 * Ensure that any D-cache lines for the interval [addr, addr+size)
 * are invalidated to the PoC.
 * - addr    - address
 * - size    - size in question
 */
ENTRY(__inval_dcache_area)
	dcache_by_line_op ivac, sy, x0, x1, x2, x3
	ret
ENDPIPROC(__inval_dcache_area)
