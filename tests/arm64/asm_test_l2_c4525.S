// ------------------------------------------------------------
// ARMv8-A separate core program
//
// Description: Test case
// ------------------------------------------------------------

#ifdef TESTOS
#include <target/raven.h>
ENTRY(asm_test_l2_c4525)
#else
    .global asm_test_l2_c4525
    .type asm_test_l2_c4525, "function"
    .cfi_startproc
asm_test_l2_c4525:
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
     ldr x0, =ADDR_X0
	ldr x1, =0x1800
     beq core0_addr
     add x0, x0, x1
     
core0_addr:

//No.24 max power
#define RESULT_X4  0x1800

    dmb ish    

    mov x20, x0
    add x22, x20, x1

    mov x2, x22

    mov x1, #1
    mov w1, #1
    mov x3, #0
    mov x4, #0
//    ldr x5, =ADDR_X5_RTL
    mov x6, #0
//    ldr x7, =ADDR_X7_RTL
    mov x8, #0
    mov w9, #0
    mov x10, #0

set_dirty_cpu0:
    strb w1, [x0], #64
    cmp x0, x2
    b.ne set_dirty_cpu0
    dmb ish    
//    ldr x0, =ADDR_X0_RTL
//    ldr x2, =ADDR_X2_RTL
    mov x0, x20
    mov x2, x22
    add x3, x3, #1
    add x0, x0, x3
    add x2, x2, x3
    cmp x3, #64
    b.ne set_dirty_cpu0
    
    dmb ish
//
//    ldr x5, =0x0
//    msr s3_3_c15_c0_6, x5 //set the value of L2CPUSRSELR_EL1 
//
//    smc #0x0000  //trap to EL3 exception program,change state from NS to S
//
//    mov x5, #0x00000002
//    msr s1_0_c11_c2_1, x5 //invalidate l2 v0 bank 
//    orr x6, x5, #0x80
//    msr s1_0_c11_c2_1, x6 //invalidate l2 v1 bank
//
//    smc #0x0000 // trap to EL3 exception program, change state from S to NS
//drop tp EL1

//  ldr x0, =ADDR_X0_RTL
//  ldr x2, =ADDR_X2_RTL
    mov x0, x20
    mov x2, x22

dcache_clean_loop:
    dc civac, x2 
    add x0, x0, #64
    cmp x0, x2
    b.ne dcache_clean_loop

   dmb ish    

//    ldr x0, =ADDR_X0_RTL
    mov x0, x20

l2prefetch_loop_cpu0:
    prfm 0b00010, [x0]
    add x0, x0, #64
    cmp x0, x2
    b.ne l2prefetch_loop_cpu0
  
    dmb ish
  
//    ldr x0, =ADDR_X0_RTL
    mov x0, x20
    mov x3, #0
    mov x5, #0
 
read_loop_cpu0:
    ldrb w1, [x0], #64
    add x3, x3, x1
    cmp x0, x2
    b.ne read_loop_cpu0
    dmb ish
//    ldr x0, =ADDR_X0_RTL
//    ldr x2, =ADDR_X2_RTL
    mov x0, x20
    mov x2, x22
    add x5, x5, #1
    add x0, x0, x5
    add x2, x2, x5
    cmp x5, #64
    b.ne read_loop_cpu0
 
    dmb ish
  
    ldr x4, = RESULT_X4
    cmp x3,x4
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
ENDPROC(asm_test_l2_c4525)
define_asm_testfn asm_test_l2_c4525 0 CPU_EXEC_SYNC
#else
    .cfi_endproc
#endif
