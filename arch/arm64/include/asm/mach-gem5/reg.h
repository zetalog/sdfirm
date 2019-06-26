#ifndef __REG_GEM5_H_INCLUDE__
#define __REG_GEM5_H_INCLUDE__

#include <target/sizes.h>

#define ROM_BASE		0x0000000
#define ROMEND			0x4000000

#if 0
#define RAM_BASE		0x80000000
#define RAMEND			0xA0000000
#else
#define RAM_BASE		0x0000000
#define RAMEND			0x4000000
#endif

#define DEV_BASE		0x1A000000
#define DEVEND			0x30000000

#endif /* __REG_GEM5_H_INCLUDE__ */
