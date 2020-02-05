#ifndef __IRQC_SPIKE_H_INCLUDE__
#define __IRQC_SPIKE_H_INCLUDE__

#ifndef ARCH_HAVE_IRQC_
#define ARCH_HAVE_IRQC		1
#else
#error "Multiple IRQ controller defined"
#endif

#define irqc_hw_enable_irq(irq)		riscv_enable_irq(irq)
#define irqc_hw_disable_irq(irq)	riscv_disable_irq(irq)
#define irqc_hw_trigger_irq(irq)	riscv_trigger_irq(irq)
#define irqc_hw_clear_irq(irq)		riscv_clear_irq(irq)
#define irqc_hw_configure_irq(irq, priority, trigger)	\
	do { } while (0)
#define irqc_hw_ctrl_init()		do { } while (0)

#endif /* __IRQC_SPIKE_H_INCLUDE__ */
