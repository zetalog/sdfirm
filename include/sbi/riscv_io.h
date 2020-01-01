/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ifndef __RISCV_IO_H__
#define __RISCV_IO_H__

#include <sbi/sbi_types.h>

/* FIXME: These are now the same as asm-generic */

/* clang-format off */

#define __io_rbr()		do {} while (0)
#define __io_rar()		do {} while (0)
#define __io_rbw()		do {} while (0)
#define __io_raw()		do {} while (0)

#define readb_relaxed(c)	({ u8  __v; __io_rbr(); __v = __raw_readb(c); __io_rar(); __v; })
#define readw_relaxed(c)	({ u16 __v; __io_rbr(); __v = __raw_readw(c); __io_rar(); __v; })
#define readl_relaxed(c)	({ u32 __v; __io_rbr(); __v = __raw_readl(c); __io_rar(); __v; })

#define writeb_relaxed(v,c)	({ __io_rbw(); __raw_writeb((v),(c)); __io_raw(); })
#define writew_relaxed(v,c)	({ __io_rbw(); __raw_writew((v),(c)); __io_raw(); })
#define writel_relaxed(v,c)	({ __io_rbw(); __raw_writel((v),(c)); __io_raw(); })

#if __riscv_xlen != 32
#define readq_relaxed(c)	({ u64 __v; __io_rbr(); __v = __raw_readq(c); __io_rar(); __v; })
#define writeq_relaxed(v,c)	({ __io_rbw(); __raw_writeq((v),(c)); __io_raw(); })
#endif

#define __io_br()	do {} while (0)
#define __io_ar()	__asm__ __volatile__ ("fence i,r" : : : "memory");
#define __io_bw()	__asm__ __volatile__ ("fence w,o" : : : "memory");
#define __io_aw()	do {} while (0)

#define readb(c)	({ u8  __v; __io_br(); __v = __raw_readb(c); __io_ar(); __v; })
#define readw(c)	({ u16 __v; __io_br(); __v = __raw_readw(c); __io_ar(); __v; })
#define readl(c)	({ u32 __v; __io_br(); __v = __raw_readl(c); __io_ar(); __v; })

#define writeb(v,c)	({ __io_bw(); __raw_writeb((v),(c)); __io_aw(); })
#define writew(v,c)	({ __io_bw(); __raw_writew((v),(c)); __io_aw(); })
#define writel(v,c)	({ __io_bw(); __raw_writel((v),(c)); __io_aw(); })

#if __riscv_xlen != 32
#define readq(c)	({ u64 __v; __io_br(); __v = __raw_readq(c); __io_ar(); __v; })
#define writeq(v,c)	({ __io_bw(); __raw_writeq((v),(c)); __io_aw(); })
#endif

/* clang-format on */

#endif
