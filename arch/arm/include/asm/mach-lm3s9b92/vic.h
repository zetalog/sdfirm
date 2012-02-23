#ifndef __VIC_LM3S9B92_H_INCLUDE__
#define __VIC_LM3S9B92_H_INCLUDE__

#include <target/config.h>
#include <target/generic.h>
#include <asm/reg.h>
#include <asm/mach/mem.h>
#include <asm/mach/irq.h>
#include <asm/mach/scb.h>

#ifndef ARCH_HAVE_VIC
#define ARCH_HAVE_VIC			1
#else
#error "Multiple VIC controller defined"
#endif

/* Nested Vectored Intrrupt Controller (NVIC) registers */
#define EN0			CORTEXM3(0x100)
#define EN1			CORTEXM3(0x104)
#define DIS0			CORTEXM3(0x180)
#define DIS1			CORTEXM3(0x184)
#define PEND0			CORTEXM3(0x200)
#define PEND1			CORTEXM3(0x204)
#define UNPEND0			CORTEXM3(0x280)
#define UNPEND1			CORTEXM3(0x284)
#define ACTIVE0			CORTEXM3(0x300)
#define ACTIVE1			CORTEXM3(0x304)
#define PRI0			CORTEXM3(0x400)
#define PRI1			CORTEXM3(0x404)
#define PRI2			CORTEXM3(0x408)
#define PRI3			CORTEXM3(0x40C)
#define PRI4			CORTEXM3(0x410)
#define PRI5			CORTEXM3(0x414)
#define PRI6			CORTEXM3(0x418)
#define PRI7			CORTEXM3(0x41C)
#define PRI8			CORTEXM3(0x420)
#define PRI9			CORTEXM3(0x424)
#define PRI10			CORTEXM3(0x428)
#define PRI11			CORTEXM3(0x42C)
#define PRI12			CORTEXM3(0x430)
#define PRI13			CORTEXM3(0x434)
#define SWTRIG			CORTEXM3(0xF00)

#define __VIC_HW_PRIO_MAX	7
#define __VIC_HW_PRIO_TRAP_MIN	TRAP_MPU /* minimum priority configurable trap */

typedef void (*irq_handler)(void);

void vic_hw_register_irq(uint8_t nr, irq_handler h);
void vic_hw_register_trap(uint8_t nr, irq_handler h);
void vic_hw_vectors_init(void);

#define VIC_TRIG_MASK			0x1F
#define VIC_TRIG_A(__a, __vi)		((__a)+(((__vi) & ~VIC_TRIG_MASK) >> 3))
#define VIC_TRIG_V(__vi)		(1<<((__vi) & VIC_TRIG_MASK))
#define VIC_PRIO_MASK			0x03
#define VIC_PRIO_A(__a, __vi)		((__a)+((__vi) & ~VIC_PRIO_MASK))
#define VIC_PRIO_V(__v, __vi)		((__v << 5) << (((__vi) & VIC_PRIO_MASK) << 3))

#define vic_hw_irq_enable(irq)		__raw_setl(VIC_TRIG_V(irq), VIC_TRIG_A(EN0, irq))
#define vic_hw_irq_disable(irq)		__raw_setl(VIC_TRIG_V(irq), VIC_TRIG_A(DIS0, irq))
#define vic_hw_irq_trigger(irq)		__raw_writel(irq, SWTRIG)
void vic_hw_irq_priority(uint8_t irq, uint8_t prio);
void vic_hw_trap_priority(uint8_t trap, uint8_t prio);

#define vic_hw_irq_set_pending(irq)	__raw_setl(VIC_TRIG_V(irq), VIC_TRIG_A(PEND0, irq))
#define vic_hw_irq_clear_pending(irq)	__raw_setl(VIC_TRIG_V(irq), VIC_TRIG_A(UNPEND0, irq))
#define vic_hw_irq_test_pending(irq)	__raw_testl(VIC_TRIG_V(irq), VIC_TRIG_A(PEND0, irq))
#define vic_hw_irq_active(irq)		__raw_testl(VIC_TRIG_V(irq), VIC_TRIG_A(ACTIVE0, irq))

#endif /* __VIC_LM3S9B92_H_INCLUDE__ */
