// ------------------------------------------------------------
// ARMv8-A separate core program
//
// Description: Test case
// ------------------------------------------------------------

#ifdef TESTOS
#include <target/raven.h>
ENTRY(asm_test_xu_c3042)
#else
    .global asm_test_xu_c3042
    .type asm_test_xu_c3042, "function"
    .cfi_startproc
asm_test_xu_c3042:
#endif
    nop
    mov w9, 0

    // add your core code
    movi v0.4s, #0
    movi v1.4s, #0
    movi v2.4s, #0
    movi v3.4s, #0
    movi v4.4s, #0
    movi v5.4s, #0
    movi v6.4s, #0
    movi v7.4s, #0
    movi v8.4s, #0
    movi v9.4s, #0
    movi v10.4s, #0
    movi v11.4s, #0
    movi v29.4s, #0
    movi v30.4s, #0
    orr v0.4s, #0x3d
    orr v1.4s, #0x41
    orr v2.4s, #0xaf
    orr v3.4s, #0xf2
    ldr x6, =0x100000001
    dup v4.16b, w6
    ext v5.16b, v1.16b, v3.16b, #1
    ins v6.d[1], x6
    rev64 v7.16b, v6.16b
    tbx v8.16b, {v0.16b, v1.16b, v2.16b, v3.16b}, v4.16b
    trn1 v9.16b, v4.16b, v8.16b
    zip1 v10.16b, v8.16b, v2.16b
    uzp1 v11.16b, v8.16b, v2.16b
    and v30.16b, v30.16b, v29.16b
    orr v30.4s, #0xaf
    cmeq v30.4s, v2.4s, v30.4s
    fmov x1, d30
    cbz x1, _skip_pass
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
ENDPROC(asm_test_xu_c3042)
define_asm_testfn asm_test_xu_c3042 0
#else
    .cfi_endproc
#endif
