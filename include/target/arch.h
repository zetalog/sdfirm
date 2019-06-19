#ifndef __ARCH_H_INCLUDE__
#define __ARCH_H_INCLUDE__

#include <asm/arch.h>
#include <target/cpus.h>
#include <target/cache.h>
#include <target/page.h>
#include <target/smp.h>

#ifndef __ASSEMBLY__
extern uintptr_t __stext[];
extern uintptr_t __etext[];
extern uintptr_t __sdata[];
extern uintptr_t __edata[];
extern uintptr_t __bss_start[];
extern uintptr_t __bss_stop[];
extern uintptr_t __pageable_start[];

#define PAGEABLE_START	((uint64_t)(&__pageable_start))
#define PAGEABLE_END	PERCPU_STACKS_START
#endif /* !__ASSEMBLY__ */

#endif /* __ARCH_H_INCLUDE__ */
