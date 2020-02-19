// ------------------------------------------------------------
// ARMv8-A separate core program
//
// Description: Test case
// ------------------------------------------------------------
//
#ifdef TESTOS
#include <target/raven.h>
ENTRY(asm_test_iu_c0500)
#else
    .global asm_test_iu_c0500
    .type asm_test_iu_c0500, "function"
    .cfi_startproc
asm_test_iu_c0500:
#endif
     // add your core code

#ifdef TESTOS
#define ADDR_BASE test_array
#else
#define ADDR_BASE 0xff70030000
#endif

        ldr x12, =(ADDR_BASE + 0xc000)	  	 // address
	ldr x11, =0x9100218D   		 // add x13, x12, #0x8
	movk x11, 0x01A0, lsl #32    // Br x13
	movk x11, 0xD61F, lsl #48    // Br x13
	str x11, [x12], #0x8		 //

	ldr x11, =0xD65F03C0	// ret
	movk x11, 0x0000, lsl #32    // initial mem
	movk x11, 0x0000, lsl #48    //
	str x11, [x12], #0x8		 //

	mov x11, #0
	str x11, [x12], #0x8		 //

	mov x11, #0
	str x11, [x12], #0x8		 //

	mov x11, #0
	str x11, [x12], #0x8		 //

	mov x11, #0
	str x11, [x12], #0x8		 //

	mov x11, #0
	str x11, [x12], #0x8		 //

	mov x11, #0
	str x11, [x12], #0x0    	 //

////////////////////////////////////////////////////////////////

        ldr x12, =(ADDR_BASE + 0x8000)	  	 // address
	ldr x11, =0x9100218D   		 // add x13, x12, #0x8
	movk x11, 0x01A0, lsl #32    // Br x13
	movk x11, 0xD61F, lsl #48    // Br x13
	str x11, [x12], #0x8		 //

	ldr x11, =0xD65F03C0	// ret
	movk x11, 0x0000, lsl #32    // initial mem
	movk x11, 0x0000, lsl #48    //
	str x11, [x12], #0x8		 //

	mov x11, #0
	str x11, [x12], #0x8		 //

	mov x11, #0
	str x11, [x12], #0x8		 //

	mov x11, #0
	str x11, [x12], #0x8		 //

	mov x11, #0
	str x11, [x12], #0x8		 //

	mov x11, #0
	str x11, [x12], #0x8		 //

	mov x11, #0
	str x11, [x12], #0x0    	 //

////////////////////////////////////////////////////////////////

        ldr x12, =(ADDR_BASE + 0x4000)	  	 // address
	ldr x11, =0x9100218D   		 // add x13, x12, #0x8
	movk x11, 0x01A0, lsl #32    // Br x13
	movk x11, 0xD61F, lsl #48    // Br x13
	str x11, [x12], #0x8		 //

	ldr x11, =0xD65F03C0	// ret
	movk x11, 0x0000, lsl #32    // initial mem
	movk x11, 0x0000, lsl #48    //
	str x11, [x12], #0x8		 //

	mov x11, #0
	str x11, [x12], #0x8		 //

	mov x11, #0
	str x11, [x12], #0x8		 //

	mov x11, #0
	str x11, [x12], #0x8		 //

	mov x11, #0
	str x11, [x12], #0x8		 //

	mov x11, #0
	str x11, [x12], #0x8		 //

	mov x11, #0
	str x11, [x12], #0x0    	 //
////////////////////////////////////////////////////////////////

        ldr x12, =(ADDR_BASE + 0x0000)	  	 // address
	ldr x11, =0x9100218D   		 // add x13, x12, #0x8
	movk x11, 0x01A0, lsl #32    // Br x13
	movk x11, 0xD61F, lsl #48    // Br x13
	str x11, [x12], #0x8		 //

	ldr x11, =0xD65F03C0	// ret
	movk x11, 0x0000, lsl #32    // initial mem
	movk x11, 0x0000, lsl #48    //
	str x11, [x12], #0x8		 //

	mov x11, #0
	str x11, [x12], #0x8		 //

	mov x11, #0
	str x11, [x12], #0x8		 //

	mov x11, #0
	str x11, [x12], #0x8		 //

	mov x11, #0
	str x11, [x12], #0x8		 //

	mov x11, #0
	str x11, [x12], #0x8		 //

	mov x11, #0
	str x11, [x12], #0x0    	 //


	dsb sy
	ic iallu
	isb


mov x10, #0
top_loop:
	add x10, x10, #1
        ldr x12, =(ADDR_BASE + 0xc000)	  	 // address
	str x30, [sp, #-0x8]
	blr x12
        ldr x30, [sp, #-0x8]
        ldr x12, =(ADDR_BASE + 0x0000)	  	 // address
	str x30, [sp, #-0x8]
	blr x12
        ldr x30, [sp, #-0x8]
        ldr x12, =(ADDR_BASE + 0x4000)	  	 // address
	str x30, [sp, #-0x8]
	blr x12
        ldr x30, [sp, #-0x8]
        ldr x12, =(ADDR_BASE + 0x8000)	  	 // address
	str x30, [sp, #-0x8]
	blr x12
        ldr x30, [sp, #-0x8]

//////////
	cmp x10, #10
	b.ne top_loop
	//
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
ENDPROC(asm_test_iu_c0500)
define_asm_testfn asm_test_iu_c0500 0 CPU_EXEC_SYNC
#else
    .cfi_endproc
#endif
