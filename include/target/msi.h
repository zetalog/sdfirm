#ifndef __MSI_H_INCLUDE__
#define __MSI_H_INCLUDE__

#include <target/irq.h>
#include <target/smp.h>

#ifdef CONFIG_MSI
irq_t irq_register_mapping(irq_t irq);
void irq_reserve_mapping(irq_t start, irq_t end);
irq_t irq_locate_mapping(cpu_t cpu, irq_t msi);
void msi_init(void);
#else
#define irq_register_mapping(irq)		INVALID_IRQ
#define irq_reserve_mapping(start, end)		do { } while (0)
#define irq_locate_mapping(cpu, msi)		INVALID_IRQ
#define msi_init()				do { } while (0)
#endif

#endif /* __MSI_H_INCLUDE__ */
