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

#define PCIE_SUBSYS_REG(offset)			(PCIE_SUB_CUST_BASE + (offset))
#define PCIE_CORE_REG(x, offset)		\
	(PCIE0_CUST_BASE + ((x) << 12) + (offset))

/* PCIe core_x[lane] IDs */
#define PCIE_CORE_X16				0
#define PCIE_CORE_X8				1
#define PCIE_CORE_X4_0				2
#define PCIE_CORE_X4_1				3
#define PCIE_CORE_CHIPLINK			PCIE_CORE_X4_1
#define PCIE_MAX_CORES				4

/* PCIe subsystem registers */
#define PCIE_RESET_CONTROL_X16			PCIE_SUBSYS_REG(0x000)
#define PCIE_RESET_CONTROL_X8			PCIE_SUBSYS_REG(0x004)
#define PCIE_RESET_CONTROL_X4_0			PCIE_SUBSYS_REG(0x008)
#define PCIE_RESET_CONTROL_X4_1			PCIE_SUBSYS_REG(0x00C)
#define PCIE_RESET_CONTROL_PHY			PCIE_SUBSYS_REG(0x010)
#define PCIE_SUBSYSTEM_CONTROL			PCIE_SUBSYS_REG(0x014)
#define PCIE_REFCLK_CONTROL			PCIE_SUBSYS_REG(0x018)
#define PCIE_SRAM_CONTROL			PCIE_SUBSYS_REG(0x01C)
#define PCIE_SRAM_STATUS			PCIE_SUBSYS_REG(0x020)

/* PCIe core_x[lane] registers */
#define PCIE_CFGINFO_LEVEL(x)			PCIE_CORE_REG(x, 0x000)
#define PCIE_OBFF(x)				PCIE_CORE_REG(x, 0x004)
#define PCIE_ELECTROMECHANICAL_CFG(x)		PCIE_CORE_REG(x, 0x00C)
#define PCIE_LINK_STATUS(x)			PCIE_CORE_REG(x, 0x010)
#define PCIE_INT_STATUS(x)			PCIE_CORE_REG(x, 0x014)
#define PCIE_INT_STATUS_MASK(x)			PCIE_CORE_REG(x, 0x018)
#define PCIE_AER_STATUS(x)			PCIE_CORE_REG(x, 0x01C)
#define PCIE_AER_STATUS_MASK(x)			PCIE_CORE_REG(x, 0x020)
#define PCIE_PME_STATUS(x)			PCIE_CORE_REG(x, 0x024)
#define PCIE_PME_STATUS_MASK(x)			PCIE_CORE_REG(x, 0x028)
#define PCIE_HP_STATUS(x)			PCIE_CORE_REG(x, 0x02C)
#define PCIE_HP_STATUS_MASK(x)			PCIE_CORE_REG(x, 0x030)
#define PCIE_PM_STATUS(x)			PCIE_CORE_REG(x, 0x034)
#define PCIE_LINK_TRAINING_STATUS(x)		PCIE_CORE_REG(x, 0x038)
#define PCIE_LINK_TRAINING_STATUS_MASK(x)	PCIE_CORE_REG(x, 0x03C)
#define PCIE_MSI_INT_STATUS(x)			PCIE_CORE_REG(x, 0x040)
#define PCIE_MSI_INT_STATUS_MASK(x)		PCIE_CORE_REG(x, 0x044)
#define PCIE_LTR_REQ(x)				PCIE_CORE_REG(x, 0x048)
#define PCIE_LTR_REQ_LATENCY(x)			PCIE_CORE_REG(x, 0x04C)
#define PCIE_LTR_CORE_STATUS(x)			PCIE_CORE_REG(x, 0x050)
#define PCIE_DEBUGINFO_EI(x)			PCIE_CORE_REG(x, 0x058)
#define PCIE_DEBUGINFO0(x)			PCIE_CORE_REG(x, 0x05C)
#define PCIE_DEBUGINFO1(x)			PCIE_CORE_REG(x, 0x060)
#define PCIE_BDF(x)				PCIE_CORE_REG(x, 0x064)
#define PCIE_SEND_VENDOR_MESSAGE_REQ(x)		PCIE_CORE_REG(x, 0x068)
#define PCIE_SEND_VENDOR_MESSAGE_INFO0(x)	PCIE_CORE_REG(x, 0x06C)
#define PCIE_SEND_VENDOR_MESSAGE_INFO1(x)	PCIE_CORE_REG(x, 0x070)
#define PCIE_SEND_VENDOR_MESSAGE_INFO2(x)	PCIE_CORE_REG(x, 0x074)
#define PCIE_SEND_VENDOR_MESSAGE_INFO3(x)	PCIE_CORE_REG(x, 0x078)
#define PCIE_RECV_VENDOR_MESSAGE_VALID(x)	PCIE_CORE_REG(x, 0x088)
#define PCIE_RECV_VENDOR_MESSAGE_INFO0(x)	PCIE_CORE_REG(x, 0x08C)
#define PCIE_RECV_VENDOR_MESSAGE_INFO1(x)	PCIE_CORE_REG(x, 0x090)
#define PCIE_RECV_VENDOR_MESSAGE_INFO2(x)	PCIE_CORE_REG(x, 0x094)
#define PCIE_RECV_VENDOR_MESSAGE_INFO3(x)	PCIE_CORE_REG(x, 0x098)
#define PCIE_RECV_VENDOR_MESSAGE_INFO4(x)	PCIE_CORE_REG(x, 0x09C)
#define PCIE_LOCAL_DMA_INT(x)			PCIE_CORE_REG(x, 0x0A0)
#define PCIE_LOCAL_DMA_INT_MASK(x)		PCIE_CORE_REG(x, 0x0A4)
#define PCIE_LINK_DOWN_INT_STATUS(x)		PCIE_CORE_REG(x, 0x0A8)
#define PCIE_LINK_DOWN_INT_STATUS_MASK(x)	PCIE_CORE_REG(x, 0x0AC)
#define PCIE_BRIDGE_FLUSH_STATUS(x)		PCIE_CORE_REG(x, 0x0B0)
#define PCIE_BRIDGE_FLUSH_STATUS_MASK(x)	PCIE_CORE_REG(x, 0x0B4)
#define PCIE_CFG_MSI_PENDING(x)			PCIE_CORE_REG(x, 0x0B8)
#define PCIE_DIAG_BUS_SEL(x)			PCIE_CORE_REG(x, 0x0BC)
#define PCIE_DIAG_BUS(x, n)			\
	PCIE_CORE_REG(x, 0x0C0 + ((n) << 2))

/* PCIE_RESET_CONTROL_Xx */
#define DW_PCIE_dbi_rst_n			_BV(7)
#define DW_PCIE_core_rst_n			_BV(6)
#define DW_PCIE_button_rst_n			_BV(5)
#define DW_PCIE_non_sticky_rst_n		_BV(4)
#define DW_PCIE_sticky_rst_n			_BV(3)
#define DW_PCIE_perst_n				_BV(2)
#define DW_PCIE_Slv_aresetn			_BV(1)
#define DW_PCIE_Mstr_aresetn			_BV(0)
/* Controller reset */
#define DW_PCIE_RESET_CTRL_ALL			\
	(DW_PCIE_dbi_rst_n |			\
	 DW_PCIE_core_rst_n |			\
	 DW_PCIE_button_rst_n |			\
	 DW_PCIE_non_sticky_rst_n |		\
	 DW_PCIE_sticky_rst_n |			\
	 DW_PCIE_perst_n |			\
	 DW_PCIE_Slv_aresetn |			\
	 DW_PCIE_Mstr_aresetn)

/* PCIE_RESET_CONTROL_PHY */
#define DW_PCIE_phy_rst_d			_BV(3)
#define DW_PCIE_phy_rst_c			_BV(2)
#define DW_PCIE_phy_rst_b			_BV(1)
#define DW_PCIE_phy_rst_a			_BV(0)
#define DW_PCIE_RESET_PHY_ALL			\
	(DW_PCIE_phy_rst_d |			\
	 DW_PCIE_phy_rst_c |			\
	 DW_PCIE_phy_rst_b |			\
	 DW_PCIE_phy_rst_a)

/* PCIE_SUBSYSTEM_CONTROL */
#define DW_PCIE_link_mode_OFFSET		0
#define DW_PCIE_link_mode_MASK			REG_2BIT_MASK
#define DW_PCIE_link_mode(value)		\
	_SET_FV(DW_PCIE_link_mode, value)
/* Note:
 * Available link modes for DUOWEN:
 * 0: DUOWEN_PCIE_LINK_MODE_4_4_4_4
 * 1: DUOWEN_PCIE_LINK_MODE_8_4_0_4
 * 2: DUOWEN_PCIE_LINK_MODE_8_8_0_0
 * 3: DUOWEN_PCIE_LINK_MODE_16_0_0_0
 */
#define DW_PCIE_LINK_MODE_INVALID		0xff

/* PCIE_REFCLK_CONTROL */
#define DW_PCIE_ref_use_pad			_BV(0)

/* PCIE_SRAM_CONTROL */
#define DW_PCIE_phy_sram_bypass(n)		_BV(((n) << 1) + 1)
#define DW_PCIE_phy_sram_ext_ld_done(n)		_BV((n) << 1)
#define DW_PCIE_phy_sram_bypass_all		\
	(DW_PCIE_phy_sram_bypass(0) |		\
	 DW_PCIE_phy_sram_bypass(1) |		\
	 DW_PCIE_phy_sram_bypass(2) |		\
	 DW_PCIE_phy_sram_bypass(3))
#define DW_PCIE_phy_sram_ext_ld_done_all	\
	(DW_PCIE_phy_sram_ext_ld_done(0) |	\
	 DW_PCIE_phy_sram_ext_ld_done(1) |	\
	 DW_PCIE_phy_sram_ext_ld_done(2) |	\
	 DW_PCIE_phy_sram_ext_ld_done(3))

/* PCIE_SRAM_STATUS */
#define DW_PCIE_phy_sram_init_done(n)		_BV(n)
#define DW_PCIE_phy_sram_init_done_all		\
	(DW_PCIE_phy_sram_init_done(0) |	\
	 DW_PCIE_phy_sram_init_done(1) |	\
	 DW_PCIE_phy_sram_init_done(2) |	\
	 DW_PCIE_phy_sram_init_done(3))

/* PCIE_CFGINFO_LEVEL */
#define DW_PCIE_app_clk_req_n			_BV(28)
#define DW_PCIE_clkreq_in_n			_BV(27)
#define DW_PCIE_app_margining_ready		_BV(26)
#define DW_PCIE_app_margining_software_ready	_BV(25)
#define DW_PCIE_app_l1sub_disable		_BV(24)
#define DW_PCIE_app_hold_phy_rst		_BV(23)
#define DW_PCIE_app_clk_pm_en			_BV(21)
#define DW_PCIE_diag_ctrl_bus_OFFSET		18
#define DW_PCIE_diag_ctrl_bus_MASK		REG_3BIT_MASK
#define DW_PCIE_diag_ctrl_bus(value)		\
	_SET_FV(DW_PCIE_diag_ctrl_bus, value)
#define DW_PCIE_app_dbi_ro_wr_disable		_BV(17)
#define DW_PCIE_app_sris_mode			_BV(16)
#define DW_PCIE_app_ltssm_enable		_BV(15)
#define DW_PCIE_sys_aux_pwr_det			_BV(14)
#define DW_PCIE_apps_pm_xmt_pme			_BV(13)
#define DW_PCIE_outband_pwrup_cmd		_BV(12)
#define DW_PCIE_tx_lane_flip_en			_BV(11)
#define DW_PCIE_rx_lane_flip_en			_BV(10)
#define DW_PCIE_app_ready_entr_l23		_BV(9)
#define DW_PCIE_app_req_exit_l1			_BV(8)
#define DW_PCIE_app_xfer_pending		_BV(7)
#define DW_PCIE_app_init_rst			_BV(6)
#define DW_PCIE_device_type_OFFSET		2
#define DW_PCIE_device_type_MASK		REG_4BIT_MASK
#define DW_PCIE_device_type(value)		\
	_SET_FV(DW_PCIE_device_type, value)
#define DW_PCIE_ENDPOINT			0x00
#define DW_PCIE_LEGACY_ENDPOINT			0x01
#define DW_PCIE_ROOT_COMPLEX			0x04
#define DW_PCIE_app_pf_req_retry_en		_BV(1)
#define DW_PCIE_app_req_retry_en		_BV(0)
#define DW_PCIE_LTSSM_DETECT			\
	(DW_PCIE_clkreq_in_n |			\
	 DW_PCIE_app_margining_ready |		\
	 DW_PCIE_app_hold_phy_rst |		\
	 DW_PCIE_app_sris_mode)
#define DW_PCIE_LTSSM_ENABLE			\
	(DW_PCIE_clkreq_in_n |			\
	 DW_PCIE_app_margining_ready |		\
	 DW_PCIE_app_sris_mode |		\
	 DW_PCIE_app_ltssm_enable)

/* PCIE_LINK_STATUS */
#define DW_PCIE_smlh_link_up			_BV(11)
#define DW_PCIE_smlh_ltssm_state_rcvry_eq	_BV(10)
#define DW_PCIE_smlh_ltssm_state_OFFSET		4
#define DW_PCIE_smlh_ltssm_state_MASK		REG_6BIT_MASK
#define DW_PCIE_smlh_ltssm_state(value)		\
	_GET_FV(DW_PCIE_smlh_ltssm_state, value)
#define DW_PCIE_LTSSM_S_DETECT_QUIET		0
#define DW_PCIE_LTSSM_S_DETECT_ACT		1
#define DW_PCIE_LTSSM_S_POLL_ACTIVE		2
#define DW_PCIE_LTSSM_S_POLL_COMPLETE		3
#define DW_PCIE_LTSSM_S_POLL_CONFIG		4
#define DW_PCIE_LTSSM_S_PRE_DETECT_QUIET	5
#define DW_PCIE_LTSSM_S_DETECT_WAIT		6
#define DW_PCIE_LTSSM_S_CFG_LINKWD_START	7
#define DW_PCIE_LTSSM_S_CFG_LINKWD_ACEPT	8
#define DW_PCIE_LTSSM_S_CFG_LANENUM_WAI		9
#define DW_PCIE_LTSSM_S_CFG_LANENUM_ACEPT	10
#define DW_PCIE_LTSSM_S_CFG_COMPLETE		11
#define DW_PCIE_LTSSM_S_CFG_IDLE		12
#define DW_PCIE_LTSSM_RCVRY_LOCK		13
#define DW_PCIE_LTSSM_RCVRY_SPEED		14
#define DW_PCIE_LTSSM_RCVRY_RCVRCFG		15
#define DW_PCIE_LTSSM_RCVRY_IDLE		16
#define DW_PCIE_LTSSM_L0			17
#define DW_PCIE_LTSSM_L0S			18
#define DW_PCIE_LTSSM_L123_SEND_EIDLE		19
#define DW_PCIE_LTSSM_L1_IDLE			20
#define DW_PCIE_LTSSM_L2_IDLE			21
#define DW_PCIE_LTSSM_L2_WAKE			22
#define DW_PCIE_LTSSM_S_DISABLED_ENTRY		23
#define DW_PCIE_LTSSM_S_DISABLED_IDLE		24
#define DW_PCIE_pm_curnt_state_OFFSET		1
#define DW_PCIE_pm_curnt_state_MASK		REG_3BIT_MASK
#define DW_PCIE_pm_curnt_state(value)		\
	_GET_FV(DW_PCIE_pm_curnt_state, value)
#define DW_PCIE_rdlh_link_up			_BV(0)
#define DW_PCIE_link_up				\
	(DW_PCIE_smlh_link_up | DW_PCIE_rdlh_link_up)

/* The lane link information can be decoded from the following link modes
 * encodings.
 */
#define DUOWEN_PCIE_LINK_MODE_0			0
#define DUOWEN_PCIE_LINK_MODE_4			1
#define DUOWEN_PCIE_LINK_MODE_8			2
#define DUOWEN_PCIE_LINK_MODE_16		3
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

#ifdef IPBENCH
#define CFG_APB_SUBSYS			0x0
#define CFG_APB_CORE(x)			0x0
#define CFG_APB_PHY(x)			0x0
#define CFG_AXI_CORE(x)			0
#else
#define CFG_APB_SUBSYS			ULL(0xff09000000)
#define CFG_APB_CORE(x)			(ULL(0xff09001000) + ((x) << 12))
#define CFG_APB_PHY(x)			(ULL(0xff09180000) + ((x) << 20))
#define CFG_AXI_CORE(x)			(ULL(0xff09100000) + ((x) << 20))
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
	uint64_t core_base[PCIE_MAX_CORES];
	uint64_t subsys_base;
	uint8_t linkmode;
};

void pci_platform_init(void);

#ifdef CONFIG_DW_PCIE
void smmu_pcie_alloc_sme(void);
#else
#define smmu_pcie_alloc_sme()		do { } while (0)
#endif

#endif /* __PCI_DUOWEN_H_INCLUDE__ */
