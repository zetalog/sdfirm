// ------------------------------------------------------------
// ARMv8-A separate core program
//
// Description: Test case
// ------------------------------------------------------------

#ifdef TESTOS
#include <target/raven.h>
ENTRY(asm_test_xu_c3012)
#else
    .global asm_test_xu_c3012
    .type asm_test_xu_c3012, "function"
    .cfi_startproc
asm_test_xu_c3012:
#endif
    nop
    mov w9, 0

    // add your core code
    MOV x1, #0xffffffff
    MOV x0, #0x01
    ADD w2, w0, w1, uxtb #0x2
    ADD w3, w2, w1, uxth #0x2
    ADD w4, w2, w3, uxtw #0x2
    ADD w5, w4, w4, uxtx #0x2
    ADD w6, w5, w2, sxtb #0x2
    ADD w7, w0, w1, sxth #0x2
    ADD w8, w2, w1, sxtw #0x2
    ADD w9, w7, w8, sxtx #0x2
    ADD x10, x9, w1, uxtb #0x2
    ADD x11, x10, w10, uxth #0x2
    ADD x12, x10, w11, uxtw #0x2
    ADD x13, x12, x11, uxtx #0x2
    ADD x14, x12, w13, sxtb #0x2
    ADD x15, x0, w14, sxth #0x2
    ADD x16, x2, w3, sxtw #0x2
    ADD x17, x15, x16, sxtx #0x2
    ADD w18, w0, #0x2, lsl #0
    ADD w19, w17, #0x2, lsl #12
    ADD x20, x18, #0x2, lsl #0
    ADD x21, x0, #0x2, lsl #12
    ADD w22, w5, w2, lsl #0x2
    ADD w23, w21, w22, lsr #0x2
    ADD w24, w23, w20, asr #0x2
    ADD x25, x24, x23, lsl #0x2
    ADD x26, x25, x22, lsr #0x2
    ADD x27, x22, x21, asr #0x2
    ADDS w2, w0, w1, uxtb #0x2
    ADDS w3, w2, w1, uxth #0x2
    ADDS w4, w2, w3, uxtw #0x2
    ADDS w5, w4, w4, uxtx #0x2
    ADDS w6, w5, w2, sxtb #0x2
    ADDS w7, w0, w1, sxth #0x2
    ADDS w8, w2, w1, sxtw #0x2
    ADDS w9, w7, w8, sxtx #0x2
    ADDS x10, x9, w1, uxtb #0x2
    ADDS x11, x10, w10, uxth #0x2
    ADDS x12, x10, w11, uxtw #0x2
    ADDS x13, x12, x11, uxtx #0x2
    ADDS x14, x12, w13, sxtb #0x2
    ADDS x15, x0, w14, sxth #0x2
    ADDS x16, x2, w3, sxtw #0x2
    ADDS x17, x15, x16, sxtx #0x2
    ADDS w18, w0, #0x2, lsl #0
    ADDS w19, w17, #0x2, lsl #12
    ADDS x20, x18, #0x2, lsl #0
    ADDS x21, x0, #0x2, lsl #12
    ADDS w22, w5, w2, lsl #0x2
    ADDS w23, w21, w22, lsr #0x2
    ADDS w24, w23, w20, asr #0x2
    ADDS x25, x24, x23, lsl #0x2
    ADDS x26, x25, x22, lsr #0x2
    ADDS x27, x22, x21, asr #0x2
    ldr x28, =0x507b59
    ldr x29, =0x13dd
    subs x28, x27, x28
    cbnz x28, _skip_pass
    subs x29, x10, x29
    cbnz x29, _skip_pass
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
ENDPROC(asm_test_xu_c3012)
define_asm_testfn asm_test_xu_c3012 0
#else
    .cfi_endproc
#endif
