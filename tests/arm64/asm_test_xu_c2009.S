// ------------------------------------------------------------
// ARMv8-A separate core program
//
// Description: Test case
// ------------------------------------------------------------
//
#ifdef TESTOS
#include <target/raven.h>
ENTRY(asm_test_xu_c2009)
#else
    .global asm_test_xu_c2009
    .type asm_test_xu_c2009, "function"
    .cfi_startproc
asm_test_xu_c2009:
#endif
    nop
    nop
    mov w9, 0

    // add your core code
    movz x15, #0
    mov x15, #0x0110
    mov x1, #0xfffffffffffffff0
    mov x2, #20

    SBCS x16, x15, x1
    SBCS x17, x15, x2
    SBCS x18, x15, x1
    SBCS x19, x15, x2

    mov x15, #0xFF10
    mov x15, #0xFF20
    mov x15, #0xFF30
    mov x16, #0xFF40
    mov x16, #0xFF50
    mov x16, #0xFF60
    mov x17, #0xFF70
    mov x17, #0xFF80
    mov x17, #0xFF90

    SUB x15, x15, 1
    SUB x16, x15, 2
    SUB x17, x16, 3
    SUB x18, x17, 4
    SUB x19, x18, 5
    SUB x20, x19, 6
    SUB x21, x20, 7
    SUB x22, x21, 8
    SUB x23, x22, 9
    SUB x24, x23, 10

    SUB x15, x15, 11
    SUB x16, x15, 12
    SUB x17, x16, 13
    SUB x18, x17, 14
    SUB x19, x18, 15
    SUB x20, x19, 16
    SUB x21, x20, 17
    SUB x22, x21, 18
    SUB x23, x22, 19
    SUB x24, x23, 20

    SUB x15, x15, 21
    SUB x16, x15, 22
    SUB x17, x16, 23
    SUB x18, x17, 24
    SUB x19, x18, 25
    SUB x20, x19, 26
    SUB x21, x20, 27
    SUB x22, x21, 28
    SUB x23, x22, 29
    SUB x24, x23, 30
    SUB x25, x23, 30

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
ENDPROC(asm_test_xu_c2009)
define_asm_testfn asm_test_xu_c2009 0
#else
    .cfi_endproc
#endif
