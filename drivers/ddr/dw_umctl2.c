#include <target/ddr.h>
#include <target/clk.h>

static void dw_umctl2_init_static_regs(uint8_t n, uint8_t c,
				       uint8_t f, uint8_t spd)
{
	if (spd == DDR4_1600) {
		__raw_writel(0x00000001, UMCTL2_DBG1(n, c));
		__raw_writel(0x00000001, UMCTL2_PWRCTL(n, c));
		__raw_writel(0x6F040210, UMCTL2_MSTR(n));
		__raw_writel(0x00000000, UMCTL2_MSTR2(n));
#ifdef CONFIG_UMCTL2_TRAINING
		__raw_writel(0x000000A2, UMCTL2_PWRCTL(n, c));
#else
		__raw_writel(0x00000000, UMCTL2_PWRCTL(n, c));
#endif
		__raw_writel(0x00400004, UMCTL2_PWRTMG(n, f));
		__raw_writel(0x00210000, UMCTL2_RFSHCTL0(n, f));
		__raw_writel(0x004D0048, UMCTL2_RFSHCTL1(n));
		__raw_writel(0x00690067, UMCTL2_RFSHCTL2(n));
		__raw_writel(0x00000000, UMCTL2_RFSHCTL2(n));
		__raw_writel(0x00A200EA, UMCTL2_RFSHTMG(n, f));
#ifdef CONFIG_UMCTL2_ECC
		__raw_writel(0x00000004, UMCTL2_ECCCFG0(n));
#else
		__raw_writel(0x00000000, UMCTL2_ECCCFG0(n));
#endif
		__raw_writel(0x00000000, UMCTL2_ECCCFG1(n));
		__raw_writel(0x00000300, UMCTL2_ECCCTL(n, c));
#ifdef CONFIG_UMCTL2_TRAINING
		__raw_writel(0xC0030128, UMCTL2_INIT0(n));
#else
		__raw_writel(0x00030128, UMCTL2_INIT0(n));
#endif
		__raw_writel(0x00010002, UMCTL2_INIT1(n));
		__raw_writel(0x0A150001, UMCTL2_INIT3(n, f));
		__raw_writel(0x00000000, UMCTL2_INIT4(n, f));
		__raw_writel(0x00110000, UMCTL2_INIT5(n));
	}
}

void dw_umctl2_start(void)
{
	ddr_wait_dfi(8);
	clk_enable(ddr_arst);
	ddr_wait_dfi(8);
	clk_enable(ddr_rst);

	/* TODO: configure dw_umctl2 after setting ddr_clk/ddr_rst */
}

void dw_umctl2_init(void)
{
	ddr_wait_dfi(8);
	clk_enable(ddr_por);
	ddr_wait_dfi(8);
	clk_enable(ddr_prst);

	dw_umctl2_init_static_regs(0, 0, 0, DDR4_1600);
}
