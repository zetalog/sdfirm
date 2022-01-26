#ifndef __ETH_DPULP_H_INCLUDE__
#define __ETH_DPULP_H_INCLUDE__

#include <target/clk.h>

#define DW_XGMAC_BASE		XGMAC0_BASE
#define DW_XPCS_BASE		XPCS0_BASE
#define DW_XGMAC_CLK_SRC	sysfab_clk

#define DW_XPCS_PHY_ADDR	0
#define DW_XPCS_APBI_OFFSET	1
#ifdef CONFIG_DPULP_ETH_10000BASE_KX4
#define DW_XPCS_PMA_TYPE	SR_PMA_10000BASE_KX4
#define DW_XPCS_PCS_TYPE	SR_PCS_10000BASE_X
#endif
#ifdef CONFIG_DPULP_ETH_10000BASE_KR
#define DW_XPCS_PMA_TYPE	SR_PMA_10000BASE_KR
#define DW_XPCS_PCS_TYPE	SR_PCS_10000BASE_R
#endif

#include <driver/dw_xgmac.h>
#include <driver/dw_xpcs.h>

#ifdef CONFIG_DPULP_ETH_10000BASE_KX4
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

#ifdef CONFIG_DPULP_ETH_10000BASE_RXAUI
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

#ifdef CONFIG_DPULP_ETH_SGMII_1000M
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

#ifdef CONFIG_DPULP_ETH_USXGMII_AN37
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

#ifdef CONFIG_DPULP_ETH_LOOPBACK
#define dw_xpcs_hw_ctrl_init()		dw_xpcs_enable_loopback()
#else
#define dw_xpcs_hw_ctrl_init()		do { } while (0)
#endif

#define dpulp_eth_clock_init()				\
	do {						\
		clk_enable(eth_alt_ref_clk);		\
		clk_enable(sgmii_ref_clk);		\
	} while (0)
#define dpulp_eth_gpio_cfg(pin, pad, func)		\
	do {						\
		gpio_config_pad(GPIO2C, pin, pad, 8);	\
		gpio_config_mux(GPIO2C, pin, func);	\
	} while (0)
#define dpulp_eth_gpio_init()				\
	do {						\
		dpulp_eth_gpio_cfg(pad_gpio_144,	\
				    GPIO_PAD_PULL_DOWN,	\
				    TLMM_PAD_FUNCTION);	\
		dpulp_eth_gpio_cfg(pad_gpio_145,	\
				    GPIO_PAD_PULL_DOWN,	\
				    TLMM_PAD_FUNCTION);	\
		dpulp_eth_gpio_cfg(pad_gpio_146,	\
				    GPIO_PAD_PULL_DOWN,	\
				    TLMM_PAD_FUNCTION);	\
		dpulp_eth_gpio_cfg(pad_gpio_147,	\
				    GPIO_PAD_PULL_DOWN,	\
				    TLMM_PAD_FUNCTION);	\
		dpulp_eth_gpio_cfg(pad_gpio_148,	\
				    GPIO_PAD_PULL_DOWN,	\
				    TLMM_PAD_FUNCTION);	\
	} while (0)

#ifdef CONFIG_DPULP_ETH
#define dpulp_eth_init()				\
	do {						\
		dpulp_eth_gpio_init();			\
		dpulp_eth_clock_init();		\
		dw_xgmac_init_ctrl();			\
	} while (0)
#define eth_hw_mdio_read(a, r, d)	dw_xgmac_mdio_read(a, r, d)
#define eth_hw_mdio_write(a, r, d)	dw_xgmac_mdio_write(a, r, d)
#else
#define dpulp_eth_init()				\
	do {						\
		dpulp_eth_gpio_init();			\
		dpulp_eth_clock_init();		\
	} while (0)
#endif

#ifdef CONFIG_DPULP_ETH_INIT
#define dpulp_eth_late_init()		dpulp_eth_init()
#else
#define dpulp_eth_late_init()		do { } while (0)
#endif

#endif /* __ETH_DPULP_H_INCLUDE__ */
