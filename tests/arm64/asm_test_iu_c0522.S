// ------------------------------------------------------------
// ARMv8-A separate core program
//
// Description: Test case
// ------------------------------------------------------------
//
#ifdef TESTOS
#include <target/raven.h>
ENTRY(asm_test_iu_c0522)
#else
    .global asm_test_iu_c0522
    .type asm_test_iu_c0522, "function"
    .cfi_startproc
asm_test_iu_c0522:
#endif
     // add your core code

#ifdef TESTOS
#define ADDR_BASE test_array
#else
#define ADDR_BASE 0xff70030000
#endif


	ldr x12, =(ADDR_BASE + 0x4000)

	ldr x13, =0xD2800000	// mov x0, #0, imm is located in [20:5]
    movk x13, #0x201F, lsl #32    // nop (0xD503201F)
    movk x13, #0xD503, lsl #48
	str x13, [x12], #0x8	// inst 0 & 1

	ldr x13, =0xD503201F	// nop (0xD503201F)
    movk x13, #0x201F, lsl #32    // nop (0xD503201F)
    movk x13, #0xD503, lsl #48
	str x13, [x12], #0x8	// inst 2 & 3

	ldr x13, =0xD503201F	// nop (0xD503201F)
    movk x13, #0x201F, lsl #32    // nop (0xD503201F)
    movk x13, #0xD503, lsl #48
	str x13, [x12], #0x8	// inst 4 & 5

	ldr x13, =0xD503201F	// nop (0xD503201F)
    movk x13, #0x03C0, lsl #32    // ret (0xD65F03C0)
    movk x13, #0xD65F, lsl #48
	str x13, [x12], #0x8	// inst 6 & 7

	mov x13, #0
// ------------------------------------------------------------

	ldr x12, =(ADDR_BASE + 0x5000)

	ldr x13, =0xD2800020	// mov x0, #1, imm is located in [20:5]
    movk x13, #0x201F, lsl #32    // nop (0xD503201F)
    movk x13, #0xD503, lsl #48
	str x13, [x12], #0x8	// inst 0 & 1

	ldr x13, =0xD503201F	// nop (0xD503201F)
    movk x13, #0x201F, lsl #32    // nop (0xD503201F)
    movk x13, #0xD503, lsl #48
	str x13, [x12], #0x8	// inst 2 & 3

	ldr x13, =0xD503201F	// nop (0xD503201F)
    movk x13, #0x201F, lsl #32    // nop (0xD503201F)
    movk x13, #0xD503, lsl #48
	str x13, [x12], #0x8	// inst 4 & 5

	ldr x13, =0xD503201F	// nop (0xD503201F)
    movk x13, #0x03C0, lsl #32    // ret (0xD65F03C0)
    movk x13, #0xD65F, lsl #48
	str x13, [x12], #0x8	// inst 6 & 7

	mov x13, #0
// ------------------------------------------------------------

	ldr x12, =(ADDR_BASE + 0x6000)

	ldr x13, =0xD2800020	// mov x0, #1, imm is located in [20:5]
    movk x13, #0x201F, lsl #32    // nop (0xD503201F)
    movk x13, #0xD503, lsl #48
	str x13, [x12], #0x8	// inst 0 & 1

	ldr x13, =0xD503201F	// nop (0xD503201F)
    movk x13, #0x201F, lsl #32    // nop (0xD503201F)
    movk x13, #0xD503, lsl #48
	str x13, [x12], #0x8	// inst 2 & 3

	ldr x13, =0xD503201F	// nop (0xD503201F)
    movk x13, #0x201F, lsl #32    // nop (0xD503201F)
    movk x13, #0xD503, lsl #48
	str x13, [x12], #0x8	// inst 4 & 5

	ldr x13, =0xD503201F	// nop (0xD503201F)
    movk x13, #0x03C0, lsl #32    // ret (0xD65F03C0)
    movk x13, #0xD65F, lsl #48
	str x13, [x12], #0x8	// inst 6 & 7

	mov x13, #0
// ------------------------------------------------------------

	ldr x12, =(ADDR_BASE + 0x7000)

	ldr x13, =0xD2800020	// mov x0, #1, imm is located in [20:5]
    movk x13, #0x201F, lsl #32    // nop (0xD503201F)
    movk x13, #0xD503, lsl #48
	str x13, [x12], #0x8	// inst 0 & 1

	ldr x13, =0xD503201F	// nop (0xD503201F)
    movk x13, #0x201F, lsl #32    // nop (0xD503201F)
    movk x13, #0xD503, lsl #48
	str x13, [x12], #0x8	// inst 2 & 3

	ldr x13, =0xD503201F	// nop (0xD503201F)
    movk x13, #0x201F, lsl #32    // nop (0xD503201F)
    movk x13, #0xD503, lsl #48
	str x13, [x12], #0x8	// inst 4 & 5

	ldr x13, =0xD503201F	// nop (0xD503201F)
    movk x13, #0x03C0, lsl #32    // ret (0xD65F03C0)
    movk x13, #0xD65F, lsl #48
	str x13, [x12], #0x8	// inst 6 & 7

	mov x13, #0

	dsb sy
	ic iallu
	isb

//////////
//--------------------
    mov x21, #0
loop_test_outer_begin:
	cmp x21, 10
	b.eq loop_test_outer_end
//--------------------

	ldr x12, =(ADDR_BASE + 0x4000)
    //save upper level pointer for link  address
	str x30, [sp, #-0x8]
	blr x12
	//restore upper level pointer for link  address
    ldr x30, [sp, #-0x8]

	ldr x12, =(ADDR_BASE + 0x5000)
	//save upper level pointer for link  address
	str x30, [sp, #-0x8]
	blr x12
	//restore upper level pointer for link  address
    ldr x30, [sp, #-0x8]

	ldr x12, =(ADDR_BASE + 0x6000)
	//save upper level pointer for link  address
	str x30, [sp, #-0x8]
	blr x12
	//restore upper level pointer for link  address
    ldr x30, [sp, #-0x8]

	ldr x12, =(ADDR_BASE + 0x7000)
	//save upper level pointer for link  address
	str x30, [sp, #-0x8]
	blr x12
	//restore upper level pointer for link  address
    ldr x30, [sp, #-0x8]

//--------------------
	add x21, x21, #0x1
	b loop_test_outer_begin
loop_test_outer_end:
//--------------------

	mov w9, 1
	cmp x0, #0x1
	b.eq test_pass
	// test failure
	mov w9, 0

test_pass:





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
ENDPROC(asm_test_iu_c0522)
define_asm_testfn asm_test_iu_c0522 0 CPU_EXEC_SYNC
#else
    .cfi_endproc
#endif
