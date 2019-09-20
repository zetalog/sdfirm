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
 * @(#)reg.h: SPIKE space and register definitions
 * $Id: reg.h,v 1.1 2019-09-05 14:53:00 zhenglv Exp $
 */

#ifndef __REG_SPIKE_H_INCLUDE__
#define __REG_SPIKE_H_INCLUDE__

#include <target/config.h>
#include <target/sizes.h>

/* MEM1 default:
 *
 * memory@80000000 {
 *   device_type = "memory";
 *   reg = <0x0 0x80000000 0x0 0x80000000>;
 * };
 * Can be specified via spike -m<n> or -m<a:m,b:n,...>.
 */
#define MEM1_BASE		CONFIG_SPIKE_MEM1_BASE
#define MEM1_SIZE		CONFIG_SPIKE_MEM1_SIZE
/* MEM2 and more memory banks can be specified via -m<a:m,b:n,...>
 * However only MEM2 is used in case we need a ROM to test self loader.
 */
#ifdef CONFIG_SPIKE_MEM2
#define MEM2_BASE		CONFIG_SPIKE_MEM2_BASE
#define MEM2_SIZE		CONFIG_SPIKE_MEM2_SIZE
#endif

#ifdef CONFIG_SPIKE_MEM2_ROM
/* Use MEM2 as ROM to test some features */
#define ROM_BASE		MEM2_BASE
#define ROMEND			(MEM2_BASE + MEM2_SIZE)
#else
/* Default ROM is hard coded in spike and is not programmable.
 * Located in 0x0000_0000, contains the following instructions:
 *  0x297			// auipc  t0,0x0
 *  0x28593 + (ROM_SIZE << 20)	// addi   a1, t0, &dtb
 *  0xf1402573			// csrr   a0, mhartid
 *  32bit: 0x0182a283u		// lw     t0,24(t0)
 *  64bit: 0x0182b283u		// ld     t0,24(t0)
 *  0x28067			// jr     t0
 *  0
 *  HIWORD(entry)
 *  LOWORD(entry)
 */
#define ROM_BASE		UL(0x00000000)
#define ROM_SIZE		UL(0x20)
#endif
#define RAM_BASE		MEM1_BASE
#define RAMEND			(MEM1_BASE + MEM1_SIZE)

#endif /* __REG_SPIKE_H_INCLUDE__ */
