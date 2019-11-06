// ------------------------------------------------------------
// ARMv8-A separate core program
//
// Description: Test case
// ------------------------------------------------------------
//
#ifdef TESTOS
#include <target/raven.h>
ENTRY(asm_test_xu_c2102)
#else
    .global asm_test_xu_c2102
    .type asm_test_xu_c2102, "function"
    .cfi_startproc
asm_test_xu_c2102:
#endif
    nop
    nop
    mov w9, 0

    // add your core code
    fmov v0.2d, #2.0
    fmov v1.2d, #1.0
    mov  x0, #0
    mov  x1, #1
    fmul v2.2d, v0.2d, v1.2d
    mul  x2, x0, x1

    mov x20, #0
    mov x10, #0x9
loop_test_begin:
    cmp x20, #0x9
    b.eq loop_test_end

    mul  x3, x2, x1
    fmla v3.2d, v2.2d, v1.2d

    mul  x4, x3, x1
    fmla v4.2d, v3.2d, v1.2d

    mul  x5, x4, x1
    fmla v5.2d, v4.2d, v1.2d

    mul  x6, x5, x1
    fmla v6.2d, v5.2d, v1.2d

    mul  x7, x6, x1
    fmla v7.2d, v6.2d, v1.2d

    mul  x8, x7, x1

    add x20, x20, #0x1
    sub x10, x10, #0x1
    b loop_test_begin
loop_test_end:
     
    cmp x10, #0
    b.ne _skip_pass
    // end of add your code

    mov w9, 1
_skip_pass:    
#ifdef TESTOS
    mov w0, w9
#else
    // write to address 44'h0FF_7003_FFC0 if your program pass; if fail, write 0
#ifdef RTL_SIM
    movk x8, #0xFFC0
    movk x8, #0x7003,      lsl #16
    movk x8, #0x00FF,      lsl #32
    movk x8, #0x0000,      lsl #48
#else
    movk x8, #0xFFC0
    movk x8, #0x8003,      lsl #16
    movk x8, #0x0000,      lsl #32
    movk x8, #0x0000,      lsl #48
#endif
    str x30, [sp, #-0x8]             // x30 is lr
    bl GetCPUID     // 0-47
    add x8, x8, x0
    strb w9, [x8, #0]
    nop
    dsb ish
    ldr x30, [sp, #-0x8]
#endif
    ret
#ifdef TESTOS
ENDPROC(asm_test_xu_c2102)
define_asm_testfn asm_test_xu_c2102 0
#else
    .cfi_endproc
#endif
