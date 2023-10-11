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
 * @(#)sysreg.h: K1-max system registers definitions
 * $Id: sysreg.h,v 1.1 2023-05-30 18:48:00 zhenglv Exp $
 */

#ifndef __SYSREG_K1MAX_H_INCLUDE__
#define __SYSREG_K1MAX_H_INCLUDE__

#include <asm/mach/reg.h>

#define SYSREG_REG(offset)	(SYSREG_BASE + (offset))

#define SYS_IP_CTRL_0		SYSREG_REG(0x00)
#define SYS_IP_CTRL_1		SYSREG_REG(0x04)
#define SYS_GLB_SOFTWARE_RST	SYSREG_REG(0x08)
#define SYS_DDR_RD_DLY_CNT	SYSREG_REG(0x0C)
#define SYS_CPU_SOFTWARE_RST	SYSREG_REG(0x10)

#define SYS_CPU_RST(cpu)		_BV((cpu) - 1)
#define SYS_CLUSTER_RST(cluster)	_BV((cluster) + 6)

#define sysreg_soft_reset()		\
	__raw_writel(0xAA55A5A5, SYS_GLB_SOFTWARE_RST)
#define sysreg_soft_reset_cpu(cpu)	\
	__raw_setl(SYS_CPU_RST(cpu), SYS_CPU_SOFTWARE_RST)
#define sysreg_soft_reset_cluster(cl)	\
	__raw_setl(SYS_CLUSTER_RST(cpu), SYS_CPU_SOFTWARE_RST)

#ifndef __ASSEMBLY__
#ifdef CONFIG_K1M_SOC
void k1max_cpu_reset(void);
#else
#define k1max_cpu_reset()		do { } while (0)
#endif
#endif

#endif /* __SYSREG_K1MAX_H_INCLUDE__ */
