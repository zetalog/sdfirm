#include <target/generic.h>
#include <target/uart.h>
#include <target/paging.h>

#ifdef CONFIG_UART_ACE
#ifdef CONFIG_CONSOLE_OUTPUT
void uart_hw_con_write(uint8_t byte)
{
	pl01x_write_byte(byte);
}
#endif

#ifdef CONFIG_CONSOLE_INPUT
bool uart_hw_con_poll(void)
{
	return pl01x_read_poll();
}

uint8_t uart_hw_con_read(void)
{
	return pl01x_read_byte();
}
#endif

void uart_hw_irq_init(void)
{
	pl01x_irq_init();
}

void uart_hw_irq_ack(void)
{
	pl01x_irq_ack();
}

#ifdef CONFIG_CONSOLE
void uart_hw_con_init(void)
{
	pl01x_config_baudrate(UART_CON_ID, UART_CLK_SRC_FREQ, UART_CON_BAUDRATE);
	pl01x_con_init();
}
#endif

#ifdef CONFIG_MMU
caddr_t uart_hw_reg_base[UART_HW_MAX_PORTS] = {
	__UART_BASE(0),
	__UART_BASE(1),
	__UART_BASE(2),
	__UART_BASE(3),
};

void uart_hw_mmu_init(void)
{
	set_fixmap_io(FIX_UART0, __UART_BASE(0) & PAGE_MASK);
	uart_hw_reg_base[0] = fix_to_virt(FIX_UART0);
	printf("FIXMAP: %016llx -> %016llx: UART0\n",
	       (uint64_t)__UART_BASE(0), fix_to_virt(FIX_UART0));
	uart_hw_con_init();
}
#endif
#endif /* CONFIG_UART_ACE */

#ifdef CONFIG_UART_APB
#ifdef CONFIG_CONSOLE_OUTPUT
void uart_hw_con_write(uint8_t byte)
{
	dw_uart_con_write(byte);
}
#endif

#ifdef CONFIG_CONSOLE_INPUT
bool uart_hw_con_poll(void)
{
	return dw_uart_con_poll();
}

uint8_t uart_hw_con_read(void)
{
	return dw_uart_con_read();
}
#endif

#ifdef CONFIG_CONSOLE
void uart_hw_con_init(void)
{
	dw_uart_ctrl_init(UART_CLK_SRC_FREQ);
}
#endif
#endif /* CONFIG_UART_APB */
