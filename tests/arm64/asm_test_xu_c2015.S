// ------------------------------------------------------------
// ARMv8-A separate core program
//
// Description: Test case
// ------------------------------------------------------------
//
#ifdef TESTOS
#include <target/raven.h>
ENTRY(asm_test_xu_c2015)
#else
    .global asm_test_xu_c2015
    .type asm_test_xu_c2015, "function"
    .cfi_startproc
asm_test_xu_c2015:
#endif
    nop
    nop
    mov w9, 0

    // add your core code
    mov x15, #0xFF
    mov x1, 2
    UDIV x15, x15, x1
    UDIV x15, x15, x1
    UDIV x15, x15, x1
    UDIV x15, x15, x1
    UDIV x15, x15, x1
    UDIV x15, x15, x1
    UDIV x15, x15, x1
    UDIV x15, x15, x1
    UDIV x15, x15, x1

    mov x15, #0xFF10
    mov x16, #0xFF20
    mov x17, #0xFF30
    mov x18, #0xFF40
    mov x19, #0xFF50
    mov x20, #0xFF60
    mov x21, #0xFF70

    UDIV x15, x15, x1
    UDIV x16, x15, x1
    UDIV x17, x15, x1
    UDIV x18, x15, x1
    UDIV x19, x15, x1
    UDIV x20, x15, x1
    UDIV x21, x15, x1
    UDIV x22, x15, x1
    mov x16, #0xFF80
    UDIV x23, x16, x1
    UDIV x24, x16, x1
    UDIV x25, x16, x1
    UDIV x26, x16, x1
    UDIV x27, x16, x1
    UDIV x28, x16, x1
    UDIV x29, x16, x1

    UDIV x15, x15, x1
    UDIV x15, x15, x1
    UDIV x15, x15, x1
    UDIV x15, x15, x1
    UDIV x15, x15, x1
    UDIV x15, x15, x1
    UDIV x15, x15, x1
    UDIV x15, x15, x1
    UDIV x15, x15, x1

    mov x15, #0xFF90
    UDIV x15, x15, x1
    UDIV x16, x15, x1
    UDIV x17, x15, x1
    UDIV x18, x15, x1
    UDIV x19, x15, x1
    UDIV x20, x15, x1
    UDIV x21, x15, x1
    UDIV x22, x15, x1

    mov x16, #0xFFA0
    UDIV x23, x16, x1
    UDIV x24, x16, x1
    UDIV x25, x16, x1
    UDIV x26, x16, x1
    UDIV x27, x16, x1
    UDIV x28, x16, x1
    UDIV x29, x16, x1

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
ENDPROC(asm_test_xu_c2015)
define_asm_testfn asm_test_xu_c2015 0
#else
    .cfi_endproc
#endif
