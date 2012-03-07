#ifndef __UART_DRIVER_H_INCLUDE__
#define __UART_DRIVER_H_INCLUDE__

#include <target/config.h>

#ifdef CONFIG_ARCH_HAS_UART
#include <asm/mach/uart.h>
#endif

#ifndef ARCH_HAVE_UART
#define uart_hw_sync_init()
#define uart_hw_sync_start()
#define uart_hw_sync_stop()

#define uart_hw_write_byte(byte)
#define uart_hw_read_byte()		(0)

#define uart_hw_set_params(params, baudrate)

#define uart_hw_async_init()
#endif

#endif /* __UART_DRIVER_H_INCLUDE__ */
