#ifndef __IRQC_SPIKE_H_INCLUDE__
#define __IRQC_SPIKE_H_INCLUDE__

#ifndef ARCH_HAVE_IRQC
#define ARCH_HAVE_IRQC		1
#else
#error "Multiple IRQ controller defined"
#endif

#ifdef CONFIG_SPIKE_PLIC
#ifdef CONFIG_MMU
#define PLIC_BASE		spike_plic_reg_base
#define PLIC_CTX_BASE		(spike_plic_ctx_base[0])
#ifndef __ASSEMBLY__
extern caddr_t spike_plic_reg_base;
extern caddr_t spike_plic_ctx_base[2];
#endif
#else
#define PLIC_BASE		PLIC_REG_BASE
#define PLIC_CTX_BASE		(PLIC_REG_BASE + PLIC_CONTEXT_BASE)
#endif

#define PLIC_HW_PRI_MAX		31
#define plic_hw_m_ctx(cpu)	(cpu)
#define plic_hw_s_ctx(cpu)	((cpu) + 1)

#include <asm/plic.h>

#define plic_hw_ctrl_init()			\
	do {					\
		plic_hw_enable_int(IRQ_EXT);	\
	} while (0)

/* Internal IRQs */
#define plic_hw_enable_int(irq)		riscv_enable_irq(irq)
#define plic_hw_disable_int(irq)	riscv_disable_irq(irq)
#define plic_hw_clear_int(irq)		riscv_clear_irq(irq)
#define plic_hw_trigger_int(irq)	riscv_trigger_irq(irq)

#ifdef CONFIG_MMU
#ifndef __ASSEMBLY__
void plic_hw_mmu_init(void);
#endif
#endif
#else /* !CONFIG_SPIKE_PLIC */
#define irqc_hw_enable_irq(irq)		riscv_enable_irq(irq)
#define irqc_hw_disable_irq(irq)	riscv_disable_irq(irq)
#define irqc_hw_trigger_irq(irq)	riscv_trigger_irq(irq)
#define irqc_hw_clear_irq(irq)		riscv_clear_irq(irq)
#define irqc_hw_configure_irq(irq, priority, trigger)	\
	do { } while (0)

/* clint handles no external IRQs */
#define irqc_hw_handle_irq()		do { } while (0)

/* clint requires no CPU specific initialization */
#define irqc_hw_ctrl_init()		do { } while (0)
#ifdef CONFIG_SMP
#define irqc_hw_smp_init()		do { } while (0)
#endif
#ifdef CONFIG_MMU
#define irqc_hw_mmu_init()		do { } while (0)
#endif
#endif

#endif /* __IRQC_SPIKE_H_INCLUDE__ */
