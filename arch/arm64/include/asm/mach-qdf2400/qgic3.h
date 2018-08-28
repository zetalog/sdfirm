#ifndef __QGIC3_QDF2400_H_INCLUDE__
#define __QGIC3_QDF2400_H_INCLUDE__

#define GICD_BASE			ULL(0xFF7EFC0000)
#define GICR_BASE			ULL(0x0FF7F000000)
#define GICR_CPU_SHIFT			18
#define GIC_PRIORITY_SHIFT		0

#include <asm/gicv3.h>

#define qgic3_handle_irq()		\
	do {				\
		;			\
	} while (0)

#endif /* __QGIC3_QDF2400_H_INCLUDE__ */
