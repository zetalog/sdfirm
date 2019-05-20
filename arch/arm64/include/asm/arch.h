#ifndef __ARCH_ARM64_H_INCLUDE__
#define __ARCH_ARM64_H_INCLUDE__

#include <target/config.h>
#include <target/barrier.h>
#include <asm/mach/arch.h>
#include <asm/reg.h>
#include <asm/sysreg.h>

#define current_el()	CURRENT_EL(read_sysreg(CurrentEL))

/* The default boot exception level, however, each image should use
 * current_el() to aid determination.
 */
#ifdef CONFIG_TZ_SECURE
#define EL_BOOT		ARM_EL3
#endif
#ifdef CONFIG_TZ_NORMAL
#define EL_BOOT		ARM_EL1
#endif

/* The default runtime exception level, however, special image may contain
 * runtime code running on other exception levels.
 */
#ifdef CONFIG_SYS_MONITOR
#define EL_RUNTIME	ARM_EL3
#endif
#ifdef CONFIG_SYS_KERNEL
#ifdef CONFIG_VM_HYPERVISOR
#define EL_RUNTIME	ARM_EL2
#endif
#ifdef CONFIG_VM_GUEST
#define EL_RUNTIME	ARM_EL1
#endif
#endif
#ifdef CONFIG_SYS_USER
#define EL_RUNTIME	ARM_EL0
#endif

#define nop()		__nops(1)
#define wait_irq()	wfi()

#endif /* __ARCH_ARM64_H_INCLUDE__ */
