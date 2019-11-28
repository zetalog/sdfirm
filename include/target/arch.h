#ifndef __ARCH_H_INCLUDE__
#define __ARCH_H_INCLUDE__

#include <target/cpus.h>
#include <target/cache.h>
#include <target/paging.h>
#include <asm/arch.h>

#ifdef CONFIG_XIP
#define TEXT_OFFSET		CONFIG_BOOT_BASE
#else
#define TEXT_OFFSET		CONFIG_LOAD_BASE
#endif

/* Virtual address space */
#ifdef __ASSEMBLY__
#if defined(CONFIG_XIP) && !defined(CONFIG_BFM)
#define SDFIRM_START		_sdata
#else
#define SDFIRM_START		_start
#endif
#define SDFIRM_END		_end
#define SDFIRM_DATA_END		_edata
#else /* !__ASSEMBLY__ */
extern uintptr_t _start[];
extern uintptr_t _end[];
extern uintptr_t _stext[];
extern uintptr_t _etext[];
extern uintptr_t _edata[];

extern uintptr_t __stext[];
extern uintptr_t __etext[];
extern uintptr_t _sdata[];
extern uintptr_t _edata[];
extern uintptr_t __start_rodata[];
extern uintptr_t __end_rodata[];

#if defined(CONFIG_XIP) && !defined(CONFIG_BFM)
#define SDFIRM_START		((caddr_t)(&_sdata))
#else
#define SDFIRM_START		((caddr_t)(&_start))
#endif
#define SDFIRM_END		((caddr_t)(&_end))
#define SDFIRM_DATA_END		((caddr_t)(&_edata))
#endif /* __ASSEMBLY__ */

/* CPU initial stacks, assign 1 page for each CPU as stack */
#define PERCPU_STACKS_START		SDFIRM_DATA_END
#define PERCPU_STACKS_SIZE		(PERCPU_STACK_SIZE * NR_CPUS)
#define PERCPU_STACKS_END		(PERCPU_STACKS_START + PERCPU_STACKS_SIZE)
#define PERCPU_STACK_SHIFT		PAGE_SHIFT
#define PERCPU_STACK_SIZE		PAGE_SIZE
#define PERCPU_STACK_START(x)		(PERCPU_STACKS_START + (x) * PERCPU_STACK_SIZE)
#define FIXADDR_START			(FIXADDR_END - FIXADDR_SIZE)

/* Physcal address space */
#define PAGEABLE_START			__PA(PERCPU_STACKS_END)
#define PAGEABLE_END			RAMEND

#include <target/smp.h>

#endif /* __ARCH_H_INCLUDE__ */
