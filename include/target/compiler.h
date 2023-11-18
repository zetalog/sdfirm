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

/* Used to print macro value during compile time:
 * #pragma message(__macro(XXX))
 */
#define __compiletime_macro(x)	#x"="__stringify(x)

#define ARRAY_ADDR(arr)		(&((arr)[0]))
#define ARRAY_SIZE(arr)		(sizeof(arr) / sizeof((arr)[0]))

#ifndef __compiletime_warning
#define __compiletime_warning(message)
#endif

#ifndef __compiletime_error
#define __compiletime_error(message)
#endif

#define compiletime_assert(condition, msg)				\
	do {								\
		extern void nonexist(void) __compiletime_error(msg);	\
		if (!(condition))					\
			nonexist();					\
	} while (0)

#define BUILD_BUG_ON_MSG(cond, msg)	compiletime_assert(!(cond), msg)
#define BUILD_BUG_ON(condition)		\
	BUILD_BUG_ON_MSG(condition, "BUILD_BUG_ON failed: " #condition)
#define BUILD_BUG()			\
	BUILD_BUG_ON_MSG(1, "BUILD_BUG failed")
/* Force a compilation error if a constant expression is not a power of 2 */
#define __BUILD_BUG_ON_NOT_POWER_OF_2(n)	\
	BUILD_BUG_ON(((n) & ((n) - 1)) != 0)
#define BUILD_BUG_ON_NOT_POWER_OF_2(n)			\
	BUILD_BUG_ON((n) == 0 || (((n) & ((n) - 1)) != 0))

#include <target/assembler.h>
#include <target/linkage.h>

#endif /* __COMPILER_H_INCLUDE__ */
