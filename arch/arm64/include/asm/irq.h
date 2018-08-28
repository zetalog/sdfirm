#ifndef __IRQ_ARM64_H_INCLUDE__
#define __IRQ_ARM64_H_INCLUDE__

#include <target/config.h>
#include <target/generic.h>
#include <asm/reg.h>

#define IRQ_SGI_BASE	0
#define IRQ_PPI_BASE	16
#define IRQ_SPI_BASE	32

/* Special INTIDs */
#define IRQ_ACK_SEL1	1020 /* Acked IRQ should be handled at Secure EL1 */
#define IRQ_ACK_NSEL1	1021 /* Acked IRQ should be handled at Non-secure EL1 */
#define IRQ_ACK_NONE	1023 /* No pending IRQ with sufficient priority */

/* PPIs defined by GIC architecture as ARM recommendation */
#define IRQ_DCC		22 /* Debug communications channel */
#define IRQ_PMU		23 /* Performance monitor counter overflow */
#define IRQ_CTI		24 /* Cross trigger interface */
#define IRQ_VCPUM	25 /* Virtual CPU interface maintenance */
#define IRQ_PTIMER2	26 /* EL2 physical timer */
#define IRQ_VTIMER	27 /* Virtual timer */
#define IRQ_VTIMER2	28 /* EL2 virtual timer */
#define IRQ_PTIMER3	29 /* EL3 physical timer */
#define IRQ_PTIMER1	30 /* EL1 physical timer */

typedef uint64_t irq_flags_t;

#define irq_hw_flags_save(x)					\
	__asm__ __volatile__(					\
	"mrs	%0, daif		// local_irq_save\n"	\
	"msr	daifset, #2"					\
	: "=r" (x) : : "memory")
#define irq_hw_flags_restore(x)					\
	__asm__ __volatile__(					\
	"msr	daif, %0		// local_irq_restore\n"	\
	: : "r" (x) : "memory")
#define irq_hw_flags_enable()					\
	__asm__ __volatile__(					\
	"msr	daifclr, #2		// local_irq_enable\n"	\
	: : : "memory")
#define irq_hw_flags_disable()					\
	__asm__ __volatile__(					\
	"msr	daifset, #2		// local_irq_disable\n"	\
	: : : "memory")
#define irq_hw_ctrl_init()

#include <asm/mach/irq.h>

#endif /* __IRQ_ARM64_H_INCLUDE__ */
