#ifndef __TIMER_ARM64_H_INCLUDE__
#define __TIMER_ARM64_H_INCLUDE__

#include <target/types.h>
#include <target/gpt.h>
#include <asm/reg.h>
#include <asm/sysreg.h>

/* =================================================================
 * D.7.5 Generic Timer registers
 * ================================================================= */
/* D.7.5.2 CNTHCTL_EL2, Counter-timer Hypervisor Control register */
#define CNTHCTL_EL1PCTEN	_BV(0)
#define CNTHCTL_EL1PCEN		_BV(1)

#define CNTHCTL_EL2_RES0			\
	(_BV(31)|_BV(30)|_BV(29)|_BV(28)|	\
	 _BV(27)|_BV(26)|_BV(25)|_BV(24)|	\
	 _BV(23)|_BV(21)|_BV(20)|_BV(19)|	\
	 _BV(18)|_BV(17)|_BV(16)|_BV(15)|	\
	 _BV(14)|_BV(13)|_BV(12)|_BV(11)|	\
	 _BV(10)|_BV(9)_BV(8))

/* D7.5.6 CNTKCTL_EL1, Conter-timer Kernel Control register */
#define CNTKCTL_EL0PCTEN	_BV(0)
#define CNTKCTL_EL0VCTEN	_BV(1)
#define CNTKCTL_EL0VTEN		_BV(8)
#define CNTKCTL_EL0PTEN		_BV(9)

#define CNTKCTL_EL1_RES0			\
	(_BV(31)|_BV(30)|_BV(29)|_BV(28)|	\
	 _BV(27)|_BV(26)|_BV(25)|_BV(24)|	\
	 _BV(23)|_BV(21)|_BV(20)|_BV(19)|	\
	 _BV(18)|_BV(17)|_BV(16)|_BV(15)|	\
	 _BV(14)|_BV(13)|_BV(12)|_BV(11)|	\
	 _BV(10))

/* Fields common to CNTHCTL_EL2/CNTKCTL_EL1 */
#define	CNTXCTL_FV(name, value)	_FV(CNTXCTL_##name, value)

#define CNTXCTL_EVNTEN		_BV(2)
#define CNTXCTL_EVNTDIR		_BV(3)
#define CNTXCTL_EVNTI_OFFSET	4
#define CNTXCTL_EVNTI_MASK	0x0F
#define CNTXCTL_EVNTI(value)	CNTXCTL_FV(EVNTI, value)

/* D7.5.3  CNTHP_CTL_EL0,
 *         Counter-timer Hypervisor Physical Timer Control register
 * D7.5.7  CNTP_CTL_EL0,
 *         Counter-timer Physical Timer Control register
 * D7.5.11 CNTPS_CTL_EL0,
 *         Counter-timer Physical Secure Timer Control register
 * D7.5.14 CNTV_CTL_EL0,
 *         Counter-timer Virtual Timer Control register
 */
#define CNTX_CTL_ENABLE		_BV(0)
#define CNTX_CTL_IMASK		_BV(1)
#define CNTX_CTL_ISTATUS	_BV(2)

#define TICKS_TO_MICROSECONDS	(SYSTICK_HW_FREQUENCY / UL(1000000))
#define TICKS_TO_MILISECONDS	(TICKS_TO_MICROSECONDS / 1000)

#include <asm/mach/timer.h>

void __systick_mask_irq(void);
void __systick_unmask_irq(void);
uint64_t __systick_read(void);
void __systick_set_timeout(timeout_t match_val);
void __systick_init(void);

#endif /* __TIMER_ARM64_H_INCLUDE__ */
