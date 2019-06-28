#ifndef __IRQC_DRIVER_H_INCLUDE__
#define __IRQC_DRIVER_H_INCLUDE__

#ifdef CONFIG_ARCH_HAS_IRQC
#include <asm/irqc.h>
#endif

#ifdef ARCH_HAVE_IRQC
#define irqc_enable_irq(irq)		irqc_hw_enable_irq(irq)
#define irqc_disable_irq(irq)		irqc_hw_disable_irq(irq)
#define irqc_clear_irq(irq)		irqc_hw_clear_irq(irq)
#define irqc_trigger_irq(irq)		irqc_hw_trigger_irq(irq)
#define irqc_configure_irq(irq, prio, trigger)	\
	irqc_hw_configure_irq(irq, prio, trigger)
#else
#define irqc_hw_ctrl_init()
#define irqc_enable_irq(irq)
#define irqc_disable_irq(irq)
#define irqc_clear_irq(irq)
#define irqc_trigger_irq(irq)
#define irqc_configure_irq(irq, prio, trigger)
#endif

#endif /* __IRQC_DRIVER_H_INCLUDE__ */
