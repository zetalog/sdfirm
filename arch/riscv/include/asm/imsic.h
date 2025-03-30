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
 * @(#)imsic.h: incoming MSI controller (IMSIC) definitions
 * $Id: imsic.h,v 1.1 2025-3-25 20:43:00 zhenglv Exp $
 */
#ifndef __IMSIC_RISCV_H_INCLUDE__
#define __IMSIC_RISCV_H_INCLUDE__

#include <target/arch.h>

/* Pre-allocated shared interrupts */
#define IMSIC_NO_IRQ			0
#define IMSIC_IPI			1	/* IMSIC IPI ID */
#define IMSIC_NR_SIRQS			8
#define IMSIC_DIRQ_BASE			IMSIC_NR_SIRQS
#define IMSIC_NR_DIRQS			(IMSIC_HW_MAX_IRQS - IMSIC_NR_SIRQS)

#define IMSIC_MMIO_PAGE_SHIFT		12
#define IMSIC_MMIO_PAGE_SZ		(1UL << IMSIC_MMIO_PAGE_SHIFT)
#define IMSIC_MAX_REGS			16

#define IMSIC_MMIO_PAGE_LE		0x00
#define IMSIC_MMIO_PAGE_BE		0x04

#define IMSIC_MIN_ID			63
#define IMSIC_MAX_ID			2047

#define IMSIC_EIDELIVERY		0x70
#define IMSIC_EITHRESHOLD		0x72
#define IMSIC_EIP0			0x80
#define IMSIC_EIP63			0xbf
#define IMSIC_EIE0			0xc0
#define IMSIC_EIE63			0xff

#define IMSIC_TOPEI_ID_SHIFT		16
#define IMSIC_TOPEI_ID_MASK		0x7ff
#define IMSIC_TOPEI_PRIO_MASK		0x7ff

#define IMSIC_EIPx_BITS			32
#define IMSIC_EIEx_BITS			32

/* EIDELIVERY values */
#define IMSIC_OFF			0
#define IMSIC_MSI			1
#define IMSIC_WSI			0x40000000

/* EITHRESHOLD values */
#define IMSIC_NONE			0
#define IMSIC_MIN			1
#define IMSIC_MAX			IMSIC_HW_MAX_IRQS
#define IMSIC_ALL			IMSIC_MIN

#define imsic_claim_irq()				\
	({						\
		unsigned long __topei;			\
		irq_t __irq;				\
		__topei = csr_swap(CSR_TOPEI, 0);	\
	 	__irq = __topei >> TOPEI_ID_SHIFT;	\
		__irq;					\
	})

struct imsic_regs {
	unsigned long addr;
	unsigned long size;
};

struct imsic_data {
	bool targets_mmode;
	uint32_t guest_index_bits;
	uint32_t hart_index_bits;
	uint32_t group_index_bits;
	uint32_t group_index_shift;
	unsigned long num_ids;
	struct imsic_regs regs[IMSIC_MAX_REGS];
};

#define imsic_clear_irq(irq)		__imsic_id_read_clear_pending(irq)
#define imsic_trigger_irq(irq)		__imsic_id_set_pending(irq)
#define imsic_enable_irq(irq)		__imsic_id_set_enabled(irq)
#define imsic_disable_irq(irq)		__imsic_id_clear_enabled(irq)
#define imsic_mask_irq(irq)
#define imsic_unmask_irq(irq)

bool __imsic_id_read_clear_enabled(unsigned long id);
bool __imsic_id_read_clear_pending(unsigned long id);
void __imsic_id_set_enabled(unsigned long id);
void __imsic_id_set_pending(unsigned long id);
void __imsic_id_clear_enabled(unsigned long id);
void __imsic_id_clear_pending(unsigned long id);
void __imsic_eix_update(unsigned long base_id, unsigned long num_id,
			bool pend, bool val);
void imsic_ctrl_init(void);
int imsic_map_hartid_to_data(uint32_t hartid, struct imsic_data *imsic, int file);
struct imsic_data *imsic_get_data(uint32_t hartid);
int imsic_get_target_file(uint32_t hartid);
void imsic_local_irqchip_init(void);
int imsic_warm_irqchip_init(void);
int imsic_data_check(struct imsic_data *imsic);
int imsic_cold_irqchip_init(struct imsic_data *imsic);
int imsic_sbi_init_cold(void);

#endif /* __IMSIC_RISCV_H_INCLUDE__ */
