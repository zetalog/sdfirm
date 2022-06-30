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
 * @(#)arch.h: VIRAPA machine specific definitions
 * $Id: arch.h,v 1.1 2022-06-27 17:19:00 zhenglv Exp $
 */

#ifndef __ARCH_VIRAPA_H_INCLUDE__
#define __ARCH_VIRAPA_H_INCLUDE__

#ifdef CONFIG_HTIF
#include <asm/htif.h>
#endif
#ifdef CONFIG_VAISRA_PMA
#include <asm/vaisra_pma.h>
#endif

/* This file is intended to be used for implementing SoC specific
 * instructions, registers.
 */

#define OSC_FREQ		33333333
#define CPU_FREQ		(52 * OSC_FREQ)

#ifndef __ASSEMBLY__
void vaisra_error(void);
#ifdef CONFIG_VIRAPA_BOOT_RAM
void virapa_cpu_init(void);
#else
#define virapa_cpu_init()	do { } while (0)
#endif
#ifdef CONFIG_SHUTDOWN
void virapa_sim_shutdown(void);
#else
#define virapa_sim_shutdown()	do { } while (0)
#endif
#endif

#endif /* __ARCH_VIRAPA_H_INCLUDE__ */
