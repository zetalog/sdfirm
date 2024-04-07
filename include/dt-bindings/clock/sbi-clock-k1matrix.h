/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 *  Copyright (C) 2023 SpacemiT Co., Ltd.
 *    Author: 2022 Lv Zheng <lv.zheng@spacemit.com>
 */

#ifndef __DT_BINDINGS_CLOCK_SBI_K1MATRIX_H
#define __DT_BINDINGS_CLOCK_SBI_K1MATRIX_H

#include <dt-bindings/clock/sbi-clock.h>

#define SYS_CLK_FREQ		20000000
#define CFG_CLK_FREQ		10000000
#define DDR_CLK_FREQ		100000000

#define CPU_CLK_FREQ		SYS_CLK_FREQ
#define APB_CLK_FREQ		CFG_CLK_FREQ
#define PIC_CLK_FREQ		CFG_CLK_FREQ

/* MESH2SYS CPU -> SYS_SUB_AXI
 * SYS2MESH DMA -> MESH AXI
 * DDR ACLK
 */
#define AXI_CLK_FREQ		SYS_CLK_FREQ

#endif /* __DT_BINDINGS_CLOCK_SBI_K1MATRIX_H */
