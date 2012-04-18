#ifndef __UART_H_INCLUDE__
#define __UART_H_INCLUDE__

#include <stdio.h>

typedef int uart_t;

uart_t uart_open(int id);
void uart_close(uart_t uart);
int uart_config(uart_t uart, int baudr);
int uart_write(uart_t uart, void *buf, size_t len);
int uart_read(uart_t uart, void *buf, size_t len);

#endif /* __UART_H_INCLUDE__ */
