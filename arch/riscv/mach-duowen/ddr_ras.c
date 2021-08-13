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
 * @(#)ddr_ras.c: DDR RAS (reli/avail/service-ability) implementation
 * $Id: ddr_ras.c,v 1.1 2020-09-15 06:36:00 zhenglv Exp $
 */

#include <target/ras.h>
#include <target/reg.h>
#ifdef CONFIG_DUOWEN_DDR_RAS
void duowen_ddr0_ecc_ce_handler(irq_t irq)
{
	irqc_mask_irq(IRQ_DDR0_ECC_CE);
       printf("DDR0:Receive ECC UE interrupt\n");
	__raw_writel(DDR0_CTRL_BASE + 0x7c, 0x00010100);
	irqc_unmask_irq(IRQ_DDR0_ECC_CE);
	irqc_ack_irq(IRQ_DDR0_ECC_CE);
}

void duowen_ddr0_ecc_ue_handler(irq_t irq)
{
	irqc_mask_irq(IRQ_DDR0_ECC_UE);
       printf("DDR0:Receive ECC  CE interrupt\n");
	__raw_writel(DDR0_CTRL_BASE + 0x7c, 0x00020200);
	irqc_unmask_irq(IRQ_DDR0_ECC_UE);
	irqc_ack_irq(IRQ_DDR0_ECC_UE);
}

void duowen_ddr0_dfi_alert_handler(irq_t irq)
{
	irqc_mask_irq(IRQ_DDR0_DFI_ALERT_ERR);
       printf("DDR0:Receive DFI ALERT interrupt\n");
	duowen_ddr_handle_irq(IRQ_DDR0_DFI_ALERT_ERR);
	irqc_unmask_irq(IRQ_DDR0_DFI_ALERT_ERR);
	irqc_ack_irq(IRQ_DDR0_DFI_ALERT_ERR);
}

void duowen_ddr0_sbr_done_handler(irq_t irq)
{
	irqc_mask_irq(IRQ_DDR0_SBR_DONE);
       printf("DDR0:Receive SBR DONE interrupt\n");
	irqc_unmask_irq(IRQ_DDR0_SBR_DONE);
	irqc_ack_irq(IRQ_DDR0_SBR_DONE);
}

void duowen_ddr0_awpoison_handler(irq_t irq)
{
	irqc_mask_irq(IRQ_DDR0_AWPOISON_0);
	printf("DDR0:Receive AWPOISON interrupt\n");
	irqc_unmask_irq(IRQ_DDR0_AWPOISON_0);
	irqc_ack_irq(IRQ_DDR0_AWPOISON_0);
}

void duowen_ddr0_arpoison_handler(irq_t irq)
{
	irqc_mask_irq(IRQ_DDR0_ARPOISON_0);
	printf("DDR0:Receive ECC UE interrupt\n");
	irqc_unmask_irq(IRQ_DDR0_ARPOISON_0);
	irqc_ack_irq(IRQ_DDR0_ARPOISON_0);
}

void duowen_ddr1_ecc_ce_handler(irq_t irq)
{
	irqc_mask_irq(IRQ_DDR1_ECC_CE);
       printf("DDR1:Receive ECC UE interrupt\n");
	__raw_writel(DDR1_CTRL_BASE + 0x7c, 0x00010100);
	irqc_unmask_irq(IRQ_DDR1_ECC_CE);
	irqc_ack_irq(IRQ_DDR1_ECC_CE);
}

void duowen_ddr1_ecc_ue_handler(irq_t irq)
{
	irqc_mask_irq(IRQ_DDR1_ECC_UE);
       printf("DDR1:Receive ECC  CE interrupt\n");
	__raw_writel(DDR1_CTRL_BASE + 0x7c, 0x00020200);
	irqc_unmask_irq(IRQ_DDR1_ECC_UE);
	irqc_ack_irq(IRQ_DDR1_ECC_UE);
}

void duowen_ddr1_dfi_alert_handler(irq_t irq)
{
	irqc_mask_irq(IRQ_DDR1_DFI_ALERT_ERR);
	printf("DDR1:Receive DFI ALERT interrupt\n");
	irqc_unmask_irq(IRQ_DDR1_DFI_ALERT_ERR);
	irqc_ack_irq(IRQ_DDR1_DFI_ALERT_ERR);
}

void duowen_ddr1_sbr_done_handler(irq_t irq)
{
	irqc_mask_irq(IRQ_DDR1_SBR_DONE);
       printf("DDR1:Receive SBR DONE interrupt\n");
	irqc_unmask_irq(IRQ_DDR1_SBR_DONE);
	irqc_ack_irq(IRQ_DDR1_SBR_DONE);
}

void duowen_ddr1_awpoison_handler(irq_t irq)
{
	irqc_mask_irq(IRQ_DDR1_AWPOISON_0);
	printf("DDR0:Receive AWPOISON interrupt\n");
	irqc_unmask_irq(IRQ_DDR1_AWPOISON_0);
	irqc_ack_irq(IRQ_DDR1_AWPOISON_0);
}

void duowen_ddr1_arpoison_handler(irq_t irq)
{
	irqc_mask_irq(IRQ_DDR1_ARPOISON_0);
	printf("DDR1:Receive ECC UE interrupt\n");
	irqc_unmask_irq(IRQ_DDR1_ARPOISON_0);
	irqc_ack_irq(IRQ_DDR1_ARPOISON_0);
}

void duowen_ddr_ras_init(void)
{
	uint64_t val;

	irqc_configure_irq(IRQ_DDR0_ECC_CE, 0, IRQ_LEVEL_TRIGGERED);
	irq_register_vector(IRQ_DDR0_ECC_CE, duowen_ddr0_ecc_ce_handler);
	irqc_enable_irq(IRQ_DDR0_ECC_CE);

	irqc_configure_irq(IRQ_DDR0_ECC_CE, 0, IRQ_LEVEL_TRIGGERED);
	irq_register_vector(IRQ_DDR0_ECC_CE, duowen_ddr0_ecc_ue_handler);
	irqc_enable_irq(IRQ_DDR0_ECC_CE);

	irqc_configure_irq(IRQ_DDR0_ECC_CE, 0, IRQ_LEVEL_TRIGGERED);
	irq_register_vector(IRQ_DDR0_ECC_CE, duowen_ddr0_dfi_alert_handler);
	irqc_enable_irq(IRQ_DDR0_ECC_CE);

	irqc_configure_irq(IRQ_DDR0_ECC_CE, 0, IRQ_LEVEL_TRIGGERED);
	irq_register_vector(IRQ_DDR0_ECC_CE, duowen_ddr0_sbr_done_handler);
	irqc_enable_irq(IRQ_DDR0_ECC_CE);

	irqc_configure_irq(IRQ_DDR0_ECC_CE, 0, IRQ_LEVEL_TRIGGERED);
	irq_register_vector(IRQ_DDR0_ECC_CE, duowen_ddr0_awpoison_handler);
	irqc_enable_irq(IRQ_DDR0_ECC_CE);

	irqc_configure_irq(IRQ_DDR0_ECC_CE, 0, IRQ_LEVEL_TRIGGERED);
	irq_register_vector(IRQ_DDR0_ECC_CE, duowen_ddr0_arpoison_handler);
	irqc_enable_irq(IRQ_DDR0_ECC_CE);

	irqc_configure_irq(IRQ_DDR1_ECC_CE, 0, IRQ_LEVEL_TRIGGERED);
	irq_register_vector(IRQ_DDR1_ECC_CE, duowen_ddr1_ecc_ce_handler);
	irqc_enable_irq(IRQ_DDR1_ECC_CE);

	irqc_configure_irq(IRQ_DDR1_ECC_CE, 0, IRQ_LEVEL_TRIGGERED);
	irq_register_vector(IRQ_DDR1_ECC_CE, duowen_ddr1_ecc_ue_handler);
	irqc_enable_irq(IRQ_DDR1_ECC_CE);

	irqc_configure_irq(IRQ_DDR1_ECC_CE, 0, IRQ_LEVEL_TRIGGERED);
	irq_register_vector(IRQ_DDR1_ECC_CE, duowen_ddr1_dfi_alert_handler);
	irqc_enable_irq(IRQ_DDR1_ECC_CE);

	irqc_configure_irq(IRQ_DDR1_ECC_CE, 0, IRQ_LEVEL_TRIGGERED);
	irq_register_vector(IRQ_DDR1_ECC_CE, duowen_ddr1_sbr_done_handler);
	irqc_enable_irq(IRQ_DDR1_ECC_CE);

	irqc_configure_irq(IRQ_DDR1_ECC_CE, 0, IRQ_LEVEL_TRIGGERED);
	irq_register_vector(IRQ_DDR1_ECC_CE, duowen_ddr1_awpoison_handler);
	irqc_enable_irq(IRQ_DDR1_ECC_CE);

	irqc_configure_irq(IRQ_DDR1_ECC_CE, 0, IRQ_LEVEL_TRIGGERED);
	irq_register_vector(IRQ_DDR1_ECC_CE, duowen_ddr1_arpoison_handler);
	irqc_enable_irq(IRQ_DDR1_ECC_CE);
}
#endif
