// ------------------------------------------------------------
// ARMv8-A separate core program
//
// Description: Test case
// ------------------------------------------------------------
//
#ifdef TESTOS
#include <target/raven.h>
ENTRY(asm_test_xu_c2014)
#else
    .global asm_test_xu_c2014
    .type asm_test_xu_c2014, "function"
    .cfi_startproc
asm_test_xu_c2014:
#endif
    nop
    nop
    mov w9, 0

    // add your core code
    movz x15, #0
    mov x1, #01
    mov x2, #02
    mov x3, #03
    mov x4, #04
    mov x5, #05
    mov x6, #06
    mov x7, #07
    mov x8, #10
    mov x9, #11
    mov x10, #12

    mov x15, #0xFF10
    mov x15, #0xFF20
    mov x15, #0xFF30
    mov x16, #0xFF40
    mov x16, #0xFF50
    mov x16, #0xFF60
    mov x17, #0xFF70
    mov x17, #0xFF80
    mov x17, #0xFF90

    MUL x15, x15, x1
    MUL x16, x15, x2
    MUL x17, x16, x3
    MUL x18, x17, x4
    MUL x19, x18, x5
    MUL x20, x19, x6
    MUL x21, x20, x7
    MUL x22, x21, x8
    MUL x23, x22, x9
    MUL x24, x23, x10
    MUL x25, x23, x10

    add x1, x2, x3
    cmp x24, x25
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
ENDPROC(asm_test_xu_c2014)
define_asm_testfn asm_test_xu_c2014 0
#else
    .cfi_endproc
#endif
