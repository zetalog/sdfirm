#ifndef __UART_AT8XC5122_H_INCLUDE__
#define __UART_AT8XC5122_H_INCLUDE__

#include <target/config.h>
#include <target/generic.h>

#include <asm/reg.h>
#include <asm/mach/irq.h>
#include <asm/mach/pm.h>
#include <asm/mach/clk.h>

#ifndef ARCH_HAVE_UART
#define ARCH_HAVE_UART			1
#else
#error "Multiple UART controller defined"
#endif

Sbit(TXD,	0xB0, 1);	/* UART Transmit Port */
Sbit(RXD,	0xB0, 0);	/* UART Receive Port */

Sfr(SCON,	0x98);		/* UART Interface Control */

Sfr(SBUF,	0x99);		/* UART Data Buffer */
Sfr(SADDR,	0xA9);		/* Slave Address */
Sfr(SADEN,	0xB9);		/* Slave Address Mask */

Sfr(BRL,	0x9A);		/* Baud Rate Relaod */
Sfr(BDRCON,	0x9B);		/* Baud Rate Control*/

/*---------------------------UART SFRs MASK-----------------------------*/
#define MSK_SCON_SM0		0x80	/* SCON */
#define MSK_SCON_FE		0x80
#define MSK_SCON_SM1		0x40
#define MSK_SCON_SM2		0x20
#define MSK_SCON_REN		0x10
#define MSK_SCON_TB8		0x08
#define MSK_SCON_RB8		0x04
#define UART_SCON_TI		0x02
#define UART_SCON_RI		0x01

/*---------------------BAUD RATE GENERATOR SFRs MASK--------------------*/
#define MSK_BDRCON_BRR		0x10	/* BDRCON */
#define MSK_BDRCON_TBCK		0x08
#define MSK_BDRCON_RBCK		0x04
#define MSK_BDRCON_SPD		0x02
#define MSK_BDRCON_M0SRC	0x01

void uart_hw_sync_init(void);
void uart_hw_sync_start(void);
void uart_hw_sync_stop(void);

void uart_hw_async_init(void);

void uart_hw_write_byte(uint8_t byte);
uint8_t uart_hw_read_byte(void);
void uart_hw_set_params(uint8_t params, uint32_t baudrate);

#endif /* __UART_AT8XC5122_H_INCLUDE__ */
