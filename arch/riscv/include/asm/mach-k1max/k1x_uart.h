#ifndef __UART_K1X_K1MAX_H_INCLUDE__
#define __UART_K1X_K1MAX_H_INCLUDE__

#ifdef CONFIG_K1M_K1X_UART
void k1max_k1x_uart_init(void);
#else
#define k1max_k1x_uart_init() do { } while (0)
#endif

#endif
