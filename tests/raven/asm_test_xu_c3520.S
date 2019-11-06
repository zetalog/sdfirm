// ------------------------------------------------------------
// ARMv8-A separate core program
//
// Description: Test case
// ------------------------------------------------------------
//
#ifdef TESTOS
#include <target/raven.h>
ENTRY(asm_test_xu_c3520)
#else
    .global asm_test_xu_c3520
    .type asm_test_xu_c3520, "function"
    .cfi_startproc
asm_test_xu_c3520:
#endif

    // add your core code
    //3520 FADD FarPath 2d
    fmov v2.2d, #1.0
    fmov v3.2d, #4.0
    fadd v4.2d, v2.2d, v3.2d
    fmov v5.2d, #5.0
    fcmeq v6.2d, v4.2d, v5.2d
    fmov x9, d6
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
ENDPROC(asm_test_xu_c3520)
define_asm_testfn asm_test_xu_c3520 0
#else
    .cfi_endproc
#endif
