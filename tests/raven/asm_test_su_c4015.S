// ------------------------------------------------------------
// ARMv8-A separate core program
//
// Description: Test case
// ------------------------------------------------------------

#ifdef TESTOS
#include <target/raven.h>
ENTRY(asm_test_su_c4015)
#else
    .global asm_test_su_c4015
    .type asm_test_su_c4015, "function"
    .cfi_startproc
asm_test_su_c4015:
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

//     mrs  x1, midr_el1
//     str  x1, [x8, #0x0]  // this code is to confirm the hxt1 use the 0x68 which is authorized by ARM inc.


     ldr x1, [x8], #0x8
     ldr x1, [x8], #0x8
     ldr x1, [x8], #0x8
     ldr x1, [x8], #0x8
     ldr x1, [x8], #0x8
     ldr x1, [x8], #0x8
     ldr x1, [x8], #0x8
     ldr x1, [x8], #0x8
     ldr x1, [x8], #0x8
     ldr x1, [x8], #0x8
     ldr x1, [x8], #0x8
     ldr x1, [x8], #0x8
     ldr x1, [x8], #0x8
     ldr x1, [x8], #0x8
     ldr x1, [x8], #0x8
     ldr x1, [x8], #0x8
     ldr x1, [x8], #0x8    // the 17th load

     // load from next 2 cachelines
     ldr x1, [x8], #0x8
     ldr x1, [x8], #0x8
     ldr x1, [x8], #0x8
     ldr x1, [x8], #0x8
     ldr x1, [x8], #0x8
     ldr x1, [x8], #0x8
     ldr x1, [x8], #0x8
     ldr x1, [x8], #0x8
     ldr x1, [x8], #0x8
     ldr x1, [x8], #0x8
     ldr x1, [x8], #0x8
     ldr x1, [x8], #0x8
     ldr x1, [x8], #0x8
     ldr x1, [x8], #0x8
     ldr x1, [x8], #0x8



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
ENDPROC(asm_test_su_c4015)
define_asm_testfn asm_test_su_c4015 0 CPU_EXEC_SYNC
#else
    .cfi_endproc
#endif
