// ------------------------------------------------------------
// ARMv8-A separate core program
//
// Description: Test case
// ------------------------------------------------------------
//
#ifdef TESTOS
#include <target/raven.h>
ENTRY(asm_test_xu_c3517)
#else
    .global asm_test_xu_c3517
    .type asm_test_xu_c3517, "function"
    .cfi_startproc
asm_test_xu_c3517:
#endif

    // add your core code
    //3517 FCVTAS 2D
    //3cycle fcvt fwd
    fmov v2.2d, #2.0
    fcvtas v3.2d, v2.2d
    scvtf v4.2d, v3.2d
    //v2=40000000_00000000_40000000_00000000
    //v3=2 2
    //v4=40000000_00000000_40000000_00000000
    fcmeq v5.2d, v4.2d, v2.2d
    fmov x9, d5
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
ENDPROC(asm_test_xu_c3517)
define_asm_testfn asm_test_xu_c3517 0
#else
    .cfi_endproc
#endif
