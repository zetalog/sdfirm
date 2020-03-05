#include <target/clk.h>
#include <target/delay.h>

void dpu_pll_reg_write(uint8_t pll, uint8_t reg, uint8_t val)
{
	__raw_writel(PLL_REG_WRITE | PLL_REG_SEL(reg) | PLL_REG_WDATA(val),
		     PLL_REG_ACCESS(pll));
	while (!(__raw_readl(PLL_REG_ACCESS(pll)) & PLL_REG_IDLE));
}

uint8_t dpu_pll_reg_read(uint8_t pll, uint8_t reg)
{
	uint32_t val;

	__raw_writel(PLL_REG_READ | PLL_REG_SEL(reg),
		     PLL_REG_ACCESS(pll));
	do {
		val = __raw_readl(PLL_REG_ACCESS(pll));
	} while (val & PLL_REG_IDLE);
	return PLL_REG_RDATA(val);
}

void dpu_pll_soft_reset(uint8_t comp)
{
	uint32_t srst = PLL_SRST(comp);

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
	__raw_setl(srst, PLL_SOFT_RST(comp));
	while (!(__raw_readl(PLL_SOFT_RST(comp)) & srst));
}

struct freqplan {
	uint64_t f_pll0_vco;
	uint64_t f_pll1_vco;
	uint64_t f_pll2_vco;
	uint64_t f_pll3_vco;
	uint64_t f_pll4_vco;
	uint32_t f_pll0_p;
	uint32_t f_pll1_p;
	uint32_t f_pll2_p;
	uint32_t f_pll3_p;
	uint32_t f_pll4_p;
	uint32_t f_pll3_r;
};

struct freqplan freqplans[NR_FREQPLANS] = {
	{
		.f_pll0_vco = PLL0_VCO_FREQ,
		.f_pll1_vco = PLL1_VCO_FREQ,
		.f_pll2_vco = PLL2_VCO_FREQ,
		.f_pll3_vco = PLL3_VCO_FREQ,
		.f_pll4_vco = PLL4_VCO_FREQ,
		.f_pll0_p = PLL0_P_FREQ,
		.f_pll1_p = PLL1_P_FREQ,
		.f_pll2_p = PLL2_P_FREQ,
		.f_pll3_p = PLL3_P_FREQ,
		.f_pll4_p = PLL4_P_FREQ,
		.f_pll3_r = PLL3_R_FREQ,
	},
#ifndef CONFIG_DPU_BOOT
	{
		.f_pll0_vco = ULL(3600000000),
		.f_pll1_vco = ULL(3600000000),
		.f_pll2_vco = ULL(2933333332),
		.f_pll3_vco = ULL(3200000000),
		.f_pll4_vco = ULL(3200000000),
		.f_pll0_p = UL(900000000),
		.f_pll1_p = UL(900000000),
		.f_pll2_p = UL(733333333),
		.f_pll3_p = UL(900000000),
		.f_pll4_p = UL(1800000000),
		.f_pll3_r = PLL3_R_FREQ,
	},
	{
		.f_pll0_vco = ULL(3200000000),
		.f_pll1_vco = ULL(3200000000),
		.f_pll2_vco = ULL(2666666664),
		.f_pll3_vco = ULL(3200000000),
		.f_pll4_vco = ULL(3200000000),
		.f_pll0_p = UL(800000000),
		.f_pll1_p = UL(800000000),
		.f_pll2_p = UL(666666666),
		.f_pll3_p = UL(800000000),
		.f_pll4_p = UL(1600000000),
		.f_pll3_r = PLL3_R_FREQ,
	},
	{
		.f_pll0_vco = ULL(2800000000),
		.f_pll1_vco = ULL(2800000000),
		.f_pll2_vco = ULL(4800000000),
		.f_pll3_vco = ULL(2800000000),
		.f_pll4_vco = ULL(2800000000),
		.f_pll0_p = UL(700000000),
		.f_pll1_p = UL(700000000),
		.f_pll2_p = UL(600000000),
		.f_pll3_p = UL(700000000),
		.f_pll4_p = UL(1400000000),
		.f_pll3_r = PLL3_R_FREQ,
	},
	{
		.f_pll0_vco = ULL(4800000000),
		.f_pll1_vco = ULL(4800000000),
		.f_pll2_vco = ULL(4266666664),
		.f_pll3_vco = ULL(3600000000),
		.f_pll4_vco = ULL(4800000000),
		.f_pll0_p = UL(600000000),
		.f_pll1_p = UL(600000000),
		.f_pll2_p = UL(533333333),
		.f_pll3_p = UL(600000000),
		.f_pll4_p = UL(1200000000),
		.f_pll3_r = PLL3_R_FREQ,
	},
	{
		.f_pll0_vco = ULL(4000000000),
		.f_pll1_vco = ULL(4000000000),
		.f_pll2_vco = ULL(3733333328),
		.f_pll3_vco = ULL(4000000000),
		.f_pll4_vco = ULL(4000000000),
		.f_pll0_p = UL(500000000),
		.f_pll1_p = UL(500000000),
		.f_pll2_p = UL(466666666),
		.f_pll3_p = UL(500000000),
		.f_pll4_p = UL(1000000000),
		.f_pll3_r = PLL3_R_FREQ,
	},
	{
		.f_pll0_vco = ULL(3200000000),
		.f_pll1_vco = ULL(3200000000),
		.f_pll2_vco = ULL(3200000000),
		.f_pll3_vco = ULL(3200000000),
		.f_pll4_vco = ULL(3200000000),
		.f_pll0_p = UL(400000000),
		.f_pll1_p = UL(400000000),
		.f_pll2_p = UL(400000000),
		.f_pll3_p = UL(400000000),
		.f_pll4_p = UL(800000000),
		.f_pll3_r = PLL3_R_FREQ,
	},
#endif
};
