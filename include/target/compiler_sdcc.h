#ifndef __COMPILER_SDCC_H_INCLUDE__ 
#define __COMPILER_SDCC_H_INCLUDE__

#define inline

/* SDCC stores numbers in little-endian format */
#define DEFINE_ISR(x, y)		x __interrupt(y)
#define DECLARE_ISR			DEFINE_ISR

#pragma disable_warning 85

#define __packed

#endif /* __COMPILER_SDCC_H_INCLUDE__ */
