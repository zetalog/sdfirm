#include <target/arch.h>

#define DDR_RESET_OFF	0x90

#define SOC_CLK_CFG	0x200
#define PCIE_CLK_CFG	0x204
#define CLUSTER_CLK_CFG	0x040
#define BUS_CLK_CFG	0x080
#define DDR_CLK_CFG	0x084

// cru: In; 0: EAST; 1: WEST
// rst: In; 0: Reset release; 1: Reset assert
void cru_ddr_reset(uint8_t cru, uint8_t rst)
{
	uint32_t *reg_ddr_reset;

	if (cru)
		reg_ddr_reset = (uint32_t *)(WEST_CRU_BASE + DDR_RESET_OFF);
	else
		reg_ddr_reset = (uint32_t *)(EAST_CRU_BASE + DDR_RESET_OFF);

	if (rst)
		*(reg_ddr_reset) = 0xf;
	else
		*(reg_ddr_reset) = 0x0;
}

void cru_config(uint64_t base, uint64_t offset, uint32_t and, uint32_t or)
{
	uint32_t *addr = (uint32_t *)(base + offset);
	uint32_t val;

	val = *addr;
	val &= and;
	val |= or;

	*(addr) = val;
}

// 0-pll, 1-xo
static inline void cru_clk_sel_pll(void)
{
	cru_config(CLUSTER0_CFG_BASE, CLUSTER_CLK_CFG, 0xfe, 0x0);
	cru_config(CLUSTER1_CFG_BASE, CLUSTER_CLK_CFG, 0xfe, 0x0);
	cru_config(CLUSTER2_CFG_BASE, CLUSTER_CLK_CFG, 0xfe, 0x0);
	cru_config(CLUSTER3_CFG_BASE, CLUSTER_CLK_CFG, 0xfe, 0x0);
	cru_config(CLUSTER4_CFG_BASE, CLUSTER_CLK_CFG, 0xfe, 0x0);
	cru_config(CLUSTER5_CFG_BASE, CLUSTER_CLK_CFG, 0xfe, 0x0);
	cru_config(CLUSTER6_CFG_BASE, CLUSTER_CLK_CFG, 0xfe, 0x0);
	cru_config(CLUSTER7_CFG_BASE, CLUSTER_CLK_CFG, 0xfe, 0x0);

	cru_config(SOC_CRU_BASE, SOC_CLK_CFG, 0xfc, 0x0);

	cru_config(EAST_CRU_BASE, BUS_CLK_CFG, 0xfe, 0x0);
	cru_config(EAST_CRU_BASE, BUS_CLK_CFG, 0xfc, 0x0);
	cru_config(WEST_CRU_BASE, BUS_CLK_CFG, 0xfe, 0x0);
	cru_config(WEST_CRU_BASE, BUS_CLK_CFG, 0xfc, 0x0);
}

void cru_rst_clk_init(void)
{
	cru_clk_sel_pll();
	cru_ddr_reset(0, 0);
	cru_ddr_reset(1, 0);
}
