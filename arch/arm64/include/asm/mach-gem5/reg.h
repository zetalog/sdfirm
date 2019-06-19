#ifndef __REG_GEM5_H_INCLUDE__
#define __REG_GEM5_H_INCLUDE__

#define RAM_START		0x0000000000
#define RAM_SIZE		0x0004000000
#define RAM_END			(RAM_START + RAM_SIZE)

#define ROM_BASE		RAM_START
#define ROMEND			RAM_END

#define RAM_BASE		RAM_START
#define RAMEND			RAM_END

/* APC stack/heap:
 *
 * SYS IMEM can be used to synchronize between IMC/APC, while stack
 * variables needn't be synchronized. Considering the rarity of SYS IMC
 * (only 256K), the PERCPU stacks are put in the IMC internal RAM.
 */
#define PERCPU_STACKS_START             (PERCPU_STACKS_END - PERCPU_STACKS_SIZE)
#define PERCPU_STACKS_END               RAM_END
#define PERCPU_STACKS_SIZE              (PERCPU_STACK_SIZE * NR_CPUS)
#define PERCPU_STACK_SHIFT              PAGE_SHIFT
#define PERCPU_STACK_SIZE               PAGE_SIZE
#define PERCPU_STACK_START(x)           (PERCPU_STACKS_START + (x) * PERCPU_STACK_SIZE)

#endif /* __REG_GEM5_H_INCLUDE__ */
