#include <target/lpc.h>

void k1matrix_lpc_init(void)
{
	clk_enable(rmu_lpc_clk);
	clk_enable(rmu_lpc_lclk);
	spacemit_lpc_init();
}
