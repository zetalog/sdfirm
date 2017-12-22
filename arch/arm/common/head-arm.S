#include <target/config.h>
#include <target/linkage.h>
#include <target/init.h>
#include <asm/reg.h>
#include <asm/assembler.h>

	.arm					@ ARM

	__HEAD

ENTRY(stext)
	ldr pc,		.			@ reset
	ldr pc,		handle_und		@ undefined instruction
#ifdef CONFIG_ARM_MONITOR
	ldr pc,		handle_smc		@ SMC
#else
	ldr pc,		handle_svc		@ SVC
#endif
	ldr pc,		handle_pabt		@ prefetch abort
	ldr pc,		handle_dabt		@ data abort
	ldr pc,		__bad_interrupt
	ldr pc,		handle_irq		@ IRQ
	ldr pc,		handle_fiq		@ FIQ
END(stext)

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
