#include <target/pci.h>
#ifdef SDFIRM
#include <target/clk.h>
#include <target/irq.h>
#include <target/console.h>
#endif

struct dw_pcie duowen_pcie_ctrls[PCIE_MAX_CORES] = {
	/* X16 */
	[0] = {
		.dbi_base = CFG_AXI_CORE(0),
		.pp.cfg_bar0 = PCIE_CORE_CFG0_START,
		.pp.cfg_bar1 = PCIE_CORE_CFG1_START,
		.pp.cfg_size = PCIE_CORE_CFG_SIZE,
		.pp.mem_base = 0,
		.pp.mem_size = PCIE_CORE_MEM_SIZE,
		.pp.chiplink = 0,
		.pp.role = ROLE_RC,
	},
	/* X8 */
	[1] = {
		.dbi_base = CFG_AXI_CORE(1),
		.pp.cfg_bar0 = PCIE_CORE_CFG0_START,
		.pp.cfg_bar1 = PCIE_CORE_CFG1_START,
		.pp.cfg_size = PCIE_CORE_CFG_SIZE,
		.pp.mem_base = 0,
		.pp.mem_size = PCIE_CORE_MEM_SIZE,
		.pp.chiplink = 0,
		.pp.role = ROLE_RC,
	},
	/* X4_0 */
	[2] = {
		.dbi_base = CFG_AXI_CORE(2),
		.pp.cfg_bar0 = PCIE_CORE_CFG0_START,
		.pp.cfg_bar1 = PCIE_CORE_CFG1_START,
		.pp.cfg_size = PCIE_CORE_CFG_SIZE,
		.pp.mem_base = 0,
		.pp.mem_size = PCIE_CORE_MEM_SIZE,
		.pp.chiplink = 0,
		.pp.role = ROLE_RC,
	},
	/* X4_1 */
	[PCIE_CORE_CHIPLINK] = {
		.dbi_base = CFG_AXI_CORE(3),
		.pp.cfg_bar0 = PCIE_CORE_CFG0_START,
		.pp.cfg_bar1 = PCIE_CORE_CFG1_START,
		.pp.cfg_size = PCIE_CORE_CFG_SIZE,
		.pp.mem_base = 0,
		.pp.mem_size = PCIE_CORE_MEM_SIZE,
		.pp.chiplink = 0,
		.pp.role = ROLE_RC,
	},
};
static uint8_t duowen_pcie_linkmode;

#ifndef CONFIG_DUOWEN_PCIE_IPDV
uint32_t dw_pcie_read_apb(uint64_t addr)
{
	uint32_t data;

	data = __raw_readl(addr);
#ifdef CONFIG_DUOWEN_PCIE_DEBUG
	con_dbg("dw_pcie: APB(R): 0x%llx=0x%08x\n", addr, data);
#endif
	return data;
}

void dw_pcie_write_apb(uint64_t addr, uint32_t data)
{
#ifdef CONFIG_DUOWEN_PCIE_DEBUG
	con_dbg("dw_pcie: APB(W): 0x%llx=0x%x\n", addr, data);
#endif
	__raw_writel(data, addr);
}

uint32_t dw_pcie_read_axi(uint64_t addr)
{
	uint32_t data;

	data = __raw_readl(addr);
#ifdef CONFIG_DUOWEN_PCIE_DEBUG
	con_dbg("dw_pcie: AXI(R): 0x%llx=0x%x\n", addr);
#endif
	return data;
}

void dw_pcie_write_axi(uint64_t addr, uint32_t data)
{
#ifdef CONFIG_DUOWEN_PCIE_DEBUG
	con_dbg("dw_pcie: AXI(W): 0x%llx=0x%x\n", addr, data);
#endif
	__raw_writel(data, addr);
}

void duowen_pcie_clock_init(void)
{
	clk_enable(pcie_aclk);
	clk_enable(pcie_pclk);
	clk_enable(pcie_aux_clk);
	clk_enable(pcie_alt_ref_clk);
}
#endif

uint32_t dw_get_pci_conf_reg(int bus, int dev, int fun, int reg,
			     uint8_t index)
{
	struct pcie_port *pp = &(duowen_pcie_ctrls[index].pp);
	uint64_t pci_addr, cpu_addr, base;
	uint32_t val;

	if (bus > 1)
		cpu_addr = pp->cfg_bar1;
	else
		cpu_addr = pp->cfg_bar0;
	pci_addr = form_pci_addr(bus, dev, fun);

	base = PCIE_SUBSYS_ADDR_START + PCIE_CORE_ADDR_START(index);
	val = __raw_readl(base + cpu_addr + pci_addr + reg);
	return val;
}

void dw_set_pci_conf_reg(int bus, int dev, int fun, int reg,
			 uint32_t val, uint8_t index)
{
	struct pcie_port *pp = &(duowen_pcie_ctrls[index].pp);
	uint64_t pci_addr, cpu_addr, base;

	if (bus > 1)
		cpu_addr = pp->cfg_bar1;
	else
		cpu_addr = pp->cfg_bar0;
	pci_addr = form_pci_addr(bus, dev, fun);

	base = PCIE_SUBSYS_ADDR_START + PCIE_CORE_ADDR_START(index);
	__raw_writel(val, base + cpu_addr + pci_addr + reg);
}

uint8_t duowen_pcie_link_modes[] = {
	[0] = DUOWEN_PCIE_LINK_MODE_4_4_4_4,
	[1] = DUOWEN_PCIE_LINK_MODE_8_4_0_4,
	[2] = DUOWEN_PCIE_LINK_MODE_8_8_0_0,
	[3] = DUOWEN_PCIE_LINK_MODE_16_0_0_0,
};

/* Converts ROM link mode to PCIe link mode */
static uint8_t duowen_pcie_link_mode(uint8_t mode)
{
	uint8_t i;

	for (i = 0; i < ARRAY_SIZE(duowen_pcie_link_modes); i++) {
		if (duowen_pcie_link_modes[i] == mode)
			return i;
	}
	return DW_PCIE_LINK_MODE_INVALID;
}

void duowen_pcie_config_info(int index, uint32_t value)
{
	if (duowen_pcie_ctrls[index].pp.role == ROLE_EP)
		dw_pcie_write_apb(PCIE_CFGINFO_LEVEL(index),
				  value |
				  DW_PCIE_device_type(DW_PCIE_ENDPOINT));
	else
		dw_pcie_write_apb(PCIE_CFGINFO_LEVEL(index),
				  value |
				  DW_PCIE_device_type(DW_PCIE_ROOT_COMPLEX));
}

void duowen_pcie_wait_linkup(int index)
{
	uint32_t data;

	con_log("dw_pcie: Waiting for controller %d smlh&rdlh ready\n", index);
	do {
		data = dw_pcie_read_apb(PCIE_LINK_STATUS(index));
	} while ((data & DW_PCIE_link_up) != DW_PCIE_link_up);
}

static void duowen_pcie_pre_reset(void)
{
	int i;

	/* #10ns */
	dw_pcie_write_apb(PCIE_SUBSYSTEM_CONTROL,
			  duowen_pcie_link_mode(duowen_pcie_linkmode));
	for (i = 0; i < PCIE_MAX_CORES; i++)
		dw_pcie_write_apb(PCIE_RESET_CONTROL(i),
				  DW_PCIE_RESET_CTRL_ALL);
}

static void duowen_pcie_post_reset(void)
{
	dw_pcie_write_apb(PCIE_RESET_CONTROL_PHY, DW_PCIE_phy_reset);
	dw_pcie_write_apb(PCIE_SRAM_CONTROL, DW_PCIE_phy_sram_bypass_none);
	dw_pcie_write_apb(PCIE_REFCLK_CONTROL,
			  DW_PCIE_ref_clk_en | DW_PCIE_ref_use_pad);
	/* #200ns */
	dw_pcie_write_apb(PCIE_RESET_CONTROL_PHY, DW_PCIE_phy_rst_all);
	/* #100ns */
	dw_pcie_sram_init();
}

static void duowen_pcie_init_ctrls(void)
{
	int i;

	for (i = 0; i < PCIE_MAX_CORES; i++) {
		if (duowen_pcie_ctrls[i].active)
			duowen_pcie_config_info(i, DW_PCIE_LTSSM_DETECT);
	}
}

static void duowen_pcie_reset_ctrls(void)
{
	int i;

	for (i = 0; i < PCIE_MAX_CORES; i++) {
		if (duowen_pcie_ctrls[i].active)
			duowen_pcie_config_info(i, DW_PCIE_LTSSM_ENABLE);
	}
}

void duowen_pcie_cfg_init(int socket_id, bool chiplink)
{
	int i;
	int order = 0;
	uint8_t linkmode = rom_get_pcie_link_mode();
	uint8_t mode;

	/* Configuring duowen_pcie */
	if (duowen_pcie_link_mode(linkmode) == DW_PCIE_LINK_MODE_INVALID) {
		if (chiplink)
			linkmode = DUOWEN_PCIE_LINK_MODE_CHIPLINK;
		else
			linkmode = DUOWEN_PCIE_LINK_MODE_DEFAULT;
	}
	duowen_pcie_linkmode = linkmode;

	/* Configuring dw_pcie */
	for (i = 0; i < PCIE_MAX_CORES; i++) {
		duowen_pcie_ctrls[i].dbi_base += SOC_BASE;
		duowen_pcie_ctrls[i].pp.cfg_bar0 += SOC_BASE;
		duowen_pcie_ctrls[i].pp.cfg_bar1 += SOC_BASE;
		mode = rom_pcie_link_ctrl(i, linkmode);
		if (mode == DUOWEN_PCIE_LINK_MODE_0) {
			duowen_pcie_ctrls[i].active = false;
			duowen_pcie_ctrls[i].lane_num = 0;
			duowen_pcie_ctrls[i].order = 0xff;
			continue;
		}
		duowen_pcie_ctrls[i].active = true;
		duowen_pcie_ctrls[i].lane_num = DUOWEN_PCIE_LINK_LANE(mode);
		duowen_pcie_ctrls[i].order = order++;
	}
	if (chiplink)
		duowen_pcie_ctrls[PCIE_CORE_CHIPLINK].pp.chiplink = 1;
	if (socket_id)
		duowen_pcie_ctrls[PCIE_CORE_CHIPLINK].pp.role = ROLE_EP;
}

#ifdef CONFIG_DUOWEN_PCIE_TEST
void duowen_pcie_handle_msi(bool en)
{
	int i;

	for (i = 0; i < PCIE_MAX_CORES; i++) {
		if (duowen_pcie_ctrls[i].active) {
			/* We just mask this interrupt, since there is no
			 * register we can manipulate to clear the
			 * interrupt from VIP.
			 */
			__raw_writel(en ? 0 : 1,
				     PCIE_MSI_INT_STATUS_MASK(i));
			break;
		}
	}

}

void duowen_pcie_handle_inta(bool en)
{
	/* en=true:  trigger assert INTA msg
	 * en=false: trigger de-assert INTA msg
	 */
	__raw_writel(en ? 0x2 : 0x4, MSG_REG(0x80));
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

#ifdef CONFIG_DW_PCIE_RC
static void __duowen_pcie_test(struct pcie_port *pp)
{
	uint64_t val;

	irqc_configure_irq(IRQ_PCIE_X16_MSI, 0, IRQ_LEVEL_TRIGGERED);
	irq_register_vector(IRQ_PCIE_X16_MSI, duowen_pcie_msi_handler);
	irqc_enable_irq(IRQ_PCIE_X16_MSI);

	irqc_configure_irq(IRQ_PCIE_X16_INTA, 0, IRQ_LEVEL_TRIGGERED);
	irq_register_vector(IRQ_PCIE_X16_INTA, duowen_pcie_inta_handler);
	irqc_enable_irq(IRQ_PCIE_X16_INTA);

	dw_pcie_enable_msi(pp);

	/* trigger EP VIP MSI interrupt */
	__raw_writel(0x3, MSG_REG(0x80));
	/* trigger EP VIP INTA interrupt */
	__raw_writel(0x2, MSG_REG(0x80));

	__raw_writel(0x64646464, (PCIE_SUBSYS_ADDR_START + 0x10));
	val = __raw_readl(PCIE_SUBSYS_ADDR_START + 0x10);
	if (val == 0x64646464)
		printf("MEM64 Read/Write transaction passed\n");

	__raw_writel(0x32323232, (PCIE_SUBSYS_ADDR_START + 0x100));
	val = __raw_readl(PCIE_SUBSYS_ADDR_START + 0x100);
	if (val == 0x32323232)
		printf("MEM32 Read/Write transaction passed\n");

	/* cfg0 read */
	val = __raw_readl(PCIE_SUBSYS_ADDR_START +
			  PCIE_CORE_ADDR_START(PCIE_CORE_X16) +
			  PCIE_CORE_CFG0_START + 0x0);
	printf("cfg0: %x\n", val);

	/* cfg1 read */
	val = __raw_readl(PCIE_SUBSYS_ADDR_START +
			  PCIE_CORE_ADDR_START(PCIE_CORE_X16) +
			  PCIE_CORE_CFG1_START + 0x100000);
	printf("cfg1: %x\n", val);
}
#else
static void __duowen_pcie_test(struct pcie_port *pp)
{
	/* Carry out EP DMA test */
	dw_pcie_ep_dma_test(pp);
}
#endif

static void duowen_pcie_test(void)
{
	int i;

	printf("bird: PCIE TEST start\n");
	/* Find which controller is in use, and enable its MSI int */
	for (i = 0; i < PCIE_MAX_CORES; i++) {
		if (duowen_pcie_ctrls[i].active) {
			__duowen_pcie_test(&(duowen_pcie_ctrls[i].pp));
			break;
		}
	}
}
#else
#define duowen_pcie_test()		do { } while (0)
#endif

void pci_platform_init(void)
{
	int i, chiplink = 0, socket_id = 0;
	volatile uint32_t val;

#ifdef CONFIG_DUOWEN_PCIE_CHIPLINK
	chiplink = imc_chip_link();
	socket_id = imc_socket_id();

	if (chiplink) {
		if (rom_get_chiplink_ready())
			return;
		rom_set_chiplink_ready();
	}
#endif
	duowen_pcie_cfg_init(socket_id, chiplink);

	duowen_pcie_clock_init();
	duowen_pcie_pre_reset();
	duowen_pcie_init_ctrls();
	duowen_pcie_post_reset();
	for (i = 0; i < PCIE_MAX_CORES; i++) {
		if (duowen_pcie_ctrls[i].active)
			dw_pcie_setup_ctrl(&(duowen_pcie_ctrls[i].pp));
	}
	duowen_pcie_reset_ctrls();
	/* Wait until chiplink is established */
	if (chiplink)
		duowen_pcie_wait_linkup(PCIE_CORE_CHIPLINK);

#ifdef CONFIG_DUOWEN_PCIE_SMMU_BYPASS
	/* FIXME: chiplink and socket ID support
	 *
	 * Need DMA coherence working here by bypassing pcie SMMU. Should
	 * be done by an SMMU API to support chiplink and socket ID
	 * configuration.
	 */
	__raw_writel(0x9f0001, (caddr_t)0xff08400000);
#endif

#ifdef CONFIG_DUOWEN_PCIE_ENUM
	for (i = 0; i < PCIE_MAX_CORES; i++) {
		if (duowen_pcie_ctrls[i].active)
			pci_enum(1, i);
	}
#endif

	/* This part carry out an simple communication test between dual
	 * sockets via X4_1 ctrls in both side.
	 * The way is as follows:
	 * -- Socket0 sends 0x11111111 to 0x80000000010 which is in remote
	 *    socket1 DDR range;
	 * -- Socket1 sends 0x11111111 to 0x10 which is in remote socket0
	 *    DDR range;
	 * -- Socket0 read data from addr 0x10 in its local DDR range
	 * -- Socket1 read data from addr 0x80000000010 in its local DDR
	 *    range
	 * -- When both side get the data equals to 0x11111111, test passed
	 */
	if (chiplink) {
		if (!socket_id) {
			con_log("dw_pcie: chiplink RC start\n");

#if 0
			struct dw_pcie *pci =
				to_dw_pcie_from_pp(&(duowen_pcie_ctrls[0].pp));

			dw_pcie_write_dbi(pci, DW_PCIE_CDM, 0x20, 0x0, 0x4);
			setup_ep();
			/* ep BAR */
			dw_set_pci_conf_reg(1, 0, 0, 0x10, 0x0, index);
			/* ep command reg */
			val = dw_get_pci_conf_reg(1, 0, 0, 0x4, index);
			val |= 0x6;
			dw_set_pci_conf_reg(1, 0, 0, 0x4, val, index);
#endif

			/* Sends test data to socket1's DDR */
			__raw_writel(0x11111111, (0x10 + 0x80000000000));
			asm volatile("fence rw, rw\n\t");
			do {
				val = __raw_readl(0x10);
			} while (val != 0x11111111);

			con_log("dw_pcie: chiplink RC end\n");
		} else {
			con_log("dw_pcie: chiplink EP start\n");

			/* Sends test data to socket0's DDR */
			__raw_writel(0x11111111, 0x10);
			asm volatile("fence rw, rw\n\t");
			do {
				val = __raw_readl(0x10 + 0x80000000000);
			} while (val != 0x11111111);

			con_log("dw_pcie: chiplink EP end\n");
		}
	}

#if 0
	int sub_bus = pci_enum(1);
	/* TODO:
	 * Construct(0, 1, sub_bus), and updata HB bus resource(0x18) via
	 * dbi bus.
	 */
#endif
	duowen_pcie_test();
}
