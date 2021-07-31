#ifndef __PCIE_IPDV_H_INCLUDE__
#define __PCIE_IPDV_H_INCLUDE__

#define IPBENCH				1

#define IPBENCH_BASE(port)		((port) << 28)
#define IPBENCH_PORT(addr)		((addr) >> 28)
#define IPBENCH_ADDR(addr)		((addr) & ~IPBENCH_BEASE(0xf))
#define COUNTER_ADDR			IPBENCH_BASE(0xf)

#define CFG_APB_SUBSYS			IPBENCH_BASE(0x8)
#define CFG_APB_CORE(x)			IPBENCH_BASE(0x4 + (x))
#define CFG_APB_PHY(x)			0x0
#define CFG_AXI_CORE(x)			IPBENCH_BASE(x)

void apb_read_c(uint32_t addr, uint32_t *data, int port);
void apb_write_c(uint32_t addr, uint32_t data, int port);

static inline uint32_t dw_pcie_read_apb(uint64_t addr)
{
	uint32_t data;

	apb_read_c(IPBENCH_ADDR(addr), &data, IPBENCH_PORT(addr));
#ifdef CONFIG_DUOWEN_PCIE_DEBUG
	printf("dw_pcie: APB(R): addr: 0x%llx; data: 0x%08x\n",
	       addr, data);
#endif
	return data;
}

static inline void dw_pcie_write_apb(uint64_t addr, uint32_t data)
{
#ifdef CONFIG_DUOWEN_PCIE_DEBUG
	printf("dw_pcie: APB(W): addr: 0x%llx; data: 0x%x\n",
	       addr, data);
#endif
	apb_write_c(IPBENCH_ADDR(addr), data, IPBENCH_PORT(addr));
}

static inline uint32_t get_counter(void)
{
	uint32_t cnt;

	cnt = dw_pcie_read_apb(COUNTER_ADDR, 8);
	return cnt;
}

static inline void udelay(uint32_t time)
{
	uint32_t current, next;

	current = get_counter();
	next = current + time * 125;

	do {
		current =  get_counter();
	} while(current < next);
}

#endif /* __PCIE_IPDV_H_INCLUDE__ */
