#ifndef __BLOCK_H_INCLUDE__
#define __BLOCK_H_INCLUDE__

typedef uint8_t blk_t;
typedef uint32_t blk_addr_t;

#ifdef CONFIG_BLOCK_MAX_DRIVERS
#define BLOCK_MAX_DRIVERS		CONFIG_BLOCK_MAX_DRIVERS
#else
#define BLOCK_MAX_DRIVERS		1
#endif

uint8_t block_readb(blk_t dev, blk_addr_t addr);
void block_writeb(blk_t dev, blk_addr_t addr, uint8_t value);

#endif
