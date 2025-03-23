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
 * @(#)aplic.h: advanced platform level interrupt controller (APLIC) definitions
 * $Id: aplic.h,v 1.1 2023-3-11 15:43:00 zhenglv Exp $
 */
#ifndef __APLIC_RISCV_H_INCLUDE__
#define __APLIC_RISCV_H_INCLUDE__

#include <target/arch.h>

#define APLIC_MAX_DELEGATE	16

struct aplic_msicfg_data {
	unsigned long lhxs;
	unsigned long lhxw;
	unsigned long hhxs;
	unsigned long hhxw;
	unsigned long base_addr;
};

struct aplic_delegate_data {
	uint32_t first_irq;
	uint32_t last_irq;
	uint32_t child_index;
};

struct aplic_data {
	unsigned long addr;
	unsigned long size;
	unsigned long num_idc;
	unsigned long num_source;
	bool targets_mmode;
	bool has_msicfg_mmode;
	struct aplic_msicfg_data msicfg_mmode;
	bool has_msicfg_smode;
	struct aplic_msicfg_data msicfg_smode;
	struct aplic_delegate_data delegate[APLIC_MAX_DELEGATE];
};

#define APLIC_MAX_IDC			(1UL << 14)
#define APLIC_MAX_SOURCE		1024

#define APLIC_DOMAINCFG			0x0000
#define APLIC_DOMAINCFG_IE		(1 << 8)
#define APLIC_DOMAINCFG_DM		(1 << 2)
#define APLIC_DOMAINCFG_BE		(1 << 0)

#define APLIC_SOURCECFG_BASE		0x0004
#define APLIC_SOURCECFG_D		(1 << 10)
#define APLIC_SOURCECFG_CHILDIDX_MASK	0x000003ff
#define APLIC_SOURCECFG_SM_MASK		0x00000007
#define APLIC_SOURCECFG_SM_INACTIVE	0x0
#define APLIC_SOURCECFG_SM_DETACH	0x1
#define APLIC_SOURCECFG_SM_EDGE_RISE	0x4
#define APLIC_SOURCECFG_SM_EDGE_FALL	0x5
#define APLIC_SOURCECFG_SM_LEVEL_HIGH	0x6
#define APLIC_SOURCECFG_SM_LEVEL_LOW	0x7

#define APLIC_MMSICFGADDR		0x1bc0
#define APLIC_MMSICFGADDRH		0x1bc4
#define APLIC_SMSICFGADDR		0x1bc8
#define APLIC_SMSICFGADDRH		0x1bcc

#define APLIC_xMSICFGADDRH_L		(1UL << 31)
#define APLIC_xMSICFGADDRH_HHXS_MASK	0x1f
#define APLIC_xMSICFGADDRH_HHXS_SHIFT	24
#define APLIC_xMSICFGADDRH_LHXS_MASK	0x7
#define APLIC_xMSICFGADDRH_LHXS_SHIFT	20
#define APLIC_xMSICFGADDRH_HHXW_MASK	0x7
#define APLIC_xMSICFGADDRH_HHXW_SHIFT	16
#define APLIC_xMSICFGADDRH_LHXW_MASK	0xf
#define APLIC_xMSICFGADDRH_LHXW_SHIFT	12
#define APLIC_xMSICFGADDRH_BAPPN_MASK	0xfff

#define APLIC_xMSICFGADDR_PPN_SHIFT	12

#define APLIC_xMSICFGADDR_PPN_HART(__lhxs) \
	((1UL << (__lhxs)) - 1)
#define APLIC_xMSICFGADDR_PPN_LHX_MASK(__lhxw) \
	((1UL << (__lhxw)) - 1)
#define APLIC_xMSICFGADDR_PPN_LHX_SHIFT(__lhxs) \
	((__lhxs))
#define APLIC_xMSICFGADDR_PPN_LHX(__lhxw, __lhxs) \
	(APLIC_xMSICFGADDR_PPN_LHX_MASK(__lhxw) << \
	 APLIC_xMSICFGADDR_PPN_LHX_SHIFT(__lhxs))
#define APLIC_xMSICFGADDR_PPN_HHX_MASK(__hhxw) \
	((1UL << (__hhxw)) - 1)
#define APLIC_xMSICFGADDR_PPN_HHX_SHIFT(__hhxs) \
	((__hhxs) + APLIC_xMSICFGADDR_PPN_SHIFT)
#define APLIC_xMSICFGADDR_PPN_HHX(__hhxw, __hhxs) \
	(APLIC_xMSICFGADDR_PPN_HHX_MASK(__hhxw) << \
	 APLIC_xMSICFGADDR_PPN_HHX_SHIFT(__hhxs))

#define APLIC_SETIP_BASE		0x1c00
#define APLIC_SETIPNUM			0x1cdc

#define APLIC_CLRIP_BASE		0x1d00
#define APLIC_CLRIPNUM			0x1ddc

#define APLIC_SETIE_BASE		0x1e00
#define APLIC_SETIENUM			0x1edc

#define APLIC_CLRIE_BASE		0x1f00
#define APLIC_CLRIENUM			0x1fdc

#define APLIC_SETIPNUM_LE		0x2000
#define APLIC_SETIPNUM_BE		0x2004

#define APLIC_TARGET_BASE		0x3004
#define APLIC_TARGET_HART_IDX_SHIFT	18
#define APLIC_TARGET_HART_IDX_MASK	0x3fff
#define APLIC_TARGET_GUEST_IDX_SHIFT	12
#define APLIC_TARGET_GUEST_IDX_MASK	0x3f
#define APLIC_TARGET_IPRIO_MASK		0xff
#define APLIC_TARGET_EIID_MASK		0x7ff

#define APLIC_IDC_BASE			0x4000
#define APLIC_IDC_SIZE			32

#define APLIC_IDC_IDELIVERY		0x00

#define APLIC_IDC_IFORCE		0x04

#define APLIC_IDC_ITHRESHOLD		0x08

#define APLIC_IDC_TOPI			0x18
#define APLIC_IDC_TOPI_ID_SHIFT		16
#define APLIC_IDC_TOPI_ID_MASK		0x3ff
#define APLIC_IDC_TOPI_PRIO_MASK	0xff

#define APLIC_IDC_CLAIMI		0x1c

#define APLIC_DEFAULT_PRIORITY		1
#define APLIC_DISABLE_IDELIVERY		0
#define APLIC_ENABLE_IDELIVERY		1
#define APLIC_DISABLE_ITHRESHOLD	1
#define APLIC_ENABLE_ITHRESHOLD		0

#ifdef ARCH_HAVE_IRQC
#ifdef CONFIG_APLIC
void irqc_hw_enable_irq(irq_t irq);
void irqc_hw_disable_irq(irq_t irq);
#ifdef CONFIG_PLIC_MASK_PRIORITY
void irqc_hw_mask_irq(irq_t irq);
void irqc_hw_unmask_irq(irq_t irq);
#endif
void irqc_hw_clear_irq(irq_t irq);
void irqc_hw_trigger_irq(irq_t irq);
void irqc_hw_configure_irq(irq_t irq, uint8_t prio, uint8_t trigger);
void irqc_hw_handle_irq(void);
#ifndef CONFIG_APLIC_COMPLETION
void irqc_hw_ack_irq(irq_t irq);
#endif
#ifdef CONFIG_SBI
/* APLIC requires no special initialization other than that is done
 * in SBI.
 */
#define irqc_hw_ctrl_init()	aplic_hw_ctrl_init()
#ifdef CONFIG_SMP
#define irqc_hw_smp_init()	do { } while (0)
#endif /* CONFIG_SMP */
#else /* CONFIG_SBI */
#define irqc_hw_ctrl_init()				\
	do {						\
		aplic_hw_ctrl_init();			\
		aplic_sbi_init_cold();			\
	} while (0)
#ifdef CONFIG_SMP
#define irqc_hw_smp_init()		aplic_sbi_init_warm(smp_processor_id())
#endif /* CONFIG_SMP */
#endif /* CONFIG_SBI */
#ifdef CONFIG_MMU
#define irqc_hw_mmu_init()		aplic_hw_mmu_init()
#endif /* CONFIG_MMU */
#else /* CONFIG_PLIC */
#define irqc_hw_ctrl_init()		do { } while (0)
#define irqc_hw_handle_irq()		do { } while (0)
#define irqc_hw_enable_irq(irq)		do { } while (0)
#define irqc_hw_disable_irq(irq)	do { } while (0)
#ifdef CONFIG_APLIC_MASK_PRIORITY
#define irqc_hw_mask_irq(irq)		do { } while (0)
#define irqc_hw_unmask_irq(irq)		do { } while (0)
#endif
#define irqc_hw_clear_irq(irq)		do { } while (0)
#define irqc_hw_trigger_irq(irq)	do { } while (0)
#define irqc_hw_configure_irq(irq, prio, trigger)	\
					do { } while (0)
#define irqc_hw_handle_irq()		do { } while (0)
#ifdef CONFIG_APLIC_MASK_PRIORITY
#define irqc_hw_ack_irq(irq)		do { } while (0)
#endif
#ifdef CONFIG_SMP
#define irqc_hw_smp_init()		do { } while (0)
#endif /* CONFIG_SMP */
#ifdef CONFIG_MMU
#define irqc_hw_mmu_init()		do { } while (0)
#endif /* CONFIG_SMP */
#endif /* CONFIG_APLIC */
#endif /* ARCH_HAVE_IRQC */

int aplic_cold_irqchip_init(struct aplic_data *aplic);
int aplic_sbi_init_cold(void);

#endif /* __APLIC_RISCV_H_INCLUDE__ */
