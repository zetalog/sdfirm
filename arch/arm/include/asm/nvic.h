#ifndef __NVIC_ARM_H_INCLUDE__
#define __NVIC_ARM_H_INCLUDE__

#include <target/config.h>
#include <target/generic.h>
#include <asm/reg.h>

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

#define NVIC_PRIO_MAX		7
#define NVIC_PRIO_TRAP_MIN	TRAP_MPU /* minimum priority configurable trap */

#define NVIC_TRIG_MASK			0x1F
#define NVIC_TRIG_A(__a, __vi)		((__a)+(((__vi) & ~NVIC_TRIG_MASK) >> 3))
#define NVIC_TRIG_V(__vi)		(1<<((__vi) & NVIC_TRIG_MASK))
#define NVIC_PRIO_MASK			0x03
#define NVIC_PRIO_A(__a, __vi)		((__a)+((__vi) & ~NVIC_PRIO_MASK))
#define NVIC_PRIO_V(__v, __vi)		((__v << 5) << (((__vi) & NVIC_PRIO_MASK) << 3))

#define nvic_irq_enable_irq(irq)	\
	__raw_setl(NVIC_TRIG_V(irq), NVIC_TRIG_A(EN0, irq))
#define nvic_irq_disable_irq(irq)	\
	__raw_setl(NVIC_TRIG_V(irq), NVIC_TRIG_A(DIS0, irq))
#define nvic_irq_trigger_irq(irq)	\
	__raw_writel(irq, SWTRIG)

#define nvic_irq_set_priority(irq, prio)		\
	do {						\
		__raw_clearl(NVIC_PRIO_V(7, irq),	\
			     NVIC_PRIO_A(PRI0, irq));	\
		__raw_setl(NVIC_PRIO_V(prio, irq),	\
			   NVIC_PRIO_A(PRI0, irq));	\
	} while (0)

#define nvic_irq_set_pending(irq)	\
	__raw_setl(NVIC_TRIG_V(irq), NVIC_TRIG_A(PEND0, irq))
#define nvic_irq_clear_pending(irq)	\
	__raw_setl(NVIC_TRIG_V(irq), NVIC_TRIG_A(UNPEND0, irq))
#define nvic_irq_is_pending(irq)	\
	__raw_testl(NVIC_TRIG_V(irq), NVIC_TRIG_A(PEND0, irq))
#define nvic_irq_is_active(irq)		\
	__raw_testl(NVIC_TRIG_V(irq), NVIC_TRIG_A(ACTIVE0, irq))

#define nvic_trap_set_priority(trap, prio)			\
	do {							\
		__raw_clearl(NVIC_PRIO_V(7, trap),		\
			     NVIC_PRIO_A(SYSPRI1-4, trap));	\
		__raw_setl(NVIC_PRIO_V(prio, trap),		\
			   NVIC_PRIO_A(SYSPRI1-4, trap));	\
	} while (0)

#endif /* __NVIC_ARM_H_INCLUDE__ */
