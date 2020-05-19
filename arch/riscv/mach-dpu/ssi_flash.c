#include <target/spiflash.h>
#include <target/cmdline.h>
#include <target/mem.h>
#include <target/efi.h>

#define FLASH_PAGE_SIZE		256
#define FLASH_TOTAL_SIZE	4000000

mtd_t board_flash = INVALID_MTD_ID;

void dpu_ssi_flash_init(void)
{
	board_flash = spiflash_register_bank(0);
}

void dpu_ssi_flash_copy(void *buf, uint32_t addr, uint32_t size)
{
	mtd_t smtd;
	int i;
	uint8_t *dst = buf;

	smtd = mtd_save_device(board_flash);
	mtd_open(OPEN_READ, addr, size);
	for (i = 0; i < size; i++)
		dst[i] = mtd_read_byte();
	mtd_close();
	mtd_restore_device(smtd);
}

static inline void dpu_ssi_flash_select(uint32_t chips)
{
	__raw_clearl(SSI_EN, SSI_SSIENR(0));
	__raw_writel(chips, SSI_SER(0));
	__raw_setl(SSI_EN, SSI_SSIENR(0));
}

static inline void dpu_ssi_flash_writeb(uint8_t byte)
{
	while (!(__raw_readl(SSI_RISR(0)) & SSI_TXEI));
	__raw_writel(byte, SSI_DR(0, 0));
}

static inline uint8_t dpu_ssi_flash_readb(void)
{
        while (!(__raw_readl(SSI_RISR(0)) & SSI_RXFI));
        return __raw_readl(SSI_DR(0, 0));
}

static inline uint8_t dpu_ssi_flash_read(uint32_t addr)
{
	uint8_t byte;

	dpu_ssi_flash_select(_BV(0));
	dpu_ssi_flash_writeb(SF_READ_DATA);
	dpu_ssi_flash_writeb((uint8_t)(addr >> 16));
	dpu_ssi_flash_writeb((uint8_t)(addr >> 8));
	dpu_ssi_flash_writeb((uint8_t)(addr >> 0));
	byte = dpu_ssi_flash_readb();
	dpu_ssi_flash_select(0);
	return byte;
}

void __dpu_ssi_flash_boot(void *boot, uint32_t addr, uint32_t size)
{
	int i;
	uint8_t *dst = boot;
	void (*boot_entry)(void) = boot;

	for (i = 0; i < size; i++, addr++)
		dst[i] = dpu_ssi_flash_read(addr);
	boot_entry();
}

void dpu_ssi_flash_boot(void *boot, uint32_t addr, uint32_t size)
{
	dpu_boot_cb boot_func;
#ifdef CONFIG_DPU_BOOT_STACK
	__align(32) uint8_t boot_from_stack[256];

	boot_func = (dpu_boot_cb)boot_from_stack;
	memcpy(boot_from_stack, __dpu_ssi_flash_boot, 256);
#else
	boot_func = __dpu_ssi_flash_boot;
#endif
	boot_func(boot, addr, size);
	unreachable();
}

static int do_flash_gpt(int argc, char *argv[])
{
	uint8_t gpt_buf[FLASH_PAGE_SIZE];
	gpt_header hdr;
	uint64_t partition_entries_lba_end;
	gpt_partition_entry *gpt_entries;
	uint64_t i;
	uint32_t j;
	uint32_t num_entries;

	dpu_ssi_flash_copy(&hdr,
		GPT_HEADER_LBA * GPT_LBA_SIZE, GPT_HEADER_BYTES);
	mem_print_data(0, &hdr, 1, sizeof (gpt_header));
	partition_entries_lba_end = (hdr.partition_entries_lba +
		(hdr.num_partition_entries * hdr.partition_entry_size +
		 FLASH_PAGE_SIZE - 1) / FLASH_PAGE_SIZE);
	for (i = hdr.partition_entries_lba;
	     i < partition_entries_lba_end; i++) {
		dpu_ssi_flash_copy(gpt_buf, i * FLASH_PAGE_SIZE,
				  FLASH_PAGE_SIZE);
		gpt_entries = (gpt_partition_entry *)gpt_buf;
		num_entries = FLASH_PAGE_SIZE / hdr.partition_entry_size;
		for (j = 0; j < num_entries; j++) {
			printf("%s:\n",
			       uuid_export(gpt_entries[j].partition_type_guid.u.uuid));
			printf("%016llX - %016llX \n",
			       gpt_entries[j].first_lba,
			       gpt_entries[i].last_lba);
		}
	}
	return 0;
}

static int do_flash_dump(int argc, char *argv[])
{
	uint8_t buffer[FLASH_PAGE_SIZE];
	uint32_t addr = 128;
	size_t size = 32;

	if (argc > 2)
		addr = strtoul(argv[2], NULL, 0);
	if (argc > 3)
		size = strtoul(argv[3], NULL, 0);
	if (addr >= FLASH_TOTAL_SIZE) {
		printf("addr should be less than %d\n", FLASH_TOTAL_SIZE);
		return -EINVAL;
	}
	if (addr > FLASH_PAGE_SIZE) {
		printf("size should be less or equal than %d\n",
		       FLASH_PAGE_SIZE);
		return -EINVAL;
	}
	dpu_ssi_flash_copy(buffer, addr, size);
	mem_print_data(0, buffer, 1, size);
	return 0;
}

#ifdef CONFIG_DPU_SIM_SSI_IRQ
static uint32_t triggered = false;

static void dpu_ssi_handler(void)
{
	triggered = true;
	irqc_mask_irq(IRQ_SPI);
	printf("SSI IRQ\n");
}

void dpu_ssi_irq_init(void)
{
	irqc_configure_irq(IRQ_SPI, 0, IRQ_LEVEL_TRIGGERED);
	irq_register_vector(IRQ_SPI, dpu_ssi_handler);
	irqc_enable_irq(IRQ_SPI);
}

static int do_flash_irq(int argc, char *argv[])
{
	uint8_t status;

	dw_ssi_enable_irqs(SSI_ID, SSI_RXFI | SSI_TXEI);
	dw_ssi_write_byte(SSI_ID, SF_READ_STATUS_1);
        status = dw_ssi_read_byte(SSI_ID);
	while (!triggered) {
		irq_local_enable();
		irq_local_disable();
	}
	irqc_ack_irq(IRQ_SPI);
	dw_ssi_disable_irqs(SSI_ID, SSI_RXFI | SSI_TXEI);
	irqc_unmask_irq(IRQ_SPI);
	return -EINVAL;
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
	if (strcmp(argv[1], "gpt") == 0)
		return do_flash_gpt(argc, argv);
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
