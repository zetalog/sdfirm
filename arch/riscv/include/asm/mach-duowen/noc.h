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
 * @(#)noc.h: DUOWEN specific network on chip (NoC) interface
 * $Id: noc.h,v 1.0 2020-08-07 23:24:00 zhenglv Exp $
 */

#ifndef __NOC_DUOWEN_H_INCLUDE__
#define __NOC_DUOWEN_H_INCLUDE__

#include <target/arch.h>

#define NCORE_BASE		COHFAB_BASE
#include <driver/ncore.h>
#ifndef ARCH_HAVE_NOC
#define ARCH_HAVE_NOC		1
#else
#error "Multiple NOC controller defined"
#endif

#define MAX_DDR_SEGMENTS	2

#define noc_hw_ctrl_init()	\
	ncore_init(MAX_CPU_CLUSTERS, 0, MAX_DDR_SEGMENTS, MAX_DDR_SEGMENTS)

#ifdef CONFIG_DUOWEN_BOOT_APC
#define duowen_imc_noc_init()	do { } while (0)
#define duowen_apc_noc_init()	noc_hw_ctrl_init()
#else
#define duowen_imc_noc_init()	noc_hw_ctrl_init()
#define duowen_apc_noc_init()	do { } while (0)
#endif

#endif /* __NOC_DUOWEN_H_INCLUDE__ */
