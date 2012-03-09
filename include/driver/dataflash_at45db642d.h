#ifndef __DATAFLASH_AT45DB642D_H_INCLUDE__
#define __DATAFLASH_AT45DB642D_H_INCLUDE__

#include <target/config.h>
#include <target/generic.h>
#include <target/spi.h>

#ifndef ARCH_HAVE_DATAFLASH_CHIP
#define ARCH_HAVE_DATAFLASH_CHIP	1
#else
#error "Multiple DataFlash chip types defined"
#endif

mtd_size_t dataflash_hw_page_size(void);
#define DATAFLASH_HW_PAGE_SIZE		1056
#define DATAFLASH_HW_MAX_PAGES		8192
#define DATAFLASH_HW_MAX_FREQ		66000
#define DATAFLASH_HW_SPI_MODE		(SPI_MSB | SPI_MODE_3)

void dataflash_hw_chip_init(void);

#endif /* __DATAFLASH_AT45DB642D_H_INCLUDE__ */
