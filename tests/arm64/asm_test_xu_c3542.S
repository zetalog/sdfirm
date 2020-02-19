// ------------------------------------------------------------
// ARMv8-A separate core program
//
// Description: Test case
// ------------------------------------------------------------
//
#ifdef TESTOS
#include <target/raven.h>
ENTRY(asm_test_xu_c3542)
#else
    .global asm_test_xu_c3542
    .type asm_test_xu_c3542, "function"
    .cfi_startproc
asm_test_xu_c3542:
#endif

    // add your core code
    //3542
    //maxpower3 fdiv with fadd 4s
    fmov v1.4s, #1.0
    fmov v2.4s, #2.0
    fmov v3.4s, #1.0
    movi v5.16b, #0xae
    movi v6.16b, #0x0b

    ldr x1, =0x1388//5k
loop:
    fdiv v7.4s, v5.4s, v6.4s
    fadd v3.4s, v2.4s, v1.4s
    fadd v3.4s, v1.4s, v2.4s
    fadd v3.4s, v2.4s, v1.4s
    fadd v3.4s, v1.4s, v2.4s
    fadd v3.4s, v2.4s, v1.4s
    fadd v3.4s, v1.4s, v2.4s
    sub  x1, x1, #1
    cbnz x1, loop
    // end of add your code

    mov x9, #1
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
ENDPROC(asm_test_xu_c3542)
define_asm_testfn asm_test_xu_c3542 0
#else
    .cfi_endproc
#endif
