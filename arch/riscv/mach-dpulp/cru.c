#include <target/clk.h>
#include <target/delay.h>
#include <target/cmdline.h>

void __dpulp_pll_reg_write(uint8_t pll, uint8_t reg, uint8_t val)
{
	__raw_writel(PLL_REG_WRITE | PLL_REG_SEL(reg) | PLL_REG_WDATA(val),
		     PLL_REG_ACCESS(pll));
	while (!(__raw_readl(PLL_REG_ACCESS(pll)) & PLL_REG_IDLE));
}

void dpulp_pll_reg_write(uint8_t pll, uint8_t reg, uint8_t val)
{
	if (pll == 2) {
		dpu_pll_reg_select_ddr0();
		__dpu_pll_reg_write(pll, reg, val);
		dpu_pll_reg_select_ddr1();
		__dpu_pll_reg_write(pll, reg, val);
	} else
		__dpu_pll_reg_write(pll, reg, val);
}

uint8_t dpulp_pll_reg_read(uint8_t pll, uint8_t reg)
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
