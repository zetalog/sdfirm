// ------------------------------------------------------------
// ARMv8-A separate core program
//
// Description: Test case
// ------------------------------------------------------------

#ifdef TESTOS
#include <target/raven.h>
ENTRY(asm_test_su_c4014)
#else
    .global asm_test_su_c4014
    .type asm_test_su_c4014, "function"
    .cfi_startproc
asm_test_su_c4014:
#endif
     // add your core code
     nop
     mov w9, 0


     movk x0, #0x1111
     movk x0, #0x2222,      lsl #16
     movk x0, #0x3333,      lsl #32
     movk x0, #0x4444,      lsl #48

     movk x1, #0x5555
     movk x1, #0x6666,      lsl #16
     movk x1, #0x7777,      lsl #32
     movk x1, #0x8888,      lsl #48

     movk x2, #0x9999
     movk x2, #0xaaaa,      lsl #16
     movk x2, #0xbbbb,      lsl #32
     movk x2, #0xcccc,      lsl #48

     movk x3, #0xdddd
     movk x3, #0xeeee,      lsl #16
     movk x3, #0xffff,      lsl #32
     movk x3, #0x1111,      lsl #48

     movk x4, #0x1111
     movk x4, #0x1111,      lsl #16
     movk x4, #0x1111,      lsl #32
     movk x4, #0x1111,      lsl #48

     movk x5, #0x2222
     movk x5, #0x2222,      lsl #16
     movk x5, #0x2222,      lsl #32
     movk x5, #0x2222,      lsl #48

     movk x6, #0x3333
     movk x6, #0x3333,      lsl #16
     movk x6, #0x3333,      lsl #32
     movk x6, #0x3333,      lsl #48

     movk x7, #0x4444
     movk x7, #0x4444,      lsl #16
     movk x7, #0x4444,      lsl #32
     movk x7, #0x4444,      lsl #48

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

     stp x0, x1, [x8], #0x10
     ldp x10,x11, [x8, #-0x10]
     cmp x10, x0
     b.ne _skip_pass
     cmp x11, x1
     b.ne _skip_pass

     stp x2, x3, [x8], #0x10
     ldp x10,x11, [x8, #-0x10]
     cmp x10, x2
     b.ne _skip_pass
     cmp x11, x3
     b.ne _skip_pass

     stp x4, x5, [x8], #0x10
     ldp x10,x11, [x8, #-0x10]
     cmp x10, x4
     b.ne _skip_pass
     cmp x11, x5
     b.ne _skip_pass

     stp x6, x7, [x8], #0x10
     ldp x10,x11, [x8, #-0x10]
     cmp x10, x6
     b.ne _skip_pass
     cmp x11, x7
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
ENDPROC(asm_test_su_c4014)
define_asm_testfn asm_test_su_c4014 0 CPU_EXEC_SYNC
#else
    .cfi_endproc
#endif
