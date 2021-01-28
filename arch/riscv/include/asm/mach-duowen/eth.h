#ifndef __ETH_DUOWEN_H_INCLUDE__
#define __ETH_DUOWEN_H_INCLUDE__

#include <target/clk.h>

#define DW_XGMAC_BASE		XGMAC_BASE
#define DW_XGMAC_CLK_SRC	sysfab_clk

#define DW_XPCS_BASE		XPCS_BASE
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
