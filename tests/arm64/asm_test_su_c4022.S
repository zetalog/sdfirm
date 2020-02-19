// ------------------------------------------------------------
// ARMv8-A separate core program
//
// Description: Test case
// ------------------------------------------------------------

#ifdef TESTOS
#include <target/raven.h>
ENTRY(asm_test_su_c4022)
#else
    .global asm_test_su_c4022
    .type asm_test_su_c4022, "function"
    .cfi_startproc
asm_test_su_c4022:
#endif
     // add your core code

     nop
     nop
#ifdef TESTOS
    ldr x0, =x5_data
#elif defined(RTL_SIM)
     movk x0, #0x0000
     movk x0, #0x7002,      lsl #16
     movk x0, #0x00FF,      lsl #32
     movk x0, #0x0000,      lsl #48
#else
     movk x0, #0x0000
     movk x0, #0x8002,      lsl #16
     movk x0, #0x0000,      lsl #32
     movk x0, #0x0000,      lsl #48
#endif
     movk x1, #0xffff
     movk x1, #0xffff,      lsl #16
     movk x1, #0xffff,      lsl #32
     movk x1, #0xffff,      lsl #48

     str  x1, [x0], #0x8
     sub  x1, x1, 1
     str  x1, [x0], #0x8
     sub  x1, x1, 1
     str  x1, [x0], #0x8
     sub  x1, x1, 1
     str  x1, [x0], #0x8
     sub  x1, x1, 1
     str  x1, [x0], #0x8
     sub  x1, x1, 1
     str  x1, [x0], #0x8
     sub  x1, x1, 1
     str  x1, [x0], #0x8
     sub  x1, x1, 1
     str  x1, [x0], #0x8
     sub  x1, x1, 1

     ldr  x2, [x0, #-0x40]
     ldr  x3, [x0, #-0x38]
     ldr  x4, [x0, #-0x30]
     ldr  x5, [x0, #-0x28]
     ldr  x6, [x0, #-0x20]
     ldr  x7, [x0, #-0x18]
     ldr  x8, [x0, #-0x10]
     ldr  x9, [x0, #-0x8]

     sub  x2, x2, x3   //1
     sub  x3, x3, x4   //1
     sub  x4, x4, x5   //1
     sub  x5, x5, x6   //1
     sub  x6, x6, x7   //1
     sub  x7, x7, x8   //1
     sub  x8, x8, x9   //1
     sub  x9, x9, x1   //1

     add  x2, x2, x3
     add  x3, x2, x4
     add  x4, x3, x5
     add  x5, x4, x6
     add  x6, x5, x7
     add  x7, x6, x8
     add  x8, x7, x9

     cmp  x8, 8
     mov  w9, 0
     b.ne _skip_pass
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
ENDPROC(asm_test_su_c4022)
define_asm_testfn asm_test_su_c4022 0 CPU_EXEC_SYNC
#else
    .cfi_endproc
#endif
