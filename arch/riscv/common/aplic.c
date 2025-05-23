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

#ifdef CONFIG_APLIC_WSI
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

void aplic_wsi_init(uint8_t soc)
{
	uint32_t i;

	for (i = 0; i < MAX_HARTS; i++) {
		__raw_writel(APLIC_ITHRES_ALL, APLIC_IDELIVERY(soc, i));
		__raw_writel(0, APLIC_IFORCE(soc, i));
		__raw_writel(APLIC_ITHRES_NONE, APLIC_IDELIVERY(soc, i));
	}
}

#define aplic_msi_deleg_init(soc)	do { } while (0)
#define aplic_msi_init(soc)		do { } while (0)
#else
#define aplic_wsi_init(soc)		do { } while (0)

static void aplic_check_msicfg(void)
{
	BUG_ON(APLIC_LHXS_MASK < APLIC_MSI_LHXS);
	BUG_ON(APLIC_LHXW_MASK < APLIC_MSI_LHXW);
	BUG_ON(APLIC_HHXS_MASK < APLIC_MSI_HHXS);
	BUG_ON(APLIC_HHXW_MASK < APLIC_MSI_HHXW);
}

void aplic_msi_init(uint8_t soc)
{
	if (APLIC_MODE_MSIADDR)
		aplic_config_msiaddr(soc, APLIC_MODE_MSIADDR);
}

void aplic_msi_deleg_init(uint8_t soc)
{
	if (APLIC_HW_MMODE_MSIADDR && !APLIC_IS_MMODE)
		__aplic_config_msiaddr(soc, APLIC_HW_MMODE_MSIADDR, true);
	if (APLIC_HW_SMODE_MSIADDR && APLIC_IS_MMODE)
		__aplic_config_msiaddr(soc, APLIC_HW_SMODE_MSIADDR, false);
}
#endif

#ifdef CONFIG_APLIC_DELEG
#ifndef CONFIG_ARCH_HAS_APLIC_DELEG
/* This is a default delegation for M mode APLIC */
#define aplic_hw_deleg_num		1
static struct aplic_delegate_data aplic_hw_deleg_data[] = {
	{
		.first_irq = 1,
		.last_irq = APLIC_MAX_IRQS,	/* All delegated to S mode APLIC */
		.child_index = 0,
	},
};
#endif

static void aplic_deleg_init(uint8_t soc)
{
	uint32_t i, tmp;
	struct aplic_delegate_data *deleg;
	int first_deleg_irq, last_deleg_irq;
	irq_t irq;

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
	aplic_msi_deleg_init(soc);
}
#else
#define aplic_deleg_init(soc)		do { } while (0)
#endif

static void aplic_irqs_init(void)
{
	irq_t irq;

	for (irq = 1; irq <= APLIC_MAX_IRQS; irq++) {
		aplic_disable_irq(irq);
		__raw_writel(APLIC_SM(APLIC_SM_INACTIVE),
			     APLIC_SOURCECFG(aplic_hw_irq_soc(irq), irq));
		aplic_configure_wsi(irq, BOOT_HART, APLIC_IPRIO_DEF);
	}
}

static void __aplic_ctrl_init(uint8_t soc)
{
	aplic_ctrl_disable(soc);
	aplic_irqs_init();
	aplic_wsi_init(soc);
	aplic_msi_init(soc);
}

void aplic_sbi_init_cold(uint8_t soc)
{
	__aplic_ctrl_init(soc);
	aplic_deleg_init(soc);
}

void aplic_ctrl_init(void)
{
	__aplic_ctrl_init(0);
	aplic_ctrl_enable(0);
}

void irqc_hw_enable_irq(irq_t irq)
{
	irq_t msi;
	cpu_t cpu;

	if (irq >= IRQ_PLATFORM) {
		aplic_enable_irq(irq_ext(irq));
		msi = irq_mapped_msi(irq);
		cpu = irq_mapped_cpu(irq);
		if (msi != INVALID_IRQ) {
			irqc_hw_enable_irq(msi);
			aplic_configure_msi(irq_ext(irq),
					    cpu, 0, irq_msi(msi));
		}
		aplic_trigger_irq(irq_ext(irq));
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
	if (irq >= IRQ_PLATFORM) {
		volatile irq_t airq = irq_ext(irq);
		aplic_configure_irq(airq,
				    prio + APLIC_IPRIO_MIN,
				    trigger);
	}
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
		printf("Dynamic MSI %d on %d\n", irq, cpu);
#endif
		eirq = irq_locate_mapping(cpu, MSI_IRQ(irq));
		if (eirq == IMSIC_NO_IRQ)
			imsic_disable_irq(irq);
		else {
#ifdef CONFIG_RISCV_IRQ_VERBOSE
			printf("External IRQ %d on %d\n",
			       irq_ext(eirq), cpu);
#endif
			if (!do_IRQ(eirq))
				aplic_disable_irq(eirq);
		}
	} else {
		/* Handle INT MSIs */
#ifdef CONFIG_RISCV_IRQ_VERBOSE
		printf("Static MSI %d on %d\n", irq, cpu);
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
	aplic_ctrl_init();
}
#endif
