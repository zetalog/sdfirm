// ------------------------------------------------------------
// ARMv8-A separate core program
//
// Description: Test case
// ------------------------------------------------------------
#include <target/raven.h>

ENTRY(asm_test_su_c5005)
     // add your core code
     nop
     mov w9, 0
//test write by Xing Penghu

#ifdef TESTOS
#define ADDR_X0  test_array
#define ADDR_X2  (ADDR_X0+0x300)
#define ADDR_X5	 x5_data
#define ADDR_X7	 x7_data
#elif defined(RTL_SIM)
#define ADDR_X0  0xFF7003C000
#define ADDR_X2  0xFF7003F000
#define ADDR_X5  0xFF7003FF00
#define ADDR_X7  0xFF70040000
#else
#define ADDR_X0  0x8003C000
#define ADDR_X2  0x8003F000
#define ADDR_X5  0x8003FF00
#define ADDR_X7  0x80040000
#endif

#define RESULT_X4  0x0294

range_as_loop_cpu0:
//init_ext_mem_cpu0
    
    ldr x0, =ADDR_X0
    mov x1, #0
    ldr x2, =ADDR_X2
    mov x3, #0
    mov x4, #0
    ldr x5, =ADDR_X5
    mov x6, #0
    ldr x7, =ADDR_X7
    mov x8, #0
    mov w9, #0
    mov x10, #0
//init_mem:
//    str x10, [x0]
//    add x0, x0, #8
//    cmp x0, x7
//    b.ne init_mem
//
//    mov x0, x0

//#ifdef RTL_SIM
//    movk x0, #0xC000
//    movk x0, #0x7003, lsl #16
//    movk x0, #0x00FF, lsl #32
//    movk x0, #0x0000, lsl #48
//#else
//    movk x0, #0xC000
//    movk x0, #0x8003, lsl #16
//    movk x0, #0x0000, lsl #32
//    movk x0, #0x0000, lsl #48
//#endif
set_dirty_cpu0:
    str x1, [x0]
    str x1, [x0, #8]
    str x1, [x0, #16]
    str x1, [x0, #24]
    str x1, [x0, #32]
    str x1, [x0, #40]
    str x1, [x0, #48]
    str x1, [x0, #56]
    add x0, x0, #64
    add x1, x1, #1
    cmp x0, x2
    b.ne set_dirty_cpu0

    mov x0, x0

    ldr x0, =ADDR_X0
    ldr x2, =ADDR_X2
    mov x3, #0
    ldr x5, =ADDR_X5

    str x10, [x5]
read_loop_cpu0:
    ldr x6, [x5]
    ldr x1, [x0]
    add x3, x3, x1
    ldr x6, [x5]
    ldr x1, [x0,#8]
    add x3, x3, x1
    ldr x6, [x5]
    ldr x1, [x0]
    add x3, x3, x1
    ldr x6, [x5]
    ldr x1, [x0,#16]
    add x3, x3, x1

    ldr x6, [x5]
    ldr x1, [x0]
    add x3, x3, x1
    ldr x6, [x5]
    ldr x1, [x0,#24]

    ldr x6, [x5]
    ldr x1, [x0]
    add x3, x3, x1
    ldr x6, [x5]
    ldr x1, [x0,#32]

    ldr x6, [x5]
    ldr x1, [x0]
    add x3, x3, x1
    ldr x6, [x5]
    ldr x1, [x0,#40]

    ldr x6, [x5]
    ldr x1, [x0]
    add x3, x3, x1
    ldr x6, [x5]
    ldr x1, [x0,#48]

    ldr x6, [x5]
    ldr x1, [x0]
    add x3, x3, x1
    ldr x6, [x5]
    ldr x1, [x0,#56]
    add x3, x3, x1
    add x0, x0, #64

    cmp x0, x2
    b.ne read_loop_cpu0

    ldr x4, =RESULT_X4
    cmp x3,x4
    b.ne _skip_pass

pass:
    mov w9, 1

_skip_pass:
    // end of add your code
#ifdef TESTOS
    mov w0, w9
#else
    // write to address 44'h0FF_7003_FFC0 if your program pass; if fail, write 0
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
ENDPROC(asm_test_su_c5005)
define_asm_testfn asm_test_su_c5005 0 CPU_EXEC_SYNC
#else
    .cfi_endproc
#endif
