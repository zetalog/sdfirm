#ifndef __MSI_H_INCLUDE__
#define __MSI_H_INCLUDE__

#include <target/irq.h>
#include <target/smp.h>

#ifdef CONFIG_MSI
irq_t irq_register_matrix(cpu_mask_t *msk);
void irq_reserve_matrix(cpu_mask_t *msk, irq_t start, irq_t end);
#else
#define irq_register_matrix(msk)
#define irq_reserve_matrix(msk, start, end)
#endif

#endif /* __MSI_H_INCLUDE__ */
