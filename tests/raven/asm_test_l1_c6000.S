// ------------------------------------------------------------
// ARMv8-A separate core program
//
// Description: Test case
// ------------------------------------------------------------

#ifdef TESTOS
    #include <testos/raven.h>
ENTRY(asm_test_l1_c0001)
#else
    #include "test_defines.h"

    .global test_0000
    .type test_0000, "function"
    .cfi_startproc
test_0000:
#endif
     // add your core code
     
     // start of the reproduce code
     nop
     mov x13, #-0x1
     mov x14, #-0x8
     mov x12,0
     mov x11,1
_label0:
     nop
     mov w9, 0

     // load sequence
     // x0: 0
     // x1: 4g
     // x2: 64g
     movk x0, #0x0000
     movk x0, #0x0000,      lsl #16
     movk x0, #0x0000,      lsl #32
     movk x0, #0x0000,      lsl #48

     movk x1, #0x0000
     movk x1, #0x0000,      lsl #16
     movk x1, #0x0001,      lsl #32
     movk x1, #0x0000,      lsl #48

     movk x2, #0x0000
     movk x2, #0x0000,      lsl #16
     movk x2, #0x0010,      lsl #32
     movk x2, #0x0000,      lsl #48

     strh w0, [x2, #-0x2]

     ldrh w3, [x0]            // set ox0, miss
     ldrh w4, [x1]            // set 0x0, miss
     ldrh w5, [x1, #-0x3]     // set 0x1f, miss
     dsb ish
     isb
     nop
     fmov  d11, x12
     fmov  d1, x12
     fmov  d2, x12
     nop
     nop
     isb
     nop
     nop
     nop
     nop
     nop
     nop
     nop
     nop
     nop
     nop
     nop
     nop
     nop
     nop
     nop
     nop
     nop
     nop
     nop
     nop
     nop
     nop
     nop
     nop
     nop
     nop
     nop
     nop
     nop
     nop
     nop
     nop
     nop
     nop
     nop
     nop
     nop
     nop
     nop
     nop
     nop
     nop
     nop
     nop
     nop
     nop
     nop
     nop
     nop
     nop
     nop
     nop
     nop
     nop
     nop
     nop
     nop
     nop
     nop
     nop
     nop
     nop
     nop
     nop
     nop
     nop
     nop
     nop
     nop
     nop
     nop
     nop
     nop
     nop
     nop
     nop
     nop
     nop
     nop
     nop
     nop
     nop
     nop
     nop
     nop
     nop
     nop
     nop
     nop
     nop
     nop
     nop
     nop
     nop
     nop
     nop
     nop
     nop
     nop
     nop
     nop
     nop
     nop
     nop
     nop
     nop
     nop
     nop
     nop
     nop
     nop
     nop
     nop
     nop
     nop
     nop
     add x1, x1, x13
     add x2, x2, x14
     add x0, x0, x11
     nop
     //ldrh w6, [x1, #-0x1]!    // bug instruction, 0x1f and 0x0, hit and multi-hit
     ld4 {v10.B-v13.B}[6], [x1], x12    // bug instruction, 0x1f and 0x0, hit and multi-hit

     //ldrh w7, [x2, #-0x2]!    // set 0x1f, miss and will fill 0x1f
     ld1 {v0.S}[1], [x2], x12    // set 0x1f, miss and will fill 0x1f
     //ldrh w8, [x0]           // set 0x0,multi-hit
     ld4 {v29.H, v30.H, v31.H, v0.H}[3], [x0], x12

     fadd  d3, d17, d17
     fadd  d4, d18, d18
     fadd  d5, d19, d19
     fadd  d6, d20, d20
     fadd  d7, d29, d29
     fadd  d8, d30, d30
     fadd  d9, d31, d31
     fadd  d10, d24, d24
     fadd  d11, d21, d21
     fadd  d12, d20, d20
     fadd  d13, d19, d19
     fadd  d14, d18, d18
     fadd  d15, d17, d17
     fadd  d16, d16, d16
     fadd  d17, d15, d15
     fadd  d18, d14, d14
     fadd  d19, d13, d13
     fadd  d20, d12, d12
     fadd  d21, d11, d11
     fadd  d22, d10, d10
     fadd  d23, d9, d9
     fadd  d24, d8, d8
     fadd  d25, d7, d7
     fadd  d26, d6, d6
     fadd  d27, d5, d5
     fadd  d28, d4, d4
     fadd  d29, d3, d3
     fadd  d30, d2, d2
     fadd  d31, d1, d1

     // end of the reproduce code
     dsb ish
     isb
     mov w9, 1

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
ENDPROC(asm_test_l1_c0001)
#ifdef CONFIG_TESTOS
define_asm_testfn asm_test_l1_c0001 1 CPU_EXEC_SYNC | CPU_EXEC_DDR
#endif
#else
    .cfi_endproc
#endif
