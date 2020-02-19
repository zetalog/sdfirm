// ------------------------------------------------------------
// ARMv8-A separate core program
//
// Description: Test case
// ------------------------------------------------------------

#ifdef TESTOS
#include <target/raven.h>
ENTRY(asm_test_xu_c3031)
#else
    .global asm_test_xu_c3031
    .type asm_test_xu_c3031, "function"
    .cfi_startproc
asm_test_xu_c3031:
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
    movi v12.4s, #0
    movi v13.4s, #0
    movi v14.4s, #0
    movi v15.4s, #0
    movi v16.4s, #0
    movi v17.4s, #0
    movi v18.4s, #0
    orr v0.4s, #0x3d
    orr v1.4s, #0x41
    orr v2.4s, #0xaf
    orr v3.4s, #0xf2
    orr v4.4s, #0x92
    add v5.16b, v0.16b, v1.16b
    addhn v6.8b, v2.8h, v3.8h
    addp v7.16b, v4.16b, v3.16b
    addv b8, v7.16b
    saba v9.16b, v7.16b, v8.16b
    sabal2 v10.8h, v7.16b, v8.16b
    sabd v11.16b, v10.16b, v10.16b
    sabdl2 v12.8h, v11.16b, v10.16b
    sadalp v13.8h, v12.16b
    saddl2 v14.8h, v2.16b, v4.16b
    saddlp v15.8h, v12.16b
    saddlv h16, v7.16b
    saddw2 v17.8h, v4.8h, v13.16b
    orr v18.4s, #0xe
    cmeq v18.4s, v10.4s, v18.4s
    fmov x1, d18
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
ENDPROC(asm_test_xu_c3031)
define_asm_testfn asm_test_xu_c3031 0
#else
    .cfi_endproc
#endif
