#include <target/pci.h>
#include <target/clk.h>
#include <target/irq.h>
#include <target/page.h>
#include <target/arch.h>

#ifndef CONFIG_SIMULATION
static int pcie_dma_bh_poll;
#endif
int huge_data_finished = 0;
int dma_irq_triggered = false;
struct duowen_pcie_subsystem pcie_subsystem;
uint64_t ep_rsved_page_addr;

struct dw_pcie controllers[] = {
	// X16
	{
		//.axi_dbi_port = AXI_DBI_PORT_X16,
		.dbi_base = CFG_AXI_CORE_X16,
		.pp.cfg_bar0 = PCIE_CORE_X16_CFG0_START,
		.pp.cfg_bar1 = PCIE_CORE_X16_CFG1_START,
		.pp.cfg_size = PCIE_CORE_CFG_SIZE,
		.pp.mem_base = 0,
		.pp.mem_size = PCIE_CORE_MEM_SIZE,
	},

#ifndef CONFIG_DPU_GEN2
	// X8
	{
		//.axi_dbi_port = AXI_DBI_PORT_X8,
		.dbi_base = CFG_AXI_CORE_X8,
		.pp.cfg_bar0 = PCIE_CORE_X8_CFG0_START,
		.pp.cfg_bar1 = PCIE_CORE_X8_CFG1_START,
		.pp.cfg_size = PCIE_CORE_CFG_SIZE,
		.pp.mem_base = 0,
		.pp.mem_size = PCIE_CORE_MEM_SIZE,
	},

	// X4_0
	{
		//.axi_dbi_port = AXI_DBI_PORT_X4_0,
		.dbi_base = CFG_AXI_CORE_X4_0,
		.pp.cfg_bar0 = PCIE_CORE_X4_0_CFG0_START,
		.pp.cfg_bar1 = PCIE_CORE_X4_0_CFG1_START,
		.pp.cfg_size = PCIE_CORE_CFG_SIZE,
		.pp.mem_base = 0,
		.pp.mem_size = PCIE_CORE_MEM_SIZE,
	},

	//X4_1
	{
		//.axi_dbi_port = AXI_DBI_PORT_X4_1,
		.dbi_base = CFG_AXI_CORE_X4_1,
		.pp.cfg_bar0 = PCIE_CORE_X4_1_CFG0_START,
		.pp.cfg_bar1 = PCIE_CORE_X4_1_CFG1_START,
		.pp.cfg_size = PCIE_CORE_CFG_SIZE,
		.pp.mem_base = 0,
		.pp.mem_size = PCIE_CORE_MEM_SIZE,
	},
#endif /* CONFIG_DPU_GEN2 */
};

#ifndef CONFIG_DPU_PCIE_IPDV
uint32_t dw_pcie_read_apb(uint64_t addr)
{
	uint32_t data;

	data = __raw_readl(addr);
#ifdef CONFIG_DPU_PCIE_DEBUG
	con_dbg("dw_pcie: APB(R): 0x%llx=0x%08x\n", addr, data);
#endif
	return data;
}

void dw_pcie_write_apb(uint64_t addr, uint32_t data)
{
#ifdef CONFIG_DPU_PCIE_DEBUG
	con_dbg("dw_pcie: APB(W): 0x%llx=0x%x\n", addr, data);
#endif
	__raw_writel(data, addr);
}

uint32_t dw_pcie_read_axi(uint64_t addr)
{
	uint32_t data;

	data = __raw_readl(addr);
#ifdef CONFIG_DPU_PCIE_DEBUG
	con_dbg("dw_pcie: AXI(R): 0x%llx=0x%x\n", addr);
#endif
	return data;
}

void dw_pcie_write_axi(uint64_t addr, uint32_t data)
{
#ifdef CONFIG_DPU_PCIE_DEBUG
	con_dbg("dw_pcie: AXI(W): 0x%llx=0x%x\n", addr, data);
#endif
	__raw_writel(data, addr);
}
#endif

static uint8_t duowen_get_link_mode(struct duowen_pcie_subsystem *pcie_subsystem)
{
	return DEFAULT_LINK_MODE;
}

void reset_init(struct duowen_pcie_subsystem *pcie_subsystem)
{
	uint64_t base = pcie_subsystem->cfg_apb[SUBSYS];
	uint8_t mode = duowen_get_link_mode(pcie_subsystem);

	dw_pcie_write_apb((base + RESET_PHY), 0x00);
//	dw_pcie_write_apb((base + SRAM_CONTROL), 0x0);
	dw_pcie_write_apb((base + REFCLK_CONTROL), 0x3);//xkm use ref clk from pad
	// #200ns
	dw_pcie_write_apb((base + RESET_PHY), 0x1f);
	// #100ns

	if (mode == LINK_MODE_16_0_0_0)
		dw_pcie_write_apb((base + RESET_CORE_X16), 0xff);
	else if (mode == LINK_MODE_8_8_0_0)
		dw_pcie_write_apb((base + RESET_CORE_X8), 0xff);
	else if (mode == LINK_MODE_4_4_4_4)
		dw_pcie_write_apb((base + RESET_CORE_X4_0), 0xff);
	else
		dw_pcie_write_apb((base + RESET_CORE_X4_1), 0xff);
#if 0/*xkm: temply if0 here since rtl set it to fixed hz state*/
	while ((data & 0xf) != 0xf)
		data = dw_pcie_read_apb((base + SRAM_STATUS));
#endif
  //  dw_pcie_write_apb((base + SRAM_CONTROL), 0x55);//xkm:should be 0xaa,now don't set this register
	//dw_pcie_write_apb((base + RESET_PHY), 0xff);
}

static void subsys_link_init_pre(struct duowen_pcie_subsystem *pcie_subsystem)
{
	uint64_t base = pcie_subsystem->cfg_apb[SUBSYS];
	struct dw_pcie *controller = pcie_subsystem->controller;
	uint8_t link_mode = pcie_subsystem->link_mode;
	uint32_t val;

	//assert(link_mode != LINK_MODE_INVALID);

	// #10ns
	//dw_pcie_write_apb((base + SUBSYS_CONTROL), link_mode);//xkm: there is no 0x14 reg

	switch (link_mode) {
	case LINK_MODE_4_4_4_4:	 // 0: In DPU, X4_0
#ifdef DPU
		(controller + X16)->lane_num = 0;
		(controller + X8)->lane_num = 0;
		(controller + X4_0)->lane_num = 4;
		(controller + X4_1)->lane_num = 0;

		(controller + X16)->active = false;
		(controller + X8)->active = false;
		(controller + X4_0)->active = true;
		(controller + X4_1)->active = false;

		(controller + X16)->order = 0xff;
		(controller + X8)->order = 0xff;
		(controller + X4_0)->order = 0;
		(controller + X4_1)->order = 0xff;
#else
		(controller + X16)->lane_num = 4;
		(controller + X8)->lane_num = 4;
		(controller + X4_0)->lane_num = 4;
		(controller + X4_1)->lane_num = 4;

		(controller + X16)->active = true;
		(controller + X8)->active = true;
		(controller + X4_0)->active = true;
		(controller + X4_1)->active = true;

		(controller + X16)->order = 0;
		(controller + X8)->order = 1;
		(controller + X4_0)->order = 2;
		(controller + X4_1)->order = 3;
#endif
		base = pcie_subsystem->cfg_apb[X4_0];
		break;
	case LINK_MODE_8_4_0_4:	 // 1: In DPU, X4_1
#ifdef DPU
		(controller + X16)->lane_num = 0;
		(controller + X8)->lane_num = 0;
		(controller + X4_0)->lane_num = 0;
		(controller + X4_1)->lane_num = 4;

		(controller + X16)->active = false;
		(controller + X8)->active = false;
		(controller + X4_0)->active = false;
		(controller + X4_1)->active = true;

		(controller + X16)->order = 0xff;
		(controller + X8)->order = 0xff;
		(controller + X4_0)->order = 0xff;
		(controller + X4_1)->order = 0;
#else
		(controller + X16)->lane_num = 8;
		(controller + X8)->lane_num = 4;
		(controller + X4_0)->lane_num = 0;
		(controller + X4_1)->lane_num = 4;

		(controller + X16)->active = true;
		(controller + X8)->active = true;
		(controller + X4_0)->active = false;
		(controller + X4_1)->active = true;

		(controller + X16)->order = 0;
		(controller + X8)->order = 1;
		(controller + X4_0)->order = 0xff;
		(controller + X4_1)->order = 2;
#endif
		base = pcie_subsystem->cfg_apb[X4_1];
		break;
	case LINK_MODE_8_8_0_0:	 // 2: In DPU, X8
#ifdef DPU
		(controller + X16)->lane_num = 0;
		(controller + X8)->lane_num = 4;
		(controller + X4_0)->lane_num = 0;
		(controller + X4_1)->lane_num = 0;

		(controller + X16)->active = false;
		(controller + X8)->active = true;
		(controller + X4_0)->active = false;
		(controller + X4_1)->active = false;

		(controller + X16)->order = 0xff;
		(controller + X8)->order = 0;
		(controller + X4_0)->order = 0xff;
		(controller + X4_1)->order = 0xff;
#else
		(controller + X16)->lane_num = 8;
		(controller + X8)->lane_num = 8;
		(controller + X4_0)->lane_num = 0;
		(controller + X4_1)->lane_num = 0;

		(controller + X16)->active = true;
		(controller + X8)->active = true;
		(controller + X4_0)->active = false;
		(controller + X4_1)->active = false;

		(controller + X16)->order = 0;
		(controller + X8)->order = 1;
		(controller + X4_0)->order = 0xff;
		(controller + X4_1)->order = 0xff;
#endif
		base = pcie_subsystem->cfg_apb[X8];
		break;
	case LINK_MODE_16_0_0_0:	//  3: In DPU: X16
#ifdef DPU
		(controller + X16)->lane_num = 16;
		(controller + X8)->lane_num = 0;
		(controller + X4_0)->lane_num = 0;
		(controller + X4_1)->lane_num = 0;

		(controller + X16)->active = true;
		(controller + X8)->active = false;
		(controller + X4_0)->active = false;
		(controller + X4_1)->active = false;

		(controller + X16)->order = 0;
		(controller + X8)->order = 0xff;
		(controller + X4_0)->order = 0xff;
		(controller + X4_1)->order = 0xff;
#else
		(controller + X16)->lane_num = 16;
		(controller + X8)->lane_num = 0;
		(controller + X4_0)->lane_num = 0;
		(controller + X4_1)->lane_num = 0;

		(controller + X16)->active = true;
		(controller + X8)->active = false;
		(controller + X4_0)->active = false;
		(controller + X4_1)->active = false;

		(controller + X16)->order = 0;
		(controller + X8)->order = 0xff;
		(controller + X4_0)->order = 0xff;
		(controller + X4_1)->order = 0xff;
#endif
		base = pcie_subsystem->cfg_apb[X16];
		break;
	}

#ifdef CONFIG_DW_PCIE_RC
	dw_pcie_write_apb((base + 0), 0xc810010);
#else
	val = dw_pcie_read_apb((base + 0));
	printf("before setting=%x\n", val);//xkm:if no read this reg,the vcs will fail
	/* xkm:can't write this reg,otherwise will case error */
	/* dw_pcie_write_apb((base + 0), 0xc810000); */
#endif
}

void wait_controller_linkup(struct duowen_pcie_subsystem *pcie_subsys)
{
	uint8_t i = 0;
	uint32_t data;
	uint64_t base = pcie_subsys->cfg_apb[X16];

	data = dw_pcie_read_apb((base + 0x10));
	printf("wait controller smlh&rdlh ready\n");
	while ((data & (_BV(0) | _BV(11))) != (_BV(0) | _BV(11))) {
		data = dw_pcie_read_apb((base + 0x10));
		printf("count %d: link_status is:0x%x\n", i, data);
		i++;
		if (i > 100) {
			printf("controller failed to establish link up\n");
			break;
		}
	}

}

static void subsys_link_init_post(struct duowen_pcie_subsystem *pcie_subsys)
{
	uint8_t mode = duowen_get_link_mode(pcie_subsys);

	switch (mode) {
	case LINK_MODE_4_4_4_4:
		dw_pcie_write_apb(pcie_subsys->cfg_apb[X4_0], 0xc018010);
#ifdef DPU
		break;
#endif
	case LINK_MODE_8_4_0_4:
		dw_pcie_write_apb(pcie_subsys->cfg_apb[X4_1], 0xc018010);
#ifdef DPU
		break;
#endif
	case LINK_MODE_8_8_0_0:
		dw_pcie_write_apb(pcie_subsys->cfg_apb[X8], 0xc018010);
#ifdef DPU
		break;
#endif
	case LINK_MODE_16_0_0_0:
#ifdef CONFIG_DW_PCIE_RC
		dw_pcie_write_apb(pcie_subsys->cfg_apb[X16], 0xc018010);//xkm
#else
		dw_pcie_write_apb(pcie_subsys->cfg_apb[X16], 0xc018000);//xkm
#endif
#ifdef DPU
		break;
#endif
		break;
	}

	wait_controller_linkup(pcie_subsys);//xkm
}

void dw_controller_init(struct dw_pcie *pci)
{
}

void instance_subsystem(struct duowen_pcie_subsystem *pcie_subsystem)
{
	uint8_t i;

	memset(pcie_subsystem, 0, sizeof(*pcie_subsystem));

	pcie_subsystem->cfg_apb[X16] = CFG_APB_CORE_X16;
	pcie_subsystem->cfg_apb[X8] = CFG_APB_CORE_X8;
	pcie_subsystem->cfg_apb[X4_0] = CFG_APB_CORE_X4_0;
	pcie_subsystem->cfg_apb[X4_1] = CFG_APB_CORE_X4_1;
	pcie_subsystem->cfg_apb[SUBSYS] = CFG_APB_SUBSYS;

	pcie_subsystem->link_mode = duowen_get_link_mode(pcie_subsystem);

	pcie_subsystem->controller = &controllers[0];
	for (i = 0; i < sizeof(controllers) / sizeof(struct dw_pcie); i++)
		dw_controller_init(pcie_subsystem->controller + i);
}

void clock_init(void)
{
	clk_enable(pcie0_aclk);
	clk_enable(pcie0_pclk);
	clk_enable(pcie0_aux_clk);
	//clk_enable(pcie0_ref_clk);//xkm
	clk_enable(apb_clk);//xkm
	//__raw_writel(0x2, 0x401405c);

	clk_enable(srst_pcie0);
	/* still need this to give pcie power comes from apb_clk */
	clk_enable(srst_pcie0_por);
}

#ifdef CONFIG_DPU_PCIE_TEST
void dpu_pcie_handle_msi(bool en)
{
	uint8_t mode = duowen_get_link_mode(&pcie_subsystem);
	int val;
	uint64_t base;

	switch (mode) {
	case LINK_MODE_0:
		base = pcie_subsystem.cfg_apb[X4_1];
		break;
	case LINK_MODE_1:
		base = pcie_subsystem.cfg_apb[X4_0];
		break;
	case LINK_MODE_2:
		base = pcie_subsystem.cfg_apb[X8];
		break;
	case LINK_MODE_3:
	default:
		base = pcie_subsystem.cfg_apb[X16];
	}

	if (en)
		val = 0;
	else
		val = 1;

	// We just mask this interrupt, since there is no
	// register we can manipulate to clear the interrupt
	// from VIP
	__raw_writel(val, base + 0x44);
}

void dpu_pcie_handle_inta(bool en)
{
	if (en) // trigger assert INTA msg
		__raw_writel(0x2, TCSR_MSG_REG(0x80));
	else	// trigger de-assert INTA msg
		__raw_writel(0x4, TCSR_MSG_REG(0x80));
}

void dpu_pcie_handle_irq(int int_type)
{
	if (int_type == MSI_INT)
		dpu_pcie_handle_msi(0);
	else if (int_type == INTA_INT)
		dpu_pcie_handle_inta(0);
	else
		printf("Unknown IRQ number: 0x%x\n", int_type);
}

void dpu_pcie_msi_handler(irq_t irq)
{
	printf("bird: Receive PCIE MSI interrupt\n");
	irqc_mask_irq(IRQ_PCIE_X16_MSI);
	dpu_pcie_handle_irq(MSI_INT);
	irqc_unmask_irq(IRQ_PCIE_X16_MSI);
	irqc_ack_irq(IRQ_PCIE_X16_MSI);
}

static void dw_pcie_rd_cdm_reg(struct pcie_port *pp, int where, int size,
				uint32_t *val)
{
	struct dw_pcie *pci;

	pci = to_dw_pcie_from_pp(pp);
	*val = dw_pcie_read_dbi(pci, DW_PCIE_CDM, where, size);
}

/* This api used for DMA initiated by Riscv */
void dw_pcie_ep_dma_ep2rc(struct pcie_port *pp, uint8_t channel)
{
	struct dw_pcie *pci = to_dw_pcie_from_pp(pp);
	uint32_t val, data, size;
#ifndef CONFIG_SIMULATION
	uint32_t intflg;
#endif
	#if 0//read some regs:0x1fc is the base addr of the vsec dma capability
	uint32_t val, addr;

	printf("read vsec dma capability\n");
	for (addr = 0x1fc; addr <= (0x1fc + 0x14); ) {
		val = dw_pcie_read_dbi(pci, DW_PCIE_CDM, addr, 0x4);
		addr += 4;
	}
	#endif
	/* DMA write control 1 reg */
	data = dw_pcie_read_dbi(pci, DW_PCIE_DMA, (0x200 + (channel * 0x200)), 0x4);
	if ((data & 0x60) == 0x20) {
		printf("DMA wr ch%d is in-progress\n", channel);
		return;
	}

	// config max payload size/read request size, 0x78?
	dw_pcie_write_dbi(pci, DW_PCIE_CDM, 0x78, 0x50b0, 0x4);

	// set DMA Engine Enable register
	dw_pcie_write_dbi(pci, DW_PCIE_DMA, 0xc, 0x1, 0x4);
#ifdef CONFIG_SIMULATION
	// set DMA write interrupt mask register
	dw_pcie_write_dbi(pci, DW_PCIE_DMA, 0x54, 0x0, 0x4);
#else
	intflg = __raw_readl(0x800000008);
	if (intflg == 0)//0 means local DMA interrupt test;1 means msi test
		val = 0;
	else
		val = 0xffffffff;

	// set DMA write interrupt mask register
	dw_pcie_write_dbi(pci, DW_PCIE_DMA, 0x54, val, 0x4);

	dw_pcie_rd_cdm_reg(pp, 0x54, 4, &val);//msi low addr
	dw_pcie_write_dbi(pci, DW_PCIE_DMA, 0x60, val, 0x4);

	dw_pcie_rd_cdm_reg(pp, 0x58, 4, &val);//msi high addr
	dw_pcie_write_dbi(pci, DW_PCIE_DMA, 0x64, val, 0x4);

	dw_pcie_rd_cdm_reg(pp, 0x5c, 4, &val);//msi data

	data = dw_pcie_read_dbi(pci, DW_PCIE_DMA, 0x70, 0x4);//msi data reg
	if ((channel % 2) == 1) {
		data &= 0xffff;
		data |= (val << 16);
	} else {
		data &= 0xffff0000;
		data |= val;
	}
	dw_pcie_write_dbi(pci, DW_PCIE_DMA, 0x70, data, 0x4);
#endif

#ifndef CONFIG_SIMULATION
	if (intflg == 0)
		val = 0x04000008;//enable local dma interrupt
	else
		val = 0x04000018;//enable local & msi interrupt
#else
		val = 0x04000018;//enable local & msi interrupt
#endif
	// set DMA Channel control 1 register
	dw_pcie_write_dbi(pci, DW_PCIE_DMA, (0x200 + (channel * 0x200)), val, 0x4);
#ifndef CONFIG_SIMULATION
	size = __raw_readl(0x80000000c);
#else
	size = 0x10;
#endif

	// set DMA transfer size register
	dw_pcie_write_dbi(pci, DW_PCIE_DMA, (0x208 + (channel * 0x200)), size, 0x4);
	// set DMA SAR low register
	dw_pcie_write_dbi(pci, DW_PCIE_DMA, (0x20c + (channel * 0x200)), 0x0, 0x4);
	// set DMA SAR high register
	dw_pcie_write_dbi(pci, DW_PCIE_DMA, (0x210 + (channel * 0x200)), 0x8, 0x4);
	// set DMA DAR low register
#ifndef CONFIG_SIMULATION
	val = __raw_readl(0x800000004);
#else
	val = 0x10000000;
#endif
	dw_pcie_write_dbi(pci, DW_PCIE_DMA, (0x214 + (channel * 0x200)), val, 0x4);
	// set DMA DAR high register
	dw_pcie_write_dbi(pci, DW_PCIE_DMA, (0x218 + (channel * 0x200)), 0x0, 0x4);
	// set DMA write doorbell register
	dw_pcie_write_dbi(pci, DW_PCIE_DMA, 0x10, channel, 0x4);

#ifndef CONFIG_SIMULATION
	if (intflg == 0) {
		val = dw_pcie_read_dbi(pci, DW_PCIE_DMA, 0x4c, 0x4);
		//write channel 0 DMA done interrupt status
		while (!(val & (1 << channel))) {
			val = dw_pcie_read_dbi(pci, DW_PCIE_DMA, 0x4c, 0x4);
			if (val != 0)
				printf("wr ch%d st=%x\n", channel, val);
		}
	}
#endif

}

/* This api used for DMA initiated by Riscv */
void dw_pcie_ep_dma_rc2ep(struct pcie_port *pp, uint8_t channel)
{
	struct dw_pcie *pci = to_dw_pcie_from_pp(pp);
	uint32_t val, data, size;
#ifndef CONFIG_SIMULATION
	uint32_t intflg;
#endif
	#if 0//read some regs:0x1fc is the base addr of the vsec dma capability
	uint32_t val, addr;

	printf("read vsec dma capability\n");
	for (addr = 0x1fc; addr <= (0x1fc + 0x14);) {
		val = dw_pcie_read_dbi(pci, DW_PCIE_CDM, addr, 0x4);
		addr += 4;
	}
	#endif
	/* DMA read control 1 reg */
	data = dw_pcie_read_dbi(pci, DW_PCIE_DMA, (0x300 + (channel * 0x200)), 0x4);
	if ((data & 0x60) == 0x20) {
		printf("DMA rd ch%d is in-progress\n", channel);
		return;
	}
	// config max payload size/read request size, 0x78?
	dw_pcie_write_dbi(pci, DW_PCIE_CDM, 0x78, 0x50b0, 0x4);

	// set DMA Engine Enable register
	dw_pcie_write_dbi(pci, DW_PCIE_DMA, 0x2c, 0x1, 0x4);
#ifdef CONFIG_SIMULATION
	// set DMA write interrupt mask register
	dw_pcie_write_dbi(pci, DW_PCIE_DMA, 0xa8, 0x0, 0x4);
#else
	intflg = __raw_readl(0x800000008);
	if (intflg == 0)//0 means local DMA interrupt test;1 means msi test
		val = 0;
	else
		val = 0xffffffff;
	// set DMA write interrupt mask register
	dw_pcie_write_dbi(pci, DW_PCIE_DMA, 0xa8, val, 0x4);

	dw_pcie_rd_cdm_reg(pp, 0x54, 4, &val);//msi low addr
	dw_pcie_write_dbi(pci, DW_PCIE_DMA, 0xcc, val, 0x4);

	dw_pcie_rd_cdm_reg(pp, 0x58, 4, &val);//msi high addr
	dw_pcie_write_dbi(pci, DW_PCIE_DMA, 0xd0, val, 0x4);

	dw_pcie_rd_cdm_reg(pp, 0x5c, 4, &val);//msi data

	data = dw_pcie_read_dbi(pci, DW_PCIE_DMA, 0xdc, 0x4);//msi data reg
	if ((channel % 2) == 1) {
		data &= 0xffff;
		data |= (val << 16);
	} else {
		data &= 0xffff0000;
		data |= val;
	}
	dw_pcie_write_dbi(pci, DW_PCIE_DMA, 0xdc, data, 0x4);
#endif
#ifndef CONFIG_SIMULATION
	if (intflg == 0)
		val = 0x04000008;//enable local dma interrupt
	else
		val = 0x04000018;//enable local & msi interrupt
#else
		val = 0x04000018;//enable local & msi interrupt
#endif

	// set DMA Channel control 1 register
	dw_pcie_write_dbi(pci, DW_PCIE_DMA, (0x300 + (channel * 0x200)), val, 0x4);

#ifndef CONFIG_SIMULATION
	size = __raw_readl(0x80000000c);
#else
	size = 0x10;
#endif
	// set DMA transfer size register
	dw_pcie_write_dbi(pci, DW_PCIE_DMA, (0x308 + (channel * 0x200)), size, 0x4);
	// set DMA SAR low register
#ifndef CONFIG_SIMULATION
	val = __raw_readl(0x800000004);
#else
	val = 0x10000000;
#endif
	dw_pcie_write_dbi(pci, DW_PCIE_DMA, (0x30c + (channel * 0x200)), val, 0x4);
	// set DMA SAR high register
	dw_pcie_write_dbi(pci, DW_PCIE_DMA, (0x310 + (channel * 0x200)), 0x0, 0x4);
	// set DMA DAR low register
	dw_pcie_write_dbi(pci, DW_PCIE_DMA, (0x314 + (channel * 0x200)), 0x0, 0x4);
	// set DMA DAR high register
	dw_pcie_write_dbi(pci, DW_PCIE_DMA, (0x318 + (channel * 0x200)), 0x8, 0x4);
	// set DMA write doorbell register
	dw_pcie_write_dbi(pci, DW_PCIE_DMA, 0x30, channel, 0x4);

#ifndef CONFIG_SIMULATION
	if (intflg == 0) {
		val = dw_pcie_read_dbi(pci, DW_PCIE_DMA, 0xa0, 0x4);
		//read channel 0 DMA done interrupt status
		while (!(val & (1 << channel))) {
			val = dw_pcie_read_dbi(pci, DW_PCIE_DMA, 0xa0, 0x4);
			if (val != 0)
				printf("rd ch%d st=%x\n", channel, val);
		}
	}
#endif
}

void dw_pcie_ep_dma_ep2rc_api(struct pcie_port *pp, uint8_t channel,
		uint64_t src, uint64_t dst, uint32_t size)
{
	struct dw_pcie *pci = to_dw_pcie_from_pp(pp);
	uint32_t val, data;

	/* DMA write control 1 reg */
	data = dw_pcie_read_dbi(pci, DW_PCIE_DMA, (0x200 + (channel * 0x200)), 0x4);
	if ((data & 0x60) == 0x20) {
		printf("DMA wr ch%d is in-progress\n", channel);
		return;
	}

	// config max payload size/read request size, 0x78?
	dw_pcie_write_dbi(pci, DW_PCIE_CDM, 0x78, 0x50b0, 0x4);

	// set DMA Engine Enable register
	dw_pcie_write_dbi(pci, DW_PCIE_DMA, 0xc, 0x1, 0x4);
	// set DMA write interrupt mask register
	val = 0xffffffff;
	dw_pcie_write_dbi(pci, DW_PCIE_DMA, 0x54, val, 0x4);

	dw_pcie_rd_cdm_reg(pp, 0x54, 4, &val);//msi low addr
	dw_pcie_write_dbi(pci, DW_PCIE_DMA, 0x60, val, 0x4);

	dw_pcie_rd_cdm_reg(pp, 0x58, 4, &val);//msi high addr
	dw_pcie_write_dbi(pci, DW_PCIE_DMA, 0x64, val, 0x4);

	dw_pcie_rd_cdm_reg(pp, 0x5c, 4, &val);//msi data

	data = dw_pcie_read_dbi(pci, DW_PCIE_DMA, 0x70, 0x4);//msi data reg
	if ((channel % 2) == 1) {
		data &= 0xffff;
		data |= (val << 16);
	} else {
		data &= 0xffff0000;
		data |= val;
	}
	dw_pcie_write_dbi(pci, DW_PCIE_DMA, 0x70, data, 0x4);

	val = 0x04000018;//enable local & msi interrupt
	// set DMA Channel control 1 register
	dw_pcie_write_dbi(pci, DW_PCIE_DMA, (0x200 + (channel * 0x200)), val, 0x4);

	//printf("size=%x\n",size);
	dw_pcie_write_dbi(pci, DW_PCIE_CDM, 0x84c, size, 0x4);
	// set DMA transfer size register
	dw_pcie_write_dbi(pci, DW_PCIE_DMA, (0x208 + (channel * 0x200)), size, 0x4);
	// set DMA SAR low register
	val = src & 0xffffffff;
	//printf("src low addr=%x\n",val);
	dw_pcie_write_dbi(pci, DW_PCIE_DMA, (0x20c + (channel * 0x200)), val, 0x4);
	// set DMA SAR high register
	val = ((src & 0xffffffff00000000) >> 32);
	//printf("src high addr=%x\n",val);
	dw_pcie_write_dbi(pci, DW_PCIE_DMA, (0x210 + (channel * 0x200)), val, 0x4);
	// set DMA DAR low register
	val = dst & 0xffffffff;
	//printf("dst low addr=%x\n",val);
	dw_pcie_write_dbi(pci, DW_PCIE_DMA, (0x214 + (channel * 0x200)), val, 0x4);
	// set DMA DAR high register
	val = ((dst & 0xffffffff00000000) >> 32);
	//printf("dst high addr=%x\n",val);
	dw_pcie_write_dbi(pci, DW_PCIE_DMA, (0x218 + (channel * 0x200)), val, 0x4);
	// set DMA write doorbell register
	dw_pcie_write_dbi(pci, DW_PCIE_DMA, 0x10, channel, 0x4);

	val = dw_pcie_read_dbi(pci, DW_PCIE_DMA, 0x4c, 0x4);
	//write channel 0 DMA done interrupt status
	while (!(val & (1 << channel)))	{
		val = dw_pcie_read_dbi(pci, DW_PCIE_DMA, 0x4c, 0x4);
		if (val != 0) {
			//printf("wr ch%d st=%x\n", channel, val);
		}
	}

}

void dma_ep2rc(struct pcie_port *pp, uint8_t channel, uint64_t src, uint64_t dst, uint32_t size)
{
	uint32_t times, lastsize, i, sizepertime, data;
	uint64_t srcpertime, dstpertime;
	struct dw_pcie *pci = NULL;
	struct dw_pcie *controller;

	controller = &controllers[0];
	pci = to_dw_pcie_from_pp(&(controller->pp));


	if (size > 0x400000) {
		times = size / 0x400000;
		lastsize = size % 0x400000;
		if (lastsize)
			times++;
		else
			lastsize = 0x400000;
	} else {
		times = 1;
		lastsize = size;
	}

	printf("times=%d,lastsize=%x\n", times, lastsize);
	srcpertime = src;
	dstpertime = dst;
	for (i = 0; i < times; i++) {
		sizepertime = 0x400000;
		if ((times - 1) == i) {
			sizepertime = lastsize;
			/* EP write to RC finished */
			data = dw_pcie_read_dbi(pci, DW_PCIE_CDM, PCIE_FLAG_REG, 0x4);
			data &= 0xfffffff8;
			data |= EP2RC_LAST_PKG_FLG;
			/* EP write to RC finished */
			dw_pcie_write_dbi(pci, DW_PCIE_CDM, PCIE_FLAG_REG, data, 0x4);
			//printf("850 set to 1\n");
		}
		//printf("src=%llx,dst=%llx,size=%x\n",srcpertime,dstpertime,sizepertime);
		dw_pcie_ep_dma_ep2rc_api(pp, channel, srcpertime, dstpertime, sizepertime);

		srcpertime += 0x400000;
		data = dw_pcie_read_dbi(pci, DW_PCIE_CDM, PCIE_RC_RCVED_REG, 0x4);//RC rcved
		while (!data) {//wait until RC rcved
			data = dw_pcie_read_dbi(pci, DW_PCIE_CDM, PCIE_RC_RCVED_REG, 0x4);//RC rcved
		}
		//printf("RC rcved %d\n",i);
		dw_pcie_write_dbi(pci, DW_PCIE_CDM, PCIE_RC_RCVED_REG, 0, 0x4);
	}
}

void pcie_dw_dma_ep2rc(uint8_t channel, uint64_t src, uint32_t size)
{
	struct dw_pcie *controller;
	uint32_t val;
	struct dw_pcie *pci = NULL;
	uint64_t dst, value;

	controller = &controllers[0];
	pci = to_dw_pcie_from_pp(&(controller->pp));

	val = dw_pcie_read_dbi(pci, DW_PCIE_CDM, PCIE_RC_ADDR_LOW_REG, 0x4);//rc addr low
	dst = val;
	value = dw_pcie_read_dbi(pci, DW_PCIE_CDM, PCIE_RC_ADDR_HI_REG, 0x4);//rc addr high
	dst |= (value << 32);
	printf("rc ddr addr 4 result=%llx\n", dst);

	dma_ep2rc(&(controller->pp), channel, src, dst, size);
}

void dpu_rc_pcie_inta_handler(irq_t irq)
{
#if 0
	struct dw_pcie *controller = &controllers[0];
	struct dw_pcie *pci = to_dw_pcie_from_pp(&(controller->pp));
#endif

	irqc_mask_irq(IRQ_PCIE_X16_INTA);

	printf("RC Receive INTA interrupt\n");

	irqc_unmask_irq(IRQ_PCIE_X16_INTA);
	irqc_ack_irq(IRQ_PCIE_X16_INTA);
}

void dpu_pcie_test(void)
{
#ifdef CONFIG_SIMULATION
	int channel;
#endif
#ifdef CONFIG_DW_PCIE_RC
	// trigger EP VIP MSI interrupt
	__raw_writel(0x3, TCSR_MSG_REG(0x80));
	// trigger EP VIP INTA interrupt
	__raw_writel(0x2, TCSR_MSG_REG(0x80));

	__raw_writeq(0x64646464, (PCIE_SUBSYS_ADDR_START + 0x10));
	val = __raw_readq(PCIE_SUBSYS_ADDR_START + 0x10);
	if (val == 0x64646464)
		printf("MEM64 Read/Write transaction passed\n");

	__raw_writel(0x32323232, (PCIE_SUBSYS_ADDR_START + 0x100));
	val = __raw_readl(PCIE_SUBSYS_ADDR_START + 0x100);
	if (val == 0x32323232)
		printf("MEM32 Read/Write transaction passed\n");

	// cfg0 read
	val = __raw_readl(PCIE_SUBSYS_ADDR_START + PCIE_CORE_X16_CFG0_START + 0x0);
	printf("cfg0: %x\n", val);

	// cfg1 read
	val = __raw_readl(PCIE_SUBSYS_ADDR_START + PCIE_CORE_X16_CFG1_START + 0x100000);
	printf("cfg1: %x\n", val);

	// cfg0 read
	val = __raw_readl(PCIE_SUBSYS_ADDR_START + PCIE_CORE_X16_CFG0_START + 0x700);
	printf("cfg0 0x700= %x\n", val);

	__raw_writel(0x0ca5045a, (PCIE_SUBSYS_ADDR_START + PCIE_CORE_X16_CFG0_START + 0x700));
	val = __raw_readl(PCIE_SUBSYS_ADDR_START + PCIE_CORE_X16_CFG0_START + 0x700);
	printf("cfg0 0x700 after wr= %x\n", val);

	// cfg1 read
	val = __raw_readl(PCIE_SUBSYS_ADDR_START + PCIE_CORE_X16_CFG1_START + 0x700);
	printf("cfg1 0x700= %x\n", val);

	__raw_writel(0x0ba5035a, (PCIE_SUBSYS_ADDR_START + PCIE_CORE_X16_CFG1_START + 0x700));
	val = __raw_readl(PCIE_SUBSYS_ADDR_START + PCIE_CORE_X16_CFG1_START + 0x700);
	printf("cfg1 0x700 after wr= %x\n", val);
#else
#ifdef CONFIG_DPU_TEST_LINK_SPEED_CHANGE
	change_link_speed(pci, pcie_subsys);
#endif	
#ifndef CONFIG_DDR
	clk_enable(axi_clk);
	clk_enable(srst_ddr0_0);
	__raw_writel(0xffffffff, SRST_REG(0));
#endif
	// carry out EP DMA test
#ifdef CONFIG_SIMULATION
	for (channel = 0; channel < 2; channel++) {
		printf("RC2EP 4 ch%d\n", channel);
		__raw_writel((0x12345678 + channel), 0x100000000);
		__raw_writel((0x23456789 + channel), 0x100000004);

		val = __raw_readl(0x100000000);
		printf("RC_Ori0:%x\n", val);
		val = __raw_readl(0x100000004);
		printf("RC_Ori4:%x\n", val);

		dw_pcie_ep_dma_rc2ep(&(controller->pp), channel);

		num = 0;
		dma_irq_triggered = false;
		while (!dma_irq_triggered) {
			irq_local_enable();
			irq_local_disable();
			num++;
			if (num >= 20) {
				printf("nottrigged\n");
				break;
			}
		}

		asm("fence.i\n\t");
		val = __raw_readl(0x800000000);
		asm("fence.i\n\t");
		printf("EP_aft0:%x\n", val);
		asm("fence.i\n\t");
		val = __raw_readl(0x800000004);
		asm("fence.i\n\t");
		printf("EP_aft4:%x\n", val);

		printf("EP2RC 4 ch%d\n", channel);

		__raw_writel((0x32323232 + channel), 0x100000000);
		__raw_writel((0x64646464 + channel), 0x100000004);

		val = __raw_readl(0x100000000);
		printf("RC_Ori0:%x\n", val);
		val = __raw_readl(0x100000004);
		printf("RC_Ori4:%x\n", val);

		asm("fence.i\n\t");
		__raw_writel((0x11111111 + channel), 0x800000000);
		__raw_writel((0x22222222 + channel), 0x800000004);
		asm("fence.i\n\t");

		asm("fence.i\n\t");
		val = __raw_readl(0x800000000);
		asm("fence.i\n\t");
		printf("EP_Ori0:%x\n", val);
		asm("fence.i\n\t");
		val = __raw_readl(0x800000004);
		asm("fence.i\n\t");
		printf("EP_Ori4:%x\n", val);

		dma_irq_triggered = false;
		dw_pcie_ep_dma_ep2rc(&(controller->pp), channel);

		num = 0;
		while (!dma_irq_triggered) {
			irq_local_enable();
			irq_local_disable();
			num++;
			if (num >= 20) {
				printf("nottrigged2\n");
				break;
			}
		}

		val = __raw_readl(0x100000000);
		printf("RC_aft0:%x\n", val);
		val = __raw_readl(0x100000004);
		printf("RC_aft4:%x\n", val);
	}
#endif
#endif
}
#else
#define dpu_pcie_test()		do { } while (0)
#endif

void dpu_pcie_inta_handler(irq_t irq)
{
	uint32_t val;
	struct dw_pcie *pci = NULL;
	struct dw_pcie *controller;

	controller = &controllers[0];
	pci = to_dw_pcie_from_pp(&(controller->pp));

	irqc_mask_irq(IRQ_PCIE_X16_LDMA);

	dma_irq_triggered = true;
	//printf("bird: Receive PCIE INTA interrupt\n");

	val = dw_pcie_read_dbi(pci, DW_PCIE_DMA, 0x4c, 0x4);//DMA write interrupt status reg
	if (val & 0x1) {//channel 0
		//clear interrupt
		val = dw_pcie_read_dbi(pci, DW_PCIE_DMA, 0x58, 0x4);
		val |= 0x1;
		dw_pcie_write_dbi(pci, DW_PCIE_DMA, 0x58, val, 0x4);
		printf("clear int wr ch0\n");
	}
	if (val & 0x2) {//channel 1
		//clear interrupt
		val = dw_pcie_read_dbi(pci, DW_PCIE_DMA, 0x58, 0x4);
		val |= 0x2;
		dw_pcie_write_dbi(pci, DW_PCIE_DMA, 0x58, val, 0x4);
		printf("clear int wr ch1\n");
	}

	val = dw_pcie_read_dbi(pci, DW_PCIE_DMA, 0xa0, 0x4);//DMA read interrupt status reg
	if (val & 0x1) {//channel 0
		//clear interrupt
		val = dw_pcie_read_dbi(pci, DW_PCIE_DMA, 0xac, 0x4);
		val |= 0x1;
		dw_pcie_write_dbi(pci, DW_PCIE_DMA, 0xac, val, 0x4);
		//printf("clear int rd ch0\n");
		/* huge data from RC transferred */
		val = dw_pcie_read_dbi(pci, DW_PCIE_CDM, PCIE_FLAG_REG, 0x4);
		val &= 0x7;
		//printf("whether huge data transferred,val=%x\n",val);
		if (val == RC2EP_LAST_PKG_FLG) {//the last package of the huge data
			printf("huge data transferred\n");
			huge_data_finished = 1;
			val = dw_pcie_read_dbi(pci, DW_PCIE_CDM, PCIE_FLAG_REG, 0x4);
			val &= 0xfffffff8;
			val |= EP2RC_INVALID_FLG;
			dw_pcie_write_dbi(pci, DW_PCIE_CDM, PCIE_FLAG_REG, val, 0x4);
		} else {
			dw_pcie_write_dbi(pci, DW_PCIE_CDM, PCIE_EP_RCVED_REG, 1, 0x4);//EP rcved
		}
	}
	if (val & 0x2) {//channel 1
		//clear interrupt
		val = dw_pcie_read_dbi(pci, DW_PCIE_DMA, 0xac, 0x4);
		val |= 0x2;
		dw_pcie_write_dbi(pci, DW_PCIE_DMA, 0xac, val, 0x4);
		printf("clear int rd ch1\n");
	}

	//dpu_pcie_handle_irq(INTA_INT);
	irqc_unmask_irq(IRQ_PCIE_X16_LDMA);
	irqc_ack_irq(IRQ_PCIE_X16_LDMA);
}

#ifndef CONFIG_SIMULATION
static void pcie_dma_bh_poll_handler(uint8_t events)//forever polling
{
	uint32_t val, i, tmp, j;
	struct dw_pcie *pci = NULL;
	struct dw_pcie *controller;
	uint64_t dst, value, addr64;
	int nr_pages;
	void (*boot_entry)(void);

	controller = &controllers[0];
	pci = to_dw_pcie_from_pp(&(controller->pp));
#if 0
	if (!dma_irq_triggered) {
		printf("irq local enable called\n");
		irq_local_enable();
		irq_local_disable();
	}

#endif

#ifndef CONFIG_DPU_INITIATE_DMA_BY_LOCAL
	if (huge_data_finished == 1) {
		huge_data_finished = 0;
#ifdef CONFIG_DPU_TEST_DDR_BY_DMA
		val = dw_pcie_read_dbi(pci, DW_PCIE_CDM, PCIE_RC_ADDR_LOW_REG, 0x4);//rc addr low
		//printf("rc ddr addr low=%x\n",val);
		src = val;
		value = dw_pcie_read_dbi(pci, DW_PCIE_CDM, PCIE_RC_ADDR_HI_REG, 0x4);//rc addr high
		//printf("rc ddr addr high=%x\n",value);
		src |= (value << 32);
		printf("rc ddr addr=%llx\n", src);
		val = dw_pcie_read_dbi(pci, DW_PCIE_CDM, PCIE_EP_ADDR_LOW_REG, 0x4);//ep addr low
		//printf("ep ddr addr low=%x\n",val);
		dst = val;
		value = dw_pcie_read_dbi(pci, DW_PCIE_CDM, PCIE_EP_ADDR_HI_REG, 0x4);//ep addr high
		//printf("ep ddr addr high=%x\n",value);
		dst |= (value << 32);
		printf("ep ddr addr=%llx\n", dst);
		/* huge data size */
		val = dw_pcie_read_dbi(pci, DW_PCIE_CDM, PCIE_HUGE_DATA_SIZE_REG, 0x4);
		printf("size=%x\n", val);
		size = val;
		dw_pcie_write_dbi(pci, DW_PCIE_CDM, PCIE_EP_RCVED_REG, 1, 0x4);//EP rcved
		dma_ep2rc(&(controller->pp), 0, dst, src, size);


#else
		val = dw_pcie_read_dbi(pci, DW_PCIE_CDM, PCIE_EP_ADDR_LOW_REG, 0x4);//ep addr low
		//printf("ep ddr addr low=%x\n",val);
		dst = val;
		value = dw_pcie_read_dbi(pci, DW_PCIE_CDM, PCIE_EP_ADDR_HI_REG, 0x4);//ep addr high
		//printf("ep ddr addr high=%x\n",value);
		dst |= (value << 32);
		printf("ep ddr addr=%llx\n", dst);

		if (ep_rsved_page_addr == dst) {// invalid DMA
			/* bit[2:0]:1 for malloc;2 for free;3 for dump */
			tmp = dw_pcie_read_dbi(pci, DW_PCIE_CDM,
					PCIE_CMD_REG, 0x4);
			tmp &= 0x7;
			//printf("val(1:malloc;2:free;3:dump)=%x\n",tmp);

			if (tmp == MEM_ALLOC_REQ) {
				val = dw_pcie_read_dbi(pci, DW_PCIE_CDM,
						PCIE_TOTAL_SIZE_LW_REG, 0x4);
				//printf("rc malloc size low=%x\n",val);
				dst = val;
				value = dw_pcie_read_dbi(pci, DW_PCIE_CDM,
						PCIE_TOTAL_SIZE_HI_REG, 0x4);
				value &= 0xf;
				//printf("rc malloc size hi=%x\n",value);
				dst |= (value << 32);
				printf("rc malloc size=%llx\n", dst);

				nr_pages = (dst/(1 << PAGE_SHIFT));
				if ((dst%(1 << PAGE_SHIFT)))
					nr_pages++;

				addr64 = (uint64_t)page_alloc_pages(nr_pages);
				printf("alloc ep addr = %llx\n", addr64);

				val = addr64&0xffffffff;
				dw_pcie_write_dbi(pci, DW_PCIE_CDM,
						EP_ALLOCED_MEM_LW_REG, val, 0x4);

				val = dw_pcie_read_dbi(pci, DW_PCIE_CDM,
						EP_ALLOCED_MEM_HI_REG, 0x4);
				val &= 0xfffffff0;
				val |= ((uint32_t)((addr64&0xf00000000) >> 32));
				dw_pcie_write_dbi(pci, DW_PCIE_CDM,
						EP_ALLOCED_MEM_HI_REG, val, 0x4);

				val = dw_pcie_read_dbi(pci, DW_PCIE_CDM, PCIE_FLAG_REG, 0x4);
				val &= 0xfffffff8;
				val |= EP2RC_MALLOC_OK_FLG;
				dw_pcie_write_dbi(pci, DW_PCIE_CDM, PCIE_FLAG_REG, val, 0x4);
			}

			if (tmp == MEM_FREE_REQ) {
				val = dw_pcie_read_dbi(pci, DW_PCIE_CDM,
						PCIE_TOTAL_SIZE_LW_REG, 0x4);
				//printf("rc free size low=%x\n",val);
				dst = val;
				value = dw_pcie_read_dbi(pci, DW_PCIE_CDM,
						PCIE_TOTAL_SIZE_HI_REG, 0x4);
				value &= 0xf;
				//printf("rc free size hi=%x\n",value);
				dst |= (value << 32);
				printf("rc free size=%llx\n", dst);

				nr_pages = (dst/(1 << PAGE_SHIFT));
				if ((dst%(1 << PAGE_SHIFT)))
					nr_pages++;

				val = dw_pcie_read_dbi(pci, DW_PCIE_CDM, RC_FREE_MEM_LW_REG, 0x4);
				//printf("rc free addr low=%x\n",val);
				dst = val;
				val = dw_pcie_read_dbi(pci, DW_PCIE_CDM, RC_FREE_MEM_HI_REG, 0x4);
				val &= 0xf;
				//printf("rc free addr high=%x\n",val);
				value = val;
				dst |= (value << 32);
				//printf("rc free addr=%llx\n",dst);

				page_free_pages(dst, nr_pages);
				printf("free ep addr = %llx\n", dst);

				val = dw_pcie_read_dbi(pci, DW_PCIE_CDM, PCIE_FLAG_REG, 0x4);
				val &= 0xfffffff8;
				val |= EP2RC_FREE_OK_FLG;
				dw_pcie_write_dbi(pci, DW_PCIE_CDM, PCIE_FLAG_REG, val, 0x4);

			}
			if (tmp == MEM_DUMP_REQ) {
				#ifdef CONFIG_PAGE
				do_page_dump(0, 0);
				#endif
				val = dw_pcie_read_dbi(pci, DW_PCIE_CDM, PCIE_FLAG_REG, 0x4);
				val &= 0xfffffff8;
				val |= EP2RC_INVALID_FLG;
				dw_pcie_write_dbi(pci, DW_PCIE_CDM, PCIE_FLAG_REG, val, 0x4);

			}
			val = dw_pcie_read_dbi(pci, DW_PCIE_CDM, PCIE_CMD_REG, 0x4);
			val &= 0xfffffff8;
			val |= INVALID_CMD_REQ;
			dw_pcie_write_dbi(pci, DW_PCIE_CDM, PCIE_CMD_REG, val, 0x4);
		} else {
			printf("check ddr start\n");
			asm("fence.i\n\t");
			for (i = 0; i < 1; i++) {
				printf("%llx:\n", (dst + i*0x400000));
				for (j = 0; j < 4; j++) {
					val = __raw_readl((dst+i*0x400000)+j*4);
					printf("%08x ", val);
				}
				printf("\n");
			}
			asm("fence.i\n\t");
			printf("check ddr end\n");
		}
		dw_pcie_write_dbi(pci, DW_PCIE_CDM, PCIE_EP_RCVED_REG, 1, 0x4);//EP rcved
#endif
	}
#else
	asm("fence.i\n\t");
	tmp = __raw_readl(0x800000000);
	asm("fence.i\n\t");
	printf("tmp=%x\n", tmp);
	if ((tmp != 1) && (tmp != 2) && (tmp != 3) && (tmp != 4) && (tmp != 5) && (tmp != 6)) {
		asm("fence.i\n\t");
		for (i = 0; i < 0x6; i++) {
			val = __raw_readl((0x800000000+i*4));
			printf("fakeddr:%x\n", val);
		}
		asm("fence.i\n\t");

	}

	val = 0x12 + num;
	if (tmp == 1) {
		printf("RC has configured the ATU==>start to write\n");
		printf("write RC DDR10,14 with val %x,%x\n", val, (val+2));
		__raw_writel(val, 0x100000010);
		__raw_writel((val+2), 0x100000014);
		num++;
	}
	if (tmp == 2) {
		printf("RC has configured the ATU==>start to read\n");
		val = __raw_readl(0x100000010);
		printf("RC ddr10:%x\n", val);
		val = __raw_readl(0x100000014);
		printf("RC ddr14:%x\n", val);
	}
	if (tmp == 3) {
		printf("EP2RC 4 ch0\n");
		asm("fence.i\n\t");
		__raw_writel(val, 0x800000010);
		__raw_writel((val+3), 0x800000014);
		asm("fence.i\n\t");
		num++;
		printf("EP ddr val:%x %x\n", val, (val+3));
		dw_pcie_ep_dma_ep2rc(&(controller->pp), 0);
	}
	if (tmp == 4) {
		printf("RC2EP 4 ch0\n");
		dw_pcie_ep_dma_rc2ep(&(controller->pp), 0);
		asm("fence.i\n\t");
		for (i = 0; i < 0x6; i++) {
			val = __raw_readl((0x800000000+i*4));
			printf("fakeddr after RC2EP:%x\n", val);
		}
		asm("fence.i\n\t");

	}
	if (tmp == 5) {
		printf("EP2RC 4 ch1\n");
		asm("fence.i\n\t");
		__raw_writel(val, 0x800000010);
		__raw_writel((val+3), 0x800000014);
		asm("fence.i\n\t");
		num++;
		printf("EP ddr val1:%x %x\n", val, (val+3));
		dw_pcie_ep_dma_ep2rc(&(controller->pp), 1);
	}
	if (tmp == 6) {
		printf("RC2EP 4 ch1\n");
		dw_pcie_ep_dma_rc2ep(&(controller->pp), 1);
		asm("fence.i\n\t");
		for (i = 0; i < 0x6; i++) {
			val = __raw_readl((0x800000000+i*4));
			printf("fakeddr after RC2EP1:%x\n", val);
		}
		asm("fence.i\n\t");

	}
#endif
	val = dw_pcie_read_dbi(pci, DW_PCIE_CDM, PCIE_FLAG_REG, 0x4);
	val &= 0x7;
	if (val == RC2EP_RUN_MCU_CODE_FLG) {
		//printf("run mcu code\n");
		__raw_writel(0xfffffffe, SRST_REG(0));
		__raw_writel(0xffffffff, SRST_REG(0));
		while (!(__raw_readl(SRST_REG(0)) & 0x1))
			;

		val = dw_pcie_read_dbi(pci, DW_PCIE_CDM, PCIE_FLAG_REG, 0x4);
		val &= 0xfffffff8;
		val |= EP2RC_INVALID_FLG;
		dw_pcie_write_dbi(pci, DW_PCIE_CDM, PCIE_FLAG_REG, val, 0x4);

		val = dw_pcie_read_dbi(pci, DW_PCIE_CDM, PCIE_EP_ADDR_LOW_REG, 0x4);//ep addr low
		//printf("ep ddr addr low=%x\n",val);
		dst = val;
		value = dw_pcie_read_dbi(pci, DW_PCIE_CDM, PCIE_EP_ADDR_HI_REG, 0x4);//ep addr high
		//printf("ep ddr addr high=%x\n",value);
		dst |= (value << 32);
		printf("mcu code run addr=%llx\n", dst);
		boot_entry = (void *)dst;
		boot_entry();

		val = dw_pcie_read_dbi(pci, DW_PCIE_CDM, PCIE_FLAG_REG, 0x4);
		val &= 0xfffffff8;
		val |= EP2RC_RUN_MCU_OK_FLG;
		dw_pcie_write_dbi(pci, DW_PCIE_CDM, PCIE_FLAG_REG, val, 0x4);

		printf("returned from main\n");
		// unreachable();
	}
}
#endif

void change_link_speed(struct dw_pcie *pci, struct duowen_pcie_subsystem *pcie_subsys)
{
	//uint32_t lanes = pci->lane_num;
	#define TEST_SPEED_NUM 10
	uint32_t target_speed;
	uint8_t  i;
	uint32_t val;
	enum {
		GEN1 = 1,
		GEN2 = 2,
		GEN3 = 3,
		GEN4 = 4,
		GEN5 = 5
	};
	uint8_t current_speed, test_speed[TEST_SPEED_NUM];

	val = dw_pcie_read_dbi(pci, DW_PCIE_CDM, 0x80, 0x4) & 0xf0000; // [19:16] bit
	printf("0x80(val) before= 0x%x\n", val);
	__raw_writel(val, TCSR_MSG_REG(0x80));
	switch (val) {
	case 0x10000:
		current_speed = GEN1;
		break;
	case 0x20000:
		current_speed = GEN2;
		break;
	case 0x30000:
		current_speed = GEN3;
		break;
	case 0x40000:
		current_speed = GEN4;
		break;
	case 0x50000:
		current_speed = GEN5;
		break;
	default:
		current_speed = GEN1;
		break;
	}
	printf("current speed = %d\n", current_speed);
	__raw_writel(current_speed, TCSR_MSG_REG(0x80));

	switch (current_speed) {
	case GEN2:
		test_speed[0] = GEN1; // Gen2 -> Gen1
		test_speed[1] = GEN2; // Gen1 -> Gen2
		break;
	case GEN3:
		test_speed[0] = GEN2; // GEN3 -> GEN2
		test_speed[1] = GEN1; // GEN2 -> GEN1
		test_speed[2] = GEN2; // GEN1 -> GEN2
		test_speed[3] = GEN3; // GEN2 -> GEN3
		test_speed[4] = GEN2; // GEN3 -> GEN1
		test_speed[5] = GEN2; // GEN1 -> GEN3
		break;
	case GEN4:
		test_speed[0] = GEN3; // GEN4 -> GEN3
		test_speed[1] = GEN2; // GEN3 -> GEN2
		test_speed[2] = GEN1; // GEN2 -> GEN1

		test_speed[3] = GEN2; // GEN1 -> GEN2
		test_speed[4] = GEN3; // GEN2 -> GEN3
		test_speed[5] = GEN4; // GEN3 -> GEN4

		test_speed[6] = GEN2; // GEN4 -> GEN2
		test_speed[7] = GEN4; // GEN2 -> GEN4

		test_speed[8] = GEN1; // GEN4 -> GEN1
		test_speed[9] = GEN4; // GEN1 -> GEN4
		break;
	default:
		break;
	}

//	target_speed = dw_pcie_read_dbi(pci, DW_PCIE_CDM, 0xa0, 0x4);
//	printf("0xa0(target) = 0x%x\n", val);
	//dw_pcie_write_dbi(pci, DW_PCIE_CDM, 0xa0, val|0x1, 0x4);
	for (i = 0; i < TEST_SPEED_NUM; i++) {
		target_speed = dw_pcie_read_dbi(pci, DW_PCIE_CDM, 0xa0, 0x4);
		printf("0xa0(target) = 0x%x\n", (target_speed & 0xf));
		printf("test speed = 0x%x\n", test_speed[i]);
		val = (target_speed & (~0xf)) | test_speed[i];
		__raw_writel(val, TCSR_MSG_REG(0x80));
		dw_pcie_write_dbi(pci, DW_PCIE_CDM, 0xa0, val, 0x4);
#if 0
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
#endif
		val = dw_pcie_read_dbi(pci, DW_PCIE_CDM, PCIE_LINK_WIDTH_SPEED_CONTROL, 0x4);
		val &= ~PORT_LOGIC_SPEED_CHANGE;
		dw_pcie_write_dbi(pci, DW_PCIE_CDM, PCIE_LINK_WIDTH_SPEED_CONTROL, val, 0x4);

		val = dw_pcie_read_dbi(pci, DW_PCIE_CDM, PCIE_LINK_WIDTH_SPEED_CONTROL, 0x4);
		val |= PORT_LOGIC_SPEED_CHANGE;
		dw_pcie_write_dbi(pci, DW_PCIE_CDM, PCIE_LINK_WIDTH_SPEED_CONTROL, val, 0x4);

		wait_controller_linkup(pcie_subsys);
		val = dw_pcie_read_dbi(pci, DW_PCIE_CDM, 0x80, 0x4);
		__raw_writel(val, TCSR_MSG_REG(0x80)); //current speed
		val = ((val & 0xf0000) >> 16);
		printf("current speed = 0x%x\n", val);
		__raw_writel(val, TCSR_MSG_REG(0x80)); //current speed
		printf("test speed = 0x%x\n", test_speed[i]);
		__raw_writel(test_speed[i], TCSR_MSG_REG(0x80)); //test speed
		if (val != test_speed[i]) {
			printf("0x80(val) after= 0x%x\n", val);
			__raw_writel(0x2, TCSR_MSG_REG(0x80)); // change link speed test fail
		}
		__raw_writel(0x1, TCSR_MSG_REG(0x80)); // change link speed test pass
	}
}

uint64_t rd_pcie_rsved_reg_4_baseaddr(uint32_t addr_low, uint32_t addr_hi)
{
	uint32_t val;
	uint64_t val64;
	struct dw_pcie *controller;
	struct dw_pcie *pci = NULL;

	controller = &controllers[0];
	pci = to_dw_pcie_from_pp(&(controller->pp));

	val = dw_pcie_read_dbi(pci, DW_PCIE_CDM, addr_low, 0x4);//port logic rsved reg
	val64 = dw_pcie_read_dbi(pci, DW_PCIE_CDM, addr_hi, 0x4);//port logic rsved reg
	val64 = ((val64 << 32) | val);
	return val64;
}

void pci_platform_init(void)
{
	struct duowen_pcie_subsystem *pcie_subsys;
	struct dw_pcie *controller;
	int i, j;
	uint32_t num, val;
	uint32_t addr;
	uint64_t addr64;
	struct dw_pcie *pci = NULL;

	//imc_addr_trans(0, 0x20000000, 0xc00000000, 0);

	pcie_subsys = &pcie_subsystem;
	instance_subsystem(pcie_subsys);

	clock_init();
	subsys_link_init_pre(pcie_subsys);
	reset_init(pcie_subsys);

	controller = pcie_subsys->controller;
	pci = to_dw_pcie_from_pp(&(controller->pp));

	for (i = 0; i < ARRAY_SIZE(controllers); i++) {
		if (i != 0)//xkm
			break;
		if (controller->active == true)
			dw_pcie_setup_dpu2(&(controller->pp));

		controller++;
	}

	subsys_link_init_post(pcie_subsys);

	/* Find first active controller to enable MSI */
	controller = pcie_subsys->controller;
	for (i = 0; i < ARRAY_SIZE(controllers); i++) {
		if (i != 0)//xkm
			break;
		if (controller->active == true)
			break;
		controller++;
	}

#ifdef CONFIG_DW_PCIE_RC
	irqc_configure_irq(IRQ_PCIE_X16_MSI, 0, IRQ_LEVEL_TRIGGERED);
	irq_register_vector(IRQ_PCIE_X16_MSI, dpu_pcie_msi_handler);
	irqc_enable_irq(IRQ_PCIE_X16_MSI);

	irqc_configure_irq(IRQ_PCIE_X16_INTA, 0, IRQ_LEVEL_TRIGGERED);
	irq_register_vector(IRQ_PCIE_X16_INTA, dpu_rc_pcie_inta_handler);
	irqc_enable_irq(IRQ_PCIE_X16_INTA);

	dw_pcie_enable_msi(&(controller->pp));
#else
	irqc_configure_irq(IRQ_PCIE_X16_LDMA, 0, IRQ_LEVEL_TRIGGERED);
	irq_register_vector(IRQ_PCIE_X16_LDMA, dpu_pcie_inta_handler);
	irqc_enable_irq(IRQ_PCIE_X16_LDMA);
	/* Configure maximum payload size, read request size */
	dw_pcie_write_dbi(pci, DW_PCIE_CDM, 0x78, 0x5030, 0x4);

	j = 0;
	for (i = 0; i < 27; i++) {
		addr = 0x700 + (0x120 + i*4);
		//num = 0x12345678 + i;
		//num = 0xffffffff;
		num = 0;
		dw_pcie_write_dbi(pci, DW_PCIE_CDM, addr, num, 0x4);
		val = dw_pcie_read_dbi(pci, DW_PCIE_CDM, addr, 0x4);//port logic rsved reg
		if (num == val) {
			j++;
			//printf("addr:%x,val=%x\n",addr,val);
		}
	}
	printf("There are %d reserved regs can be used\n", j);

	val = dw_pcie_read_dbi(pci, DW_PCIE_CDM, PCIE_FLAG_REG, 0x4);
	val &= 0xfffffff8;
	val |= EP2RC_RUN_MCU_OK_FLG;
	dw_pcie_write_dbi(pci, DW_PCIE_CDM, PCIE_FLAG_REG, val, 0x4);
	/* Reserved 1 page(4KB) for invalid DMA just for generate interrupt */
	addr64 = (uint64_t)page_alloc_pages(1);
	ep_rsved_page_addr = addr64;
	printf("Reserved addr = %llx\n", addr64);
	if (addr64 != 0)
		memset((void *)addr64, 0xff, PAGE_SIZE);

	addr = (uint32_t)(addr64 & 0xffffffff);
	printf("EP rsved page low=%x\n", addr);
	/* save EP reserved page LOW[31:0] for invalid DMA */
	dw_pcie_write_dbi(pci, DW_PCIE_CDM, EP_RSVED_MEM_LW_REG, addr, 0x4);
	addr = (uint32_t)((addr64 & 0xffffffff00000000) >> 32);
	addr <<= 4;
	val = dw_pcie_read_dbi(pci, DW_PCIE_CDM, RC_FREE_MEM_HI_REG, 0x4);
	val &= 0xffffff0f;//bit[7:4] to save reserved page HI[3:0]
	val |= addr;
	printf("EP rsved page hi[3:0] in [7:4] in %x\n", val);
	/* save EP reserved page HI[3:0] for invalid DMA */
	dw_pcie_write_dbi(pci, DW_PCIE_CDM, RC_FREE_MEM_HI_REG, val, 0x4);


	irq_local_enable();
	pcie_dma_bh_poll = bh_register_handler(pcie_dma_bh_poll_handler);
	irq_register_poller(pcie_dma_bh_poll);
#endif
	dpu_pcie_test();
}
