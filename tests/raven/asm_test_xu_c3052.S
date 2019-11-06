// ------------------------------------------------------------
// ARMv8-A separate core program
//
// Description: Test case
// ------------------------------------------------------------

#ifdef TESTOS
#include <target/raven.h>
ENTRY(asm_test_xu_c3052)
#else
    .global asm_test_xu_c3052
    .type asm_test_xu_c3052, "function"
    .cfi_startproc
asm_test_xu_c3052:
#endif
    nop
    mov w9, 0

    // add your core code
    ldr x0, =0xffffffffffffffff
    ldr x1, =1
    ldr x3, =3
    ldr x2, =10000
    fmov d0, x0
    fmov d3, x1
    dup v1.2d, v0.d[0]
    dup v2.2d, v0.d[0]
    dup v3.2d, v3.d[0]
loop:
    uaddlv h4, v1.16b
    rev x4, x2
    mov x6, x0
    lsr x7, x2, x3
    sub x2, x2, #1
    cbnz x2, loop
    // end of add your code

    mov w9, 1
_skip_pass:
    // write to address 44'h0FF_7003_FFC0 if your program pass; if fail, write 0
#ifdef TESTOS
    mov w0, w9
#else
#ifdef RTL_SIM
    movk x8, #0xFFC0
    movk x8, #0x7003, lsl #16
    movk x8, #0x00FF, lsl #32
    movk x8, #0x0000, lsl #48
#else
    movk x8, #0xFFC0
    movk x8, #0x8003, lsl #16
    movk x8, #0x0000, lsl #32
    movk x8, #0x0000, lsl #48
#endif
    str x30, [sp, #-0x8]	// x30 is lr
    bl GetCPUID			// 0-47
    add x8, x8, x0
    strb w9, [x8, #0]
    nop
    dsb ish
    ldr x30, [sp, #-0x8]
#endif
    ret
#ifdef TESTOS
ENDPROC(asm_test_xu_c3052)
define_asm_testfn asm_test_xu_c3052 0
#else
    .cfi_endproc
#endif
