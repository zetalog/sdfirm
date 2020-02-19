// ------------------------------------------------------------
// ARMv8-A separate core program
//
// Description: Test case
// ------------------------------------------------------------

#ifdef TESTOS
#include <target/raven.h>
ENTRY(asm_test_xu_c3003)
#else
    .global asm_test_xu_c3003
    .type asm_test_xu_c3003, "function"
    .cfi_startproc
asm_test_xu_c3003:
#endif
    nop
    mov w9, 0

    // add your core code
    ORR v1.4s, #0x2f
    ORR v0.4s, #0xd2
    ORR v2.4s, #0xad
    ORR v3.4s, #0x2a
    ORR v4.4s, #0x4f
    ADDP v5.16b, v3.16b, v4.16b
    ABS v6.16b, v5.16b
    SADDLV d7, v6.4s
    ADDP v8.16b, v3.16b, v4.16b
    ABS v9.16b, v8.16b
    SADDLV d10, v9.4s
    fcmeq v8.2d, v5.2d, v8.2d
    fcmeq v10.2d, v7.2d, v10.2d
    fmov x8, d8
    mov x0, 1
    and x8, x8, x0
    fmov x9, d10
    and x9, x9, x8
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
ENDPROC(asm_test_xu_c3003)
define_asm_testfn asm_test_xu_c3003 0
#else
    .cfi_endproc
#endif
