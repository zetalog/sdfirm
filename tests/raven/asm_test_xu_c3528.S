// ------------------------------------------------------------
// ARMv8-A separate core program
//
// Description: Test case
// ------------------------------------------------------------
//
#ifdef TESTOS
#include <target/raven.h>
ENTRY(asm_test_xu_c3528)
#else
    .global asm_test_xu_c3528
    .type asm_test_xu_c3528, "function"
    .cfi_startproc
asm_test_xu_c3528:
#endif

    // add your core code
    //3528 FMA
    fmov d2, #2.0
    fmov d3, #3.0
    fmov d4, #4.0
    fmov d5, #5.0
    fmov d6, #2.0
    fmov d7, #3.0
    fmov d8, #4.0
    fmov d9, #5.0
    fnmul d20, d2, d3
    fnmul d21, d20, d4
    fnmul d22, d21, d5
    fnmul d23, d6, d7
    fnmul d24, d23, d8
    fnmul d25, d24, d9
    fcmeq d30, d22, d25
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
ENDPROC(asm_test_xu_c3528)
define_asm_testfn asm_test_xu_c3528 0
#else
    .cfi_endproc
#endif
