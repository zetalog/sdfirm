// ------------------------------------------------------------
// ARMv8-A separate core program
//
// Description: Test case
// ------------------------------------------------------------
//
#ifdef TESTOS
#include <target/raven.h>
ENTRY(asm_test_xu_c2006)
#else
    .global asm_test_xu_c2006
    .type asm_test_xu_c2006, "function"
    .cfi_startproc
asm_test_xu_c2006:
#endif
    nop
    nop
    mov w9, 0

    // add your core code
    movz x15, #0
    mov x15, #110
    mov x16, #120

    mov x1, #0xfffffffffffffff0
    mov x2, #20
    ADCS x16, x15, x1
    ADCS x17, x15, x2
    ADCS x18, x15, x1
    ADCS x19, x15, x2

    mov x3, #0xffffffffffffff30
    mov x4, #40
    mov x5, #0xffffffffffffff50
    mov x6, #60
    mov x7, #0xffffffffffffff70
    mov x8, #0xffffffffffffff80
    mov x9, #90
    mov x10, #0xffffffffffffffa0
    mov x11, #0xffffffffffffffb0

    ADCS x15, x15, x3
    SUB x15, x15, x4
    SUB x15, x15, x5

    SUB x15, x15, x6
    SUB x15, x15, x7
    SUB x15, x15, x8

    SUB x15, x15, x9
    SUB x15, x15, x1
    SUB x15, x15, x2

    SUB x15, x15, x3
    SUB x15, x15, x4
    SUB x15, x15, x5

    ADCS x15, x15, x6
    ADCS x15, x15, x7
    ADCS x15, x15, x8
    ADCS x15, x15, x9

    mov x15, #0xFF90
    SUBS x15, x15, 1
    SUBS x16, x15, 1
    SUBS x17, x15, 1
    SUBS x18, x15, 1
    SUBS x19, x15, 1
    SUBS x20, x15, 1
    SUBS x21, x15, 1
    SUBS x22, x15, 1

    mov x16, #0xFFA0
    SUBS x23, x16, 1
    SUBS x24, x16, 1
    SUBS x25, x16, 1
    SUBS x26, x16, 1
    SUBS x27, x16, 1
    SUBS x28, x16, 1
    SUBS x29, x16, 1

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
ENDPROC(asm_test_xu_c2006)
define_asm_testfn asm_test_xu_c2006 0
#else
    .cfi_endproc
#endif
