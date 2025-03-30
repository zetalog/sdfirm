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
 * @(#)aplic.c: advanced platform level interrupt controller (APLIC) implementation
 * $Id: aplic.c,v 1.1 2023-3-11 15:43:00 zhenglv Exp $
 */
#include <target/irq.h>
#include <target/smp.h>
#include <target/sbi.h>
#include <target/panic.h>
#include <target/msi.h>

#ifndef CONFIG_ARCH_HAS_APLIC_DELEG
/* This is a default delegation for M mode APLIC */
#define aplic_hw_deleg_num		1
static struct aplic_delegate_data aplic_hw_deleg_data[] = {
	{
		.first_irq = 0,
		.last_irq = APLIC_MAX_IRQS,	/* All delegated to S mode APLIC */
		.child_index = APLIC_HW_SMODE_CHILD,
	},
};
#endif

#ifdef CONFIG_APLIC_MSI
static void aplic_writel_msicfg(uint8_t soc, unsigned long base_addr)
{
	unsigned long base_ppn;

	if (aplic_msiaddr_locked(soc))
		return;

	base_ppn = APLIC_ADDR2PFN(base_addr);
	base_ppn &= ~APLIC_PPN_HART(APLIC_HW_MSI_LHXS);
	base_ppn &= ~APLIC_PPN_LHX(APLIC_HW_MSI_LHXW, APLIC_HW_MSI_LHXS);
	base_ppn &= ~APLIC_PPN_HHX(APLIC_HW_MSI_HHXW, APLIC_HW_MSI_HHXS);
	__raw_writel(LODWORD(base_ppn), APLIC_MSICFGADDR(soc));
	__raw_writel(APLIC_BASE_PPN_H(HIDWORD(base_ppn)) |
		     APLIC_LHXW(APLIC_HW_MSI_LHXW) |
		     APLIC_HHXW(APLIC_HW_MSI_HHXW) |
		     APLIC_LHXS(APLIC_HW_MSI_LHXS) |
		     APLIC_HHXS(APLIC_HW_MSI_HHXS),
		     APLIC_MSICFGADDRH(soc));
}

static void aplic_check_msicfg(void)
{
	BUG_ON(APLIC_LHXS_MASK < APLIC_HW_MSI_LHXS);
	BUG_ON(APLIC_LHXW_MASK < APLIC_HW_MSI_LHXW);
	BUG_ON(APLIC_HHXS_MASK < APLIC_HW_MSI_HHXS);
	BUG_ON(APLIC_HHXW_MASK < APLIC_HW_MSI_HHXW);
}
#else
#define aplic_writel_msicfg(cfg, addr)	do { } while (0)
#define aplic_check_msicfg(cfg)		do { } while (0)

void irqc_hw_mask_irq(irq_t irq)
{
	irq_t msi;

	if (irq >= IRQ_PLATFORM) {
		aplic_mask_irq(irq_ext(irq));
		msi = irq_mapped_msi(irq);
		if (msi != INVALID_IRQ)
			irqc_hw_mask_irq(msi);
	} else if (irq >= NR_INT_IRQS)
		imsic_mask_irq(irq_msi(irq));
	else
		aplic_hw_disable_int(irq);
}

void irqc_hw_unmask_irq(irq_t irq)
{
	irq_t msi;

	if (irq >= IRQ_PLATFORM) {
		aplic_unmask_irq(irq_ext(irq));
		msi = irq_mapped_msi(irq);
		if (msi != INVALID_IRQ)
			irqc_hw_mask_irq(msi);
	} else if (irq >= NR_INT_IRQS)
		imsic_unmask_irq(irq_msi(irq));
	else
		aplic_hw_enable_int(irq);
}
#endif

void aplic_sbi_init(uint8_t soc)
{
	uint32_t i, tmp;
	struct aplic_delegate_data *deleg;
	uint32_t first_deleg_irq, last_deleg_irq;
	irq_t irq;

	aplic_ctrl_disable(soc);
	aplic_disable_all_irqs(soc);

	for (irq = 1; irq <= APLIC_MAX_IRQS; irq++) {
		__raw_writel(APLIC_SM(APLIC_SM_INACTIVE),
			     APLIC_SOURCECFG(soc, irq));
		aplic_configure_wsi(irq, BOOT_HART, APLIC_IPRIO_DEF);
	}

	aplic_check_msicfg();

	/* Configure IRQ delegation */
	first_deleg_irq = -1U;
	last_deleg_irq = 0;
	for (i = 0; i < aplic_hw_deleg_num; i++) {
		deleg = &aplic_hw_deleg_data[i];
		if (!deleg->first_irq || !deleg->last_irq)
			continue;
		if (APLIC_MAX_IRQS < deleg->first_irq ||
		    APLIC_MAX_IRQS < deleg->last_irq)
			continue;
		if (APLIC_CHILD_INDEX_MASK < deleg->child_index)
			continue;
		if (deleg->first_irq > deleg->last_irq) {
			tmp = deleg->first_irq;
			deleg->first_irq = deleg->last_irq;
			deleg->last_irq = tmp;
		}
		if (deleg->first_irq < first_deleg_irq)
			first_deleg_irq = deleg->first_irq;
		if (last_deleg_irq < deleg->last_irq)
			last_deleg_irq = deleg->last_irq;
		for (irq = deleg->first_irq; irq <= deleg->last_irq; irq++)
			aplic_delegate_irq(irq, deleg->child_index);
	}

#ifdef CONFIG_APLIC_WSI
	/* Default initialization of IDC structures */
	for (i = 0; i < MAX_HARTS; i++) {
		__raw_writel(APLIC_ITHRES_ALL, APLIC_IDELIVERY(soc, i));
		__raw_writel(0, APLIC_IFORCE(soc, i));
		__raw_writel(APLIC_ITHRES_NONE, APLIC_IDELIVERY(soc, i));
	}
#endif

	/* MSI configuration */
	if (APLIC_HW_MMODE_MSIADDR)
		aplic_writel_msicfg(soc, APLIC_HW_MMODE_MSIADDR);
	if (APLIC_HW_SMODE_MSIADDR)
		aplic_writel_msicfg(soc, APLIC_HW_SMODE_MSIADDR);
}

void irqc_hw_enable_irq(irq_t irq)
{
	irq_t msi;

	if (irq >= IRQ_PLATFORM) {
		aplic_enable_irq(irq_ext(irq));
		msi = irq_mapped_msi(irq);
		if (msi != INVALID_IRQ)
			irqc_hw_enable_irq(msi);
	} else if (irq >= NR_INT_IRQS)
		imsic_enable_irq(irq_msi(irq));
	else
		aplic_hw_enable_int(irq);
}

void irqc_hw_disable_irq(irq_t irq)
{
	irq_t msi;

	if (irq >= IRQ_PLATFORM) {
		aplic_disable_irq(irq_ext(irq));
		msi = irq_mapped_msi(irq);
		if (msi != INVALID_IRQ)
			irqc_hw_enable_irq(msi);
	} else if (irq >= NR_INT_IRQS)
		imsic_enable_irq(irq_msi(irq));
	else
		aplic_hw_disable_int(irq);
}

void irqc_hw_clear_irq(irq_t irq)
{
	if (irq >= IRQ_PLATFORM)
		aplic_clear_irq(irq_ext(irq));
	else if (irq >= NR_INT_IRQS)
		imsic_clear_irq(irq_msi(irq));
	else
		aplic_hw_clear_int(irq);
}

void irqc_hw_trigger_irq(irq_t irq)
{
	if (irq >= IRQ_PLATFORM)
		aplic_trigger_irq(irq_ext(irq));
	else if (irq >= NR_INT_IRQS)
		imsic_trigger_irq(irq_msi(irq));
	else
		aplic_hw_trigger_int(irq);
}

void irqc_hw_configure_irq(irq_t irq, uint8_t prio, uint8_t trigger)
{
	if (irq >= IRQ_PLATFORM)
		aplic_configure_pri(irq_ext(irq),
				    prio + APLIC_IPRIO_MIN);
}

#ifdef CONFIG_APLIC_WSI
void irqc_hw_handle_irq(void)
{
	irq_t irq;
	__unused uint8_t cpu = smp_processor_id();

	aplic_hw_disable_int(IRQ_EXT);
	irq = aplic_claim_irq(cpu);
	if (irq >= NR_EXT_IRQS) {
		/* Invalid IRQ */
		aplic_disable_irq(irq);
	} else {
#ifdef CONFIG_RISCV_IRQ_VERBOSE
		printf("External IRQ %d\n", irq);
#endif
		if (!do_IRQ(EXT_IRQ(irq)))
			/* No IRQ handler registered, disabling... */
			aplic_disable_irq(irq);
	}
	aplic_hw_enable_int(IRQ_EXT);
}
#endif

#ifdef CONFIG_APLIC_MSI
void irqc_hw_handle_irq(void)
{
	irq_t irq;
	irq_t eirq;
	__unused uint8_t cpu = smp_processor_id();

	imsic_hw_disable_int(IRQ_EXT);
	irq = imsic_claim_irq();
	if (irq >= IMSIC_NR_SIRQS) {
		/* Handle EXT MSIs */
#ifdef CONFIG_RISCV_IRQ_VERBOSE
		printf("Dynamic MSI %d\n", irq);
#endif
		eirq = irq_locate_mapping(cpu, irq);
		if (eirq == IMSIC_NO_IRQ)
			imsic_disable_irq(irq);
		else {
#ifdef CONFIG_RISCV_IRQ_VERBOSE
			printf("External IRQ %d\n", eirq);
#endif
			if (!do_IRQ(EXT_IRQ(eirq)))
				aplic_disable_irq(eirq);
		}
	} else {
		/* Handle INT MSIs */
#ifdef CONFIG_RISCV_IRQ_VERBOSE
		printf("Static MSI %d\n", irq);
#endif
		if (!do_IRQ(MSI_IRQ(irq)))
			/* No IRQ handler registered, disabling... */
			imsic_disable_irq(irq);
	}
	imsic_hw_enable_int(IRQ_EXT);
}

void msi_hw_ctrl_init(void)
{
	imsic_ctrl_init();
}
#endif
