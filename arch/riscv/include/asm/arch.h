#ifndef __ARCH_RISCV_H_INCLUDE__
#define __ARCH_RISCV_H_INCLUDE__

#include <target/config.h>
#include <target/barrier.h>
#include <asm/mach/arch.h>

#define wait_irq()		wfi()

#endif /* __ARCH_RISCV_H_INCLUDE__ */
