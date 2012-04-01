#ifndef __UART_MACH_H_INCLUDE__
#define __UART_MACH_H_INCLUDE__

#include <target/config.h>
#include <target/generic.h>

#ifndef ARCH_HAVE_UART
#define ARCH_HAVE_UART		1
#else
#error "Multiple UART controller defined"
#endif

void uart_hw_dbg_init(void);
void uart_hw_dbg_start(void);
void uart_hw_dbg_stop(void);
void uart_hw_dbg_write(uint8_t byte);
void uart_hw_dbg_config(uint8_t params, uint32_t baudrate);

#endif /* __UART_MACH_H_INCLUDE__ */
