#ifndef __ETH_DUOWEN_H_INCLUDE__
#define __ETH_DUOWEN_H_INCLUDE__

#include <target/clk.h>

#define ETH_REG(offset)		(ETH_PLL_BASE + (offset))
#define XGMAC_CFG		ETH_REG(0x100)
#define XGMAC_SBD_CFG		ETH_REG(0x104)
#define XGMAC_STATUS		ETH_REG(0x108)
#define E16PHY_CFG		ETH_REG(0x120)
#define E16PHY_ADAPT_CFG	ETH_REG(0x124)
#define E16PHY_STATUS		ETH_REG(0x128)

/* XGMAC_CFG */
#define XGMAC_port_id_OFFSET		7
#define XGMAC_port_id_MASK		REG_5BIT_MASK
#define XGMAC_port_id(value)		_GET_FV(XGMAC_port_id, value)
#define XGMAC_matst_mode		_BV(6)
#define XGMAC_eth_ov_clk_en		_BV(5)
#define XGMAC_eth_pgc_pg_en		_BV(4)
#define XGMAC_enter_d3_req		_BV(3)
#define XGMAC_data_bit_reverse		_BV(2)
#define XGMAC_ref_dig_fr_clk_sel_OFFSET	0
#define XGMAC_ref_dig_fr_clk_sel_MASK	REG_2BIT_MASK
#define XGMAC_ref_dig_fr_clk_sel(value)	_SET_FV(XGMAC_ref_dig_fr_clk_sel, value)

/* XGMAC_SBD_CFG */
#define XGMAC_sbd_flowctrl_OFFSET	5
#define XGMAC_sbd_flowctrl_MASK		REG_8BIT_MASK
#define XGMAC_sbd_flowctrl(value)	_SET_FV(XGMAC_sbd_flowctrl, value)
#define XGMAC_sbd_lpitxen		_BV(4)
#define XGMAC_sbd_lpi_link_sts		_BV(3)
#define XGMAC_sbd_speed_sel_OFFSET	0
#define XGMAC_sbd_speed_sel_MASK	REG_3BIT_MASK
#define XGMAC_sbd_speed_sel(value)	_SET_FV(XGMAC_sbd_speed_sel, value)

/* XGMAC_STATUS */
#define XGMAC_mtl_tx_active		_BV(13)
#define XGMAC_mtl_rx_active		_BV(12)
#define XGMAC_mti_tx_pause		_BV(11)
#define XGMAC_mti_disable_priority_OFFSET	3
#define XGMAC_mti_disable_priority_MASK		REG_8BIT_MASK
#define XGMAC_mti_disable_priority(value)	_SET_FV(XGMAC_mti_disable_priority, value)
#define XGMAC_eth_pgc_pg_ack		_BV(2)
#define XGMAC_ether_d3_ack		_BV(1)
#define XGMAC_loopback_mode		_BV(0)

/* E16PHY_CFG */
#define E16PHY_vph_nominal_OFFSET	3
#define E16PHY_vph_nominal_MASK		REG_2BIT_MASK
#define E16PHY_vph_nominal(value)	_SET_FV(E16PHY_vph_nominal, value)
#define E16PHY_vph_1_5V			0x2
#define E16PHY_vph_1_8V			0x3
#define E16PHY_mplla_force_en		_BV(2)
#define E16PHY_mpllb_force_en		_BV(1)
#define E16PHY_sram_bypass		_BV(0)

/* E16PHY_ADAPT_CFG */
#define E16PHY_rx3_adapt_in_prog	_BV(3)
#define E16PHY_rx2_adapt_in_prog	_BV(2)
#define E16PHY_rx1_adapt_in_prog	_BV(1)
#define E16PHY_rx0_adapt_in_prog	_BV(0)

/* E16PHY_STATUS */
#define E16PHY_mplla_force_ack		_BV(3)
#define E16PHY_mpllb_force_ack		_BV(2)
#define E16PHY_pcs_pwr_en		_BV(1)
#define E16PHY_pma_pwr_en		_BV(0)

#ifdef CONFIG_DUOWEN_SBI_DUAL
#if 0
#define DW_XGMAC_BASE(soc)	(__SOC_BASE(soc) + XGMAC_BASE)
#define DW_XPCS_BASE(soc)	(__SOC_BASE(soc) + XPCS_BASE)
#define DW_XGMAC_CLK_SRC(soc)	((soc) ? sysfab_clk2 : sysfab_clk)
#endif
#define DW_XGMAC_BASE		XGMAC_BASE
#define DW_XPCS_BASE		XPCS_BASE
#define DW_XGMAC_CLK_SRC	sysfab_clk
#else /* CONFIG_DUOWEN_SBI_DUAL */
#define DW_XGMAC_BASE		XGMAC_BASE
#define DW_XPCS_BASE		XPCS_BASE
#define DW_XGMAC_CLK_SRC	sysfab_clk
#endif /* CONFIG_DUOWEN_SBI_DUAL */

#define DW_XPCS_PHY_ADDR	0
#define DW_XPCS_APBI_OFFSET	1
#ifdef CONFIG_DUOWEN_ETH_10000BASE_KX4
#define DW_XPCS_PMA_TYPE	SR_PMA_10000BASE_KX4
#define DW_XPCS_PCS_TYPE	SR_PCS_10000BASE_X
#endif
#ifdef CONFIG_DUOWEN_ETH_10000BASE_KR
#define DW_XPCS_PMA_TYPE	SR_PMA_10000BASE_KR
#define DW_XPCS_PCS_TYPE	SR_PCS_10000BASE_R
#endif

#include <driver/dw_xgmac.h>
#include <driver/dw_xpcs.h>

#ifdef CONFIG_DUOWEN_ETH_10000BASE_KX4
#define CTLE_BOOST_0_LEVEL	6
#define TX_EQ_POST_LEVEL	0
#define RX3_DELTA_IQ_VAL	0
#define RX2_DELTA_IQ_VAL	0
#define RX1_DELTA_IQ_VAL	0
#define RX0_DELTA_IQ_VAL	0
#define VCO_FRQBAND_VAL		2
#define RX_DFE_BYP_3_1_VAL	7
#define RX_DFE_BYP_0_VAL	RX_DFE_BYP_0
#define RX3_MISC_VAL		7
#define RX2_MISC_VAL		7
#define RX1_MISC_VAL		7
#define RX0_MISC_VAL		7
#define TX_EQ_MAIN_AMPLITUDE	40
#define MPLLA_DIV_CLK_EN	2
#define WIDTH_BITS		WIDTH_10BIT
#define BAUD_DIV4		2
#define MPLLA_MULTIPLIER	40
#define VCO_LD_VAL_0		1360
#define VCO_LD_VAL_1		1360
#define VCO_LD_VAL_2		1360
#define VCO_LD_VAL_3		1360
#define RX_ADPT_MODE_VAL	0
#define VCO_REF_LD_1_VAL	0x22
#define VCO_REF_LD_0_VAL	0x22
#define VCO_REF_LD_3_VAL	0x22
#define VCO_REF_LD_2_VAL	0x22
#endif

#ifdef CONFIG_DUOWEN_ETH_10000BASE_RXAUI
#define CTLE_BOOST_0_LEVEL	6
#define TX_EQ_POST_LEVEL	0x28
#define RX3_DELTA_IQ_VAL	0
#define RX2_DELTA_IQ_VAL	0
#define RX1_DELTA_IQ_VAL	0
#define RX0_DELTA_IQ_VAL	0
#define VCO_FRQBAND_VAL		2
#define RX_DFE_BYP_3_1_VAL	0
#define RX_DFE_BYP_0_VAL	0
#define RX3_MISC_VAL		5
#define RX2_MISC_VAL		5
#define RX1_MISC_VAL		5
#define RX0_MISC_VAL		5
#define TX_EQ_MAIN_AMPLITUDE	30
#define MPLLA_DIV_CLK_EN	2
#define WIDTH_BITS		WIDTH_20BIT
#define BAUD_DIV4		1
#define MPLLA_MULTIPLIER	40
#define VCO_LD_VAL_0		1360
#define VCO_LD_VAL_1		1360
#define VCO_LD_VAL_2		1360
#define VCO_LD_VAL_3		1360
#define RX_ADPT_MODE_VAL	3
#define VCO_REF_LD_1_VAL	0x22
#define VCO_REF_LD_0_VAL	0x22
#define VCO_REF_LD_3_VAL	0x22
#define VCO_REF_LD_2_VAL	0x22
#endif

#ifdef CONFIG_DUOWEN_ETH_SGMII_1000M
#define CTLE_BOOST_0_LEVEL	6
#define TX_EQ_POST_LEVEL	0
#define RX0_DELTA_IQ_VAL	0
#define VCO_FRQBAND_VAL		1
#define RX_DFE_BYP_0_VAL	RX_DFE_BYP_0
#define RX0_MISC_VAL		6
#define TX_EQ_MAIN_AMPLITUDE	40
#define MPLLA_DIV_CLK_EN	2
#define WIDTH_BITS		WIDTH_10BIT
#define BAUD_DIV4		3
#define MPLLA_MULTIPLIER	32
#define VCO_LD_VAL_0		1344
#define RX_ADPT_MODE_VAL	0
#define VCO_REF_LD_1_VAL	0x2a
#define VCO_REF_LD_0_VAL	0x2a
#endif

#ifdef CONFIG_DUOWEN_ETH_USXGMII_AN37
#define CTLE_BOOST_0_LEVEL	0xa
#define RX0_DELTA_IQ_VAL	3
#define VCO_FRQBAND_VAL		1
#define RX_DFE_BYP_3_1_VAL	0
#define RX_DFE_BYP_0_VAL	0
#define RX1_MISC_VAL		5
#define RX0_MISC_VAL		2
#define MPLLA_DIV_CLK_EN	6
#define WIDTH_BITS		WIDTH_20BIT
#define BAUD_DIV4		0
#define MPLLA_MULTIPLIER	33
#define VCO_LD_VAL_0		1353
#define RX_ADPT_MODE_VAL	3
#define VCO_REF_LD_1_VAL	0x29
#define VCO_REF_LD_0_VAL	0x29
#endif

#define dw_xpcs_hw_sram_init()				\
	do {						\
		dw_xpcs_int_sram_init();		\
		dw_xpcs_ext_sram_init();		\
	} while (0)

#ifdef CONFIG_DUOWEN_ETH_LOOPBACK
#define dw_xpcs_hw_ctrl_init()		dw_xpcs_enable_loopback()
#else
#define dw_xpcs_hw_ctrl_init()		do { } while (0)
#endif

#define duowen_eth_clock_init()				\
	do {						\
		clk_enable(eth_alt_ref_clk);		\
		clk_enable(sgmii_ref_clk);		\
	} while (0)
#define duowen_eth_gpio_cfg(pin, pad, func)		\
	do {						\
		gpio_config_pad(GPIO2C, pin, pad, 8);	\
		gpio_config_mux(GPIO2C, pin, func);	\
	} while (0)
#define duowen_eth_gpio_init()				\
	do {						\
		duowen_eth_gpio_cfg(pad_gpio_144,	\
				    GPIO_PAD_PULL_DOWN,	\
				    TLMM_PAD_FUNCTION);	\
		duowen_eth_gpio_cfg(pad_gpio_145,	\
				    GPIO_PAD_PULL_DOWN,	\
				    TLMM_PAD_FUNCTION);	\
		duowen_eth_gpio_cfg(pad_gpio_146,	\
				    GPIO_PAD_PULL_DOWN,	\
				    TLMM_PAD_FUNCTION);	\
		duowen_eth_gpio_cfg(pad_gpio_147,	\
				    GPIO_PAD_PULL_DOWN,	\
				    TLMM_PAD_FUNCTION);	\
		duowen_eth_gpio_cfg(pad_gpio_148,	\
				    GPIO_PAD_PULL_DOWN,	\
				    TLMM_PAD_FUNCTION);	\
	} while (0)

#ifdef CONFIG_DUOWEN_ETH
#define duowen_eth_init()				\
	do {						\
		duowen_eth_gpio_init();			\
		duowen_eth_clock_init();		\
		dw_xgmac_init_ctrl();			\
	} while (0)
#define eth_hw_mdio_read(a, r, d)	dw_xgmac_mdio_read(a, r, d)
#define eth_hw_mdio_write(a, r, d)	dw_xgmac_mdio_write(a, r, d)
#else
#define duowen_eth_init()				\
	do {						\
		duowen_eth_gpio_init();			\
		duowen_eth_clock_init();		\
	} while (0)
#endif

#ifdef CONFIG_DUOWEN_ETH_INIT
#define duowen_eth_late_init()		duowen_eth_init()
#else
#define duowen_eth_late_init()		do { } while (0)
#endif

#endif /* __ETH_DUOWEN_H_INCLUDE__ */
