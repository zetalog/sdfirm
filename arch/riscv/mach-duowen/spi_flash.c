#include <target/arch.h>

void duowen_flash_set_frequency(uint32_t freq)
{
	__raw_writel(div32u(SYSFAB_HALF_CLK_FREQ + freq - 1, freq), SPI_DIVIDER);
}
