#ifdef SDFIRM
#include <driver/pcie_designware.h>
#else
#include "pcie_designware.h"
#endif

#ifdef IPBENCH
void axi_read_c(uint64_t addr, uint32_t *data, int port);
void axi_write_c(uint64_t addr, uint32_t data, int port);
#else
uint32_t axi_read_c(uint64_t addr, uint8_t port)
{
    return 0;
}

void axi_write_c(uint64_t addr, uint32_t val, uint8_t port)
{
    printf("WriteAXI: addr: 0x%08x, data: 0x%08x, port: %d\n", addr, val, port);
}
#endif


static uint64_t format_dbi_addr(enum dw_pcie_access_type type, uint32_t reg)
{
    //uint64_t route0, route1, addr;
    uint32_t route1, addr;

    //assert(reg & 0x3 == 0);
 
    switch (type) {
    case DW_PCIE_CDM:
        //route0 = 0;
        route1 = 0;
        break;
    case DW_PCIE_SHADOW:
        //route0 = 1;
        route1 = 1;
        break;
    case DW_PCIE_ATU:
        //route0 = 3;
        route1 = 2;
        break;
    case DW_PCIE_DMA:
        //route0 = 3;
        route1 = 3;
        break;
    default:
        dev_err(pci->dev, "Other types is not supported\n");
        return 0xffffffff;
    }

    //addr = (route0 << 31) | (route1 << 17) | (uint64_t)reg;
    addr = (route1 << 17) | reg;
    return addr;
}

static uint32_t read_dbi(struct dw_pcie *pci, enum dw_pcie_access_type type, uint32_t reg)
{
    uint32_t val;
    uint64_t dbi_offset;
    
    dbi_offset = format_dbi_addr(type, reg);
#ifdef IPBENCH
    axi_read_c((pci->dbi_base + dbi_offset), &val, pci->axi_dbi_port);
#else
    val = readl(pci->dbi_base + dbi_offset);
#endif

	return val;
}

uint32_t readl_dbi(struct dw_pcie *pci, enum dw_pcie_access_type type, uint32_t reg)
{
    uint32_t val;

    val = read_dbi(pci, type, reg);

    return val;
}

uint16_t readw_dbi(struct dw_pcie *pci, enum dw_pcie_access_type type, uint16_t reg)
{
    uint32_t val;
    uint8_t shift;

    shift = (reg & 0x2);
    val = read_dbi(pci, type, reg);
    val = val >> (8 * shift);

    return (uint16_t)val;
}

uint8_t readb_dbi(struct dw_pcie *pci, enum dw_pcie_access_type type, uint8_t reg)
{
    uint32_t val;
    uint8_t shift;

    shift = (reg & 0x3);
    val = read_dbi(pci, type, reg);
    val = val >> (8 * shift);

    return (uint8_t)val;
}

uint32_t dw_pcie_read_dbi(struct dw_pcie *pci, enum dw_pcie_access_type type, uint32_t reg, size_t size)
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
        val = readl_dbi(pci, type, reg);
    }

    return val;
}

static void write_dbi(struct dw_pcie *pci, enum dw_pcie_access_type type, uint32_t reg, uint32_t val)
{
    uint64_t dbi_offset;

    dbi_offset = format_dbi_addr(type, reg);

#ifdef IPBENCH
    axi_write_c(pci->dbi_base + dbi_offset, val, pci->axi_dbi_port);
#else
    printf("AXIWrite: Addr: 0x%llx, Data: 0x%x\n", pci->dbi_base + dbi_offset, val);
    writel(pci->dbi_base + dbi_offset, val);
#endif

}

void dw_pcie_writel_dbi(struct dw_pcie *pci, enum dw_pcie_access_type type, uint32_t reg, uint32_t val)
{
    write_dbi(pci, type, reg, val);
}

void dw_pcie_writeb_dbi(struct dw_pcie *pci, enum dw_pcie_access_type type, uint32_t reg, uint8_t val)
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

void dw_pcie_writew_dbi(struct dw_pcie *pci, enum dw_pcie_access_type type, uint32_t reg, uint16_t val)
{
    uint32_t tmp;
    uint8_t shift;

    shift = (reg & 0x2);
    tmp = readl_dbi(pci, type, (reg & 0xfffffffc));
    tmp = (tmp & (0xffff0000 >> (shift * 8))) | (val << (shift * 8));

    write_dbi(pci, type, (reg & 0xfffffffc), tmp);
}

void dw_pcie_write_dbi(struct dw_pcie *pci, enum dw_pcie_access_type type, uint32_t addr, uint32_t val, size_t size)
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
    }
}

uint32_t dw_pcie_read_atu(struct dw_pcie *pci, enum dw_pcie_region_type region, uint32_t index, uint32_t reg, size_t size)
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
        dev_err(pci->dev, "Wrong Inbound/Outbound input\n");
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
        dev_err(pci->dev, "Wrong Inbound/Outbound input\n");
        return;
    }

    addr = ((index << 9) | (dir << 8) | reg);
    dw_pcie_write_dbi(pci, acc_type, addr, val, size);
}

/*
static uint32_t dw_pcie_readl_ob_unroll(struct dw_pcie *pci, uint32_t index, uint32_t reg)
{
	uint32_t offset = PCIE_GET_ATU_OUTB_UNR_REG_OFFSET(index);

	return dw_pcie_readl_atu(pci, offset + reg);
}
*/

/*
static void dw_pcie_writel_ob_unroll(struct dw_pcie *pci, uint32_t index, uint32_t reg,
				     uint32_t val)
{
	uint32_t offset = PCIE_GET_ATU_OUTB_UNR_REG_OFFSET(index);

	dw_pcie_writel_atu(pci, offset + reg, val);
}
*/

/*
static void dw_pcie_prog_outbound_atu_unroll(struct dw_pcie *pci, int index,
					     int type, uint64_t cpu_addr,
					     uint64_t pci_addr, uint32_t size)
{
	uint32_t retries, val;

	dw_pcie_writel_ob_unroll(pci, index, PCIE_ATU_UNR_LOWER_BASE,
				 lower_32_bits(cpu_addr));
	dw_pcie_writel_ob_unroll(pci, index, PCIE_ATU_UNR_UPPER_BASE,
				 upper_32_bits(cpu_addr));
	dw_pcie_writel_ob_unroll(pci, index, PCIE_ATU_UNR_LIMIT,
				 lower_32_bits(cpu_addr + size - 1));
	dw_pcie_writel_ob_unroll(pci, index, PCIE_ATU_UNR_LOWER_TARGET,
				 lower_32_bits(pci_addr));
	dw_pcie_writel_ob_unroll(pci, index, PCIE_ATU_UNR_UPPER_TARGET,
				 upper_32_bits(pci_addr));
	dw_pcie_writel_ob_unroll(pci, index, PCIE_ATU_UNR_REGION_CTRL1,
				 type);
	dw_pcie_writel_ob_unroll(pci, index, PCIE_ATU_UNR_REGION_CTRL2,
				 PCIE_ATU_ENABLE);

	*
	 * Make sure ATU enable takes effect before any subsequent config
	 * and I/O accesses.
	 *
	for (retries = 0; retries < LINK_WAIT_MAX_IATU_RETRIES; retries++) {
		val = dw_pcie_readl_ob_unroll(pci, index,
					      PCIE_ATU_UNR_REGION_CTRL2);
		if (val & PCIE_ATU_ENABLE)
			return;

		mdelay(LINK_WAIT_IATU);
	}
	dev_err(pci->dev, "Outbound iATU is not being enabled\n");
}
*/

void dw_pcie_prog_outbound_atu(struct dw_pcie *pci, int index, int type,
			       uint64_t cpu_addr, uint64_t pci_addr, uint64_t size)
{
	//uint32_t retries, val;

	dw_pcie_write_atu(pci, DW_PCIE_REGION_OUTBOUND, index,
                PCIE_IATU_LWR_BASE_ADDR_OFF_OUTBOUND, 0x4, lower_32_bits(cpu_addr));
    dw_pcie_write_atu(pci, DW_PCIE_REGION_OUTBOUND, index,
                PCIE_IATU_UPPER_BASE_ADDR_OFF_OUTBOUND, 0x4, upper_32_bits(cpu_addr));
    dw_pcie_write_atu(pci, DW_PCIE_REGION_OUTBOUND, index,
                PCIE_IATU_LWR_LIMIT_ADDR_OFF_OUTBOUND, 0x4, lower_32_bits(cpu_addr + size -1));
    dw_pcie_write_atu(pci, DW_PCIE_REGION_OUTBOUND, index,
                PCIE_IATU_UPPER_LIMIT_ADDR_OFF_OUTBOUND, 0x4, upper_32_bits(cpu_addr + size -1));
    dw_pcie_write_atu(pci, DW_PCIE_REGION_OUTBOUND, index,
                PCIE_IATU_LWR_TARGET_ADDR_OFF_OUTBOUND, 0x4, lower_32_bits(pci_addr));
    dw_pcie_write_atu(pci, DW_PCIE_REGION_OUTBOUND, index,
                PCIE_IATU_UPPER_TARGET_ADDR_OFF_OUTBOUND, 0x4, upper_32_bits(pci_addr));
	dw_pcie_write_atu(pci, DW_PCIE_REGION_OUTBOUND, index,
                PCIE_IATU_REGION_CTRL1_OFF_OUTBOUND, 0x4, type);
	dw_pcie_write_atu(pci, DW_PCIE_REGION_OUTBOUND, index,
                PCIE_IATU_REGION_CTRL2_OFF_OUTBOUND, 0x4, PCIE_ATU_ENABLE);

	/*
	 * Make sure ATU enable takes effect before any subsequent config
	 * and I/O accesses.
	 */
    /*
	for (retries = 0; retries < LINK_WAIT_MAX_IATU_RETRIES; retries++) {
		val = dw_pcie_readl_dbi(pci, PCIE_ATU_CR2);
		if (val & PCIE_ATU_ENABLE)
			return;

		mdelay(LINK_WAIT_IATU);
	}
	dev_err(pci->dev, "Outbound iATU is not being enabled\n");
    */
}

/*
static uint32_t dw_pcie_readl_ib_unroll(struct dw_pcie *pci, uint32_t index, uint32_t reg)
{
	uint32_t offset = PCIE_GET_ATU_INB_UNR_REG_OFFSET(index);

	return dw_pcie_readl_atu(pci, offset + reg);
}
*/
/*
static void dw_pcie_writel_ib_unroll(struct dw_pcie *pci, uint32_t index, uint32_t reg,
				     uint32_t val)
{
	uint32_t offset = PCIE_GET_ATU_INB_UNR_REG_OFFSET(index);

	dw_pcie_writel_atu(pci, offset + reg, val);
}
*/
/*
static int dw_pcie_prog_inbound_atu_unroll(struct dw_pcie *pci, int index,
					   int bar, uint64_t cpu_addr,
					   enum dw_pcie_as_type as_type)
{
	int type;
	uint32_t retries, val;

	dw_pcie_writel_ib_unroll(pci, index, PCIE_ATU_UNR_LOWER_TARGET,
				 lower_32_bits(cpu_addr));
	dw_pcie_writel_ib_unroll(pci, index, PCIE_ATU_UNR_UPPER_TARGET,
				 upper_32_bits(cpu_addr));

	switch (as_type) {
	case DW_PCIE_AS_MEM:
		type = PCIE_ATU_TYPE_MEM;
		break;
	case DW_PCIE_AS_IO:
		type = PCIE_ATU_TYPE_IO;
		break;
	default:
		return -EINVAL;
	}

	dw_pcie_writel_ib_unroll(pci, index, PCIE_ATU_UNR_REGION_CTRL1, type);
	dw_pcie_writel_ib_unroll(pci, index, PCIE_ATU_UNR_REGION_CTRL2,
				 PCIE_ATU_ENABLE |
				 PCIE_ATU_BAR_MODE_ENABLE | (bar << 8));

	*
	 * Make sure ATU enable takes effect before any subsequent config
	 * and I/O accesses.
	 *
	for (retries = 0; retries < LINK_WAIT_MAX_IATU_RETRIES; retries++) {
		val = dw_pcie_readl_ib_unroll(pci, index,
					      PCIE_ATU_UNR_REGION_CTRL2);
		if (val & PCIE_ATU_ENABLE)
			return 0;

		mdelay(LINK_WAIT_IATU);
	}
	dev_err(pci->dev, "Inbound iATU is not being enabled\n");

	return -EBUSY;
}
*/

/*
int dw_pcie_prog_inbound_atu(struct dw_pcie *pci, int index, int bar,
			     uint64_t cpu_addr, enum dw_pcie_as_type as_type)
{
	int type;
	uint32_t retries, val;

	if (pci->iatu_unroll_enabled)
		return dw_pcie_prog_inbound_atu_unroll(pci, index, bar,
						       cpu_addr, as_type);

	dw_pcie_writel_dbi(pci, PCIE_ATU_VIEWPORT, PCIE_ATU_REGION_INBOUND |
			   index);
	dw_pcie_writel_dbi(pci, PCIE_ATU_LOWER_TARGET, lower_32_bits(cpu_addr));
	dw_pcie_writel_dbi(pci, PCIE_ATU_UPPER_TARGET, upper_32_bits(cpu_addr));

	switch (as_type) {
	case DW_PCIE_AS_MEM:
		type = PCIE_ATU_TYPE_MEM;
		break;
	case DW_PCIE_AS_IO:
		type = PCIE_ATU_TYPE_IO;
		break;
	default:
		return -EINVAL;
	}

	dw_pcie_writel_dbi(pci, PCIE_ATU_CR1, type);
	dw_pcie_writel_dbi(pci, PCIE_ATU_CR2, PCIE_ATU_ENABLE
			   | PCIE_ATU_BAR_MODE_ENABLE | (bar << 8));

	*
	 * Make sure ATU enable takes effect before any subsequent config
	 * and I/O accesses.
	 *
	for (retries = 0; retries < LINK_WAIT_MAX_IATU_RETRIES; retries++) {
		val = dw_pcie_readl_dbi(pci, PCIE_ATU_CR2);
		if (val & PCIE_ATU_ENABLE)
			return 0;

		mdelay(LINK_WAIT_IATU);
	}
	dev_err(pci->dev, "Inbound iATU is not being enabled\n");

	return -EBUSY;
}
*/

/*
static uint8_t dw_pcie_iatu_unroll_enabled(struct dw_pcie *pci)
{
	uint32_t val;

	val = dw_pcie_readl_dbi(pci, PCIE_ATU_VIEWPORT);
	if (val == 0xffffffff)
		return 1;

	return 0;
}
*/

/*
void dw_pcie_disable_atu(struct dw_pcie *pci, int index,
			 enum dw_pcie_region_type type)
{
	int region;

	switch (type) {
	case DW_PCIE_REGION_INBOUND:
		region = PCIE_ATU_REGION_INBOUND;
		break;
	case DW_PCIE_REGION_OUTBOUND:
		region = PCIE_ATU_REGION_OUTBOUND;
		break;
	default:
		return;
	}

	dw_pcie_writel_dbi(pci, PCIE_ATU_VIEWPORT, region | index);
	dw_pcie_writel_dbi(pci, PCIE_ATU_CR2, (uint32_t)~PCIE_ATU_ENABLE);
}
*/

static void dw_pcie_rd_own_conf(struct pcie_port *pp, int where, int size,
			       uint32_t *val)
{
	struct dw_pcie *pci;

	//if (pp->ops->rd_own_conf)
	//	return pp->ops->rd_own_conf(pp, where, size, val);

	pci = to_dw_pcie_from_pp(pp);
	*val = dw_pcie_read_dbi(pci, DW_PCIE_CDM, where, size);
}

static void dw_pcie_wr_own_conf(struct pcie_port *pp, int where, int size,
			       uint32_t val)
{
	struct dw_pcie *pci;

	//if (pp->ops->wr_own_conf)
	//	return pp->ops->wr_own_conf(pp, where, size, val);

	pci = to_dw_pcie_from_pp(pp);
	dw_pcie_write_dbi(pci, DW_PCIE_CDM, where, val, size);
}

/*
int dw_pcie_link_up(struct dw_pcie *pci)
{
	uint32_t val;

	if (pci->ops->link_up)
		return pci->ops->link_up(pci);

	val = readl(pci->dbi_base + PCIE_PORT_DEBUG1);
	return ((val & PCIE_PORT_DEBUG1_LINK_UP) &&
		(!(val & PCIE_PORT_DEBUG1_LINK_IN_TRAINING)));
}
*/
/*
int dw_pcie_wait_for_link(struct dw_pcie *pci)
{
	int retries;

	* Check if the link is up or not *
	for (retries = 0; retries < LINK_WAIT_MAX_RETRIES; retries++) {
		if (dw_pcie_link_up(pci)) {
			dev_info(pci->dev, "Link up\n");
			return 0;
		}
		usleep_range(LINK_WAIT_USLEEP_MIN, LINK_WAIT_USLEEP_MAX);
	}

	dev_info(pci->dev, "Phy link never came up\n");

	return -ETIMEDOUT;
}
*/

/*
int post_platform_init(struct dw_pcie *pci)
{
	if (dw_pcie_link_up(pci))
		return 0;

	* Enable Link Training state machine *
	//if (pcie->ops->ltssm_enable)    //qcom_pcie_2_3_2_ltssm_enable
	//	pcie->ops->ltssm_enable(pcie);

	return dw_pcie_wait_for_link(pci);

}
*/
void dw_pcie_setup(struct dw_pcie *pci)
{
	uint32_t val, tmp;
	uint32_t lanes = pci->lane_num;
    uint8_t order = pci->order;

    /* Conifg Fast Link Scale Factor is 64(16us) */
    val = dw_pcie_read_dbi(pci, DW_PCIE_CDM, PCIE_TIMER_CTRL_MAX_FUN_NUM_OFF, 0x4);
    tmp = 0x2 << 29;
    val |= tmp;
    dw_pcie_write_dbi(pci, DW_PCIE_CDM, PCIE_TIMER_CTRL_MAX_FUN_NUM_OFF, val, 0x4);

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
	default:;
        // TODO: 
		//dev_err(pci->dev, "num-lanes %u: invalid value\n", lanes);
		//return;
	}

    val |= BIT(7); // set fast link mode
	dw_pcie_write_dbi(pci, DW_PCIE_CDM, PCIE_PORT_LINK_CONTROL, val, 0x4);

	/* Set link width speed control register */
	val = dw_pcie_read_dbi(pci, DW_PCIE_CDM, PCIE_LINK_WIDTH_SPEED_CONTROL, 0x4);
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
	dw_pcie_write_dbi(pci, DW_PCIE_CDM, PCIE_LINK_WIDTH_SPEED_CONTROL, val, 0x4);

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
/*
	if (of_property_read_bool(np, "snps,enable-cdm-check")) {
		val = dw_pcie_readl_dbi(pci, PCIE_PL_CHK_REG_CONTROL_STATUS);
		val |= PCIE_PL_CHK_REG_CHK_REG_CONTINUOUS |
		       PCIE_PL_CHK_REG_CHK_REG_START;
		dw_pcie_writel_dbi(pci, PCIE_PL_CHK_REG_CONTROL_STATUS, val);
	}
*/
}


void dw_pcie_setup_rc(struct pcie_port *pp)
{
	uint32_t val;
	struct dw_pcie *pci = to_dw_pcie_from_pp(pp);

    dw_pcie_dbi_ro_wr_en(pci);

    dw_pcie_setup(pci);

	/* Setup RC BARs */
	dw_pcie_write_dbi(pci, DW_PCIE_CDM, PCI_BASE_ADDRESS_0, 0x00000004, 0x4);
	dw_pcie_write_dbi(pci, DW_PCIE_CDM, PCI_BASE_ADDRESS_1, 0x00000000, 0x4);

	/* Setup interrupt pins */
	val = dw_pcie_read_dbi(pci, DW_PCIE_CDM, PCI_INTERRUPT_LINE, 0x4);
	val &= 0xffff00ff;
	val |= 0x00000100;
	dw_pcie_write_dbi(pci, DW_PCIE_CDM, PCI_INTERRUPT_LINE, val, 0x4);

	/* Setup bus numbers */
	val = dw_pcie_read_dbi(pci, DW_PCIE_CDM, PCI_PRIMARY_BUS, 0x4);
	val &= 0xff000000;
	val |= 0x00ff0100;
	dw_pcie_write_dbi(pci, DW_PCIE_CDM, PCI_PRIMARY_BUS, val, 0x4);

	/* Setup command register */
	val = dw_pcie_read_dbi(pci, DW_PCIE_CDM, PCI_COMMAND, 0x4);
	val &= 0xffff0000;
	val |= PCI_COMMAND_IO | PCI_COMMAND_MEMORY |
		PCI_COMMAND_MASTER | PCI_COMMAND_SERR;
	dw_pcie_write_dbi(pci, DW_PCIE_CDM, PCI_COMMAND, val, 0x4);

	/*
	 * If the platform provides ->rd_other_conf, it means the platform
	 * uses its own address translation component rather than ATU, so
	 * we should not program the ATU here.
	 */
/*    
	if (!pp->ops->rd_other_conf) {
		dw_pcie_prog_outbound_atu(pci, PCIE_ATU_REGION_INDEX0,
					  PCIE_ATU_TYPE_MEM, pp->mem_base,
					  pp->mem_bus_addr, pp->mem_size);
		if (pci->num_viewport > 2)
			dw_pcie_prog_outbound_atu(pci, PCIE_ATU_REGION_INDEX2,
						  PCIE_ATU_TYPE_IO, pp->io_base,
						  pp->io_bus_addr, pp->io_size);
	}
*/

    //dw_pcie_prog_outbound_atu(pci, 0, PCIE_ATU_TYPE_IO, 0xd000000080001000, 0x1a00000000120000, 0x80ff0000);
    dw_pcie_prog_outbound_atu(pci, 0, PCIE_ATU_TYPE_CFG1, pp->cfg_bar1, 0x1000000000000000, 0x80000);
    dw_pcie_prog_outbound_atu(pci, 1, PCIE_ATU_TYPE_CFG0, pp->cfg_bar0, 0x1100000000000000, 0x80000);
    dw_pcie_prog_outbound_atu(pci, 2, PCIE_ATU_TYPE_MEM, pp->mem_base, 0x2000000000000000, pp->mem_size);
	dw_pcie_wr_own_conf(pp, PCI_BASE_ADDRESS_0, 4, 0);

	/* Program correct class for RC */
	dw_pcie_wr_own_conf(pp, PCI_CLASS_DEVICE, 2, PCI_CLASS_BRIDGE_PCI);

	dw_pcie_rd_own_conf(pp, PCIE_LINK_WIDTH_SPEED_CONTROL, 4, &val);
	val |= PORT_LOGIC_SPEED_CHANGE;
	dw_pcie_wr_own_conf(pp, PCIE_LINK_WIDTH_SPEED_CONTROL, 4, val);

	dw_pcie_dbi_ro_wr_dis(pci);
}


int pre_platform_init(void)
{
    // configure clock and resets
    
    // phy_power_on
    return 0;
}


int dw_pcie_host_init(struct pcie_port *pp)
{
	//struct dw_pcie *pci = to_dw_pcie_from_pp(pp);

    pre_platform_init();// pcie_init_2_3_3

    dw_pcie_setup_rc(pp);

    //post_platform_init(pci); // pcie_establish_link

    return 0;
}


int pcie_init(struct dw_pcie *pci)
{
    //phy_init();
    
    dw_pcie_host_init(&pci->pp);

    //bus_enumerate();

    return 0;
}
