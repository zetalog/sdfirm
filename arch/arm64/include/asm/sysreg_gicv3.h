/*
 * ZETALOG's Personal COPYRIGHT
 *
 * Copyright (c) 2018
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
 * @(#)sysreg_gicv3.h: GICv3 system registers definitions
 * $Id: sysreg_gicv3.h,v 1.279 2018-8-8 10:19:18 zhenglv Exp $
 */
#ifndef __SYSREG_GICV3_ARM64_H_INCLUDE__
#define __SYSREG_GICV3_ARM64_H_INCLUDE__

#include <asm/sysreg.h>

/* ARM Generic Interrupt Controller Architecture Specification
 * GIC architecture version 3.0 and version 4.0
 */

/* 8.2 AArch64 System register descriptions */
#define ICC_PMR_EL1			S3_0_C4_C6_0
#define ICC_IAR0_EL1			S3_0_C12_C8_0
#define ICC_EOIR0_EL1			S3_0_C12_C8_1
#define ICC_HPPIR0_EL1			S3_0_C12_C8_2
#define ICC_BPR0_EL1			S3_0_C12_C8_3
#define ICC_AP0R0_EL1			S3_0_C12_C8_4
#define ICC_AP0R1_EL1			S3_0_C12_C8_5
#define ICC_AP0R2_EL1			S3_0_C12_C8_6
#define ICC_AP0R3_EL1			S3_0_C12_C8_7
#define ICC_AP1R0_EL1			S3_0_C12_C9_0
#define ICC_AP1R1_EL1			S3_0_C12_C9_1
#define ICC_AP1R2_EL1			S3_0_C12_C9_1
#define ICC_AP1R3_EL1			S3_0_C12_C9_3
#define ICC_DIR_EL1			S3_0_C12_C11_1
#define ICC_RPR_EL1			S3_0_C12_C11_3
#define ICC_SGI1R_EL1			S3_0_C12_C11_5
#define ICC_AGI1R_EL1			S3_0_C12_C11_6
#define ICC_SGI0R_EL1			S3_0_C12_C11_7
#define ICC_IAR1_EL1			S3_0_C12_C12_0
#define ICC_EOIR1_EL1			S3_0_C12_C12_1
#define ICC_HPPIR1_EL1			S3_0_C12_C12_2
#define ICC_BPR1_EL1			S3_0_C12_C12_3
#define ICC_CTLR_EL1			S3_0_C12_C12_4
#define ICC_SRE_EL1			S3_0_C12_C12_5
#define ICC_IGRPEN0_EL1			S3_0_C12_C12_6
#define ICC_IGRPEN1_EL1			S3_0_C12_C12_7
#define ICC_SRE_EL2			S3_4_C12_C9_5
#define ICC_CTLR_EL3			S3_6_C12_C12_4
#define ICC_SRE_EL3			S3_6_C12_C12_5
#define ICC_IGRPEN1_EL3			S3_6_C12_C12_7

/* 8.3 AArch64 System register descriptions of the virtual registers
 *
 * The ICV_* registers are only accessible at Non-secure EL1. Whether an
 * access encoding maps to an ICC_* register or the equivalent ICV_*
 * register is determined by HCR_EL2.
 */

/* 8.4 AArch64 virtualization control System registers */
#define ICH_AP0R0_EL2			S3_4_C12_C8_0
#define ICH_AP0R1_EL2			S3_4_C12_C8_1
#define ICH_AP0R2_EL2			S3_4_C12_C8_2
#define ICH_AP0R3_EL2			S3_4_C12_C8_3
#define ICH_AP1R0_EL2			S3_4_C12_C9_0
#define ICH_AP1R1_EL2			S3_4_C12_C9_1
#define ICH_AP1R2_EL2			S3_4_C12_C9_2
#define ICH_AP1R3_EL2			S3_4_C12_C9_3
#define ICH_HCR_EL2			S3_4_C12_C11_0
#define ICH_VTR_EL2			S3_4_C12_C11_1
#define ICH_MISR_EL2			S3_4_C12_C11_2
#define ICH_EISR_EL2			S3_4_C12_C11_3
#define ICH_ELRSR_EL2			S3_4_C12_C11_5
#define ICH_VMCR_EL2			S3_4_C12_C11_7
#define ICH_LR0_EL2			S3_4_C12_C12_0
#define ICH_LR1_EL2			S3_4_C12_C12_1
#define ICH_LR2_EL2			S3_4_C12_C12_2
#define ICH_LR3_EL2			S3_4_C12_C12_3
#define ICH_LR4_EL2			S3_4_C12_C12_4
#define ICH_LR5_EL2			S3_4_C12_C12_5
#define ICH_LR6_EL2			S3_4_C12_C12_6
#define ICH_LR7_EL2			S3_4_C12_C12_7
#define ICH_LR8_EL2			S3_4_C12_C13_0
#define ICH_LR9_EL2			S3_4_C12_C13_1
#define ICH_LR10_EL2			S3_4_C12_C13_2
#define ICH_LR11_EL2			S3_4_C12_C13_3
#define ICH_LR12_EL2			S3_4_C12_C13_4
#define ICH_LR13_EL2			S3_4_C12_C13_5
#define ICH_LR14_EL2			S3_4_C12_C13_6
#define ICH_LR15_EL2			S3_4_C12_C13_7

#endif /* __SYSREG_GICV3_ARM64_H_INCLUDE__ */
