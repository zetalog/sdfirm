#ifndef __COMPILER_SDCC_H_INCLUDE__ 
#define __COMPILER_SDCC_H_INCLUDE__

#define inline

#ifndef __ASSEMBLY__
typedef unsigned char * va_list;
#endif

/* SDCC stores numbers in little-endian format */
#define DEFINE_ISR(x, y)		x __interrupt(y)
#define DECLARE_ISR			DEFINE_ISR

#pragma disable_warning 85

#define __packed

#define READ_ONCE(x)			(x)
#define WRITE_ONCE(x, val)		((x) = (val))

#endif /* __COMPILER_SDCC_H_INCLUDE__ */
