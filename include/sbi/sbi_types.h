/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ifndef __SBI_TYPES_H__
#define __SBI_TYPES_H__

#include <target/generic.h>

/* clang-format off */

typedef char			s8;
typedef unsigned char		u8;

typedef short			s16;
typedef unsigned short		u16;

typedef int			s32;
typedef unsigned int		u32;

#if __riscv_xlen == 64
typedef long			s64;
typedef unsigned long		u64;
#define PRILX			"016lx"
#elif __riscv_xlen == 32
typedef long long		s64;
typedef unsigned long long	u64;
#define PRILX			"08lx"
#else
#error "Unexpected __riscv_xlen"
#endif

typedef unsigned long		ulong;
typedef unsigned long		virtual_addr_t;
typedef unsigned long		virtual_size_t;
typedef unsigned long		physical_addr_t;
typedef unsigned long		physical_size_t;

/* clang-format on */

#endif
