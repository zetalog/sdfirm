#ifndef __GPIO_QDF2400_H_INCLUDE__
#define __GPIO_QDF2400_H_INCLUDE__

#define TLMM_BASE			ULL(0xFF02010000)

#include <asm/mach/tlmm.h>

/* PIN multiplexer functions, from tlmm_gpio_pinmux_table.xlsx, associated
 * PIN direction is recorded as comments.
 */
#define GPIO0_FUNC_BLSP1_I2C0_SDA			1 /* L */
#define GPIO1_FUNC_BLSP1_I2C0_SCL			1 /* L */
#define GPIO2_FUNC_BLSP1_I2C1_SDA			1 /* L */
#define GPIO3_FUNC_BLSP1_I2C1_SCL			1 /* L */
#define GPIO4_FUNC_BLSP1_I2C2_SDA			1 /* L */
#define GPIO5_FUNC_BLSP1_I2C2_SCL			1 /* L */
#define GPIO6_FUNC_BLSP1_I2C3_SDA			1 /* L */
#define GPIO7_FUNC_BLSP1_I2C3_SCL			1 /* L */
#define GPIO8_FUNC_BLSP2_I2C0_SDA			1 /* L */
#define GPIO9_FUNC_BLSP2_I2C0_SCL			1 /* L */
#define GPIO10_FUNC_BLSP2_I2C1_SDA			1 /* L */
#define GPIO11_FUNC_BLSP2_I2C1_SCL			1 /* L */
#define GPIO12_FUNC_BLSP2_I2C2_SDA			1 /* L */
#define GPIO13_FUNC_BLSP2_I2C2_SCL			1 /* L */
#define GPIO14_FUNC_BLSP2_I2C3_SDA			1 /* L */
#define GPIO15_FUNC_BLSP2_I2C3_SCL			1 /* L */

#define GPIO4_FUNC_I2C_SLV3_SDA				2 /* L */
#define GPIO5_FUNC_I2C_SLV3_SCL				2 /* L */
#define GPIO6_FUNC_I2C_SLV2_SDA				2 /* L */
#define GPIO7_FUNC_I2C_SLV2_SCL				2 /* L */
#define GPIO12_FUNC_I2C_SLV1_SDA			2 /* L */
#define GPIO13_FUNC_I2C_SLV1_SCL			2 /* L */
#define GPIO14_FUNC_I2C_SLV0_SDA			2 /* L */
#define GPIO14_FUNC_I2C_SLV0_SCL			2 /* L */

#define GPIO16_FUNC_BLSP3_AC_UART0_TX			1 /* O */
#define GPIO17_FUNC_BLSP3_AC_UART0_RX			1 /* I */
#define GPIO18_FUNC_BLSP3_AC_UART0_RFR_N		1 /* O */
#define GPIO19_FUNC_BLSP3_AC_UART0_CTS_N		1 /* I */
#define GPIO24_FUNC_BLSP3_AC_UART1_TX			1 /* O */
#define GPIO25_FUNC_BLSP3_AC_UART1_RX			1 /* I */
#define GPIO26_FUNC_BLSP3_AC_UART1_RFR_N		1 /* O */
#define GPIO27_FUNC_BLSP3_AC_UART1_CTS_N		1 /* I */
#define GPIO28_FUNC_BLSP3_AC_UART2_TX			1 /* O */
#define GPIO29_FUNC_BLSP3_AC_UART2_RX			1 /* I */
#define GPIO30_FUNC_BLSP3_AC_UART2_RFR_N		1 /* O */
#define GPIO31_FUNC_BLSP3_AC_UART2_CTS_N		1 /* I */
#define GPIO124_FUNC_BLSP3_AC_UART3_TX			1 /* O */
#define GPIO125_FUNC_BLSP3_AC_UART3_RX			1 /* I */
#define GPIO126_FUNC_BLSP3_AC_UART3_RFR_N		1 /* O */
#define GPIO127_FUNC_BLSP3_AC_UART3_CTS_N		1 /* I */

#define GPIO16_FUNC_BLSP0_Q_UART2_TX			2 /* O */
#define GPIO17_FUNC_BLSP0_Q_UART2_RX			2 /* I */
#define GPIO18_FUNC_BLSP0_Q_UART2_RFR_N			2 /* O */
#define GPIO19_FUNC_BLSP0_Q_UART2_CTS_N			2 /* I */
#define GPIO20_FUNC_BLSP0_Q_UART3_TX			1 /* O */
#define GPIO21_FUNC_BLSP0_Q_UART3_RX			1 /* I */
#define GPIO22_FUNC_BLSP0_Q_UART3_RFR_N			1 /* O */
#define GPIO23_FUNC_BLSP0_Q_UART3_CTS_N			1 /* I */
#define GPIO24_FUNC_BLSP0_Q_UART0_TX			1 /* O */
#define GPIO25_FUNC_BLSP0_Q_UART0_RX			1 /* I */
#define GPIO26_FUNC_BLSP0_Q_UART0_RFR_N			1 /* O */
#define GPIO27_FUNC_BLSP0_Q_UART0_CTS_N			1 /* I */
#define GPIO28_FUNC_BLSP0_Q_UART1_TX			1 /* O */
#define GPIO29_FUNC_BLSP0_Q_UART1_RX			1 /* I */
#define GPIO30_FUNC_BLSP0_Q_UART1_RFR_N			1 /* O */
#define GPIO31_FUNC_BLSP0_Q_UART1_CTS_N			1 /* I */

#define GPIO32_FUNC_BLSP0_SPI0_CS1_N_B			1 /* Z */
#define GPIO33_FUNC_BLSP0_SPI0_CS2_N_B			1 /* Z */
#define GPIO34_FUNC_BLSP0_SPI0_CS3_N_B			1 /* Z */
#define GPIO40_FUNC_BLSP0_SPI0_MISO			1 /* B */
#define GPIO41_FUNC_BLSP0_SPI0_MOSI			1 /* L */
#define GPIO42_FUNC_BLSP0_SPI0_CLK			1 /* L */
#define GPIO43_FUNC_BLSP0_SPI0_CS0_N			1 /* L */
#define GPIO48_FUNC_BLSP0_SPI0_CS1_N_A			2 /* Z */
#define GPIO49_FUNC_BLSP0_SPI0_CS2_N_A			2 /* Z */
#define GPIO50_FUNC_BLSP0_SPI0_CS3_N_A			2 /* Z */
#define GPIO90_FUNC_BLSP0_SPI0_CS1_N_C			1 /* Z */
#define GPIO97_FUNC_BLSP0_SPI0_CS2_N_C			1 /* Z */
#define GPIO98_FUNC_BLSP0_SPI0_CS3_N_C			1 /* Z */

#define GPIO32_FUNC_BLSP3_SPI0_MISO			2 /* B */
#define GPIO33_FUNC_BLSP3_SPI0_MOSI			2 /* L */
#define GPIO34_FUNC_BLSP3_SPI0_CLK			2 /* L */
#define GPIO89_FUNC_BLSP3_SPI0_CS0_N			1 /* L */

#define GPIO36_FUNC_BLSP3_SPI1_MISO			1 /* B */
#define GPIO37_FUNC_BLSP3_SPI1_MOSI			1 /* L */
#define GPIO38_FUNC_BLSP3_SPI1_CLK			1 /* L */
#define GPIO39_FUNC_BLSP3_SPI1_CS0_N			1 /* L */

#define GPIO44_FUNC_BLSP0_SPI1_MISO			1 /* B */
#define GPIO45_FUNC_BLSP0_SPI1_MOSI			1 /* L */
#define GPIO46_FUNC_BLSP0_SPI1_CLK			1 /* L */
#define GPIO47_FUNC_BLSP0_SPI1_CS0_N			1 /* L */
#define GPIO48_FUNC_BLSP0_SPI1_CS1_N			1 /* Z */
#define GPIO49_FUNC_BLSP0_SPI1_CS2_N			1 /* Z */
#define GPIO50_FUNC_BLSP0_SPI1_CS3_N			1 /* Z */

#define GPIO103_FUNC_SPI_SLV_SCLK			1 /* I */
#define GPIO104_FUNC_SPI_SLV_MOSI			1 /* I */
#define GPIO105_FUNC_SPI_SLV_MISO			1 /* O */
#define GPIO106_FUNC_SPI_SLV_CS_N			1 /* I */

#define GPIO51_FUNC_SDC0_CLK				1 /* L */
#define GPIO52_FUNC_SDC0_CMD				1 /* B */
#define GPIO53_FUNC_SDC0_DATA_0				1 /* B */
#define GPIO54_FUNC_SDC0_DATA_1				1 /* B */
#define GPIO55_FUNC_SDC0_DATA_2				1 /* B */
#define GPIO56_FUNC_SDC0_DATA_3				1 /* B */
#define GPIO57_FUNC_SDC0_DATA_4				1 /* B */
#define GPIO58_FUNC_SDC0_DATA_5				1 /* B */
#define GPIO59_FUNC_SDC0_DATA_6				1 /* B */
#define GPIO60_FUNC_SDC0_DATA_7				1 /* B */

#define GPIO57_FUNC_SDC2_DATA_0				2 /* B */
#define GPIO58_FUNC_SDC2_DATA_1				2 /* B */
#define GPIO59_FUNC_SDC2_DATA_2				2 /* B */
#define GPIO60_FUNC_SDC2_DATA_3				2 /* B */
#define GPIO61_FUNC_SDC2_CLK				1 /* L */
#define GPIO62_FUNC_SDC2_CMD				1 /* B */

#define GPIO63_FUNC_SDC1_CLK				1 /* L */
#define GPIO64_FUNC_SDC1_CMD				1 /* B */
#define GPIO65_FUNC_SDC1_DATA_0				1 /* B */
#define GPIO66_FUNC_SDC1_DATA_1				1 /* B */
#define GPIO67_FUNC_SDC1_DATA_2				1 /* B */
#define GPIO68_FUNC_SDC1_DATA_3				1 /* B */
#define GPIO69_FUNC_SDC1_DATA_4				1 /* B */
#define GPIO70_FUNC_SDC1_DATA_5				1 /* B */
#define GPIO71_FUNC_SDC1_DATA_6				1 /* B */
#define GPIO72_FUNC_SDC1_DATA_7				1 /* B */

#define GPIO69_FUNC_SDC3_DATA_0				2 /* B */
#define GPIO70_FUNC_SDC3_DATA_1				2 /* B */
#define GPIO71_FUNC_SDC3_DATA_2				2 /* B */
#define GPIO72_FUNC_SDC3_DATA_3				2 /* B */
#define GPIO73_FUNC_SDC3_CLK				1 /* L */
#define GPIO74_FUNC_SDC3_CMD				1 /* B */

#define GPIO52_FUNC_MDDRX_MNC_GPIO_OUT_0		2 /* O */
#define GPIO53_FUNC_MDDRX_MNC_GPIO_OUT_1		2 /* O */
#define GPIO54_FUNC_MDDRX_MNC_GPIO_OUT_2		2 /* O */
#define GPIO55_FUNC_MDDRX_MNC_GPIO_OUT_3		2 /* O */
#define GPIO56_FUNC_MDDRX_MNC_GPIO_OUT_4		2 /* O */
#define GPIO57_FUNC_MDDRX_MNC_GPIO_OUT_5		3 /* O */
#define GPIO58_FUNC_MDDRX_MNC_GPIO_OUT_6		3 /* O */
#define GPIO59_FUNC_MDDRX_MNC_GPIO_OUT_7		3 /* O */
#define GPIO60_FUNC_MDDRX_MNC_GPIO_OUT_8		3 /* O */
#define GPIO61_FUNC_MDDRX_MNC_GPIO_OUT_9		2 /* O */
#define GPIO62_FUNC_MDDRX_MNC_GPIO_OUT_10		2 /* O */
#define GPIO63_FUNC_MDDRX_MNC_GPIO_OUT_11		2 /* O */
#define GPIO64_FUNC_MDDRX_MNC_GPIO_IN_0			2 /* I */
#define GPIO65_FUNC_MDDRX_MNC_GPIO_IN_1			2 /* I */

#define GPIO75_FUNC_SGMII_MDC_0				1 /* O */
#define GPIO76_FUNC_SGMII_MDC_1				1 /* O */
#define GPIO77_FUNC_SGMII_MDIO_0			1 /* B */
#define GPIO78_FUNC_SGMII_MDIO_1			1 /* B */

#define GPIO35_FUNC_IEEE_1588_PPS_IN			1 /* I */
#define GPIO109_FUNC_IEEE_1588_PPS_OUT_0		1 /* O */
#define GPIO110_FUNC_IEEE_1588_PPS_OUT_1		1 /* O */

#define GPIO91_FUNC_FAN_PWM_0				1 /* O */
#define GPIO92_FUNC_FAN_TACH_0				1 /* I */
#define GPIO93_FUNC_FAN_PWM_1				1 /* O */
#define GPIO94_FUNC_FAN_TACH_1				1 /* I */
#define GPIO95_FUNC_FAN_PWM_2				1 /* O */
#define GPIO96_FUNC_FAN_TACH_2				1 /* I */
#define GPIO97_FUNC_FAN_PWM_3				2 /* O */
#define GPIO98_FUNC_FAN_TACH_3				2 /* I */
#define GPIO99_FUNC_FAN_PWM_4				1 /* O */
#define GPIO100_FUNC_FAN_TACH_4				1 /* I */
#define GPIO101_FUNC_FAN_PWM_5				1 /* O */
#define GPIO102_FUNC_FAN_TACH_5				1 /* I */
#define GPIO103_FUNC_FAN_PWM_6				2 /* O */
#define GPIO104_FUNC_FAN_TACH_6				2 /* I */
#define GPIO105_FUNC_FAN_PWM_7				2 /* O */
#define GPIO106_FUNC_FAN_TACH_7				2 /* I */

#define GPIO113_FUNC_OBFF_WAKE				1 /* O */

/* GPIO132-GPIO147: Software Reserved */
/* GPIO148: Reserved for PCI hot plug interrupt */
/* GPIO149: Reserved for PCI link up/down interrupt */

#define GPIO_HW_PORT			0x00
#define GPIO_HW_MAX_PINS		TLMM_NR_GPIOS

#define gpio_hw_read_pin(port, pin)		\
	tlmm_read_input(pin)
#define gpio_hw_write_pin(port, pin, val)	\
	tlmm_write_output(pin, val)
#define gpio_hw_config_mux(port, pin, mux)	\
	tlmm_config_func(pin, mux)
#define gpio_hw_config_pad(port, pin, pad, drv)	\
	tlmm_config_pad(pin, pad, drv)
#define gpio_hw_config_irq(port, pin, mode)	\
	tlmm_config_irq(pin, mode)
#define gpio_hw_enable_irq(port, pin)		\
	tlmm_enable_irq(pin)
#define gpio_hw_disable_irq(port, pin)		\
	tlmm_disable_irq(pin)
#define gpio_hw_clear_irq(port, pin)		\
	tlmm_clear_irq(pin)
#define gpio_hw_trigger_irq(port, pin)		\
	tlmm_trigger_irq(pin)
#define gpio_hw_irq_status(port, pin)		\
	tlmm_irq_status(pin)
#define gpio_hw_ctrl_init()

#endif /* __GPIO_QDF2400_H_INCLUDE__ */
