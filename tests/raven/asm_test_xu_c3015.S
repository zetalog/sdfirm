// ------------------------------------------------------------
// ARMv8-A separate core program
//
// Description: Test case
// ------------------------------------------------------------

#ifdef TESTOS
#include <target/raven.h>
ENTRY(asm_test_xu_c3015)
#else
    .global asm_test_xu_c3015
    .type asm_test_xu_c3015, "function"
    .cfi_startproc
asm_test_xu_c3015:
#endif
    nop
    mov w9, 0

    // add your core code
    MOV x1, #0xffffffff
    MOV x0, #0x01
    ASR w2, w0, w1
    ASR x3, x0, x1
    ASR w4, w3, #10
    ASR x5, x3, #32
    ASRV w6, w4, w5
    ASRV x7, x3, x4
    ldr x8, =0x0
    ldr x9, =0x0
    subs x8, x3, x8
    cbnz x8, _skip_pass
    subs x9, x6, x9
    cbnz x9, _skip_pass
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
ENDPROC(asm_test_xu_c3015)
define_asm_testfn asm_test_xu_c3015 0
#else
    .cfi_endproc
#endif
