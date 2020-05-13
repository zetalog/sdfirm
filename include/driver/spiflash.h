#ifndef __SPIFLASH_DRIVER_H_INCLUDE__
#define __SPIFLASH_DRIVER_H_INCLUDE__

/* chip drivers */
#ifdef CONFIG_ARCH_HAS_SPIFLASH
#include <asm/mach/spiflash.h>
#endif

#ifndef ARCH_HAVE_SPIFLASH_CHIP
#define spiflash_hw_chip_init()
#endif

#endif /* __SPIFLASH_DRIVER_H_INCLUDE__ */
