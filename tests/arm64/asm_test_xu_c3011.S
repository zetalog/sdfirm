// ------------------------------------------------------------
// ARMv8-A separate core program
//
// Description: Test case
// ------------------------------------------------------------

#ifdef TESTOS
#include <target/raven.h>
ENTRY(asm_test_xu_c3011)
#else
    .global asm_test_xu_c3011
    .type asm_test_xu_c3011, "function"
    .cfi_startproc
asm_test_xu_c3011:
#endif
    nop
    mov w9, 0

    // add your core code
    MOV x1, #0xffffffff
    MOV x0, #0x01
    ADCS w2, w1, w1
    ADC x2, x0, x1
    ADC w3, w2, w1
    ADCS w4, w2, w3
    ADCS x5, x2, x0
    ldr x6, =0x100000001
    ldr x7, =0x100000002
    subs x6, x2, x6
    cbnz x6, _skip_pass
    subs x7, x5, x7
    cbnz x7, _skip_pass
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
ENDPROC(asm_test_xu_c3011)
define_asm_testfn asm_test_xu_c3011 0
#else
    .cfi_endproc
#endif
