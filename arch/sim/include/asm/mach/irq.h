#ifndef __IRQ_MACH_H_INCLUDE__
#define __IRQ_MACH_H_INCLUDE__

#define IRQ_GPT	0x00
#define IRQ_USB	0x01
#define NR_IRQS	2

typedef void (*irq_vect_cb)(void);

extern irq_vect_cb irq_vects[NR_IRQS];
extern DECLARE_BITMAP(irq_opens, NR_IRQS);

void irq_hw_register_irq(uint8_t irq, irq_vect_cb vect);
void irq_hw_irq_enable(uint8_t irq);
void irq_hw_irq_disable(uint8_t irq);
boolean irq_hw_irq_enabled(uint8_t irq);

#endif /* __IRQ_MACH_H_INCLUDE__ */
