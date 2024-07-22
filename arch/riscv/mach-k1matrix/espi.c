#include <target/espi.h>

void k1matrix_espi_init(void)
{
	clk_enable(rmu_espi_clk);
	spacemit_espi_init();
}
