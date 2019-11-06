// ------------------------------------------------------------
// ARMv8-A separate core program
//
// Description: Test case
// ------------------------------------------------------------

#ifdef TESTOS
#include <target/raven.h>
ENTRY(asm_test_l2_c4524)
#else
    .global asm_test_l2_c4524
    .type asm_test_l2_c4524, "function"
    .cfi_startproc
asm_test_l2_c4524:
#endif
     // add your core code
     nop
     mov w9, 0

#ifdef TESTOS
#define ADDR_X0 test_array
#define ADDR_X2 (ADDR_X0+0x3000)
#elif defined(RTL_SIM)
#define ADDR_X0 0xFF7003C000
#define ADDR_X2 0xFF7003F000
#else
#define ADDR_X0 0x8003C000
#define ADDR_X2 0x8003F000
#endif

     str x30, [sp, #-0x8]             // x30 is lr 
     bl GetCPUID
	 ldr x30, [sp, #-0x8]
     and x0, x0, #1
     cmp x0, #0
     ldr x2, =ADDR_X0
     add x2, x2, #0x80
     beq core0_addr
     ldr x1, =0x1800
     add x2, x2, x1
core0_addr:

//c4524.64B write gather in V1 bank
     add x7, x2, #0x10
     add x8, x7, #0x10
     add x9, x8, #0x10
     add x10, x9, #0x10
     add x11, x10, #0x10
     add x12, x11, #0x10
     add x13, x12, #0x10
     add x14, x10, #8

     ldr x3, =0x1
     ldr x4, =0x2
     ldr x5, =0x3
     str x3, [x2]
     dsb ish
     stp x4, x4, [x2]
     stp x5, x5, [x7]
     stp x4, x4, [x8]
     stp x4, x4, [x9]
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
     stp x5,x5, [x10]
     stp x5,x5, [x11]
     stp x5,x5, [x12]
     stp x5,x5, [x13]
     ldr x6, [x14]
     cmp x6, #3
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
ENDPROC(asm_test_l2_c4524)
define_asm_testfn asm_test_l2_c4524 0 CPU_EXEC_SYNC
#else
    .cfi_endproc
#endif
