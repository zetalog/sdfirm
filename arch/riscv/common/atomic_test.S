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
* @(#)atomic_test.S: RISCV atomic and LR/SC instruction test
* $Id: atomic_test.S,v 1.279 2024-05-30 14:01:30 zhenglv Exp $
*/

#include <target/linkage.h>

ENTRY(__atomic_test_addr)
    mv a3, a0
    li a0, 0xffffffff80000000
    li a1, 0xfffffffffffff800
    sw a0, 0(a3)

    amoswap.w a4, a1, 0(a3)
    lw a5, 0(a3)

    li a1, 0x0000000080000000
    amoswap.w a4, a1, 0(a3)

    lw a5, 0(a3)

    ret
ENDPROC(__atomic_test_addr)

