#ifndef __UART_GEM5_H_INCLUDE__
#define __UART_GEM5_H_INCLUDE__

#include <target/config.h>
#include <target/generic.h>
#include <target/paging.h>
#include <target/gpio.h>
#include <target/clk.h>

#ifdef CONFIG_ARM_DCC
#include <asm/debug.h>
#else /* CONFIG_ARM_DCC */
#define UART0_BASE			0x1C090000

/* Definitions to implement AMBA UART */
#define __UART_BASE(n)			(UART0_BASE + ((n) * 0x10000))
#define UART_HW_MAX_PORTS		4

#ifdef CONFIG_MMU
#define UART_BASE(n)			uart_hw_reg_base[n]
extern caddr_t uart_hw_reg_base[UART_HW_MAX_PORTS];
#else
#define UART_BASE(n)			__UART_BASE(n)
#endif
#define UART_IRQ(n)			IRQ_UART##n
#define UART_CON_IRQ			IRQ_UART0
#define UART_CON_ID			0

#if defined(CONFIG_UART_GEM5)
#include <asm/uart_pl01x.h>
#ifndef ARCH_HAVE_UART
#define ARCH_HAVE_UART		1
#else
#error "Multiple UART controller defined"
#endif
#endif

/* TODO: not sure if this is correct */
#define UART_CLK_SRC_FREQ		UL(20000000)

#ifdef CONFIG_DEBUG_PRINT
void uart_hw_dbg_init(void);
void uart_hw_dbg_start(void);
void uart_hw_dbg_stop(void);
void uart_hw_dbg_write(uint8_t byte);
void uart_hw_dbg_config(uint8_t params, uint32_t baudrate);
#endif

#ifdef CONFIG_MMU
void uart_hw_mmu_init(void);
#endif

#ifdef CONFIG_CONSOLE
void uart_hw_con_init(void);
#endif
#ifdef CONFIG_CONSOLE_OUTPUT
void uart_hw_con_write(uint8_t byte);
#endif
#ifdef CONFIG_CONSOLE_INPUT
uint8_t uart_hw_con_read(void);
bool uart_hw_con_poll(void);
void uart_hw_irq_ack(void);
void uart_hw_irq_init(void);
#endif
#endif /* CONFIG_ARM_DCC */

#endif /* __UART_GEM5_H_INCLUDE__ */
