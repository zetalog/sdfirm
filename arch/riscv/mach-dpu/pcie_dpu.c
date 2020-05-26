#include <target/pci.h>
#include <target/clk.h>
#include <target/irq.h>
#include <asm/mach/tcsr.h>

struct duowen_pcie_subsystem pcie_subsystem;

struct dw_pcie controllers[] = 
{
    // X16
    {
        .axi_dbi_port = AXI_DBI_PORT_X16,
        .dbi_base = CFG_AXI_CORE_X16,
        .pp.cfg_bar0 = 3*GB + 512*KB,
        .pp.cfg_bar1 = 3*GB + 1*MB,
        .pp.cfg_size = PCIE_CORE_CFG_SIZE,
        .pp.mem_base = 0,
        .pp.mem_size = 3*GB,
    },

    // X8
    {
        .axi_dbi_port = AXI_DBI_PORT_X8,
        .dbi_base = CFG_AXI_CORE_X8,
        .pp.cfg_bar0 = PCIE_CORE_X8_CFG0_START,
        .pp.cfg_bar1 = PCIE_CORE_X8_CFG1_START,
        .pp.cfg_size = PCIE_CORE_CFG_SIZE,
        .pp.mem_base = PCIE_CORE_X8_ADDR_START,
        .pp.mem_size = PCIE_CORE_MEM_SIZE,
    },

    // X4_0
    {
        .axi_dbi_port = AXI_DBI_PORT_X4_0,
        .dbi_base = CFG_AXI_CORE_X4_0,
        .pp.cfg_bar0 = PCIE_CORE_X4_0_CFG0_START,
        .pp.cfg_bar1 = PCIE_CORE_X4_0_CFG1_START,
        .pp.cfg_size = PCIE_CORE_CFG_SIZE,
        .pp.mem_base = PCIE_CORE_X4_0_ADDR_START,
        .pp.mem_size = PCIE_CORE_MEM_SIZE,
    },

    //X4_1
    {
        .axi_dbi_port = AXI_DBI_PORT_X4_1,
        .dbi_base = CFG_AXI_CORE_X4_1,
        .pp.cfg_bar0 = PCIE_CORE_X4_1_CFG0_START,
        .pp.cfg_bar1 = PCIE_CORE_X4_1_CFG1_START,
        .pp.cfg_size = PCIE_CORE_CFG_SIZE,
        .pp.mem_base = PCIE_CORE_X4_1_ADDR_START,
        .pp.mem_size = PCIE_CORE_MEM_SIZE,
    },
};

uint32_t read_apb(uint32_t addr, uint8_t port)
{
    uint32_t data;
#ifdef IPBENCH
    apb_read_c(addr, &data, port);
#else
    data = readl(addr);
#endif
    printf("ReadAPB: addr: 0x%08x; data: 0x%08x, port: %d\n", addr, data, port);
    return data;
}

void write_apb(uint64_t addr, uint32_t data, uint8_t port)
{
    printf("WriteAPB: addr: 0x%llx; data: 0x%x port: %d\n", addr, data, port);
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
    return DEFAULT_LINK_MODE;
}

void reset_init(struct duowen_pcie_subsystem *pcie_subsystem)
{
    uint64_t base = pcie_subsystem->cfg_apb[SUBSYS];
    uint8_t port = APB_PORT_SUBSYS;
    uint8_t mode = duowen_get_link_mode(pcie_subsystem);
    uint32_t data = 0;

    write_apb((base + RESET_PHY), 0x10, port);
    write_apb((base + SRAM_CONTROL), 0x0, port);
    write_apb((base + REFCLK_CONTROL), 0x2, port);
    // #200ns
    write_apb((base + RESET_PHY), 0xf, port);
    // #100ns

    if (mode == LINK_MODE_16_0_0_0)
        write_apb((base + RESET_CORE_X16), 0xff, port);
    else if (mode == LINK_MODE_8_8_0_0)
        write_apb((base + RESET_CORE_X8), 0xff, port);
    else if (mode == LINK_MODE_4_4_4_4)
        write_apb((base + RESET_CORE_X4_0), 0xff, port);
    else
        write_apb((base + RESET_CORE_X4_1), 0xff, port);

    while ((data & 0x1) != 0x1) {
        data = read_apb((base + SRAM_STATUS), port);
    }
    write_apb((base + SRAM_CONTROL), 0x1, port);
    //write_apb((base + RESET_PHY), 0xff, port);
}

static void subsys_link_init_pre(struct duowen_pcie_subsystem *pcie_subsystem)
{
    uint64_t base = pcie_subsystem->cfg_apb[SUBSYS];
    struct dw_pcie *controller = pcie_subsystem->controller;
    uint8_t port = APB_PORT_SUBSYS;
    uint8_t link_mode = pcie_subsystem->link_mode;

    //assert(link_mode != LINK_MODE_INVALID);

    // #10ns
    write_apb((base + SUBSYS_CONTROL), link_mode, port);

    switch (link_mode) {
    case LINK_MODE_4_4_4_4:     // 0: In DPU, X4_0
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
        port = APB_PORT_X4_0;
        break;
    case LINK_MODE_8_4_0_4:     // 1: In DPU, X4_1
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
        port = APB_PORT_X4_1;
        break;
    case LINK_MODE_8_8_0_0:     // 2: In DPU, X8
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
        port = APB_PORT_X8;
        break;
    case LINK_MODE_16_0_0_0:    //  3: In DPU: X16
#ifdef DPU
        (controller + X16)->lane_num = 4;
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
        port = APB_PORT_X16;
        break;
    }

    write_apb((base + 0), 0xc810010, port);
}

static void subsys_link_init_post(struct duowen_pcie_subsystem *pcie_subsys)
{
    uint8_t mode = duowen_get_link_mode(pcie_subsys);

    switch (mode) {
        case LINK_MODE_4_4_4_4:
            write_apb(pcie_subsys->cfg_apb[X4_0], 0xc018010, APB_PORT_X4_0);
#ifdef DPU
            break;
#endif
        case LINK_MODE_8_4_0_4:
            write_apb(pcie_subsys->cfg_apb[X4_1], 0xc018010, APB_PORT_X4_1);
#ifdef DPU
            break;
#endif
        case LINK_MODE_8_8_0_0:
            write_apb(pcie_subsys->cfg_apb[X8], 0xc018010, APB_PORT_X8);
#ifdef DPU
            break;
#endif
        case LINK_MODE_16_0_0_0:
            write_apb(pcie_subsys->cfg_apb[X16], 0xc018010, APB_PORT_X16);
#ifdef DPU
            break;
#endif
        break;
    }
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
    for(i = 0; i < sizeof(controllers)/sizeof(struct dw_pcie); i++)
        dw_controller_init(pcie_subsystem->controller + i);
}

void clock_init(void)
{
    clk_enable(pcie0_aclk);
    clk_enable(pcie0_pclk);
    clk_enable(pcie0_aux_clk);
    clk_enable(pcie0_ref_clk);
    //__raw_writel(0x2, 0x401405c);

    clk_enable(srst_pcie0);
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
    else    // trigger de-assert INTA msg
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

void dpu_pcie_msi_handler(void)
{
    printf("bird: Receive PCIE MSI interrupt\n");
	irqc_mask_irq(IRQ_PCIE_X16_MSI);
    dpu_pcie_handle_irq(MSI_INT);
    irqc_unmask_irq(IRQ_PCIE_X16_MSI);
    irqc_ack_irq(IRQ_PCIE_X16_MSI);
}

void dpu_pcie_inta_handler(void)
{
    printf("bird: Receive PCIE INTA interrupt\n");
	irqc_mask_irq(IRQ_PCIE_X16_INTA);
    dpu_pcie_handle_irq(INTA_INT);
    irqc_unmask_irq(IRQ_PCIE_X16_INTA);
    irqc_ack_irq(IRQ_PCIE_X16_INTA);
}
#endif

//#define TCSR_BASE 0x4100000
void pci_platform_init(void)
{
    struct duowen_pcie_subsystem *pcie_subsys;
    struct dw_pcie *controller;
    int i;
    //uint32_t base = TCSR_BASE, val;
    uint64_t val;

    printf("bird: PCIE start\n");
    //imc_addr_trans(0, 0x20000000, 0xc00000000, 0);

    pcie_subsys = &pcie_subsystem;
    instance_subsystem(pcie_subsys);

    clock_init();
    subsys_link_init_pre(pcie_subsys);
    reset_init(pcie_subsys);
    
    controller = pcie_subsys->controller;

    for(i = 0; i < sizeof(controllers)/sizeof(struct dw_pcie); i++) {
        if (controller->active == true) {
            dw_pcie_setup_rc(&(controller->pp));
            controller++;
        }
    }

    subsys_link_init_post(pcie_subsys);

    //__raw_writel(0xa0, base + 0x40);
    //__raw_writel(0x80000c00, base + 0x44);

#ifdef CONFIG_DPU_PCIE_TEST
    irqc_configure_irq(IRQ_PCIE_X16_MSI, 0, IRQ_LEVEL_TRIGGERED);
    irq_register_vector(IRQ_PCIE_X16_MSI, dpu_pcie_msi_handler);
    irqc_enable_irq(IRQ_PCIE_X16_MSI);

    irqc_configure_irq(IRQ_PCIE_X16_INTA, 0, IRQ_LEVEL_TRIGGERED);
    irq_register_vector(IRQ_PCIE_X16_INTA, dpu_pcie_inta_handler);
    irqc_enable_irq(IRQ_PCIE_X16_INTA);

    // find which controller is in use, and enable its MSI int
    for(i = 0; i < sizeof(controllers)/sizeof(struct dw_pcie); i++) {
        if (controller->active == true)
            break;
    }
    dw_pcie_enable_msi(&(controller->pp));

    // trigger EP VIP MSI interrupt
    __raw_writel(0x3, TCSR_MSG_REG(0x80));
    // trigger EP VIP INTA interrupt
    __raw_writel(0x2, TCSR_MSG_REG(0x80));

    __raw_writeq(0x64646464, 0xc00000010);
    val = __raw_readq(0xc00000010);
    if (val == 0x64646464)
        printf("MEM64 Read/Write transaction passed\n");

    __raw_writel(0x32323232, 0xc00000100);
    val = __raw_readl(0xc00000100);
    if (val == 0x32323232)
        printf("MEM32 Read/Write transaction passed\n");

    // cfg0 read
    val = __raw_readl(0xcc0080000);
    printf("cfg0: %x\n", val);

    // cfg1 read
    val = __raw_readl(0xc00100000);
    printf("cfg1: %x\n", val);
#endif
}
