#ifndef __SPIFLASH_DRIVER_H_INCLUDE__
#define __SPIFLASH_DRIVER_H_INCLUDE__

/* chip drivers */
#ifdef CONFIG_SPIFLASH_W25Q32
#include <driver/spiflash_w25q32.h>
#endif

#ifndef ARCH_HAVE_SPIFLASH_CHIP
#define spiflash_hw_chip_init()
#endif

#endif /* __SPIFLASH_DRIVER_H_INCLUDE__ */
