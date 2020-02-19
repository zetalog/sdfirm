// ------------------------------------------------------------
// ARMv8-A separate core program
//
// Description: Test case
// ------------------------------------------------------------

#ifdef TESTOS
#include <target/raven.h>
ENTRY(asm_test_su_c4010)
#else
    .global asm_test_su_c4010
    .type asm_test_su_c4010, "function"
    .cfi_startproc
asm_test_su_c4010:
#endif
     // add your core code
     nop
     mov w9, 0

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

     str  x1, [x0], #0x40
     sub  x1, x1, 1
     str  x1, [x0], #0x40
     sub  x1, x1, 1
     str  x1, [x0], #0x40
     sub  x1, x1, 1
     str  x1, [x0], #0x40
     sub  x1, x1, 1
     str  x1, [x0], #0x40
     sub  x1, x1, 1
     str  x1, [x0], #0x40
     sub  x1, x1, 1
     str  x1, [x0], #0x40
     sub  x1, x1, 1
     str  x1, [x0], #0x40
     sub  x1, x1, 1
     str  x1, [x0], #0x40
     sub  x1, x1, 1
     str  x1, [x0], #0x40
     sub  x1, x1, 1
     str  x1, [x0], #0x40
     sub  x1, x1, 1
     str  x1, [x0], #0x40
     sub  x1, x1, 1
     str  x1, [x0], #0x40
     sub  x1, x1, 1
     str  x1, [x0], #0x40
     sub  x1, x1, 1
     str  x1, [x0], #0x40
     sub  x1, x1, 1
     str  x1, [x0], #0x40
     sub  x1, x1, 1
     str  x1, [x0], #0x40
     sub  x1, x1, 1
     str  x1, [x0], #0x40
     sub  x1, x1, 1
     str  x1, [x0], #0x40
     sub  x1, x1, 1
     str  x1, [x0], #0x40
     sub  x1, x1, 1
     str  x1, [x0], #0x40
     sub  x1, x1, 1
     str  x1, [x0], #0x40
     sub  x1, x1, 1
     str  x1, [x0], #0x40
     sub  x1, x1, 1
     str  x1, [x0], #0x40
     sub  x1, x1, 1
     str  x1, [x0], #0x40
     sub  x1, x1, 1
     str  x1, [x0], #0x40
     sub  x1, x1, 1

     str  x1, [x0], #0x40
     sub  x1, x1, 1


     str  x1, [x0], #0x40
     sub  x1, x1, 1
     str  x1, [x0], #0x40
     sub  x1, x1, 1
     str  x1, [x0], #0x40
     sub  x1, x1, 1
     str  x1, [x0], #0x40
     sub  x1, x1, 1
     str  x1, [x0], #0x0
     sub  x1, x1, 1


     ldr  x2, [x0], #-0x40
     ldr  x3, [x0], #-0x40
     ldr  x4, [x0], #-0x40
     ldr  x5, [x0], #-0x40
     ldr  x6, [x0], #-0x40
     ldr  x7, [x0], #-0x40
     ldr  x8, [x0], #-0x40
     ldr  x9, [x0], #-0x40
     ldr  x10, [x0], #-0x40
     ldr  x11, [x0], #-0x40
     ldr  x12, [x0], #-0x40
     ldr  x13, [x0], #-0x40
     ldr  x14, [x0], #-0x40
     ldr  x15, [x0], #-0x40
     ldr  x16, [x0], #-0x40
     ldr  x17, [x0], #-0x40

     sub  x1, x2, x1   //1
     sub  x2, x3, x2   //1
     sub  x3, x4, x3   //1
     sub  x4, x5, x4   //1
     sub  x5, x6, x5   //1
     sub  x6, x7, x6   //1
     sub  x7, x8, x7   //1
     sub  x8, x9, x8   //1
     sub  x9, x10, x9  //1
     sub  x10, x11, x10  //1
     sub  x11, x12, x11  //1
     sub  x12, x13, x12  //1
     sub  x13, x14, x13  //1
     sub  x14, x15, x14  //1
     sub  x15, x16, x15  //1
     sub  x16, x17, x16  //1

     add  x2, x2, x1
     add  x3, x2, x3
     add  x4, x3, x4
     add  x5, x4, x5
     add  x6, x5, x6
     add  x7, x6, x7
     add  x8, x7, x8
     add  x9, x8, x9
     add  x10, x9, x10
     add  x11, x10, x11
     add  x12, x11, x12
     add  x13, x12, x13
     add  x14, x13, x14
     add  x15, x14, x15
     add  x16, x15, x16

     mov  w9, 0
     cmp  x16, 16
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
ENDPROC(asm_test_su_c4010)
define_asm_testfn asm_test_su_c4010 0 CPU_EXEC_SYNC
#else
    .cfi_endproc
#endif
