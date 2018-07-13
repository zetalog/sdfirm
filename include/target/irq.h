#ifndef __IRQ_H_INCLUDE__
#define __IRQ_H_INCLUDE__

#include <target/config.h>
#include <target/generic.h>
#include <target/bh.h>
#include <asm/irq.h>

#if NR_IRQS <= 256
typedef uint8_t irq_t;
#elif NR_IRQS <= 65536
typedef uint16_t irq_t;
#endif

#include <driver/vic.h>

#define irq_local_enable()		irq_hw_flags_enable()
#define irq_local_disable()		irq_hw_flags_disable()
#define irq_local_save(__flags__)	irq_hw_flags_save(__flags__)
#define irq_local_restore(__flags__)	irq_hw_flags_restore(__flags__)

void irq_init(void);

#ifndef CONFIG_CC_ISR_VECTOR
void irq_register_vector(irq_t nr, irq_handler isr);
void irq_vectors_init(void);
#else
#define irq_register_vector(nr, isr)
#define irq_vectors_init()
#endif

/* irq pollers */
void irq_register_poller(bh_t bh);
void irq_poll_bh(bh_t bh);

#ifndef irq_hw_flags_enable
void irq_hw_flags_enable(void);
#endif
#ifndef irq_hw_flags_disable
void irq_hw_flags_disable(void);
#endif
#ifndef irq_hw_ctrl_init
void irq_hw_ctrl_init(void);
#endif

#endif /* __IRQ_H_INCLUDE__ */
