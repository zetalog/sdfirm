#include <target/irq.h>
#include <target/console.h>
#include <target/arch.h>
#include <target/ras.h>
#include <target/ghes.h>
#include <target/cper.h>
#include <target/cmdline.h>
#include <target/ecc_sram.h>

#define ECC_SRAM_MODNAME	"ecc_sram"

#define UNKNOWN_ERROR		0x0000
#define SINGLE_BIT_ERROR	0x0002
#define MULTIPLE_BIT_ERROR	0x0003

#define ECC_SRAM_ERR_TYPE_SEC		0x1
#define ECC_SRAM_ERR_TYPE_DED		0x2

#define ESRAM_HIGH_ADDR			0x400000000ULL

bh_t ecc_sram_bh;

/**
 * struct ecc_sram_error_info - SRAM error information structure
 * @error_type: Type of error (correctable/uncorrectable)
 * @error_status: Error status register value
 * @error_address: Error address
 * @error_syndrome: Error syndrome
 * @error_count: Error count
 */
struct ecc_sram_error_info {
	uint32_t error_type;
	uint32_t error_status;
	uint64_t error_address;
	uint32_t error_syndrome;
	uint32_t error_count;
};

/**
 * ecc_sram_collect_error_info - Collect SRAM error information
 * @info: Pointer to error info structure
 *
 * This function collects error information from SRAM registers
 * and fills the error info structure.
 */
static void ecc_sram_collect_error_info(struct ecc_sram_error_info *info)
{
	uint32_t status;
	uint64_t sec_addr, ded_addr;

	status = __raw_readl(ESRAM_INT_STATUS);

	info->error_status = status;

	if (status & ESRAM_INT_SEC) {
		info->error_type = SINGLE_BIT_ERROR;
		sec_addr = __raw_readl(ESRAM_SEC_ADDR);
		info->error_address = ESRAM_HIGH_ADDR | sec_addr;
		info->error_count = __raw_readl(ESRAM_SEC_CNT) & ESRAM_CNT_MASK;
		con_dbg(ECC_SRAM_MODNAME ": SEC error at 0x%016llx (raw: 0x%llx)\n",
		       info->error_address, sec_addr);
	} else if (status & ESRAM_INT_DED) {
		info->error_type = MULTIPLE_BIT_ERROR;
		ded_addr = __raw_readl(ESRAM_DED_ADDR);
		info->error_address = ESRAM_HIGH_ADDR | ded_addr;
		info->error_count = __raw_readl(ESRAM_DED_CNT) & ESRAM_CNT_MASK;
		con_dbg(ECC_SRAM_MODNAME ": DED error at 0x%016llx (raw: 0x%llx)\n",
		       info->error_address, ded_addr);
	} else {
		info->error_type = UNKNOWN_ERROR;
		info->error_address = 0;
		info->error_count = 0;
	}

	info->error_syndrome = 0;
}

/**
 * ecc_sram_report_error - Report SRAM error to GHES
 *
 * This function collects SRAM error information and reports it
 * to GHES using CPER format.
 */
void ecc_sram_report_error(void)
{
	struct ecc_sram_error_info info;
	acpi_ghes_error_info einfo;
	uint32_t acpi_ghes_source_id;

	/* 1. Collect error information */
	ecc_sram_collect_error_info(&info);

	/* 2. Initialize error info structure */
	memset(&einfo, 0, sizeof(einfo));
	einfo.etype = ERROR_TYPE_MEM;

	/* 3. Set error severity */
	if (info.error_type == SINGLE_BIT_ERROR)
		einfo.info.me.err_sev = GHES_SEV_CORRECTED;
	else
		einfo.info.me.err_sev = GHES_SEV_PANIC;

	/* 4. Set validation bits */
	einfo.info.me.validation_bits = CPER_MEM_VALID_PA
					| CPER_MEM_VALID_ERROR_TYPE;

	/* 5. Set physical address */
	einfo.info.me.physical_address = info.error_address;
	einfo.info.me.physical_address_mask = ~0ULL;
	einfo.info.me.node = 0;
	einfo.info.me.card = 0;
	einfo.info.me.module = 0;
	einfo.info.me.bank = 0;
	einfo.info.me.device = 0;
	einfo.info.me.row = 0;
	einfo.info.me.column = 0;
	einfo.info.me.rank = 0;
	einfo.info.me.bit_pos = 0;
	einfo.info.me.err_type = info.error_type;
	einfo.info.me.err_status = 0;

	/* 6. Set source ID (SRAM) */
	acpi_ghes_source_id = 0x70;  /* SRAM error source ID */

	/* 7. Report error */
	acpi_ghes_record_errors(acpi_ghes_source_id, &einfo);

	/* Log error information */
	if (info.error_type == SINGLE_BIT_ERROR) {
		con_log(ECC_SRAM_MODNAME ": SEC error detected at address 0x%llx, count: %d\n",
		       info.error_address, info.error_count);
	} else if (info.error_type == MULTIPLE_BIT_ERROR) {
		con_log(ECC_SRAM_MODNAME ": DED error detected at address 0x%llx, count: %d\n",
		       info.error_address, info.error_count);
	}
}

/**
 * ecc_sram_handle_irq - Handle SRAM ECC interrupt
 */
void ecc_sram_handle_irq(irq_t irq)
{
	uint32_t status;

	/* Read interrupt status */
	status = __raw_readl(ESRAM_INT_STATUS);

	/* Handle SEC interrupt */
	if (status & ESRAM_INT_SEC) {
		con_log("ecc_sram_handle_irq: irq=%d\n", irq_ext(irq));
		con_dbg(ECC_SRAM_MODNAME ": SEC error detected at address 0x%016llx\n",
		       ESRAM_HIGH_ADDR | __raw_readl(ESRAM_SEC_ADDR));
		con_dbg(ECC_SRAM_MODNAME ": SEC count: %d\n",
		       __raw_readl(ESRAM_SEC_CNT) & 0xFFFF);

		ecc_sram_report_error();

		__raw_writel(ESRAM_INT_SEC, ESRAM_INT_CLR);
	}

	/* Handle DED interrupt */
	if (status & ESRAM_INT_DED) {
		con_log("ecc_sram_handle_irq: irq=%d\n", irq_ext(irq));
		con_dbg(ECC_SRAM_MODNAME ": DED error detected at address 0x%016llx\n",
		       ESRAM_HIGH_ADDR | __raw_readl(ESRAM_DED_ADDR));
		con_dbg(ECC_SRAM_MODNAME ": DED count: %d\n",
		       __raw_readl(ESRAM_DED_CNT) & 0xFFFF);

		ecc_sram_report_error();

		__raw_writel(ESRAM_INT_DED, ESRAM_INT_CLR);
	}
}

/**
 * ecc_sram_bh_handler - Bottom half handler for SRAM ECC
 * @events: Event flags
 */
static void ecc_sram_bh_handler(uint8_t events)
{
	if (events == BH_POLLIRQ) {
		ecc_sram_handle_irq(IRQ_R_SRAM_RAS);
		return;
	}
}

#ifdef SYS_REALTIME
static void ecc_sram_poll_init(void)
{
	irq_register_poller(ecc_sram_bh);
}
#define ecc_sram_irq_init()	do {} while (0)
#else
static void ecc_sram_irq_init(void)
{
	irqc_configure_irq(IRQ_R_SRAM_RAS, 0, IRQ_LEVEL_TRIGGERED);
	irq_register_vector(IRQ_R_SRAM_RAS, ecc_sram_handle_irq);
	irqc_enable_irq(IRQ_R_SRAM_RAS);
}
#define ecc_sram_poll_init()	do {} while (0)
#endif

/**
 * ecc_sram_init - Initialize SRAM ECC
 */
void ecc_sram_init(void)
{
	/* Initialize SRAM */
	con_log(ECC_SRAM_MODNAME ": initialize ECC SRAM\n");
	con_log(ECC_SRAM_MODNAME ": RMU_BOOT_DONE: 0x%08x\n", __raw_readl(RMU_BOOT_DONE));

	/* Enable interrupts for SEC and DED */
	__raw_writel(ESRAM_INT_SEC | ESRAM_INT_DED, ESRAM_INT_EN);

	/* Set SEC threshold count (default to 1) */
	__raw_writel(ESRAM_SEC_THRES_EN | 0x1, ESRAM_SEC_THRES_CNT);

	/* Initialize bottom half handler */
	ecc_sram_bh = bh_register_handler(ecc_sram_bh_handler);

	/* Initialize interrupt or polling */
	ecc_sram_irq_init();
	ecc_sram_poll_init();

	con_log(ECC_SRAM_MODNAME ": ECC SRAM initialized successfully\n");
	con_log(ECC_SRAM_MODNAME ": SEC threshold count set to 1\n");
	con_log(ECC_SRAM_MODNAME ": SEC and DED interrupts enabled\n");
}

/**
 * ecc_sram_inject_error - Inject ECC error into SRAM
 * It does not work on K1Matrix platform, because ECC_SRAM does not
 * provide software injection error function.
 * Software bit flip only modifies the data content of SRAM, but does not trigger
 * the physical storage unit error detected by the ECC mechanism, and the ECC
 * circuit will consider this to be normal data writing, not reporting errors.
 * @test_addr: Test address
 * @test_type: Test type (0: SEC, 1: DED)
 */
static void ecc_sram_inject_error(uint64_t test_addr, uint32_t test_type)
{
	uint32_t orig_data;
	volatile uint32_t *addr = (volatile uint32_t *)(uintptr_t)test_addr;

	/* Save original data */
	orig_data = *addr;
	con_dbg(ECC_SRAM_MODNAME ": Original data at 0x%016llx: 0x%08x\n",
	       test_addr, orig_data);

	/* Write test data with error */
	if (test_type == 0) {
		/* SEC error injection */
		*addr ^= 0x00000001; // revert the lowest bit
		con_dbg(ECC_SRAM_MODNAME ": Injected SEC error at 0x%016llx: 0x%08x\n",
		       test_addr, *addr);
	} else {
		/* DED error injection */
		*addr ^= 0x00000002; // revert the lowest 2 bits
		con_dbg(ECC_SRAM_MODNAME ": Injected DED error at 0x%016llx: 0x%08x\n",
		       test_addr, *addr);
	}

	/* Read back to trigger ECC error */
	(void)*addr;
	con_dbg(ECC_SRAM_MODNAME ": Triggered ECC error check\n");

	/* Restore original data */
	*addr = orig_data;
	con_dbg(ECC_SRAM_MODNAME ": Restored original data at 0x%016llx: 0x%08x\n",
	       test_addr, orig_data);
}

/**
 * do_ecc_sram_test - ECC SRAM test command handler
 * @argc: Argument count
 * @argv: Argument vector
 *
 * This function handles the ECC SRAM test command with the following options:
 * -t: Test type (0: SEC, 1: DED)
 * -a: Test address (optional, default: 0x1000)
 * -c: SEC threshold count (optional, default: 1)
 */
static int do_ecc_sram_test(int argc, char **argv)
{
	uint32_t test_type = 0;
	uint64_t test_addr = SRAM0_BASE;
	uint32_t sec_thres = 1;
	int i;

	if (argc < 2)
		return -EINVAL;

	for (i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-t") == 0) {
			if (i + 1 >= argc)
				return -EINVAL;
			test_type = (uint32_t)strtoull(argv[i + 1], NULL, 16);
			i++;
		} else if (strcmp(argv[i], "-a") == 0) {
			if (i + 1 >= argc)
				return -EINVAL;
			test_addr = (uint64_t)strtoull(argv[i + 1], NULL, 16);
			i++;
		} else if (strcmp(argv[i], "-c") == 0) {
			if (i + 1 >= argc)
				return -EINVAL;
			sec_thres = (uint32_t)strtoull(argv[i + 1], NULL, 16);
			i++;
		} else {
			con_err(ECC_SRAM_MODNAME ": Invalid option: %s\n", argv[i]);
			return -EINVAL;
		}
	}

	/* Set SEC threshold count */
	__raw_writel(ESRAM_SEC_THRES_EN | sec_thres, ESRAM_SEC_THRES_CNT);

	/* Inject error based on test type */
	if (test_type == 0) {
		/* SEC error injection */
		con_dbg(ECC_SRAM_MODNAME ": Injecting SEC error at address 0x%016llx\n",
		       test_addr);
		ecc_sram_inject_error(test_addr, 0);
	} else if (test_type == 1) {
		/* DED error injection */
		con_dbg(ECC_SRAM_MODNAME ": Injecting DED error at address 0x%016llx\n",
		       test_addr);
		ecc_sram_inject_error(test_addr, 1);
	} else {
		con_err(ECC_SRAM_MODNAME ": Invalid test type: %d\n", test_type);
		return -EINVAL;
	}

	return 0;
}

DEFINE_COMMAND(eccsram, do_ecc_sram_test, "ECC SRAM inject error commands",
	"eccsram [-t test_type] [-a test_addr] [-c sec_thres]\n"
	"  -t: Test type (0: SEC, 1: DED)\n"
	"  -a: Test address (default: 0x410100000)\n"
	"  -c: SEC threshold count (default: 1)\n"
);
