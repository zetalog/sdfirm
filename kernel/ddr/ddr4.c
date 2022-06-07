#include <target/ddr.h>
#include <target/console.h>
#include <target/panic.h>

struct ddr4_dev ddr4_devices[DDR_MAX_DEVICES];

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

#ifdef CONFIG_DDR4_TCR
#define ddr4_tcr_mode(__m)					\
	do {							\
		if ((__m) & DDR4_REFRESH_TempControlled)	\
			__m = DDR4_REFRESH_TempControlled |	\
			      DDR4_REFRESH_Fixed_1x;		\
	} while (0)
#else
#define ddr4_tcr_mode(__m)	((__m) &= ~DDR4_REFRESH_TempControlled)
#endif

void ddr4_config_refresh(uint8_t n, uint8_t cap, uint8_t mode)
{
	uint8_t fgr;
	uint32_t tREFI;
	uint32_t tRFCmin;

	BUG_ON(n >= DDR_MAX_DEVICES);

	if (cap >= DDR4_MAX_ADDRESSINGS) {
		con_err("Unsupported capacity - %d\n", cap);
		return;
	}
	ddr4_tcr_mode(mode);
	fgr = mode & DDR4_REFRESH_MASK;
	if (fgr >= DDR4_MAX_REFRESHES) {
		con_err("Unsupported refresh mode - %d\n", fgr);
		return;
	}

	ddr4_devices[n].tREFI = tREFI = DDR4_tREFI[cap];
	ddr4_devices[n].tRFCmin = tRFCmin = DDR4_tRFCmin[cap][fgr];

	ddr4_hw_config_refresh(n, mode, tREFI, tRFCmin);
}

/* 3.3 RESET and Initialization Procedure */
/* 3.3.1 Power-up Initialization Sequence
 *
 * Implements step 2-14 of this sequence.
 */
void ddr4_powerup_init(uint8_t n)
{
	BUG_ON(n >= DDR_MAX_DEVICES);

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
	ddr4_hw_init_RESET_n_TEN(n, 200000000, 700000000);
	ddr4_reset_init(n);
#if 0
	ddr4_hw_init_ZQCL(n, 0);
	ddr4_hw_init_tDLLK_tZQinit(n, 0, 0);
#endif
}

/* 3.3.2 Reset Initialization with Stable Power
 *
 * Implements step 2-10 of this sequence.
 */
void ddr4_reset_init(uint8_t n)
{
	uint32_t tXS;

	BUG_ON(n >= DDR_MAX_DEVICES);

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
	ddr_hw_init_MR(n, 3, DDR4_MR3);
	/* 7. Issue MRS command to load MR6 with all application settings
	 *    (To issue MRS command to MR6, provide "Low" to BA0, "High" to
	 *    BG0, BA1)
	 */
	ddr_hw_init_MR(n, 6, DDR4_MR6);
	/* 8. Issue MRS command to load MR5 with all application settings
	 *    (To issue MRS command to MR5, provide "Low" to BA1, "High" to
	 *    BG0, BA0)
	 */
	ddr_hw_init_MR(n, 5, DDR4_MR5);
	/* 9. Issue MRS command to load MR4 with all application settings
	 *    (To issue MRS command to MR4, provide "Low" to BA1, BA0,
	 *    "High" to BG0)
	 */
	ddr_hw_init_MR(n, 4, DDR4_MR4);
	/* 10. Issue MRS command to load MR2 with all application settings
	 *     (To issue MRS command to MR2, provide "Low" to BG0, BA0,
	 *     "High" to BA1)
	 */
	ddr_hw_init_MR(n, 2, DDR4_MR2);
	/* 11. Issue MRS command to load MR1 with all application settings
	 *     (To issue MRS command to MR1, provide "Low" to BG0, BA1,
	 *     "High" to BA0)
	 */
	ddr_hw_init_MR(n, 1, DDR4_MR1);
	/* 12. Issue MRS command to load MR0 with all application settings
	 *     (To issue MRS command to MR0, provide "Low" to BG0, BA1,
	 *     BA0)
	 */
	ddr_hw_init_MR(n, 0, DDR4_MR0);
}
