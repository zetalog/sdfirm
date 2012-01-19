#include <target/dataflash.h>
#include <target/delay.h>

mtd_size_t dataflash_hw_page_size(void)
{
	return DATAFLASH_HW_PAGE_SIZE;
}

void dataflash_hw_chip_init(void)
{
}
