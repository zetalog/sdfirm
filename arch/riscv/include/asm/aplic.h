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

#ifdef APLIC_HW_MAX_CHIPS
#define APLIC_MAX_CHIPS			APLIC_HW_MAX_CHIPS
#else /* APLIC_HW_MAX_CHIPS */
#define APLIC_MAX_CHIPS			1
#endif /* APLIC_HW_MAX_CHIPS */

#ifdef APLIC_HW_MAX_IRQS
#define APLIC_MAX_IRQS			APLIC_HW_MAX_IRQS
#else /* APLIC_HW_MAX_IRQS */
#define APLIC_MAX_IRQS			1024
#endif /* APLIC_HW_MAX_IRQS */
#define APLIC_NO_IRQ			0

#ifdef APLIC_HW_MAX_DOMAINS
#define APLIC_MAX_DOMAINS		APLIC_HW_MAX_DOMAINS
#else /* APLIC_HW_MAX_DOMAINS */
#define APLIC_MAX_DOMAINS		2
#endif /* APLIC_HW_MAX_DOMAINS */
#ifdef APLIC_HW_MAX_DELEGS
#define APLIC_MAX_DELEGS		APLIC_HW_MAX_DELEGS
#else /* APLIC_HW_MAX_DELEGS */
#define APLIC_MAX_DELEGS		16
#endif /* APLIC_HW_MAX_DELEGS */

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
	struct aplic_delegate_data delegate[APLIC_MAX_DELEGS];
};

#if APLIC_MAX_CHIPS > 1
#define APLIC_REG(soc, offset)		(APLIC_BASE(soc) + (offset))
#define APLIC_1BIT_REG(soc, offset, irq)	\
	REG_1BIT_ADDR(APLIC_BASE(soc) + (offset), irq)
#else /* APLIC_MAX_CHIPS == 1 */
#define APLIC_REG(soc, offset)		(APLIC_BASE + (offset))
#define APLIC_1BIT_REG(soc, offset, irq)	\
	REG_1BIT_ADDR(APLIC_BASE + (offset), irq)
#define aplic_hw_irq_soc(irq)		0
#define aplic_hw_irq_irq(irq)		(irq)
#endif /* APLIC_MAX_CHIPS == 1 */

#define APLIC_MAX_IDC			(1UL << 14)
#define APLIC_MAX_SOURCE		1024

#define APLIC_DOMAINCFG(soc)		APLIC_REG(soc, 0x0000)
#define APLIC_IE			_BV(8)

#ifdef CONFIG_APLIC_WSI
#define APLIC_WSI			(0 << 2)
#else /* CONFIG_APLIC_WSI */
#define APLIC_WSI			APLIC_MSI
#define APLIC_DM			APLIC_MSI
#endif /* CONFIG_APLIC_WSI */

#ifdef CONFIG_APLIC_MSI
#define APLIC_MSI			_BV(2)
#else /* CONFIG_APLIC_MSI */
#define APLIC_MSI			APLIC_WSI
#define APLIC_DM			APLIC_WSI
#endif /* CONFIG_APLIC_MSI */

#ifndef APLIC_DM
#define APLIC_DM			APLIC_WSI
#endif /* APLIC_DM */

#ifdef CONFIG_APLIC_BE
#define APLIC_BE			_BV(0)
#else /* CONFIG_APLIC_BE */
#define APLIC_BE			APLIC_LE
#define APLIC_END			APLIC_LE
#endif /* CONFIG_APLIC_BE */

#ifdef CONFIG_APLIC_LE
#define APLIC_LE			(0 << 0)
#else /* CONFIG_APLIC_LE */
#define APLIC_LE			APLIC_BE
#define APLIC_END			APLIC_BE
#endif /* CONFIG_APLIC_LE */

#ifndef APLIC_END
#define APLIC_END			APLIC_BE
#endif /* APLIC_END */

#define APLIC_SOURCECFG(soc, irq)	APLIC_REG(soc, 0x0004 + (((irq) - 1) << 2))
#define APLIC_D				_BV(10)
#define APLIC_CHILD_INDEX_OFFSET	0
#define APLIC_CHILD_INDEX_MASK		REG_10BIT_MASK
#define APLIC_CHILD_INDEX(value)	_SET_FV(APLIC_CHILD_INDEX, value)
#define APLIC_SM_OFFSET			0
#define APLIC_SM_MASK			REG_3BIT_MASK
#define APLIC_SM(value)			_SET_FV(APLIC_SM, value)
#define APLIC_SM_INACTIVE		0x0
#define APLIC_SM_DETACH			0x1
#define APLIC_SM_EDGE1			0x4
#define APLIC_SM_EDGE0			0x5
#define APLIC_SM_LEVEL1			0x6
#define APLIC_SM_LEVEL0			0x7
#define APLIC_TRIG_TO_SM(trig)			\
	(((trig) == IRQ_LEVEL_TRIGGERED) ?	\
	 APLIC_SM_LEVEL1 : APLIC_SM_EDGE1)

#ifdef CONFIG_APLIC_MSI
#define APLIC_MMSICFGADDR(soc)		APLIC_REG(soc, 0x1BC0)
#define APLIC_MMSICFGADDRH(soc)		APLIC_REG(soc, 0x1BC4)
#define APLIC_SMSICFGADDR(soc)		APLIC_REG(soc, 0x1BC8)
#define APLIC_SMSICFGADDRH(soc)		APLIC_REG(soc, 0x1BCC)
#ifdef CONFIG_RISCV_EXIT_M
#define APLIC_MSICFGADDR(soc)		APLIC_MMSICFGADDR(soc)
#define APLIC_MSICFGADDRH(soc)		APLIC_MMSICFGADDRH(soc)
#endif /* CONFIG_RISCV_EXIT_M */
#ifdef CONFIG_RISCV_EXIT_S
#define APLIC_MSICFGADDR(soc)		APLIC_SMSICFGADDR(soc)
#define APLIC_MSICFGADDRH(soc)		APLIC_SMSICFGADDRH(soc)
#endif /* CONFIG_RISCV_EXIT_S */
#define APLIC_L				_BV(31)
#define APLIC_HHXS_MASK			REG_5BIT_MASK
#define APLIC_HHXS_OFFSET		24
#define APLIC_HHXS(value)		_SET_FV(APLIC_HHXS, value)
#define APLIC_LHXS_MASK			REG_3BIT_MASK
#define APLIC_LHXS_OFFSET		20
#define APLIC_LHXS(value)		_SET_FV(APLIC_LHXS, value)
#define APLIC_HHXW_MASK			REG_3BIT_MASK
#define APLIC_HHXW_OFFSET		16
#define APLIC_HHXW(value)		_SET_FV(APLIC_HHXW, value)
#define APLIC_LHXW_MASK			REG_4BIT_MASK
#define APLIC_LHXW_OFFSET		12
#define APLIC_LHXW(value)		_SET_FV(APLIC_LHXW, value)
#define APLIC_BASE_PPN_H_MASK		REG_12BIT_MASK
#define APLIC_BASE_PPN_H_OFFSET		0
#define APLIC_BASE_PPN_H(ppn_h)		_SET_FV(APLIC_BASE_PPN_H, ppn_h)
#define APLIC_BASE_PPN_SHIFT		12
#define APLIC_ADDR2PFN(addr)		((addr) >> APLIC_BASE_PPN_SHIFT)
#define APLIC_PPN_HART(__lhxs)		(_BV(__lhxs) - 1)
#define APLIC_PPN_LHX_MASK(__lhxw)	(_BV(__lhxw) - 1)
#define APLIC_PPN_LHX_SHIFT(__lhxs)	((__lhxs))
#define APLIC_PPN_LHX(__lhxw, __lhxs)	\
	(APLIC_PPN_LHX_MASK(__lhxw) << APLIC_PPN_LHX_SHIFT(__lhxs))
#define APLIC_PPN_HHX_MASK(__hhxw)	(_BV(__hhxw) - 1)
#define APLIC_PPN_HHX_SHIFT(__hhxs)	((__hhxs) + APLIC_BASE_PPN_SHIFT)
#define APLIC_PPN_HHX(__hhxw, __hhxs)	\
	(APLIC_PPN_HHX_MASK(__hhxw) << APLIC_PPN_HHX_SHIFT(__hhxs))
#endif /* CONFIG_APLIC_MSI */

#define APLIC_IRQ_OFFSET(irq)		REG_1BIT_OFFSET(irq)
#define APLIC_IRQ_MASK			REG_1BIT_MASK
/* Macros used to program PLIC_PENDINGR/PLIC_ENABLER */
#define APLIC_IRQ(irq)			_BV(APLIC_IRQ_OFFSET(irq))
#define APLIC_REG_IRQS			32

#define APLIC_SETIP(soc, irq)		APLIC_1BIT_REG(soc, 0x1C00, irq)
#define APLIC_IN_CLRIP(soc, irq)	APLIC_1BIT_REG(soc, 0x1D00, irq)
#define APLIC_SETIE(soc, irq)		APLIC_1BIT_REG(soc, 0x1E00, irq)
#define APLIC_CLRIE(soc, irq)		APLIC_1BIT_REG(soc, 0x1F00, irq)

#define APLIC_SETIPNUM(soc)		APLIC_REG(soc, 0x1CDC)
#define APLIC_CLRIPNUM(soc)		APLIC_REG(soc, 0x1DDC)
#define APLIC_SETIENUM(soc)		APLIC_REG(soc, 0x1EDC)
#define APLIC_CLRIENUM(soc)		APLIC_REG(soc, 0x1FDC)

#define APLIC_SETIPNUM_LE(soc)		APLIC_REG(soc, 0x2000)
#define APLIC_SETIPNUM_BE(soc)		APLIC_REG(soc, 0x2004)

#define APLIC_GENMSI(soc)		APLIC_REG(soc, 0x3000)
#define APLIC_TARGET(soc, irq)		APLIC_REG(soc, 0x3004 + ((irq) << 2))

#define APLIC_EIID_OFFSET		0
#define APLIC_EIID_MASK			REG_11BIT_MASK
#define APLIC_EIID(value)		_SET_FV(APLIC_EIID, value)
#define APLIC_BUSY			_BV(12)
#define APLIC_HART_INDEX_OFFSET		18
#define APLIC_HART_INDEX_MASK		REG_14BIT_MASK
#define APLIC_HART_INDEX(value)		_SET_FV(APLIC_HART_INDEX, value)

#ifdef CONFIG_APLIC_MSI
#define APLIC_GUEST_INDEX_OFFSET	12
#define APLIC_GUEST_INDEX_MASK		REG_6BIT_MASK
#define APLIC_GUEST_INDEX(value)	_SET_FV(APLIC_GUEST_INDEX, value)
#endif /* CONFIG_APLIC_MSI */

#ifdef CONFIG_APLIC_WSI
#define APLIC_IPRIO_OFFSET		0
#define APLIC_IPRIO_MASK		REG_8BIT_MASK
#define APLIC_IPRIO(value)		_SET_FV(APLIC_IPRIO, value)
#define APLIC_IDC_REG(soc, cpu, offset)	APLIC_REG(soc, 0x4000 + ((cpu) << 5) + (offset))
#define APLIC_IDELIVERY(soc, cpu)	APLIC_IDC_REG(soc, cpu, 0x00)
#define APLIC_IFORCE(soc, cpu)		APLIC_IDC_REG(soc, cpu, 0x04)
#define APLIC_ITHRESHOLD(soc, cpu)	APLIC_IDC_REG(soc, cpu, 0x08)
#define APLIC_TOPI(soc, cpu)		APLIC_IDC_REG(soc, cpu, 0x18)
#define APLIC_IID_OFFSET		16
#define APLIC_IID_MASK			REG_10BIT_MASK
#define APLIC_IID(value)		_SET_FV(APLIC_IID, value)
#define APLIC_IDC_CLAIMI(soc, cpu)	APLIC_IDC_REG(soc, cpu, 0x1C)
#define APLIC_ENABLE_IDELIVERY		1
#define APLIC_DISABLE_IDELIVERY		0
#define APLIC_DISABLE_ITHRESHOLD	1
#define APLIC_ENABLE_ITHRESHOLD		0
#endif /* CONFIG_APLIC_WSI */

#define APLIC_IPRIO_NONE		0
#define APLIC_IPRIO_MIN			1
#ifdef APLIC_HW_IPRIO_MAX
#define APLIC_IPRIO_MAX			APLIC_HW_IPRIO_MAX
#else /* APLIC_HW_PRI_MAX */
#define APLIC_IPRIO_MAX			APLIC_IPRIO_MIN
#endif /* APLIC_HW_PRI_MAX */
#ifdef APLIC_HW_IPRIO_DEF
#define APLIC_IPRIO_DEF			APLIC_HW_IPRIO_DEF
#else /* APLIC_HW_IPRIO_DEF */
#define APLIC_IPRIO_DEF			APLIC_IPRIO_MIN
#endif /* APLIC_HW_IPRIO_DEF */

/* Default priority threshold allows all IRQs */
#define APLIC_ITHRES_ALL		APLIC_IPRIO_NONE
#define APLIC_ITHRES_NONE		APLIC_IPRIO_MAX

#define aplic_ctrl_enable(soc)					\
	__raw_writel(APLIC_DM |	APLIC_END | APLIC_IE,		\
		     APLIC_DOMAINCFG(soc))
#define aplic_ctrl_disable(soc)					\
	__raw_writel(0, APLIC_DOMAINCFG(soc))

#ifdef CONFIG_APLIC_WSI
#define aplic_configure_pri(irq, prio)				\
	__raw_writel_mask(APLIC_IPRIO(prio),			\
			  APLIC_IPRIO(APLIC_IPRIO_MASK),	\
			  APLIC_TARGET(aplic_hw_irq_soc(irq), irq))
#define aplic_configure_wsi(irq, hart, prio)			\
	__raw_writel(APLIC_HART_INDEX(hart) | APLIC_IPRIO(prio),\
		     APLIC_TARGET(aplic_hw_irq_soc(irq), irq))
#define aplic_configure_msi(irq, hart, guest, msi)	\
	do { } while (0)
#else /* CONFIG_APLIC_WSI */
#define aplic_configure_pri(irq, prio)			\
	do { } while (0)
#define aplic_configure_wsi(irq, hart, prio)		\
	do { } while (0)
#define aplic_configure_msi(irq, hart, guest, msi)		\
	__raw_writel(APLIC_HART_INDEX(hart) |			\
		     APLIC_GUEST_INDEX(guest) |			\
		     APLIC_EIID(msi),				\
		     APLIC_TARGET(aplic_hw_irq_soc(irq), irq))
#endif /* CONFIG_APLIC_WSI */
#define aplic_configure_irq(irq, prio, trigger)			\
	do {							\
		__raw_writel(APLIC_TRIG_TO_SM(trigger),		\
			APLIC_SOURCECFG(aplic_hw_irq_soc(irq),	\
					irq));			\
		aplic_configure_wsi(irq, BOOT_HART, prio);	\
	} while (0)
#define aplic_delegate_irq(irq, child)				\
	__raw_writel(APLIC_D | APLIC_CHILD_INDEX(child),	\
		APLIC_SOURCECFG(aplic_hw_irq_soc(irq), irq))
#define aplic_msiaddr_locked(soc)				\
	(__raw_readl(APLIC_MSICFGADDRH(soc)) & APLIC_L)

#define aplic_mask_irq(irq)				\
	aplic_configure_pri(aplic_hw_irq_irq(irq),	\
			    APLIC_IPRIO_NONE)
#define aplic_unmask_irq(irq)				\
	aplic_configure_pri(aplic_hw_irq_irq(irq),	\
			    APLIC_IPRIO_DEF)

#ifdef CONFIG_APLIC_IRQNUM
#define aplic_enable_irq(irq)				\
	__raw_writel(irq, APLIC_SETIENUM(aplic_hw_irq_soc(irq)))
#define aplic_disable_irq(irq)				\
	__raw_writel(irq, APLIC_CLRIENUM(aplic_hw_irq_soc(irq)))
#define aplic_trigger_irq(irq)				\
	__raw_writel(irq, APLIC_SETIPNUM(aplic_hw_irq_soc(irq)))
#define aplic_clear_irq(irq)				\
	__raw_writel(irq, APLIC_CLRIPNUM(aplic_hw_irq_soc(irq)))
#else /* CONFIG_APLIC_IRQNUM */
#define aplic_enable_irq(irq)				\
	__raw_setl(APLIC_IRQ(aplic_hw_irq_irq(irq)),	\
		   APLIC_SETIE(aplic_hw_irq_soc(irq),	\
			       aplic_hw_irq_irq(irq)))
#define aplic_disable_irq(irq)				\
	__raw_setl(APLIC_IRQ(aplic_hw_irq_irq(irq)),	\
		   APLIC_CLRIE(aplic_hw_irq_soc(irq),	\
			       aplic_hw_irq_irq(irq)))
#define aplic_trigger_irq(irq)				\
	__raw_setl(APLIC_IRQ(aplic_hw_irq_irq(irq)),	\
		   APLIC_SETIP(aplic_hw_irq_soc(irq),	\
			       aplic_hw_irq_irq(irq)))
#define aplic_clear_irq(irq)				\
	__raw_setl(APLIC_IRQ(aplic_hw_irq_irq(irq)),	\
		   APLIC_IN_CLRIP(aplic_hw_irq_soc(irq),\
				  aplic_hw_irq_irq(irq)))
#endif /* CONFIG_APLIC_IRQNUM */
#define aplic_disable_all_irqs(soc)					\
	do {								\
		irq_t irq;						\
		for (irq = 0; irq < APLIC_MAX_IRQS;			\
		     irq += APLIC_REG_IRQS)				\
			__raw_writel(0xFFFFFFFF,			\
			  APLIC_CLRIE(soc, aplic_hw_irq_irq(irq)));	\
	} while (0)

#ifdef ARCH_HAVE_IRQC
#ifdef CONFIG_APLIC
void irqc_hw_enable_irq(irq_t irq);
void irqc_hw_disable_irq(irq_t irq);
#ifdef CONFIG_APLIC_WSI
void irqc_hw_mask_irq(irq_t irq);
void irqc_hw_unmask_irq(irq_t irq);
#endif /* CONFIG_APLIC_WSI */
void irqc_hw_clear_irq(irq_t irq);
void irqc_hw_trigger_irq(irq_t irq);
void irqc_hw_configure_irq(irq_t irq, uint8_t prio, uint8_t trigger);
void irqc_hw_handle_irq(void);
#ifndef CONFIG_APLIC_COMPLETION
void irqc_hw_ack_irq(irq_t irq);
#endif /* CONFIG_APLIC_COMPLETION */
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
		aplic_sbi_init_cold(0);			\
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

void aplic_sbi_init_cold(uint8_t soc);

#endif /* __APLIC_RISCV_H_INCLUDE__ */
