// ------------------------------------------------------------
// ARMv8-A separate core program
//
// Description: Test case
// ------------------------------------------------------------
//
#ifdef TESTOS
#include <target/raven.h>
ENTRY(asm_test_xu_c3541)
#else
    .global asm_test_xu_c3541
    .type asm_test_xu_c3541, "function"
    .cfi_startproc
asm_test_xu_c3541:
#endif

    // add your core code
    //3541
    //maxpower2 fdiv with fadd 2d
    fmov v1.2d, #1.0
    fmov v2.2d, #2.0
    fmov v3.2d, #1.0
    movi v4.16b, #0x2e
    movi v5.16b, #0x07

    ldr x1, =0x1388//5k
loop:
    fdiv v6.4s, v4.4s, v5.4s
    fmla v3.2d, v2.2d, v1.2d
    fmla v3.2d, v1.2d, v2.2d
    fmla v3.2d, v2.2d, v1.2d
    fmla v3.2d, v1.2d, v2.2d
    sub  x1, x1, #1
    cbnz x1, loop

    mov x9, #1
_skip_pass:
    // end of add your code
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
ENDPROC(asm_test_xu_c3541)
define_asm_testfn asm_test_xu_c3541 0
#else
    .cfi_endproc
#endif
