#ifndef __UART_AT90USB1287_H_INCLUDE__
#define __UART_AT90USB1287_H_INCLUDE__

#include <target/config.h>
#include <target/generic.h>
#include <asm/reg.h>
#include <asm/mach/irq.h>
#include <asm/mach/clk.h>
#include <asm/mach/usart.h>

#ifndef ARCH_HAVE_UART
#define ARCH_HAVE_UART		1
#else
#error "Multiple UART controller defined"
#endif

#define UCSR1A  _SFR_MEM8(0xC8)
#define RXC1	7
#define TXC1	6
#define UDRE1	5
#define FE1	4
#define DOR1	3
#define UPE1 	2
#define U2X1	1
#define MPCM1	0

#define UCSR1B  _SFR_MEM8(0XC9)
#define RXCIE1	7
#define TXCIE1  6
#define UDRIE1  5
#define RXEN1	4
#define TXEN1   3
#define UCSZ12  2
#define RXB81   1
#define TXB81   0

#define UCSR1C  _SFR_MEM8(0xCA)
#define UMSEL11 7
#define UMSEL10 6
#define UPM11	5
#define UPM10	4
#define USBS1	3
#define UCSZ11  2
#define UCSZ10  1
#define UCPOL1  0

/* Baudrate */
#define UBRR1   _SFR_MEM16(0xCC)
#define UBRR1L  _SFR_MEM8(0xCC)
#define UBRR1H  _SFR_MEM8(0xCD)
/* USART Data */
#define UDR1    _SFR_MEM8(0XCE)

#define uart_hw_ctrl_init()
void uart_hw_ctrl_start(void);
void uart_hw_ctrl_stop(void);
void uart_hw_set_params(uint8_t params, uint32_t baudrate);
void uart_hw_write_byte(uint8_t byte);
uint8_t uart_hw_read_byte(void);

#endif /* __UART_AT90USB1287_H_INCLUDE__ */
