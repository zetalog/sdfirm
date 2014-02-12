#include "winacpi.h"
#include "dirent.h"
#include <direct.h>

#define ACPI_EMU_XSDT_IDX_FADT			0
#define ACPI_EMU_XSDT_NR_ENTRIES		(ACPI_EMU_XSDT_IDX_FADT+1)
#define ACPI_EMU_XSDT_LENGTH			\
	(sizeof (struct acpi_table_xsdt) +	\
	((ACPI_EMU_XSDT_NR_ENTRIES -1) * ACPI_XSDT_ENTRY_SIZE))
#define ACPI_EMU_DEBUG_TIMEOUT			30000 /* 30 seconds */

struct acpi_semaphore_info {
	uint16_t max_units;
	uint16_t current_units;
	void *handle;
};
#define ACPI_OS_MAX_SEMAPHORES			256

static struct acpi_table_rsdp acpi_emu_local_rsdp;
static struct acpi_table_fadt acpi_emu_local_fadt;
static struct acpi_table_fadt *acpi_emu_external_fadt;
static struct acpi_table_xsdt *acpi_emu_local_xsdt;
static struct acpi_table_facs acpi_emu_local_facs;
static struct acpi_table_desc *acpi_emu_table_list = NULL;
static uint32_t acpi_emu_table_count = 0;
static struct acpi_table_header *acpi_emu_dsdt_override;
static boolean acpi_emu_reduced_hardware = false;
static struct acpi_semaphore_info acpi_emu_semaphores[ACPI_OS_MAX_SEMAPHORES];
static boolean acpi_emu_debug_timeout = false;
static boolean acpi_emu_dsdt_reloaded = false;
static boolean acpi_emu_facs_reloaded = false;
static boolean acpi_emu_fadt_reloaded = false;

static unsigned char acpi_emu_default_dsdt[] = {
	0x44,0x53,0x44,0x54,0x24,0x00,0x00,0x00,  /* 00000000    "DSDT$..." */
	0x02,0x6F,0x49,0x6E,0x74,0x65,0x6C,0x00,  /* 00000008    ".oIntel." */
	0x4E,0x75,0x6C,0x6C,0x44,0x53,0x44,0x54,  /* 00000010    "NullDSDT" */
	0x01,0x00,0x00,0x00,0x49,0x4E,0x54,0x4C,  /* 00000018    "....INTL" */
	0x04,0x12,0x08,0x20,
};

static acpi_status_t acpi_table_read_file(const char *path, loff_t offset,
					  char *signature,
					  struct acpi_table_header **table)
{
	FILE *fp;
	struct acpi_table_header header;
	struct acpi_table_header *local_table = NULL;
	uint32_t table_length, file_length;
	int count;
	uint32_t total = 0;
	acpi_status_t status = 0;

	fp = fopen(path, "rb");
	if (!fp)
		return AE_NOT_FOUND;
	fseek(fp, 0, SEEK_END);
	file_length = ftell(fp);
	fseek(fp, offset, SEEK_SET);

	count = fread(&header, 1, sizeof (struct acpi_table_header), fp);
	if (count != sizeof (struct acpi_table_header)) {
		status = AE_BAD_HEADER;
		goto err_exit;
	}

	if (!ACPI_NAMECMP(ACPI_TAG_NULL, signature)) {
		if (ACPI_RSDP_SIG_CMP(signature)) {
			if (!ACPI_RSDP_SIG_CMP(ACPI_CAST_RSDP(&header)->signature)) {
				status = AE_BAD_SIGNATURE;
				goto err_exit;
			}
		} else {
			if (!ACPI_NAMECMP((acpi_tag_t)signature, header.signature)) {
				status = AE_BAD_SIGNATURE;
				goto err_exit;
			}
		}
	}

	table_length = acpi_table_get_length(&header);
	if (table_length == 0 || table_length > (file_length - offset)) {
		status = AE_BAD_HEADER;
		goto err_exit;
	}

	local_table = acpi_os_allocate_zeroed(table_length);
	if (!local_table) {
		status = AE_NO_MEMORY;
		goto err_exit;
	}
	fseek (fp, offset, SEEK_SET);

	while (!feof(fp) && total < table_length) {
		count = fread(local_table, 1, table_length-total, fp);
		if (count < 0) {
			status = AE_INVALID_TABLE_LENGTH;
			goto err_exit;
		}
		total += count;
	}

	(void)acpi_table_checksum_valid(local_table);

err_exit:
	fclose(fp);
	*table = local_table;
	return status;
}

acpi_status_t acpi_emu_load_table(const char *file, acpi_ddb_t *ddb)
{
	struct acpi_table_header *table;
	acpi_status_t status;
	acpi_ddb_t local_ddb;
	boolean versioning = true;

	status = acpi_table_read_file(file, 0, ACPI_NULL_NAME, &table);
	if (ACPI_FAILURE(status))
		return status;

	if (ACPI_NAMECMP(ACPI_SIG_DSDT, table->signature)) {
		versioning = acpi_emu_dsdt_reloaded ? true : false;
		acpi_emu_dsdt_reloaded = true;
	}
	if (ACPI_NAMECMP(ACPI_SIG_FACS, table->signature)) {
		versioning = acpi_emu_facs_reloaded ? true : false;
		acpi_emu_facs_reloaded = true;
	}
	if (ACPI_NAMECMP(ACPI_SIG_FADT, table->signature)){
		versioning = acpi_emu_fadt_reloaded ? true : false;
		acpi_emu_fadt_reloaded = true;
	}

	status = acpi_install_table(table, ACPI_TABLE_INTERNAL_VIRTUAL,
				    versioning, &local_ddb);
	if (ACPI_SUCCESS(status)) {
		acpi_table_decrement(local_ddb);
		if (ddb)
			*ddb = local_ddb;
	}

	return status;
}

acpi_status_t acpi_emu_read_table(const char *file)
{
	int ret;
	struct acpi_table_header *table;
	struct acpi_table_desc *table_desc;

	ret = acpi_table_read_file(file, 0, ACPI_NULL_NAME, &table);
	if (ret)
		return AE_NOT_FOUND;

	/* Ignore FADT since we'll build it */
	if (!ACPI_NAMECMP(ACPI_SIG_FADT, table->signature)) {
		table_desc = acpi_os_allocate_zeroed(sizeof (struct acpi_table_desc));
		table_desc->pointer = table;
		table_desc->next = acpi_emu_table_list;
		acpi_emu_table_list = table_desc;
		acpi_emu_table_count++;
	} else
		acpi_os_free(table);

	return AE_OK;
}

acpi_status_t acpi_emu_build_tables(struct acpi_table_desc *table_list)
{
	acpi_addr_t dsdt_address = 0;
	uint32_t xsdt_size;
	struct acpi_table_desc *next_table = table_list;
	int next_index;
	struct acpi_table_fadt *external_fadt = NULL;
	int table_count = 0;

	/* count tables */
	while (next_table) {
		if (acpi_compare_sig_name(ACPI_SIG_DSDT, next_table->signature) &&
		    acpi_compare_sig_name(ACPI_SIG_FADT, next_table->signature))
			table_count++;
		next_table = next_table->next;
	}

	/* build XSDT */
	xsdt_size = ACPI_EMU_XSDT_LENGTH + (table_count * sizeof (uint64_t));
	acpi_emu_local_xsdt = acpi_os_allocate_zeroed(xsdt_size);
	if (!acpi_emu_local_xsdt)
		return AE_NO_MEMORY;
	ACPI_NAMECPY(ACPI_SIG_XSDT, acpi_emu_local_xsdt->header.signature);
	ACPI_ENCODE32(&acpi_emu_local_xsdt->header.length, xsdt_size);
	ACPI_ENCODE8(&acpi_emu_local_xsdt->header.revision, 1);
	acpi_emu_local_xsdt->table_offset_entry[ACPI_EMU_XSDT_IDX_FADT] =
		ACPI_PTR_TO_PHYSADDR(&acpi_emu_local_fadt);
	next_index = ACPI_EMU_XSDT_NR_ENTRIES;
	next_table = table_list;
	while (next_table) {
		if (ACPI_NAMECMP(ACPI_SIG_DSDT, next_table->pointer->signature)) {
			if (dsdt_address)
				return AE_ALREADY_EXISTS;
			dsdt_address = ACPI_PTR_TO_PHYSADDR(&acpi_emu_default_dsdt);
			acpi_emu_dsdt_override = next_table->pointer;
		} else if (ACPI_NAMECMP(ACPI_SIG_FADT, next_table->pointer->signature)) {
			acpi_emu_external_fadt = ACPI_CAST_PTR(struct acpi_table_fadt, next_table->pointer);
			acpi_emu_local_xsdt->table_offset_entry[ACPI_EMU_XSDT_IDX_FADT] =
				ACPI_PTR_TO_PHYSADDR(next_table->pointer);
		} else {
			acpi_emu_local_xsdt->table_offset_entry[next_index] = ACPI_PTR_TO_PHYSADDR(next_table->pointer);
			next_index++;
		}
		next_table = next_table->next;
	}
	if (!dsdt_address) {
		dsdt_address = ACPI_PTR_TO_PHYSADDR (acpi_emu_default_dsdt);
		acpi_emu_dsdt_override = ACPI_CAST_PTR(struct acpi_table_header, acpi_emu_default_dsdt);
	}

	/* build RSDP */
	memset(&acpi_emu_local_rsdp, 0, sizeof (struct acpi_table_rsdp));
	ACPI_RSDP_SIG_CPY(acpi_emu_local_rsdp.signature);
	memcpy(acpi_emu_local_rsdp.oem_id, "ZETA", 6);
	ACPI_ENCODE8(&acpi_emu_local_rsdp.revision, 2);
	ACPI_ENCODE64(&acpi_emu_local_rsdp.xsdt_physical_address,
		      ACPI_PTR_TO_PHYSADDR (acpi_emu_local_xsdt));
	ACPI_ENCODE32(&acpi_emu_local_rsdp.length, sizeof (struct acpi_table_xsdt));

	/* Set checksums for both XSDT and RSDP */
	acpi_table_calc_checksum(&acpi_emu_local_xsdt->header);
	acpi_rsdp_calc_checksum(&acpi_emu_local_rsdp);

	if (acpi_emu_external_fadt) {
		/* Update FADT fields */
		acpi_emu_external_fadt->dsdt = (uint32_t)dsdt_address;
		if (!acpi_emu_reduced_hardware)
			ACPI_ENCODE32(&acpi_emu_external_fadt->facs,
				      ACPI_PTR_TO_PHYSADDR(&acpi_emu_local_facs));
		if (ACPI_DECODE32(&acpi_emu_external_fadt->header.length) >
		    (uint32_t)(ACPI_PTR_DIFF(&acpi_emu_external_fadt->Xdsdt, acpi_emu_external_fadt))) {
			ACPI_ENCODE64(&acpi_emu_external_fadt->Xdsdt, dsdt_address);
			if (!acpi_emu_reduced_hardware)
				ACPI_ENCODE64(&acpi_emu_external_fadt->Xfacs,
					      ACPI_PTR_TO_PHYSADDR(&acpi_emu_local_facs));
		}
		acpi_table_calc_checksum(&acpi_emu_external_fadt->header);
	} else {
		/* Build FADT */
		memset(&acpi_emu_local_fadt, 0, sizeof (struct acpi_table_fadt));
		ACPI_NAMECPY(ACPI_SIG_FADT, acpi_emu_local_fadt.header.signature);
		ACPI_ENCODE32(&acpi_emu_local_fadt.dsdt, 0);
		ACPI_ENCODE64(&acpi_emu_local_fadt.Xdsdt, dsdt_address);
		ACPI_ENCODE32(&acpi_emu_local_fadt.header.length,
			      sizeof (struct acpi_table_fadt));
		if (!acpi_emu_reduced_hardware) {
			ACPI_ENCODE8(&acpi_emu_local_fadt.header.revision, 0);
			ACPI_ENCODE32(&acpi_emu_local_fadt.facs, 0);
			ACPI_ENCODE64(&acpi_emu_local_fadt.Xfacs,
				      ACPI_PTR_TO_PHYSADDR(&acpi_emu_local_facs));
			ACPI_ENCODE8(&acpi_emu_local_fadt.gpe0_block_length, 16);
			ACPI_ENCODE32(&acpi_emu_local_fadt.gpe0_block, 0x00001234);
			ACPI_ENCODE8(&acpi_emu_local_fadt.gpe1_block_length, 6);
			ACPI_ENCODE32(&acpi_emu_local_fadt.gpe1_block, 0x00005678);
			ACPI_ENCODE8(&acpi_emu_local_fadt.gpe1_base, 96);	
			ACPI_ENCODE8(&acpi_emu_local_fadt.pm1_event_length, 4);
			ACPI_ENCODE32(&acpi_emu_local_fadt.pm1a_event_block, 0x00001aaa);
			ACPI_ENCODE32(&acpi_emu_local_fadt.pm1b_event_block, 0x00001bbb);
			ACPI_ENCODE8(&acpi_emu_local_fadt.pm1_control_length, 2);
			ACPI_ENCODE32(&acpi_emu_local_fadt.pm1a_control_block, 0xB0);
			ACPI_ENCODE8(&acpi_emu_local_fadt.pm_timer_length, 4);
			ACPI_ENCODE32(&acpi_emu_local_fadt.pm_timer_block, 0xA0);
			ACPI_ENCODE32(&acpi_emu_local_fadt.pm2_control_block, 0xC0);
			ACPI_ENCODE8(&acpi_emu_local_fadt.pm2_control_length, 1);
			acpi_encode_generic_address(&acpi_emu_local_fadt.xpm1b_event_block,
						    ACPI_ADR_SPACE_SYSTEM_IO,
						    ACPI_DECODE32(&acpi_emu_local_fadt.pm1b_event_block),
						    (uint8_t)ACPI_MUL_8(ACPI_DECODE8(&acpi_emu_local_fadt.pm1_event_length)));
		} else
			ACPI_ENCODE8(&acpi_emu_local_fadt.header.revision, 5);
		acpi_table_calc_checksum(&acpi_emu_local_fadt.header);
	}

	/* build FACS */
	memset(&acpi_emu_local_facs, 0, sizeof (struct acpi_table_facs));
	ACPI_NAMECPY(ACPI_SIG_FACS, acpi_emu_local_facs.signature);
	ACPI_ENCODE32(&acpi_emu_local_facs.length, sizeof (struct acpi_table_facs));
	ACPI_ENCODE32(&acpi_emu_local_facs.global_lock, 0x11AA0011);
	
	return AE_OK;
}

acpi_addr_t acpi_os_get_root_pointer(void)
{
	return (acpi_addr_t)&acpi_emu_local_rsdp;
}

void *acpi_os_map_memory(acpi_addr_t where, acpi_size_t length)
{
	return ACPI_TO_POINTER(where);
}

void acpi_os_unmap_memory(void *where, acpi_size_t length)
{
	return;
}

acpi_status_t acpi_os_table_override(struct acpi_table_header *existing_table,
				     acpi_addr_t *address, acpi_table_flags_t *flags)
{
	if (!existing_table || !address || !flags)
		return AE_BAD_PARAMETER;
	*address = ACPI_PTR_TO_PHYSADDR(NULL);

	if (ACPI_NAMECMP(ACPI_SIG_DSDT, existing_table->signature)) {
		*address = ACPI_PTR_TO_PHYSADDR(acpi_emu_dsdt_override);
		*flags = ACPI_TABLE_EXTERNAL_VIRTUAL;
	}
	return AE_OK;
}

acpi_status_t acpi_os_create_semaphore(uint32_t max_units,
				       uint32_t initial_units,
				       acpi_handle_t *phandle)
{
	void *mutex;
	uint32_t i;

	if (max_units == ACPI_UINT32_MAX)
		max_units = 255;
	if (initial_units == ACPI_UINT32_MAX)
		initial_units = max_units;
	if (initial_units > max_units)
		return (AE_BAD_PARAMETER);
	
	/* Find an empty slot */
	
	for (i = 0; i < ACPI_OS_MAX_SEMAPHORES; i++) {
		if (!acpi_emu_semaphores[i].handle)
			break;
	}
	if (i >= ACPI_OS_MAX_SEMAPHORES) {
		acpi_err("Reached max semaphores (%u), could not create",
			 ACPI_OS_MAX_SEMAPHORES);
		return AE_LIMIT;
	}
	
	mutex = CreateSemaphore (NULL, initial_units, max_units, NULL);
	if (!mutex) {
		acpi_err("Could not create semaphore");
		return AE_NO_MEMORY;
	}
	
	acpi_emu_semaphores[i].max_units = (uint16_t)max_units;
	acpi_emu_semaphores[i].current_units = (uint16_t)initial_units;
	acpi_emu_semaphores[i].handle = mutex;
	
	*phandle = (void *)i;
	return AE_OK;
}

acpi_status_t acpi_os_delete_semaphore(acpi_handle_t handle)
{
	uint32_t index = (uint32_t)handle;

	BUG_ON(index >= ACPI_OS_MAX_SEMAPHORES || !acpi_emu_semaphores[index].handle);

	CloseHandle(acpi_emu_semaphores[index].handle);
	acpi_emu_semaphores[index].handle = NULL;

	return AE_OK;
}

acpi_status_t acpi_os_wait_semaphore(acpi_handle_t handle,
				     uint32_t units,
				     uint16_t timeout)
{
	uint32_t index = (uint32_t)handle;
	uint32_t wait_status;
	uint32_t wait_timeout = timeout;

	BUG_ON(index >= ACPI_OS_MAX_SEMAPHORES || !acpi_emu_semaphores[index].handle);

	if (units > 1) {
		acpi_err("WaitSemaphore: Attempt to receive %u units\n", units);
		return AE_NOT_IMPLEMENTED;
	}
	
	if (timeout == ACPI_WAIT_FOREVER) {
		wait_timeout = INFINITE;
		if (acpi_emu_debug_timeout) {
			/* The debug timeout will prevent hang conditions */
			wait_timeout = ACPI_EMU_DEBUG_TIMEOUT;
		}
	} else {
		/* Add 10ms to account for clock tick granularity */
		wait_timeout += 10;
	}

	acpi_dbg("%d locked", index);
	wait_status = WaitForSingleObject(acpi_emu_semaphores[index].handle, wait_timeout);
	if (wait_status == WAIT_TIMEOUT) {
		if (acpi_emu_debug_timeout) {
			acpi_err("Debug timeout on semaphore 0x%04X (%ums)\n",
				 index, ACPI_EMU_DEBUG_TIMEOUT);
		}
		return AE_TIME;
	}
	
	if (acpi_emu_semaphores[index].current_units == 0) {
		acpi_err("No unit received. Timeout 0x%X, OS_Status 0x%X",
			 timeout, wait_status);
		return AE_OK;
	}
	
	acpi_emu_semaphores[index].current_units--;
	return AE_OK;
}

acpi_status_t acpi_os_signal_semaphore(acpi_handle_t handle,
				       uint32_t units)
{
	uint32_t index = (uint32_t)handle;

	BUG_ON(index >= ACPI_OS_MAX_SEMAPHORES || !acpi_emu_semaphores[index].handle);
	
	if (!acpi_emu_semaphores[index].handle) {
		acpi_err("SignalSemaphore: Null OS handle, Index %2.2X\n", index);
		return AE_BAD_PARAMETER;
	}
	
	if (units > 1) {
		acpi_err("SignalSemaphore: Attempt to signal %u units, Index %2.2X\n", units, index);
		return AE_NOT_IMPLEMENTED;
	}
	
	if ((acpi_emu_semaphores[index].current_units + 1) >
	     acpi_emu_semaphores[index].max_units) {
		acpi_err("Oversignalled semaphore[%u]! Current %u Max %u",
			 index, acpi_emu_semaphores[index].current_units,
			 acpi_emu_semaphores[index].max_units);
		return AE_LIMIT;
	}
	
	acpi_emu_semaphores[index].current_units++;

	acpi_dbg("%d unlocked", index);
	ReleaseSemaphore(acpi_emu_semaphores[index].handle, units, NULL);
	
	return AE_OK;
}

acpi_status_t acpi_os_create_lock(acpi_spinlock_t *phandle)
{
	return acpi_os_create_semaphore(1, 1, phandle);
}

void acpi_os_delete_lock(acpi_spinlock_t handle)
{
	acpi_os_delete_semaphore(handle);
}

acpi_cpuflags_t acpi_os_acquire_lock(acpi_spinlock_t handle)
{
	acpi_os_wait_semaphore(handle, 1, ACPI_WAIT_FOREVER);
	return 0;
}

void acpi_os_release_lock(acpi_spinlock_t handle, acpi_cpuflags_t flags)
{
	acpi_os_signal_semaphore(handle, 1);
}

void acpi_os_initialize(void)
{
	memset(acpi_emu_semaphores, 0x00, sizeof (acpi_emu_semaphores));
}

void acpi_os_sleep(uint32_t msecs)
{
	Sleep(msecs);
}

void acpi_os_debug_print(const char *fmt, ...)
{
	va_list	arglist;
	char output[400] = "ACPI: ";

	va_start(arglist, fmt);
	wvsprintfA(&output[6], fmt, arglist);
	va_end(arglist);

	lstrcatA(output, "\r\n");
	OutputDebugStringA(output);
}

void *acpi_os_allocate(acpi_size_t size)
{
	return (void *)heap_alloc((size_t)size);
}

void *acpi_os_allocate_zeroed(acpi_size_t size)
{
	return heap_calloc((size_t)size);
}

void acpi_os_free(void *mem)
{
	heap_free(mem);
}

struct acpi_test_TableUnload {
	int iterations;
	char filename[MAX_PATH];
};

boolean acpi_test_TableUnload_started = false;
struct acpi_reference acpi_test_TableUnload_count;

DWORD WINAPI acpi_test_TableUnload_thread(void *args)
{
	struct acpi_test_TableUnload *param = (struct acpi_test_TableUnload *)args;
	acpi_ddb_t ddb;
	int count = 20;

	while (param->iterations--) {
		if (!acpi_test_TableUnload_started)
			break;
		acpi_emu_load_table(param->filename, &ddb);
		acpi_os_sleep(1000);
		acpi_uninstall_table(ddb);
	}

	free(param);
	acpi_reference_dec(&acpi_test_TableUnload_count);
	return 0;
}

void acpi_test_TableUnload_start(const char *path,
				  int nr_threads, int iterations)
{
	HANDLE thread;
	int i;
	DIR *dirp;
	struct direct *entry = (struct direct *)0;
	struct acpi_test_TableUnload *param;

	if (!acpi_test_TableUnload_started && path) {
		dirp = opendir(path);
		if (!dirp) {
			acpi_err("can't open directory `%s'.\n", path);
			return;
		}

		acpi_test_TableUnload_started = true;

		i = 0;
		while ((entry = readdir(dirp)) != NULL && i < nr_threads) {
			if (strstr(entry->d_name, ".dat")) {
				param = acpi_os_allocate_zeroed(sizeof (struct acpi_test_TableUnload));
				if (!param)
					continue;
				param->iterations = iterations;
				sprintf(param->filename, "%s\\%s", path, entry->d_name);
				acpi_dbg("> %s\n", param->filename);
				thread = CreateThread(NULL, 0,
						      acpi_test_TableUnload_thread,
						      (void *)param, 0, NULL);
				acpi_reference_inc(&acpi_test_TableUnload_count);
				i++;
			}
		}
	}
	closedir(dirp);
}

void acpi_test_TableUnload_stop(void)
{
	while (acpi_reference_get(&acpi_test_TableUnload_count) != 0) {
		acpi_test_TableUnload_started = false;
		acpi_os_sleep(1000);
	}
}

void acpi_test_init(void)
{
	acpi_reference_set(&acpi_test_TableUnload_count, 0);
}

void acpi_test_exit(void)
{
	acpi_test_TableUnload_stop();
}

void acpi_emu_init(void)
{
	acpi_emu_build_tables(acpi_emu_table_list);
}
