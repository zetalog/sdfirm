// ------------------------------------------------------------
// ARMv8-A separate core program
//
// Description: Test case
// ------------------------------------------------------------
//
#ifdef TESTOS
#include <target/raven.h>
ENTRY(asm_test_xu_c2004)
#else
    .global asm_test_xu_c2004
    .type asm_test_xu_c2004, "function"
    .cfi_startproc
asm_test_xu_c2004:
#endif
    nop
    nop
    mov w9, 0

    // add your core code
    movz x15, #0
    mov x15, #0x0110
    mov x1, #0xfffffffffffffff0
    mov x2, #20

    ADCS x16, x15, x1
    ADCS x17, x15, x2
    ADCS x18, x15, x1
    ADCS x19, x15, x2

    mov x15, #0xFF10
    mov x15, #0xFF20
    mov x15, #0xFF30
    mov x16, #0xFF40
    mov x16, #0xFF50
    mov x16, #0xFF60
    mov x17, #0xFF70
    mov x17, #0xFF80
    mov x17, #0xFF90

    ADD x15, x15, 1
    ADD x16, x15, 2
    ADD x17, x16, 3
    ADD x18, x17, 4
    ADD x19, x18, 5
    ADD x20, x19, 6
    ADD x21, x20, 7
    ADD x22, x21, 8
    ADD x23, x22, 9
    ADD x24, x23, 10

    ADD x15, x15, 11
    ADD x16, x15, 12
    ADD x17, x16, 13
    ADD x18, x17, 14
    ADD x19, x18, 15
    ADD x20, x19, 16
    ADD x21, x20, 17
    ADD x22, x21, 18
    ADD x23, x22, 19
    ADD x24, x23, 20

    ADD x15, x15, 21
    ADD x16, x15, 22
    ADD x17, x16, 23
    ADD x18, x17, 24
    ADD x19, x18, 25
    ADD x20, x19, 26
    ADD x21, x20, 27
    ADD x22, x21, 28
    ADD x23, x22, 29
    ADD x24, x23, 30
    ADD x25, x23, 30

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
ENDPROC(asm_test_xu_c2004)
define_asm_testfn asm_test_xu_c2004 0
#else
    .cfi_endproc
#endif
