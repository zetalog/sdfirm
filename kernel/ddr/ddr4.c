#include <target/ddr.h>
#include <target/console.h>
#include <target/panic.h>

struct ddr4_dev ddr4_devices[DDR_MAX_CHANNELS];

/* Table 24 - tREFI and tRFC parameters */
static uint32_t DDR4_tREFI[DDR4_MAX_ADDRESSINGS] = {
	[DDR4_2GB] = 7800000,
	[DDR4_4GB] = 7800000,
	[DDR4_8GB] = 7800000,
	[DDR4_16GB] = 7800000,
};

static uint32_t DDR4_tRFCmin[DDR4_MAX_ADDRESSINGS]
			    [DDR4_MAX_REFRESHES] = {
	[DDR4_2GB] = {
		[DDR4_REFRESH_Fixed_1x] = 160000,
		[DDR4_REFRESH_Fixed_2x] = 110000,
		[DDR4_REFRESH_Fixed_4x] = 90000,
	},
	[DDR4_4GB] = {
		[DDR4_REFRESH_Fixed_1x] = 260000,
		[DDR4_REFRESH_Fixed_2x] = 160000,
		[DDR4_REFRESH_Fixed_4x] = 110000,
	},
	[DDR4_8GB] = {
		[DDR4_REFRESH_Fixed_1x] = 350000,
		[DDR4_REFRESH_Fixed_2x] = 260000,
		[DDR4_REFRESH_Fixed_4x] = 160000,
	},
	[DDR4_16GB] = {
		[DDR4_REFRESH_Fixed_1x] = 550000,
		[DDR4_REFRESH_Fixed_2x] = 350000,
		[DDR4_REFRESH_Fixed_4x] = 260000,
	},
};

/* Table 9 - Write Command Latency when CRC and DM are both enabled */
static uint8_t DDR4_WriteCommandLatencies[DDR4_MAX_SPDS] = {
	4,
	5,
	5,
	5,
	5,
	6,
	6,
};

/* Table 14 - tCCD_L and tDLLK */
static uint8_t DDR4_tCCD_Lmin[DDR4_MAX_SPDS] = {
	5,
	5,
	6,
	6,
	7,
	7,
	8,
};

static uint16_t DDR4_tDLLK[DDR4_MAX_SPDS] = {
	597,
	597,
	768,
	768,
	1024,
	1024,
	1024,
};

/* Table 13 - C/A Parity Latency Mode */
static uint8_t DDR4_CAPAR_latency_modes[DDR4_MAX_SPDS] = {
	4,
	4,
	4,
	5,
	5,
	6,
	6,
};

/* Table 6 - CWL */
#define DDR4_MAX_CWL_SETS	2
static uint8_t DDR4_CWL_1T[DDR4_MAX_CWL_SETS][DDR4_MAX_SPDS] = {
	[0] = {
		9,
		10,
		11,
		12,
		14,
		16,
		16,
	},
	[1] = {
		11,
		12,
		14,
		16,
		18,
		20,
		20,
	},
};

static uint8_t DDR4_CWL_2T[DDR4_MAX_CWL_SETS][DDR4_MAX_SPDS] = {
	[0] = {
		0,
		0,
		0,
		14,
		16,
		18,
		18,
	},
	[1] = {
		0,
		0,
		0,
		16,
		18,
		20,
		20,
	},
};

#ifdef CONFIG_DDR4_TCR
#define ddr4_sanitize_tcr_mode(__m)				\
	do {							\
		if ((__m) & DDR4_TCR_MODE_MASK)	{		\
			(__m) &= ~DDR4_FGR_MODE_MASK;		\
			(__m) |= DDR4_REFRESH_Fixed_1x;		\
		}						\
	} while (0)
#else
#define ddr4_sanitize_tcr_mode(__m)	((__m) &= ~DDR4_TCR_MODE_MASK)
#endif

void ddr4_config_refresh(uint8_t n, uint8_t cap, uint8_t mode)
{
	uint8_t rfsh;
	uint32_t tREFI;
	uint32_t tRFCmin;

	BUG_ON(n >= NR_DDR_CHANNELS);

	if (cap >= DDR4_MAX_ADDRESSINGS) {
		con_err("Unsupported capacity - %d\n", cap);
		return;
	}
	ddr4_sanitize_tcr_mode(mode);
	rfsh = mode & DDR4_REFRESH_MASK;
	if (rfsh >= DDR4_MAX_REFRESHES) {
		con_err("Unsupported refresh mode - %d\n", rfsh);
		return;
	}

	if (mode & DDR4_REFRESH_TCR_ModeEnabled) {
		ddr4_devices[n].mode |= DDR4_MODE_TCR;
		if (mode & DDR4_REFRESH_TCR_RangeExtended)
			ddr4_devices[n].mode |= DDR4_MODE_TCR_RangeExtended;
	} else {
		ddr4_devices[n].mode &= ~DDR4_MODE_TCR;
		ddr4_devices[n].mode &= ~DDR4_MODE_TCR_RangeExtended;
	}
	ddr4_devices[n].FGR_mode = mode & DDR4_FGR_MODE_MASK;

	ddr4_devices[n].tREFI = tREFI = DDR4_tREFI[cap];
	ddr4_devices[n].tRFCmin = tRFCmin = DDR4_tRFCmin[cap][rfsh];

	ddr4_hw_config_refresh(n, mode, tREFI, tRFCmin);
}

/* 3.3.2 Reset Initialization with Stable Power
 *
 * Implements step 2-10 of this sequence.
 */
static inline void __ddr4_reset_init(uint8_t n)
{
	uint32_t tXS;
	uint16_t MR;

	tXS = ddr4_devices[n].tRFCmin + 10000;

	/* 2. After RESET_n is de-asserted, wait for another 500us until
	 *    CKE becomes active. During this time, the DRAM will start
	 *    internal initialization; this will be done independently of
	 *    external clocks.
	 * 3. Clocks (CK_t,CK_c) need to be started and stabilized for at
	 *    least 10ns or 5tCK (which is larger) before CKE goes active.
	 *    Since CKE is a synchronous signal, the corresponding setup
	 *    time to clock (tIS) must be met. Also a Deselect command
	 *    must be registered (with tIS set up time to clock) at clock
	 *    edge Td. Once the CKE registered “High” after Reset, CKE
	 *    needs to be continuously registered "High" until the
	 *    initialization sequence is finished, including expiration of
	 *    tDLLK and tZQinit.
	 * 4. The DDR4 SDRAM keeps its on-die termination in
	 *    high-impedance state as long as RESET_n is asserted.
	 *    Further, the SDRAM keeps its on-die termination in high
	 *    impedance state after RESET_n deassertion until CKE is
	 *    registered HIGH. The ODT input signal may be in undefined
	 *    state until tIS before CKE is registered HIGH. When CKE is
	 *    registered HIGH, the ODT input signal may be statically held
	 *    at either LOW or HIGH. If RTT_NOM is to be enabled in MR1
	 *    the ODT input signal must be statically held LOW. In all
	 *    cases, the ODT input signal remains static until the power
	 *    up initialization sequence is finished, including the
	 *    expiration of tDLLK and tZQinit.
	 */
	ddr4_hw_init_CKE(n, 500000000, 10000);
	/* 5. After CKE is being registered high, wait minimum of Reset
	 *    CKE Exit time, tXPR, before issuing the first MRS command to
	 *    load mode register. (tXPR=Max(tXS, 5nCK)]
	 */
	ddr4_hw_init_tXPR(n, tXS);
	/* 6. Issue MRS Command to to load MR3 with all application
	 *    settings (To issue MRS command to MR3, provide "Low" to BG0,
	 *    "High" to BA1, BA0)
	 */
	MR = DDR4_MR_MR3(ddr4_devices[n].mode,
			 ddr4_devices[n].WR_CMD_latency,
			 ddr4_devices[n].FGR_mode);
	ddr_hw_init_MR(n, MR, DDR4_MR3);
	/* 7. Issue MRS command to load MR6 with all application settings
	 *    (To issue MRS command to MR6, provide "Low" to BA0, "High" to
	 *    BG0, BA1)
	 */
	MR = DDR4_MR_MR6(ddr4_devices[n].mode,
			 ddr4_devices[n].tCCD_Lmin,
			 DDR4_TRAINING_VrefDQ_Range2, 6125);
	ddr_hw_init_MR(n, MR, DDR4_MR6);
	/* 8. Issue MRS command to load MR5 with all application settings
	 *    (To issue MRS command to MR5, provide "Low" to BA1, "High" to
	 *    BG0, BA0)
	 */
	MR = DDR4_MR_MR5(ddr4_devices[n].mode,
			 ddr4_devices[n].RTT_PARK,
			 ddr4_devices[n].CAPAR_latency_mode);
	ddr_hw_init_MR(n, MR, DDR4_MR5);
	/* 9. Issue MRS command to load MR4 with all application settings
	 *    (To issue MRS command to MR4, provide "Low" to BA1, BA0,
	 *    "High" to BG0)
	 */
	MR = DDR4_MR_MR4(ddr4_devices[n].mode,
			 ddr4_devices[n].CS_CA_latency_mode);
	ddr_hw_init_MR(n, MR, DDR4_MR4);
	/* 10. Issue MRS command to load MR2 with all application settings
	 *     (To issue MRS command to MR2, provide "Low" to BG0, BA0,
	 *     "High" to BA1)
	 */
	MR = DDR4_MR_MR2(ddr4_devices[n].mode,
			 ddr4_devices[n].RTT_WR,
			 ddr4_devices[n].LP_ASR_mode,
			 ddr4_devices[n].CWL);
	ddr_hw_init_MR(n, MR, DDR4_MR2);
	/* 11. Issue MRS command to load MR1 with all application settings
	 *     (To issue MRS command to MR1, provide "Low" to BG0, BA1,
	 *     "High" to BA0)
	 */
	MR = DDR4_MR_MR1(ddr4_devices[n].mode,
			 ddr4_devices[n].RTT_NOM,
			 ddr4_devices[n].additive_latency,
			 ddr4_devices[n].output_driver_impedance_control);
	ddr_hw_init_MR(n, MR, DDR4_MR1);
	/* 12. Issue MRS command to load MR0 with all application settings
	 *     (To issue MRS command to MR0, provide "Low" to BG0, BA1,
	 *     BA0)
	 */
	MR = DDR4_MR_MR0(ddr4_devices[n].mode,
			 ddr4_devices[n].WR_RTP,
			 ddr4_devices[n].CAS_latency,
			 ddr4_devices[n].burst_length);
	ddr_hw_init_MR(n, MR, DDR4_MR0);
}

/* 3.3 RESET and Initialization Procedure */
/* 3.3.1 Power-up Initialization Sequence
 *
 * Implements step 2-14 of this sequence.
 */
void ddr4_powerup_init(uint8_t n)
{
	BUG_ON(n >= NR_DDR_CHANNELS);

	/* 1. Apply power (RESET_n and TEN are recommended to be
	 *    maintained below 0.2 x VDD; all other inputs may be
	 *    undefined). RESET_n needs to be maintained below 0.2 x VDD
	 *    for minimum 200us with stable power and TEN needs to be
	 *    maintained below 0.2 x VDD for minimum 700us with stable
	 *    power. CKE is pulled "Lowr"” anytime before RESET_n being
	 *    de-asserted (min. time 10ns). The power voltage ramp time
	 *    between 300mV to VDD min must be no greater than 200ms; and
	 *    during the ramp, VDD ≥ VDDQ and (VDD-VDDQ) < 0.3 V. VPP must
	 *    ramp at the same time or earlier than VDD and VPP must be
	 *    equal to or higher than VDD at all times.
	 */
	ddr4_hw_init_RESET_n(n, 200000000);
	__ddr4_reset_init(n);
	/* 13. Issue ZQCL command to starting ZQ calibration
	 * 14. Wait for both tDLLK and tZQ init completed
	 */
	ddr4_hw_init_ZQCL(n, DDR4_tZQinit * ddr4_devices[n].tCK);
	/* 15. The DDR4 SDRAM is now ready for read/Write training (include
	 *     Vref training and Write leveling).
	 */
}

void ddr4_reset_init(uint8_t n)
{
	BUG_ON(n >= NR_DDR_CHANNELS);

	/* 1. Asserted RESET_n below 0.2 * VDD anytime when reset is
	 *    needed (all other inputs may be undefined). RESET_n needs to
	 *    be maintained for minimum tPW_RESET. CKE is pulled "LOW"
	 *    before RESET_n being de-asserted (min. time 10 ns).
	 */
	ddr4_hw_init_RESET_n(n, 200000000);
	/* 2. Follow steps 2 to 10 in "Power-up Initialization Sequence" */
	__ddr4_reset_init(n);
	/* 3. The Reset sequence is now completed, DDR4 SDRAM is ready for
	 *    Read/Write training (include Vref training and Write
	 *    leveling)
	 */
}

void ddr4_config_write_leveling(uint8_t n, uint8_t c, uint8_t ranks,
				bool enable, bool Qoff)
{
	uint16_t MR;

	/* NOTE:
	 * In write leveling mode, only DQS_t/DQS_c terminations are
	 * activated and deactivated via ODT pin, unlike normal operation.
	 * NOTE 1:
	 * In Write Leveling Mode with its output buffer disabled
	 * (MR1[bit A7] = 1 with MR1[bit A12] = 1) all RTT_NOM and
	 * RTT_PARK settings are allowed;
	 * in Write Leveling Mode with its output buffer enabled
	 * (MR1[bit A7] = 1 with MR1[bit A12] = 0) all RTT_NOM and
	 * RTT_PARK settings are allowed.
	 * NOTE 2:
	 * Dynamic ODT function is not available in Write Leveling Mode.
	 * DRAM MR2 bits A[11:9] must be ‘000’ prior to entering Write
	 * Leveling Mode.
	 */
	if (enable) {
		MR = ddr_hw_mr_read(n, c, ranks, DDR4_MR2);
		MR &= ~DDR4_MR2_RTT_WR_mask;
		ddr_hw_mr_write(n, c, ranks, MR, DDR4_MR4);
	}
	MR = ddr_hw_mr_read(n, c, ranks, DDR4_MR1);
	MR &= ~(DDR4_MR1_WriteLevelingEnable(1) | DDR4_MR1_Qoff(1));
	MR |= DDR4_MR1_WriteLevelingEnable(enable) | DDR4_MR1_Qoff(Qoff);
	ddr_hw_mr_write(n, c, ranks, MR, DDR4_MR1);
}

void ddr4_config_ZQ_caibration(uint8_t n, uint8_t c, uint8_t ranks)
{
}

void ddr4_config_DQ_Vref_training(uint8_t n, uint8_t c, uint8_t ranks)
{
}

void ddr4_config_speed(uint8_t n)
{
	uint8_t ddr4spd;

	BUG_ON(n >= NR_DDR_CHANNELS);

	/* Sanity check SPD */
	if (!ddr4_spd_valid(ddr_spd)) {
		con_err("Unsupported DDR4 SPD: %d\n", ddr_spd);
		return;
	}
	ddr4spd = __ddr4_spd(ddr_spd);

	ddr4_devices[n].mode = DDR4_MODE_ODTInputBuffer |
			       DDR4_MODE_DLL;
	/* According to DE, DM must be disabled when ECC is enabled in
	 * dw_umctl2.
	 */
#ifdef CONFIG_DW_UMCTL2_ECC
	ddr4_devices[n].mode &= ~DDR4_MODE_DM;
#else
	ddr4_devices[n].mode |= DDR4_MODE_DM;
#endif
	ddr4_devices[n].tCK = DDR_SPD2tCK(ddr_spd);
	ddr4_devices[n].tCCD_Lmin = DDR4_tCCD_Lmin[ddr4spd];
	ddr4_devices[n].tDLLK = DDR4_tDLLK[ddr4spd];
	ddr4_devices[n].WR_CMD_latency = DDR4_WriteCommandLatencies[ddr4spd];
	ddr4_devices[n].CAPAR_latency_mode = DDR4_CAPAR_latency_modes[ddr4spd];
	if (ddr4_devices[n].mode & DDR4_MODE_WritePreamble_2T &&
	    ddr_spd >= DDR4_2400)
		ddr4_devices[n].CWL = DDR4_CWL_2T[0][ddr4spd];
	else
		ddr4_devices[n].CWL = DDR4_CWL_1T[0][ddr4spd];

	/* TODO: Put to specific configuration function */
	ddr4_devices[n].CS_CA_latency_mode = 0;
	ddr4_devices[n].RTT_WR = DDR4_RTT_WR_HiZ;
	ddr4_devices[n].RTT_PARK = 1;
	ddr4_devices[n].RTT_NOM = DDR4_RTT_NOM_Disable;
	ddr4_devices[n].LP_ASR_mode = DDR4_LP_MANUAL_Normal;
	ddr4_devices[n].additive_latency = DDR4_AdditiveLatency_Disable;
	ddr4_devices[n].output_driver_impedance_control = 7;
	ddr4_devices[n].WR_RTP = 24;
	ddr4_devices[n].CAS_latency = 20;
	ddr4_devices[n].burst_length = DDR4_BL8_BC4_OTF;
}

#if 0
void ddr4_config_module(uint8_t n, uint8_t type, uint8_t dies,
			uint8_t ranks, uint8_t width)
{
	uint32_t mode;

	ddr4_devices[n].ranks = ranks;
	ddr4_devices[n].width = width;

	switch (width) {
	case 4:
		break;
	case 8:
		mode |= DDR4_MODE_TDQS;
		mode |= DDR4_MODE_DM;
		mode |= DDR4_MODE_ReadDBI;
		mode |= DDR4_MODE_WriteDBI;
		break;
	case 16:
		mode |= DDR4_MODE_DM;
		mode |= DDR4_MODE_ReadDBI;
		mode |= DDR4_MODE_WriteDBI;
		break;
	}
}
#endif
