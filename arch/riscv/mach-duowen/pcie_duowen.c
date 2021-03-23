#include <target/pci.h>
#include <target/clk.h>
#include <target/irq.h>

struct duowen_pcie_subsystem pcie_subsystem;

struct dw_pcie controllers[] = 
{
	// X16
	{
		.axi_dbi_port = AXI_DBI_PORT_X16,
		.dbi_base = CFG_AXI_CORE_X16,
		.pp.cfg_bar0 = PCIE_CORE_X16_CFG0_START,
		.pp.cfg_bar1 = PCIE_CORE_X16_CFG1_START,
		.pp.cfg_size = PCIE_CORE_CFG_SIZE,
		.pp.mem_base = 0,
		.pp.mem_size = PCIE_CORE_MEM_SIZE,
		.pp.chiplink = 0,
		.pp.role = ROLE_RC,
	},

	// X8
	{
		.axi_dbi_port = AXI_DBI_PORT_X8,
		.dbi_base = CFG_AXI_CORE_X8,
		.pp.cfg_bar0 = PCIE_CORE_X8_CFG0_START,
		.pp.cfg_bar1 = PCIE_CORE_X8_CFG1_START,
		.pp.cfg_size = PCIE_CORE_CFG_SIZE,
		.pp.mem_base = 0,
		.pp.mem_size = PCIE_CORE_MEM_SIZE,
		.pp.chiplink = 0,
		.pp.role = ROLE_RC,
	},

	// X4_0
	{
		.axi_dbi_port = AXI_DBI_PORT_X4_0,
		.dbi_base = CFG_AXI_CORE_X4_0,
		.pp.cfg_bar0 = PCIE_CORE_X4_0_CFG0_START,
		.pp.cfg_bar1 = PCIE_CORE_X4_0_CFG1_START,
		.pp.cfg_size = PCIE_CORE_CFG_SIZE,
		.pp.mem_base = 0,
		.pp.mem_size = PCIE_CORE_MEM_SIZE,
		.pp.chiplink = 0,
		.pp.role = ROLE_RC,
	},

	//X4_1
	{
		.axi_dbi_port = AXI_DBI_PORT_X4_1,
		.dbi_base = CFG_AXI_CORE_X4_1,
		.pp.cfg_bar0 = PCIE_CORE_X4_1_CFG0_START,
		.pp.cfg_bar1 = PCIE_CORE_X4_1_CFG1_START,
		.pp.cfg_size = PCIE_CORE_CFG_SIZE,
		.pp.mem_base = 0,
		.pp.mem_size = PCIE_CORE_MEM_SIZE,
		.pp.chiplink = 0,
		.pp.role = ROLE_RC,
	},
};

uint32_t read_apb(uint64_t addr, uint8_t port)
{
	uint32_t data;
#ifdef IPBENCH
	apb_read_c(addr, &data, port);
#else
	data = readl(addr);
#endif
#ifdef CONFIG_DUOWEN_PCIE_DEBUG_ENABLE
	printf("ReadAPB: addr: 0x%llx; data: 0x%08x, port: %d\n", addr, data, port);
#endif
	return data;
}

void write_apb(uint64_t addr, uint32_t data, uint8_t port)
{
#ifdef CONFIG_DUOWEN_PCIE_DEBUG_ENABLE
	printf("WriteAPB: addr: 0x%llx; data: 0x%x port: %d\n", addr, data, port);
#endif
#ifdef IPBENCH
	apb_write_c(addr, data, port);
#else
	writel(addr, data);
#endif
}


#ifdef IPBENCH
#define COUNTER_ADDR    0xf0000000
static uint32_t get_counter(void)
{
	uint32_t cnt;

	cnt =  read_apb(COUNTER_ADDR, 8);
	return cnt;
}

void udelay(uint32_t time)
{
	uint32_t current, next;

	current = get_counter();
	next = current + time * 125;

	do {
		current =  get_counter();
	} while(current < next);
}
#endif

static uint8_t duowen_get_link_mode(struct duowen_pcie_subsystem *pcie_subsystem)
{
	return pcie_subsystem->link_mode;
}

void reset_init(struct duowen_pcie_subsystem *pcie_subsystem)
{
	uint64_t base = pcie_subsystem->cfg_apb[SUBSYS];
	uint8_t port = APB_PORT_SUBSYS;
	uint32_t data = 0;

	write_apb((base + RESET_PHY), 0x10, port);
	write_apb((base + SRAM_CONTROL), 0x0, port);
	write_apb((base + REFCLK_CONTROL), 0x2, port);
	// #200ns
	write_apb((base + RESET_PHY), 0xf, port);
	// #100ns

#ifndef TEST
	while ((data & 0xf) != 0xf) {
		data = read_apb((base + SRAM_STATUS), port);
	}
#endif
	write_apb((base + SRAM_CONTROL), 0x55, port);
	//write_apb((base + RESET_PHY), 0xff, port);
}

void dw_controller_init(struct duowen_pcie_subsystem *pcie_subsystem, int index)
{
	uint8_t port = APB_PORT_X16 + index;
	uint64_t base = pcie_subsystem->cfg_apb[X16 + index];
	struct dw_pcie *controller = pcie_subsystem->controller;
	int role = (controller + index)->pp.role;

	if (role == ROLE_EP )
		write_apb((base + 0), 0xc810000, port);
	else
		write_apb((base + 0), 0xc810010, port);
}

void wait_controller_linkup(struct duowen_pcie_subsystem *pcie_subsystem, int index)
{
	uint8_t i = 0, port = APB_PORT_X16 + index;
	uint32_t data;
	uint64_t base = pcie_subsystem->cfg_apb[X16 + index];

	data = read_apb((base + 0x10), port);
	printf("wait controller %d smlh&rdlh ready\n", index);
	while ((data & ( BIT(0) | BIT(11) )) != ( BIT(0) | BIT(11) )) {
		data = read_apb((base + 0x10), APB_PORT_X16);
		printf("count: %d: link_status is : 0x%x\n", i,data);
		i++;
		if (i > 100) {
			printf("controller %d: failed to establish link up\n", index);
			break;
		}
	}

}

static void subsys_controllers_init(struct duowen_pcie_subsystem *pcie_subsystem, void (*func)(struct duowen_pcie_subsystem *, int))
{
	struct dw_pcie *controller = pcie_subsystem->controller;
	int i;

	controller = &controllers[0];
	for(i = 0; i < pcie_subsystem->ctrl_cnt; i++) {
		if ((controller + i)->active == true)
			(*func)(pcie_subsystem, i);
	}
}

static void subsys_link_init_pre(struct duowen_pcie_subsystem *pcie_subsystem)
{
	uint64_t base = pcie_subsystem->cfg_apb[SUBSYS];
	struct dw_pcie *controller = pcie_subsystem->controller;
	uint8_t port = APB_PORT_SUBSYS;
	uint8_t link_mode = pcie_subsystem->link_mode;
	bool chiplink = pcie_subsystem->chiplink;
	int socket_id = pcie_subsystem->socket_id;

	//assert(link_mode != LINK_MODE_INVALID);

	// #10ns
	write_apb((base + SUBSYS_CONTROL), link_mode, port);
	write_apb((base + RESET_CORE_X4_0), 0xff, port);
	write_apb((base + RESET_CORE_X4_1), 0xff, port);
	write_apb((base + RESET_CORE_X8), 0xff, port);
	write_apb((base + RESET_CORE_X16), 0xff, port);

	switch (link_mode) {
		case LINK_MODE_4_4_4_4:     // 0: In DPU, X4_0
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

			break;
		case LINK_MODE_8_4_0_4:     // 1: In DPU, X4_1
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

			break;
		case LINK_MODE_8_8_0_0:     // 2: In DPU, X8
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

			break;
		case LINK_MODE_16_0_0_0:    //  3: In DPU: X16
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

			break;
		case LINK_MODE_ZEBU:
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
			(controller + X4_1)->order = 0x0;
	}

	if (chiplink)
		(controller + X4_1)->pp.chiplink = 1;

	if (socket_id)
		(controller + X4_1)->pp.role = ROLE_EP;

	subsys_controllers_init(pcie_subsystem, dw_controller_init);
}

static void subsys_link_init_post(struct duowen_pcie_subsystem *pcie_subsys)
{
	uint8_t mode = duowen_get_link_mode(pcie_subsys);
	int id = pcie_subsys->socket_id;
	bool chiplink = pcie_subsys->chiplink;

	switch (mode) {
		case LINK_MODE_4_4_4_4:
			write_apb(pcie_subsys->cfg_apb[X4_0], 0xc018010, APB_PORT_X4_0);

		case LINK_MODE_8_4_0_4:
			// Only X4_1 ctrl in both side will possilbly be used as underlay of chiplink
			if (chiplink && id)
				write_apb(pcie_subsys->cfg_apb[X4_1], 0xc018000, APB_PORT_X4_1);
			else
				write_apb(pcie_subsys->cfg_apb[X4_1], 0xc018010, APB_PORT_X4_1);

		case LINK_MODE_8_8_0_0:
			write_apb(pcie_subsys->cfg_apb[X8], 0xc018010, APB_PORT_X8);

		case LINK_MODE_16_0_0_0:
			write_apb(pcie_subsys->cfg_apb[X16], 0xc018010, APB_PORT_X16);

			break;

		case LINK_MODE_ZEBU: // 0_0_0_4
			if (chiplink && id)
				write_apb(pcie_subsys->cfg_apb[X4_1], 0xc018000, APB_PORT_X4_1);
			else
				write_apb(pcie_subsys->cfg_apb[X4_1], 0xc018010, APB_PORT_X4_1);

			break;
	}
	subsys_controllers_init(pcie_subsys, wait_controller_linkup);
} 

void instance_subsystem(struct duowen_pcie_subsystem *pcie_subsystem, int socket_id, bool chiplink)
{
	memset(pcie_subsystem, 0, sizeof(*pcie_subsystem));

	pcie_subsystem->cfg_apb[X16] = CFG_APB_CORE_X16;
	pcie_subsystem->cfg_apb[X8] = CFG_APB_CORE_X8;
	pcie_subsystem->cfg_apb[X4_0] = CFG_APB_CORE_X4_0;
	pcie_subsystem->cfg_apb[X4_1] = CFG_APB_CORE_X4_1;
	pcie_subsystem->cfg_apb[SUBSYS] = CFG_APB_SUBSYS;

	pcie_subsystem->link_mode = DEFAULT_LINK_MODE;
	pcie_subsystem->controller = &controllers[0];
	pcie_subsystem->ctrl_cnt = (sizeof(controllers)/sizeof(struct dw_pcie));
	pcie_subsystem->socket_id = socket_id;
	pcie_subsystem->chiplink = chiplink;

	if (chiplink)
		pcie_subsystem->link_mode = LINK_MODE_4_4_4_4;

#ifdef CONFIG_DUOWEN_ZEBU
	if (chiplink)
		pcie_subsystem->link_mode = LINK_MODE_ZEBU;
#endif

}

#ifndef TEST
void clock_init(void)
{
	clk_enable(pcie_aclk);
	clk_enable(pcie_pclk);
	clk_enable(pcie_aux_clk);
	clk_enable(pcie_alt_ref_clk);
}
#endif

#ifdef CONFIG_DUOWEN_PCIE_TEST
void duowen_pcie_handle_msi(bool en)
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

void duowen_pcie_handle_inta(bool en)
{
	if (en) // trigger assert INTA msg
		__raw_writel(0x2, MSG_REG(0x80));
	else    // trigger de-assert INTA msg
		__raw_writel(0x4, MSG_REG(0x80));
}

void duowen_pcie_handle_irq(int int_type)
{
	if (int_type == MSI_INT)
		duowen_pcie_handle_msi(0);
	else if (int_type == INTA_INT)
		duowen_pcie_handle_inta(0);
	else
		printf("Unknown IRQ number: 0x%x\n", int_type);
}

void duowen_pcie_msi_handler(irq_t irq)
{
	printf("bird: Receive PCIE MSI interrupt\n");
	irqc_mask_irq(IRQ_PCIE_X16_MSI);
	duowen_pcie_handle_irq(MSI_INT);
	irqc_unmask_irq(IRQ_PCIE_X16_MSI);
	irqc_ack_irq(IRQ_PCIE_X16_MSI);
}

void duowen_pcie_inta_handler(irq_t irq)
{
	printf("bird: Receive PCIE INTA interrupt\n");
	irqc_mask_irq(IRQ_PCIE_X16_INTA);
	duowen_pcie_handle_irq(INTA_INT);
	irqc_unmask_irq(IRQ_PCIE_X16_INTA);
	irqc_ack_irq(IRQ_PCIE_X16_INTA);
}
#endif

uint32_t dw_get_pci_conf_reg(int bus, int dev, int fun, int reg, uint8_t index) {
	struct pcie_port *pp = &(controllers[index].pp);
	uint64_t pci_addr, cpu_addr, base;
	uint32_t val;

	if (bus > 1) {
		cpu_addr = pp->cfg_bar1;
	} else {
		cpu_addr = pp->cfg_bar0;
	}
	pci_addr = form_pci_addr(bus, dev, fun);

	base = PCIE_SUBSYS_ADDR_START;
	val = __raw_readl(base + cpu_addr + pci_addr + reg);
	return val;
}

void dw_set_pci_conf_reg(int bus, int dev, int fun, int reg, uint32_t val, uint8_t index) {
	struct pcie_port *pp = &(controllers[index].pp);
	uint64_t pci_addr, cpu_addr, base;

	if (bus > 1) {
		cpu_addr = pp->cfg_bar1;
	} else {
		cpu_addr = pp->cfg_bar0;
	}
	pci_addr = form_pci_addr(bus, dev, fun);

	base = PCIE_SUBSYS_ADDR_START;
	__raw_writel(val, base + cpu_addr + pci_addr + reg);
}
//void setup_ep()
void pci_platform_init(void)
{
	struct duowen_pcie_subsystem *pcie_subsys;
	struct dw_pcie *controller;
	struct pcie_port *pp = &(controllers[0].pp);
	struct dw_pcie *pci = to_dw_pcie_from_pp(pp);
	int i, chiplink = 0, socket_id = 0;
	volatile uint32_t val;

#ifdef CONFIG_DUOWEN_PCIE_CHIPLINK
	chiplink = imc_chip_link();
	socket_id = imc_socket_id();
#endif
	pcie_subsys = &pcie_subsystem;
	instance_subsystem(pcie_subsys, socket_id, chiplink);

#ifndef TEST
	clock_init();
#endif
	subsys_link_init_pre(pcie_subsys);
	reset_init(pcie_subsys);

	controller = pcie_subsys->controller;

	for(i = 0; i < pcie_subsys->ctrl_cnt; i++) {
		if (controller->active == true) {
			dw_pcie_setup_ctrl(&(controller->pp));
		}
		controller++;
	}

	subsys_link_init_post(pcie_subsys);


	// This part carry out an simple communication test between dual sockets
	// via X4_1 ctrls in both side
	// The way is as follows:
	// -- Socket0 sends 0x11111111 to 0x80000000010 which is in remote socket1 DDR range;
	// -- Socket1 sends 0x11111111 to 0x10 which is in remote socket0 DDR range;
	// -- Socket0 read data from addr 0x10 in its local DDR range
	// -- Socket1 read data from addr 0x80000000010 in its local DDR range
	// -- When both side get the data equals to 0x11111111, test passed

	if (chiplink) {
		if (!socket_id) {
			printf("rc start\n");

			/* For Chiplink test, rc side */
			dw_pcie_write_dbi(pci, DW_PCIE_CDM, 0x20, 0x0, 0x4);
			//setup_ep();

#if 0
			// ep BAR
			dw_set_pci_conf_reg(1, 0, 0, 0x10, 0x0, index);
			// ep command reg
			val = dw_get_pci_conf_reg(1, 0, 0, 0x4, index);
			val |= 0x6;
			dw_set_pci_conf_reg(1, 0, 0, 0x4, val, index);
#endif

			// test starts: sends test data to socket1's ddr
			__raw_writel(0x11111111, (0x10 + 0x80000000000));

			asm volatile("fence rw, rw\n\t");

			val = 0;
			while (val != 0x11111111)
				val = __raw_readl(0x10);

			printf("rc end\n");
		} else {

			//__raw_writel(0x64646464, (0x80000000000 + 0x10));
			//__raw_writel(0x64646464, (PCIE_SUBSYS_ADDR_START + 512*GB + 0x10));
			printf("ep start\n");

			// sends test data to socket0's ddr
			__raw_writel(0x11111111, 0x10);

			asm volatile("fence rw, rw\n\t");

			val = 0;
			while (val != 0x11111111)
				val = __raw_readl(0x10 + 0x80000000000);

			printf("ep end\n");
		}
	}
	//int sub_bus;
	//sub_bus = pci_enum(1);
	// TODO: construct(0, 1, sub_bus), and updata HB bus resource(0x18) via dbi bus

#ifdef CONFIG_DUOWEN_PCIE_TEST
	printf("bird: PCIE TEST start\n");
	// find which controller is in use, and enable its MSI int
	controller = pcie_subsys->controller;
	for(i = 0; i < pcie_subsys->ctrl_cnt; i++) {
		if (controller->active == true)
			break;
		controller++;
	}
#ifdef CONFIG_DW_PCIE_RC
	uint64_t val;

	irqc_configure_irq(IRQ_PCIE_X16_MSI, 0, IRQ_LEVEL_TRIGGERED);
	irq_register_vector(IRQ_PCIE_X16_MSI, duowen_pcie_msi_handler);
	irqc_enable_irq(IRQ_PCIE_X16_MSI);

	irqc_configure_irq(IRQ_PCIE_X16_INTA, 0, IRQ_LEVEL_TRIGGERED);
	irq_register_vector(IRQ_PCIE_X16_INTA, duowen_pcie_inta_handler);
	irqc_enable_irq(IRQ_PCIE_X16_INTA);

	dw_pcie_enable_msi(&(controller->pp));

	// trigger EP VIP MSI interrupt
	__raw_writel(0x3, MSG_REG(0x80));
	// trigger EP VIP INTA interrupt
	__raw_writel(0x2, MSG_REG(0x80));

	__raw_writel(0x64646464, (PCIE_SUBSYS_ADDR_START + 0x10));
	val = __raw_readl(PCIE_SUBSYS_ADDR_START + 0x10);
	if (val == 0x64646464)
		printf("MEM64 Read/Write transaction passed\n");

	__raw_writel(0x32323232, (PCIE_SUBSYS_ADDR_START + 0x100));
	val = __raw_readl(PCIE_SUBSYS_ADDR_START + 0x100);
	if (val == 0x32323232)
		printf("MEM32 Read/Write transaction passed\n");

#define where(bus, device, func, reg) ((bus << 20) | (device << 15) | (func << 12) | (reg << 2))
	// cfg0 read
	val = __raw_readl(PCIE_CORE_X16_CFG0_START + 0x0);
	printf("cfg0: %x\n", val);

	// cfg1 read
	val = __raw_readl(PCIE_CORE_X16_CFG1_START + 0x100000);
	printf("cfg1: %x\n", val);
#else
	// carry out EP DMA test
	dw_pcie_ep_dma_test(&(controller->pp));
#endif
#endif
}
