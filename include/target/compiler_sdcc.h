#ifndef __COMPILER_SDCC_H_INCLUDE__ 
#define __COMPILER_SDCC_H_INCLUDE__

#define inline

#ifdef __ASSEMBLY__
#define ULL(x)		x
#define UL(x)		x
#else
#define ULL(x)		x##ULL
#define UL(x)		x##UL
typedef unsigned char * va_list;
#endif

/* SDCC stores numbers in little-endian format */
#define DEFINE_ISR(x, y)		x __interrupt(y)
#define DECLARE_ISR			DEFINE_ISR

#pragma disable_warning 85

#define __packed

#endif /* __COMPILER_SDCC_H_INCLUDE__ */
