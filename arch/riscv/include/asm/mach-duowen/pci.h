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
 * @(#)pci.h: DPU specific PCI controller definitions
 * $Id: pci.h,v 1.1 2020-03-23 14:52:00 zhenglv Exp $
 */

#ifndef __PCI_DUOWEM_H_INCLUDE__
#define __PCI_DUOWEN_H_INCLUDE__

#include <target/arch.h>

#ifdef CONFIG_DW_PCIE
#ifndef ARCH_HAVE_PCI
#define ARCH_HAVE_PCI		1
/* PCIe driver sdfirm/dpu instantiation */
#define SDFIRM			1
#else
#error "Multiple PCI controller defined"
#endif
#endif

#ifdef SDFIRM
#include <driver/pcie_designware.h>
#else
#include "pcie_designware.h"
#endif

#define X16				0
#define X8				1
#define X4_0				2
#define X4_1				3
#define SUBSYS				4

#define DUOWEN_PCIE_LINK_MODE_0		0
#define DUOWEN_PCIE_LINK_MODE_4		1
#define DUOWEN_PCIE_LINK_MODE_8		2
#define DUOWEN_PCIE_LINK_MODE_16	3
#define DUOWEN_PCIE_LINK_LANE(mode)	(_BV((mode) - 1) << 2)
#ifdef CONFIG_DUOWEN_PCIE_LINK_MODE_0
#define DUOWEN_PCIE_LINK_MODE_DEFAULT	DUOWEN_PCIE_LINK_MODE_4_4_4_4
#define DUOWEN_PCIE_LINK_MODE_CHIPLINK	DUOWEN_PCIE_LINK_MODE_4_4_4_4
#endif
#ifdef CONFIG_DUOWEN_PCIE_LINK_MODE_1
#define DUOWEN_PCIE_LINK_MODE_DEFAULT	DUOWEN_PCIE_LINK_MODE_8_4_0_4
#define DUOWEN_PCIE_LINK_MODE_CHIPLINK	DUOWEN_PCIE_LINK_MODE_8_4_0_4
#endif
#ifdef CONFIG_DUOWEN_PCIE_LINK_MODE_2
#define DUOWEN_PCIE_LINK_MODE_DEFAULT	DUOWEN_PCIE_LINK_MODE_8_8_0_0
#define DUOWEN_PCIE_LINK_MODE_CHIPLINK	DUOWEN_PCIE_LINK_MODE_8_4_0_4
#endif
#ifdef CONFIG_DUOWEN_PCIE_LINK_MODE_3
#define DUOWEN_PCIE_LINK_MODE_DEFAULT	DUOWEN_PCIE_LINK_MODE_16_0_0_0
#define DUOWEN_PCIE_LINK_MODE_CHIPLINK	DUOWEN_PCIE_LINK_MODE_8_4_0_4
#endif

#define DUOWEN_PCIE_LINK_CTRL_OFFSET(n)	REG_2BIT_OFFSET(n)
#define DUOWEN_PCIE_LINK_CTRL_MASK	REG_2BIT_MASK
#define DUOWEN_PCIE_LINK_CTRL(n, value)	\
	_SET_FVn(n, DUOWEN_PCIE_LINK_CTRL, value)
#define rom_pcie_link_ctrl(n, value)	\
	_GET_FVn(n, DUOWEN_PCIE_LINK_CTRL, value)
#define DUOWEN_PCIE_LINK_MODE_16_0_0_0				\
	(DUOWEN_PCIE_LINK_CTRL(0, DUOWEN_PCIE_LINK_MODE_16) |	\
	 DUOWEN_PCIE_LINK_CTRL(1, DUOWEN_PCIE_LINK_MODE_0) |	\
	 DUOWEN_PCIE_LINK_CTRL(2, DUOWEN_PCIE_LINK_MODE_0) |	\
	 DUOWEN_PCIE_LINK_CTRL(3, DUOWEN_PCIE_LINK_MODE_0))
#define DUOWEN_PCIE_LINK_MODE_8_8_0_0				\
	(DUOWEN_PCIE_LINK_CTRL(0, DUOWEN_PCIE_LINK_MODE_8) |	\
	 DUOWEN_PCIE_LINK_CTRL(1, DUOWEN_PCIE_LINK_MODE_8) |	\
	 DUOWEN_PCIE_LINK_CTRL(2, DUOWEN_PCIE_LINK_MODE_0) |	\
	 DUOWEN_PCIE_LINK_CTRL(3, DUOWEN_PCIE_LINK_MODE_0))
#define DUOWEN_PCIE_LINK_MODE_8_4_0_4				\
	(DUOWEN_PCIE_LINK_CTRL(0, DUOWEN_PCIE_LINK_MODE_8) |	\
	 DUOWEN_PCIE_LINK_CTRL(1, DUOWEN_PCIE_LINK_MODE_4) |	\
	 DUOWEN_PCIE_LINK_CTRL(2, DUOWEN_PCIE_LINK_MODE_0) |	\
	 DUOWEN_PCIE_LINK_CTRL(3, DUOWEN_PCIE_LINK_MODE_4))
#define DUOWEN_PCIE_LINK_MODE_4_4_4_4				\
	(DUOWEN_PCIE_LINK_CTRL(0, DUOWEN_PCIE_LINK_MODE_4) |	\
	 DUOWEN_PCIE_LINK_CTRL(1, DUOWEN_PCIE_LINK_MODE_4) |	\
	 DUOWEN_PCIE_LINK_CTRL(2, DUOWEN_PCIE_LINK_MODE_4) |	\
	 DUOWEN_PCIE_LINK_CTRL(3, DUOWEN_PCIE_LINK_MODE_4))
#define DUOWEN_PCIE_LINK_MODE_INVALID				\
	(DUOWEN_PCIE_LINK_CTRL(0, DUOWEN_PCIE_LINK_MODE_0) |	\
	 DUOWEN_PCIE_LINK_CTRL(1, DUOWEN_PCIE_LINK_MODE_0) |	\
	 DUOWEN_PCIE_LINK_CTRL(2, DUOWEN_PCIE_LINK_MODE_0) |	\
	 DUOWEN_PCIE_LINK_CTRL(3, DUOWEN_PCIE_LINK_MODE_0))

#define APB_PORT_X16			0x4
#define APB_PORT_X8			0x5
#define APB_PORT_X4_0			0x6
#define APB_PORT_X4_1			0x7
#define APB_PORT_SUBSYS			0x8

#define AXI_DBI_PORT_X16		0x0
#define AXI_DBI_PORT_X8			0x1
#define AXI_DBI_PORT_X4_0		0x2
#define AXI_DBI_PORT_X4_1		0x3

#define RESET_CORE_X16			0x0
#define RESET_CORE_X8			0x4
#define RESET_CORE_X4_0			0x8
#define RESET_CORE_X4_1			0xC
#define RESET_PHY			0x10
#define SUBSYS_CONTROL			0x14
#define REFCLK_CONTROL			0x18
#define SRAM_CONTROL			0x1c
#define SRAM_STATUS			0x20

#define LINK_MODE_INVALID		0xff

#ifdef IPBENCH
#define CFG_APB_SUBSYS			0x0
#define CFG_APB_CORE_X16		0x0
#define CFG_APB_CORE_X8			0x0
#define CFG_APB_CORE_X4_0		0x0
#define CFG_APB_CORE_X4_1		0x0

#define CFG_APB_PHY_0			0x0
#define CFG_APB_PHY_1			0x0
#define CFG_APB_PHY_2			0x0
#define CFG_APB_PHY_3			0x0

#define CFG_AXI_CORE_X16		0x0
#define CFG_AXI_CORE_X8			0x0
#define CFG_AXI_CORE_X4_0		0x0
#define CFG_AXI_CORE_X4_1		0x0
#else
#define CFG_APB_SUBSYS			ULL(0xff09000000)
#define CFG_APB_CORE_X16		ULL(0xff09001000)
#define CFG_APB_CORE_X8			ULL(0xff09002000)
#define CFG_APB_CORE_X4_0		ULL(0xff09003000)
#define CFG_APB_CORE_X4_1		ULL(0xff09004000)

#define CFG_APB_PHY_0			ULL(0xff09180000)
#define CFG_APB_PHY_1			ULL(0xff09280000)
#define CFG_APB_PHY_2			ULL(0xff09380000)
#define CFG_APB_PHY_3			ULL(0xff09480000)

#define CFG_AXI_CORE_X16		ULL(0xff09100000)
#define CFG_AXI_CORE_X8			ULL(0xff09200000)
#define CFG_AXI_CORE_X4_0		ULL(0xff09300000)
#define CFG_AXI_CORE_X4_1		ULL(0xff09400000)
#endif

#define KB				(1UL << 10)
#define MB				(1UL << 20)
#define GB				(1UL << 30)

#define PCIE_CORE_RANGE			512*GB
#define PCIE_SUBSYS_ADDR_START		ULL(0x40000000000)
#define PCIE_CORE_X16_ADDR_START	0
#define PCIE_CORE_X8_ADDR_START		\
	(PCIE_CORE_X16_ADDR_START + PCIE_CORE_RANGE)
#define PCIE_CORE_X4_0_ADDR_START	\
	(PCIE_CORE_X8_ADDR_START + PCIE_CORE_RANGE)
#define PCIE_CORE_X4_1_ADDR_START	\
	(PCIE_CORE_X4_0_ADDR_START + PCIE_CORE_RANGE)
#define PCIE_SUBSYS_ADDR_END		\
	(PCIE_CORE_X4_1_ADDR_START + PCIE_CORE_RANGE)

#define PCIE_CORE_CFG_SIZE		256*MB
#define PCIE_CORE_MEM_SIZE		\
	(PCIE_CORE_RANGE - 2*PCIE_CORE_CFG_SIZE)
#define PCIE_CORE_CFG1_START		\
	(PCIE_CORE_RANGE - PCIE_CORE_CFG_SIZE)
#define PCIE_CORE_CFG0_START		\
	(PCIE_CORE_RANGE - 2*PCIE_CORE_CFG_SIZE)

#ifdef CONFIG_DUOWEN_PCIE_TEST
#define MSI_INT				0
#define INTA_INT			1
#endif

#ifdef IPBENCH
void apb_read_c(uint32_t addr, uint32_t *data, int port);
void apb_write_c(uint32_t addr, uint32_t data, int port);
#endif

struct duowen_pcie {
	uint64_t cfg_apb[5];
	uint8_t linkmode;
	int socket_id;
	bool chiplink;
};

void pci_platform_init(void);

#ifdef CONFIG_DW_PCIE
void smmu_pcie_alloc_sme(void);
#else
#define smmu_pcie_alloc_sme()		do { } while (0)
#endif

#endif /* __PCI_DUOWEN_H_INCLUDE__ */
