#ifndef __UART_DRIVER_H_INCLUDE__
#define __UART_DRIVER_H_INCLUDE__

#include <target/config.h>

#ifdef CONFIG_ARCH_HAS_UART
#include <asm/mach/uart.h>
#endif

#ifndef ARCH_HAVE_UART
#define uart_hw_ctrl_init()
#define uart_hw_ctrl_start()
#define uart_hw_ctrl_stop()
#define uart_hw_set_params(params, baudrate)
#define uart_hw_write_byte(byte)
#define uart_hw_read_byte()		(0)
#endif

#endif /* __UART_DRIVER_H_INCLUDE__ */
