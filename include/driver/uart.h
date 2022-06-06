#ifndef __UART_DRIVER_H_INCLUDE__
#define __UART_DRIVER_H_INCLUDE__

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
#ifdef CONFIG_DEBUG_PRINT
#define uart_hw_dbg_init()
#define uart_hw_dbg_start()
#define uart_hw_dbg_stop()
#define uart_hw_dbg_write(byte)
#define uart_hw_dbg_config(params, baudrate)
#endif
#ifdef CONFIG_CONSOLE
#define uart_hw_con_init()	do { } while (0)
#endif
#ifdef CONFIG_CONSOLE_OUTPUT
#define uart_hw_con_write(byte)	do { } while (0)
#endif
#ifdef CONFIG_CONSOLE_INPUT
#define uart_hw_con_read()	-1
#define uart_hw_con_poll()	false
#endif
#ifdef CONFIG_MMU
#define uart_hw_mmu_init()	do { } while (0)
#endif
#ifdef CONFIG_UART
#define uart_hw_ctrl_init()
#endif
#else /* ARCH_HAVE_UART */
#ifndef CONFIG_CONSOLE
#define uart_hw_con_init()	do { } while (0)
#endif
#ifndef CONFIG_MMU
#define uart_hw_mmu_init()	do { } while (0)
#endif
#ifndef CONFIG_CONSOLE_OUTPUT
#define uart_hw_con_write(byte)	do { } while (0)
#endif
#ifndef CONFIG_CONSOLE_INPUT
#define uart_hw_con_read()	-1
#define uart_hw_con_poll()	false
#endif
#endif

#endif /* __UART_DRIVER_H_INCLUDE__ */
