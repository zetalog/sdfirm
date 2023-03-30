#include <target/spiflash.h>
#include <target/cmdline.h>
#include <target/mem.h>
#include <target/uefi.h>
#include <target/irq.h>
#include <target/smp.h>
#include <asm/mach/boot.h>

#define FLASH_TOTAL_SIZE	4000000

mtd_t board_flash = INVALID_MTD_ID;

void dpu_ssi_flash_init(void)
{
	board_flash = spiflash_register_bank(0);
}

typedef void (*dpu_boot_cb)(void *, uint32_t, uint32_t);

static __always_inline void dpu_ssi_flash_select(uint32_t chips)
{
	__raw_writel(chips, SSI_SER(0));
}

static __always_inline void dpu_ssi_flash_writeb(uint8_t byte)
{
	while (!(__raw_readl(SSI_RISR(0)) & SSI_TXEI));
	__raw_writel(byte, SSI_DR(0, 0));
}

static __always_inline uint8_t dpu_ssi_flash_readb(void)
{
	while (!(__raw_readl(SSI_RISR(0)) & SSI_RXFI));
	return __raw_readl(SSI_DR(0, 0));
}

static __always_inline uint8_t dpu_ssi_flash_read(uint32_t addr)
{
	uint8_t byte;

	dpu_ssi_flash_writeb(SF_READ_DATA);
	dpu_ssi_flash_writeb((uint8_t)(addr >> 16));
	dpu_ssi_flash_writeb((uint8_t)(addr >> 8));
	dpu_ssi_flash_writeb((uint8_t)(addr >> 0));
	dpu_ssi_flash_select(_BV(0));
	byte = dpu_ssi_flash_readb();
	dpu_ssi_flash_select(0);
	return byte;
}

#ifdef CONFIG_DPU_SSI_FLASH_DUMP_PAYLOAD
#define PAYLOAD_START	0x200000
#define PAYLOAD_CHUNK	0x2000

uint8_t dpu_payload_dump[PAYLOAD_CHUNK];
caddr_t dpu_payload_addr = 0;

void __dpu_ssi_flash_dump_payload(size_t size)
{
	if (size <= 0)
		return;

	hexdump(dpu_payload_addr + 1 - PAYLOAD_CHUNK,
		dpu_payload_dump, 1, size);
}

void __dpu_ssi_flash_save_payload(int i, uint8_t byte)
{
	caddr_t offset;

	if (i < 0)
		return;

	offset = i % PAYLOAD_CHUNK;
	dpu_payload_addr = i;
	dpu_payload_dump[offset] = byte;
	if ((offset + 1) == PAYLOAD_CHUNK)
		__dpu_ssi_flash_dump_payload(PAYLOAD_CHUNK);
}
#else /* CONFIG_DPU_SSI_FLASH_DUMP_PAYLOAD */
#define PAYLOAD_START	0
#define PAYLOAD_CHUNK	0x2000

#define __dpu_ssi_flash_dump_payload(size)	do { } while (0)

void __dpu_ssi_flash_save_payload(int i, uint8_t byte)
{
	if ((i % PAYLOAD_CHUNK) == 0)
		__boot_dbg('.');
}
#endif /* CONFIG_DPU_SSI_FLASH_DUMP_PAYLOAD */

__align(__WORDSIZE)
void __dpu_ssi_flash_boot(void *boot, uint32_t addr, uint32_t size)
{
	int i;
	uint8_t *dst = boot;
	void (*boot_entry)(void) = boot;

#define is_last(index, length)		(((index) + 1) == length)

	__boot_dbg('\n');
	for (i = 0; i < size; i++, addr++) {
		dst[i] = dpu_ssi_flash_read(addr);
		__dpu_ssi_flash_save_payload(i - PAYLOAD_START, dst[i]);
	}
	__dpu_ssi_flash_dump_payload((i - PAYLOAD_START) % PAYLOAD_CHUNK);
	__boot_dbg('\n');
	__boot_dbg('B');
	__boot_dbg('o');
	__boot_dbg('o');
	__boot_dbg('t');
	__boot_dbg('\n');
	__boot_msg(smp_processor_id());
	smp_boot_secondary_cpus((caddr_t)boot_entry);
	__boot_fini();
	local_flush_icache_all();
	boot_entry();
}

void dpu_ssi_flash_boot(void *boot, uint32_t addr, uint32_t size)
{
	dpu_boot_cb boot_func;
#ifdef CONFIG_DPU_BOOT_STACK
	__align(__WORDSIZE) uint8_t boot_from_stack[1024];

	boot_func = (dpu_boot_cb)boot_from_stack;
	memcpy(boot_from_stack, __dpu_ssi_flash_boot,
	       sizeof(boot_from_stack));
#else
	boot_func = __dpu_ssi_flash_boot;
#endif
	boot_func(boot, addr, size);
	unreachable();
}

static int do_flash_dump(int argc, char *argv[])
{
	uint8_t buffer[GPT_LBA_SIZE];
	uint32_t addr = 128;
	size_t size = 32;

	if (argc > 2)
		addr = strtoul(argv[2], NULL, 0);
	if (argc > 3)
		size = strtoul(argv[3], NULL, 0);
	if (size > GPT_LBA_SIZE) {
		printf("size should be less or equal than %d\n",
		       GPT_LBA_SIZE);
		return -EINVAL;
	}
	mtd_load(board_flash, buffer, addr, size);
	hexdump(0, buffer, 1, size);
	return 0;
}

#ifdef CONFIG_DPU_SIM_SSI_IRQ
static uint32_t dw_ssi_irqs;

static void dpu_ssi_handler(irq_t irq)
{
	uint32_t irqs = dw_ssi_irqs_status(SSI_ID);

	if ((irqs & SSI_TXEI) && !(dw_ssi_irqs & SSI_TXEI)) {
		dw_ssi_disable_irqs(SSI_ID, SSI_TXEI);
		dw_ssi_irqs |= SSI_TXEI;
		dw_ssi_write_dr(SSI_ID, SF_READ_STATUS_1);
	}
	if ((irqs & SSI_RXFI) && !(dw_ssi_irqs & SSI_RXFI)) {
		dw_ssi_disable_irqs(SSI_ID, SSI_RXFI);
		dw_ssi_irqs |= SSI_RXFI;
		(void)dw_ssi_read_dr(SSI_ID);
	}
	dw_ssi_clear_irqs(SSI_ID, irqs);
	irqc_ack_irq(IRQ_SPI);
}

void dpu_ssi_irq_init(void)
{
	irqc_configure_irq(IRQ_SPI, 0, IRQ_LEVEL_TRIGGERED);
	irq_register_vector(IRQ_SPI, dpu_ssi_handler);
}

static int do_flash_irq(int argc, char *argv[])
{
	uint32_t irqs = SSI_RXFI | SSI_TXEI;
	mtd_t smtd;

	smtd = mtd_save_device(board_flash);
	mtd_open(OPEN_READ, 0, 8);
	mtd_close();
	mtd_restore_device(smtd);
	printf("IRQ test ready.\n");

	dw_ssi_irqs = 0;

	dw_ssi_select_chip(SSI_ID, 0);
	dw_ssi_enable_irqs(SSI_ID, irqs);
	irqc_enable_irq(IRQ_SPI);
	while (dw_ssi_irqs != irqs) {
		irq_local_enable();
		irq_local_disable();
	}
	irqc_disable_irq(IRQ_SPI);
	dw_ssi_disable_irqs(SSI_ID, irqs);
	dw_ssi_deselect_chips(SSI_ID);
	printf("IRQ test passed.\n");
	return 0;
}
#else
static int do_flash_irq(int argc, char *argv[])
{
	return -EINVAL;
}
#endif

static int do_flash(int argc, char *argv[])
{
	if (argc < 2)
		return -EINVAL;

	if (strcmp(argv[1], "dump") == 0)
		return do_flash_dump(argc, argv);
	if (strcmp(argv[1], "gpt") == 0) {
		gpt_mtd_dump(board_flash);
		return 0;
	}
	if (strcmp(argv[1], "irq") == 0)
		return do_flash_irq(argc, argv);
	return -ENODEV;
}

DEFINE_COMMAND(flash, do_flash, "SSI flash commands",
	"dump [addr] [size]\n"
	"    - dump content of SSI flash\n"
	"      addr: default to 128\n"
	"      size: default to 32\n"
	"gpt\n"
	"    - dump GPT partitions from SSI flash\n"
	"irq\n"
	"    - testing SSI IRQ\n"
);
