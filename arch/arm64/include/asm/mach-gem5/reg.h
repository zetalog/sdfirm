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

/* APC stack/heap:
 *
 * SYS IMEM can be used to synchronize between IMC/APC, while stack
 * variables needn't be synchronized. Considering the rarity of SYS IMC
 * (only 256K), the PERCPU stacks are put in the IMC internal RAM.
 */
#define PERCPU_STACKS_START             (PERCPU_STACKS_END - PERCPU_STACKS_SIZE)
#define PERCPU_STACKS_END               RAMEND
#define PERCPU_STACKS_SIZE              (PERCPU_STACK_SIZE * NR_CPUS)
#define PERCPU_STACK_SHIFT              PAGE_SHIFT
#define PERCPU_STACK_SIZE               PAGE_SIZE
#define PERCPU_STACK_START(x)           (PERCPU_STACKS_START + (x) * PERCPU_STACK_SIZE)

#endif /* __REG_GEM5_H_INCLUDE__ */
