#include <target/ddr.h>

uint8_t ddr_spd = DDR4_3200;

void ddr_init(void)
{
	/* TODO: SPD initialization */
	ddr_config_speed(ddr_spd);
	ddr_enable_speed(ddr_spd);

	ddr_hw_ctrl_init();
	ddr_hw_ctrl_start();
}
