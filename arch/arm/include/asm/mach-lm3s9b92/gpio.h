#ifndef __GPIO_LM3S9B92_H_INCLUDE__
#define __GPIO_LM3S9B92_H_INCLUDE__

#include <target/config.h>
#include <target/generic.h>
#include <asm/reg.h>
#include <asm/mach/pm.h>

#ifdef CONFIG_GPIO_LM3S9B92_AHB
#define GPIOA_BASE		0x40058000
#define GPIOB_BASE		0x40059000
#define GPIOC_BASE		0x4005A000
#define GPIOD_BASE		0x4005B000
#define GPIOE_BASE		0x4005C000
#define GPIOF_BASE		0x4005D000
#define GPIOG_BASE		0x4005E000
#define GPIOH_BASE		0x4005F000
#define GPIOJ_BASE		0x40060000
#else
#define GPIOA_BASE		0x40004000
#define GPIOB_BASE		0x40005000
#define GPIOC_BASE		0x40006000
#define GPIOD_BASE		0x40007000
#define GPIOE_BASE		0x40024000
#define GPIOF_BASE		0x40025000
#define GPIOG_BASE		0x40026000
#define GPIOH_BASE		0x40027000
#define GPIOJ_BASE		0x4003D000
#endif

#define GPIOHBCTL		SYSTEM(0x06C)

/* GPIO registers' offset */
#define GPIODATA		(0x000)
#define GPIODIR			(0x400)
#define GPIOIS			(0x404)
#define GPIOIBE			(0x408)
#define GPIOIEV			(0x40C)
#define GPIOIM			(0x410)
#define GPIORIS			(0x414)
#define GPIOMIS			(0x418)
#define GPIOAFSEL		(0x420)
#define GPIODR2R		(0x500)
#define GPIODR4R		(0x504)
#define GPIODR8R		(0x508)
#define GPIOODR			(0x50C)
#define GPIOPUR			(0x510)
#define GPIOPDR			(0x514)
#define GPIOSLR			(0x518)
#define GPIODEN			(0x51C)
#define GPIOLOCK		(0x520)
#define GPIOCR			(0x524)
#define GPIOAMSEL		(0x528)
#define GPIOPCTL		(0x52C)
#define GPIOPERIPHID4		(0xFD0)
#define GPIOPERIPHID5		(0xFD4)
#define GPIOPERIPHID6		(0xFD8)
#define GPIOPERIPHID7		(0xFDC)
#define GPIOPERIPHID0		(0xFE0)
#define GPIOPERIPHID1		(0xFE4)
#define GPIOPERIPHID2		(0xFE8)
#define GPIOPERIPHID3		(0xFEC)

#define GPIOREG_N(port, name)	GPIO##port##name
#define GPIOREG_O(port, offset)	(GPIO##port##_BASE+offset)

/*=========================================================================
 * GPIO port numbers
 *=======================================================================*/
#define GPIOA		0
#define GPIOB		1
#define GPIOC		2
#define GPIOD		3
#define GPIOE		4
#define GPIOF		5
#define GPIOG		6
#define GPIOH		7
#define GPIOJ		8

/*=========================================================================
 * GPIO mux configuration
 *=======================================================================*/
#define GPIO_MUX_NONE		0

/* GPIO PIN A0 */
#define GPIOA0_MUX_U0RX		0x01
#define GPIOA0_MUX_I2C1SCL	0x08

/* GPIO PIN A1 */
#define GPIOA1_MUX_U0TX		0x01
#define GPIOA1_MUX_I2C1SDA	0x08

/* GPIO PIN A2 */
#define GPIOA2_MUX_SSI0CLK	0x01

/* GPIO PIN A3 */
#define GPIOA3_MUX_SSI0FSS	0x01

/* GPIO PIN A4 */
#define GPIOA4_MUX_SSI0RX	0x01

/* GPIO PIN A5 */
#define GPIOA5_MUX_SSI0TX	0x01

/* GPIO PIN B4 */
#define GPIOB4_MUX_U1RX		0x05

/* GPIO PIN B5 */
#define GPIOB5_MUX_U1TX		0x05

/* GPIO PIN E0 */
#define GPIOE0_MUX_SSI1CLK	0x02

/* GPIO PIN E1 */
#define GPIOE1_MUX_SSI1FSS	0x02

/* GPIO PIN E2 */
#define GPIOE2_MUX_SSI1RX	0x02

/* GPIO PIN E3 */
#define GPIOE3_MUX_SSI1TX	0x02

/*=========================================================================
 * GPIO pad configuration
 *=======================================================================*/
#define GPIO_DIR_IN		0x00  /* GPIO input */
#define GPIO_DIR_OUT		0x01  /* GPIO output */
#define GPIO_DIR_HW		0x02  /* peripheral function */

#define GPIO_DRIVE_2MA		0x01
#define GPIO_DRIVE_4MA		0x02
#define GPIO_DRIVE_8MA		0x04
#define GPIO_DRIVE_SLEW_RATE	0x10 /* can only be used with 8MA drive */

#define GPIO_PAD_ANALOG_IO	0x00 /* only valid for port D and E */
#define GPIO_PAD_DIGITAL_IO	0x08
#define GPIO_PAD_PULL_DOWN	0x04
#define GPIO_PAD_PULL_UP	0x02
#define GPIO_PAD_PUSH_PULL	0x00
#define GPIO_PAD_OPEN_DRAIN	0x01

#define GPIO_PAD_PP		(GPIO_PAD_DIGITAL_IO | GPIO_PAD_PUSH_PULL)
#define GPIO_PAD_OD		(GPIO_PAD_DIGITAL_IO | GPIO_PAD_OPEN_DRAIN)
/* push pull with weak pull up */
#define GPIO_PAD_PP_WU		(GPIO_PAD_PP | GPIO_PAD_PULL_UP)
/* push pull with weak pull down */
#define GPIO_PAD_PP_WD		(GPIO_PAD_PP | GPIO_PAD_PULL_DOWN)
/* open drain with weak pull up */
#define GPIO_PAD_OD_WU		(GPIO_PAD_OD | GPIO_PAD_PULL_UP)
/* open drain with weak pull down */
#define GPIO_PAD_OD_WD		(GPIO_PAD_OD | GPIO_PAD_PULL_DOWN)

void gpio_hw_config_mux(uint8_t port, uint8_t pin, uint8_t mux);
void gpio_hw_config_pad(uint8_t port, uint8_t pin, uint8_t dir,
			uint8_t pad, uint8_t drv);
unsigned long gpio_hw_read_pin(uint8_t port, uint8_t pin);
void gpio_hw_write_pin(uint8_t port, uint8_t pin, unsigned long val);

void gpio_hw_ctrl_init(void);

#endif /* __GPIO_LM3S9B92_H_INCLUDE__ */
