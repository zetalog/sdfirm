#ifndef __MEM_LM3S9B92_H_INCLUDE__
#define __MEM_LM3S9B92_H_INCLUDE__

#include <target/config.h>
#include <asm/reg.h>

#define SYSTEM_BASE		0x400FE000 /* system control */
#define FLASH_BASE		0x400FD000 /* flash memory control */
#define CORTEXM3_BASE		0xE000E000 /* Cortex-M3 peripherals */

#define SYSTEM(offset)		(SYSTEM_BASE+offset)
#define FLASH(offset)		(FLASH_BASE+offset)
#define CORTEXM3(offset)	(CORTEXM3_BASE+offset)

#define USECRL			FLASH(0x140)

#endif /* __MEM_LM3S9B92_H_INCLUDE__ */
