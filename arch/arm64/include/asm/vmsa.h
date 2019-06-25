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
 * @(#)vmsa.h: virtual memory system architecture (VMSA) definitions
 * $Id: vmsa.h,v 1.279 2018-7-19 10:19:18 zhenglv Exp $
 */

#ifndef __VMSA_ARM64_H_INCLUDE__
#define __VMSA_ARM64_H_INCLUDE__

#include <target/const.h>
#include <asm/reg.h>
#include <asm/sysreg.h>

#ifdef VMSA_VA_4PB
#define VMSA_VA_SIZE_SHIFT	52
#endif
#ifdef VMSA_VA_256TB
#define VMSA_VA_SIZE_SHIFT	48
#endif
#ifdef VMSA_VA_16TB
#define VMSA_VA_SIZE_SHIFT	44
#endif
#ifdef VMSA_VA_1TB
#define VMSA_VA_SIZE_SHIFT	40
#endif
#ifdef VMSA_VA_64GB
#define VMSA_VA_SIZE_SHIFT	36
#endif
#ifdef VMSA_VA_4GB
#define VMSA_VA_SIZE_SHIFT	32
#endif

#ifndef VMSA_VA_SHIFT_SIZE
#ifdef CONFIG_CPU_64v8_2_LVA
#define VMSA_VA_SIZE_SHIFT	52
#else
#define VMSA_VA_SIZE_SHIFT	48
#endif
#endif

#ifdef CONFIG_CPU_64v8_2_LPA
#define VMSA_PA_SIZE_SHIFT	52
#else
#define VMSA_PA_SIZE_SHIFT	48
#endif

#ifdef CONFIG_VMSA_PHYS_OFFSET
#define PHYS_OFFSET		CONFIG_VMSA_PHYS_OFFSET
#endif

/* #include <asm/mach/arch.h> */
#ifdef CONFIG_VMSA_VA_2_RANGES
#define VMSA_VA_BASE_HI		(ULL(0) - (ULL(1) << VMSA_VA_SIZE_SHIFT))
#define VMSA_VA_BASE_LO		ULL(0x0000000000000000)
#else
#define VMSA_VA_BASE_HI		ULL(0x0000000000000000)
#define VMSA_VA_BASE_LO		ULL(0x0000000000000000)
#endif

#define MAIR(attr, mt)		((attr) << ((mt) * 8))
/* Encoding index */
#define MT_DEVICE_nGnRnE	0
#define MT_DEVICE_nGnRE		1
#define MT_DEVICE_GRE		2
#define MT_NORMAL_NC		3
#define MT_NORMAL		4
#define MT_NORMAL_WT		5
#define MT_DEVICE_nGRE		6

#ifdef CONFIG_VMSA_ADDRESS_TAGGING
#if (EL_RUNTIME == ARM_EL3)
#define is_tbi(address)		(!!(read_sysreg(TCR_EL3) & TCR_TBI))
#elif (EL_RUNTIME == ARM_EL2)
#ifdef CONFIG_CPU_64v8_1_VHE
#define is_tbi(address)				\
	(((uintptrt_t)address) & _BV_ULL(55) ?	\
	 !!(read_sysreg(TCR_EL2) & TCR_TBI1) :	\
	 !!(read_sysreg(TCR_EL2) & TCR_TBI0))
#else
#define is_tbi(address)		(!!(read_sysreg(TCR_EL2) & TCR_TBI))
#endif
#elif (EL_RUNTIME == ARM_EL1)
#define is_tbi(address)				\
	(((uintptrt_t)address) & _BV_ULL(55) ?	\
	 !!(read_sysreg(TCR_EL1) & TCR_TBI1) :	\
	 !!(read_sysreg(TCR_EL1) & TCR_TBI0))
#else
#define is_tbi(address)		true
#endif
#else
#define is_tbi(address)		true
#endif

#define write_ttbr_el1(tbl, top)	 \
	((top) ? write_sysreg(tbl, TTBR1_EL1) : write_sysreg(tbl, TTBR0_EL1))
#ifdef CONFIG_SYS_HOST
#define write_ttbr_el2(tbl, top)	 \
	((top) ? write_sysreg(tbl, TTBR1_EL2) : write_sysreg(tbl, TTBR0_EL2))
#else
#define write_ttbr_el2(tbl, top)	 write_sysreg(tbl, TTBR0_EL2)
#endif
#define write_ttbr_el3(tbl, top)	 write_sysreg(tbl, TTBR0_EL3)

#ifndef __ASSEMBLY__
static inline void write_ttbr(caddr_t tbl, uint8_t el, bool top)
{
	switch (el) {
	case ARM_EL1:
		write_ttbr_el1(tbl, top);
		break;
	case ARM_EL2:
		write_ttbr_el2(tbl, top);
		break;
	case ARM_EL3:
		write_ttbr_el3(tbl, top);
		break;
	default:
		break;
	}
}
#endif

#endif /* __VMSA_ARM64_H_INCLUDE__ */
