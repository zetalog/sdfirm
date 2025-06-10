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

	xsdt->table_offset_entry[acpi_xsdt_num] = (uint64_t)table;
	acpi_xsdt_num++;
}

static void acpi_bios_producer_init(void)
{
	int i;
	struct acpi_table_rsdp *rsdp = (struct acpi_table_rsdp *)acpi_rsdp;
	struct acpi_table_fadt *fadt = (struct acpi_table_fadt *)acpi_fadt;
	int nr_tables = ((uintptr_t)__acpi_bios_end -
			 (uintptr_t)__acpi_bios_start) /
			sizeof (struct acpi_bios_table);
	struct acpi_table_header *table;
	struct acpi_bios_table *start = __acpi_bios_start;
	struct acpi_bios_table *end = __acpi_bios_end;

	con_log("acpi_bios: %016llx - %016llx\n", (uint64_t)start, (uint64_t)end);
	rsdp->revision = 2;
	rsdp->xsdt_physical_address = (uint64_t)acpi_xsdt;

	acpi_bios_install_table(acpi_fadt);

	for (i = 0; i < nr_tables; i++) {
		table = (struct acpi_table_header *)start[i].table;
		con_log("acpi_bios: installing [%4.4s] at %016llx\n",
			table->signature, (uint64_t)table);
		acpi_bios_install_table(table);
	}

	fadt->Xdsdt = (uint64_t)acpi_dsdt;
}

static void acpi_bios_consumer_init(void)
{
	int i;

	acpi_initialize_subsystem();
	for (i = 0; i < ACPI_MAX_TABLES; i++)
		acpi_gbl_initial_tables_ids[i] = &acpi_gbl_initial_tables[i];
	acpi_initialize_namespace();
	acpi_initialize_tables(acpi_gbl_initial_tables_ids, ACPI_MAX_TABLES, false);
}

void acpi_bios_init(void)
{
	acpi_bios_producer_init();
	acpi_bios_consumer_init();
}
