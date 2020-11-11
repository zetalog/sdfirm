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
#define DDR_SPD_DEFAULT		DDR4_1333
#endif
#ifdef CONFIG_DDR_SPD_1066
#define DDR_SPD_DEFAULT		DDR4_1066
#endif
#ifdef CONFIG_DDR_SPD_800
#define DDR_SPD_DEFAULT		DDR4_800
#endif
#ifdef CONFIG_DDR_SPD_667
#define DDR_SPD_DEFAULT		DDR4_667
#endif
#ifdef CONFIG_DDR_SPD_533
#define DDR_SPD_DEFAULT		DDR4_533
#endif
#ifdef CONFIG_DDR_SPD_400
#define DDR_SPD_DEFAULT		DDR4_400
#endif
#ifdef CONFIG_DDR_SPD_266
#define DDR_SPD_DEFAULT		DDR4_266
#endif
#ifdef CONFIG_DDR_SPD_200
#define DDR_SPD_DEFAULT		DDR4_200
#endif
uint8_t ddr_spd = DDR_SPD_DEFAULT;

void ddr_init(void)
{
	/* TODO: SPD initialization */
	ddr_config_speed(ddr_spd);
	ddr_enable_speed(ddr_spd);

	ddr_hw_ctrl_init();
	ddr_hw_ctrl_start();
}
