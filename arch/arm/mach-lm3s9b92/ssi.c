#include <target/spi.h>
#include <target/arch.h>

#define MAX_SSI_CLK	CLK_SYS / 2

LM3S9B92_SSI(0)
LM3S9B92_SSI(1)

static inline void __ssi0_hw_config_pins(void)
{
	pm_hw_resume_device(DEV_GPIOA, DEV_MODE_ON);
	/* config SSI0 pin */
	gpio_hw_config_mux(GPIOA, 2, GPIOA2_MUX_SSI0CLK);
	gpio_hw_config_pad(GPIOA, 2, GPIO_DIR_HW, GPIO_PAD_PP, GPIO_DRIVE_2MA);
	gpio_hw_config_mux(GPIOA, 3, GPIOA3_MUX_SSI0FSS);
	gpio_hw_config_pad(GPIOA, 3, GPIO_DIR_HW, GPIO_PAD_PP, GPIO_DRIVE_2MA);
	gpio_hw_config_mux(GPIOA, 4, GPIOA4_MUX_SSI0RX);
	gpio_hw_config_pad(GPIOA, 4, GPIO_DIR_HW, GPIO_PAD_PP, GPIO_DRIVE_2MA);
	gpio_hw_config_mux(GPIOA, 5, GPIOA5_MUX_SSI0TX);
	gpio_hw_config_pad(GPIOA, 5, GPIO_DIR_HW, GPIO_PAD_PP, GPIO_DRIVE_2MA);

	pm_hw_resume_device(DEV_GPIOG, DEV_MODE_ON);
	/* config SSI0 dev select */
	gpio_hw_config_mux(GPIOG, 0, GPIO_MUX_NONE);
	gpio_hw_config_pad(GPIOG, 0, GPIO_DIR_OUT, GPIO_PAD_PP, GPIO_DRIVE_2MA);
	gpio_hw_config_mux(GPIOG, 1, GPIO_MUX_NONE);
	gpio_hw_config_pad(GPIOG, 1, GPIO_DIR_OUT, GPIO_PAD_PP, GPIO_DRIVE_2MA);
}

static inline void __ssi1_hw_config_pins(void)
{
	pm_hw_resume_device(DEV_GPIOE, DEV_MODE_ON);
	/* config SSI1 pin */
	gpio_hw_config_mux(GPIOE, 0, GPIOE0_MUX_SSI1CLK);
	gpio_hw_config_pad(GPIOE, 0, GPIO_DIR_HW, GPIO_PAD_PP, GPIO_DRIVE_2MA);
	gpio_hw_config_mux(GPIOE, 1, GPIOE1_MUX_SSI1FSS);
	gpio_hw_config_pad(GPIOE, 1, GPIO_DIR_HW, GPIO_PAD_PP, GPIO_DRIVE_2MA);
	gpio_hw_config_mux(GPIOE, 2, GPIOE2_MUX_SSI1RX);
	gpio_hw_config_pad(GPIOE, 2, GPIO_DIR_HW, GPIO_PAD_PP, GPIO_DRIVE_2MA);
	gpio_hw_config_mux(GPIOE, 3, GPIOE3_MUX_SSI1TX);
	gpio_hw_config_pad(GPIOE, 3, GPIO_DIR_HW, GPIO_PAD_PP, GPIO_DRIVE_2MA);
}

#if 1
uint16_t ssi_nr = 0;
uint8_t ssi_mode = 0;
uint32_t ssi_khz = 0;

#define __ssi_hw_write_byte(byte)			\
	__raw_writel(byte, (SSIDR(0) + ssi_nr))

static inline uint8_t __ssi_hw_read_byte(void)
{
	return (uint8_t)__raw_readl((SSIDR(0) + ssi_nr));
}

#define __ssi_hw_config_clk_prescale(pres)		\
	__raw_writel(pres, SSICPSR(0) + ssi_nr)

#define __ssi_hw_config_clk_phase(phase)		\
	__raw_writel_mask(phase<<(__SSI_SCR_OFFSET),	\
			  __SSI_SCR_MASK<<(__SSI_SCR_OFFSET),	\
			  SSICR0(0) + ssi_nr)

#define __ssi_hw_config_mode(mode)			\
	__raw_writel_mask(LOHALF(spi_mode(mode)<<__SSI_MODE_OFFSET),	\
			  __SSI_MODE_MASK<<__SSI_MODE_OFFSET, SSICR0(0) + ssi_nr)

#if 0
#define __ssi_hw_ctrl_disable()			\
	do {					\
		while (__raw_testl_atomic(SSE, (SSICR1(0) + ssi_nr)))	\
			__raw_clearl_atomic(SSE, (SSICR1(0) + ssi_nr));	\
	} while (0)
#endif

#define __ssi_hw_ctrl_enable()		\
	do {				\
		do {			\
			__raw_setl_atomic(SSE, (SSICR1(0) + ssi_nr));	\
		} while (!__raw_testl_atomic(SSE, (SSICR1(0) + ssi_nr)));\
	} while (0)

#define __ssi_hw_master_mode()				\
	__raw_setl_atomic(MS, (SSICR1(0) + ssi_nr))

#define __ssi_hw_config_frame_type(type)		\
	__raw_writel_mask(type<<(__SSI_FRF_OFFSET),	\
			  __SSI_FRF_MASK<<(__SSI_FRF_OFFSET),	\
			  (SSICR0(0) + ssi_nr))

#define __ssi_hw_config_size_8bit()		\
	__raw_writel_mask(__SSI_DSS_8BIT<<(__SSI_DSS_OFFSET),	\
		          __SSI_DSS_MASK<<(__SSI_DSS_OFFSET),	\
			  (SSICR0(0) + ssi_nr))

#define __ssi_hw_loopback_mode()		\
	__raw_setl_atomic(LBM, (SSICR1(0) + ssi_nr))
#endif

void spi_hw_write_byte(uint8_t byte)
{
	while (__raw_testl_atomic(BSY, (SSISR(0) + ssi_nr)))
		;
	__ssi_hw_write_byte(byte);
}

uint8_t spi_hw_read_byte(void)
{
	while (__raw_testl_atomic(BSY, (SSISR(0) + ssi_nr)))
		;
	return __ssi_hw_read_byte();
}

#ifdef CONFIG_SPI_MASTER
void spi_hw_ctrl_start(void)
{
}
void ssi_hw_ctrl_start(void)
{
	__ssi_hw_ctrl_enable();
}
#endif

#ifdef CONFIG_SPI_SLAVE
void spi_hw_ctrl_start(void)
{
}
#endif

void spi_hw_config_mode(uint8_t mode)
{
	ssi_mode = mode;
}

void ssi_hw_config_mode(uint8_t mode)
{
	BUG_ON(spi_order(mode) == SPI_LSB);
	__ssi_hw_config_mode(mode);
	__ssi_hw_config_frame_type(__SSI_FRF_FREESCALE);
	__ssi_hw_config_size_8bit();

	/* spi loopback test */
#ifdef CONFIG_PORTING_SPI
	__ssi_hw_loopback_mode();
#endif
}

void spi_hw_deselect_chips(void)
{
	pm_hw_resume_device(DEV_GPIOA, DEV_MODE_OFF);
	pm_hw_resume_device(DEV_GPIOG, DEV_MODE_OFF);
	pm_hw_resume_device(DEV_GPIOE, DEV_MODE_OFF);
}

void ssi_config_master(uint32_t khz, uint8_t mode)
{
	ssi_hw_config_mode(mode);
	ssi_hw_config_freq(SPI_FREQ_DEF);
	ssi_hw_ctrl_start();
}

void spi_hw_chip_select(uint8_t chip)
{
	spi_t ssi_id = HIHALF(chip);
	uint8_t dev_id = LOHALF(chip);
	spi_hw_deselect_chips();
	if (ssi_id) {
		ssi_nr = 0x1000;
		__ssi1_hw_config_pins();
	} else if (!ssi_id) {
		ssi_nr = 0;
		__ssi0_hw_config_pins();

		if (dev_id == LCD_HW_SPI_CHIP) {
			gpio_hw_write_pin(GPIOG, 1, 0x00);
			gpio_hw_write_pin(GPIOG, 0, 0x01);
		} else if (dev_id == SD_HW_SPI_CHIP) {
			gpio_hw_write_pin(GPIOG, 0, 0x00);
			gpio_hw_write_pin(GPIOG, 1, 0x01);
		}
	}
	ssi_config_master(ssi_khz, ssi_mode);
}

void spi_hw_ctrl_stop(void)
{
	__ssi0_hw_ctrl_disable();
	__ssi1_hw_ctrl_disable();
}

void spi_hw_config_freq(uint32_t khz)
{
	ssi_khz = khz;
}

void ssi_hw_config_freq(uint32_t khz)
{
	/*
	 * SSICLK = SysClk / (CPSDVSR * (1 + SCR)) ->
	 * CPSDVSR * (1 + SCR) = SysClk / SSICLK ->
	 * CPSDVSR * (1 + SCR) = CLK_SYS / khz = clk
	 */
	uint16_t clk = (div32u(CLK_SYS, khz) & 0xfffe);
	uint16_t div, mod, fls;
	uint8_t cpsdvsr, scr;

	clk = min(MAX_SSI_CLK, clk);
	fls = __fls16(clk);
	div = div16u(fls, 2);
	mod = mod16u(fls, 2);

	cpsdvsr = (1 << div) + (mod ? ((1 << div) - 2) : 0);
	scr = div16u(clk, cpsdvsr) - 1;

	__ssi_hw_config_clk_prescale(cpsdvsr);
	__ssi_hw_config_clk_phase(scr);
}

void spi_hw_ctrl_init(void)
{
	pm_hw_resume_device(DEV_SSI0, DEV_MODE_ON);
	pm_hw_resume_device(DEV_SSI1, DEV_MODE_ON);
	__ssi0_hw_config_pins();
	__ssi1_hw_config_pins();
}
