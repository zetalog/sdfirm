#ifndef __COMPILER_H_INCLUDE__
#define __COMPILER_H_INCLUDE__

#include <target/config.h>

/* compiler specific */

#define __barrier()	asm volatile("": : :"memory")

#ifdef __GNUC__
#include <target/compiler_gcc.h>
#elif SDCC
#include <target/compiler_sdcc.h>
#elif __C51__
#include <target/compiler_keil.h>
#endif

#ifndef __always_inline
#define __always_inline		inline
#endif
#define ARRAY_ADDR(arr)		(&((arr)[0]))
#define ARRAY_SIZE(arr)		(sizeof(arr) / sizeof((arr)[0]))

#include <target/assembler.h>
#include <target/linkage.h>

#endif /* __COMPILER_H_INCLUDE__ */
