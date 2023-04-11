#include <target/ddr.h>
#include <target/jiffies.h>

#ifdef CONFIG_DPU_PLL
struct ddr_speed {
	uint64_t f_pll_vco;
	uint32_t f_pll_pclk;
	uint32_t f_pll_rclk;
};

struct ddr_speed ddr_speeds[NR_DDR_SPEEDS] = {
	[DDR4_3200] = {
		.f_pll_vco = ULL(3200000000),
		.f_pll_pclk = UL(800000000),
		.f_pll_rclk = INVALID_FREQ,
	},
	[DDR4_2933] = {
		.f_pll_vco = ULL(2933333333),
		.f_pll_pclk = UL(733333333),
		.f_pll_rclk = INVALID_FREQ,
	},
	[DDR4_2666] = {
		.f_pll_vco = ULL(2666666666),
		.f_pll_pclk = UL(666666666),
		.f_pll_rclk = INVALID_FREQ,
	},
	[DDR4_2400] = {
		.f_pll_vco = ULL(4800000000),
		.f_pll_pclk = UL(600000000),
		.f_pll_rclk = INVALID_FREQ,
	},
	[DDR4_2133] = {
		.f_pll_vco = ULL(4266666666),
		.f_pll_pclk = UL(533333333),
		.f_pll_rclk = INVALID_FREQ,
	},
	[DDR4_1866] = {
		.f_pll_vco = ULL(3733333333),
		.f_pll_pclk = UL(466666666),
		.f_pll_rclk = INVALID_FREQ,
	},
	[DDR4_1600] = {
		.f_pll_vco = ULL(3200000000),
		.f_pll_pclk = UL(400000000),
		.f_pll_rclk = INVALID_FREQ,
	},
	[DDR3_1333] = {
		.f_pll_vco = ULL(2666666666),
		.f_pll_pclk = UL(333333333),
		.f_pll_rclk = INVALID_FREQ,
	},
	[DDR3_1066] = {
		.f_pll_vco = ULL(3200000000),
		.f_pll_pclk = UL(266666666),
		.f_pll_rclk = INVALID_FREQ,
	},
	[DDR3_800] = {
		.f_pll_vco = ULL(3200000000),
		.f_pll_pclk = UL(200000000),
		.f_pll_rclk = INVALID_FREQ,
	},
	[DDR2_667] = {
		.f_pll_vco = ULL(2666666666),
		.f_pll_pclk = UL(166666666),
		.f_pll_rclk = UL(666666666),
	},
	[DDR2_533] = {
		.f_pll_vco = ULL(4266666666),
		.f_pll_pclk = UL(133333333),
		.f_pll_rclk = UL(533333333),
	},
	[DDR2_400] = {
		.f_pll_vco = ULL(3200000000),
		.f_pll_pclk = UL(100000000),
		.f_pll_rclk = UL(400000000),
	},
	[DDR_266] = {
		.f_pll_vco = ULL(3200000000),
		.f_pll_pclk = UL(66666666),
		.f_pll_rclk = UL(266666666),
	},
	[DDR_200] = {
		.f_pll_vco = ULL(3200000000),
		.f_pll_pclk = UL(50000000),
		.f_pll_rclk = UL(200000000),
	},
};

static struct ddr_speed *ddr_get_speed(int speed)
{
	if (speed > NR_DDR_SPEEDS)
		return NULL;
	return &ddr_speeds[speed];
}

clk_freq_t ddr_get_fvco(int speed)
{
	struct ddr_speed *ds;

	ds = ddr_get_speed(speed);
	return ds ? ds->f_pll_vco : INVALID_FREQ;
}

clk_freq_t ddr_get_fpclk(int speed)
{
	struct ddr_speed *ds;

	ds = ddr_get_speed(speed);
	return ds ? ds->f_pll_pclk: INVALID_FREQ;
}

clk_freq_t ddr_get_frclk(int speed)
{
	struct ddr_speed *ds;

	ds = ddr_get_speed(speed);
	return ds ? ds->f_pll_rclk: INVALID_FREQ;
}

void ddr_hw_config_speed(uint8_t speed)
{
	struct ddr_speed *ds;

	ds = ddr_get_speed(speed);
	if (ds) {
		clk_apply_vco(PLL2_VCO, ds->f_pll_vco);
		clk_apply_pll(PLL2_P, ds->f_pll_pclk);
		clk_apply_pll(PLL2_R, ds->f_pll_rclk);
	}
}

void ddr_hw_enable_speed(uint8_t speed)
{
	struct ddr_speed *ds;

	ds = ddr_get_speed(speed);
	if (ds) {
		if (ds->f_pll_pclk != INVALID_FREQ)
			clk_enable(pll2_p);
		if (ds->f_pll_rclk != INVALID_FREQ)
			clk_enable(pll2_r);
	}
}
#endif

static ddr_cid_t ddr_cids[NR_DDR_SLOTS] = {
	0, 1,
};

static uint8_t ddr_spd_addrs[NR_DDR_SLOTS] = {
	0x51, 0x50,
};

void ddr_hw_wait_dfi(uint32_t cycles)
{
	uint32_t ratio;
	tsc_count_t last;

	ratio = DIV_ROUND_UP(clk_get_frequency(ddr_clk), __TSC_FREQ);
	last = DIV_ROUND_UP(cycles, ratio) + tsc_read_counter();
	while (time_before(tsc_read_counter(), last));
}

void ddr_hw_slot_reset(void)
{
	ddr_slot_ctrl.cid = ddr_cids[ddr_sid];
	ddr_slot_ctrl.smbus = 0;
	ddr_slot_ctrl.spd_addr = ddr_spd_addrs[ddr_sid];
}

void ddr_hw_slot_select(ddr_sid_t sid)
{
}

void ddr_hw_chan_reset(void)
{
}

void ddr_hw_chan_select(ddr_cid_t cid)
{
}
