#ifndef __COMPILER_KEIL_H_INCLUDE__
#define __COMPILER_KEIL_H_INCLUDE__

#ifndef __always_inline
#define __always_inline		inline
#endif

#define DEFINE_ISR(x, y)		x interrupt y
#define DECLARE_ISR(x, y)

#define volatile

#endif /* __COMPILER_KEIL_H_INCLUDE__ */
