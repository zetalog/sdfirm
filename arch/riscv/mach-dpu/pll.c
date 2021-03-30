#include <target/clk.h>
#include <target/delay.h>

void __dpu_pll_reg_write(uint8_t pll, uint8_t reg, uint8_t val)
{
	__raw_writel(PLL_REG_WRITE | PLL_REG_SEL(reg) | PLL_REG_WDATA(val),
		     PLL_REG_ACCESS(pll));
	while (!(__raw_readl(PLL_REG_ACCESS(pll)) & PLL_REG_IDLE));
}

void dpu_pll_reg_write(uint8_t pll, uint8_t reg, uint8_t val)
{
	if (pll == 2) {
		dpu_pll_reg_select_ddr0();
		__dpu_pll_reg_write(pll, reg, val);
		dpu_pll_reg_select_ddr1();
		__dpu_pll_reg_write(pll, reg, val);
	} else
		__dpu_pll_reg_write(pll, reg, val);
}

uint8_t dpu_pll_reg_read(uint8_t pll, uint8_t reg)
{
	uint32_t val;

	__raw_writel(PLL_REG_READ | PLL_REG_SEL(reg),
		     PLL_REG_ACCESS(pll));
	do {
		val = __raw_readl(PLL_REG_ACCESS(pll));
	} while (!(val & PLL_REG_IDLE));
	return PLL_REG_RDATA(val);
}

void dpu_pll_soft_reset(uint8_t comp)
{
	uint32_t srst = PLL_SRST(comp);

#ifdef CONFIG_DPU_PLL_FORCE_RESET
	if (__raw_readl(PLL_SOFT_RST(comp)) & srst) {
		__raw_clearl(srst, PLL_SOFT_RST(comp));
		/* XXX: How long should we wait?
		 *
		 * The fxxxing SoC provides no clock enabling sequences,
		 * and provides no programming information for software
		 * to reset the IP component. It all depends on some what
		 * happen-to-work logics.
		 * Thus the underlying fxxxing design is there are multiple
		 * clock/reset lines for a single IP, the SoC simply
		 * enables all clocks during POR, and AND all resets to
		 * the register bit dedicated to the single IP.
		 */
	}
#endif
	__raw_setl(srst, PLL_SOFT_RST(comp));
	while (!(__raw_readl(PLL_SOFT_RST(comp)) & srst));
}

struct freqplan {
	uint64_t f_pll_vco;
	uint32_t f_pll_pclk;
	uint32_t f_pll_rclk;
};

#ifdef CONFIG_DPU_PLL_FREQPLAN_PE
static struct freqplan pe_freqplan[NR_FREQPLANS] = {
	[0] = {
		.f_pll_vco = ULL(4000000000),
		.f_pll_pclk = UL(1000000000),
	},
	[1] = {
		.f_pll_vco = ULL(3600000000),
		.f_pll_pclk = UL(900000000),
	},
	[2] = {
		.f_pll_vco = ULL(3200000000),
		.f_pll_pclk = UL(800000000),
	},
	[3] = {
		.f_pll_vco = ULL(2800000000),
		.f_pll_pclk = UL(700000000),
	},
#ifdef CONFIG_DPU_PLL_FREQPLAN_PE_HISPEED
	[4] = {
		.f_pll_vco = ULL(4000000000),
		.f_pll_pclk = UL(2000000000),
	},
#else
	[4] = {
		.f_pll_vco = ULL(4800000000),
		.f_pll_pclk = UL(600000000),
	},
#endif
#if 0
	[5] = {
		.f_pll_vco = ULL(4000000000),
		.f_pll_pclk = UL(500000000),
	},
	[6] = {
		.f_pll_vco = ULL(3200000000),
		.f_pll_pclk = UL(400000000),
	},
#endif
};
#else
#define pe_freqplan		NULL
#endif

#ifdef CONFIG_DPU_PLL_FREQPLAN_BUS
struct freqplan bus_freqplan[NR_FREQPLANS] = {
	[0] = {
		.f_pll_vco = ULL(3200000000),
		.f_pll_pclk = UL(800000000),
		.f_pll_rclk = UL(100000000),
	},
	[1] = {
		.f_pll_vco = ULL(2800000000),
		.f_pll_pclk = UL(700000000),
		.f_pll_rclk = UL(100000000),
	},
	[2] = {
		.f_pll_vco = ULL(3600000000),
		.f_pll_pclk = UL(600000000),
		.f_pll_rclk = UL(100000000),
	},
	[3] = {
		.f_pll_vco = ULL(4000000000),
		.f_pll_pclk = UL(500000000),
		.f_pll_rclk = UL(100000000),
	},
	[4] = {
		.f_pll_vco = ULL(3200000000),
		.f_pll_pclk = UL(400000000),
		.f_pll_rclk = UL(100000000),
	},
};
#else
#define bus_freqplan		NULL
#endif

struct freqplan *freqplans[NR_PLLS] = {
	[IMC_CLK] = NULL,
	[PE_CLK] = pe_freqplan,
	[DDR_CLK] = NULL,
	[AXI_CLK] = bus_freqplan,
	[VPU_BCLK] = NULL,
#ifdef CONFIG_DPU_GEN2
	[TSENSOR_XO_CLK] = NULL,
#else /* CONFIG_DPU_GEN2 */
	[PCIE_REF_CLK] = NULL,
#endif /* CONFIG_DPU_GEN2 */
};

struct freqplan freqplans_def[NR_PLLS] = {
	[IMC_CLK] = {
		.f_pll_vco = PLL0_VCO_FREQ,
		.f_pll_pclk = PLL0_P_FREQ,
		.f_pll_rclk = INVALID_FREQ,
	},
#ifndef CONFIG_DPU_PLL_FREQPLAN_PE
	[PE_CLK] = {
		.f_pll_vco = PLL1_VCO_FREQ,
		.f_pll_pclk = PLL1_P_FREQ,
		.f_pll_rclk = INVALID_FREQ,
	},
#endif
#ifndef CONFIG_DPU_PLL_FREQPLAN_BUS
	[AXI_CLK] = {
		.f_pll_vco = PLL3_VCO_FREQ,
		.f_pll_pclk = PLL3_P_FREQ,
		.f_pll_rclk = PLL3_R_FREQ,
	},
#endif
	[VPU_BCLK] = {
		.f_pll_vco = PLL4_VCO_FREQ,
		.f_pll_pclk = PLL4_P_FREQ,
		.f_pll_rclk = PLL4_R_FREQ,
	},
#ifdef CONFIG_DPU_GEN2
	[TSENSOR_XO_CLK] = {
		.f_pll_vco = PLL5_VCO_FREQ,
		.f_pll_pclk = PLL5_P_FREQ,
		.f_pll_rclk = INVALID_FREQ,
	},
#else /* CONFIG_DPU_GEN2 */
	[PCIE_REF_CLK] = {
		.f_pll_vco = PLL5_VCO_FREQ,
		.f_pll_pclk = PLL5_P_FREQ,
		.f_pll_rclk = INVALID_FREQ,
	},
#endif /* CONFIG_DPU_GEN2 */
};

static struct freqplan *freqplan_get(int pll, int plan)
{
	struct freqplan *fp;

	if (pll > NR_PLLS || plan > NR_FREQPLANS)
		return NULL;
	fp = freqplans[pll];
	if (!fp)
		return fp;
	return &fp[plan];
}

clk_freq_t freqplan_get_fvco(int pll, int plan)
{
	struct freqplan *fp;

	if (pll > NR_PLLS)
		return INVALID_FREQ;
	fp = freqplan_get(pll, plan);
	return fp ? fp->f_pll_vco : freqplans_def[pll].f_pll_vco;
}

clk_freq_t freqplan_get_fpclk(int pll, int plan)
{
	struct freqplan *fp;

	if (pll > NR_PLLS)
		return INVALID_FREQ;
	fp = freqplan_get(pll, plan);
	return fp ? fp->f_pll_pclk : freqplans_def[pll].f_pll_pclk;
}

clk_freq_t freqplan_get_frclk(int pll, int plan)
{
	struct freqplan *fp;

	if (pll > NR_PLLS)
		return INVALID_FREQ;
	fp = freqplan_get(pll, plan);
	return fp ? fp->f_pll_rclk : freqplans_def[pll].f_pll_rclk;
}
