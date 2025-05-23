/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 * Copyright (c) 2022 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#include <asm/aplic.h>
#include <asm/io.h>
#include <target/smp.h>
#include <target/irq.h>
#include <target/sbi.h>

#define APLIC_MAX_IDC                   (1UL << 14)
#define APLIC_MAX_SOURCE                1024

#define __APLIC_DOMAINCFG         0x0000
#define APLIC_DOMAINCFG_IE              (1 << 8)
#define APLIC_DOMAINCFG_DM              (1 << 2)
#define APLIC_DOMAINCFG_BE              (1 << 0)

#define __APLIC_SOURCECFG_BASE            0x0004
#define APLIC_SOURCECFG_D               (1 << 10)
#define APLIC_SOURCECFG_CHILDIDX_MASK   0x000003ff
#define APLIC_SOURCECFG_SM_MASK 0x00000007
#define APLIC_SOURCECFG_SM_INACTIVE     0x0
#define APLIC_SOURCECFG_SM_DETACH       0x1
#define APLIC_SOURCECFG_SM_EDGE_RISE    0x4
#define APLIC_SOURCECFG_SM_EDGE_FALL    0x5
#define APLIC_SOURCECFG_SM_LEVEL_HIGH   0x6
#define APLIC_SOURCECFG_SM_LEVEL_LOW    0x7

#define __APLIC_MMSICFGADDR		0x1bc0
#define __APLIC_MMSICFGADDRH		0x1bc4
#define __APLIC_SMSICFGADDR		0x1bc8
#define __APLIC_SMSICFGADDRH		0x1bcc

#define APLIC_xMSICFGADDRH_L            (1UL << 31)
#define APLIC_xMSICFGADDRH_HHXS_MASK    0x1f
#define APLIC_xMSICFGADDRH_HHXS_SHIFT   24
#define APLIC_xMSICFGADDRH_LHXS_MASK    0x7
#define APLIC_xMSICFGADDRH_LHXS_SHIFT   20
#define APLIC_xMSICFGADDRH_HHXW_MASK    0x7
#define APLIC_xMSICFGADDRH_HHXW_SHIFT   16
#define APLIC_xMSICFGADDRH_LHXW_MASK    0xf
#define APLIC_xMSICFGADDRH_LHXW_SHIFT   12
#define APLIC_xMSICFGADDRH_BAPPN_MASK   0xfff

#define APLIC_xMSICFGADDR_PPN_SHIFT     12

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

#define __APLIC_SETIP_BASE		0x1c00
#define __APLIC_SETIPNUM		0x1cdc

#define __APLIC_CLRIP_BASE		0x1d00
#define __APLIC_CLRIPNUM		0x1ddc

#define __APLIC_SETIE_BASE		0x1e00
#define __APLIC_SETIENUM		0x1edc

#define __APLIC_CLRIE_BASE		0x1f00
#define __APLIC_CLRIENUM		0x1fdc

#define __APLIC_SETIPNUM_LE		0x2000
#define __APLIC_SETIPNUM_BE		0x2004

#define __APLIC_TARGET_BASE		0x3004
#define APLIC_TARGET_HART_IDX_SHIFT     18
#define APLIC_TARGET_HART_IDX_MASK      0x3fff
#define APLIC_TARGET_GUEST_IDX_SHIFT    12
#define APLIC_TARGET_GUEST_IDX_MASK     0x3f
#define APLIC_TARGET_IPRIO_MASK 0xff
#define APLIC_TARGET_EIID_MASK  0x7ff

#define APLIC_IDC_BASE                  0x4000
#define APLIC_IDC_SIZE                  32

#define APLIC_IDC_IDELIVERY             0x00

#define APLIC_IDC_IFORCE                0x04

#define APLIC_IDC_ITHRESHOLD            0x08

#define APLIC_IDC_TOPI                  0x18
#define APLIC_IDC_TOPI_ID_SHIFT 16
#define APLIC_IDC_TOPI_ID_MASK  0x3ff
#define APLIC_IDC_TOPI_PRIO_MASK        0xff

#define APLIC_IDC_CLAIMI                0x1c

#define APLIC_DEFAULT_PRIORITY          1
#define APLIC_DISABLE_IDELIVERY         0
#define APLIC_ENABLE_IDELIVERY          1
#define APLIC_DISABLE_ITHRESHOLD        1
#define APLIC_ENABLE_ITHRESHOLD         0

static void aplic_writel_msicfg(struct aplic_msicfg_data *msicfg,
                                void *msicfgaddr, void *msicfgaddrH)
{
        uint32_t val;
        unsigned long base_ppn;

        /* Check if MSI config is already locked */
        if (__raw_readl((caddr_t)msicfgaddrH) & APLIC_xMSICFGADDRH_L)
                return;

        /* Compute the MSI base PPN */
        //sbi_printf("aplic.c:msicfg->base_addr:%llx\n", msicfg->base_addr);
        base_ppn = msicfg->base_addr >> APLIC_xMSICFGADDR_PPN_SHIFT;
        base_ppn &= ~APLIC_xMSICFGADDR_PPN_HART(msicfg->lhxs);
        base_ppn &= ~APLIC_xMSICFGADDR_PPN_LHX(msicfg->lhxw, msicfg->lhxs);
        base_ppn &= ~APLIC_xMSICFGADDR_PPN_HHX(msicfg->hhxw, msicfg->hhxs);

        /* Write the lower MSI config register */
        __raw_writel((uint32_t)base_ppn, (caddr_t)msicfgaddr);

        /* Write the upper MSI config register */
        val = (((uint64_t)base_ppn) >> 32) &
                APLIC_xMSICFGADDRH_BAPPN_MASK;
        val |= (msicfg->lhxw & APLIC_xMSICFGADDRH_LHXW_MASK)
                << APLIC_xMSICFGADDRH_LHXW_SHIFT;
        val |= (msicfg->hhxw & APLIC_xMSICFGADDRH_HHXW_MASK)
                << APLIC_xMSICFGADDRH_HHXW_SHIFT;
        val |= (msicfg->lhxs & APLIC_xMSICFGADDRH_LHXS_MASK)
                << APLIC_xMSICFGADDRH_LHXS_SHIFT;
        val |= (msicfg->hhxs & APLIC_xMSICFGADDRH_HHXS_MASK)
                << APLIC_xMSICFGADDRH_HHXS_SHIFT;
        __raw_writel(val, (caddr_t)msicfgaddrH);
}

static int aplic_check_msicfg(struct aplic_msicfg_data *msicfg)
{
        if (APLIC_xMSICFGADDRH_LHXS_MASK < msicfg->lhxs)
                return SBI_EINVAL;

        if (APLIC_xMSICFGADDRH_LHXW_MASK < msicfg->lhxw)
                return SBI_EINVAL;

        if (APLIC_xMSICFGADDRH_HHXS_MASK < msicfg->hhxs)
                return SBI_EINVAL;

        if (APLIC_xMSICFGADDRH_HHXW_MASK < msicfg->hhxw)
                return SBI_EINVAL;

        return 0;
}

int aplic_cold_irqchip_init(struct aplic_data *aplic)
{
        int rc;
        uint32_t i, j, tmp;
//      struct sbi_domain_memregion reg;
        struct aplic_delegate_data *deleg;
        uint32_t first_deleg_irq, last_deleg_irq;

        //sbi_printf("aplic.c:num_source:%ld, num_idc:%ld, addr:%llx\n",
        //        aplic->num_source, aplic->num_idc, aplic->addr);
        /* Sanity checks */
        if (!aplic ||
            !aplic->num_source || APLIC_MAX_SOURCE <= aplic->num_source ||
            APLIC_MAX_IDC <= aplic->num_idc)
                return SBI_EINVAL;
        if (aplic->targets_mmode && aplic->has_msicfg_mmode) {
                rc = aplic_check_msicfg(&aplic->msicfg_mmode);
                if (rc)
                        return rc;
        }
        if (aplic->targets_mmode && aplic->has_msicfg_smode) {
                rc = aplic_check_msicfg(&aplic->msicfg_smode);
                if (rc)
                        return rc;
        }

        /* Set domain configuration to 0 */
        __raw_writel(0, (caddr_t)(aplic->addr + __APLIC_DOMAINCFG));

        /* Disable all interrupts */
	for (i = 0; i <= aplic->num_source; i += 32)
		__raw_writel(-1U, (caddr_t)(aplic->addr + __APLIC_CLRIE_BASE +
			     (i / 32) * sizeof(uint32_t)));

        /* Set interrupt type and priority for all interrupts */
	for (i = 1; i <= aplic->num_source; i++) {
		/* Set IRQ source configuration to 0 */
		__raw_writel(0,
			     (caddr_t)(aplic->addr + __APLIC_SOURCECFG_BASE +
			     (i - 1) * sizeof(uint32_t)));
		/* Set IRQ target hart index and priority to 1 */
		__raw_writel(APLIC_DEFAULT_PRIORITY, (caddr_t)(aplic->addr +
			     __APLIC_TARGET_BASE + (i - 1) * sizeof(uint32_t)));
        }

        /* Configure IRQ delegation */
        first_deleg_irq = -1U;
        last_deleg_irq = 0;
        for (i = 0; i < APLIC_MAX_DELEGS; i++) {
                //sbi_printf("aplic.c: delegate[%d], first_irq=%d, last_irq=%d, aplic->addr=%lx\n",
                //        i, deleg->first_irq, deleg->last_irq, aplic->addr);
                deleg = &aplic->delegate[i];
                if (!deleg->first_irq || !deleg->last_irq)
                        continue;
                if (aplic->num_source < deleg->first_irq ||
                    aplic->num_source < deleg->last_irq)
                        continue;
                if (APLIC_SOURCECFG_CHILDIDX_MASK < deleg->child_index)
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
                for (j = deleg->first_irq; j <= deleg->last_irq; j++)
                        __raw_writel(APLIC_SOURCECFG_D | deleg->child_index,
                                (caddr_t)(aplic->addr + __APLIC_SOURCECFG_BASE +
                                (j - 1) * sizeof(uint32_t)));
        }

        /* Default initialization of IDC structures */
        for (i = 0; i < aplic->num_idc; i++) {
                __raw_writel(0, (caddr_t)(aplic->addr + APLIC_IDC_BASE +
                          i * APLIC_IDC_SIZE + APLIC_IDC_IDELIVERY));
                __raw_writel(0, (caddr_t)(aplic->addr + APLIC_IDC_BASE +
                          i * APLIC_IDC_SIZE + APLIC_IDC_IFORCE));
                __raw_writel(APLIC_DISABLE_ITHRESHOLD, (caddr_t)(aplic->addr +
                                                  APLIC_IDC_BASE +
                                                  (i * APLIC_IDC_SIZE) +
                                                  APLIC_IDC_ITHRESHOLD));
        }

        /* MSI configuration */
        if (aplic->targets_mmode && aplic->has_msicfg_mmode) {
                aplic_writel_msicfg(&aplic->msicfg_mmode,
                                (void *)(aplic->addr + __APLIC_MMSICFGADDR),
                                (void *)(aplic->addr + __APLIC_MMSICFGADDRH));
        }
        if (aplic->targets_mmode && aplic->has_msicfg_smode) {
                aplic_writel_msicfg(&aplic->msicfg_smode,
                                (void *)(aplic->addr + __APLIC_SMSICFGADDR),
                                (void *)(aplic->addr + __APLIC_SMSICFGADDRH));
        }

//      /*
//       * Add APLIC region to the root domain if:
//       * 1) It targets M-mode of any HART directly or via MSIs
//       * 2) All interrupts are delegated to some child APLIC
//       */
//      if (aplic->targets_mmode ||
//          ((first_deleg_irq < last_deleg_irq) &&
//          (last_deleg_irq == aplic->num_source) &&
//          (first_deleg_irq == 1))) {
//              sbi_domain_memregion_init(aplic->addr, aplic->size,
//                                        (SBI_DOMAIN_MEMREGION_MMIO |
//                                         SBI_DOMAIN_MEMREGION_M_READABLE |
//                                         SBI_DOMAIN_MEMREGION_M_WRITABLE),
//                                        &reg);
//              rc = sbi_domain_root_add_memregion(&reg);
//              if (rc)
//                      return rc;
//      }

        return 0;
}
