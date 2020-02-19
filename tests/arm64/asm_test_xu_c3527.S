// ------------------------------------------------------------
// ARMv8-A separate core program
//
// Description: Test case
// ------------------------------------------------------------
//
#ifdef TESTOS
#include <target/raven.h>
ENTRY(asm_test_xu_c3527)
#else
    .global asm_test_xu_c3527
    .type asm_test_xu_c3527, "function"
    .cfi_startproc
asm_test_xu_c3527:
#endif
     // add your core code

     //3527 FMA
     fmov d2, #2.0
     fmov d3, #3.0
     fmov d4, #4.0
     fmov d5, #5.0
     fmov d6, #6.0
     fmov d7, #7.0
     fmul d20, d2, d3
     fmul d21, d20, d4
     fmul d22, d21, d5
     fmul d23, d22, d6
     fmul d24, d23, d7
     fmulx d25, d6, d7
     fmulx d26, d25, d5
     fmulx d27, d26, d4
     fmulx d28, d27, d3
     fmulx d29, d28, d2
     fcmeq d30, d24, d29
     fmov x9, d30
     mov x0, 1
     and x9, x9, x0

_skip_pass:
    // end of add your code
    // write to address 44'h0FF_7003_FFC0 if your program pass; if fail, write 0
#ifdef TESTOS
     mov w0, w9
#else
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
ENDPROC(asm_test_xu_c3527)
define_asm_testfn asm_test_xu_c3527 0
#else
    .cfi_endproc
#endif
