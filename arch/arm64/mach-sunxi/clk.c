#include <target/clk.h>

void clk_hw_ctrl_init(void)
{
#ifdef CONFIG_SUNXI_SPL
	clock_init_safe();
#endif
	clock_init_uart();
	clock_init_sec();
}
