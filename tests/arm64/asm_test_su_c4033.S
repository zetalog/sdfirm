// ------------------------------------------------------------
// ARMv8-A separate core program
//
// Description: Test case
// ------------------------------------------------------------

#ifdef TESTOS
#include <target/raven.h>
ENTRY(asm_test_su_c4033)
#else
    .global asm_test_su_c4033
    .type asm_test_su_c4033, "function"
    .cfi_startproc
asm_test_su_c4033:
#endif
     // add your core code
     nop
     mov w9, 0

#ifdef TESTOS
    ldr x8, =x5_data
#elif defined(RTL_SIM)
     movk x8, #0x0000
     movk x8, #0x7002,      lsl #16
     movk x8, #0x00FF,      lsl #32
     movk x8, #0x0000,      lsl #48
#else
     movk x8, #0x0000
     movk x8, #0x8002,      lsl #16
     movk x8, #0x0000,      lsl #32
     movk x8, #0x0000,      lsl #48
#endif

     movk x10, #0x1111
     movk x10, #0x2222,      lsl #16
     movk x10, #0x3333,      lsl #32
     movk x10, #0x4444,      lsl #48

     str w10, [x8], #0x0
     dsb ish

     ldr w11, [x8], #0x0

     cmp w11, w10



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
ENDPROC(asm_test_su_c4033)
define_asm_testfn asm_test_su_c4033 0 CPU_EXEC_SYNC
#else
    .cfi_endproc
#endif
