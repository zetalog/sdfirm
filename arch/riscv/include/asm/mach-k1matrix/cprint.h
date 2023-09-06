/*
 * ZETALOG's Personal COPYRIGHT
 *
 * Copyright (c) 2023
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
 * @(#)cprint.h: K1Matrix system registers definitions
 * $Id: cprint.h,v 1.1 2023-09-06 18:48:00 zhenglv Exp $
 */

#ifndef __CPRINT_K1MATRIX_H_INCLUDE__
#define __CPRINT_K1MATRIX_H_INCLUDE__

#include <asm/mach/reg.h>

#define SIM_CTRL_BASE		(PLIC_REG_BASE + 0x3FFFFF0)

#define SIM_DEBUG		SIM_CTRL_BASE

#define SIM_REG(type, info)	(((type) << 24) | (info))
#define SIM_FINISH(code)	SIM_REG(0x10, code)
#define SIM_PRINT(byte)		SIM_REG(0x14, byte)
#define SIM_WAIT_NS		SIM_REG(0x18, 0x00)
#define SIM_C2SV(info)		SIM_REG(0x1C, info)

#ifdef CONFIG_K1MATRIX_CPRINT
#define k1matrix_finish(code)	\
	__raw_writel(SIM_FINISH(code), SIM_DEBUG)
#else /* CONFIG_K1MATRIX_CPRINT */
#define k1matrix_finish(code)	do { } while (0)
#endif /* CONFIG_K1MATRIX_CPRINT */

#endif /* __CPRINT_K1MATRIX_H_INCLUDE__ */
