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
 * @(#)freqplan20.c: version 2.0 chip frequency plan
 * $Id: freqplan20.c,v 1.279 2019-04-14 10:19:18 zhenglv Exp $
 */

#include <target/clk.h>

const caddr_t Qdf2400V20CBCRs[] = {
	PCC_I2C_APPS_CBCR(PCC_BLSP1_QUP0),
	PCC_I2C_APPS_CBCR(PCC_BLSP1_QUP1),
	PCC_I2C_APPS_CBCR(PCC_BLSP1_QUP2),
	PCC_I2C_APPS_CBCR(PCC_BLSP1_QUP3),
	PCC_I2C_APPS_CBCR(PCC_BLSP2_QUP0),
	PCC_I2C_APPS_CBCR(PCC_BLSP2_QUP1),
	PCC_I2C_APPS_CBCR(PCC_BLSP2_QUP2),
	PCC_I2C_APPS_CBCR(PCC_BLSP2_QUP3),
	PCC_UART_APPS_CBCR(PCC_BLSP3_UART0),
	PCC_UART_APPS_CBCR(PCC_BLSP3_UART1),
	PCC_UART_APPS_CBCR(PCC_BLSP3_UART2),
	PCC_UART_APPS_CBCR(PCC_BLSP3_UART3),
	PCC_PDM2_CBCR,
	PCC_PERIPH_SPI_CORE_CBCR,
	PCC_SFAB_PFAB_QSMMUV3_CLIENT_CBCR,
	PCIE_CC_PCIE_X16_MSTR_AXI_CBCR(0),
	PCIE_CC_PCIE_X16_SLV_AXI_CBCR(0),
	PCIE_CC_PCIE_X8_SLV_AXI_CBCR(0, PCIE_CC_X8_0),
	PCIE_CC_PCIE_X8_MSTR_AXI_CBCR(0, PCIE_CC_X8_0),
	PCIE_CC_PCIE_X8_MSTR_AXI_CBCR(0, PCIE_CC_X8_1),
	PCIE_CC_PCIE_X8_SLV_AXI_CBCR(0, PCIE_CC_X8_1),
	PCIE_CC_PCIE_X16_MSTR_AXI_CBCR(1),
	PCIE_CC_PCIE_X16_SLV_AXI_CBCR(1),
	PCIE_CC_PCIE_X8_MSTR_AXI_CBCR(1, PCIE_CC_X8_0),
	PCIE_CC_PCIE_X8_MSTR_AXI_CBCR(1, PCIE_CC_X8_1),
	PCIE_CC_PCIE_X8_SLV_AXI_CBCR(1, PCIE_CC_X8_0),
	PCIE_CC_PCIE_X8_SLV_AXI_CBCR(1, PCIE_CC_X8_1),
	0,
};

const clk_t clk_2_0[] = {
	gccmw_ce1_clk,
	gccmw_cfab_n1_csfpb_fclk,
	gccmw_fan_tach_ahb_clk,
	gccmw_hdma_sfpb_ahb_clk,
	gccmw_ncsfpb_xpu_cfg_ahb_clk,
	gccmw_hmss_cprc_apcs_clk,
	gccmw_cprc_dig_clk,
	gccmw_cprc_mem_clk,
	gccmw_cprc_nvdda_clk,
	gccmw_cprc_svdda_clk,
	gccmw_emac0_125m_clk,
	gccmw_emac0_ahb_clk,
	gccmw_emac0_iommu_cfg_ahb_clk,
	gccmw_emac0_sys_25m_clk,
	gccmw_gmii0_tx_clk,
	gccmw_emac0_ram_sys_clk,
	gccmw_emac0_rx_clk,
	gccmw_emac1_125m_clk,
	gccmw_emac1_ahb_clk,
	gccmw_emac1_iommu_cfg_ahb_clk,
	gccmw_emac1_sys_25m_clk,
	gccmw_gmii1_tx_clk,
	gccmw_emac1_ram_sys_clk,
	gccmw_emac1_rx_clk,
	gccmw_fan_tach_pwm_clk,
	gccmw_gp0_clk,
	gccmw_gp1_clk,
	gccmw_gp2_clk,
	gccmw_hdma0_ahb_clk,
	gccmw_hdma1_ahb_clk,
	gccmw_hdma2_ahb_clk,
	gccmw_hdma3_ahb_clk,
	gccmw_hdma_sfpb_always_on_clk,
	gccmw_hdma_sfpb_clk,
	gccmw_hdma_vbu_cfg_ahb_clk,
	gccmw_hdma0_qsmmuv3_client_clk,
	gccmw_hdma0_sys_clk,
	gccmw_hdma1_qsmmuv3_client_clk,
	gccmw_hdma1_sys_clk,
	gccmw_hdma2_qsmmuv3_client_clk,
	gccmw_hdma2_sys_clk,
	gccmw_hdma3_qsmmuv3_client_clk,
	gccmw_hdma3_sys_clk,
	gccmw_hdma_vbu_core_clk,
	gccmw_hmss_axi_clk,
	gccmw_hmss_w_ahb_clk,
	gccmw_hmss_sw_ahb_clk,
	gccmw_hmss_nw_ahb_clk,
	gccmw_mddr2_ahb_clk,
	gccmw_mddr3_ahb_clk,
	gccmw_tsens2_ahb_clk,
	gccmw_tsens3_ahb_clk,
	gccmw_tsens4_ahb_clk,
	gccmw_at_w1_clk,
	gccmw_at_w2_clk,
	gccmw_at_w3_clk,
	gccmw_at_w5_clk,
	gccmw_at_w7_clk,
	gccmw_at_w9_clk,
	gccmw_hmss_at_w_clk,
	gccmw_qdss_traceclkin_clk,
	gccmw_mddr0_ahb_clk,
	gccmw_mddr1_ahb_clk,
	gccmw_tsens9_ahb_clk,
	gccmw_tsens10_ahb_clk,
	gccmw_sata_ln0_ahb_clk,
	gccmw_sata_ln1_ahb_clk,
	gccmw_sata_ln2_ahb_clk,
	gccmw_sata_ln3_ahb_clk,
	gccmw_sata_ln4_ahb_clk,
	gccmw_sata_ln5_ahb_clk,
	gccmw_sata_ln6_ahb_clk,
	gccmw_sata_ln7_ahb_clk,
	gccmw_sata_ln0_asic_clk,
	gccmw_sata_ln0_rx_clk,
	gccmw_sata_ln1_asic_clk,
	gccmw_sata_ln1_rx_clk,
	gccmw_sata_ln2_asic_clk,
	gccmw_sata_ln2_rx_clk,
	gccmw_sata_ln3_asic_clk,
	gccmw_sata_ln3_rx_clk,
	gccmw_sata_ln4_asic_clk,
	gccmw_sata_ln4_rx_clk,
	gccmw_sata_ln5_asic_clk,
	gccmw_sata_ln5_rx_clk,
	gccmw_sata_ln6_asic_clk,
	gccmw_sata_ln6_rx_clk,
	gccmw_sata_ln7_asic_clk,
	gccmw_sata_ln7_rx_clk,
	gccmw_sata_ln0_pmalive_clk,
	gccmw_sata_ln1_pmalive_clk,
	gccmw_sata_ln2_pmalive_clk,
	gccmw_sata_ln3_pmalive_clk,
	gccmw_sata_ln4_pmalive_clk,
	gccmw_sata_ln5_pmalive_clk,
	gccmw_sata_ln6_pmalive_clk,
	gccmw_sata_ln7_pmalive_clk,
	gccmw_sata_ln0_axi_clk,
	gccmw_sata_ln1_axi_clk,
	gccmw_sata_ln2_axi_clk,
	gccmw_sata_ln3_axi_clk,
	gccmw_sata_ln4_axi_clk,
	gccmw_sata_ln5_axi_clk,
	gccmw_sata_ln6_axi_clk,
	gccmw_sata_ln7_axi_clk,
	gccmw_sata_ln0_qsmmuv3_client_clk,
	gccmw_sata_ln1_qsmmuv3_client_clk,
	gccmw_sata_ln2_qsmmuv3_client_clk,
	gccmw_sata_ln3_qsmmuv3_client_clk,
	gccmw_sata_ln4_qsmmuv3_client_clk,
	gccmw_sata_ln5_qsmmuv3_client_clk,
	gccmw_sata_ln6_qsmmuv3_client_clk,
	gccmw_sata_ln7_qsmmuv3_client_clk,
	gccmw_sata_ln0_rxoob_clk,
	gccmw_sata_ln1_rxoob_clk,
	gccmw_sata_ln2_rxoob_clk,
	gccmw_sata_ln3_rxoob_clk,
	gccmw_sata_ln4_rxoob_clk,
	gccmw_sata_ln5_rxoob_clk,
	gccmw_sata_ln6_rxoob_clk,
	gccmw_sata_ln7_rxoob_clk,
	gccmw_sata_phy0_rxoob_clk,
	gccmw_sata_phy1_rxoob_clk,
	gccmw_sata_vbu_ahb_cfg_clk,
	gccmw_sata_vbu_core_clk,
	gccmw_fan_tach_sleep_clk,
	gccmw_ce1_ahb_clk,
	gccmw_ce1_axi_clk,
	gccmw_ce1_iommu_cfg_ahb_clk,
	gccmw_ce1_qsmmuv3_client_clk,
	gccmw_emac0_axi_clk,
	gccmw_emac0_qsmmuv3_client_clk,
	gccmw_emac1_axi_clk,
	gccmw_emac1_qsmmuv3_client_clk,
	gccmw_qdss_fclk,
	gccmw_qdss_stm_clk,
	gccmw_sfab_ce1_fclk,
	gccmw_sfab_emac0_fclk,
	gccmw_sfab_emac1_fclk,
	gccmw_sfab_vbu_fclk,
	gccmw_sfvbu_ce1_dvm_clk,
	gccmw_sfvbu_emac0_dvm_clk,
	gccmw_sfvbu_emac1_dvm_clk,
	gccmw_sfvbu_q22m_clk,
	gccmw_sfvbu_q22s_clk,
	gccmw_sfab_vbu_core_clk,
	gccmw_emac_ahb2phy_cfg_ahb_clk,
	gccmw_qdss_cfg_ahb_clk,
	gccmw_qdss_dap_ahb_clk,
	gccmw_qdss_rbcpr_xpu_ahb_clk,
	gccmw_sfab_vbu_cfg_clk,
	gccmw_tsens1_ahb_clk,
	gccmw_tic_clk,
	gcce_hmss_ne_ahb_clk,
	gcce_hmss_se_ahb_clk,
	gcce_mddr4_ahb_clk,
	gcce_tsens5_ahb_clk,
	gcce_pcie_ss_0_vbu_core_clk,
	gcce_pcie_ss_1_vbu_core_clk,
	gcce_at_e2_clk,
	gcce_at_e3_clk,
	gcce_at_e5_clk,
	gcce_at_e7_clk,
	gcce_at_e9_clk,
	gcce_hmss_at_e_clk,
	gcce_mddr5_ahb_clk,
	gcce_tsens8_ahb_clk,
	gcce_pcie_ss_0_secsfpb_ahb_clk,
	gcce_pcie_ss_1_secsfpb_ahb_clk,
	gcce_psclkgen_cfg_ahb_clk,
	gcce_tsens6_ahb_clk,
	gcce_tsens7_ahb_clk,
	gcce_xo_clk,
	gcce_tsens5_ext_clk,
	gcce_tsens6_ext_clk,
	gcce_tsens7_ext_clk,
	gcce_tsens8_ext_clk,
	gcce_xo_div4_clk,
	pcie_cc_0_pcie_phy_aa_cfg_ahb_clk,
	pcie_cc_0_pcie_phy_ab_cfg_ahb_clk,
	pcie_cc_0_pcie_phy_b_cfg_ahb_clk,
	pcie_cc_0_pcie_phy_c_cfg_ahb_clk,
	pcie_cc_0_pcie_x16_mmu_mstr_q23_clk,
	pcie_cc_0_pcie_x16_mstr_q23_clk,
	pcie_cc_0_pcie_x16_slv_q23_clk,
	pcie_cc_0_pcie_x8_0_mmu_mstr_q23_clk,
	pcie_cc_0_pcie_x8_0_mstr_q23_clk,
	pcie_cc_0_pcie_x8_1_mmu_mstr_q23_clk,
	pcie_cc_0_pcie_x8_1_mstr_q23_clk,
	pcie_cc_0_pcie_x8_0_slv_q23_clk,
	pcie_cc_0_pcie_x8_1_slv_q23_clk,
	pcie_cc_1_pcie_phy_aa_cfg_ahb_clk,
	pcie_cc_1_pcie_phy_ab_cfg_ahb_clk,
	pcie_cc_1_pcie_phy_b_cfg_ahb_clk,
	pcie_cc_1_pcie_phy_c_cfg_ahb_clk,
	pcie_cc_1_pcie_x16_mmu_mstr_q23_clk,
	pcie_cc_1_pcie_x16_mstr_q23_clk,
	pcie_cc_1_pcie_x16_slv_q23_clk,
	pcie_cc_1_pcie_x8_0_mmu_mstr_q23_clk,
	pcie_cc_1_pcie_x8_0_mstr_q23_clk,
	pcie_cc_1_pcie_x8_1_mmu_mstr_q23_clk,
	pcie_cc_1_pcie_x8_1_mstr_q23_clk,
	pcie_cc_1_pcie_x8_0_slv_q23_clk,
	pcie_cc_1_pcie_x8_1_slv_q23_clk,
	invalid_clk,
};

const clk_mux_t clk_mux_2_0[] = {
	/* IMC_CC */
	{
		.output_clk = imc_proc_clk_src,
		.input_clk = sys_pll0_out_main,
	},
	{
		.output_clk = imc_clk_src,
		.input_clk = sys_pll0_out_main,
	},
	/* GCCMW */
	{
		.output_clk = gccmw_xo_clk_src,
		.input_clk = west_xo,
	},
	{
		.output_clk = config_fab_clk_src,
		.input_clk = west_pll0_out_main,
	},
	{
		.output_clk = system_fab_clk_src,
		.input_clk = west_pll0_out_main,
	},
	{
		.output_clk = wcsfpb_clk_src,
		.input_clk = west_pll0_out_main,
	},
	{
		.output_clk = n1_csfpb_clk_src,
		.input_clk = west_pll0_out_main,
	},
	{
		.output_clk = s1_csfpb_clk_src,
		.input_clk = west_pll0_out_main,
	},
	{
		.output_clk = emac0_ahb_clk_src,
		.input_clk = west_pll0_out_main,
	},
	{
		.output_clk = emac1_ahb_clk_src,
		.input_clk = west_pll0_out_main,
	},
	{
		.output_clk = hmss_w_bus_clk_src,
		.input_clk = west_pll0_out_main,
        },
	{
		.output_clk = ce0_clk_src,
		.input_clk = west_pll1_out_main,
	},
	{
		.output_clk = ce1_clk_src,
		.input_clk = west_pll1_out_main,
	},
	{
		.output_clk = cprc_dig_clk_src,
		.input_clk = west_pll1_out_main,
	},
	{
		.output_clk = cprc_mem_clk_src,
		.input_clk = west_pll1_out_main,
	},
	{
		.output_clk = cprc_apcs_clk_src,
		.input_clk = west_pll1_out_main,
	},
	{
		.output_clk = cprc_nvdda_clk_src,
		.input_clk = west_pll1_out_main,
	},
	{
		.output_clk = cprc_svdda_clk_src,
		.input_clk = west_pll1_out_main,
	},
	{
		.output_clk = fan_tach_clk_src,
		.input_clk = west_pll0_out_main_div10,
	},
	{
		.output_clk = gp0_clk_src,
		.input_clk = west_pll0_out_main,
	},
	{
		.output_clk = gp1_clk_src,
		.input_clk = west_pll0_out_main,
	},
	{
		.output_clk = gp2_clk_src,
		.input_clk = west_pll0_out_main,
	},
	{
		.output_clk = hdma_ahb_clk_src,
		.input_clk = west_pll0_out_main,
	},
	{
		.output_clk = hdma_sfpb_clk_src,
		.input_clk = west_pll0_out_main,
	},
	{
		.output_clk = hdma_sys_clk_src,
		.input_clk = west_pll0_out_main,
	},
	{
		.output_clk = hdma_vbu_clk_src,
		.input_clk = west_pll0_out_main,
	},
	{
		.output_clk = w_qdss_at_clk_src,
		.input_clk = west_pll0_out_main,
	},
	{
		.output_clk = sys_fab_vbu_clk_src,
		.input_clk = west_pll0_out_main,
	},
	{
		.output_clk = sata_vbu_clk_src,
		.input_clk = west_pll0_out_main,
	},
	{
		.output_clk = parb0_ser_clk_src,
		.input_clk = parb_pre_div,
	},
	{
		.output_clk = parb1_ser_clk_src,
		.input_clk = parb_pre_div,
	},
	{
		.output_clk = parb2_ser_clk_src,
		.input_clk = parb_pre_div,
	},
	{
		.output_clk = qdss_traceclkin_clk_src,
		.input_clk = west_pll0_out_main,
	},
	{
		.output_clk = acc_clk_src,
		.input_clk = west_pll0_out_main,
	},
	{
		.output_clk = sec_ctrl_clk_src,
		.input_clk = west_xo,
	},
	{
		.output_clk = sata_ahb_clk_src,
		.input_clk = west_pll0_out_main,
	},
	{
		.output_clk = sata_q22_clk_src,
		.input_clk = west_pll0_out_main,
	},
	{
		.output_clk = sata_pmalive_clk_src,
		.input_clk = west_pll0_out_main,
	},
	{
		.output_clk = sata_rxoob_clk_src,
		.input_clk = west_pll0_out_main,
	},
	{
		.output_clk = sata_csfpb_clk_src,
		.input_clk = west_pll0_out_main,
	},
	/* GCCE */
	{
		.output_clk = secsfpb_clk_src,
		.input_clk = east_pll0_out_even,
	},
	{
		.output_clk = s2_csfpb_clk_src,
		.input_clk = east_pll0_out_even,
	},
	{
		.output_clk = n2_csfpb_clk_src,
		.input_clk = east_pll0_out_even,
	},
	{
		.output_clk = hmss_e_bus_clk_src,
		.input_clk = east_pll0_out_even,
	},
	{
		.output_clk = e_qdss_at_clk_src,
		.input_clk = east_pll0_out_even,
	},
	{
		.output_clk = pcie_ss_0_vbu_clk_src,
		.input_clk = east_pll0_out_even,
	},
	{
		.output_clk = pcie_ss_1_vbu_clk_src,
		.input_clk = east_pll0_out_even,
	},
	{
		.output_clk = gcce_xo_clk_src,
		.input_clk = east_xo,
	},
	/* PCC */
	{
		.output_clk = periph_fab_clk_src,
		.input_clk = west_pll0_out_main,
	},
	{
		.output_clk = blsp0_qup0_spi_apps_clk_src,
		.input_clk = west_xo,
	},
	{
		.output_clk = blsp0_qup1_spi_apps_clk_src,
		.input_clk = west_pll1_out_main,
	},
	{
		.output_clk = blsp0_uart0_apps_clk_src,
		.input_clk = west_xo,
	},
	{
		.output_clk = blsp0_uart1_apps_clk_src,
		.input_clk = west_xo,
	},
	{
		.output_clk = blsp0_uart2_apps_clk_src,
		.input_clk = west_xo,
	},
	{
		.output_clk = blsp0_uart3_apps_clk_src,
		.input_clk = west_xo,
	},
	{
		.output_clk = blsp1_qup0_i2c_apps_clk_src,
		.input_clk = west_pll1_out_main,
	},
	{
		.output_clk = blsp1_qup1_i2c_apps_clk_src,
		.input_clk = west_pll1_out_main,
	},
	{
		.output_clk = blsp1_qup2_i2c_apps_clk_src,
		.input_clk = west_pll1_out_main,
	},
	{
		.output_clk = blsp1_qup3_i2c_apps_clk_src,
		.input_clk = west_pll1_out_main,
	},
	{
		.output_clk = blsp2_qup0_i2c_apps_clk_src,
		.input_clk = west_pll1_out_main,
	},
	{
		.output_clk = blsp2_qup1_i2c_apps_clk_src,
		.input_clk = west_pll1_out_main,
	},
	{
		.output_clk = blsp2_qup2_i2c_apps_clk_src,
		.input_clk = west_pll1_out_main,
	},
	{
		.output_clk = blsp2_qup3_i2c_apps_clk_src,
		.input_clk = west_pll1_out_main,
	},
	{
		.output_clk = blsp3_qup0_spi_apps_clk_src,
		.input_clk = west_pll1_out_main,
	},
	{
		.output_clk = blsp3_qup1_spi_apps_clk_src,
		.input_clk = west_pll1_out_main,
	},
	{
		.output_clk = blsp3_uart0_apps_clk_src,
		.input_clk = west_xo,
	},
	{
		.output_clk = blsp3_uart1_apps_clk_src,
		.input_clk = west_xo,
	},
	{
		.output_clk = blsp3_uart2_apps_clk_src,
		.input_clk = west_xo,
	},
	{
		.output_clk = blsp3_uart3_apps_clk_src,
		.input_clk = west_xo,
	},
	{
		.output_clk = pdm2_clk_src,
		.input_clk = west_pll0_out_main,
	},
	{
		.output_clk = periph_spi_clk_src,
		.input_clk = west_pll0_out_main,
	},
	{
		.output_clk = sdcc0_apps_clk_src,
		.input_clk = west_pll0_out_main,
	},
	{
		.output_clk = sdcc1_apps_clk_src,
		.input_clk = west_pll0_out_main,
	},
	{
		.output_clk = sdcc2_apps_clk_src,
		.input_clk = west_pll0_out_main,
	},
	{
		.output_clk = sdcc3_apps_clk_src,
		.input_clk = west_pll0_out_main,
	},
	{
		.output_clk = usb2_mstr_clk_src,
		.input_clk = west_pll0_out_main,
	},
	/* PCIE_CC */
	{
		.output_clk = pcie0_sfpb_clk_src,
		.input_clk = east_pll0_out_main,
	},
	{
		.output_clk = pcie0_aux_clk_src,
		.input_clk = east_xo,
	},
	{
		.output_clk = pcie1_sfpb_clk_src,
		.input_clk = east_pll0_out_main,
	},
	{
		.output_clk = pcie1_aux_clk_src,
		.input_clk = east_xo,
	},
	/* END */
	{
		.output_clk = invalid_clk,
	},
};
