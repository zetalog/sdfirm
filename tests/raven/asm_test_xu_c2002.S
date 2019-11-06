// ------------------------------------------------------------
// ARMv8-A separate core program
//
// Description: Test case
// ------------------------------------------------------------
//
#ifdef TESTOS
#include <target/raven.h>
ENTRY(asm_test_xu_c2002)
#else
    .global asm_test_xu_c2002
    .type asm_test_xu_c2002, "function"
    .cfi_startproc
asm_test_xu_c2002:
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
    ADCS x16, x15, x1
    ADCS x17, x15, x2
    ADCS x18, x15, x1
    ADCS x19, x15, x2

    mov x15, #0xFE10
    mov x15, #0xFE20
    mov x15, #0xFE30
    mov x15, #0xFE40
    mov x15, #0xFE50
    mov x15, #0xFE60
    mov x15, #0xFE70
    mov x15, #0xFE80
    mov x15, #0xFE90

    mov x16, #0xFD10
    mov x16, #0xFD20
    mov x16, #0xFD30
    mov x16, #0xFD40
    mov x16, #0xFD50
    mov x16, #0xFD60
    mov x16, #0xFD70
    mov x16, #0xFD80
    mov x16, #0xFD90

    mov x17, #0xFC10
    mov x17, #0xFC20
    mov x17, #0xFC30
    mov x17, #0xFC40
    mov x17, #0xFC50
    mov x17, #0xFC60
    mov x17, #0xFC70
    mov x17, #0xFC80
    mov x17, #0xFD90

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
ENDPROC(asm_test_xu_c2002)
define_asm_testfn asm_test_xu_c2002 0
#else
    .cfi_endproc
#endif
