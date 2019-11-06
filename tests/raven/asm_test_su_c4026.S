// ------------------------------------------------------------
// ARMv8-A separate core program
//
// Description: Test case
// ------------------------------------------------------------

#ifdef TESTOS
#include <target/raven.h>
ENTRY(asm_test_su_c4026)
#else
    .global asm_test_su_c4026
    .type asm_test_su_c4026, "function"
    .cfi_startproc
asm_test_su_c4026:
#endif
     // add your core code
     nop
     mov w9, 0

#ifdef TESTOS
     ldr x8, =x5_data
#elif defined(RTL_SIM)
     movk x8, #0x0080
     movk x8, #0x7000,      lsl #16
     movk x8, #0x00FF,      lsl #32
     movk x8, #0x0000,      lsl #48
#else
     movk x8, #0x0080
     movk x8, #0x8000,      lsl #16
     movk x8, #0x0000,      lsl #32
     movk x8, #0x0000,      lsl #48
#endif

     mov x10, 1
     str XZR, [x8]

lock_check:
     ldaxr w11, [x8]
     cbnz  w11, lock_check

     stxr w5, x10, [x8]
     cbnz w5, lock_check

unlock:
     stlr WZR, [x8]



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
ENDPROC(asm_test_su_c4026)
define_asm_testfn asm_test_su_c4026 0 CPU_EXEC_SYNC
#else
    .cfi_endproc
#endif
