#ifndef __UART_VEXPRESSA9_H_INCLUDE__
#define __UART_VEXPRESSA9_H_INCLUDE__

#include <target/config.h>
#include <target/generic.h>
#include <asm/reg.h>
#include <asm/mach/clk.h>
#include <asm/mach/irq.h>
#include <target/muldiv.h>

#ifdef CONFIG_UART_VEXPRESSA9
#ifndef ARCH_HAVE_UART
#define ARCH_HAVE_UART		1
#else
#error "Multiple UART controller defined"
#endif
#endif

/* Versatile express A9x4 generators */
#define UART_BASE		UATX_UART0
#define UART_BLOCK_SHIFT	12

/* 24M reference clock of PL011 PrimeCell UART */
#define CLK_UART		24000

#include <asm/uart_pl01x.h>

/* Versatile express A9x4 definitions */
#ifdef CONFIG_DEBUG_PRINT
void uart_hw_dbg_init(void);
void uart_hw_dbg_start(void);
void uart_hw_dbg_stop(void);
void uart_hw_dbg_write(uint8_t byte);
void uart_hw_dbg_config(uint8_t params, uint32_t baudrate);
#endif

#endif /* __UART_VEXPRESSA9_H_INCLUDE__ */
