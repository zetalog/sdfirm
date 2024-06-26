#include <target/lpc.h>
#include <target/cmdline.h>
#include <target/irq.h>
#include <target/panic.h>

#ifdef SYS_REALTIME
#define lpc_poll_init()		spacemit_lpc_poll_init()
#define lpc_irq_init()		do { } while (0)
#else /* SYS_REALTIME */
#define lpc_poll_init()		do { } while (0)
#define lpc_irq_init()		spacemit_lpc_irq_init()
#endif /* SYS_REALTIME */

bh_t lpc_bh;
uint8_t lpc_state;
uint8_t lpc_event;

#define LPC_OP_WAIT		_BV(0)
#define LPC_SERIRQ_EVENT	_BV(1)

static void lpc_raise_event(uint8_t event)
{
	lpc_event |= event;
	bh_resume(lpc_bh);
}

static void lpc_clear_event(uint8_t event)
{
	lpc_event &= ~event;
}

static void lpc_start_serirq(void)
{
	lpc_serirq_start();
	lpc_raise_event(LPC_SERIRQ_EVENT);
}

static void lpc_stop_serirq(void)
{
	lpc_clear_event(LPC_SERIRQ_EVENT);
}

static void lpc_handle_serirq(void)
{
	uint8_t serirq;

	for (serirq = 0; serirq < LPC_HW_SERIRQ_NUM; serirq++) {
		if (lpc_get_serirq(serirq)) {
			con_log("lpc: LPC_INT_SERIRQ_INT: %d\n", serirq);
			lpc_clear_serirq(serirq);
		}
	}
}

static void lpc_handle_irq(irq_t irq)
{
	uint32_t sts;
	uint32_t status;
	uint32_t serirq;

	sts = lpc_get_int_status();
	status = sts & LPC_INT_OP_STATUS;
	serirq = sts & (LPC_INT_SERIRQ_INT | LPC_INT_SERIRQ_DONE);
	if (status) {
		if (status & LPC_INT_SYNC_ERR)
			con_err("lpc: LPC_INT_SYNC_ERR\n");
		if (status & LPC_INT_NO_SYNC)
			con_err("lpc: LPC_INT_NO_SYNC\n");
		if (status & LPC_INT_LWAIT_TIMEOUT)
			con_err("lpc: LPC_INT_LWAIT_TIMEOUT\n");
		if (status & LPC_INT_SWAIT_TIMEOUT)
			con_err("lpc: LPC_INT_SWAIT_TIMEOUT\n");
		if (status & LPC_INT_OP_DONE)
			con_dbg("lpc: LPC_INT_OP_DONE\n");
		__raw_setl(status, LPC_INT_CLR);
		lpc_clear_event(LPC_OP_WAIT);
	}
	if (serirq) {
		if (serirq & LPC_INT_SERIRQ_INT)
			lpc_handle_serirq();
		if (serirq & LPC_INT_SERIRQ_DONE) {
			con_dbg("lpc: LPC_INT_SERIRQ_DONE\n");
			lpc_stop_serirq();
		}
		__raw_setl(serirq, LPC_INT_CLR);
	}
	irqc_ack_irq(LPC_IRQ);
}

static void lpc_bh_handler(uint8_t events)
{
	if (events == BH_POLLIRQ) {
		lpc_handle_irq(LPC_IRQ);
		return;
	}
}

static void lpc_sync(void)
{
	do {
		bh_sync();
		if (lpc_event)
			bh_resume(lpc_bh);
	} while (lpc_event);
}

#ifndef CONFIG_SPACEMIT_LPC_BRIDGE
void lpc_io_write8(uint8_t v, uint16_t a)
{
	BUG_ON(lpc_event & LPC_OP_WAIT);
	lpc_raise_event(LPC_OP_WAIT);
	__lpc_io_write8(v, a);
	lpc_sync();
}

uint8_t lpc_io_read8(uint16_t a)
{
	BUG_ON(lpc_event & LPC_OP_WAIT);
	lpc_raise_event(LPC_OP_WAIT);
	__lpc_io_read8(a);
	lpc_sync();
	return __raw_readl(LPC_RDATA);
}

void lpc_mem_write8(uint8_t v, uint32_t a)
{
	BUG_ON(lpc_event & LPC_OP_WAIT);
	lpc_raise_event(LPC_OP_WAIT);
	__lpc_mem_write8(v, a);
	lpc_sync();
}

uint8_t lpc_mem_read8(uint32_t a)
{
	BUG_ON(lpc_event & LPC_OP_WAIT);
	lpc_raise_event(LPC_OP_WAIT);
	__lpc_mem_read8(a);
	lpc_sync();
	return __raw_readl(LPC_RDATA);
}
#endif /* CONFIG_SPACEMIT_LPC_BRIDGE */

void lpc_firm_write8(uint8_t v, uint32_t a)
{
	BUG_ON(lpc_event & LPC_OP_WAIT);
	lpc_raise_event(LPC_OP_WAIT);
	__lpc_firm_write8(v, a);
	lpc_sync();
}

uint8_t lpc_firm_read8(uint32_t a)
{
	BUG_ON(lpc_event & LPC_OP_WAIT);
	lpc_raise_event(LPC_OP_WAIT);
	__lpc_firm_read8(a);
	lpc_sync();
	return __raw_readl(LPC_RDATA);
}

void lpc_firm_write16(uint16_t v, uint32_t a)
{
	BUG_ON(lpc_event & LPC_OP_WAIT);
	lpc_raise_event(LPC_OP_WAIT);
	__lpc_firm_write16(v, a);
	lpc_sync();
}

uint16_t lpc_firm_read16(uint32_t a)
{
	BUG_ON(lpc_event & LPC_OP_WAIT);
	lpc_raise_event(LPC_OP_WAIT);
	__lpc_firm_read16(a);
	lpc_sync();
	return __raw_readl(LPC_RDATA);
}

void lpc_firm_write32(uint32_t v, uint32_t a)
{
	BUG_ON(lpc_event & LPC_OP_WAIT);
	lpc_raise_event(LPC_OP_WAIT);
	__lpc_firm_write32(v, a);
	lpc_sync();
}

uint32_t lpc_firm_read32(uint32_t a)
{
	BUG_ON(lpc_event & LPC_OP_WAIT);
	lpc_raise_event(LPC_OP_WAIT);
	__lpc_firm_read32(a);
	lpc_sync();
	return __raw_readl(LPC_RDATA);
}

#ifdef SYS_REALTIME
static void spacemit_lpc_poll_init(void)
{
	irq_register_poller(lpc_bh);
}
#else
static void spacemit_lpc_irq_init(void)
{
	lpc_mask_all_irqs();
	lpc_mask_all_serirqs();
	irqc_configure_irq(LPC_IRQ, 0, IRQ_LEVEL_TRIGGERED);
	irq_register_vector(LPC_IRQ, lpc_handle_irq);
	irqc_enable_irq(LPC_IRQ);
	/* TODO enable irqs */
}
#endif

#ifdef CONFIG_SPACEMIT_LPC_SERIRQ
static void spacemit_lpc_serirq_init(void)
{
	lpc_serirq_enable();
	__raw_writel_mask(SERIRQ_CFG_SERIRQ_NUM(SERIRQ_NUM(LPC_HW_SERIRQ_NUM)),
			  SERIRQ_CFG_SERIRQ_NUM(SERIRQ_CFG_SERIRQ_NUM_MASK),
			  SERIRQ_CFG);
	__raw_writel_mask(SERIRQ_CFG_SERIRQ_IDLE_WIDE(LPC_HW_SERIRQ_IDLE),
			  SERIRQ_CFG_SERIRQ_IDLE_WIDE(SERIRQ_CFG_SERIRQ_IDLE_WIDE_MASK),
			  SERIRQ_CFG);
	__raw_writel_mask(SERIRQ_CFG_SERIRQ_START_WIDE((LPC_HW_SERIRQ_START- 2) >> 1),
			  SERIRQ_CFG_SERIRQ_START_WIDE(SERIRQ_CFG_SERIRQ_START_WIDE_MASK),
			  SERIRQ_CFG);
	con_log("lpc: start serirq.\n");
}
#else
#define spacemit_lpc_serirq_init()		do { } while (0)
#endif

void spacemit_lpc_init(void)
{
	lpc_bh = bh_register_handler(lpc_bh_handler);
	lpc_irq_init();
	lpc_poll_init();
	lpc_mem_init();
	lpc_count_init();
	spacemit_lpc_serirq_init();
#if 0
	clk_enable(lpc_clk);
	clk_enable(lpc_lclk);
#endif
}

static int do_lpc_read(int argc, char *argv[])
{
	caddr_t addr;
	uint32_t val;

	if (argc < 3) 
		return -EINVAL;

	if (strcmp(argv[2], "fw") == 0) {
		if (argc < 4) 
			return -EINVAL;
		addr = (caddr_t)strtoull(argv[4], 0, 0);
		if (strcmp(argv[3], "1") == 0) {
			val = lpc_firm_read8(addr);
			printf("Firmware: 0x%08lx=%02x\n", addr, (uint8_t)val);
		} else if (strcmp(argv[3], "2") == 0) {
			val = lpc_firm_read16(addr);
			printf("Firmware: 0x%08lx=%04x\n", addr, (uint16_t)val);
		} else if (strcmp(argv[3], "4") == 0) {
			val = lpc_firm_read32(addr);
			printf("Firmware: 0x%08lx=%08x\n", addr, val);
		} else
			return -EINVAL;
		return 0;
	} else {
		addr = (caddr_t)strtoull(argv[3], 0, 0);
		if (strcmp(argv[2], "io") == 0) {
			val = lpc_io_read8(addr);
			printf("IO: 0x%08lx=%02x\n", addr, (uint8_t)val);
		} else if (strcmp(argv[2], "mem") == 0) {
			val = lpc_mem_read8(addr);
			printf("Memory: 0x%08lx=%02x\n", addr, (uint8_t)val);
		} else
			return -EINVAL;
		return 0;
	}
	return -EINVAL;
}

static int do_lpc_write(int argc, char *argv[])
{
	caddr_t addr;

	if (argc < 5)
		return -EINVAL;
	if (strcmp(argv[2], "fw") == 0) {
		uint32_t v;
		int size;
		if (argc < 6) 
			return -EINVAL;
		size = (uint32_t)strtoull(argv[3], 0, 0);
		v = (uint32_t)strtoull(argv[4], 0, 0);
		addr = (caddr_t)strtoull(argv[5], 0, 0);
		if (size == 1)
			lpc_firm_write8(v, addr);
		else if (size == 2)
			lpc_firm_write16(v, addr);
		else if (size == 4)
			lpc_firm_write32(v, addr);
		else
			return -EINVAL;
		return 0;
	} else {
		uint8_t v;

		v = (uint32_t)strtoull(argv[3], 0, 0);
		addr = (caddr_t)strtoull(argv[4], 0, 0);
		if (strcmp(argv[2], "io") == 0)
			lpc_io_write8(v, addr);
		else if (strcmp(argv[2], "mem") == 0)
			lpc_mem_write8(v, addr);
		else
			return -EINVAL;
		return 0;
	}
	return -EINVAL;
}

static int do_lpc_irq(int argc, char *argv[])
{
	uint8_t irq;
	if (argc < 4)
		return -EINVAL;
	lpc_mask_irq(1);
	irq = (uint8_t)strtoull(argv[3], 0, 0);
	if (strcmp(argv[2], "mask") == 0)
		lpc_mask_irq(irq);
	else if (strcmp(argv[2], "unmask") == 0)
		lpc_unmask_irq(irq);
	else if (strcmp(argv[2], "clear") == 0)
		lpc_clear_int(_BV(irq));
	else if (strcmp(argv[2], "get") == 0)
		return lpc_get_irq(irq);
	else
		return -EINVAL;
	return 0;
}

#ifdef CONFIG_SPACEMIT_LPC_SERIRQ
static int do_lpc_serirq(int argc, char *argv[])
{
	uint8_t slot;
	uint8_t mode;

	if (strcmp(argv[2], "start") == 0) {
		lpc_start_serirq();
		return 0;
	}

	if (argc < 3)
		return -EINVAL;
	if (strcmp(argv[2], "config") == 0) {
		mode = (uint8_t)strtoull(argv[3], 0, 0);
		printf("Enter %s mode.\n", mode ? "quiet" : "continuous");
		lpc_serirq_config(mode);
		return 0;
	}

	if (argc < 4)
		return -EINVAL;
	slot = (uint8_t)strtoull(argv[3], 0, 0);
	if (strcmp(argv[2], "mask") == 0)
		lpc_mask_serirq(slot);
	else if (strcmp(argv[2], "unmask") == 0)
		lpc_unmask_serirq(slot);
	else if (strcmp(argv[2], "clear") == 0)
		lpc_clear_serirq(slot);
	else if (strcmp(argv[2], "get") == 0)
		return lpc_get_serirq(slot);
	else
		return -EINVAL;
	return 0;
}
#else
static int do_lpc_serirq(int argc, char *argv[])
{
	return -EINVAL;
}
#endif

static int do_lpc_trans(int argc, char *argv[])
{
	uint32_t address;
	uint8_t address0, address1;

	if (argc < 4)
		return -EINVAL;
	address = (uint32_t)strtoull(argv[3], 0, 0);
	address0 = HIBYTE(HIWORD(address));

	if (argc > 4)
		address = (uint32_t)strtoull(argv[4], 0, 0);
	else
		address = address0 + LPC_MEM_SIZE;
	address1 = HIBYTE(HIWORD(address));
	lpc_mem_cfg(0, (uint8_t)strtoull(argv[2], 0, 0),
		    address0, address1);
	return 0;
}

static int do_lpc(int argc, char *argv[])
{
	if (argc < 2)
		return -EINVAL;
	if (strcmp(argv[1], "read") == 0)
		return do_lpc_read(argc, argv);
	if (strcmp(argv[1], "write") == 0)
		return do_lpc_write(argc, argv);
	if (strcmp(argv[1], "irq") == 0)
		return do_lpc_irq(argc, argv);
	if (strcmp(argv[1], "serirq") == 0)
		return do_lpc_serirq(argc, argv);
	if (strcmp(argv[1], "trans") == 0)
		return do_lpc_trans(argc, argv);
	return -EINVAL;
}

DEFINE_COMMAND(lpc, do_lpc, "SpacemiT low pin count commands",
	"lpc read io <addr>\n"
	"lpc read mem <addr>\n"
	"lpc read fw [1|2|4] <addr>\n"
	"    -LPC read sequence\n"
	"lpc write io <value> <addr>\n"
	"lpc write mem <value> <addr>\n"
	"lpc write fw [1|2|4] <value> <addr>\n"
	"    -LPC write sequence\n"
	"lpc trans <address0> <address1> <cycle> [0|1]\n"
	"    -config LPC address translation\n"
	"lpc irq mask <irq>\n"
	"lpc irq unmask <irq>\n"
	"lpc irq clear <irq>\n"
	"lpc irq get <irq>\n"
	"    -LPC control IRQs\n"
	"lpc serirq mask <slot>\n"
	"lpc serirq unmask <slot>\n"
	"lpc serirq clear <slot>\n"
	"lpc serirq get <slot>\n"
	"    -LPC control SERIRQs\n"
	"lpc serirq config <mode>\n"
	"    -LPC configure SERIRQ mode\n"
	"lpc serirq start\n"
	"    -LPC start SERIRQ operation\n"
);
