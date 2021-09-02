#include <target/pci.h>
#ifdef SDFIRM
#include <target/console.h>
#endif

uint32_t form_pci_addr(int bus, int dev, int fun)
{
	return ((bus << 20) | (dev << 15) | (fun << 12));
	/* return ((bus << 24) | (dev << 19) | (fun << 16)); */
}

static uint64_t format_dbi_addr(enum dw_pcie_access_type type, uint32_t reg)
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

static uint32_t read_dbi(struct dw_pcie *pci, enum dw_pcie_access_type type,
			 uint32_t reg)
{
	uint64_t dbi_offset;

	dbi_offset = format_dbi_addr(type, reg);
	return dw_pcie_read_axi(pci->dbi_base + dbi_offset);
}

uint32_t readl_dbi(struct dw_pcie *pci, enum dw_pcie_access_type type,
		   uint32_t reg)
{
	return read_dbi(pci, type, reg);
}

uint16_t readw_dbi(struct dw_pcie *pci, enum dw_pcie_access_type type,
		   uint16_t reg)
{
	uint32_t val;
	uint8_t shift;

	shift = (reg & 0x2);
	val = read_dbi(pci, type, reg);
	val = val >> (8 * shift);

	return (uint16_t)val;
}

uint8_t readb_dbi(struct dw_pcie *pci, enum dw_pcie_access_type type,
		  uint8_t reg)
{
	uint32_t val;
	uint8_t shift;

	shift = (reg & 0x3);
	val = read_dbi(pci, type, reg);
	val = val >> (8 * shift);

	return (uint8_t)val;
}

uint32_t dw_pcie_read_dbi(struct dw_pcie *pci, enum dw_pcie_access_type type,
			  uint32_t reg, size_t size)
{
	uint32_t val;

	switch (size) {
	case 1:
		val = readb_dbi(pci, type, reg);
		break;
	case 2:
		val = readw_dbi(pci, type, reg);
		break;
	case 4:
	default:
		val = readl_dbi(pci, type, reg);
		break;
	}

	return val;
}

static void write_dbi(struct dw_pcie *pci, enum dw_pcie_access_type type,
		      uint32_t reg, uint32_t val)
{
	uint64_t dbi_offset;

	dbi_offset = format_dbi_addr(type, reg);
	dw_pcie_write_axi(pci->dbi_base + dbi_offset, val);
}

void dw_pcie_writel_dbi(struct dw_pcie *pci, enum dw_pcie_access_type type,
			uint32_t reg, uint32_t val)
{
	write_dbi(pci, type, reg, val);
}

void dw_pcie_writeb_dbi(struct dw_pcie *pci, enum dw_pcie_access_type type,
			uint32_t reg, uint8_t val)
{
	uint32_t tmp, mask = 0xff;
	uint8_t shift;

	shift = (reg & 0x3);
	tmp = readl_dbi(pci, type, (reg & 0xfffffffc));
	mask = mask << (8 * shift);
	mask = ~mask;
	tmp = ((tmp & mask) | val << (8 * shift));

	write_dbi(pci, type, (reg & 0xfffffffc), tmp);
}

void dw_pcie_writew_dbi(struct dw_pcie *pci, enum dw_pcie_access_type type,
			uint32_t reg, uint16_t val)
{
	uint32_t tmp;
	uint8_t shift;

	shift = (reg & 0x2);
	tmp = readl_dbi(pci, type, (reg & 0xfffffffc));
	tmp = (tmp & (0xffff0000 >> (shift * 8))) | (val << (shift * 8));

	write_dbi(pci, type, (reg & 0xfffffffc), tmp);
}

void dw_pcie_write_dbi(struct dw_pcie *pci, enum dw_pcie_access_type type,
		       uint32_t addr, uint32_t val, size_t size)
{
	switch (size) {
	case 1:
		dw_pcie_writeb_dbi(pci, type, addr, (uint8_t)val);
		break;
	case 2:
		dw_pcie_writew_dbi(pci, type, addr, (uint16_t)val);
		break;
	case 4:
		dw_pcie_writel_dbi(pci, type, addr, val);
		break;
	}
}

uint32_t dw_pcie_read_atu(struct dw_pcie *pci, enum dw_pcie_region_type region,
			  uint32_t index, uint32_t reg, size_t size)
{
	uint32_t addr, dir;
	enum dw_pcie_access_type acc_type = DW_PCIE_ATU;

	switch (region) {
	case DW_PCIE_REGION_INBOUND:
		dir = 1;
		break;
	case DW_PCIE_REGION_OUTBOUND:
		dir = 0;
		break;
	default:
		con_err("dw_pcie: Wrong Inbound/Outbound input\n");
		return -1;
	}

	addr = ((index << 9) | (dir << 8) | reg);
	return dw_pcie_read_dbi(pci, acc_type, addr, size);
}

void dw_pcie_write_atu(struct dw_pcie *pci, enum dw_pcie_region_type region,
		       uint32_t index, uint32_t reg, size_t size,  uint32_t val)
{
	uint32_t addr, dir;
	enum dw_pcie_access_type acc_type = DW_PCIE_ATU;

	switch (region) {
	case DW_PCIE_REGION_INBOUND:
		dir = 1;
		break;
	case DW_PCIE_REGION_OUTBOUND:
		dir = 0;
		break;
	default:
		con_err("dw_pcie: Wrong Inbound/Outbound input\n");
		return;
	}

	addr = ((index << 9) | (dir << 8) | reg);
	dw_pcie_write_dbi(pci, acc_type, addr, val, size);
}

void dw_pcie_prog_outbound_atu(struct dw_pcie *pci, int index, int type,
		uint64_t cpu_addr, uint64_t pci_addr, uint64_t size)
{
	uint32_t val;

	dw_pcie_write_atu(pci, DW_PCIE_REGION_OUTBOUND, index,
			  PCIE_IATU_LWR_BASE_ADDR_OFF_OUTBOUND, 0x4,
			  lower_32_bits(cpu_addr));
	dw_pcie_write_atu(pci, DW_PCIE_REGION_OUTBOUND, index,
			  PCIE_IATU_UPPER_BASE_ADDR_OFF_OUTBOUND, 0x4,
			  upper_32_bits(cpu_addr));
	dw_pcie_write_atu(pci, DW_PCIE_REGION_OUTBOUND, index,
			  PCIE_IATU_LWR_LIMIT_ADDR_OFF_OUTBOUND, 0x4,
			  lower_32_bits(cpu_addr + size - 1));
	dw_pcie_write_atu(pci, DW_PCIE_REGION_OUTBOUND, index,
			  PCIE_IATU_UPPER_LIMIT_ADDR_OFF_OUTBOUND, 0x4,
			  upper_32_bits(cpu_addr + size - 1));
	dw_pcie_write_atu(pci, DW_PCIE_REGION_OUTBOUND, index,
			  PCIE_IATU_LWR_TARGET_ADDR_OFF_OUTBOUND, 0x4,
			  lower_32_bits(pci_addr));
	dw_pcie_write_atu(pci, DW_PCIE_REGION_OUTBOUND, index,
			  PCIE_IATU_UPPER_TARGET_ADDR_OFF_OUTBOUND, 0x4,
			  upper_32_bits(pci_addr));
	dw_pcie_write_atu(pci, DW_PCIE_REGION_OUTBOUND, index,
			  PCIE_IATU_REGION_CTRL1_OFF_OUTBOUND, 0x4, type);
#if 1
/* #ifdef CONFIG_DW_PCIE_RC */
	val = PCIE_ATU_ENABLE;
	if ((type == PCIE_ATU_TYPE_CFG0) || (type == PCIE_ATU_TYPE_CFG1))
		val |= _BV(28);
	dw_pcie_write_atu(pci, DW_PCIE_REGION_OUTBOUND, index,
			  PCIE_IATU_REGION_CTRL2_OFF_OUTBOUND, 0x4, val);
#else
	/* for EP: enable regine & bypass DMA request */
	dw_pcie_write_atu(pci, DW_PCIE_REGION_OUTBOUND, index,
			  PCIE_IATU_REGION_CTRL2_OFF_OUTBOUND, 0x4, 0x88);
#endif
#if 0
	/* Make sure ATU enable takes effect before any subsequent config
	 * and I/O accesses.
	 */
	for (retries = 0; retries < LINK_WAIT_MAX_IATU_RETRIES; retries++) {
		val = dw_pcie_readl_dbi(pci, PCIE_ATU_CR2);
		if (val & PCIE_ATU_ENABLE)
			return;
		mdelay(LINK_WAIT_IATU);
	}
	con_err("dw_pcie: Outbound iATU is not being enabled\n");
#endif
}

static void dw_pcie_rd_own_conf(struct pcie_port *pp, int where, int size,
				uint32_t *val)
{
	struct dw_pcie *pci;

	pci = to_dw_pcie_from_pp(pp);
	*val = dw_pcie_read_dbi(pci, DW_PCIE_CDM, where, size);
}

static void dw_pcie_wr_own_conf(struct pcie_port *pp, int where, int size,
				uint32_t val)
{
	struct dw_pcie *pci;

	pci = to_dw_pcie_from_pp(pp);
	dw_pcie_write_dbi(pci, DW_PCIE_CDM, where, val, size);
}

#if 0
int dw_pcie_link_up(struct dw_pcie *pci)
{
	uint32_t val;

	if (pci->ops->link_up)
		return pci->ops->link_up(pci);

	val = readl(pci->dbi_base + PCIE_PORT_DEBUG1);
	return ((val & PCIE_PORT_DEBUG1_LINK_UP) &&
		(!(val & PCIE_PORT_DEBUG1_LINK_IN_TRAINING)));
}

int dw_pcie_wait_for_link(struct dw_pcie *pci)
{
	int retries;

	/* Check if the link is up or not */
	for (retries = 0; retries < LINK_WAIT_MAX_RETRIES; retries++) {
		if (dw_pcie_link_up(pci)) {
			con_log("dw_pcie: Link up\n");
			return 0;
		}
		usleep_range(LINK_WAIT_USLEEP_MIN, LINK_WAIT_USLEEP_MAX);
	}

	con_err("dw_pcie: Phy link never came up\n");
	return -ETIMEDOUT;
}

int post_platform_init(struct dw_pcie *pci)
{
	if (dw_pcie_link_up(pci))
		return 0;

#if 0
	/* Enable Link Training state machine: e.x.,
	 * qcom_pcie_2_3_2_ltssm_enable().
	 */
	if (pcie->ops->ltssm_enable)
		pcie->ops->ltssm_enable(pcie);
#endif

	return dw_pcie_wait_for_link(pci);
}
#endif

void dw_pcie_setup(struct dw_pcie *pci)
{
	uint32_t val, tmp;
	uint32_t lanes = pci->lane_num;
	uint8_t order = pci->order;

#if 0
	/* Test: disable DLL feature exchange */
	dw_pcie_write_dbi(pci, DW_PCIE_CDM, 0x2d0, 0x0, 0x4);
#endif

	/* Conifg Fast Link Scale Factor is 64(16us) */
	val = dw_pcie_read_dbi(pci, DW_PCIE_CDM,
			       PCIE_TIMER_CTRL_MAX_FUN_NUM_OFF, 0x4);
	tmp = 0x2 << 29;
	val |= tmp;
	dw_pcie_write_dbi(pci, DW_PCIE_CDM,
			  PCIE_TIMER_CTRL_MAX_FUN_NUM_OFF, val, 0x4);

	/* Set the number of lanes */
	val = dw_pcie_read_dbi(pci, DW_PCIE_CDM, PCIE_PORT_LINK_CONTROL, 0x4);
	val &= ~PORT_LINK_MODE_MASK;
	switch (lanes) {
	case 4:
		val |= PORT_LINK_MODE_4_LANES;
		break;
	case 8:
		val |= PORT_LINK_MODE_8_LANES;
		break;
	case 16:
		val |= PORT_LINK_MODE_16_LANES;
		break;
	default:
		con_err("dw_pcie: num-lanes %u: invalid value\n", lanes);
		return;
	}

	/* Set fast link mode */
	val |= _BV(7);
	dw_pcie_write_dbi(pci, DW_PCIE_CDM,
			  PCIE_PORT_LINK_CONTROL, val, 0x4);

	/* Set link width speed control register */
	val = dw_pcie_read_dbi(pci, DW_PCIE_CDM,
			       PCIE_LINK_WIDTH_SPEED_CONTROL, 0x4);
	val &= ~PORT_LOGIC_LINK_WIDTH_MASK;
	switch (lanes) {
	case 4:
		val |= PORT_LOGIC_LINK_WIDTH_4_LANES;
		break;
	case 8:
		val |= PORT_LOGIC_LINK_WIDTH_8_LANES;
		break;
	case 16:
		val |= PORT_LOGIC_LINK_WIDTH_16_LANES;
		break;
	}
	dw_pcie_write_dbi(pci, DW_PCIE_CDM,
			  PCIE_LINK_WIDTH_SPEED_CONTROL, val, 0x4);

	val = dw_pcie_read_dbi(pci, DW_PCIE_CDM, PCIE_PORT_FORCE_OFF, 0x4);
	val &= ~PORT_LINK_NUM_MASK;
	val |= PORT_LINK_NUM(order);
	dw_pcie_write_dbi(pci, DW_PCIE_CDM, PCIE_PORT_FORCE_OFF, val, 0x4);

	val = dw_pcie_read_dbi(pci, DW_PCIE_CDM, PCIE_GEN3_RELATED_OFF, 0x4);
	val |= 0x200;
	val &= ~RATE_SHADOW_SEL_MASK;
	val |= RATE_SHADOW_SEL(0x0);
	dw_pcie_write_dbi(pci, DW_PCIE_CDM, PCIE_GEN3_RELATED_OFF, val, 0x4);

	val = dw_pcie_read_dbi(pci, DW_PCIE_CDM, PCIE_GEN3_RELATED_OFF, 0x4);
	val &= ~RATE_SHADOW_SEL_MASK;
	val |= RATE_SHADOW_SEL(0x1);
	dw_pcie_write_dbi(pci, DW_PCIE_CDM, PCIE_GEN3_RELATED_OFF, val, 0x4);

	val = dw_pcie_read_dbi(pci, DW_PCIE_CDM, PCIE_GEN3_RELATED_OFF, 0x4);
	val |= 0x200;
	val &= ~RATE_SHADOW_SEL_MASK;
	val |= RATE_SHADOW_SEL(0x1);
	dw_pcie_write_dbi(pci, DW_PCIE_CDM, PCIE_GEN3_RELATED_OFF, val, 0x4);
#if 0
	if (of_property_read_bool(np, "snps,enable-cdm-check")) {
		val = dw_pcie_readl_dbi(pci, PCIE_PL_CHK_REG_CONTROL_STATUS);
		val |= PCIE_PL_CHK_REG_CHK_REG_CONTINUOUS |
		       PCIE_PL_CHK_REG_CHK_REG_START;
		dw_pcie_writel_dbi(pci, PCIE_PL_CHK_REG_CONTROL_STATUS, val);
	}
#endif
}

void dw_pcie_setup_ctrl(struct pcie_port *pp)
{
	uint32_t val;
	struct dw_pcie *pci = to_dw_pcie_from_pp(pp);

	dw_pcie_dbi_ro_wr_en(pci);

	dw_pcie_setup(pci);

	if (pp->role == ROLE_RC) {
		/* Setup RC BARs */
		dw_pcie_write_dbi(pci, DW_PCIE_CDM,
				  PCI_BASE_ADDRESS_0, 0x00000004, 0x4);
		dw_pcie_write_dbi(pci, DW_PCIE_CDM,
				  PCI_BASE_ADDRESS_1, 0x00000000, 0x4);

		/* Setup interrupt pins */
		val = dw_pcie_read_dbi(pci, DW_PCIE_CDM,
				       PCI_INTERRUPT_LINE, 0x4);
		val &= 0xffff00ff;
		val |= 0x00000100;
		dw_pcie_write_dbi(pci, DW_PCIE_CDM,
				  PCI_INTERRUPT_LINE, val, 0x4);

		/* Setup bus numbers */
		val = dw_pcie_read_dbi(pci, DW_PCIE_CDM,
				       PCI_PRIMARY_BUS, 0x4);
		val &= 0xff000000;
		val |= 0x00ff0100;
		dw_pcie_write_dbi(pci, DW_PCIE_CDM,
				  PCI_PRIMARY_BUS, val, 0x4);

		/* Setup command register */
		val = dw_pcie_read_dbi(pci, DW_PCIE_CDM, PCI_COMMAND, 0x4);
		val &= 0xffff0000;
		val |= PCI_COMMAND_IO | PCI_COMMAND_MEMORY |
			PCI_COMMAND_MASTER | PCI_COMMAND_SERR;
		dw_pcie_write_dbi(pci, DW_PCIE_CDM, PCI_COMMAND, val, 0x4);

		dw_pcie_prog_outbound_atu(pci, 0, PCIE_ATU_TYPE_CFG0,
					  pp->cfg_bar0, 0x0, pp->cfg_size);
		dw_pcie_prog_outbound_atu(pci, 1, PCIE_ATU_TYPE_CFG1,
					  pp->cfg_bar1, 0x0, pp->cfg_size);
		dw_pcie_prog_outbound_atu(pci, 2, PCIE_ATU_TYPE_MEM,
					  pp->mem_base, 0x0, pp->mem_size);
		dw_pcie_wr_own_conf(pp, PCI_BASE_ADDRESS_0, 4, 0);

		/* Program correct class for RC */
		dw_pcie_wr_own_conf(pp, PCI_CLASS_DEVICE, 2,
				    PCI_CLASS_BRIDGE_PCI);
	} else {
		/* Choose Storage class for EP test seems reasonably */
		dw_pcie_wr_own_conf(pp, PCI_CLASS_DEVICE, 2,
				    PCI_CLASS_STORAGE_OTHER);
		dw_pcie_wr_own_conf(pp, PCI_BASE_ADDRESS_0, 4, 0);
		dw_pcie_rd_own_conf(pp, PCI_COMMAND, 4, &val);
		val |= 0x6;
		dw_pcie_wr_own_conf(pp, PCI_COMMAND, 4, val);
#if 0
		dw_pcie_prog_outbound_atu(pci, 0, PCIE_ATU_TYPE_MEM,
					  pp->mem_base, 0x100000, 0x100000);
#endif
	}

#ifdef CONFIG_DUOWEN_ZEBU
	if (pp->chiplink) {
		/* link_control2_link_status */
		val = dw_pcie_read_dbi(pci, DW_PCIE_CDM, 0xa0, 0x4);
		val &= 0xfffffff0;
		val |= 0x1;
		dw_pcie_write_dbi(pci, DW_PCIE_CDM, 0xa0, val, 0x4);
	}
#endif

	dw_pcie_rd_own_conf(pp, PCIE_LINK_WIDTH_SPEED_CONTROL, 4, &val);
	val |= PORT_LOGIC_SPEED_CHANGE;
	dw_pcie_wr_own_conf(pp, PCIE_LINK_WIDTH_SPEED_CONTROL, 4, val);
	dw_pcie_dbi_ro_wr_dis(pci);
}

void dw_pcie_enable_msi(struct pcie_port *pp)
{
	struct dw_pcie *pci = to_dw_pcie_from_pp(pp);

	dw_pcie_dbi_ro_wr_en(pci);
	/* Set MSI addr */
	dw_pcie_write_dbi(pci, DW_PCIE_CDM, 0x820, 0x80201100, 0x4);
	/* Enable MSI int */
	dw_pcie_write_dbi(pci, DW_PCIE_CDM, 0x828, 0x3, 0x4);
	dw_pcie_dbi_ro_wr_dis(pci);
}

#ifndef CONFIG_DW_PCIE_RC
void dw_pcie_ep_dma_test(struct pcie_port *pp)
{
	struct dw_pcie *pci = to_dw_pcie_from_pp(pp);

	/* config max payload size/read request size, 0x78? */
	dw_pcie_write_dbi(pci, DW_PCIE_CDM, 0x78, 0x5850, 0x4);

	/* set DMA Engine Enable register */
	dw_pcie_write_dbi(pci, DW_PCIE_DMA, 0xc, 0x1, 0x4);
	/* set DMA write interrupt mask register */
	dw_pcie_write_dbi(pci, DW_PCIE_DMA, 0x54, 0x0, 0x4);
	/* set DMA Channel control 1 register */
	dw_pcie_write_dbi(pci, DW_PCIE_DMA, 0x200, 0x4400008, 0x4);
	/* set DMA transfer size register */
	dw_pcie_write_dbi(pci, DW_PCIE_DMA, 0x208, 0x100, 0x4);
	/* set DMA SAR low register */
	dw_pcie_write_dbi(pci, DW_PCIE_DMA, 0x20c, 0x8000000, 0x4);
	/* set DMA SAR high register */
	dw_pcie_write_dbi(pci, DW_PCIE_DMA, 0x210, 0x0, 0x4);
	/* set DMA DAR low register */
	dw_pcie_write_dbi(pci, DW_PCIE_DMA, 0x214, 0x10000000, 0x4);
	/* set DMA DAR high register */
	dw_pcie_write_dbi(pci, DW_PCIE_DMA, 0x218, 0x0, 0x4);
	/* set DMA write doorbell register */
	dw_pcie_write_dbi(pci, DW_PCIE_DMA, 0x10, 0x0, 0x4);
}
#endif

int pre_platform_init(void)
{
	/* configure clock and resets
	 * phy_power_on
	 */
	return 0;
}

int dw_pcie_host_init(struct pcie_port *pp)
{
	pre_platform_init();
	dw_pcie_setup_ctrl(pp);
#if 0
	post_platform_init(pci);
#endif
	return 0;
}

int pcie_init(struct dw_pcie *pci)
{
#if 0
	pcie_phy_init();
#endif
	dw_pcie_host_init(&pci->pp);
	return 0;
}
