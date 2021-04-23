/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ifndef __RISCV_UNPRIV_H__
#define __RISCV_UNPRIV_H__

struct sbi_scratch;

struct unpriv_trap {
	unsigned long ilen;
	unsigned long cause;
	unsigned long tval;
};

#define DECLARE_UNPRIVILEGED_LOAD_FUNCTION(type, __type)   \
	__type load_##type(const __type *addr,             \
			 struct sbi_scratch *scratch,      \
			 struct unpriv_trap *trap);

#define DECLARE_UNPRIVILEGED_STORE_FUNCTION(type, __type) \
	void store_##type(__type *addr, __type val,       \
			  struct sbi_scratch *scratch,    \
			  struct unpriv_trap *trap);

DECLARE_UNPRIVILEGED_LOAD_FUNCTION(u8, uint8_t)
DECLARE_UNPRIVILEGED_LOAD_FUNCTION(u16, uint16_t)
DECLARE_UNPRIVILEGED_LOAD_FUNCTION(s8, int8_t)
DECLARE_UNPRIVILEGED_LOAD_FUNCTION(s16, int16_t)
DECLARE_UNPRIVILEGED_LOAD_FUNCTION(s32, int32_t)
DECLARE_UNPRIVILEGED_STORE_FUNCTION(u8, uint8_t)
DECLARE_UNPRIVILEGED_STORE_FUNCTION(u16, uint16_t)
DECLARE_UNPRIVILEGED_STORE_FUNCTION(u32, uint32_t)
DECLARE_UNPRIVILEGED_LOAD_FUNCTION(u32, uint32_t)
DECLARE_UNPRIVILEGED_LOAD_FUNCTION(u64, uint64_t)
DECLARE_UNPRIVILEGED_STORE_FUNCTION(u64, uint64_t)
DECLARE_UNPRIVILEGED_LOAD_FUNCTION(ulong, unsigned long)

ulong get_insn(ulong mepc, ulong *mstatus);

#endif
