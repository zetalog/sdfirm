#include <target/console.h>
#include <target/paging.h>

void k1max_k1x_uart_init(void)
{
	#if 0
	uint32_t data;

	data = __raw_readl(0xD4090104); // pll
	data |= 0x1f << 3;
	__raw_writel(data, 0xD4090104);

	data = __raw_readl(REG_PMUM_ACGR);
	data |= (1<<1) | (1<<8);
	__raw_writel(data, REG_PMUM_ACGR);

	__raw_writel(1, 0xD4051050ul); // apb clock: 52MHz

	__raw_writel(7, APB_BUSCLK_BASE+0x8); // gpio clock rst
	__raw_writel(3, APB_BUSCLK_BASE+0x8);

	__raw_writel(7, APB_BUSCLK_BASE+0x3c); // aib clk rst
	__raw_writel(3, APB_BUSCLK_BASE+0x3c);

	data = __raw_readl(MFPR_BASE+0x290); // uart txd
	data &= ~7;
	data |= 1;
	__raw_writel(data, MFPR_BASE+0x290);

	data = __raw_readl(MFPR_BASE+0x294); // uart rxd
	data &= ~7;
	data |= 1;
	__raw_writel(data, MFPR_BASE+0x294);
	#endif

}
