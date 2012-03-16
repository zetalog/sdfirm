#include <target/spi.h>
#include <target/arch.h>

#ifdef CONFIG_PORTING_SPI
#define __SPI_HW_LBM	_BV(LBM)
#else
#define __SPI_HW_LBM	0
#endif
#ifdef CONFIG_SPI_MASTER
#define __SPI_HW_MS	0
static inline void __ssi0_hw_config_cs(void)
{
	gpio_hw_config_mux(GPIOA, 3, GPIO_MUX_NONE);
	gpio_hw_config_pad(GPIOA, 3, GPIO_DIR_OUT,
			   GPIO_PAD_PP, GPIO_DRIVE_2MA);
}

static inline void __ssi0_hw_write_cs(uint8_t val)
{
	gpio_hw_write_pin(GPIOA, 3, val);
}
#else
#define __SPI_HW_MS	_BV(MS)
static inline void __ssi0_hw_config_cs(void)
{
	gpio_hw_config_mux(GPIOA, 3, GPIOA3_MUX_SSI0FSS);
	gpio_hw_config_pad(GPIOA, 3, GPIO_DIR_HW,
			   GPIO_PAD_PP, GPIO_DRIVE_2MA);
}
#endif
#define __SPI_HW_CTRL	(__SPI_HW_MS | __SPI_HW_LBM)

LM3S9B92_SSI(0)

uint8_t __spi_hw_mode;
#define __spi_hw_is_lsb()	(spi_order(__spi_hw_mode) == SPI_LSB)

static inline void __ssi0_hw_config_pins(void)
{
	pm_hw_resume_device(DEV_GPIOA, DEV_MODE_ON);
	/* config SSI0 pin */
	gpio_hw_config_mux(GPIOA, 2, GPIOA2_MUX_SSI0CLK);
	gpio_hw_config_pad(GPIOA, 2, GPIO_DIR_HW,
			   GPIO_PAD_PP, GPIO_DRIVE_2MA);
	gpio_hw_config_mux(GPIOA, 4, GPIOA4_MUX_SSI0RX);
	gpio_hw_config_pad(GPIOA, 4, GPIO_DIR_HW,
			   GPIO_PAD_PP, GPIO_DRIVE_2MA);
	gpio_hw_config_mux(GPIOA, 5, GPIOA5_MUX_SSI0TX);
	gpio_hw_config_pad(GPIOA, 5, GPIO_DIR_HW,
			   GPIO_PAD_PP, GPIO_DRIVE_2MA);
	__ssi0_hw_config_cs();
}

void spi_hw_write_byte(uint8_t byte)
{
	if (__spi_hw_is_lsb())
		byte = bitrev8(byte);
	while (__ssi0_hw_tx_full());
	__ssi0_hw_write_byte(byte);
}

uint8_t spi_hw_read_byte(void)
{
	uint8_t byte;

	while (__ssi0_hw_rx_empty());
	byte = __ssi0_hw_read_byte();
	if (__spi_hw_is_lsb())
		byte = bitrev8(byte);

	return byte;
}

void spi_hw_ctrl_start(void)
{
}

void spi_hw_config_mode(uint8_t mode)
{
	__spi_hw_mode = mode;
	__ssi0_hw_config_mode(mode);
	__ssi0_hw_config_frame(__SSI_FRF_FREESCALE);
	__ssi0_hw_config_bits(8);
}

void spi_hw_deselect_chips(void)
{
	while (!__ssi0_hw_io_idle());
	__ssi0_hw_write_cs(1);
}

void spi_hw_chip_select(uint8_t chip)
{
	spi_hw_deselect_chips();
	__ssi0_hw_write_cs(0);
}

void spi_hw_ctrl_stop(void)
{
}

void spi_hw_config_freq(uint32_t khz)
{
	uint32_t ratio;
	uint8_t cpsdvsr;
	uint16_t scr;

	BUG_ON(khz > SPI_HW_MAX_FREQ);

	/* The calculation is based on the following definitions:
	 *
	 * SSICLK = SysClk / (CPSDVSR * (1 + SCR)) ->
	 * CPSDVSR * (1 + SCR) = SysClk / SSICLK ->
	 * CPSDVSR * (1 + SCR) = CLK_SYS / khz = ratio
	 */
	ratio = div32u(CLK_SYS, khz);
	cpsdvsr = 0;
	do
	{
		cpsdvsr += 2;
		scr = (uint16_t)div32u(ratio, cpsdvsr) - 1;
	} while (scr > 255);

	/* ensure calculated SSICLK < wanted SSICLK */
	while (div32u(CLK_SYS, mul16u(cpsdvsr, (1+scr))) > khz)
		scr++;

	__ssi0_hw_config_prescale(cpsdvsr);
	__ssi0_hw_config_phase(scr);
}

void spi_hw_ctrl_init(void)
{
	pm_hw_resume_device(DEV_SSI0, DEV_MODE_ON);
	__ssi0_hw_config_pins();
	__ssi0_hw_ctrl_enable();
	__ssi0_hw_config_ctrl(__SPI_HW_CTRL);
}
