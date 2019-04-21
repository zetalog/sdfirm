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
 * @(#)sysop_pmuv3.h: ARM64 PMUv3 system opcode defintions
 * $Id: sysop_pmuv3.h,v 1.279 2019-04-14 10:19:18 zhenglv Exp $
 */

#ifndef __SYSREG_PMUV3_ARM64_H_INCLUDE__
#define __SYSREG_PMUV3_ARM64_H_INCLUDE__

#include <asm/sysreg.h>

#if 0
#define PMINTENSET_EL1		S3_0_C9_C14_1
#define PMINTENCLR_EL1		S3_0_C9_C14_2

#define PMCR_EL0		S3_3_C9_C12_0
#define PMCNTENSET_EL0		S3_3_C9_C12_1
#define PMCNTENCLR_EL0		S3_3_C9_C12_2
#define PMOVSCLR_EL0		S3_3_C9_C12_3
#define PMSWINC_EL0		S3_3_C9_C12_4
#define PMSELR_EL0		S3_3_C9_C12_5
#define PMCEID0_EL0		S3_3_C9_C12_6
#define PMCEID1_EL0		S3_3_C9_C12_7
#define PMCCNTR_EL0		S3_3_C9_C13_0
#define PMXEVTYPER_EL0		S3_3_C9_C13_1
#define PMXEVCNTR_EL0		S3_3_C9_C13_2
#define PMUSERENR_EL0		S3_3_C9_C14_0
#define PMOVSSET_EL0		S3_3_C9_C14_3

#define PMEVCNTR0_EL0		S3_3_C14_C8_0
#define PMEVCNTR1_EL0		S3_3_C14_C8_1
#define PMEVCNTR2_EL0		S3_3_C14_C8_2
#define PMEVCNTR3_EL0		S3_3_C14_C8_3
#define PMEVCNTR4_EL0		S3_3_C14_C8_4
#define PMEVCNTR5_EL0		S3_3_C14_C8_5
#define PMEVCNTR6_EL0		S3_3_C14_C8_6
#define PMEVCNTR7_EL0		S3_3_C14_C8_7
#define PMEVCNTR8_EL0		S3_3_C14_C9_0
#define PMEVCNTR9_EL0		S3_3_C14_C9_1
#define PMEVCNTR10_EL0		S3_3_C14_C9_2
#define PMEVCNTR11_EL0		S3_3_C14_C9_3
#define PMEVCNTR12_EL0		S3_3_C14_C9_4
#define PMEVCNTR13_EL0		S3_3_C14_C9_5
#define PMEVCNTR14_EL0		S3_3_C14_C9_6
#define PMEVCNTR15_EL0		S3_3_C14_C9_7
#define PMEVCNTR16_EL0		S3_3_C14_C10_0
#define PMEVCNTR17_EL0		S3_3_C14_C10_1
#define PMEVCNTR18_EL0		S3_3_C14_C10_2
#define PMEVCNTR19_EL0		S3_3_C14_C10_3
#define PMEVCNTR20_EL0		S3_3_C14_C10_4
#define PMEVCNTR21_EL0		S3_3_C14_C10_5
#define PMEVCNTR22_EL0		S3_3_C14_C10_6
#define PMEVCNTR23_EL0		S3_3_C14_C10_7
#define PMEVCNTR24_EL0		S3_3_C14_C11_0
#define PMEVCNTR25_EL0		S3_3_C14_C11_1
#define PMEVCNTR26_EL0		S3_3_C14_C11_2
#define PMEVCNTR27_EL0		S3_3_C14_C11_3
#define PMEVCNTR28_EL0		S3_3_C14_C11_4
#define PMEVCNTR29_EL0		S3_3_C14_C11_5
#define PMEVCNTR30_EL0		S3_3_C14_C11_6

#define PMEVTYPER0_EL0		S3_3_C14_C12_0
#define PMEVTYPER1_EL0		S3_3_C14_C12_1
#define PMEVTYPER2_EL0		S3_3_C14_C12_2
#define PMEVTYPER3_EL0		S3_3_C14_C12_3
#define PMEVTYPER4_EL0		S3_3_C14_C12_4
#define PMEVTYPER5_EL0		S3_3_C14_C12_5
#define PMEVTYPER6_EL0		S3_3_C14_C12_6
#define PMEVTYPER7_EL0		S3_3_C14_C12_7
#define PMEVTYPER8_EL0		S3_3_C14_C13_0
#define PMEVTYPER9_EL0		S3_3_C14_C13_1
#define PMEVTYPER10_EL0		S3_3_C14_C13_2
#define PMEVTYPER11_EL0		S3_3_C14_C13_3
#define PMEVTYPER12_EL0		S3_3_C14_C13_4
#define PMEVTYPER13_EL0		S3_3_C14_C13_5
#define PMEVTYPER14_EL0		S3_3_C14_C13_6
#define PMEVTYPER15_EL0		S3_3_C14_C13_7
#define PMEVTYPER16_EL0		S3_3_C14_C14_0
#define PMEVTYPER17_EL0		S3_3_C14_C14_1
#define PMEVTYPER18_EL0		S3_3_C14_C14_2
#define PMEVTYPER19_EL0		S3_3_C14_C14_3
#define PMEVTYPER20_EL0		S3_3_C14_C14_4
#define PMEVTYPER21_EL0		S3_3_C14_C14_5
#define PMEVTYPER22_EL0		S3_3_C14_C14_6
#define PMEVTYPER23_EL0		S3_3_C14_C14_7
#define PMEVTYPER24_EL0		S3_3_C14_C15_0
#define PMEVTYPER25_EL0		S3_3_C14_C15_1
#define PMEVTYPER26_EL0		S3_3_C14_C15_2
#define PMEVTYPER27_EL0		S3_3_C14_C15_3
#define PMEVTYPER28_EL0		S3_3_C14_C15_4
#define PMEVTYPER29_EL0		S3_3_C14_C15_5
#define PMEVTYPER30_EL0		S3_3_C14_C15_6

#define PMCCFILTR_EL0		S3_3_C14_C15_7
#endif

#endif /* __SYSREG_PMUV3_ARM64_H_INCLUDE__ */
