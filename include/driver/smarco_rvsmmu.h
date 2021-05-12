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
 * @(#)smarco_rvsmmu.h: SmarCo RISCV system mmu (RVSMMU) interfaces
 * $Id: smarco_rvsmmu.h,v 1.1 2020-10-17 07:46:00 zhenglv Exp $
 */

#ifndef __SMARCO_RVSMMU_H_INLCUDE__
#define __SMARCO_RVSMMU_H_INCLUDE__

#include <target/generic.h>
#include <driver/arm_smmuv2.h>

/* Physical Memory Protection */
#define SMMU_PMP_BASE(smmu)		(SMMU_SSD_BASE(smmu) + SMMU_HW_PAGESIZE)

#define SMMU_PMP_REG(smmu, offset)	(SMMU_PMP_BASE(smmu) + (offset))

/* RISCV Enable */
#define SMMU_RISCV(smmu)		SMMU_GR0_REG(smmu, 0x018)
/* Invalidate PMA, RISCV */
#define SMMU_TLBIPMA(smmu)		SMMU_GR0_REG(smmu, 0x0D0)
/* Bypass PMP Selection */
#define SMMU_BYPASS_PMP_SEL(smmu, n)	SMMU_GR0_REG(smmu, 0xE00 + ((n) << 2))
/* Context Bank Attribute Registers */
#define SMMU_CBA2R(smmu, n)		SMMU_GR1_REG(smmu, 0x800 + ((n) << 2))
/* Translation Control Register 2 */
#define SMMU_TCR2(smmu, n)		SMMU_CB_REG(smmu, n, 0x010)
/* PMP Selection */
#define SMMU_PMP_SEL(smmu, n)		SMMU_CB_REG(smmu, n, 0x14)
/* Supervisor Address Translation and Protection Register, stage 1 */
#define SMMU_SATP(smmu, n)		SMMU_CB_REG(smmu, n, 0x18)

/* PMP Registers */
#define SMMU_PMP_CFG(smmu, n)		SMMU_PMP_REG(smmu, 0x00 + ((n) << 3))
#define SMMU_PMP_ADDR(smmu, n)		SMMU_PMP_REG(smmu, 0x20 + ((n) << 3))

/* SMARCO SMMU 3.2 register descriptions */
#define SMMU_RISCV_EN			_BV(0)

#define smmu_riscv_enable(smmu)		\
	__raw_setl(SMMU_RISCV_EN, SMMU_RISCV(smmu))
#define smmu_riscv_disable(smmu)	\
	__raw_clearl(SMMU_RISCV_EN, SMMU_RISCV(smmu))

#define smmu_enable_global_bypass(smmu)		\
	do {					\
		smmu_riscv_enable(smmu);	\
		smmu_disable(smmu);		\
	} while (0)

#endif /* __SMARCO_RVSMMU_H_INCLUDE__ */
