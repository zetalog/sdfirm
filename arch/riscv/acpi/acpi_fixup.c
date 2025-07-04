#include <target/acpi.h>
#include <target/sbi.h>
#include <target/arch.h>
#include <target/noc.h>

static char *acpi_vid;
static char *acpi_pid;

#ifdef CONFIG_ARCH_SPIKE
static void acpi_fixup_dsdt_cpu_sta(struct acpi_table_header *dsdt, uint64_t disable_mask);
static void acpi_fixup_madt_rintc_flags(struct acpi_table_header *madt, uint64_t disable_mask);
#endif

static uint64_t acpi_total_size = 0;
static uint64_t acpi_start_addr = 0;

uint64_t acpi_get_total_size(void)
{
	return acpi_total_size;
}

uint64_t acpi_get_start_addr(void)
{
	return acpi_start_addr;
}

#ifndef CONFIG_ACPI_RHCT
extern void acpi_fixup_rhct(void);
#else
#define acpi_fixup_rhct()		do { } while (0)
#endif

char *acpi_get_vid(void)
{
	return acpi_vid;
}

char *acpi_get_pid(void)
{
	return acpi_pid;
}

#define FIXUP_TABLE_WITH_HEADER(table) \
	do { \
		if (table) { \
			ACPI_OEMCPY(acpi_get_vid(), table->header.oem_id, ACPI_OEM_ID_SIZE); \
			ACPI_OEMCPY(acpi_get_pid(), table->header.oem_table_id, ACPI_OEM_TABLE_ID_SIZE); \
			acpi_table_calc_checksum(&table->header); \
			printf("Fixing up table %4.4s at %p\n", table->header.signature, table);\
		} \
	} while (0)

#define FIXUP_TABLE_WITHOUT_HEADER(table) \
	do { \
		if (table) { \
			ACPI_OEMCPY(acpi_get_vid(), table->oem_id, ACPI_OEM_ID_SIZE); \
			ACPI_OEMCPY(acpi_get_pid(), table->oem_table_id, ACPI_OEM_TABLE_ID_SIZE); \
			acpi_table_calc_checksum(table); \
			printf("Fixing up table %4.4s at %p\n", table->signature, table); \
		} \
	} while (0)

#define FIXUP_RSDP_TABLE(table) \
	do { \
		if (table) { \
			ACPI_OEMCPY(acpi_get_vid(), table->oem_id, ACPI_OEM_ID_SIZE); \
			acpi_rsdp_calc_checksum(table); \
			printf("Fixing up RSDP table at %p\n", table); \
		} \
	} while (0)

void acpi_fixups(char *oem, char *oem_table)
{
	struct acpi_table_rsdp *rsdp;
	struct acpi_table_xsdt *xsdt;
	struct acpi_table_fadt *fadt;
	struct acpi_table_header *dsdt;
	struct acpi_table madt, spcr, rhct;
	int i;
	acpi_status_t status;

	acpi_addr_t dsdt_addr = 0;
	acpi_addr_t rsdp_addr = 0;
	acpi_addr_t xsdt_addr = 0;
	acpi_addr_t fadt_addr = 0;
	acpi_vid = oem;
	acpi_pid = oem_table;

	/* Initialize ACPI size tracking */
	acpi_total_size = 0;
	acpi_start_addr = 0;

#ifdef CONFIG_ARCH_SPIKE
	uint64_t cpu_mask = CPU_MASK;
	uint64_t disable_mask = ~cpu_mask;
#endif

	/* Get RSDP address and map it */
	rsdp_addr = acpi_os_get_root_pointer();
	if (!rsdp_addr) {
		printf("Failed to get RSDP address\n");
		return;
	}
	printf("RSDP address: 0x%llx\n", (unsigned long long)rsdp_addr);

	if (acpi_start_addr == 0) {
		acpi_start_addr = rsdp_addr;
	}

	rsdp = acpi_os_map_memory(rsdp_addr, sizeof(struct acpi_table_rsdp));
	if (!rsdp) {
		printf("Failed to map RSDP table\n");
		return;
	}

	acpi_total_size += sizeof(struct acpi_table_rsdp);

	/* Fixup RSDP table */
	FIXUP_RSDP_TABLE(rsdp);

	/* Get XSDT address from RSDP */
	if (rsdp->revision >= 2 && rsdp->xsdt_physical_address) {
		xsdt_addr = rsdp->xsdt_physical_address;
		printf("XSDT address: 0x%llx\n", (unsigned long long)xsdt_addr);

		xsdt = acpi_os_map_memory(xsdt_addr, sizeof(struct acpi_table_xsdt));
		if (xsdt) {
			FIXUP_TABLE_WITH_HEADER(xsdt);
			acpi_total_size += xsdt->header.length;

			/* Find FADT in XSDT */
			int table_count = (xsdt->header.length - sizeof(struct acpi_table_xsdt)) / sizeof(uint64_t);
			for (i = 0; i < table_count; i++) {
				acpi_addr_t table_addr = xsdt->table_offset_entry[i];
				struct acpi_table_header *table = acpi_os_map_memory(table_addr, sizeof(struct acpi_table_header));

				if (table) {
					acpi_total_size += table->length;

					if (ACPI_NAMECMP(ACPI_SIG_FADT, table->signature)) {
						fadt_addr = table_addr;
						printf("Found FADT at address: 0x%llx, size: %u\n",
							(unsigned long long)fadt_addr, table->length);
						acpi_os_unmap_memory(table, sizeof(struct acpi_table_header));
						break;
					}

					acpi_os_unmap_memory(table, sizeof(struct acpi_table_header));
				}
			}
		}
	} else {
		printf("XSDT not available, using RSDT\n");
		/* Fallback to RSDT if XSDT not available */
		acpi_addr_t rsdt_addr = rsdp->rsdt_physical_address;
		if (rsdt_addr) {
			struct acpi_table_rsdt *rsdt = acpi_os_map_memory(rsdt_addr, sizeof(struct acpi_table_rsdt));
			if (rsdt) {
				FIXUP_TABLE_WITH_HEADER(rsdt);
				acpi_total_size += rsdt->header.length;

				/* Find FADT in RSDT */
				int table_count = (rsdt->header.length - sizeof(struct acpi_table_rsdt)) / sizeof(uint32_t);
				for (i = 0; i < table_count; i++) {
					acpi_addr_t table_addr = rsdt->table_offset_entry[i];
					struct acpi_table_header *table = acpi_os_map_memory(table_addr, sizeof(struct acpi_table_header));

					if (table) {
						acpi_total_size += table->length;

						if (ACPI_NAMECMP(ACPI_SIG_FADT, table->signature)) {
							fadt_addr = table_addr;
							printf("Found FADT at address: 0x%llx, size: %u\n",
								(unsigned long long)fadt_addr, table->length);
							acpi_os_unmap_memory(table, sizeof(struct acpi_table_header));
							break;
						}

						acpi_os_unmap_memory(table, sizeof(struct acpi_table_header));
					}
				}
				acpi_os_unmap_memory(rsdt, sizeof(struct acpi_table_rsdt));
			}
		}
	}

	/* Map and fixup FADT */
	if (fadt_addr) {
		fadt = acpi_os_map_memory(fadt_addr, sizeof(struct acpi_table_fadt));
		if (fadt) {
			FIXUP_TABLE_WITH_HEADER(fadt);

			/* Get DSDT address from FADT */
			if (fadt->Xdsdt) {
				dsdt_addr = fadt->Xdsdt;
				printf("Using 64-bit DSDT address: 0x%llx\n", (unsigned long long)dsdt_addr);
			} else if (fadt->dsdt) {
				dsdt_addr = fadt->dsdt;
				printf("Using 32-bit DSDT address: 0x%x\n", (unsigned int)dsdt_addr);
			}

			if (dsdt_addr) {
				dsdt = acpi_os_map_memory(dsdt_addr, sizeof(struct acpi_table_header));
				if (dsdt && ACPI_NAMECMP(ACPI_SIG_DSDT, dsdt->signature)) {
					FIXUP_TABLE_WITHOUT_HEADER(dsdt);
					acpi_total_size += dsdt->length;
					printf("DSDT size: %u\n", dsdt->length);

#ifdef CONFIG_ARCH_SPIKE
					acpi_fixup_dsdt_cpu_sta(dsdt, disable_mask);
#endif

					acpi_os_unmap_memory(dsdt, sizeof(struct acpi_table_header));
				} else {
					printf("DSDT not found or invalid signature\n");
					if (dsdt)
						acpi_os_unmap_memory(dsdt, sizeof(struct acpi_table_header));
				}
			}
		}
	}

	/* Cleanup */
	if (rsdp)
		acpi_os_unmap_memory(rsdp, sizeof(struct acpi_table_rsdp));
	if (xsdt)
		acpi_os_unmap_memory(xsdt, sizeof(struct acpi_table_xsdt));
	if (fadt)
		acpi_os_unmap_memory(fadt, sizeof(struct acpi_table_fadt));

	status = acpi_get_table_by_inst(ACPI_NAME2TAG(ACPI_SIG_MADT), 0, &madt);
	if (ACPI_SUCCESS(status)) {
		FIXUP_TABLE_WITHOUT_HEADER(madt.pointer);
		acpi_total_size += madt.pointer->length;
		printf("MADT size: %u\n", madt.pointer->length);

#ifdef CONFIG_ARCH_SPIKE
		acpi_fixup_madt_rintc_flags(madt.pointer, disable_mask);
#endif
	}

	status = acpi_get_table_by_inst(ACPI_NAME2TAG(ACPI_SIG_SPCR), 0, &spcr);
	if(ACPI_SUCCESS(status)) {
		FIXUP_TABLE_WITHOUT_HEADER(spcr.pointer);
		acpi_total_size += spcr.pointer->length;
		printf("SPCR size: %u\n", spcr.pointer->length);
	}

	status = acpi_get_table_by_inst(ACPI_NAME2TAG(ACPI_SIG_RHCT), 0, &rhct);
	if(ACPI_SUCCESS(status)) {
		FIXUP_TABLE_WITHOUT_HEADER(rhct.pointer);
		acpi_total_size += rhct.pointer->length;
		printf("RHCT size: %u\n", rhct.pointer->length);
	}

	acpi_fixup_rhct();

	printf("ACPI fixups completed. Total ACPI table size: 0x%llx bytes\n", acpi_total_size);
}

#ifdef CONFIG_ARCH_SPIKE
#define HART_PER_CLUSTER 4

static void acpi_fixup_dsdt_cpu_sta(struct acpi_table_header *dsdt, uint64_t disable_mask)
{
	acpi_status_t status;
	int cpu_id;
	char cpu_path[32];

	printf("Fixing up DSDT CPU _STA values based on disable_mask: 0x%llx\n", disable_mask);

	for (cpu_id = 0; cpu_id < MAX_CPU_CORES; cpu_id++) {
		if (disable_mask & (1ULL << cpu_id)) {
			snprintf(cpu_path, sizeof(cpu_path), "\\_SB.CL%02d.CP%02d._STA", cpu_id/HART_PER_CLUSTER, cpu_id);

			status = acpi_sdt_patch_STA(dsdt, cpu_path, 0x0);
			if (ACPI_SUCCESS(status)) {
				printf("Disabled CPU %d _STA (set to 0x0)\n", cpu_id);
			} else {
				printf("Failed to modify CPU %d _STA (status: %d)\n", cpu_id, status);
			}
		}
	}
}

static void acpi_fixup_madt_rintc_flags(struct acpi_table_header *madt, uint64_t disable_mask)
{
	uint8_t *table_start = (uint8_t *)madt;
	uint8_t *table_end = table_start + madt->length;
	uint8_t *ptr = table_start + sizeof(struct acpi_table_madt);

	printf("MADT parsing: disable_mask: 0x%llx\n", disable_mask);

	while (ptr + sizeof(struct acpi_subtable_header) <= table_end) {
		struct acpi_subtable_header *subtable = (struct acpi_subtable_header *)ptr;

		if (subtable->length == 0) {
			printf("Invalid subtable length: 0, stopping to prevent infinite loop\n");
			break;
		}

		if (ptr + subtable->length > table_end) {
			printf("Invalid subtable length: %u (would exceed table end)\n", subtable->length);
			break;
		}

		if (subtable->type == ACPI_MADT_TYPE_RINTC &&
			subtable->length >= sizeof(struct acpi_madt_rintc)) {

			struct acpi_madt_rintc *rintc = (struct acpi_madt_rintc *)ptr;

			if (disable_mask & (1ULL << rintc->uid))
				rintc->flags &= ~ACPI_MADT_ENABLED;
		}

		ptr += subtable->length;
	}

	acpi_table_calc_checksum(madt);
}

#endif
