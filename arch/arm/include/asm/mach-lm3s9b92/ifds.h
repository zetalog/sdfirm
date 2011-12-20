#ifndef __IFDS_LM3S9B92_H_INCLUDE__
#define __IFDS_LM3S9B92_H_INCLUDE__

#include <target/config.h>
#include <target/generic.h>
#include <asm/reg.h>
#include <asm/mach/clk.h>
#include <asm/mach/pm.h>
#include <asm/mach/gpio.h>
#include <asm/mach/uart.h>

#ifndef ARCH_HAVE_IFDS
#define ARCH_HAVE_IFDS		1
#else
#error "Multiple serial IFD defined"
#endif

LM3S9B92_UART_SMART(1)

/* Port B */
#define PORT_IO			GPIOB
#define PIN_IO			4
#define PORT_CLK		GPIOB
#define PIN_CLK			5
/* Port E */
#define PORT_RST		GPIOE
#define PIN_RST			7
/* Port F */
#define PORT_VCC		GPIOF
#define PIN_VCC			4
#define PORT_PRES		GPIOF
#define PIN_PRES		5

#define PORT_VCCSEL		GPIOF
#define PIN_VCCSEL1		0
#define PIN_VCCSEL2		1

/* timings */
#define IFDS_HW_TIME_ACTIVATE	4160
#define IFDS_HW_TIM_DEACTIVATEE	250
#define IFDS_HW_TIME_DEBOUNSE	6400

void ifds_hw_ctrl_init(void);
#define ifds_hw_ctrl_stop()			__uart1_hw_smart_stop()
#define ifds_hw_ctrl_start()			__uart1_hw_smart_start()
#define ifds_hw_write_byte(byte)		__uart1_hw_write_byte(byte)
#define ifds_hw_read_byte()			__uart1_hw_smart_read()
#define ifds_hw_parity_error()			__uart1_hw_status_error()
#define ifds_hw_read_empty()			__uart1_hw_smart_empty()
#define ifds_hw_config_baudrate(br)		__uart1_hw_config_baudrate(br)
#define ifds_hw_write_pin(port, pin, val)	gpio_hw_write_pin(port, pin, val)
#define ifds_hw_read_pin(port, pin)		gpio_hw_read_pin(port, pin)

#endif /* __IFDS_LM3S9B92_H_INCLUDE__ */
