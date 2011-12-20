#ifndef __SCB_LM3S9B92_H_INCLUDE__
#define __SCB_LM3S9B92_H_INCLUDE__

#include <target/config.h>
#include <target/generic.h>
#include <asm/reg.h>
#include <asm/mach/mem.h>

/* System Control Block (SCB) registers */
#define ACTLR			CORTEXM3(0x008)
#define DISFOLD			2
#define DISWBUF			1
#define DISMCYC			0

#define CPUID			CORTEXM3(0xD00)
#define INTCTRL			CORTEXM3(0xD04)
#define VTABLE			CORTEXM3(0xD08)
#define APINT			CORTEXM3(0xD0C)
#define SYSCTRL			CORTEXM3(0xD10)
#define CFGCTRL			CORTEXM3(0xD14)
#define SYSPRI1			CORTEXM3(0xD18)
#define SYSPRI2			CORTEXM3(0xD1C)
#define SYSPRI3			CORTEXM3(0xD20)
#define SYSHNDCTRL		CORTEXM3(0xD24)
#define FAULTSTAT		CORTEXM3(0xD28)
#define HFAULTSTAT		CORTEXM3(0xD2C)
#define MMADDR			CORTEXM3(0xD34)
#define FAULTADDR		CORTEXM3(0xD38)

#endif /* __SCB_LM3S9B92_H_INCLUDE__ */
