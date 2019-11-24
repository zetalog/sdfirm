#include <target/generic.h>
#include <target/uart.h>
#include <target/gpio.h>

void sunxi_uart_gpio_init(void)
{
	sunxi_gpio_set_cfgpin(SUNXI_GPA(4), SUN8I_H3_GPA_UART0);
	sunxi_gpio_set_cfgpin(SUNXI_GPA(5), SUN8I_H3_GPA_UART0);
	sunxi_gpio_set_pull(SUNXI_GPA(5), SUNXI_GPIO_PULL_UP);
}

void uart_hw_irq_init(void)
{
}

void uart_hw_irq_ack(void)
{
}

#ifdef CONFIG_MMU
void uart_hw_mmu_init(void)
{
}
#endif
