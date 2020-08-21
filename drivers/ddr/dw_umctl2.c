#include <target/ddr.h>

void dw_umctl2_start(void)
{
	clk_enable(ddr_arst);

	/* TODO: configure dw_umctl2 after seeting ddr_clk/ddr_rst */
}

void dw_umctl2_init(void)
{
	clk_enable(ddr_por);
	clk_enable(ddr_prst);

	/* TODO: configure dw_umctl2 before seeting ddr_clk/ddr_rst */
}
