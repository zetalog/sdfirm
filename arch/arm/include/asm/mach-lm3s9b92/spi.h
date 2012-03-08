#ifndef __SPI_LM3S9B92_H_INCLUDE__
#define __SPI_LM3S9B92_H_INCLUDE__

#include <asm/mach/ssi.h>

#ifndef ARCH_HAVE_SPI
#define ARCH_HAVE_SPI		1
#else
#error "Multiple SPI controller defined"
#endif


void spi_hw_write_byte(uint8_t byte);
uint8_t spi_hw_read_byte(void);
void spi_hw_config_mode(uint8_t mode);
void spi_hw_config_freq(uint32_t khz);
void spi_hw_chip_select(uint8_t chip);
void spi_hw_deselect_chips(void);
void spi_hw_ctrl_start(void);
void spi_hw_ctrl_stop(void);
void spi_hw_ctrl_init(void);

#endif /* __SPI_LM3S9B92_H_INCLUDE__ */
