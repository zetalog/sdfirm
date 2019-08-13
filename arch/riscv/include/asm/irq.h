#ifndef __IRQ_RISCV_H_INCLUDE__
#define __IRQ_RISCV_H_INCLUDE__

#include <target/config.h>
#include <target/generic.h>
#include <asm/reg.h>

#define IRQ_PTIMER1	30 /* EL1 physical timer */

typedef uint32_t irq_flags_t;

#define irq_hw_flags_save(x)	((x) = csr_read_clear(CSR_SSTATUS, SR_SIE))
#define irq_hw_flags_restore(x)	csr_set(CSR_SSTATUS, (x) & SR_SIE)
#define irq_hw_flags_enable()	csr_set(CSR_SSTATUS, SR_SIE)
#define irq_hw_flags_disable()	csr_clear(CSR_SSTATUS, SR_SIE)
#define irq_hw_ctrl_init()

#include <asm/mach/irq.h>

struct pt_regs {
	unsigned long regs[31];
};

#endif /* __IRQ_RISCV_H_INCLUDE__ */
