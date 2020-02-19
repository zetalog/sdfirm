// ------------------------------------------------------------
// ARMv8-A separate core program
//
// Description: Test case
// ------------------------------------------------------------
//
#ifdef TESTOS
#include <target/raven.h>
ENTRY(asm_test_xu_c2018)
#else
    .global asm_test_xu_c2018
    .type asm_test_xu_c2018, "function"
    .cfi_startproc
asm_test_xu_c2018:
#endif
    // add your core code
    nop
    nop
    mov w9, 0

    movz x15, #0
    mov x15, #0xFF10
    mov x16, #120

    mov x1, #01
    mov x2, #02
    mov x3, #03
    mov x4, #04
    mov x5, #05
    mov x6, #06
    mov x7, #07
    mov x8, #10
    mov x9, #11

    mov x15, #0xFE10
    mov x15, #0xFE20
    mov x15, #0xFE30

    mov x16, #0xFD10
    mov x16, #0xFD20
    mov x16, #0xFD30

    mov x17, #0xFC10
    mov x17, #0xFC20
    mov x17, #0xFD30

    UDIV x15, x15, x1
    UDIV x15, x15, x2
    UDIV x15, x15, x3
    UDIV x15, x15, x4
    UDIV x15, x15, x5
    UDIV x15, x15, x6
    UDIV x15, x15, x7
    UDIV x15, x15, x8
    UDIV x15, x15, x9

    UDIV x16, x16, x1
    UDIV x16, x16, x2
    UDIV x16, x16, x3
    UDIV x16, x16, x4
    UDIV x16, x16, x5
    UDIV x16, x16, x6
    UDIV x16, x16, x7
    UDIV x16, x16, x8
    UDIV x16, x16, x9

    UDIV x17, x17, x1
    UDIV x17, x17, x2
    UDIV x17, x17, x3
    UDIV x17, x17, x4
    UDIV x17, x17, x5
    UDIV x17, x17, x6
    UDIV x17, x17, x7
    UDIV x17, x17, x8
    UDIV x17, x17, x9

    add x1, x2, x3
    cmp x16, x17
    b.ne _skip_pass 
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
ENDPROC(asm_test_xu_c2018)
define_asm_testfn asm_test_xu_c2018 0
#else
    .cfi_endproc
#endif
