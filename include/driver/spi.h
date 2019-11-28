#ifndef __SPI_DRIVER_H_INCLUDE__
#define __SPI_DRIVER_H_INCLUDE__

#ifdef CONFIG_ARCH_HAS_SPI
#include <asm/mach/spi.h>
#endif

#ifndef ARCH_HAVE_SPI
#define SPI_HW_MAX_FREQ		0
#define spi_hw_config_mode(mode)
#define spi_hw_config_freq(khz)
#define spi_hw_read_byte()	0x00
#define spi_hw_write_byte(byte)
#define spi_hw_chip_select(chip)
#define spi_hw_deselect_chips()
#define spi_hw_ctrl_init()
#define spi_hw_ctrl_start()
#define spi_hw_ctrl_stop()
#endif

#endif /* __SPI_DRIVER_H_INCLUDE__ */
