// ------------------------------------------------------------
// ARMv8-A separate core program
//
// Description: Test case
// ------------------------------------------------------------
//
#ifdef TESTOS
#include <target/raven.h>
ENTRY(asm_test_xu_c2016)
#else
    .global asm_test_xu_c2016
    .type asm_test_xu_c2016, "function"
    .cfi_startproc
asm_test_xu_c2016:
#endif
    nop
    nop
    mov w9, 0

    // add your core code
    mov x15, #110
    mov x16, #120

    mov x1, #0xfffffffffffffff0
    mov x2, #20
    ADCS x16, x15, x1
    ADCS x17, x15, x2
    ADCS x18, x15, x1
    ADCS x19, x15, x2

    mov x6, #60
    mov x7, #70
    mov x8, #80
    mov x9, #90

    UDIV x15, x15, x6
    UDIV x15, x15, x7
    UDIV x15, x15, x8
    UDIV x15, x15, x9

    mov x15, #0xFF90
    ADDS x15, x15, 1
    ADDS x16, x15, 1
    ADDS x17, x15, 1
    ADDS x18, x15, 1
    ADDS x19, x15, 1
    ADDS x20, x15, 1
    ADDS x21, x15, 1
    UDIV x22, x15, x6

    mov x16, #0xFFA0
    ADDS x23, x16, 1
    ADDS x24, x16, 1
    ADDS x25, x16, 1
    ADDS x26, x16, 1
    ADDS x27, x16, 1
    UDIV x28, x16, x27
    UDIV x29, x16, x27

    add x1, x2, x3
    cmp x28, x29
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
ENDPROC(asm_test_xu_c2016)
define_asm_testfn asm_test_xu_c2016 0
#else
    .cfi_endproc
#endif
