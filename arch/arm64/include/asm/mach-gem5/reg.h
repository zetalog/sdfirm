#ifndef __REG_GEM5_H_INCLUDE__
#define __REG_GEM5_H_INCLUDE__

#include <target/config.h>
#include <target/sizes.h>

#define ROM_BASE		0x0000000
#define ROMEND			0x4000000

#ifdef CONFIG_GEM5_RAM_DEFAULT
#define RAM_BASE		0x80000000
#define RAMEND			0xA0000000
#else
#define RAM_BASE		CONFIG_GEM5_RAM_BASE
#define RAMEND			RAM_BASE + (ULL(1) << CONFIG_GEM5_RAM_BITS)
#endif

#define IDMAP_DEV_BASE		0x1C090000
#define IDMAP_DEVEND		PAGE_SIZE

#endif /* __REG_GEM5_H_INCLUDE__ */
