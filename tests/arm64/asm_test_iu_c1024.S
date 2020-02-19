// ------------------------------------------------------------
// ARMv8-A separate core program
//
// Description: Test case
// ------------------------------------------------------------
//
#ifdef TESTOS
#include <target/raven.h>
ENTRY(asm_test_iu_c1024)
#else
    .global asm_test_iu_c1024
    .type asm_test_iu_c1024, "function"
    .cfi_startproc
asm_test_iu_c1024:
#endif
     // add your core code

#ifdef TESTOS
#define ADDR_BASE test_array
#else
#define ADDR_BASE 0xff70004000
#endif

#define ADDR_FIX (ADDR_BASE + 0x0)
//
	ldr x12, =ADDR_FIX	// address

	ldr x11, =0xD2800020	// mov x0, #1
	movk x11, #0x03C0, lsl #32    // ret
	movk x11, #0xD65F, lsl #48
	str x11, [x12], #0x8	// inst 0 & 1
	ldr x11, =0x0
	str x11, [x12], #0x8	// inst 2 & 3
	str x11, [x12], #0x8	// inst 4 & 5
	str x11, [x12], #0x8	// inst 6 & 7
	str x11, [x12], #0x8	// inst 8 & 9
	str x11, [x12], #0x8	// inst 10 & 11
	str x11, [x12], #0x8	// inst 12 & 13
	str x11, [x12], #0x0	// inst 14 & 15

//
	ldr x12, =(ADDR_FIX + 0x2000)	// address

	ldr x11, =0xD2800040	// mov x0, #2
	movk x11, #0x03C0, lsl #32    // ret
	movk x11, #0xD65F, lsl #48
	str x11, [x12], #0x8	// inst 0 & 1
	ldr x11, =0x0
	str x11, [x12], #0x8	// inst 2 & 3
	str x11, [x12], #0x8	// inst 4 & 5
	str x11, [x12], #0x8	// inst 6 & 7
	str x11, [x12], #0x8	// inst 8 & 9
	str x11, [x12], #0x8	// inst 10 & 11
	str x11, [x12], #0x8	// inst 12 & 13
	str x11, [x12], #0x0	// inst 14 & 15

//
	ldr x12, =(ADDR_FIX + 0x4000)	// address

	ldr x11, =0xD2800060	// mov x0, #3
	movk x11, #0x03C0, lsl #32    // ret
	movk x11, #0xD65F, lsl #48
	str x11, [x12], #0x8	// inst 0 & 1
	ldr x11, =0x0
	str x11, [x12], #0x8	// inst 2 & 3
	str x11, [x12], #0x8	// inst 4 & 5
	str x11, [x12], #0x8	// inst 6 & 7
	str x11, [x12], #0x8	// inst 8 & 9
	str x11, [x12], #0x8	// inst 10 & 11
	str x11, [x12], #0x8	// inst 12 & 13
	str x11, [x12], #0x0	// inst 14 & 15

//
	ldr x12, =(ADDR_FIX + 0x6000)	// address

	ldr x11, =0xD2800080	// mov x0, #4
	movk x11, #0x03C0, lsl #32    // ret
	movk x11, #0xD65F, lsl #48
	str x11, [x12], #0x8	// inst 0 & 1
	ldr x11, =0x0
	str x11, [x12], #0x8	// inst 2 & 3
	str x11, [x12], #0x8	// inst 4 & 5
	str x11, [x12], #0x8	// inst 6 & 7
	str x11, [x12], #0x8	// inst 8 & 9
	str x11, [x12], #0x8	// inst 10 & 11
	str x11, [x12], #0x8	// inst 12 & 13
	str x11, [x12], #0x0	// inst 14 & 15

//
	ldr x12, =(ADDR_FIX + 0x8000)	// address

	ldr x11, =0xD28000A0	// mov x0, #5
	movk x11, #0x03C0, lsl #32    // ret
	movk x11, #0xD65F, lsl #48
	str x11, [x12], #0x8	// inst 0 & 1
	ldr x11, =0x0
	str x11, [x12], #0x8	// inst 2 & 3
	str x11, [x12], #0x8	// inst 4 & 5
	str x11, [x12], #0x8	// inst 6 & 7
	str x11, [x12], #0x8	// inst 8 & 9
	str x11, [x12], #0x8	// inst 10 & 11
	str x11, [x12], #0x8	// inst 12 & 13
	str x11, [x12], #0x0	// inst 14 & 15

//
	ldr x12, =(ADDR_FIX + 0xa000)	// address

	ldr x11, =0xD28000C0	// mov x0, #6
	movk x11, #0x03C0, lsl #32    // ret
	movk x11, #0xD65F, lsl #48
	str x11, [x12], #0x8	// inst 0 & 1
	ldr x11, =0x0
	str x11, [x12], #0x8	// inst 2 & 3
	str x11, [x12], #0x8	// inst 4 & 5
	str x11, [x12], #0x8	// inst 6 & 7
	str x11, [x12], #0x8	// inst 8 & 9
	str x11, [x12], #0x8	// inst 10 & 11
	str x11, [x12], #0x8	// inst 12 & 13
	str x11, [x12], #0x0	// inst 14 & 15

//
	ldr x12, =(ADDR_FIX + 0xc000)	// address

	ldr x11, =0xD28000E0	// mov x0, #7
	movk x11, #0x03C0, lsl #32    // ret
	movk x11, #0xD65F, lsl #48
	str x11, [x12], #0x8	// inst 0 & 1
	ldr x11, =0x0
	str x11, [x12], #0x8	// inst 2 & 3
	str x11, [x12], #0x8	// inst 4 & 5
	str x11, [x12], #0x8	// inst 6 & 7
	str x11, [x12], #0x8	// inst 8 & 9
	str x11, [x12], #0x8	// inst 10 & 11
	str x11, [x12], #0x8	// inst 12 & 13
	str x11, [x12], #0x0	// inst 14 & 15

//
	ldr x12, =(ADDR_FIX + 0xe000)	// address

	ldr x11, =0xD2800100	// mov x0, #8
	movk x11, #0x03C0, lsl #32    // ret
	movk x11, #0xD65F, lsl #48
	str x11, [x12], #0x8	// inst 0 & 1
	ldr x11, =0x0
	str x11, [x12], #0x8	// inst 2 & 3
	str x11, [x12], #0x8	// inst 4 & 5
	str x11, [x12], #0x8	// inst 6 & 7
	str x11, [x12], #0x8	// inst 8 & 9
	str x11, [x12], #0x8	// inst 10 & 11
	str x11, [x12], #0x8	// inst 12 & 13
	str x11, [x12], #0x0	// inst 14 & 15

//
	ldr x12, =(ADDR_FIX + 0x10000)	// address

	ldr x11, =0xD2800120	// mov x0, #9
	movk x11, #0x03C0, lsl #32    // ret
	movk x11, #0xD65F, lsl #48
	str x11, [x12], #0x8	// inst 0 & 1
	ldr x11, =0x0
	str x11, [x12], #0x8	// inst 2 & 3
	str x11, [x12], #0x8	// inst 4 & 5
	str x11, [x12], #0x8	// inst 6 & 7
	str x11, [x12], #0x8	// inst 8 & 9
	str x11, [x12], #0x8	// inst 10 & 11
	str x11, [x12], #0x8	// inst 12 & 13
	str x11, [x12], #0x0	// inst 14 & 15

//
	ldr x12, =(ADDR_FIX + 0x12000)	// address

	ldr x11, =0xD2800140	// mov x0, #10
	movk x11, #0x03C0, lsl #32    // ret
	movk x11, #0xD65F, lsl #48
	str x11, [x12], #0x8	// inst 0 & 1
	ldr x11, =0x0
	str x11, [x12], #0x8	// inst 2 & 3
	str x11, [x12], #0x8	// inst 4 & 5
	str x11, [x12], #0x8	// inst 6 & 7
	str x11, [x12], #0x8	// inst 8 & 9
	str x11, [x12], #0x8	// inst 10 & 11
	str x11, [x12], #0x8	// inst 12 & 13
	str x11, [x12], #0x0	// inst 14 & 15

//
	ldr x12, =(ADDR_FIX + 0x14000)	// address

	ldr x11, =0xD2800160	// mov x0, #11
	movk x11, #0x03C0, lsl #32    // ret
	movk x11, #0xD65F, lsl #48
	str x11, [x12], #0x8	// inst 0 & 1
	ldr x11, =0x0
	str x11, [x12], #0x8	// inst 2 & 3
	str x11, [x12], #0x8	// inst 4 & 5
	str x11, [x12], #0x8	// inst 6 & 7
	str x11, [x12], #0x8	// inst 8 & 9
	str x11, [x12], #0x8	// inst 10 & 11
	str x11, [x12], #0x8	// inst 12 & 13
	str x11, [x12], #0x0	// inst 14 & 15

	dsb sy
	ic iallu
	isb

//////////

	ldr x12, =ADDR_FIX	// address
	str x30, [sp, #-0x8]
	blr x12
	ldr x30, [sp, #-0x8]

	ldr x12, =(ADDR_FIX + 0x2000)	// address
	str x30, [sp, #-0x8]
	blr x12
	ldr x30, [sp, #-0x8]

	ldr x12, =(ADDR_FIX + 0x4000)	// address
	str x30, [sp, #-0x8]
	blr x12
	ldr x30, [sp, #-0x8]

	ldr x12, =(ADDR_FIX + 0x6000)	// address
	str x30, [sp, #-0x8]
	blr x12
	ldr x30, [sp, #-0x8]

	ldr x12, =(ADDR_FIX + 0x8000)	// address
	str x30, [sp, #-0x8]
	blr x12
	ldr x30, [sp, #-0x8]

	ldr x12, =(ADDR_FIX + 0xa000)	// address
    str x30, [sp, #-0x8]
	blr x12
	ldr x30, [sp, #-0x8]

	ldr x12, =(ADDR_FIX + 0xc000)	// address
	str x30, [sp, #-0x8]
	blr x12
	ldr x30, [sp, #-0x8]

	ldr x12, =(ADDR_FIX + 0xe000)	// address
	str x30, [sp, #-0x8]
	blr x12
	ldr x30, [sp, #-0x8]

	ldr x12, =(ADDR_FIX + 0x10000)	// address
	str x30, [sp, #-0x8]
	blr x12
	ldr x30, [sp, #-0x8]

	ldr x12, =(ADDR_FIX + 0x12000)	// address
	str x30, [sp, #-0x8]
	blr x12
	ldr x30, [sp, #-0x8]

	ldr x12, =(ADDR_FIX + 0x14000)	// address
	str x30, [sp, #-0x8]
	blr x12
	ldr x30, [sp, #-0x8]

	//
	ldr x12, =(ADDR_FIX + 0x0)	// address
	str x30, [sp, #-0x8]
	blr x12
	ldr x30, [sp, #-0x8]

	ldr x12, =(ADDR_FIX + 0x2000)	// address
	str x30, [sp, #-0x8]
	blr x12
	ldr x30, [sp, #-0x8]

	ldr x12, =(ADDR_FIX + 0x4000)	// address
	str x30, [sp, #-0x8]
	blr x12
	ldr x30, [sp, #-0x8]

	//
	ldr x12, =(ADDR_FIX + 0x2000)	// address
	str x30, [sp, #-0x8]
	blr x12
	ldr x30, [sp, #-0x8]

	ldr x12, =(ADDR_FIX + 0x0)	// address
	str x30, [sp, #-0x8]
	blr x12
	ldr x30, [sp, #-0x8]

	ldr x12, =(ADDR_FIX + 0x4000)	// address
	str x30, [sp, #-0x8]
	blr x12
	ldr x30, [sp, #-0x8]

	//
	mov w9, 1
	cmp x0, #3
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
ENDPROC(asm_test_iu_c1024)
define_asm_testfn asm_test_iu_c1024 0 CPU_EXEC_SYNC
#else
    .cfi_endproc
#endif
