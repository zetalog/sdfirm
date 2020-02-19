// ------------------------------------------------------------
// ARMv8-A separate core program
//
// Description: Test case
// ------------------------------------------------------------
//
#ifdef TESTOS
#include <target/raven.h>
ENTRY(asm_test_xu_c2101)
#else
    .global asm_test_xu_c2101
    .type asm_test_xu_c2101, "function"
    .cfi_startproc
asm_test_xu_c2101:
#endif
     // add your core code

#ifdef TESTOS
#define ADDR_BASE test_array
#else
#define ADDR_BASE 0xff70004000
#endif

     nop
     nop
	movz x15, #0
    mov w9, 0

//==================== own code begin ====================

     fmov v0.2d, #2.0
     fmov v1.2d, #1.0
     mov  x0, #0
     mov  x1, #1
     fmul v2.2d, v0.2d, v1.2d
     mul  x2, x0, x1

mov x20, #0
mov x21, #0x9
ldr x10, =ADDR_BASE
loop_test_begin:
     cmp x20, #0x9
     b.eq loop_test_end

//     ldr q10, [x10], #0x10
//     ldr q11, [x10], #0x11
//     ldr q12, [x10], #0x12
//     ldr q13, [x10], #0x13
//     ldr q14, [x10], #0x14
//     ldr q15, [x10], #0x15
//     ldr q16, [x10], #0x16
//     ldr q17, [x10], #0x17
//     ldr q18, [x10], #0x18
//     ldr q19, [x10], #0x19
//     ldr q20, [x10], #0x20
//     ldr q21, [x10], #0x21
//     ldr q22, [x10], #0x22
//     ldr q23, [x10], #0x23
//     ldr q24, [x10], #0x24
//     ldr q25, [x10], #0x25

     //ld4 {v1.d, v2.d, v3.d, v4.d}[0], [x10]

     ldp  x3, x4, [x10], #16
     fmla v3.2d, v2.2d, v1.2d

     ldp  x5, x6, [x10], #16
     fmla v4.2d, v3.2d, v1.2d

     ldp  x7, x8, [x10], #16
     fmla v5.2d, v4.2d, v1.2d

     ldp  x9, x1, [x10], #16
     fmla v6.2d, v5.2d, v1.2d

     ldp  x3, x4, [x10], #16
     fmla v7.2d, v6.2d, v1.2d

     ldp  x5, x6, [x10], #16
     fmla v8.2d, v7.2d, v1.2d

     add x20, x20, #0x1
     sub x21, x21, #0x1
     b loop_test_begin
loop_test_end:

     mov w9, 1
     cmp x21, #0
     b.eq test_pass
     // otherwise, test fail
     mov w9, 0

test_pass:

	nop

_skip_pass:
    // end of add your code
    // write to address 44'h0FF_7003_FFC0 if your program pass; if fail, write 0
#ifdef TESTOS
     mov w0, w9
#else
#ifdef RTL_SIM
     movk x8, #0xFFC0
     movk x8, #0x7003,      lsl #16
     movk x8, #0x00FF,      lsl #32
     movk x8, #0x0000,      lsl #48
#else
     movk x8, #0xFFC0
     movk x8, #0x8003,      lsl #16
     movk x8, #0x0000,      lsl #32
     movk x8, #0x0000,      lsl #48
#endif
     str x30, [sp, #-0x8]             // x30 is lr
     bl GetCPUID     // 0-47
     add x8, x8, x0
     strb w9, [x8, #0]
     nop
     dsb ish
     ldr x30, [sp, #-0x8]
#endif
     ret
#ifdef TESTOS
ENDPROC(asm_test_xu_c2101)
define_asm_testfn asm_test_xu_c2101 0 CPU_EXEC_SYNC
#else
    .cfi_endproc
#endif
