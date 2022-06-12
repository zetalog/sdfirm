#include <target/ddr.h>
#include <target/clk.h>
#include <target/console.h>

#define CONFIG_DW_UMCTL2_DEBUG 1

/* 2.22 Mode Register Reads and Writes
 */
void dw_umctl2_mr_write(uint8_t n, uint8_t c, uint8_t ranks,
			uint16_t val, uint8_t reg)
{
	uint32_t mrctrl0, mrctrl1;

	while (dw_umctl2_read(UMCTL2_MRSTAT(n, c)) & MRSTAT_mr_wr_busy);

	mrctrl0 = MRCTRL0_mr_type(MRCTRL0_mr_Write);
	mrctrl0 |= MRCTRL0_mr_addr(reg);
	mrctrl0 |= MRCTRL0_mr_rank(ranks);
	if (reg == DDR4_RCW)
		mrctrl0 |= MRCTRL0_sw_init_int;
	mrctrl1 = MRCTRL1_mr_data(DDR4_MR162MR18(val));
	dw_umctl2_write(mrctrl0, UMCTL2_MRCTRL0(n, c));
	dw_umctl2_write(mrctrl1, UMCTL2_MRCTRL1(n, c));

	dw_umctl2_set(MRCTRL0_mr_wr, UMCTL2_MRCTRL0(n, c));
	while (dw_umctl2_read(UMCTL2_MRCTRL0(n, c)) & MRCTRL0_mr_wr);
	while (dw_umctl2_read(UMCTL2_MRSTAT(n, c)) & MRSTAT_mr_wr_busy);

	if (reg != DDR4_MR3)
		return;
	if (val != DDR4_MR3_MPROperation(DDR4_MPR_Operation_MPR))
		return;

	/* Take care of MR upadte */
	dw_umctl2_init_mr(n, val, reg);
}

uint16_t dw_umctl2_mr_read(uint8_t n, uint8_t c, uint8_t ranks, uint8_t reg)
{
	uint32_t mrctrl0;
	uint32_t mrr_data;

	while (dw_umctl2_read(UMCTL2_MRSTAT(n, c)) & MRSTAT_mr_wr_busy);

	mrctrl0 = MRCTRL0_mr_type(MRCTRL0_mr_Read);
	mrctrl0 |= MRCTRL0_mr_addr(reg);
	mrctrl0 |= MRCTRL0_mr_rank(ranks);
	if (reg == DDR4_RCW)
		mrctrl0 |= MRCTRL0_sw_init_int;
	dw_umctl2_write(mrctrl0, UMCTL2_MRCTRL0(n, c));

	dw_umctl2_set(MRCTRL0_mr_wr, UMCTL2_MRCTRL0(n, c));
	while (dw_umctl2_read(UMCTL2_MRCTRL0(n, c)) & MRCTRL0_mr_wr);
	while (dw_umctl2_read(UMCTL2_MRSTAT(n, c)) & MRSTAT_mr_wr_busy);

#if 1
	/* TODO: Should read hif_mrr_data signal instead */
	mrr_data = dw_umctl2_read(UMCTL2_MRCTRL1(n, c));
#else
	mrr_data = dw_umctl2_read(n, c, hif_mrr_data);
#endif
	return DDR4_MR182MR16(mrr_data);
}

#ifdef CONFIG_DW_UMCTL2_DDR4_MRAM
static inline void dw_umctl2_init_mram(uint8_t n)
{
	dw_umctl2_write(MSTR1_rank_tmgreg_sel_static, UMCTL2_MSTR1(n));
}

static void dw_umctl2_init_mram_rfsh(uint8_t n, uint8_t ranks)
{
	/* NYI */
}
#else
#define dw_umctl2_init_mram(n)			do { } while (0)
#define dw_umctl2_init_mram_rfsh(n, ranks)	do { } while (0)
#endif

#ifdef CONFIG_DW_UMCTL2_HET_RANK_DDR34
static inline void dw_umctl2_init_het_rank(uint8_t n)
{
	dw_umctl2_write(MSTR1_rfc_tmgreg_sel_static |
			MSTR1_alt_addrmap_en, /* static */
			UMCTL2_MSTR1(n));
}
#else
#define dw_umctl2_init_het_rank(n)		do { } while (0)
#endif

#ifndef CONFIG_DW_UMCTL2_CID_WIDTH_0
static inline void dw_umctl2_init_3ds(uint8_t n)
{
	/* NYI */
	dw_umctl2_write(MRCTRL0_mr_cid(0), UMCTL2_MRCTRL0(n));
}

static void dw_umctl2_init_3ds_rfsh(uint8_t n)
{
	dw_umctl2_write(RFSHCTL4_refresh_timer_lr_offset_x32_static,
			UMCTL2_RFSHCTL4(n));
}
#else
#define dw_umctl2_init_3ds(n)			do { } while (0)
#define dw_umctl2_init_3ds_rfsh(n)		do { } while (0)
#endif

void dw_umctl2_set_idle_mpsm(uint8_t n, uint8_t c, bool enable)
{
	if (enable)
		dw_umctl2_enable_idle_mpsm(n, c);
	else
		dw_umctl2_disable_idle_mpsm(n, c);
}

void dw_umctl2_ddr4_config_refresh(uint8_t n, uint8_t mode,
				   uint32_t tREFI, uint32_t tRFCmin)
{
	uint32_t tCK = DDR_SPD2tCK(ddr_spd);
	uint32_t t_rfc_nom_x1, t_rfc_min;

#ifdef CONFIG_DW_UMCTL2_DEBUG
	con_dbg("tCK=%d tREFI=%d tRFCmin=%d\n", tCK, tREFI, tRFCmin);
#endif
	dw_umctl2_set_msk(RFSHCTL3_refresh_mode(mode & DDR4_FGR_MODE_MASK),
			  RFSHCTL3_refresh_mode_mask,
			  UMCTL2_RFSHCTL3(n));
	t_rfc_nom_x1 = DIV_ROUND_DOWN(tREFI, tCK);
	t_rfc_min = DIV_ROUND_UP(tRFCmin, tCK);
	if (dw_umctl2_freq_ratio == MSTR_frequency_ratio_2) {
		t_rfc_nom_x1 = DIV_ROUND_DOWN(t_rfc_nom_x1, 2);
		t_rfc_min = DIV_ROUND_UP(t_rfc_min, 2);
	}
	dw_umctl2_write(RFSHTMG_t_rfc_nom_x1_x32(t_rfc_nom_x1) |
			RFSHTMG_t_rfc_min(t_rfc_min),
			UMCTL2_RFSHTMG(n, dw_umctl2_f));
}

void dw_umctl2_init_mr(uint8_t n, uint16_t val, uint8_t reg)
{
	uint8_t f = dw_umctl2_f;

	switch (reg) {
	case DDR4_MR0:
		dw_umctl2_set_msk(INIT3_mr(val), INIT3_mr_mask,
				  UMCTL2_INIT3(n, f));
		break;
	case DDR4_MR1:
		dw_umctl2_set_msk(INIT3_emr(val), INIT3_emr_mask,
				  UMCTL2_INIT3(n, f));
		break;
	case DDR4_MR2:
		dw_umctl2_set_msk(INIT4_emr2(val), INIT4_emr2_mask,
				  UMCTL2_INIT4(n, f));
		break;
	case DDR4_MR3:
		dw_umctl2_set_msk(INIT4_emr3(val), INIT4_emr3_mask,
				  UMCTL2_INIT4(n, f));
		break;
	case DDR4_MR4:
		dw_umctl2_set_msk(INIT6_mr4(val), INIT6_mr4_mask,
				  UMCTL2_INIT6(n, f));
		break;
	case DDR4_MR5:
		dw_umctl2_set_msk(INIT6_mr5(val), INIT6_mr5_mask,
				  UMCTL2_INIT6(n, f));
		break;
	case DDR4_MR6:
		dw_umctl2_set_msk(INIT7_mr6(val), INIT7_mr6_mask,
				  UMCTL2_INIT7(n, f));
		break;
	}
}

void dw_umctl2_ddr4_init_rstn(uint8_t n, uint32_t tPW_RESET)
{
	uint32_t tCK = DDR_SPD2tCK(ddr_spd);
	uint32_t t_dram_rstn;

	/* Maintain dfi_reset_n low for duration specified by
	 * INIT1.dram_rstn_x1024. Specification requires at least 200 us
	 * with stable power.
	 */
	t_dram_rstn = DIV_ROUND_UP(tPW_RESET, tCK);
	if (dw_umctl2_freq_ratio == MSTR_frequency_ratio_2)
		t_dram_rstn = DIV_ROUND_UP(t_dram_rstn, 2);

#ifdef CONFIG_DW_UMCTL2_DEBUG
	con_dbg("tCK=%d tPW_RESET=%d t_dram_rstn=%d\n",
		tCK, tPW_RESET, t_dram_rstn);
#endif
	dw_umctl2_set_msk(INIT1_dram_rstn_x1024(t_dram_rstn),
			  INIT1_dram_rstn_x1024_mask,
			  UMCTL2_INIT1(n));
}

void dw_umctl2_ddr4_init_pre_cke(uint8_t n,
				 uint32_t tCKEactive, uint32_t tCKEstab)
{
	uint32_t tCK = DDR_SPD2tCK(ddr_spd);
	uint32_t t_pre_cke;

	/* Issue NOP/deselect for duration specified by
	 * INIT0.pre_cke_x1024. Specification requires at least 500 us.
	 * For DDR3/DDR4 RDIMMs, this must include the time needed to
	 * satisfy tSTAB.
	 */
	t_pre_cke = DIV_ROUND_UP(tCKEactive + max(5 * tCK, tCKEstab), tCK);
	if (dw_umctl2_freq_ratio == MSTR_frequency_ratio_2)
		t_pre_cke = DIV_ROUND_UP(t_pre_cke, 2);

#ifdef CONFIG_DW_UMCTL2_DEBUG
	con_dbg("tCK=%d tCKEactive=%d tCKEstab=%d t_pre_cke=%d\n",
		tCK, tCKEactive, tCKEstab, t_pre_cke);
#endif
	dw_umctl2_set_msk(INIT0_pre_cke_x1024(t_pre_cke),
			  INIT0_pre_cke_x1024_mask,
			  UMCTL2_INIT0(n));
}

void dw_umctl2_ddr4_init_post_cke(uint8_t n, uint32_t tXS)
{
	uint32_t tCK = DDR_SPD2tCK(ddr_spd);
	uint32_t tXPR;
	uint32_t t_post_cke;

	/* Assert CKE and issue NOP/deselect for INIT0.post_cke_x1024
	 * (specification requires at least tXPR).
	 */
	tXPR = max(5 * tCK, tXS);
	t_post_cke = DIV_ROUND_UP(tXPR, tCK);
	if (dw_umctl2_freq_ratio == MSTR_frequency_ratio_2)
		t_post_cke = DIV_ROUND_UP(t_post_cke, 2);

#ifdef CONFIG_DW_UMCTL2_DEBUG
	con_dbg("tCK=%d tXS=%d tXPR=%d t_post_cke=%d\n",
		tCK, tXS, tXPR, t_post_cke);
#endif
	dw_umctl2_set_msk(INIT0_post_cke_x1024(t_post_cke),
			  INIT0_post_cke_x1024_mask,
			  UMCTL2_INIT0(n));
}

void dw_umctl2_ddr4_init_zqcl(uint8_t n, uint32_t tZQinit)
{
	uint32_t tCK = DDR_SPD2tCK(ddr_spd);
	uint32_t dev_zqinit;

	dev_zqinit = DIV_ROUND_UP(tZQinit, tCK);
	if (dw_umctl2_freq_ratio == MSTR_frequency_ratio_2)
		dev_zqinit = DIV_ROUND_UP(dev_zqinit, 2);

#ifdef CONFIG_DW_UMCTL2_DEBUG
	con_dbg("tCK=%d tZQinit=%d dev_zqinit=%d\n",
		tCK, tZQinit, dev_zqinit);
#endif
	dw_umctl2_set_msk(INIT5_dev_zqinit_x32(dev_zqinit),
			  INIT5_dev_zqinit_x32_mask,
			  UMCTL2_INIT5(n));
}

void dw_umctl2_ddr4_init_refresh(uint8_t n, uint8_t ranks)
{
	dw_umctl2_init_3ds_rfsh(n);
	dw_umctl2_init_mram_rfsh(n, ranks);
}

void dw_umctl2_ddr4_init_device(uint8_t n)
{
	dw_umctl2_init_mram(n);
	dw_umctl2_init_het_rank(n);
	dw_umctl2_init_3ds(n);
}
