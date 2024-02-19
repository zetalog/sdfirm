#ifndef __KCS_H_INCLUDE__
#define __KCS_H_INCLUDE__

#include <target/generic.h>

#define KCS_BASE				0x0ca2

/* KCS Interface Registers
 * IPMI Spec 2.0 Figure 9-5
 */
#define KCS_DATA_IN				KCS_REG(0x0)
#define KCS_DATA_OUT				KCS_REG(0x0)
#define KCS_COMMAND				KCS_REG(0x1)
#define KCS_STATUS				KCS_REG(0x1)

/* Status Register
 * IPMI Spec 2.0 Table 9-1
 */
#define KCS_S1					_BV(7)
#define KCS_S0					_BV(6)
#define KCS_IDLE_STATE				0x0
#define KCS_READ_STATE				KCS_S0
#define KCS_WRITE_STATE				KCS_S1
#define KCS_ERROR_STATE				(KCS_S0 | KCS_S1)
#define KCS_STATE_MASK				(KCS_S0 | KCS_S1)
#define KCS_OEM2				_BV(5)
#define KCS_OEM1				_BV(4)
#define KCS_CD					_BV(3)
#define KCS_C					0
#define KCS_D					1
#define KCS_SMS_ATN				_BV(2)
#define KCS_IBF					_BV(1)
#define KCS_OBF					_BV(0)

/* Control Code
 * IPMI Spec 2.0 Table 9-3
 */
#define KCS_GET_STATUS				0x60
#define KCS_WRITE_START				0x61
#define KCS_WRITE_END				0x62
#define KCS_READ				0x68

#define KCS_EVENT_IBF				0x1
#define KCS_EVENT_OBF				0x2

#ifdef CONFIG_ARCH_HAS_KCS
#include <asm/mach/kcs.h>
#endif

#ifdef CONFIG_KCS
void kcs_init(void);
void kcs_raise_event(uint8_t event);
#else
#define kcs_init()				do { } while (0)
#endif

#endif /* __KCS_H_INCLUDE__ */