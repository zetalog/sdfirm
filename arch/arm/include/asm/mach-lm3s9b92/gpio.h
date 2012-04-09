#ifndef __GPIO_LM3S9B92_H_INCLUDE__
#define __GPIO_LM3S9B92_H_INCLUDE__

#include <target/config.h>
#include <target/generic.h>
#include <asm/reg.h>
#include <asm/mach/pm.h>

#ifdef CONFIG_GPIO_LM3S9B92
#ifndef ARCH_HAVE_GPIO
#define ARCH_HAVE_GPIO		1
#else
#error "Multiple GPIO controller defined"
#endif
#endif

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
#define GPIOICR			(0x41C)
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

void gpio_hw_config_mux(uint8_t port, uint8_t pin, uint8_t mux);
void gpio_hw_config_pad(uint8_t port, uint8_t pin,
			uint8_t pad, uint8_t drv);
uint8_t gpio_hw_read_pin(uint8_t port, uint8_t pin);
void gpio_hw_write_pin(uint8_t port, uint8_t pin, uint8_t val);
uint8_t gpio_hw_read_port(uint8_t port, uint8_t mask);
void gpio_hw_write_port(uint8_t port, uint8_t mask,
			uint8_t val);
void gpio_hw_config_irq(uint8_t port, uint8_t pin, uint8_t mode);
void gpio_hw_enable_irq(uint8_t port, uint8_t pin);
void gpio_hw_disable_irq(uint8_t port, uint8_t pin);
uint8_t gpio_hw_irq_status(uint8_t port);
void gpio_hw_clear_irq(uint8_t port, uint8_t pin);

void gpio_hw_ctrl_init(void);
#ifdef CONFIG_PORTING_GPIO
void gpio_hw_porting_init(void);
#else
#define gpio_hw_porting_init()
#endif

#endif /* __GPIO_LM3S9B92_H_INCLUDE__ */
