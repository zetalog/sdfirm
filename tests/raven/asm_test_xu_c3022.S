// ------------------------------------------------------------
// ARMv8-A separate core program
//
// Description: Test case
// ------------------------------------------------------------

#ifdef TESTOS
#include <target/raven.h>
ENTRY(asm_test_xu_c3022)
#else
    .global asm_test_xu_c3022
    .type asm_test_xu_c3022, "function"
    .cfi_startproc
asm_test_xu_c3022:
#endif
    nop
    mov w9, 0

    // add your core code
    MOV x1, #0xffffffff
    MOV x0, #0x01
    mov x2, #0
    mov x3, #0
    mov x4, #0
    mov x7, #0
    mov x8, #0
    mov x9, #0
    mov x10, #0
    bfm w2, w0, #1, #2
    bfm w3, w1, #2, #1
    bfm w4, w3, #2, #2
    bfm x8, x7, #1, #2
    bfm x9, x7, #2, #1
    bfm x10, x7, #2, #2
    ldr x11, =0xc0000000
    ldr x12, =0x0
    subs x11, x3, x11
    cbnz x11, _skip_pass
    subs x12, x9, x12
    cbnz x12, _skip_pass
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
ENDPROC(asm_test_xu_c3022)
define_asm_testfn asm_test_xu_c3022 0
#else
    .cfi_endproc
#endif
