// ------------------------------------------------------------
// ARMv8-A separate core program
//
// Description: Test case
// ------------------------------------------------------------

#ifdef TESTOS
#include <target/raven.h>
ENTRY(asm_test_su_c4019)
#else
    .global asm_test_su_c4019
    .type asm_test_su_c4019, "function"
    .cfi_startproc
asm_test_su_c4019:
#endif
     // add your core code
     nop
     mov w9, 0


#ifdef TESTOS
    ldr x8, =x5_data
#elif defined(RTL_SIM)
     movk x8, #0x0000
     movk x8, #0x7000,      lsl #16
     movk x8, #0x00FF,      lsl #32
     movk x8, #0x0000,      lsl #48
#else
     movk x8, #0x0000
     movk x8, #0x8000,      lsl #16
     movk x8, #0x0000,      lsl #32
     movk x8, #0x0000,      lsl #48
#endif


     mov x10, 5
     stxr w11, x8, [x8]
     add x8, x8, 0x10
     //str x8, [x8]
     //stlr  x10, [x8]

     //cas x9, x8, [x8]
/*
     dc ivac, x8
     str x9, [x8]
*/
//     ldr x9, [x8]
     nop
     nop
//     dsb ish
     nop
//     dc ivac, x8
     /*
     str x9, [x8]

     // strex with data, with proceed, snoop kill required if fail if innercacheable
     add x8, x8, #0x40
     //ldr x9, [x8]
     //cas x9, x8, [x8]
     stxr w10, x9, [x8]

     // strex without data, without proceed, no snoop kill required if fail
_strex_no_kill:
     dsb ish
     nop
     nop
     stxr w10, x9, [x8]
*/


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
ENDPROC(asm_test_su_c4019)
define_asm_testfn asm_test_su_c4019 0 CPU_EXEC_SYNC
#else
    .cfi_endproc
#endif
