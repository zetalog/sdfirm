#ifndef __ARCH_H_INCLUDE__
#define __ARCH_H_INCLUDE__

#include <asm/arch.h>
#include <target/cpus.h>
#include <target/cache.h>
#include <target/page.h>
#include <target/smp.h>

#ifdef CONFIG_XIP
#define TEXT_OFFSET		CONFIG_TEXT_ROM_BASE
#else
#define TEXT_OFFSET		CONFIG_TEXT_RAM_BASE
#endif

#ifndef __ASSEMBLY__
extern uintptr_t __stext[];
extern uintptr_t __etext[];
extern uintptr_t __sdata[];
extern uintptr_t __edata[];
extern uintptr_t __start_rodata[];
extern uintptr_t __end_rodata[];
extern uintptr_t __bss_start[];
extern uintptr_t __bss_stop[];
extern uintptr_t __end[];

#define SDFIRM_END	((uint64_t)(&__end))
#define PAGEABLE_START	SDFIRM_END
#define PAGEABLE_END	PERCPU_STACKS_START
#endif /* !__ASSEMBLY__ */

#endif /* __ARCH_H_INCLUDE__ */
