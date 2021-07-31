#ifndef __PCIE_IPDV_H_INCLUDE__
#define __PCIE_IPDV_H_INCLUDE__

#define IPBENCH				1

#define IPBENCH_BASE(port)		((port) << 28)
#define IPBENCH_PORT(addr)		((addr) >> 28)
#define IPBENCH_ADDR(addr)		((addr) & ~IPBENCH_BEASE(0xf))
#define COUNTER_ADDR			IPBENCH_BASE(0xf)

#define PCIE_SUB_CUST_BASE		IPBENCH_BASE(0x8)
#define PCIE0_CUST_BASE			IPBENCH_BASE(0x4)
#define CFG_APB_PHY(x)			0x0
#define CFG_AXI_CORE(x)			IPBENCH_BASE(x)

void apb_read_c(uint32_t addr, uint32_t *data, int port);
void apb_write_c(uint32_t addr, uint32_t data, int port);
void axi_read_c(uint64_t addr, uint32_t *data, int port);
void axi_write_c(uint64_t addr, uint32_t data, int port);

static inline uint32_t dw_pcie_read_apb(uint64_t addr)
{
	uint32_t data;

	apb_read_c(IPBENCH_ADDR(addr), &data, IPBENCH_PORT(addr));
#ifdef CONFIG_DUOWEN_PCIE_DEBUG
	printf("dw_pcie: APB(R): 0x%llx=0x%08x\n", addr, data);
#endif
	return data;
}

static inline void dw_pcie_write_apb(uint64_t addr, uint32_t data)
{
#ifdef CONFIG_DUOWEN_PCIE_DEBUG
	printf("dw_pcie: APB(W): 0x%llx=0x%x\n", addr, data);
#endif
	apb_write_c(IPBENCH_ADDR(addr), data, IPBENCH_PORT(addr));
}

static inline uint32_t dw_pcie_read_axi(uint64_t addr)
{
	uint32_t data;

	axi_read_c(IPBENCH_ADDR(addr), &data, IPBENCH_PORT(addr));
#ifdef CONFIG_DUOWEN_PCIE_DEBUG
	printf("dw_pcie: AXI(R): 0x%llx=0x%x\n", addr, data);
#endif
	return data;
}

void dw_pcie_write_axi(uint64_t addr, uint32_t data)
{
#ifdef CONFIG_DUOWEN_PCIE_DEBUG
	printf("dw_pcie: AXI(W): 0x%llx=0x%x\n", addr, data);
#endif
	axi_write_c(IPBENCH_ADDR(addr), data, IPBENCH_PORT(addr));
}
#endif

#ifdef TEST
#define udelay(x)			usleep(x)
#define duowen_pcie_clock_init()	do { } while (0)
#define dw_pcie_sram_init()						\
	do {								\
		dw_pcie_write_apb(PCIE_SRAM_CONTROL,			\
				  DW_PCIE_phy_sram_ext_ld_done_all);	\
	} while (0)
#else
#define dw_pcie_sram_init()		__dw_pcie_sram_init()

static inline void udelay(uint32_t time)
{
	uint32_t current, next;

	current = dw_pcie_read_apb(COUNTER_ADDR, 8);
	next = current + time * 125;

	do {
		current = dw_pcie_read_apb(COUNTER_ADDR, 8);
	} while (current < next);
}

static inline void duowen_pcie_clock_init(void)
{
	clk_enable(pcie_aclk);
	clk_enable(pcie_pclk);
	clk_enable(pcie_aux_clk);
	clk_enable(pcie_alt_ref_clk);
}
#endif

#endif /* __PCIE_IPDV_H_INCLUDE__ */
