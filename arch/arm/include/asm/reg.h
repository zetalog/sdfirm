#ifndef __REG_ARM_H_INCLUDE__
#define __REG_ARM_H_INCLUDE__

#include <target/config.h>
#include <asm/io.h>
#include <asm/mach/reg.h>

/* Format of CPSR and SPSRs */
#define	PSR_FV(name, value)	_FV(PSR_##name, value)

#define PSR_M_OFFSET		0
#define PSR_M_MASK		0x1F
#define PSR_M(value)		PSR_FV(M, value)
#define PSR_M_USR		0x10
#define PSR_M_FIQ		0x11
#define PSR_M_IRQ		0x12
#define PSR_M_SVC		0x13
#define PSR_M_MON		0x16
#define PSR_M_ABT		0x17
#define PSR_M_UND		0x1B
#define PSR_M_SYS		0x1F

#define PSR_T			_BV(5)	/* Thumb execution state */
#define PSR_F			_BV(6)	/* Fast interrupt disable */
#define PSR_I			_BV(7)	/* Interrupt disable */
#define PSR_A			_BV(8)	/* Asynchronous abort disable */
#define PSR_E			_BV(9)	/* Endianness execution state */

/* If-then execution state */
#define PSR_IT1_OFFSET		25
#define PSR_IT1_MASK		0x03
#define PSR_IT1(value)		PSR_FV(IT1, value)
#define PSR_IT2_OFFSET		10
#define PSR_IT2_MASK		0x3F
#define PSR_IT2(value)		PSR_FV(IT2, value)
#define PSR_IT2_SPLIT		2
#define PSR_IT_MASK				\
	(PSR_IT1_MASK << PSR_IT1_OFFSET) |	\
	(PSR_IT2_MASK << PSR_IT2_OFFSET)
#define PSR_IT(value)				\
	PSR_IT1((value) & PSR_IT1_MASK) |	\
	PSR_IT2(((value) >> PSR_IT2_SPLIT) & PSR_IT2_OFFSET)

/* Greater than or Equal */
#define PSR_GE_OFFSET		16
#define PSR_GE_MASK		0x0F
#define PSR_GE(value)		PSR_FV(GE, value)

#define PSR_J			_BV(24)	/* Jazzelle */
#define PSR_Q			_BV(27)	/* Cumulative saturation */
#define PSR_V			_BV(28)	/* Overflow condition */
#define PSR_C			_BV(29)	/* Carry condition */
#define PSR_Z			_BV(30)	/* Zero condition */
#define PSR_N			_BV(31)	/* Negative condition */

#endif /* __REG_ARM_H_INCLUDE__ */
