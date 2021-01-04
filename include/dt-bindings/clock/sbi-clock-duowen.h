/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 *  Copyright (C) 2021 CAS SmartCore Co., Ltd.
 *    Author: 2021 Lv Zheng <zhenglv@smarco.cn>
 */

#ifndef __DT_BINDINGS_CLOCK_SBI_DUOWEN_H
#define __DT_BINDINGS_CLOCK_SBI_DUOWEN_H

#include <dt-bindings/clock/sbi-clock.h>

/* PLL IDs is kept AS IS to make base addresses simpler, see
 * COHFAB_CLK_PLL() and COHFAB_CLK_SEL_PLL() for details.
 */
#define SOC_PLL			0
#define DDR_BUS_PLL		1
#define DDR_PLL			2
#define PCIE_PLL		3
#define COHFAB_PLL		4
#define CL0_PLL			5
#define CL1_PLL			6
#define CL2_PLL			7
#define CL3_PLL			8
#define ETH_PLL			9
#define DUOWEN_MAX_PLLS		10

#ifdef CONFIG_DUOWEN_SOCv1
#include <dt-bindings/clock/sbi-clock-duowen-socv1.h>
#endif
#ifdef CONFIG_DUOWEN_SOCv2
#include <dt-bindings/clock/sbi-clock-duowen-socv2.h>
#endif

#endif /* __DT_BINDINGS_CLOCK_SBI_DUOWEN_H */
