#ifndef __DATAFLASH_DRIVER_H_INCLUDE__
#define __DATAFLASH_DRIVER_H_INCLUDE__

#include <target/config.h>

/* chip drivers */
#ifdef CONFIG_DATAFLASH_AT45DB642D
#include <driver/dataflash_at45db642d.h>
#endif

#ifndef ARCH_HAVE_DATAFLASH_CHIP
#define dataflash_hw_page_size()	0
#define DATAFLASH_HW_PAGE_SIZE		0
#define DATAFLASH_HW_MAX_PAGES		0
#define DATAFLASH_HW_MAX_FREQ		0
#define DATAFLASH_HW_SPI_MODE		0
#define dataflash_hw_chip_init()
#endif

#endif /* __DATAFLASH_DRIVER_H_INCLUDE__ */
