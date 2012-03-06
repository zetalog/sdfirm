#include <target/uart.h>

#ifdef CONFIG_UART_SYNC
void uart_sync_init(void);
#else
#define uart_sync_init()
#endif

#ifdef CONFIG_UART_ASYNC
void uart_async_init(void);
#else
#define uart_async_init()
#endif

void uart_init(void)
{
	uart_sync_init();
	uart_async_init();
}
