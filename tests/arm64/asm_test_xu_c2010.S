// ------------------------------------------------------------
// ARMv8-A separate core program
//
// Description: Test case
// ------------------------------------------------------------
//
#ifdef TESTOS
#include <target/raven.h>
ENTRY(asm_test_xu_c2010)
#else
    .global asm_test_xu_c2010
    .type asm_test_xu_c2010, "function"
    .cfi_startproc
asm_test_xu_c2010:
#endif
    nop
    nop
    mov w9, 0

    // add your core code
    movz x15, #0
    mov x1, 1
    MUL x15, x15, x1
    MUL x15, x15, x1
    MUL x15, x15, x1
    MUL x15, x15, x1
    MUL x15, x15, x1
    MUL x15, x15, x1
    MUL x15, x15, x1
    MUL x15, x15, x1
    MUL x15, x15, x1

    mov x15, #0xFF10
    mov x16, #0xFF20
    mov x17, #0xFF30
    mov x18, #0xFF40
    mov x19, #0xFF50
    mov x20, #0xFF60
    mov x21, #0xFF70

    MUL x15, x15, x1
    MUL x16, x15, x1
    MUL x17, x15, x1
    MUL x18, x15, x1
    MUL x19, x15, x1
    MUL x20, x15, x1
    MUL x21, x15, x1
    MUL x22, x15, x1
    mov x16, #0xFF80
    MUL x23, x16, x1
    MUL x24, x16, x1
    MUL x25, x16, x1
    MUL x26, x16, x1
    MUL x27, x16, x1
    MUL x28, x16, x1
    MUL x29, x16, x1

    MUL x15, x15, x1
    MUL x15, x15, x1
    MUL x15, x15, x1
    MUL x15, x15, x1
    MUL x15, x15, x1
    MUL x15, x15, x1
    MUL x15, x15, x1
    MUL x15, x15, x1
    MUL x15, x15, x1

    mov x15, #0xFF90
    MUL x15, x15, x1
    MUL x16, x15, x1
    MUL x17, x15, x1
    MUL x18, x15, x1
    MUL x19, x15, x1
    MUL x20, x15, x1
    MUL x21, x15, x1
    MUL x22, x15, x1

    mov x16, #0xFFA0
    MUL x23, x16, x1
    MUL x24, x16, x1
    MUL x25, x16, x1
    MUL x26, x16, x1
    MUL x27, x16, x1
    MUL x28, x16, x1
    MUL x29, x16, x1

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
ENDPROC(asm_test_xu_c2010)
define_asm_testfn asm_test_xu_c2010 0
#else
    .cfi_endproc
#endif
