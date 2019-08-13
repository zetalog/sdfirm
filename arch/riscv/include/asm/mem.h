#ifndef __RISCV_MEM_H_INCLUDE__
#define __RISCV_MEM_H_INCLUDE__

#include <target/types.h>

#ifdef CONFIG_32BIT
typedef uint32_t phys_addr_t;
#else
typedef uint64_t phys_addr_t;
#endif

#include <asm/mach/mem.h>

#endif /* __ARM64_MEM_H_INCLUDE__ */
