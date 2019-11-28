#ifndef __ARCH_ARM64_H_INCLUDE__
#define __ARCH_ARM64_H_INCLUDE__

#include <target/barrier.h>
#include <asm/mach/arch.h>
#include <asm/reg.h>
#include <asm/sysreg.h>

#define current_el()		CURRENT_EL(read_sysreg(CurrentEL))

/* The default boot exception level, however, each image should use
 * current_el() to aid determination.
 */
#ifdef CONFIG_TZ_SECURE
#define EL_BOOT			ARM_EL3
#endif
#ifdef CONFIG_TZ_NORMAL
#define EL_BOOT			ARM_EL1
#endif

/* The default runtime exception level, however, special image may contain
 * runtime code running on other exception levels.
 */
#ifdef CONFIG_SYS_MONITOR
#define EL_RUNTIME		ARM_EL3
#define SYS_SECURE_MONITOR	1
#endif
#if defined(CONFIG_SYS_HYPERVISOR) && !defined(CONFIG_SYS_HOST)
#define EL_RUNTIME		ARM_EL2
#define SYS_VMM			1
#endif
#ifdef CONFIG_SYS_HOST
#define EL_RUNTIME		ARM_EL2
#define SYS_HOST		1
#define SYS_KERNEL		1
#endif
#ifdef CONFIG_SYS_GUEST
#define EL_RUNTIME		ARM_EL1
#define SYS_GUEST		1
#define SYS_KERNEL		1
#endif
#ifdef CONFIG_SYS_USER
#define EL_RUNTIME		ARM_EL0
#define SYS_USER		1
#endif

#define nop()			__nops(1)
#define wait_irq()		wfi()

#endif /* __ARCH_ARM64_H_INCLUDE__ */
