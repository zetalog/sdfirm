#ifndef __UART_MACH_H_INCLUDE__
#define __UART_MACH_H_INCLUDE__

#include <target/config.h>
#include <target/generic.h>

#ifndef ARCH_HAVE_UART
#define ARCH_HAVE_UART		1
#else
#error "Multiple UART controller defined"
#endif

void uart_hw_ctrl_init(void);
void uart_hw_ctrl_start(void);
void uart_hw_ctrl_stop(void);
void uart_hw_set_params(uint8_t params, uint32_t baudrate);
void uart_hw_write_byte(uint8_t byte);
uint8_t uart_hw_read_byte(void);

#endif /* __UART_MACH_H_INCLUDE__ */
