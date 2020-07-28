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
 * @(#)vaisra_hpm.h: VAISRA hardware performance monitor events
 * $Id: vaisra_hpm.h,v 1.1 2020-07-28 21:45:00 zhenglv Exp $
 */

#ifndef __HPM_VAISRA_H_INCLUDE__
#define __HPM_VAISRA_H_INCLUDE__

#define HPM_SWINCR		0x0000
#define HPM_L1ICRF		0x0001 /* L1 I-cache refill */
#define HPM_L1ITRF		0x0002 /* L1 I-tlb refill */
#define HPM_L1DCRF		0x0003 /* L1 D-cache refill */
#define HPM_L1DCAC		0x0004 /* L1 D-cache access */
#define HPM_LDRET		0x0005 /* load retired */
#define HPM_STRET		0x0007 /* store retired */
#define HPM_EXCENTR		0x0009 /* exception entry */
#define HPM_EXCEXIT		0x000A /* exception exit */
#define HPM_BRUNPRED		0x0010 /* branch mis-predicted */
#define HPM_BRPRED		0x0012 /* branch predicted */
#define HPM_MEMACC		0x0013 /* data memory access */
#define HPM_L1ICAC		0x0014 /* L1 I-cache access */
#define HPM_L2DCAC		0x0016 /* L2 D-cache access */
#define HPM_L2DCRF		0x0017 /* L2 D-cache refill */
#define HPM_L2DCWB		0x0018 /* L2 D-cache write-back */
#define HPM_BUSACC		0x0019 /* bus access */
#define HPM_MEMERR		0x001A /* memory error */
#define HPM_INSTSPEC		0x001B /* inst speculatively executed */
#define HPM_SATPRET		0x001C /* SATP-write retired */
#define HPM_L1DCAL		0x001F /* L1 D-cache allocate */
#define HPM_L2DCAL		0x0020 /* L2 D-cache allocate */
#define HPM_STALLFE		0x0023 /* stall frontend */
#define HPM_STALLBE		0x0024 /* stall backend */
#define HPM_L1DTAC		0x0025 /* L1 D-tlb access */
#define HPM_L1ITAC		0x0026 /* L1 I-tlb access */
#define HPM_L2DTRF		0x002D /* L2 D-tlb refill */
#define HPM_L2ITRF		0x002E /* L2 I-tlb refill */
#define HPM_L2DTAC		0x002F /* L2 D-tlb access */
#define HPM_L2ITAC		0x0030 /* L2 I-tlb access */

#include <asm/hpm.h>

#endif /* __HPM_VAISRA_H_INCLUDE__ */
