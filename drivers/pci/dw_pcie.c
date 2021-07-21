#include <target/pci.h>
#include <target/console.h>

#ifdef IPBENCH
void dw_pcie_readl(uint64_t addr, uint32_t *data, int port);
void dw_pcie_writel(uint64_t addr, uint32_t data, int port);
#else
void dw_pcie_readl(uint64_t addr, uint32_t *data, int port)
{
	*data = readl(addr);
#ifdef CONFIG_DW_PCIE_DEBUG
	con_dbg("dw_pcie: R: 0x%llx=0x%x\n", addr, *data);
#endif
}

void dw_pcie_writel(uint64_t addr, uint32_t data, int port)
{
#ifdef CONFIG_DW_PCIE_DEBUG
	con_dbg("dw_pcie: W: 0x%llx=0x%x\n", addr, data);
#endif
	writel(addr, data);
}
#endif

static uint64_t dw_pcie_dbi_addr(enum dw_pcie_access_type type, uint32_t reg)
{
	/* uint64_t route0, route1, addr; */
	uint32_t route1, addr;

	/* BUG_ON(reg & 0x3 != 0); */
	switch (type) {
	case DW_PCIE_CDM:
		/* route0 = 0; */
		route1 = 0;
		break;
	case DW_PCIE_SHADOW:
		/* route0 = 1; */
		route1 = 1;
		break;
	case DW_PCIE_ATU:
		/* route0 = 3; */
		route1 = 2;
		break;
	case DW_PCIE_DMA:
		/* route0 = 3; */
		route1 = 3;
		break;
	default:
		con_err("dw_pcie: Other types is not supported\n");
		return 0xffffffff;
	}

	/* addr = (route0 << 31) | (route1 << 17) | (uint64_t)reg; */
	addr = (route1 << 17) | reg;
	return addr;
}
