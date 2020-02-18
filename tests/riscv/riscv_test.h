// See LICENSE for license details.

#ifndef _ENV_PHYSICAL_SINGLE_CORE_H
#define _ENV_PHYSICAL_SINGLE_CORE_H

#include <target/compiler.h>
#include <target/bench.h>
#include <asm/asm-offsets.h>

//-----------------------------------------------------------------------
// Begin Macro
//-----------------------------------------------------------------------

#define RVTEST_RV64U
#define RVTEST_RV64UF
#define RVTEST_RV64UV
#define RVTEST_RV32U
#define RVTEST_RV32UF
#define RVTEST_RV32UV
#define RVTEST_RV64M
#define RVTEST_RV64S
#define RVTEST_RV32M
#define RVTEST_RV32S

#define RVTEST_ENABLE_SUPERVISOR                                        \
  li a0, MSTATUS_MPP & (MSTATUS_MPP >> 1);                              \
  csrs mstatus, a0;                                                     \
  li a0, SIP_SSIP | SIP_STIP;                                           \
  csrs mideleg, a0;                                                     \

#define RVTEST_ENABLE_MACHINE                                           \
  li a0, MSTATUS_MPP;                                                   \
  csrs mstatus, a0;                                                     \

#define RVTEST_FP_ENABLE                                                \
  li a0, MSTATUS_FS & (MSTATUS_FS >> 1);                                \
  csrs mstatus, a0;                                                     \
  csrwi fcsr, 0

#define RVTEST_VECTOR_ENABLE                                            \
  li a0, (MSTATUS_VS & (MSTATUS_VS >> 1)) |                             \
         (MSTATUS_FS & (MSTATUS_FS >> 1));                              \
  csrs mstatus, a0;                                                     \
  csrwi fcsr, 0

#define RISCV_MULTICORE_DISABLE                                         \
  csrr a0, mhartid;                                                     \
  1: bnez a0, 1b

/* Save all registers except SP, TP, A0 */
#define RVTEST_CODE_BEGIN                                               \
	ENTRY(RVTEST_NAME);						\
	addi	sp,  sp, -16;						\
	sd	ra,  8(sp);						\
	addi	sp,  sp, -(PT_SIZE_ON_STACK);				\
	REG_S	x1,  PT_RA(sp);						\
	REG_S	x3,  PT_GP(sp);						\
	REG_S	x5,  PT_T0(sp);						\
	REG_S	x6,  PT_T1(sp);						\
	REG_S	x7,  PT_T2(sp);						\
	REG_S	x8,  PT_S0(sp);						\
	REG_S	x9,  PT_S1(sp);						\
	REG_S	x11, PT_A1(sp);						\
	REG_S	x12, PT_A2(sp);						\
	REG_S	x13, PT_A3(sp);						\
	REG_S	x14, PT_A4(sp);						\
	REG_S	x15, PT_A5(sp);						\
	REG_S	x16, PT_A6(sp);						\
	REG_S	x17, PT_A7(sp);						\
	REG_S	x18, PT_S2(sp);						\
	REG_S	x19, PT_S3(sp);						\
	REG_S	x20, PT_S4(sp);						\
	REG_S	x21, PT_S5(sp);						\
	REG_S	x22, PT_S6(sp);						\
	REG_S	x23, PT_S7(sp);						\
	REG_S	x24, PT_S8(sp);						\
	REG_S	x25, PT_S9(sp);						\
	REG_S	x26, PT_S10(sp);					\
	REG_S	x27, PT_S11(sp);					\
	REG_S	x28, PT_T3(sp);						\
	REG_S	x29, PT_T4(sp);						\
	REG_S	x30, PT_T5(sp);						\
	REG_S	x31, PT_T6(sp);						\
	add	x30, sp, zero

//-----------------------------------------------------------------------
// End Macro
//-----------------------------------------------------------------------

/* Restore all registers except SP, TP, A0 */
#define RVTEST_CODE_END                                                 \
	add	sp,  x30, zero;						\
	REG_L	x1,  PT_RA(sp);						\
	REG_L	x3,  PT_GP(sp);						\
	REG_L	x4,  PT_TP(sp);						\
	REG_L	x5,  PT_T0(sp);						\
	REG_L	x6,  PT_T1(sp);						\
	REG_L	x7,  PT_T2(sp);						\
	REG_L	x8,  PT_S0(sp);						\
	REG_L	x9,  PT_S1(sp);						\
	REG_L	x11, PT_A1(sp);						\
	REG_L	x12, PT_A2(sp);						\
	REG_L	x13, PT_A3(sp);						\
	REG_L	x14, PT_A4(sp);						\
	REG_L	x15, PT_A5(sp);						\
	REG_L	x16, PT_A6(sp);						\
	REG_L	x17, PT_A7(sp);						\
	REG_L	x18, PT_S2(sp);						\
	REG_L	x19, PT_S3(sp);						\
	REG_L	x20, PT_S4(sp);						\
	REG_L	x21, PT_S5(sp);						\
	REG_L	x22, PT_S6(sp);						\
	REG_L	x23, PT_S7(sp);						\
	REG_L	x24, PT_S8(sp);						\
	REG_L	x25, PT_S9(sp);						\
	REG_L	x26, PT_S10(sp);					\
	REG_L	x27, PT_S11(sp);					\
	REG_L	x28, PT_T3(sp);						\
	REG_L	x29, PT_T4(sp);						\
	REG_L	x30, PT_T5(sp);						\
	REG_L	x31, PT_T6(sp);						\
	REG_L	x2,  PT_SP(sp);						\
	ld      ra,  8(sp);						\
	addi	sp,  sp, 16;						\
	ret;								\
	ENDPROC(RVTEST_NAME);						\
	define_asm_testfn RVTEST_NAME 0 128 1 CPU_EXEC_META

//-----------------------------------------------------------------------
// Pass/Fail Macro
//-----------------------------------------------------------------------

#define RVTEST_PASS                                                     \
        fence;                                                          \
        li TESTNUM, 1;                                                  \
        li a7, 0;							\
        li a0, 1;

#define TESTNUM gp
#define RVTEST_FAIL                                                     \
        fence;                                                          \
1:      beqz TESTNUM, 1b;                                               \
        sll TESTNUM, TESTNUM, 1;                                        \
        or TESTNUM, TESTNUM, 1;                                         \
        addi a7, TESTNUM, 0;                                            \
	li a0, 0

//-----------------------------------------------------------------------
// Data Section Macro
//-----------------------------------------------------------------------

#define RVTEST_DATA_BEGIN
#define RVTEST_DATA_END

#endif
