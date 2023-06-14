/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 *  Copyright (C) 2022 CAS SmartCore Co., Ltd.
 *    Author: 2022 Lv Zheng <zhenglv@smart-core.cn>
 */

#ifndef __DT_BINDINGS_CLOCK_SBI_K1MAX_H
#define __DT_BINDINGS_CLOCK_SBI_K1MAX_H

#include <dt-bindings/clock/sbi-clock.h>

#ifdef CONFIG_K1M_CPU
#define CPU_CLK_FREQ		2000000000
#define APB_CLK_FREQ		2000000000
#define PIC_CLK_FREQ		2000000000
#endif /* CONFIG_K1M_CPU */

#ifdef CONFIG_K1M_SOC
#ifdef CONFIG_K1M_S2C
#define PAD_CLK_FREQ		50000000
#define APB_CLK_FREQ		PAD_CLK_FREQ
#ifdef CONFIG_K1M_S2C_SPEEDUP
#define CPU_CLK_FREQ		100000000
#else /* CONFIG_K1M_S2C_SPEEDUP */
#define CPU_CLK_FREQ		PAD_CLK_FREQ
#endif /* CONFIG_K1M_S2C_SPEEDUP */
#define PIC_CLK_FREQ		PAD_CLK_FREQ
#else /* CONFIG_K1M_S2C */
#ifdef CONFIG_K1M_K1X
#define CPU_CLK_FREQ		1500000000
#define CPU_CLK_FREQ		1500000000
#define PIC_CLK_FREQ		(2*(CPU_CLK_FREQ)/5)
#define APB_CLK_FREQ		(2*(CPU_CLK_FREQ)/5)
#endif /* CONFIG_K1M_K1X */
#ifdef CONFIG_K1M_K1PRO
/* Unknown, use CPU bench */
#define CPU_CLK_FREQ		2000000000
#define APB_CLK_FREQ		2000000000
#define PIC_CLK_FREQ		2000000000
#endif /* CONFIG_K1M_K1PRO */
#ifdef CONFIG_K1M_K1MAX
/* Unknown, use CPU bench */
#define CPU_CLK_FREQ		2000000000
#define APB_CLK_FREQ		2000000000
#define PIC_CLK_FREQ		2000000000
#endif /* CONFIG_K1M_K1MAX */
#endif /* CONFIG_K1M_S2C */
#endif /* CONFIG_K1M_SOC */

#endif /* __DT_BINDINGS_CLOCK_SBI_K1MAX_H */
