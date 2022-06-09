#include <target/ddr.h>
#include <target/clk.h>

uint8_t dw_umctl2_f;
uint16_t dw_umctl2_spd;
uint8_t dw_umctl2_freq_ratio;

#ifdef CONFIG_DW_UMCTL2_ECC
static void dw_umctl2_init_ecc(uint8_t n, uint8_t c, uint8_t f)
{
	dw_umctl2_write(0x4, UMCTL2_ECCCFG0(n));
	dw_umctl2_write(0x0, UMCTL2_ECCCFG1(n));
	dw_umctl2_write(0x300, UMCTL2_ECCCTL(n, c));
	dw_umctl2_write(0x100, UMCTL2_INIT6(n, f));
}
#else
#define dw_umctl2_init_ecc(n, c, f)		do { } while (0)
#endif

#ifdef CONFIG_DW_UMCTL2_FAST_FREQ_CHANGE
static inline void dw_umctl2_init_ffc(uint8_t n, uint8_t f, uint8_t spd)
{
	dw_umctl2_set(MSTR_frequency_mode, UMCTL2_MSTR(n));
	dw_umctl2_write(MSTR2_target_frequency(f), UMCTL2_MSTR2(n));
	dw_umctl2_f = f;
	dw_umctl2_spd = spd;
}
#else
static inline void dw_umctl2_init_ffc(uint8_t n, uint8_t f, uint8_t spd)
{
	dw_umctl2_clear(MSTR_frequency_mode, UMCTL2_MSTR(n));
	dw_umctl2_f = 0;
	dw_umctl2_spd = spd;
}
#endif

static void dw_umctl2_init_dev(uint8_t n, uint8_t spd, uint8_t dev,
			       uint8_t ranks)
{
	dw_umctl2_freq_ratio = MSTR_frequency_ratio_static;
	dw_umctl2_write(MSTR_protocols(MSTR_ddr4) | /* static */
			MSTR_device_config(MSTR_device_x8) | /* static */
			MSTR_active_ranks(ranks) | /* static */
			MSTR_burst_mode_static |
			MSTR_burstchop_static |
			MSTR_en_2t_timing_mode_static |
			MSTR_data_bus_width_static |
			MSTR_burst_rdwr_static |
			MSTR_active_logical_ranks_static |
			MSTR_frequency_ratio_static |
			MSTR_dll_off_mode_quasi |
			MSTR_geardown_mode_quasi |
			MSTR_frequency_mode_quasi, /* Do not configure? */
			UMCTL2_MSTR(n));
	dw_umctl2_ddr4_init_device(n);
}

/*===== Low power control =====*/
void dw_umctl2_set_idle_sr(uint8_t n, uint8_t c, uint8_t f,
			   bool enable, uint16_t cycles)
{
	dw_umctl2_set_msk(PWRTMG_selfref_to_x32(cycles),
			  PWRTMG_selfref_to_x32_mask,
			  UMCTL2_PWRTMG(n, f));
	if (enable)
		dw_umctl2_enable_idle_sr(n, c);
	else
		dw_umctl2_disable_idle_sr(n, c);
}

void dw_umctl2_set_idle_pd(uint8_t n, uint8_t c, uint8_t f,
			   bool enable, uint16_t cycles)
{
	dw_umctl2_set_msk(PWRTMG_powerdown_to_x32(cycles),
			  PWRTMG_powerdown_to_x32_mask,
			  UMCTL2_PWRTMG(n, f));
	if (enable)
		dw_umctl2_enable_idle_pd(n, c);
	else
		dw_umctl2_disable_idle_pd(n, c);
}

#if defined(CONFIG_DW_UMCTL2_MOBILE) || defined(CONFIG_DW_UMCTL2_LPDDR2)
void dw_umctl2_set_idle_dpd(uint8_t n, uint8_t c, uint8_t f,
			    bool enable, uint32_t cycles)
{
	dw_umctl2_set_msk(PWRTMG_t_dpd_x4096(cycles),
			  PWRTMG_t_dpd_x4096_mask,
		          UMCTL2_PWRTMG(n, f));
	if (enable)
		dw_umctl2_enable_idle_dpd(n, c);
	else
		dw_umctl2_disable_idle_dpd(n, c);
}
#endif

void dw_umctl2_set_idle_cs(uint8_t n, uint8_t c, bool enable)
{
	if (enable)
		dw_umctl2_enable_idle_cs(n, c);
	else
		dw_umctl2_disable_idle_cs(n, c);
}

void dw_umctl2_set_idle(uint8_t n, uint8_t c, uint8_t f,
			uint8_t state, uint16_t cycles_selfref,
			uint16_t cycles_powerdown)
{
	dw_umctl2_set_idle_sr(n, c, f, !!(state & DW_UMCTL2_POWER_SR),
			      cycles_selfref);
	dw_umctl2_set_idle_pd(n, c, f, !!(state & DW_UMCTL2_POWER_PD),
			      cycles_powerdown);
	dw_umctl2_set_idle_dpd(n, c, f, !!(state & DW_UMCTL2_POWER_DPD),
			       cycles_powerdown * 64);
	dw_umctl2_set_idle_mpsm(n, c, !!(state & DW_UMCTL2_POWER_MPSM));
	dw_umctl2_set_idle_cs(n, c, !!(state & DW_UMCTL2_POWER_CS));
}

static void dw_umctl2_init_pwr(uint8_t n, uint8_t c, uint8_t f)
{
	dw_umctl2_set_idle(n, c, f, DW_UMCTL2_POWER_DEF, 2048, 128);
}

/*===== Refresh control =====*/
#define dw_umctl2_set_refresh_timer_start(n, r, v)	\
	dw_umctl2_set_msk(RFSHCTL12_refresh_timern_start_value_x32(r, v),\
			RFSHCTL12_refresh_timern_start_value_x32_mask(r),\
			UMCTL2_RFSHCTL12(n, r))

#ifdef CONFIG_DW_UMCTL2_LPDDR2
static void dw_umct2l_init_lpddr2_rfsh(uint8_t n, uint8_t f)
{
	if (RFSHCTL0_per_bank_refresh_static)
		dw_umctl2_set(RFSHCTL0_per_bank_refresh,
			      UMCTL2_RFSHCTL0(n, f));
	if (RFSHTMG_t_rfc_nom_x1_sel_dynamic)
		dw_umctl2_set(RFSHTMG_t_rfc_nom_x1_sel_dynamic,
			      UMCTL2_RFSHTMG(n, f));
}
#else
#define dw_umctl2_init_lpddr2_rfsh(n, f)	do { } while (0)
#endif

#define dw_umctl2_update_rfsh(n)			\
	dw_umctl2_toggle(RFSHCTL3_refresh_update_level,	\
			 UMCTL2_RFSH3CTL(n))

static void dw_umctl2_init_rfsh(uint8_t n, uint8_t ranks)
{
	uint8_t f = dw_umctl2_f;
	uint8_t r;

	dw_umctl2_write(RFSHCTL0_refresh_burst_dynamic |
			RFSHCTL0_refresh_to_x1_x32_dynamic |
			RFSHCTL0_refresh_margin_dynamic,
			UMCTL2_RFSHCTL0(n, f));
	for (r = 0; r < ranks; r++) {
		dw_umctl2_set_refresh_timer_start(n, r,
			RFSHCTL12_refresh_timern_start_value_x32_dynamic);
	}
	dw_umctl2_write(RFSHCTL3_refresh_mode_quasi,
			UMCTL2_RFSHCTL3(n));
	dw_umctl2_ddr4_init_refresh(n, ranks);
	dw_umctl2_init_lpddr2_rfsh(n, f);
}

#ifdef CONFIG_DW_UMCTL2_DBG_RFSH
void dw_umctl2_init_dbg_rfsh(uint8_t n)
{
	dw_umctl2_set(RFSHCTL3_dis_auto_refresh,
		      UMCTL2_RFSHCTL3(n));
}

void dw_umctl2_dbg_rfsh_issue(uint8_t n, uint8_t c, uint8_t ranks)
{
	while (!(dw_umctl2_read(UMCTL2_DBGSTAT(n, c)) &
		 DBGSTAT_ranks_refresh_busy(ranks)));
	dw_umctl2_set(DBGCMD_ranks_refresh(ranks), UMCTL2_DBGCMD(n, c));
}
#else
#define dw_umctl2_init_dbg_rfsh(n)		do { } while (0)
#define dw_umctl2_dbg_rfsh_issue(n, c, ranks)	do { } while (0)
#endif

static void dw_umctl2_init_static_regs(uint8_t n, uint8_t c,
				       uint8_t f, uint8_t spd)
{
	if (spd == DDR4_1600) {
		/* jedec_ddr4_rdimm_2G_x8_1600J_1_250_2R_ecc */
		dw_umctl2_write(DBG1_dis_dq, UMCTL2_DBG1(n, c));

		dw_umctl2_init_dev(n, spd, MSTR_device_x8, 2);
		dw_umctl2_init_ffc(n, f, spd);

		dw_umctl2_init_pwr(n, c, f);
#ifdef CONFIG_DW_UMCTL2_TRAINING
		dw_umctl2_entry_sr(n, c);
#else
		dw_umctl2_exit_sr(n, c);
#endif
		dw_umctl2_init_rfsh(n, 4);
		dw_umctl2_set_refresh_timer_start(n, 0, 0x900);
		dw_umctl2_set_refresh_timer_start(n, 1, 0x9A0);
		dw_umctl2_set_refresh_timer_start(n, 2, 0xCE0);
		dw_umctl2_set_refresh_timer_start(n, 3, 0xD20);
		printf("================================\n");
		ddr4_config_refresh(n, DDR4_2GB, DDR4_REFRESH_Fixed_1x);
		dw_umctl2_write(0x00000000, UMCTL2_RFSHCTL3(n));
		dw_umctl2_write(0x00A200EA, UMCTL2_RFSHTMG(n, f));
		printf("================================\n");

#ifdef CONFIG_UMCTL2_TRAINING
		dw_umctl2_write(0xC0030128, UMCTL2_INIT0(n));
#else
		dw_umctl2_write(0x00030128, UMCTL2_INIT0(n));
#endif
		dw_umctl2_write(0x00010002, UMCTL2_INIT1(n));
		dw_umctl2_write(0x0A150001, UMCTL2_INIT3(n, f));
		dw_umctl2_write(0x00000000, UMCTL2_INIT4(n, f));
		dw_umctl2_write(0x00110000, UMCTL2_INIT5(n));
	}
	if (spd == DDR4_3200) {
		/* jedec_ddr4_rdimm_8G_x8_3200W_0_625_2R_ecc */
		dw_umctl2_write(DBG1_dis_dq, UMCTL2_DBG1(n, c));

		dw_umctl2_init_dev(n, spd, MSTR_device_x8, 2);
		dw_umctl2_init_ffc(n, f, spd);

		dw_umctl2_init_pwr(n, c, f);
		/* Configure self refresh for training */
#ifdef CONFIG_DW_UMCTL2_TRAINING
		dw_umctl2_entry_sr(n, c);
#else
		dw_umctl2_exit_sr(n, c);
#endif
		dw_umctl2_init_rfsh(n, 4);
		dw_umctl2_set_refresh_timer_start(n, 0, 0xAE0);
		dw_umctl2_set_refresh_timer_start(n, 1, 0xBA0);
		dw_umctl2_set_refresh_timer_start(n, 2, 0xF80);
		dw_umctl2_set_refresh_timer_start(n, 3, 0xFC0);
		ddr4_config_refresh(n, DDR4_8GB, DDR4_REFRESH_Fixed_1x);

		printf("================================\n");
		ddr4_powerup_init(n);
#ifdef CONFIG_DW_UMCTL2_TRAINING
		dw_umctl2_set_msk(INIT0_skip_dram_init_quasi,
				  INIT0_skip_dram_init_mask,
				  UMCTL2_INIT0(n));
		printf("================================\n");
		dw_umctl2_write(0xC0030128, UMCTL2_INIT0(n));
#else
		printf("================================\n");
		dw_umctl2_write(0x00030128, UMCTL2_INIT0(n));
#endif
		dw_umctl2_write(0x00010002, UMCTL2_INIT1(n));
		dw_umctl2_write(0x0C450001, UMCTL2_INIT3(n, f));
		dw_umctl2_write(0x00280400, UMCTL2_INIT4(n, f));
		dw_umctl2_write(0x00110000, UMCTL2_INIT5(n));
#ifdef CONFIG_DW_UMCTL2_ECC
		dw_umctl2_write(0x00000100, UMCTL2_INIT6(n, f));
#else
		dw_umctl2_write(0x00000500, UMCTL2_INIT6(n, f));
#endif
		dw_umctl2_write(0x00001059, UMCTL2_INIT7(n, f));

		printf("================================\n");
		/* Testing: configure MR7 (RCD) to 20 cycles */
		dw_umctl2_mr_write(n, c, DW_UMCTL2_NUM_RANKS_MASK(2),
				   20, DDR4_RCW);
		printf("================================\n");
	}
}

void dw_umctl2_start(void)
{
#ifndef CONFIG_DW_UMCTL2_DEBUG_REGS
	ddr_wait_dfi(8);
	clk_enable(ddr_arst);
	ddr_wait_dfi(8);
	clk_enable(ddr_rst);
#endif

	/* TODO: configure dw_umctl2 after setting ddr_clk/ddr_rst */
}

void dw_umctl2_init(void)
{
#ifndef CONFIG_DW_UMCTL2_DEBUG_REGS
	ddr_wait_dfi(8);
	clk_enable(ddr_por);
	ddr_wait_dfi(8);
	clk_enable(ddr_prst);
#endif

	dw_umctl2_init_static_regs(0, 0, 0, ddr_spd);
}
