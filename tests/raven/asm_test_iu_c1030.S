// ------------------------------------------------------------
// ARMv8-A separate core program
//
// Description: Test case
// ------------------------------------------------------------
//
#ifdef TESTOS
#include <target/raven.h>
ENTRY(asm_test_iu_c1030)
#else
    .global asm_test_iu_c1030
    .type asm_test_iu_c1030, "function"
    .cfi_startproc
asm_test_iu_c1030:
#endif
     // add your core code

#ifdef TESTOS
#define ADDR_BASE test_array
#else
#define ADDR_BASE 0xff70004000
#endif

// 3 + 8 +1
#define QTY_WAY    		0xD

// set(0-63) (this set located in every way would be executed circularly)
#define TESTED_SET 		0x1

//way(QTY_WAY-1) set(TESTED_SET)
// {x16[3:0], x17[5:0]} == {4'bxxxx, 6'bxxxxxx}
//#define COMPARE_VALUE 	0x301
#define COMPARE_VALUE 	0xD0

#define CYCLE_TIMES     0x10

	ldr x11, =ADDR_BASE
//--------------------
    mov x16, #0    // cnt for way
loop_way_begin:
	cmp x16, #QTY_WAY
	b.eq loop_way_end
	lsl x18, x16, #13
	add x15, x18, x11
//--------------------
    mov x17, #0    // cnt for set
loop_set_begin:
	cmp x17, #0x40
	b.eq loop_set_end
	// calculate addr
	lsl x19, x17, #7
	orr x12, x15, x19
//--------------------

	ldr x13, =0xD2800000	// mov x0, #0, imm is located in [20:5]
	orr x13, x13, x17, lsl #5		// x13[10:5] = x17[5:0]
	orr x13, x13, x16, lsl #11		// x13[14:11] = x16[3:0]
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
//	str x13, [x12], #0x8	// inst 8 & 9
//	str x13, [x12], #0x8	// inst 10 & 11
//	str x13, [x12], #0x8	// inst 12 & 13
//	str x13, [x12], #0x8	// inst 14 & 15
//	str x13, [x12], #0x8	// inst 16 & 17
//	str x13, [x12], #0x8	// inst 18 & 19
//	str x13, [x12], #0x8	// inst 20 & 21
//	str x13, [x12], #0x8	// inst 22 & 23
//	str x13, [x12], #0x8	// inst 24 & 25
//	str x13, [x12], #0x8	// inst 26 & 27
//	str x13, [x12], #0x8	// inst 28 & 29
//	str x13, [x12], #0x0	// inst 30 & 31

//--------------------
	add x17, x17, #0x1
	b loop_set_begin
loop_set_end:
//--------------------
	add x16, x16, #0x1
	b loop_way_begin
loop_way_end:
//--------------------

	dsb sy
	ic iallu
	isb

//////////
//--------------------
    mov x1, #0
    mov x21, #0
loop_test_outer_begin:
	cmp x21, #CYCLE_TIMES
	b.eq loop_test_outer_end
//--------------------
	ldr x12, =ADDR_BASE
	mov x23, #TESTED_SET
	orr x12, x12, x23, lsl #7
	mov x22, #0
loop_test_inner_begin:
	cmp x22, #QTY_WAY
	b.eq loop_test_inner_end
//--------------------
	//save upper level pointer for link  address
	str x30, [sp, #-0x8]
	blr x12
	//restore upper level pointer for link  address
    ldr x30, [sp, #-0x8]

	add x1, x1, #0x1
	add x12, x12, #0x2000
//--------------------
	add x22, x22, #0x1
	b loop_test_inner_begin
loop_test_inner_end:
//--------------------
	add x21, x21, #0x1
	b loop_test_outer_begin
loop_test_outer_end:
//--------------------

	mov w9, 1
	cmp x1, #COMPARE_VALUE
	b.eq test_pass
	// test failure
	mov w9, 0

test_pass:
	nop

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
ENDPROC(asm_test_iu_c1030)
define_asm_testfn asm_test_iu_c1030 0 CPU_EXEC_SYNC
#else
    .cfi_endproc
#endif
