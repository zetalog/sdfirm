/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 *  Copyright (C) 2021 CAS SmartCore Co., Ltd.
 *    Author: 2021 Lv Zheng <zhenglv@smarco.cn>
 */

#ifndef __DT_BINDINGS_CLOCK_SBI_H
#define __DT_BINDINGS_CLOCK_SBI_H

#ifndef clkid
#define clkid(cat, clk)		((clk) | ((cat) << 8))
#endif

#endif /* __DT_BINDINGS_CLOCK_SBI_H */
