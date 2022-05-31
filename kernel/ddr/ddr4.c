#include <target/ddr.h>
#include <target/console.h>

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

	tREFI = DDR4_tREFI[cap];
	tRFCmin = DDR4_tRFCmin[cap][fgr];

	ddr4_hw_config_refresh(n, mode, tREFI, tRFCmin);
}
