#include <target/config.h>
#include <target/linkage.h>
#include <target/init.h>
#include <asm/reg.h>
#include <asm/assembler.h>

#define	BOOTING		1	/* Defining this to 0 after booting */
#define BOOTING_UART	0	/* Defining this to 0 after preparing UART0 */

	.arm					@ ARM

	__HEAD

ENTRY(stext)
	b		handle_reset		@ reset
	b		handle_und		@ undefined instruction
#ifdef CONFIG_ARM_MONITOR
	b		handle_smc		@ SMC
#else
	b		handle_svc		@ SVC
#endif
	ldr pc,		handle_pabt		@ prefetch abort
	ldr pc,		handle_dabt		@ data abort
	ldr pc,		__bad_interrupt
	ldr pc,		handle_irq		@ IRQ
	ldr pc,		handle_fiq		@ FIQ
END(stext)

#ifdef CONFIG_DEBUG_PRINT
	.extern			debug_init
	.extern			dbg_putchar
	.extern			dbg_dump_str
#else
#define	debug_init		__bad_interrupt
#define	dbg_putchar		__bad_interrupt
#define	dbg_dump_str		__bad_interrupt
#endif
#define __display_char		dbg_putchar
#define __display_string	dbg_dump_str

welcome_msg:
	.asciz	"Welcome to sdfirm - by ZETALOG.\n"

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

ENTRY(handle_reset)
prepare_stack:
 ARM(	mov	r0, #RAMEND			)
 ARM(	bic	r0, r0, #7			)	/* 8-byte alignment */
	mov	sp, r0

#if BOOTING
booting:
	bl	BSYM(debug_init)
	mov	r0, #welcome_msg
	bl	BSYM(__display_string)

#if BOOTING_UART
output_uart0_wave:
	mov	r0, #5
	bl	BSYM(__display_char)
	b	output_uart0_wave
#endif
#endif
	bl	BSYM(system_init)
ENDPROC(handle_reset)

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
