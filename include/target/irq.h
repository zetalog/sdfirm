#ifndef __IRQ_H_INCLUDE__
#define __IRQ_H_INCLUDE__

#include <target/bh.h>
#include <asm/irq.h>

#define IRQ_LEVEL_TRIGGERED	0
#define IRQ_EDGE_TRIGGERED	1

#ifndef __ASSEMBLY__
#if NR_IRQS <= 256
typedef uint8_t irq_t;
#elif NR_IRQS <= 65536
typedef uint16_t irq_t;
#endif
typedef void (*irq_handler)(void);

#ifdef CONFIG_ARCH_HAS_VIC
#include <driver/vic.h>
#else
#include <driver/irqc.h>
#endif

#define irq_local_enable()		irq_hw_flags_enable()
#define irq_local_disable()		irq_hw_flags_disable()
#define irq_local_save(__flags__)	irq_hw_flags_save(__flags__)
#define irq_local_restore(__flags__)	irq_hw_flags_restore(__flags__)

void irq_init(void);
#ifdef CONFIG_SMP
void irq_smp_init(void);
#else
#define irq_smp_init()			do { } while (0)
#endif

#ifndef CONFIG_CC_ISR_VECTOR
void irq_register_vector(irq_t nr, irq_handler isr);
void irq_vectors_init(void);
#else
#define irq_register_vector(nr, isr)
#define irq_vectors_init()
#endif

boolean do_IRQ(irq_t nr);

/* irq pollers */
void irq_register_poller(bh_t bh);
void irq_poll_bh(bh_t bh);
extern boolean irq_is_polling;

#ifndef irq_hw_flags_enable
void irq_hw_flags_enable(void);
#endif
#ifndef irq_hw_flags_disable
void irq_hw_flags_disable(void);
#endif
void irq_hw_handle_irq(void);
#ifndef irq_hw_ctrl_init
void irq_hw_ctrl_init(void);
#endif

typedef void (*trap_handler)(const char *msg);
void trap_handler_vector(trap_handler handler);
#endif /* __ASSEMBLY__ */

#endif /* __IRQ_H_INCLUDE__ */
