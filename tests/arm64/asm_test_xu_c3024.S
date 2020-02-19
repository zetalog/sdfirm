// ------------------------------------------------------------
// ARMv8-A separate core program
//
// Description: Test case
// ------------------------------------------------------------

#ifdef TESTOS
#include <target/raven.h>
ENTRY(asm_test_xu_c3024)
#else
    .global asm_test_xu_c3024
    .type asm_test_xu_c3024, "function"
    .cfi_startproc
asm_test_xu_c3024:
#endif
    nop
    mov w9, 0

    // add your core code
    MOV x1, #0
    MOV x0, #0x01
    cbnz x0, a
    MOV x1, #0x1
    MOV x0, #0x2
a:
    cbnz x1, b
    MOV x1, #0x3
    MOV x0, #0x4
b:
    cbz x0, c
    MOV x1, #0x5
    MOV x0, #0x6
c:
    cbz x1, d
    MOV x1, #0x7
d:
    MOV x0, #0x8
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
ENDPROC(asm_test_xu_c3024)
define_asm_testfn asm_test_xu_c3024 0
#else
    .cfi_endproc
#endif
