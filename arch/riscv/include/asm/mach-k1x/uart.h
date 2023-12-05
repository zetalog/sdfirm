#ifndef __UART_K1X_K1MAX_H_INCLUDE__
#define __UART_K1X_K1MAX_H_INCLUDE__

#define UART_CON_ID		0

#ifdef CONFIG_NS16550
#define NS16550_REG_SIZE	(-4)
#define NS16550_CLK		APB_CLK_FREQ
#include <driver/ns16550.h>
#endif

#ifdef CONFIG_K1M_K1X_UART
void k1max_k1x_uart_init(void);
#else
#define k1max_k1x_uart_init() do { } while (0)
#endif

#endif
