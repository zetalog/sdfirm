#ifndef __SPIFLASH_W25Q32_H_INCLUDE__
#define __SPIFLASH_W25Q32_H_INCLUDE__

#include <target/spi.h>

#ifndef ARCH_HAVE_SPIFLASH_CHIP
#define ARCH_HAVE_SPIFLASH_CHIP	1
#else
#error "Multiple SPI flash chip types defined"
#endif

#define SPIFLASH_HW_MAX_FREQ		80000 /* 80MHz */
#define SPIFLASH_HW_MAX_SIZE		(4 * 1024 * 1024)
void spiflash_hw_chip_init(void);

#endif /* __SPIFLASH_W25Q32_H_INCLUDE__ */
