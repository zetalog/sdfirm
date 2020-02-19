// ------------------------------------------------------------
// ARMv8-A separate core program
//
// Description: Test case
// ------------------------------------------------------------
//
#ifdef TESTOS
#include <target/raven.h>
ENTRY(asm_test_xu_c3534)
#else
    .global asm_test_xu_c3534
    .type asm_test_xu_c3534, "function"
    .cfi_startproc
asm_test_xu_c3534:
#endif

    // add your core code
    //3534
    fmov d2, #2.0
    fmov d3, #2.0
    fsqrt d20, d2
    fsqrt d21, d20
    fsqrt d22, d21
    fsqrt d23, d22
    fsqrt d24, d23
    fsqrt d25, d3
    fsqrt d26, d25
    fsqrt d27, d26
    fsqrt d28, d27
    fsqrt d29, d28
    fcmeq d30, d24, d29
    fmov x9, d30
    mov x0, 1
    and x9, x9, x0
    // end of add your code

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
ENDPROC(asm_test_xu_c3534)
define_asm_testfn asm_test_xu_c3534 0
#else
    .cfi_endproc
#endif
