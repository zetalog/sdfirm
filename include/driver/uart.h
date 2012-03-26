#ifndef __UART_DRIVER_H_INCLUDE__
#define __UART_DRIVER_H_INCLUDE__

#include <target/config.h>
#include <target/bulk.h>

#ifdef CONFIG_ARCH_HAS_UART
#include <asm/mach/uart.h>
#endif

struct uart_port {
	void (*startup)(void);
	void (*cleanup)(void);
	void (*config)(uint8_t params, uint32_t baudrate);
	io_cb rxstart;	/* before the transfer */
	io_cb rxstop;	/* after the transfer  */
	iordwr_cb rxxmit;
	iotest_cb rxpoll;
	bulk_channel_t *tx;
	bulk_channel_t *rx;
};
__TEXT_TYPE__(const struct uart_port, uart_port_t);

#ifndef ARCH_HAVE_UART
#define uart_hw_sync_init()
#define uart_hw_sync_start()
#define uart_hw_sync_stop()
#define uart_hw_sync_write(byte)
#define uart_hw_sync_read()		(0)
#define uart_hw_sync_config(params, baudrate)

#define uart_hw_async_init()
#endif

#endif /* __UART_DRIVER_H_INCLUDE__ */
