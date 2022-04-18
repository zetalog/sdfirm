#ifndef __COMPILER_GCC_H_INCLUDE__
#define __COMPILER_GCC_H_INCLUDE__

#ifdef __CHECKER__
# define __safe		__attribute__((safe))
# define __force	__attribute__((force))
# define __nocast	__attribute__((nocast))
# define __iomem	__attribute__((noderef, address_space(2)))
#else
# define __safe
# define __force
# define __nocast
# define __iomem
#endif

#if (__GNUC__ == 4 && __GNUC_MINOR__ >= 1) || (__GNUC__ > 4)
#  define __INTR_ATTRS used, externally_visible
#else /* GCC < 4.1 */
#  define __INTR_ATTRS used
#endif

#ifdef CONFIG_CC_ISR_VECTOR
#define DEFINE_ISR(vector, ...)	\
	void vector(irq_t irq) __attribute__ ((signal,__INTR_ATTRS)) __VA_ARGS__;\
	void vector(irq_t irq)
#define _VECTOR(N) __vector_ ## N
#endif
#define ISR_BLOCK
#define ISR_NOBLOCK    __attribute__((interrupt))
#define ISR_NAKED      __attribute__((naked))
#define ISR_ALIASOF(v) __attribute__((alias(__STRINGIFY(v))))

#ifndef __ASSEMBLY__
typedef __builtin_va_list va_list;
#endif

#define __noreturn			__attribute__((noreturn))
#define __packed			__attribute__((__packed__))
#define __used				__attribute__((__used__))
#define __pure				__attribute__((__pure__))
#define __naked				__attribute__((__naked__))
#define __unused			__attribute__((__unused__))
#define __align(a)			__attribute__((__aligned__(a)))
#define __always_inline			inline __attribute__((__always_inline__))
#define __section(S)			__attribute__((__section__(#S)))

#define __stringify_1(x...)		#x
#define __stringify(x...)		__stringify_1(x)

#define __printf_chk(c)			\
	__attribute__((__format__(__printf__, c, c+1)))

#ifdef CONFIG_CC_ALWAYS_INLINE
#define __inline			__always_inline
#else
#define __inline			inline
#endif

#define likely(x)			__builtin_expect(!!(x), 1)
#define unlikely(x)			__builtin_expect(!!(x), 0)
#define unreachable()			__builtin_unreachable()
#define __compiletime_warning(message)	__attribute__((__warning__(message)))
#define __compiletime_error(message)	__attribute__((__error__(message)))

#define RELOC_HIDE(ptr, off)				\
	({						\
		unsigned long __ptr;			\
		__asm__ ("" : "=r"(__ptr) : "0"(ptr));	\
		(typeof(ptr)) (__ptr + (off));		\
	})

#endif  /* __COMPILER_GCC_H_INCLUDE__ */
