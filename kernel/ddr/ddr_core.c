#include <target/ddr.h>

#ifdef CONFIG_DDR_SPD_3200
#define DDR_SPD_DEFAULT		DDR4_3200
#endif
#ifdef CONFIG_DDR_SPD_2933
#define DDR_SPD_DEFAULT		DDR4_2933
#endif
#ifdef CONFIG_DDR_SPD_2666
#define DDR_SPD_DEFAULT		DDR4_2666
#endif
#ifdef CONFIG_DDR_SPD_2400
#define DDR_SPD_DEFAULT		DDR4_2400
#endif
#ifdef CONFIG_DDR_SPD_2133
#define DDR_SPD_DEFAULT		DDR4_2133
#endif
#ifdef CONFIG_DDR_SPD_1866
#define DDR_SPD_DEFAULT		DDR4_1866
#endif
#ifdef CONFIG_DDR_SPD_1600
#define DDR_SPD_DEFAULT		DDR4_1600
#endif
#ifdef CONFIG_DDR_SPD_1333
#define DDR_SPD_DEFAULT		DDR3_1333
#endif
#ifdef CONFIG_DDR_SPD_1066
#define DDR_SPD_DEFAULT		DDR3_1066
#endif
#ifdef CONFIG_DDR_SPD_800
#define DDR_SPD_DEFAULT		DDR3_800
#endif
#ifdef CONFIG_DDR_SPD_667
#define DDR_SPD_DEFAULT		DDR2_667
#endif
#ifdef CONFIG_DDR_SPD_533
#define DDR_SPD_DEFAULT		DDR2_533
#endif
#ifdef CONFIG_DDR_SPD_400
#define DDR_SPD_DEFAULT		DDR2_400
#endif
#ifdef CONFIG_DDR_SPD_266
#define DDR_SPD_DEFAULT		DDR_266
#endif
#ifdef CONFIG_DDR_SPD_200
#define DDR_SPD_DEFAULT		DDR_200
#endif
uint8_t ddr_spd = DDR_SPD_DEFAULT;
uint8_t ddr_dev = 0;

uint16_t ddr_spd_speeds[DDR_MAX_SPDS] = {
	[DDR_200] = 200,
	[DDR_266] = 266,
	[DDR_333] = 333,
	[DDR_400] = 400, /* DDR2_400 */
	[DDR2_533] = 533,
	[DDR2_667] = 667,
	[DDR2_800] = 800, /* DDR3_800 */
	[DDR3_1066] = 1066,
	[DDR3_1333] = 1333,
	[DDR3_1600] = 1600, /* DDR4_1600 */
	[DDR4_1866] = 1866,
	[DDR4_2133] = 2133,
	[DDR4_2400] = 2400,
	[DDR4_2666] = 2666,
	[DDR4_2933] = 2933,
	[DDR4_3200] = 3200,
};

uint16_t ddr_spd2speed(uint8_t spd)
{
	if (spd >= DDR_MAX_SPDS)
		spd = ddr_spd;
	return ddr_spd_speeds[spd];
}

void ddr_config_speed(uint8_t spd)
{
	ddr_hw_config_speed(spd);
	ddr4_config_speed(ddr_dev, ddr_spd);
}

void ddr_init(void)
{
	/* TODO: SPD initialization */
	ddr_config_speed(ddr_spd);
	ddr_enable_speed(ddr_spd);

	ddr_hw_ctrl_init();
	ddr_hw_ctrl_start();
}
