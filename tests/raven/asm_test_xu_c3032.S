// ------------------------------------------------------------
// ARMv8-A separate core program
//
// Description: Test case
// ------------------------------------------------------------

#ifdef TESTOS
#include <target/raven.h>
ENTRY(asm_test_xu_c3032)
#else
    .global asm_test_xu_c3032
    .type asm_test_xu_c3032, "function"
    .cfi_startproc
asm_test_xu_c3032:
#endif
    nop
    mov w9, 0

    // add your core code
    orr v0.4s, #0x3d
    orr v1.4s, #0x41
    orr v2.4s, #0xaf
    orr v3.4s, #0xf2
    orr v4.4s, #0x92
    aesd v1.16b, v0.16b
    aese v2.16b, v1.16b
    aesimc v3.16b, v2.16b
    aesmc v4.16b, v3.16b
    sha1c q5, s4, v3.4s
    sha1m q7, s2, v1.4s
    sha1p q8, s2, v1.4s
    sha1h s6, s4
    sha1su0 v9.4s, v7.4s, v8.4s
    sha1su1 v10.4s, v8.4s
    sha256h q11, q7, v1.4s
    sha256h2 q12, q3, v8.4s
    sha256su0 v13.4s, v11.4s
    sha256su1 v14.4s, v10.4s, v12.4s
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
ENDPROC(asm_test_xu_c3032)
define_asm_testfn asm_test_xu_c3032 0
#else
    .cfi_endproc
#endif
