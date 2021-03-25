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

#define X16         0
#define X8          1
#define X4_0        2
#define X4_1        3
#define SUBSYS      4

#define APB_PORT_X16                0x4
#define APB_PORT_X8                 0x5
#define APB_PORT_X4_0               0x6
#define APB_PORT_X4_1               0x7
#define APB_PORT_SUBSYS             0x8

#define AXI_DBI_PORT_X16            0x0
#define AXI_DBI_PORT_X8             0x1
#define AXI_DBI_PORT_X4_0           0x2
#define AXI_DBI_PORT_X4_1           0x3

#define RESET_CORE_X16              0x0
#define RESET_CORE_X8               0x4
#define RESET_CORE_X4_0             0x8
#define RESET_CORE_X4_1             0xC
#define RESET_PHY                   0x10
#define SUBSYS_CONTROL              0x14
#define REFCLK_CONTROL              0x18
#define SRAM_CONTROL                0x1c
#define SRAM_STATUS                 0x20

#define LINK_MODE_4_4_4_4		0x0
#define LINK_MODE_8_4_0_4		0x1
#define LINK_MODE_8_8_0_0		0x2
#define LINK_MODE_16_0_0_0		0x3
#define LINK_MODE_ZEBU			0x4
#define LINK_MODE_INVALID		0xff

#define LINK_MODE_0			LINK_MODE_4_4_4_4
#define LINK_MODE_1			LINK_MODE_8_4_0_4
#define LINK_MODE_2			LINK_MODE_8_8_0_0
#define LINK_MODE_3			LINK_MODE_16_0_0_0
#define LINK_MODE_4			LINK_MODE_ZEBU

#ifdef CONFIG_DUOWEN_PCIE_LINK_MODE_0
#define DEFAULT_LINK_MODE		LINK_MODE_0
#endif
#ifdef CONFIG_DUOWEN_PCIE_LINK_MODE_1
#define DEFAULT_LINK_MODE		LINK_MODE_1
#endif
#ifdef CONFIG_DUOWEN_PCIE_LINK_MODE_2
#define DEFAULT_LINK_MODE		LINK_MODE_2
#endif
#ifdef CONFIG_DUOWEN_PCIE_LINK_MODE_3
#define DEFAULT_LINK_MODE		LINK_MODE_3
#endif
#ifdef CONFIG_DUOWEN_PCIE_LINK_MODE_4
#define DEFAULT_LINK_MODE		LINK_MODE_4
#endif
#ifndef DEFAULT_LINK_MODE
#define DEFAULT_LINK_MODE		LINK_MODE_16_0_0_0
#endif

#ifdef IPBENCH
#define CFG_APB_SUBSYS              0x0
#define CFG_APB_CORE_X16            0x0
#define CFG_APB_CORE_X8             0x0
#define CFG_APB_CORE_X4_0           0x0
#define CFG_APB_CORE_X4_1           0x0

#define CFG_APB_PHY_0               0x0
#define CFG_APB_PHY_1               0x0
#define CFG_APB_PHY_2               0x0
#define CFG_APB_PHY_3               0x0

#define CFG_AXI_CORE_X16            0x0
#define CFG_AXI_CORE_X8             0x0
#define CFG_AXI_CORE_X4_0           0x0
#define CFG_AXI_CORE_X4_1           0x0
#else
#define CFG_APB_SUBSYS              0xff09000000ULL
#define CFG_APB_CORE_X16            0xff09001000ULL
#define CFG_APB_CORE_X8             0xff09002000ULL
#define CFG_APB_CORE_X4_0           0xff09003000ULL
#define CFG_APB_CORE_X4_1           0xff09004000ULL

#define CFG_APB_PHY_0               0xff09180000ULL
#define CFG_APB_PHY_1               0xff09280000ULL
#define CFG_APB_PHY_2               0xff09380000ULL
#define CFG_APB_PHY_3               0xff09480000ULL

#define CFG_AXI_CORE_X16            0xff09100000ULL
#define CFG_AXI_CORE_X8             0xff09200000ULL
#define CFG_AXI_CORE_X4_0           0xff09300000ULL
#define CFG_AXI_CORE_X4_1           0xff09400000ULL
#endif

#define KB                          (1UL << 10)
#define MB                          (1UL << 20)
#define GB                          (1UL << 30)

#define PCIE_CORE_RANGE			512*GB
#define PCIE_SUBSYS_ADDR_START      0x40000000000ULL
#define PCIE_CORE_X16_ADDR_START    0
#define PCIE_CORE_X8_ADDR_START     PCIE_CORE_X16_ADDR_START + PCIE_CORE_RANGE
#define PCIE_CORE_X4_0_ADDR_START   PCIE_CORE_X8_ADDR_START + PCIE_CORE_RANGE
#define PCIE_CORE_X4_1_ADDR_START   PCIE_CORE_X4_0_ADDR_START + PCIE_CORE_RANGE
#define PCIE_SUBSYS_ADDR_END        PCIE_CORE_X4_1_ADDR_START + PCIE_CORE_RANGE

#define PCIE_CORE_CFG_SIZE      256*MB
#define PCIE_CORE_MEM_SIZE      PCIE_CORE_RANGE - 2*PCIE_CORE_CFG_SIZE
#define PCIE_CORE_X16_CFG1_START     PCIE_CORE_X8_ADDR_START - PCIE_CORE_CFG_SIZE
#define PCIE_CORE_X16_CFG0_START     PCIE_CORE_X8_ADDR_START - 2*PCIE_CORE_CFG_SIZE
#define PCIE_CORE_X8_CFG1_START     PCIE_CORE_X4_0_ADDR_START - PCIE_CORE_CFG_SIZE
#define PCIE_CORE_X8_CFG0_START     PCIE_CORE_X4_0_ADDR_START - 2*PCIE_CORE_CFG_SIZE
#define PCIE_CORE_X4_0_CFG1_START     PCIE_CORE_X4_1_ADDR_START - PCIE_CORE_CFG_SIZE
#define PCIE_CORE_X4_0_CFG0_START     PCIE_CORE_X4_1_ADDR_START - 2*PCIE_CORE_CFG_SIZE
#define PCIE_CORE_X4_1_CFG1_START     PCIE_SUBSYS_ADDR_END - PCIE_CORE_CFG_SIZE
#define PCIE_CORE_X4_1_CFG0_START     PCIE_SUBSYS_ADDR_END - 2*PCIE_CORE_CFG_SIZE

#ifdef CONFIG_DUOWEN_PCIE_TEST
#define MSI_INT     0
#define INTA_INT    1
#endif

#ifdef IPBENCH
void apb_read_c(uint32_t addr, uint32_t *data, int port);
void apb_write_c(uint32_t addr, uint32_t data, int port);
#endif

void init_duowen_pcie_subsystem(void);


struct duowen_pcie_subsystem
{
	uint64_t cfg_apb[5];
	uint8_t link_mode;
	uint8_t ctrl_cnt;
	int socket_id;
	bool chiplink;

#ifdef IPBENCH
	//uint8_t port_
#endif

	struct dw_pcie *controller;

	//uint8_t (* get_link_mode)(struct duowen_pcie_subsystem *pcie_subsystem);
	//uint32_t (* apb_read)(uint64_t *addr);
	//void (* apb_write)(uint64_t *addr, uint32_t val);
};

void pci_platform_init(void);

#ifdef CONFIG_DW_PCIE
void smmu_pcie_alloc_sme(void);
#else
#define smmu_pcie_alloc_sme()		do { } while (0)
#endif

#endif /* __PCI_DUOWEN_H_INCLUDE__ */
