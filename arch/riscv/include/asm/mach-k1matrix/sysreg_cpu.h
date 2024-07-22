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
 * @(#)sysreg_cpu.h: K1Matrix system registers definitions
 * $Id: sysreg_cpu.h,v 1.1 2024-07-18 19:48:00 zhenglv Exp $
 */
#include <asm/mach/reg.h>

#define SYS_CPU_OFFSET				(DIE0_BASE + __RMU_SYS_REG_BASE + 0x400)
#define CLUSTER(cluster,offset)			(SYS_CPU_OFFSET + cluster * 0x80 + offset * 0x4)
#define CPUREG_REG(cluster,core,offset)		(CLUSTER(cluster,offset) + core * 0x10 + 0x10)

#define C0_L2_FLUSH_REG				CLUSTER(0, 0)
#define C0_PWR_CFG_REG				CLUSTER(0, 1)
#define C0_HAP_DM_UNAVAIL_REG			CLUSTER(0, 2)
#define C0_CORE0_RVBA_L_REG			CPUREG_REG(0, 0, 0)
#define C0_CORE0_RVBA_H_REG			CPUREG_REG(0, 0, 1)
#define C0_CORE0_RVBA_CFG_REG			CPUREG_REG(0, 0, 2)
#define C0_CORE1_RVBA_L_REG			CPUREG_REG(0, 1, 0)
#define C0_CORE1_RVBA_H_REG			CPUREG_REG(0, 1, 1)
#define C0_CORE1_RVBA_CFG_REG			CPUREG_REG(0, 1, 2)
#define C0_CORE2_RVBA_L_REG			CPUREG_REG(0, 2, 0)
#define C0_CORE2_RVBA_H_REG			CPUREG_REG(0, 2, 1)
#define C0_CORE2_RVBA_CFG_REG			CPUREG_REG(0, 2, 2)
#define C0_CORE3_RVBA_L_REG			CPUREG_REG(0, 3, 0)
#define C0_CORE3_RVBA_H_REG			CPUREG_REG(0, 3, 1)
#define C0_CORE3_RVBA_CFG_REG			CPUREG_REG(0, 3, 2)

#define C1_L2_FLUSH_REG				CLUSTER(1, 0)
#define C1_PWR_CFG_REG				CLUSTER(1, 1)
#define C1_HAP_DM_UNAVAIL_REG			CLUSTER(1, 2)
#define C1_CORE0_RVBA_L_REG			CPUREG_REG(1, 0, 0)
#define C1_CORE0_RVBA_H_REG			CPUREG_REG(1, 0, 1)
#define C1_CORE0_RVBA_CFG_REG			CPUREG_REG(1, 0, 2)
#define C1_CORE1_RVBA_L_REG			CPUREG_REG(1, 1, 0)
#define C1_CORE1_RVBA_H_REG			CPUREG_REG(1, 1, 1)
#define C1_CORE1_RVBA_CFG_REG			CPUREG_REG(1, 1, 2)
#define C1_CORE2_RVBA_L_REG			CPUREG_REG(1, 2, 0)
#define C1_CORE2_RVBA_H_REG			CPUREG_REG(1, 2, 1)
#define C1_CORE2_RVBA_CFG_REG			CPUREG_REG(1, 2, 2)
#define C1_CORE3_RVBA_L_REG			CPUREG_REG(1, 3, 0)
#define C1_CORE3_RVBA_H_REG			CPUREG_REG(1, 3, 1)
#define C1_CORE3_RVBA_CFG_REG			CPUREG_REG(1, 3, 2)

#define C2_L2_FLUSH_REG				CLUSTER(2, 0)
#define C2_PWR_CFG_REG				CLUSTER(2, 1)
#define C2_HAP_DM_UNAVAIL_REG			CLUSTER(2, 2)
#define C2_CORE0_RVBA_L_REG			CPUREG_REG(2, 0, 0)
#define C2_CORE0_RVBA_H_REG			CPUREG_REG(2, 0, 1)
#define C2_CORE0_RVBA_CFG_REG			CPUREG_REG(2, 0, 2)
#define C2_CORE1_RVBA_L_REG			CPUREG_REG(2, 1, 0)
#define C2_CORE1_RVBA_H_REG			CPUREG_REG(2, 1, 1)
#define C2_CORE1_RVBA_CFG_REG			CPUREG_REG(2, 1, 2)
#define C2_CORE2_RVBA_L_REG			CPUREG_REG(2, 2, 0)
#define C2_CORE2_RVBA_H_REG			CPUREG_REG(2, 2, 1)
#define C2_CORE2_RVBA_CFG_REG			CPUREG_REG(2, 2, 2)
#define C2_CORE3_RVBA_L_REG			CPUREG_REG(2, 3, 0)
#define C2_CORE3_RVBA_H_REG			CPUREG_REG(2, 3, 1)
#define C2_CORE3_RVBA_CFG_REG			CPUREG_REG(2, 3, 2)

#define C3_L2_FLUSH_REG				CLUSTER(3, 0)
#define C3_PWR_CFG_REG				CLUSTER(3, 1)
#define C3_HAP_DM_UNAVAIL_REG			CLUSTER(3, 2)
#define C3_CORE0_RVBA_L_REG			CPUREG_REG(3, 0, 0)
#define C3_CORE0_RVBA_H_REG			CPUREG_REG(3, 0, 1)
#define C3_CORE0_RVBA_CFG_REG			CPUREG_REG(3, 0, 2)
#define C3_CORE1_RVBA_L_REG			CPUREG_REG(3, 1, 0)
#define C3_CORE1_RVBA_H_REG			CPUREG_REG(3, 1, 1)
#define C3_CORE1_RVBA_CFG_REG			CPUREG_REG(3, 1, 2)
#define C3_CORE2_RVBA_L_REG			CPUREG_REG(3, 2, 0)
#define C3_CORE2_RVBA_H_REG			CPUREG_REG(3, 2, 1)
#define C3_CORE2_RVBA_CFG_REG			CPUREG_REG(3, 2, 2)
#define C3_CORE3_RVBA_L_REG			CPUREG_REG(3, 3, 0)
#define C3_CORE3_RVBA_H_REG			CPUREG_REG(3, 3, 1)
#define C3_CORE3_RVBA_CFG_REG			CPUREG_REG(3, 3, 2)

#define C4_L2_FLUSH_REG				CLUSTER(4, 0)
#define C4_PWR_CFG_REG				CLUSTER(4, 1)
#define C4_HAP_DM_UNAVAIL_REG			CLUSTER(4, 2)
#define C4_CORE0_RVBA_L_REG			CPUREG_REG(4, 0, 0)
#define C4_CORE0_RVBA_H_REG			CPUREG_REG(4, 0, 1)
#define C4_CORE0_RVBA_CFG_REG			CPUREG_REG(4, 0, 2)
#define C4_CORE1_RVBA_L_REG			CPUREG_REG(4, 1, 0)
#define C4_CORE1_RVBA_H_REG			CPUREG_REG(4, 1, 1)
#define C4_CORE1_RVBA_CFG_REG			CPUREG_REG(4, 1, 2)
#define C4_CORE2_RVBA_L_REG			CPUREG_REG(4, 2, 0)
#define C4_CORE2_RVBA_H_REG			CPUREG_REG(4, 2, 1)
#define C4_CORE2_RVBA_CFG_REG			CPUREG_REG(4, 2, 2)
#define C4_CORE3_RVBA_L_REG			CPUREG_REG(4, 3, 0)
#define C4_CORE3_RVBA_H_REG			CPUREG_REG(4, 3, 1)
#define C4_CORE3_RVBA_CFG_REG			CPUREG_REG(4, 3, 2)

#define C5_L2_FLUSH_REG				CLUSTER(5, 0)
#define C5_PWR_CFG_REG				CLUSTER(5, 1)
#define C5_HAP_DM_UNAVAIL_REG			CLUSTER(5, 2)
#define C5_CORE0_RVBA_L_REG			CPUREG_REG(5, 0, 0)
#define C5_CORE0_RVBA_H_REG			CPUREG_REG(5, 0, 1)
#define C5_CORE0_RVBA_CFG_REG			CPUREG_REG(5, 0, 2)
#define C5_CORE1_RVBA_L_REG			CPUREG_REG(5, 1, 0)
#define C5_CORE1_RVBA_H_REG			CPUREG_REG(5, 1, 1)
#define C5_CORE1_RVBA_CFG_REG			CPUREG_REG(5, 1, 2)
#define C5_CORE2_RVBA_L_REG			CPUREG_REG(5, 2, 0)
#define C5_CORE2_RVBA_H_REG			CPUREG_REG(5, 2, 1)
#define C5_CORE2_RVBA_CFG_REG			CPUREG_REG(5, 2, 2)
#define C5_CORE3_RVBA_L_REG			CPUREG_REG(5, 3, 0)
#define C5_CORE3_RVBA_H_REG			CPUREG_REG(5, 3, 1)
#define C5_CORE3_RVBA_CFG_REG			CPUREG_REG(5, 3, 2)

#define C6_L2_FLUSH_REG				CLUSTER(6, 0)
#define C6_PWR_CFG_REG				CLUSTER(6, 1)
#define C6_HAP_DM_UNAVAIL_REG			CLUSTER(6, 2)
#define C6_CORE0_RVBA_L_REG			CPUREG_REG(6, 0, 0)
#define C6_CORE0_RVBA_H_REG			CPUREG_REG(6, 0, 1)
#define C6_CORE0_RVBA_CFG_REG			CPUREG_REG(6, 0, 2)
#define C6_CORE1_RVBA_L_REG			CPUREG_REG(6, 1, 0)
#define C6_CORE1_RVBA_H_REG			CPUREG_REG(6, 1, 1)
#define C6_CORE1_RVBA_CFG_REG			CPUREG_REG(6, 1, 2)
#define C6_CORE2_RVBA_L_REG			CPUREG_REG(6, 2, 0)
#define C6_CORE2_RVBA_H_REG			CPUREG_REG(6, 2, 1)
#define C6_CORE2_RVBA_CFG_REG			CPUREG_REG(6, 2, 2)
#define C6_CORE3_RVBA_L_REG			CPUREG_REG(6, 3, 0)
#define C6_CORE3_RVBA_H_REG			CPUREG_REG(6, 3, 1)
#define C6_CORE3_RVBA_CFG_REG			CPUREG_REG(6, 3, 2)

#define C7_L2_FLUSH_REG				CLUSTER(7, 0)
#define C7_PWR_CFG_REG				CLUSTER(7, 1)
#define C7_HAP_DM_UNAVAIL_REG			CLUSTER(7, 2)
#define C7_CORE0_RVBA_L_REG			CPUREG_REG(7, 0, 0)
#define C7_CORE0_RVBA_H_REG			CPUREG_REG(7, 0, 1)
#define C7_CORE0_RVBA_CFG_REG			CPUREG_REG(7, 0, 2)
#define C7_CORE1_RVBA_L_REG			CPUREG_REG(7, 1, 0)
#define C7_CORE1_RVBA_H_REG			CPUREG_REG(7, 1, 1)
#define C7_CORE1_RVBA_CFG_REG			CPUREG_REG(7, 1, 2)
#define C7_CORE2_RVBA_L_REG			CPUREG_REG(7, 2, 0)
#define C7_CORE2_RVBA_H_REG			CPUREG_REG(7, 2, 1)
#define C7_CORE2_RVBA_CFG_REG			CPUREG_REG(7, 2, 2)
#define C7_CORE3_RVBA_L_REG			CPUREG_REG(7, 3, 0)
#define C7_CORE3_RVBA_H_REG			CPUREG_REG(7, 3, 1)
#define C7_CORE3_RVBA_CFG_REG			CPUREG_REG(7, 3, 2)