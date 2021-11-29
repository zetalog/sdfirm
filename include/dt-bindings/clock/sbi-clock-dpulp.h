/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 *  Copyright (C) 2021 CAS SmartCore Co., Ltd.
 *    Author: 2021 Lv Zheng <zhenglv@smart-core.cn>
 */

#ifndef __DT_BINDINGS_CLOCK_SBI_DPULP_H
#define __DT_BINDINGS_CLOCK_SBI_DPULP_H

#include <dt-bindings/clock/sbi-clock.h>

#define XO_CLK_FREQ		25000000 /* 25MHz */
#define CPU_CLK_FREQ		1000000000 /* PLL0 CPU/RAM 1GHz */
#define GPDPU_BUS_CLK_FREQ	1200000000 /* PLL1 GPDPU bus 1.2GHz */
#define GPDPU_CORE_CLK_FREQ	1200000000 /* PLL2 GPDPU core 1.2GHz */
#define DDR_CLK_FREQ		800000000 /* PLL3 DDR 800MHz */
#define AXI_CLK_FREQ		SOC_800_CLK_FREQ /* PLL4 P AXI 800MHz */
#define APB_CLK_FREQ		SOC_100_CLK_FREQ /* PLL4 R APB 100MHz */
#define VPU_BCLK_FREQ		1000000000 /* PLL5 VPU B 1GHz */
#define VPU_CCLK_FREQ		1100000000 /* PLL6 VPU C 1.1GHz */
#ifdef CONFIG_DPULP_RAB0_156250KHZ
#define RAB0_PHY_CLK_FREQ	156250000 /* PLL7 RAB0 156.25MHz */
#endif /* CONFIG_DPULP_RAB0_156250KHZ */
#ifdef CONFIG_DPULP_RAB0_125000KHZ
#define RAB0_PHY_CLK_FREQ	125000000 /* PLL7 RAB0 125.00MHz */
#endif /* CONFIG_DPULP_RAB0_125000KHZ */
#ifdef CONFIG_DPULP_RAB0_78125KHZ
#define RAB0_PHY_CLK_FREQ	78125000 /* PLL7 RAB0 78.125MHz */
#endif /* CONFIG_DPULP_RAB0_78125KHZ */
#ifdef CONFIG_DPULP_RAB0_62500KHZ
#define RAB0_PHY_CLK_FREQ	62500000 /* PLL7 RAB0 62.500MHz */
#endif /* CONFIG_DPULP_RAB0_62500KHZ */
#ifndef RAB0_PHY_CLK_FREQ
#define RAB0_PHY_CLK_FREQ	156250000 /* PLL7 RAB0 156.250MHz */
#endif /* RAB0_PHY_CLK_FREQ */
#ifdef CONFIG_DPULP_RAB1_156250KHZ
#define RAB1_PHY_CLK_FREQ	156250000 /* PLL8 RAB1 156.25MHz */
#endif /* CONFIG_DPULP_RAB1_156250KHZ */
#ifdef CONFIG_DPULP_RAB1_125000KHZ
#define RAB1_PHY_CLK_FREQ	125000000 /* PLL8 RAB1 125.00MHz */
#endif /* CONFIG_DPULP_RAB1_125000KHZ */
#ifdef CONFIG_DPULP_RAB1_78125KHZ
#define RAB1_PHY_CLK_FREQ	78125000 /* PLL8 RAB0 78.125MHz */
#endif /* CONFIG_DPULP_RAB1_78125KHZ */
#ifdef CONFIG_DPULP_RAB1_62500KHZ
#define RAB1_PHY_CLK_FREQ	62500000 /* PLL8 RAB1 62.500MHz */
#endif /* CONFIG_DPULP_RAB1_62500KHZ */
#ifndef RAB1_PHY_CLK_FREQ
#define RAB1_PHY_CLK_FREQ	156250000 /* PLL8 RAB0 156.250MHz */
#endif /* RAB1_PHY_CLK_FREQ */
#define SOC_800_CLK_FREQ	800000000 /* SoC 800MHz */
#define SOC_400_CLK_FREQ	400000000 /* SoC 400MHz */
#define SOC_200_CLK_FREQ	200000000 /* SoC 200MHz */
#define SOC_100_CLK_FREQ	100000000 /* SoC 100MHz */
#ifdef CONFIG_DPULP_ETH0_156250KHZ
#define ETH0_PHY_CLK_FREQ	156250000 /* PLL9 ETH0 156.25MHz */
#endif /* CONFIG_DPULP_ETH0_156250KHZ */
#ifdef CONFIG_DPULP_ETH0_78125KHZ
#define ETH0_PHY_CLK_FREQ	78125000 /* PLL9 ETH0 78.125MHz */
#endif /* CONFIG_DPULP_ETH0_78125KHZ */
#ifndef ETH0_PHY_CLK_FREQ
#define ETH0_PHY_CLK_FREQ	156250000 /* PLL9 ETH0 156.250MHz */
#endif /* ETH0_PHY_CLK_FREQ */
#ifdef CONFIG_DPULP_ETH1_156250KHZ
#define ETH1_PHY_CLK_FREQ	156250000 /* PLL10 ETH1 156.25MHz */
#endif /* CONFIG_DPULP_ETH1_156250KHZ */
#ifdef CONFIG_DPULP_ETH1_78125KHZ
#define ETH1_PHY_CLK_FREQ	78125000 /* PLL10 ETH1 78.125MHz */
#endif /* CONFIG_DPULP_ETH1_78125KHZ */
#ifndef ETH1_PHY_CLK_FREQ
#define ETH1_PHY_CLK_FREQ	156250000 /* PLL10 ETH1 156.250MHz */
#endif /* ETH1_PHY_CLK_FREQ */

#ifdef CONFIG_DPULP_UART_ACCEL
#define UART_CURRENT_SPEED	(APB_CLK_FREQ/16)
#else /* CONFIG_DPULP_UART_ACCEL */
#ifdef CONFIG_CONSOLE_4000000
#define UART_CURRENT_SPEED	4000000
#endif
#ifdef CONFIG_CONSOLE_3500000
#define UART_CURRENT_SPEED	3500000
#endif
#ifdef CONFIG_CONSOLE_3000000
#define UART_CURRENT_SPEED	3000000
#endif
#ifdef CONFIG_CONSOLE_2500000
#define UART_CURRENT_SPEED	2500000
#endif
#ifdef CONFIG_CONSOLE_2000000
#define UART_CURRENT_SPEED	2000000
#endif
#ifdef CONFIG_CONSOLE_1500000
#define UART_CURRENT_SPEED	1500000
#endif
#ifdef CONFIG_CONSOLE_1000000
#define UART_CURRENT_SPEED	1000000
#endif
#ifdef CONFIG_CONSOLE_500000
#define UART_CURRENT_SPEED	500000
#endif
#ifdef CONFIG_CONSOLE_1152000
#define UART_CURRENT_SPEED	1152000
#endif
#ifdef CONFIG_CONSOLE_921600
#define UART_CURRENT_SPEED	921600
#endif
#ifdef CONFIG_CONSOLE_576000
#define UART_CURRENT_SPEED	576000
#endif
#ifdef CONFIG_CONSOLE_460800
#define UART_CURRENT_SPEED	460800
#endif
#ifdef CONFIG_CONSOLE_230400
#define UART_CURRENT_SPEED	230400
#endif
#ifdef CONFIG_CONSOLE_115200
#define UART_CURRENT_SPEED	115200
#endif
#ifdef CONFIG_CONSOLE_76800
#define UART_CURRENT_SPEED	76800
#endif
#ifdef CONFIG_CONSOLE_57600
#define UART_CURRENT_SPEED	57600
#endif
#ifdef CONFIG_CONSOLE_38400
#define UART_CURRENT_SPEED	38400
#endif
#ifdef CONFIG_CONSOLE_19200
#define UART_CURRENT_SPEED	19200
#endif
#ifdef CONFIG_CONSOLE_9600
#define UART_CURRENT_SPEED	9600
#endif
#ifdef CONFIG_CONSOLE_4800
#define UART_CURRENT_SPEED	4800
#endif
#ifdef CONFIG_CONSOLE_2400
#define UART_CURRENT_SPEED	2400
#endif
#endif /* CONFIG_DPULP_UART_ACCEL */

#define I2C_MASTER_SPEED	400000

/* PLL IDs is kept AS IS to make base addresses simpler. */
#define CPU_PLL			0
#define GPDPU_BUS_PLL		1
#define GPDPU_CORE_PLL		2
#define DDR_PLL			3
#define SOC_PLL			4
#define VPU_B_PLL		5
#define VPU_C_PLL		6
#define RAB0_PHY_PLL		7
#define RAB1_PHY_PLL		8
#define ETH0_PHY_PLL		9
#define ETH1_PHY_PLL		10
#define DPULP_MAX_PLLS		11

#define CLK_REF			0
#define CLK_VCO			1
#define CLK_PLL			2
#define CLK_SEL			3
#define CLK_DIV			4
#define CLK_RST			5

/* CLK_REF */
#define XO_CLK			0
#define TIC_CLK			1
#define JTAG_CLK		2
#define NR_REF_CLKS		(JTAG_CLK + 1)
#define xo_clk			clkid(CLK_REF, XO_CLK)
#define tic_clk			clkid(CLK_REF, TIC_CLK)
#define jtag_clk		clkid(CLK_REF, JTAG_CLK)

/* CLK_VCO */
#define CPU_VCO			CPU_PLL
#define GPDPU_BUS_VCO		GPDPU_BUS_PLL
#define GPDPU_CORE_VCO		GPDPU_CORE_PLL
#define DDR_VCO			DDR_PLL
#define SOC_VCO			SOC_PLL
#define VPU_B_VCO		VPU_B_PLL
#define VPU_C_VCO		VPU_C_PLL
#define RAB0_PHY_VCO		RAB0_PHY_PLL
#define RAB1_PHY_VCO		RAB1_PHY_PLL
#define ETH0_PHY_VCO		ETH0_PHY_PLL
#define ETH1_PHY_VCO		ETH1_PHY_PLL
#define NR_VCO_CLKS		DPULP_MAX_PLLS
#define cpu_vco			clkid(CLK_VCO, CPU_VCO)
#define gpdpu_bus_vco		clkid(CLK_VCO, GPDPU_BUS_VCO)
#define gpdpu_core_vco		clkid(CLK_VCO, GPDPU_CORE_VCO)
#define ddr_vco			clkid(CLK_VCO, DDR_VCO)
#define soc_vco			clkid(CLK_VCO, SOC_VCO)
#define vpu_b_vco		clkid(CLK_VCO, VPU_B_VCO)
#define vpu_c_vco		clkid(CLK_VCO, VPU_C_VCO)
#define rab0_phy_vco		clkid(CLK_VCO, RAB0_PHY_VCO)
#define rab1_phy_vco		clkid(CLK_VCO, RAB1_PHY_VCO)
#define eth0_phy_vco		clkid(CLK_VCO, ETH0_PHY_VCO)
#define eth1_phy_vco		clkid(CLK_VCO, ETH1_PHY_VCO)

/* CLK_PLL */
/* P PLL_OUT clocks use same name as PLLs, R PLL_OUT clocks are defined
 * here.
 */
#define AXI_PLL			SOC_PLL
#define APB_PLL			(DPULP_MAX_PLLS + SOC_PLL)
#define NR_PLL_CLKS		(DPULP_MAX_PLLS * 2)
#define cpu_pll			clkid(CLK_PLL, CPU_PLL)
#define gpdpu_bus_pll		clkid(CLK_PLL, GPDPU_BUS_PLL)
#define gpdpu_core_pll		clkid(CLK_PLL, GPDPU_CORE_PLL)
#define ddr_pll			clkid(CLK_PLL, DDR_PLL)
#define axi_pll			clkid(CLK_PLL, AXI_PLL)
#define vpu_b_pll		clkid(CLK_PLL, VPU_B_PLL)
#define vpu_c_pll		clkid(CLK_PLL, VPU_C_PLL)
#define rab0_phy_pll		clkid(CLK_PLL, RAB0_PHY_PLL)
#define rab1_phy_pll		clkid(CLK_PLL, RAB1_PHY_PLL)
#define eth0_phy_pll		clkid(CLK_PLL, ETH0_PHY_PLL)
#define eth1_phy_pll		clkid(CLK_PLL, ETH1_PHY_PLL)
#define apb_pll			clkid(CLK_PLL, APB_PLL)

/* CLK_SEL: CRU_CLK_CFG */
#define CPU_CLKSEL		0
#define GPDPU_BUS_CLKSEL	1
#define GPDPU_CORE_CLKSEL	2
#define DDR_CLKSEL		3
#define SOC_800_CLKSEL		4
#define SOC_400_CLKSEL		5
#define SOC_200_CLKSEL		6
#define SOC_100_CLKSEL		7
#define VPU_CCLKSEL		8
#define VPU_BCLKSEL		9
#define RAB0_PHY_CLKSEL		10
#define RAB1_PHY_CLKSEL		11
#define NR_SEL_CLKS		(RAB1_PHY_CLKSEL + 1)
#define cpu_clksel		clkid(CLK_SEL, CPU_CLKSEL)
#define gpdpu_bus_clksel	clkid(CLK_SEL, GPDPU_BUS_CLKSEL)
#define gpdpu_core_clksel	clkid(CLK_SEL, GPDPU_CORE_CLKSEL)
#define ddr_clksel		clkid(CLK_SEL, DDR_CLKSEL)
#define soc_800_clksel		clkid(CLK_SEL, SOC_800_CLKSEL)
#define soc_400_clksel		clkid(CLK_SEL, SOC_400_CLKSEL)
#define soc_200_clksel		clkid(CLK_SEL, SOC_200_CLKSEL)
#define soc_100_clksel		clkid(CLK_SEL, SOC_100_CLKSEL)
#define vpu_cclksel		clkid(CLK_SEL, VPU_CCLKSEL)
#define vpu_bclksel		clkid(CLK_SEL, VPU_BCLKSEL)
#define rab0_phy_clksel		clkid(CLK_SEL, RAB0_PHY_CLKSEL)
#define rab1_phy_clksel		clkid(CLK_SEL, RAB1_PHY_CLKSEL)

/* CLK_DIV */
#define SOC_PLL_DIV2		0
#define SOC_PLL_DIV4		0
#define NR_DIV_CLKS		(SOC_PLL_DIV4 + 1)
#define soc_pll_div2		clkid(CLK_DIV, SOC_PLL_DIV2)
#define soc_pll_div4		clkid(CLK_DIV, SOC_PLL_DIV4)

/* CLK_RST: CRU_CLK_CFG/CRU_RESET */
#define CPU_FUNC_RESET		0
#define CPU_DBG_RESET		1
#define GPDPU_RESET		2
#define DDR0_PWROKIN		3
#define DDR0_APB_RESET		4
#define DDR0_AXI_RESET		5
#define DDR0_RESET		6
#define DDR1_PWROKIN		7
#define DDR1_APB_RESET		8
#define DDR1_AXI_RESET		9
#define DDR1_RESET		10
#define VPU_RESET		11
#define PCIE_PWR_UP_RESET	12
#define RAB0_RESET		13
#define RAB0_RIO_RESET		14
#define RAB0_RIO_LOGIC_RESET	15
#define RAB1_RESET		16
#define RAB1_RIO_RESET		17
#define RAB1_RIO_LOGIC_RESET	18
#define ETH0_RESET		19
#define ETH1_RESET		20
#define GPIO_RESET		21
#define SSI_RESET		22
#define FLASH_RESET		23
#define I2C0_RESET		24
#define I2C1_RESET		25
#define I2C2_RESET		26
#define I2C3_RESET		27
#define UART0_RESET		28
#define UART1_RESET		29
#define UART2_RESET		30
#define UART3_RESET		31
#define SD_RESET		32
#define PLIC_RESET		33
#define TMR_RESET		34
#define WDT_RESET		35
#define AON_TSENSOR_RESET	36
#define GPDPU_TSENSOR_RESET	37
#define DDR0_TSENSOR_RESET	38
#define DDR1_TSENSOR_RESET	39
#define RAB0_TSENSOR_RESET	40
#define RAB1_TSENSOR_RESET	41
#define ETH0_TSENSOR_RESET	42
#define ETH1_TSENSOR_RESET	43
#define TCSR_RESET		44
#define GPDPU_CLKEN		48
#define DDR0_CLKEN		49
#define DDR1_CLKEN		50
#define DDR_BYPASSPCLKEN	51
#define VPU0_CLKEN		52
#define VPU1_CLKEN		53
#define RAB0_CLKEN		54
#define RAB1_CLKEN		55
#define PCIE_CLKEN		56
#define NR_RST_CLKS		(PCIE_CLKEN + 1)
#define cpu_func_reset		clkid(CLK_RST, CPU_FUNC_RESET)
#define cpu_dbg_reset		clkid(CLK_RST, CPU_DBG_RESET)
#define gpdpu_reset		clkid(CLK_RST, GPDPU_RESET)
#define ddr0_pwrokin		clkid(CLK_RST, DDR0_PWROKIN)
#define ddr0_apb_reset		clkid(CLK_RST, DDR0_APB_RESET)
#define ddr0_axi_reset		clkid(CLK_RST, DDR0_AXI_RESET)
#define ddr0_reset		clkid(CLK_RST, DDR0_RESET)
#define ddr1_pwrokin		clkid(CLK_RST, DDR1_PWROKIN)
#define ddr1_apb_reset		clkid(CLK_RST, DDR1_APB_RESET)
#define ddr1_axi_reset		clkid(CLK_RST, DDR1_AXI_RESET)
#define ddr1_reset		clkid(CLK_RST, DDR1_RESET)
#define vpu_reset		clkid(CLK_RST, VPU_RESET)
#define pcie_pwr_up_reset	clkid(CLK_RST, PCIE_PWR_UP_RESET)
#define rab0_reset		clkid(CLK_RST, RAB0_RESET)
#define rab0_rio_reset		clkid(CLK_RST, RAB0_RIO_RESET)
#define rab0_rio_logic_reset	clkid(CLK_RST, RAB0_RIO_LOGIC_RESET)
#define rab1_reset		clkid(CLK_RST, RAB1_RESET)
#define rab1_rio_reset		clkid(CLK_RST, RAB1_RIO_RESET)
#define rab1_rio_logic_reset	clkid(CLK_RST, RAB1_RIO_LOGIC_RESET)
#define eth0_reset		clkid(CLK_RST, ETH0_RESET)
#define eth1_reset		clkid(CLK_RST, ETH1_RESET)
#define gpio_reset		clkid(CLK_RST, GPIO_RESET)
#define ssi_reset		clkid(CLK_RST, SSI_RESET)
#define flash_reset		clkid(CLK_RST, FLASH_RESET)
#define i2c0_reset		clkid(CLK_RST, I2C0_RESET)
#define i2c1_reset		clkid(CLK_RST, I2C1_RESET)
#define i2c2_reset		clkid(CLK_RST, I2C2_RESET)
#define i2c3_reset		clkid(CLK_RST, I2C3_RESET)
#define uart0_reset		clkid(CLK_RST, UART0_RESET)
#define uart1_reset		clkid(CLK_RST, UART1_RESET)
#define uart2_reset		clkid(CLK_RST, UART2_RESET)
#define uart3_reset		clkid(CLK_RST, UART3_RESET)
#define sd_reset		clkid(CLK_RST, SD_RESET)
#define plic_reset		clkid(CLK_RST, PLIC_RESET)
#define tmr_reset		clkid(CLK_RST, TMR_RESET)
#define wdt_reset		clkid(CLK_RST, WDT_RESET)
#define aon_tsensor_reset	clkid(CLK_RST, AON_TSENSOR_RESET)
#define gpdpu_tsensor_reset	clkid(CLK_RST, GPDPU_TSENSOR_RESET)
#define ddr0_tsensor_reset	clkid(CLK_RST, DDR0_TSENSOR_RESET)
#define ddr1_tsensor_reset	clkid(CLK_RST, DDR1_TSENSOR_RESET)
#define rab0_tsensor_reset	clkid(CLK_RST, RAB0_TSENSOR_RESET)
#define rab1_tsensor_reset	clkid(CLK_RST, RAB1_TSENSOR_RESET)
#define eth0_tsensor_reset	clkid(CLK_RST, ETH0_TSENSOR_RESET)
#define tcsr_reset		clkid(CLK_RST, TCSR_RESET)
#define gpdpu_clk		clkid(CLK_RST, GPDPU_CLKEN)
#define ddr0_clk		clkid(CLK_RST, DDR0_CLKEN)
#define ddr1_clk		clkid(CLK_RST, DDR1_CLKEN)
#define ddr_bypassPclk		clkid(CLK_RST, DDR_BYPASSPCLKEN)
#define vpu0_clk		clkid(CLK_RST, VPU0_CLKEN)
#define vpu1_clk		clkid(CLK_RST, VPU1_CLKEN)
#define rab0_clk		clkid(CLK_RST, RAB0_CLKEN)
#define rab1_clk		clkid(CLK_RST, RAB1_CLKEN)
#define pcie_clk		clkid(CLK_RST, PCIE_CLKEN)

#define pcie_aux_clk		xo_clk
#define cpu_clk			cpu_func_reset

/* Linux clk-duowen bindings */
#define sbi_cpu_clk		0
#define sbi_max_clk		1

#endif /* __DT_BINDINGS_CLOCK_SBI_DPULP_H */
