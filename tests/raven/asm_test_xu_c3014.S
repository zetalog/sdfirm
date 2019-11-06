// ------------------------------------------------------------
// ARMv8-A separate core program
//
// Description: Test case
// ------------------------------------------------------------

#ifdef TESTOS
#include <target/raven.h>
ENTRY(asm_test_xu_c3014)
#else
    .global asm_test_xu_c3014
    .type asm_test_xu_c3014, "function"
    .cfi_startproc
asm_test_xu_c3014:
#endif
    nop
    mov w9, 0

    // add your core code
    MOV x1, #0xffffffff
    MOV x0, #0x01
    AND w2, w1, #2
    AND x3, x2, #7
    AND w4, w2, w3, lsl #0x02
    AND w5, w2, w3, lsr #0x2
    AND w6, w4, w3, asr #0x02
    AND w7, w5, w6, ror #0x02
    AND x8, x7, x6, lsl #0x02
    AND x9, x2, x3, lsr #0x02
    AND x10, x5, x6, asr #0x02
    AND x11, x9, x11, ror #0x02
    ANDS w12, w11, #15
    ANDS x13, x12, #30
    ANDS w14, w12, w13, lsl #0x02
    ANDS w15, w14, w13, lsr #0x02
    ANDS w16, w13, w13, asr #0x02
    ANDS w17, w15, w15, ror #0x02
    ANDS x18, x16, x17, lsl #0x02
    ANDS x19, x2, x1, lsr #0x02
    ANDS x20, x18, x15, asr #0x02
    ANDS x21, x18, x17, ror #0x02
    ldr x22, =0x2
    ldr x23, =0x0
    subs x22, x19, x22
    cbnz x22, _skip_pass
    subs x23, x5, x23
    cbnz x23, _skip_pass
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
ENDPROC(asm_test_xu_c3014)
define_asm_testfn asm_test_xu_c3014 0
#else
    .cfi_endproc
#endif
