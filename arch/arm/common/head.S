#include <target/config.h>
#include <target/linkage.h>
#include <target/init.h>
#include <asm/reg.h>
#include <asm/assembler.h>

#define __vec_ini		__lovec
#ifdef CONFIG_XIP
#define __vec_fin		__lovec
#else
#define __vec_fin		__hivec
#endif

	.thumb

	__LOVEC
	.align 4
	.type __lovec, %object
ENTRY(__lovec)
	.word RAMEND						@ SP
#ifdef CONFIG_XIP
	.word BSYM(stext)					@ reset handler
    	.word BSYM(__bad_interrupt)				@ NMI
    	.word BSYM(__bad_interrupt)				@ hard fault
    	.word BSYM(handle_mpu_fault)				@ MPU
    	.word BSYM(handle_bus_fault)				@ BUS fault
    	.word BSYM(handle_usage_fault)				@ usage fault
    	.word BSYM(__bad_interrupt)				@ reserved
    	.word BSYM(__bad_interrupt)				@ reserved
    	.word BSYM(__bad_interrupt)				@ reserved
    	.word BSYM(__bad_interrupt)				@ reserved
    	.word BSYM(__bad_interrupt)				@ SVCall
    	.word BSYM(__bad_interrupt)				@ debug
    	.word BSYM(__bad_interrupt)				@ reserved
    	.word BSYM(__bad_interrupt)				@ PendSV
    	.word BSYM(handle_sys_tick)				@ SysTick
#else
	.word stext - CONFIG_TEXT_RAM_BASE			@ reset
    	.word BSYM(__bad_interrupt) - CONFIG_TEXT_RAM_BASE	@ NMI
    	.word BSYM(__bad_interrupt) - CONFIG_TEXT_RAM_BASE	@ hard fault
    	.word BSYM(handle_mpu_fault) - CONFIG_TEXT_RAM_BASE	@ MPU
    	.word BSYM(handle_bus_fault) - CONFIG_TEXT_RAM_BASE	@ BUS fault
    	.word BSYM(handle_usage_fault) - CONFIG_TEXT_RAM_BASE	@ usage fault
    	.word BSYM(__bad_interrupt) - CONFIG_TEXT_RAM_BASE	@ reserved
    	.word BSYM(__bad_interrupt) - CONFIG_TEXT_RAM_BASE	@ reserved
    	.word BSYM(__bad_interrupt) - CONFIG_TEXT_RAM_BASE	@ reserved
    	.word BSYM(__bad_interrupt) - CONFIG_TEXT_RAM_BASE	@ reserved
    	.word BSYM(__bad_interrupt) - CONFIG_TEXT_RAM_BASE	@ SVCall
    	.word BSYM(__bad_interrupt) - CONFIG_TEXT_RAM_BASE	@ debug
    	.word BSYM(__bad_interrupt) - CONFIG_TEXT_RAM_BASE	@ reserved
    	.word BSYM(__bad_interrupt) - CONFIG_TEXT_RAM_BASE	@ PendSV
    	.word BSYM(handle_sys_tick) - CONFIG_TEXT_RAM_BASE	@ SysTick
#endif
	.size __lovec, . - __lovec

	__HEAD
ENTRY(stext)
	cpsid	i
	mov	r0, #0x0
	mrs	r0, CONTROL
	bic	r0, r0, #1
	msr	CONTROL, r0

	adr	r3, __startup_info
#ifndef CONFIG_XIP
copy_xip:
	ldmia	r3!, {r4, r5, r6, r7}

copy_sp:
	ldmia	r4!, {r0}
	stmia	r5!, {r0}

copy_trap:
	b	BSYM(copy_trap_loop)
1:	ldmia	r4!, {r0}
	add	r0, r0, #CONFIG_TEXT_RAM_BASE
	stmia	r5!, {r0}
copy_trap_loop:
	cmp	r5, r7
	bcc	BSYM(1b)

copy_text:
	b	BSYM(copy_text_loop)
1:	ldmia	r4!, {r0}
	stmia	r5!, {r0}
copy_text_loop:
	cmp	r5, r6
	bcc	BSYM(1b)
#endif

copy_data:
	ldmia	r3!, {r4, r5, r6, r7}
	b	BSYM(copy_data_loop)
1:	ldmia	r4!, {r0}
	stmia	r5!, {r0}
copy_data_loop:
	cmp	r5, r6
	bcc	BSYM(1b)

zero_bss:
        mov	r0, #0
	b	BSYM(zero_bss_loop)
1:	stmia	r6!, {r0}
zero_bss_loop:
	cmp	r6, r7
	bcc	BSYM(1b)

prepare_stack:
 ARM(	mov	r0, #RAMEND			)
 THUMB(	movw	r0, #(RAMEND & 0xffff)		)
 THUMB(	movt	r0, #(RAMEND >> 16)		)
	mov	sp, r0
	bl	BSYM(system_init)
ENDPROC(stext)

	.align 4
	.type __startup_info, #object
__startup_info:
#ifndef CONFIG_XIP
	.word __text_loc	@ r4
	.word _stext		@ r5
	.word _etext		@ r6
	.word __lovec_end	@ r7
#endif
	.word __data_loc	@ r4
	.word _sdata		@ r5
	.word __bss_start	@ r6
	.word __bss_stop	@ r7
	.size __startup_info, . - __startup_info

#ifdef CONFIG_UART
	.extern uart_hw_sync_write
#define __display_char	uart_hw_sync_write
#else
ENTRY(__display_char)
__display_char:
	bx	lr
ENDPROC(__display_char)
#endif

ENTRY(__display_long)
	push    {r3, r4, r5, lr}
	lsrs    r5, r0, #16
	mov     r4, r0
	lsrs    r0, r5, #8
	bl      BSYM(__display_char)
	uxtb    r0, r5
	bl      BSYM(__display_char)
	ubfx    r0, r4, #8, #8
	bl      BSYM(__display_char)
	uxtb    r0, r4
	bl      BSYM(__display_char)
	pop     {r3, r4, r5, pc}
ENDPROC(__display_long)

ENTRY(__display_stacked_pc)
	@ display fault address
	tst	lr, #0x02
	bne	BSYM(1f)
	mrs	r0, msp
	b	BSYM(2f)
1:
	mrs	r0, psp 
2:
	add	r0, r0, #0x18
	bl	BSYM(__display_long)
ENDPROC(__display_stacked_pc)

ENTRY(handle_mpu_fault)
	b	.
ENDPROC(handle_mpu_fault)

ENTRY(handle_bus_fault)
	b	.
ENDPROC(handle_bus_fault)

ENTRY(handle_usage_fault)
1:
	@ bl	BSYM(__display_stacked_pc)
	b	BSYM(1b)
ENDPROC(handle_usage_fault)

ENTRY(handle_sys_tick)
	bx	lr
ENDPROC(handle_sys_tick)

ENTRY(__bad_interrupt)
	b	.
ENDPROC(__bad_interrupt)
