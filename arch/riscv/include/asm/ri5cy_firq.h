#ifndef __RI5CY_FIRQ_H_INCLUDE__
#define __RI5CY_FIRQ_H_INCLUDE__

#define CSR_MIEX		0x7D0
#define CSR_MTVECX		0x7D1
#define CSR_MIPX		0x7D2

/* MIE */
#define SR_MFIE_SHIFT	16
#define SR_MFIE		_AC(0x7FFF0000, UL) /* Machine Fast Interrupt Enable */
/* MIP */
#define SR_NMIP		_AC(0x80000000, UL) /* Non Maskable Interrupt Pending */
#define SR_MFIP_SHIFT	16
#define SR_MFIP		_AC(0x7FFF0000, UL) /* Machine Fast Interrupt Pending */

#define FIRQ2IRQ(irq)			((irq) + 16)
#define IRQ2FIRQ(irq)			((irq) - 16)
#define __riscv_enable_firq(irq)	riscv_enable_irq(FIRQ2IRQ(irq))
#define __riscv_disable_firq(irq)	riscv_disable_irq(FIRQ2IRQ(irq))
#define __riscv_trigger_firq(irq)	riscv_trigger_irq(FIRQ2IRQ(irq))
#define __riscv_clear_firq(irq)		riscv_clear_irq(FIRQ2IRQ(irq))
#define __riscv_firq_raised(irq)	riscv_irq_raised(FIRQ2IRQ(irq))

#ifdef CONFIG_RI5CY_FIRQ_EXT
#define EFIRQ2IRQ(irq)			((irq) - 16)
#define EFIRQ2IE(irq)			(_AC(0x1, UL) << EFIRQ2IRQ(irq))

#define riscv_enable_efirq(irq)		csr_set(CSR_MIEX, EFIRQ2IE(irq))
#define riscv_disable_efirq(irq)	csr_clear(CSR_MIEX, EFIRQ2IE(irq))
#define riscv_trigger_efirq(irq)	csr_set(CSR_MIPX, EFIRQ2IE(irq))
#define riscv_clear_efirq(irq)		csr_clear(CSR_EIPX, EFIRQ2IE(irq))
#define riscv_efirq_raised(irq)		(csr_read(CSR_EIPX) & EFIRQ2IE(irq))

#define riscv_enable_firq(irq)		\
	((irq) < 16 ? __riscv_enable_firq(irq) : riscv_enable_efirq(irq))
#define riscv_disable_firq(irq)		\
	((irq) < 16 ? __riscv_disable_firq(irq) : riscv_disable_efirq(irq))
#define riscv_trigger_firq(irq)		\
	((irq) < 16 ? __riscv_trigger_firq(irq) : riscv_trigger_efirq(irq))
#define riscv_clear_firq(irq)		\
	((irq) < 16 ? __riscv_clear_firq(irq) : riscv_clear_efirq(irq))
#define riscv_firq_raised(irq)		\
	((irq) < 16 ? __riscv_firq_raised(irq) : riscv_efirq_raised(irq))
#else
#define riscv_enable_firq(irq)		__riscv_enable_firq(irq)
#define riscv_disable_firq(irq)		__riscv_disable_firq(irq)
#define riscv_trigger_firq(irq)		__riscv_trigger_firq(irq)
#define riscv_clear_firq(irq)		__riscv_clear_firq(irq)
#define riscv_firq_raised(irq)		__riscv_firq_raised(irq)
#endif

#endif /* __RI5CY_FIRQ_H_INCLUDE__ */
