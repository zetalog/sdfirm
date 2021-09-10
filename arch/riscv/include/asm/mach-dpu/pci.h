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

#ifndef __PCI_DPU_H_INCLUDE__
#define __PCI_DPU_H_INCLUDE__

#include <target/arch.h>

#ifdef CONFIG_DW_PCIE
#ifndef ARCH_HAVE_PCI
#define ARCH_HAVE_PCI		1
/* PCIe driver sdfirm/dpu instantiation */
#define SDFIRM			1
#define DPU			1
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

#define LINK_MODE_4_4_4_4           0x0
#define LINK_MODE_8_4_0_4           0x1
#define LINK_MODE_8_8_0_0           0x2
#define LINK_MODE_16_0_0_0          0x3
#define LINK_MODE_INVALID           0xff

#define LINK_MODE_0                 LINK_MODE_4_4_4_4
#define LINK_MODE_1                 LINK_MODE_8_4_0_4
#define LINK_MODE_2                 LINK_MODE_8_8_0_0
#define LINK_MODE_3                 LINK_MODE_16_0_0_0

#ifndef DEFAULT_LINK_MODE
#define DEFAULT_LINK_MODE           LINK_MODE_16_0_0_0
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
#define CFG_APB_PHY_0               0x20000800UL
#define CFG_APB_SUBSYS              0x20000000UL
#define CFG_APB_CORE_X16            0x20000400UL
#define CFG_APB_CORE_X8             0x4512000UL
#define CFG_APB_CORE_X4_0           0x4513000UL
#define CFG_APB_CORE_X4_1           0x4514000UL

#define CFG_AXI_CORE_X16            0x20800000UL
#define CFG_AXI_CORE_X8             0x4380000UL
#define CFG_AXI_CORE_X4_0           0x4400000UL
#define CFG_AXI_CORE_X4_1           0x4480000UL
#endif

#define KB                          (1UL << 10)
#define MB                          (1UL << 20)
#define GB                          (1UL << 30)

#define PCIE_SUBSYS_ADDR_START      0x100000000ULL
#define PCIE_CORE_X16_ADDR_START    0
#define PCIE_CORE_X8_ADDR_START     (PCIE_CORE_X16_ADDR_START + 4*GB)
#define PCIE_CORE_X4_0_ADDR_START   (PCIE_CORE_X8_ADDR_START + 4*GB)
#define PCIE_CORE_X4_1_ADDR_START   (PCIE_CORE_X4_0_ADDR_START + 4*GB)
#define PCIE_SUBSYS_ADDR_END        (PCIE_CORE_X4_1_ADDR_START + 4*GB)

#define PCIE_CORE_CFG_SIZE      (512*KB)
#define PCIE_CORE_MEM_SIZE      (4*GB - 512*KB)
#define PCIE_CORE_X16_CFG0_START     (PCIE_CORE_X8_ADDR_START - PCIE_CORE_CFG_SIZE)
#define PCIE_CORE_X16_CFG1_START     (PCIE_CORE_X8_ADDR_START - 2*PCIE_CORE_CFG_SIZE)
#define PCIE_CORE_X8_CFG0_START     (PCIE_CORE_X4_0_ADDR_START - PCIE_CORE_CFG_SIZE)
#define PCIE_CORE_X8_CFG1_START     (PCIE_CORE_X4_0_ADDR_START - 2*PCIE_CORE_CFG_SIZE)
#define PCIE_CORE_X4_0_CFG0_START     (PCIE_CORE_X4_1_ADDR_START - PCIE_CORE_CFG_SIZE)
#define PCIE_CORE_X4_0_CFG1_START     (PCIE_CORE_X4_1_ADDR_START - 2*PCIE_CORE_CFG_SIZE)
#define PCIE_CORE_X4_1_CFG0_START     (PCIE_SUBSYS_ADDR_END - PCIE_CORE_CFG_SIZE)
#define PCIE_CORE_X4_1_CFG1_START     (PCIE_SUBSYS_ADDR_END - 2*PCIE_CORE_CFG_SIZE)

#ifdef CONFIG_DPU_PCIE_TEST
#define MSI_INT     0
#define INTA_INT    1
#endif

#define PCIE_RC_ADDR_LOW_REG 0x824
#define PCIE_RC_ADDR_HI_REG 0x828
#define PCIE_EP_ADDR_LOW_REG 0x82c
#define PCIE_EP_ADDR_HI_REG 0x834
#define PCIE_HUGE_DATA_SIZE_REG 0x838
#define PCIE_RC_RCVED_REG 0x840
#define PCIE_EP_RCVED_REG 0x844
#define PCIE_EP_WR_SIZE_PERTIME_REG 0x84c

#define PCIE_DDR_BASE_ADDR_LOW_REG 0x820
#define PCIE_DDR_BASE_ADDR_HI_REG 0x850

#define PCIE_CMD_REG 0x858
/*
 * reg[2:0]:
 * 1: mem alloc
 * 2: mem free
 * 3: mem dump
 * 7: invalid cmd
 */
#define MEM_ALLOC_REQ 1
#define MEM_FREE_REQ 2
#define MEM_DUMP_REQ 3
#define INVALID_CMD_REQ 7

#define PCIE_TOTAL_SIZE_LW_REG 0x85c
/*
 * reg[31:0]:total_size[31:0]
 */

#define PCIE_TOTAL_SIZE_HI_REG 0x864
/*
 * reg[3:0]:total_size[35:32]
 */

#define PCIE_FLAG_REG 0x868
/*
 * reg[2:0]
 * for rc2ep:
 *      0: last package finished
 *      1: malloc requrest
 *      4: run mcu_code
 * for ep2rc:
 *      2:ep return malloc response
 *      3:ep last package finished
 *      5:ep return free response
 *      6:ep run mcu code ok response
 */
#define EP2RC_INVALID_FLG 0
#define RC2EP_LAST_PKG_FLG 1
#define RC2EP_MEM_ALLOC_REQ_FLG 2
#define EP2RC_MALLOC_OK_FLG 3
#define EP2RC_LAST_PKG_FLG 4
#define RC2EP_RUN_MCU_CODE_FLG 5
#define EP2RC_FREE_OK_FLG 6
#define EP2RC_RUN_MCU_OK_FLG 7

#define EP_ALLOCED_MEM_LW_REG 0x870
/*
 * reg[31:0]: addr[31:0]
 */

#define EP_ALLOCED_MEM_HI_REG 0x874
/*
 * reg[3:0]: addr[35:32]
 */
#define RC_FREE_MEM_LW_REG 0x87c
/*
 * reg[31:0]: addr[31:0]
 */

#define RC_FREE_MEM_HI_REG 0x880
/*
 * reg[3:0]: addr[35:32]
 * reg[7:4]: reserved page for invalid DMA
 */

#define EP_RSVED_MEM_LW_REG 0x888
/*
 * reg[31:0]: addr[31:0]
 */

#ifdef IPBENCH
void apb_read_c(uint32_t addr, uint32_t *data, int port);
void apb_write_c(uint32_t addr, uint32_t data, int port);
#endif

void init_duowen_pcie_subsystem(void);

struct duowen_pcie_subsystem {
	uint64_t cfg_apb[5];
	uint8_t link_mode;

#ifdef IPBENCH
	//uint8_t port_
#endif

	struct dw_pcie *controller;

	//uint8_t (* get_link_mode)(struct duowen_pcie_subsystem *pcie_subsystem);
	//uint32_t (* apb_read)(uint64_t *addr);
	//void (* apb_write)(uint64_t *addr, uint32_t val);
};

#ifdef CONFIG_PCI
void pci_platform_init(void);
uint32_t dw_pcie_read_axi(uint64_t addr);
void dw_pcie_write_axi(uint64_t addr, uint32_t data);
#else
#define pci_platform_init()			do { } while (0)
#define dw_pcie_read_axi(addr) do { } while (0)
#define dw_pcie_write_axi(addr, data) do { } while (0)
#endif

#endif /* __PCI_DPU_H_INCLUDE__ */
