#include <target/acpi.h>

extern uint64_t acpi_rsdp[];
extern uint64_t acpi_xsdt[];
extern uint64_t acpi_fadt[];
extern uint64_t acpi_dsdt[];

#define ACPI_MAX_TABLES		CONFIG_ACPI_MAX_TABLES

static struct acpi_table_desc acpi_gbl_initial_tables[ACPI_MAX_TABLES];
static struct acpi_table_desc *acpi_gbl_initial_tables_ids[ACPI_MAX_TABLES];

static int acpi_xsdt_num = 0;

acpi_addr_t acpi_os_get_root_pointer(void)
{
	return (acpi_addr_t)acpi_rsdp;
}

void acpi_bios_install_table(void *table)
{
	struct acpi_table_xsdt *xsdt = (struct acpi_table_xsdt *)acpi_xsdt;

	BUG_ON(acpi_xsdt_num >= ACPI_MAX_TABLES);

	con_log("acpi_bios: installing [%4.4s] at %016llx\n",
		ACPI_CAST_PTR(struct acpi_table_header, table)->signature,
		((unsigned long long)table));
	xsdt->table_offset_entry[acpi_xsdt_num] = ACPI_PTR_TO_PHYSADDR(table);
	acpi_xsdt_num++;
}

static void acpi_bios_producer_init(void)
{
	int i;
	struct acpi_table_rsdp *rsdp = (struct acpi_table_rsdp *)acpi_rsdp;
	struct acpi_table_fadt *fadt = (struct acpi_table_fadt *)acpi_fadt;
	struct acpi_table_xsdt *xsdt = (struct acpi_table_xsdt *)acpi_xsdt;
	int nr_tables = ((uintptr_t)__acpi_bios_end -
			 (uintptr_t)__acpi_bios_start) /
			sizeof (struct acpi_bios_table);
	struct acpi_table_header *table;
	struct acpi_bios_table *start = __acpi_bios_start;

	con_log("acpi_bios: %016llx - %016llx\n",
		(uint64_t)start, (uint64_t)__acpi_bios_end);

	/* Build RSDP */
	ACPI_RSDP_SIG_CPY(rsdp->signature);
	ACPI_ENCODE8(&rsdp->revision, 2);
	ACPI_ENCODE64(&rsdp->xsdt_physical_address,
		      ACPI_PTR_TO_PHYSADDR(acpi_xsdt));
	ACPI_ENCODE32(&rsdp->length, sizeof (struct acpi_table_xsdt));
	acpi_rsdp_calc_checksum(rsdp);

	/* Build XSDT */
	ACPI_NAMECPY(ACPI_SIG_XSDT, xsdt->header.signature);
	ACPI_ENCODE8(&xsdt->header.revision, 1);
	acpi_bios_install_table(acpi_fadt);
	for (i = 0; i < nr_tables; i++) {
		table = (struct acpi_table_header *)start[i].table;

		/* Skip specific tables that should not be installed */
		if (ACPI_NAMECMP(ACPI_SIG_RSDP, table->signature) ||
		    ACPI_NAMECMP(ACPI_SIG_XSDT, table->signature) ||
		    ACPI_NAMECMP(ACPI_SIG_RSDT, table->signature) ||
		    ACPI_NAMECMP(ACPI_SIG_FADT, table->signature) ||
		    ACPI_NAMECMP(ACPI_SIG_DSDT, table->signature) ||
		    ACPI_NAMECMP(ACPI_SIG_FACS, table->signature)) {
			con_log("acpi_bios: skipping [%4.4s] at %016llx\n",
				table->signature, ((unsigned long long)table));
			continue;
		}

		acpi_bios_install_table(table);
	}
	ACPI_ENCODE32(&xsdt->header.length,
		sizeof (struct acpi_table_header) +
		(acpi_xsdt_num * ACPI_XSDT_ENTRY_SIZE));
	acpi_table_calc_checksum(&xsdt->header);

	memset(fadt, 0, sizeof (struct acpi_table_fadt));
	ACPI_NAMECPY(ACPI_SIG_FADT, fadt->header.signature);
	ACPI_ENCODE32(&fadt->dsdt, 0);
	ACPI_ENCODE64(&fadt->Xdsdt, ACPI_PTR_TO_PHYSADDR(acpi_dsdt));
	ACPI_ENCODE32(&fadt->header.length, ACPI_FADT_V6_SIZE);
	ACPI_ENCODE8(&fadt->header.revision, 6);
	ACPI_ENCODE8(&fadt->minor_revision, 6);
	ACPI_ENCODE32(&fadt->flags, ACPI_FADT_HW_REDUCED);
	acpi_table_calc_checksum(&fadt->header);
}

static void acpi_bios_consumer_init(void)
{
	int i;

	acpi_initialize_subsystem();
	for (i = 0; i < ACPI_MAX_TABLES; i++)
		acpi_gbl_initial_tables_ids[i] = &acpi_gbl_initial_tables[i];
	acpi_initialize_namespace();
	acpi_initialize_tables(acpi_gbl_initial_tables_ids, ACPI_MAX_TABLES, false);
	acpi_load_tables();
}

void acpi_bios_init(void)
{
	acpi_bios_producer_init();
	acpi_bios_consumer_init();
}
