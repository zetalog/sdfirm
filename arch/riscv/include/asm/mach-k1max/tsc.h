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
 * @(#)tsc.h: K1MAX specific mandatory TSC driver
 * $Id: tsc.h,v 1.1 2022-10-15 13:41:00 zhenglv Exp $
 */

#ifndef __TSC_K1MAX_H_INCLUDE__
#define __TSC_K1MAX_H_INCLUDE__

#include <target/arch.h>
#include <target/clk.h>

#define TSC_FREQ		(CPU_FREQ/1000)
#define TSC_MAX			ULL(0xFFFFFFFFFFFFFFFF)

#define CLINT_MTIMECMP_BASE	0x80
#define CLINT_MTIME_BASE	0x3F8

#include <asm/mach/timer.h>
#include <asm/clint.h>

#ifndef __ASSEMBLY__
#define tsc_hw_ctrl_init()	do { } while (0)
#if defined(CONFIG_RISCV_COUNTERS) || defined(CONFIG_SBI)
#define tsc_hw_read_counter()	rdtime()
#else
#define tsc_hw_read_counter()	clint_read_mtime()
#endif
#endif /* __ASSEMBLY__ */

#endif /* __TSC_K1MAX_H_INCLUDE__ */
