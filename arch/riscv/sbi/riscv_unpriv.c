/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#include <target/sbi.h>

#define DEFINE_UNPRIVILEGED_LOAD_FUNCTION(type, __type, insn, insnlen)        \
	__type load_##type(const __type *addr,                                \
			   struct sbi_scratch *scratch,                       \
			   struct unpriv_trap *trap)                          \
	{                                                                     \
		register ulong __mstatus asm("a2");                           \
		__type val = 0;                                               \
		trap->ilen = insnlen;                                         \
		trap->cause = 0;                                              \
		trap->tval = 0;                                               \
		sbi_hart_set_trap_info(scratch, trap);                        \
		asm volatile(                                                 \
			"csrrs %0, " __stringify(CSR_MSTATUS) ", %3\n"        \
			#insn " %1, %2\n"                                     \
			"csrw " __stringify(CSR_MSTATUS) ", %0"               \
		    : "+&r"(__mstatus), "=&r"(val)                            \
		    : "m"(*addr), "r"(SR_MPRV));                              \
		sbi_hart_set_trap_info(scratch, NULL);                        \
		return val;                                                   \
	}

#define DEFINE_UNPRIVILEGED_STORE_FUNCTION(type, __type, insn, insnlen)       \
	void store_##type(__type *addr, __type val,                           \
			struct sbi_scratch *scratch,                          \
			struct unpriv_trap *trap)                             \
	{                                                                     \
		register ulong __mstatus asm("a3");                           \
		trap->ilen = insnlen;                                         \
		trap->cause = 0;                                              \
		trap->tval = 0;                                               \
		sbi_hart_set_trap_info(scratch, trap);                        \
		asm volatile(                                                 \
			"csrrs %0, " __stringify(CSR_MSTATUS) ", %3\n"        \
			#insn " %1, %2\n"                                     \
			"csrw " __stringify(CSR_MSTATUS) ", %0"               \
			: "+&r"(__mstatus)                                    \
			: "r"(val), "m"(*addr), "r"(SR_MPRV));                \
		sbi_hart_set_trap_info(scratch, NULL);                        \
	}

DEFINE_UNPRIVILEGED_LOAD_FUNCTION(u8, uint8_t, lbu, 4)
DEFINE_UNPRIVILEGED_LOAD_FUNCTION(u16, uint16_t, lhu, 4)
DEFINE_UNPRIVILEGED_LOAD_FUNCTION(s8, int8_t, lb, 4)
DEFINE_UNPRIVILEGED_LOAD_FUNCTION(s16, int16_t, lh, 4)
DEFINE_UNPRIVILEGED_LOAD_FUNCTION(s32, int32_t, lw, 2)
DEFINE_UNPRIVILEGED_STORE_FUNCTION(u8, uint8_t, sb, 4)
DEFINE_UNPRIVILEGED_STORE_FUNCTION(u16, uint16_t, sh, 4)
DEFINE_UNPRIVILEGED_STORE_FUNCTION(u32, uint32_t, sw, 2)
#if __riscv_xlen == 64
DEFINE_UNPRIVILEGED_LOAD_FUNCTION(u32, uint32_t, lwu, 4)
DEFINE_UNPRIVILEGED_LOAD_FUNCTION(u64, uint64_t, ld, 2)
DEFINE_UNPRIVILEGED_STORE_FUNCTION(u64, uint64_t, sd, 2)
DEFINE_UNPRIVILEGED_LOAD_FUNCTION(ulong, unsigned long, ld, 2)
#else
DEFINE_UNPRIVILEGED_LOAD_FUNCTION(u32, uint32_t, lw, 2)
DEFINE_UNPRIVILEGED_LOAD_FUNCTION(ulong, unsigned long, lw, 2)

uint64_t load_u64(const uint64_t *addr,
		  struct sbi_scratch *scratch, struct unpriv_trap *trap)
{
	uint64_t ret = load_u32((uint32_t *)addr, scratch, trap);

	if (trap->cause)
		return 0;
	ret |= ((uint64_t)load_u32((uint32_t *)addr + 1, scratch, trap) << 32);
	if (trap->cause)
		return 0;

	return ret;
}

void store_u64(uint64_t *addr, uint64_t val,
	       struct sbi_scratch *scratch, struct unpriv_trap *trap)
{
	store_u32((uint32_t *)addr, val, scratch, trap);
	if (trap->cause)
		return;

	store_u32((uint32_t *)addr + 1, val >> 32, scratch, trap);
	if (trap->cause)
		return;
}
#endif

ulong get_insn(ulong mepc, ulong *mstatus)
{
	register ulong __mepc asm("a2") = mepc;
	register ulong __mstatus asm("a3");
	ulong val;
#ifndef __riscv_compressed
	asm("csrrs %[mstatus], " __stringify(CSR_MSTATUS) ", %[mprv]\n"
#if __riscv_xlen == 64
	    "lwu %[insn], (%[addr])\n"
#else
	    "lw %[insn], (%[addr])\n"
#endif
	    "csrw " __stringify(CSR_MSTATUS) ", %[mstatus]"
	    : [mstatus] "+&r"(__mstatus), [insn] "=&r"(val)
	    : [mprv] "r"(SR_MPRV | SR_MXR), [addr] "r"(__mepc));
#else
	ulong rvc_mask = 3, tmp;
	asm("csrrs %[mstatus], " __stringify(CSR_MSTATUS) ", %[mprv]\n"
	    "and %[tmp], %[addr], 2\n"
	    "bnez %[tmp], 1f\n"
#if __riscv_xlen == 64
	    "lwu %[insn], (%[addr])\n"
#else
	    "lw %[insn], (%[addr])\n"
#endif
	    "and %[tmp], %[insn], %[rvc_mask]\n"
	    "beq %[tmp], %[rvc_mask], 2f\n"
	    "sll %[insn], %[insn], %[xlen_minus_16]\n"
	    "srl %[insn], %[insn], %[xlen_minus_16]\n"
	    "j 2f\n"
	    "1:\n"
	    "lhu %[insn], (%[addr])\n"
	    "and %[tmp], %[insn], %[rvc_mask]\n"
	    "bne %[tmp], %[rvc_mask], 2f\n"
	    "lhu %[tmp], 2(%[addr])\n"
	    "sll %[tmp], %[tmp], 16\n"
	    "add %[insn], %[insn], %[tmp]\n"
	    "2: csrw " __stringify(CSR_MSTATUS) ", %[mstatus]"
	    : [mstatus] "+&r"(__mstatus), [insn] "=&r"(val), [tmp] "=&r"(tmp)
	    : [mprv] "r"(SR_MPRV | SR_MXR), [addr] "r"(__mepc),
	      [rvc_mask] "r"(rvc_mask), [xlen_minus_16] "i"(__riscv_xlen - 16));
#endif
	if (mstatus)
		*mstatus = __mstatus;
	return val;
}
