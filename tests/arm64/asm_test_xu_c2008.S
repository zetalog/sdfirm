// ------------------------------------------------------------
// ARMv8-A separate core program
//
// Description: Test case
// ------------------------------------------------------------
//
#ifdef TESTOS
#include <target/raven.h>
ENTRY(asm_test_xu_c2008)
#else
    .global asm_test_xu_c2008
    .type asm_test_xu_c2008, "function"
    .cfi_startproc
asm_test_xu_c2008:
#endif
    nop
    nop
    mov w9, 0

    // add your core code
    movz x15, #0
    mov x15, #0xFF10
    mov x16, #120

    mov x1, #0xfffffffffffffff0
    mov x2, #20
    SBCS x16, x15, x1
    SBCS x17, x15, x2
    SBCS x18, x15, x1
    SBCS x19, x15, x2

    mov x15, #0xFE10
    mov x15, #0xFE20
    mov x15, #0xFE30

    mov x16, #0xFD10
    mov x16, #0xFD20
    mov x16, #0xFD30

    mov x17, #0xFC10
    mov x17, #0xFC20
    mov x17, #0xFD30

    SUB x15, x15, 1
    SUB x15, x15, 2
    SUB x15, x15, 3
    SUB x15, x15, 4
    SUB x15, x15, 5
    SUB x15, x15, 6
    SUB x15, x15, 7
    SUB x15, x15, 8
    SUB x15, x15, 9

    SUB x16, x16, 1
    SUB x16, x16, 2
    SUB x16, x16, 3
    SUB x16, x16, 4
    SUB x16, x16, 5
    SUB x16, x16, 6
    SUB x16, x16, 7
    SUB x16, x16, 8
    SUB x16, x16, 9

    SUB x17, x17, 1
    SUB x17, x17, 2
    SUB x17, x17, 3
    SUB x17, x17, 4
    SUB x17, x17, 5
    SUB x17, x17, 6
    SUB x17, x17, 7
    SUB x17, x17, 8
    SUB x17, x17, 9

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
ENDPROC(asm_test_xu_c2008)
define_asm_testfn asm_test_xu_c2008 0
#else
    .cfi_endproc
#endif
