// ------------------------------------------------------------
// ARMv8-A separate core program
//
// Description: Test case
// ------------------------------------------------------------

#ifdef TESTOS
#include <target/raven.h>
ENTRY(asm_test_l2_c4503)
#else
    .global asm_test_l2_c4503
    .type asm_test_l2_c4503, "function"
    .cfi_startproc
asm_test_l2_c4503:
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
     beq core0_addr
     ldr x1, =0x1800
     add x2, x2, x1
core0_addr:

//c4503.store miss, use V0 PLRU
     mov x3, #7
     str x3, [x2]
     isb
     ldr x4, [x2]
     cmp x4, #7
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
ENDPROC(asm_test_l2_c4503)
define_asm_testfn asm_test_l2_c4503 0 CPU_EXEC_SYNC
#else
    .cfi_endproc
#endif
