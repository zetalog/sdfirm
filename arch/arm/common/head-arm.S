#include <target/config.h>
#include <target/linkage.h>
#include <target/init.h>
#include <asm/reg.h>
#include <asm/assembler.h>

#define	BOOTING		1	/* Defining this to 0 after booting */
#define BOOTING_UART	0	/* Defining this to 0 after preparing UART0 */

#ifdef CONFIG_DEBUG_PRINT
	.extern			debug_init
	.extern			dbg_putchar
	.extern			dbg_dump_str
#else
#define	debug_init		__bad_interrupt
#define	dbg_putchar		__bad_interrupt
#define	dbg_dump_str		__bad_interrupt
#endif
#define __display_init		debug_init
#define __display_char		dbg_putchar
#define __display_string	dbg_dump_str

	.arm					@ ARM

	__HEAD
ENTRY(stext)
	b		BSYM(handle_reset)	@ reset, PIT
	ldr		pc, =BSYM(handle_und)	@ undefined instruction
#ifdef CONFIG_ARM_MONITOR
	ldr		pc, =BSYM(handle_smc)	@ SMC
#else
	ldr		pc, =BSYM(handle_svc)	@ SVC
#endif
	ldr		pc, =BSYM(handle_pabt)	@ prefetch abort
	ldr		pc, =BSYM(handle_dabt)	@ data abort
	ldr		pc, =BSYM(__bad_interrupt)
	ldr		pc, =BSYM(handle_irq)	@ IRQ
	ldr		pc, =BSYM(handle_fiq)	@ FIQ
END(stext)

ENTRY(handle_reset)

prepare_stack:
 ARM(	mov	r0, #RAMEND			)
 ARM(	bic	r0, r0, #7			)	/* 8-byte alignment */
	mov	sp, r0

#if BOOTING_UART
output_debug_uart_wave:
	mov	r0, #5
	bl	BSYM(__display_char)
	b	BSYM(output_debug_uart_wave)
#endif

#if BOOTING
	bl	BSYM(__display_init)
	mov	r0, #welcome_msg_rom
	bl	BSYM(__display_string)
#endif

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

#if BOOTING
	bl	BSYM(__display_init)
	ldr	r0, =welcome_msg_ram
	bl	BSYM(__display_string)
#endif

	ldr	pc, =BSYM(system_init)
ENDPROC(handle_reset)

ENTRY(__startup_info)
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
ENDOBJ(__startup_info)

welcome_msg_rom:
	.asciz	"Welcome to sdfirm ROM - by ZETALOG.\n"

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

#ifdef CONFIG_ARM_MONITOR
ENTRY(handle_smc)
	b	.
ENDPROC(handle_smc)

ENTRY(handle_irq)
	b	.
ENDPROC(handle_irq)

ENTRY(handle_fiq)
	b	.
ENDPROC(handle_fiq)
#else
ENTRY(handle_svc)
	b	.
ENDPROC(handle_svc)

ENTRY(handle_irq)
	b	.
ENDPROC(handle_irq)

ENTRY(handle_fiq)
	b	.
ENDPROC(handle_fiq)
#endif

ENTRY(handle_dabt)
	b	.
ENDPROC(handle_dabt)

ENTRY(handle_pabt)
	b	.
ENDPROC(handle_pabt)

ENTRY(handle_und)
	b	.
ENDPROC(handle_und)

ENTRY(__bad_interrupt)
	b	.
ENDPROC(__bad_interrupt)

	.pushsection	.data
ENTRY(welcome_msg_ram)
	.asciz	"Welcome to sdfirm RAM - by ZETALOG.\n"
	.popsection
