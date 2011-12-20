#ifndef __PWM_AT90USB1287_H_INCLUDE__
#define __PWM_AT90USB1287_H_INCLUDE__

#include <target/config.h>
#include <asm/reg.h>

/* Power Reduction Register */
#define PRR0		_SFR_MEM8(0x64)
#define PRADC		0
#define PRSPI		2
#define PRTIM1		3
#define PRTIM0		5
#define PRTIM2		6
#define PRTWI		7

#define PRR1		_SFR_MEM8(0x65)
#define PRUSART1	0
#define PRTIM3		3
#define PRUSB		7

/* Sleep Mode Control Register */
#define SMCR		_SFR_IO8(0x33)
#define SE		0
#define SMCR_MODE_IDLE		0
#define SMCR_MODE_AC_NOISE	1
#define SMCR_MODE_POWER_DOWN	2
#define SMCR_MODE_POWER_SAVE	3
#define SMCR_MODE_STANDBY	6
#define SMCR_MODE_STANDBY_EX	7

#define AT90USB1287_POWER_ENABLE(r, v)	(r &= ~_BV(v))
#define AT90USB1287_POWER_DISABLE(r, v)	(r |= _BV(v))

#endif /* __PWM_AT90USB1287_H_INCLUDE__ */
