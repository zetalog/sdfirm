// ------------------------------------------------------------
// ARMv8-A separate core program
//
// Description: Test case
// ------------------------------------------------------------

#ifdef TESTOS
#include <target/raven.h>
ENTRY(asm_test_xu_c2622)
#else
    .global asm_test_xu_c2622
    .type asm_test_xu_c2622, "function"
    .cfi_startproc
asm_test_xu_c2622:
#endif
    nop
    mov w9, 0

    // add your core code
    fmov d5, 1.0
    fmov d1, 1.0
    fmov d2, 2.0
    fdiv d3, d2, d1
    fmul d5, d3, d1
    fmov d6, d5
    fcmp d6, d5
    b.ne _skip_pass
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
ENDPROC(asm_test_xu_c2622)
define_asm_testfn asm_test_xu_c2622 0
#else
    .cfi_endproc
#endif
