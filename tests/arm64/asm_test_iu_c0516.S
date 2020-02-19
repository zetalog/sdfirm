// ------------------------------------------------------------
// ARMv8-A separate core program
//
// Description: Test case
// ------------------------------------------------------------
//
#ifdef TESTOS
#include <target/raven.h>
ENTRY(asm_test_iu_c0516)
#else
    .global asm_test_iu_c0516
    .type asm_test_iu_c0516, "function"
    .cfi_startproc
asm_test_iu_c0516:
#endif

    // add your core code
    mov x10, #0
top_loop:
    add x10, x10, #1
    mov x11, #0
    str x30, [sp, #-0x8]
inner_loop:
    add x11, x11, #1
    UDIV x13, x10, x11
    MUL x14, x13, x11
    SUB  x15, x10, x14
    cmp x15, #0
    b.eq test_branch_I0
    mov x9 , #0
    mov x9 , #0
    mov x9 , #0
    mov x9 , #0
    mov x9 , #0
    mov x9 , #0
    mov x9 , #0
    mov x9 , #0
    mov x9 , #0
    mov x9 , #0
    mov x9 , #0
    mov x9 , #0
    mov x9 , #0
    mov x9 , #0
    mov x9 , #0
    mov x9 , #0
test_branch_I0:
    cmp x15, #0
    b.eq test_branch_I1
    mov x9 , #0
    mov x9 , #0
    mov x9 , #0
    mov x9 , #0
    mov x9 , #0
    mov x9 , #0
    mov x9 , #0
    mov x9 , #0
test_branch_I1:
    cmp x11, #4
    b.ne inner_loop
    cmp x10, #30
    b.ne top_loop
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
ENDPROC(asm_test_iu_c0516)
define_asm_testfn asm_test_iu_c0516 0
#else
    .cfi_endproc
#endif
