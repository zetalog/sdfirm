#ifndef __UART_AT90SCR100_H_INCLUDE__
#define __UART_AT90SCR100_H_INCLUDE__

#include <target/config.h>
#include <target/generic.h>
#include <asm/reg.h>
#include <asm/mach/irq.h>
#include <asm/mach/clk.h>
#include <asm/mach/usart.h>

#ifdef CONFIG_UART_AT90SCR100
#ifndef ARCH_HAVE_UART
#define ARCH_HAVE_UART		1
#else
#error "Multiple UART controller defined"
#endif
#endif

#define UCSR0A  _SFR_MEM8(0xC0)
#define RXC0	7
#define TXC0	6
#define UDRE0	5
#define FE0	4
#define DOR0	3
#define UPE0 	2
#define U2X0	1
#define MPCM0	0

#define UCSR0B  _SFR_MEM8(0XC1)
#define RXCIE0	7
#define TXCIE0  6
#define UDRIE0  5
#define RXEN0	4
#define TXEN0   3
#define UCSZ02  2
#define RXB80   1
#define TXB80   0

#define UCSR0C  _SFR_MEM8(0xC2)
#define UMSEL01 7
#define UMSEL00 6
#define UPM01	5
#define UPM00	4
#define USBS0	3
#define UCSZ01  2
#define UCSZ00  1
#define UCPOL0  0

/* Baudrate */
#define UBRR0   _SFR_MEM16(0xC4)
#define UBRR0L  _SFR_MEM8(0xC4)
#define UBRR0H  _SFR_MEM8(0xC5)
/* USART Data */
#define UDR0    _SFR_MEM8(0XC6)

#define uart_hw_ctrl_init()
void uart_hw_ctrl_start(void);
void uart_hw_ctrl_stop(void);
void uart_hw_set_params(uint8_t params, uint32_t baudrate);
void uart_hw_write_byte(uint8_t byte);
uint8_t uart_hw_read_byte(void);

#endif /* __UART_AT90SCR100_H_INCLUDE__ */
