#include <asm/fdt.h>
#include <target/reri.h>
#include <target/acpi.h>
#include <sbi/sbi_heap.h>

/* Structure for ghesv2 memory allocation housekeeping */
typedef struct {
	uint64_t init_done;
	uint64_t ghes_gas_reg_mem_sz;
	uint64_t ghes_gas_reg_addr;
	uint64_t ghes_gas_reg_addr_curr;
	uint64_t ghes_gas_reg_end_addr;
	uint64_t ghes_err_addr;
	uint64_t ghes_err_mem_sz;
	uint64_t ghes_err_addr_curr;
	uint64_t ghes_err_end_addr;
} acpi_ghes_mem_info;

typedef u64 read_ack_reg_t;

/* Ghes entries for error sources */
static struct acpi_hest_generic_v2 *err_sources;
static int err_src_alloc_idx;

/* ghesv2 memory housekeeping */
static acpi_ghes_mem_info ghes_mem;

/* GAS registers that will contain status block addresses */
static u64 *gas_register_allocs[MAX_ERR_SRCS];
static int gai;

#define ROUNDUP_2_64B(sz) ((sz + 0x40) & ~(0x40 - 1))

/* OSPM reach/ack register is at the end of each status block */
#define GHES_BLOCK_STATUS_SZ (sizeof(acpi_ghes_status_block) + sizeof(read_ack_reg_t))

void acpi_ghes_init(uint64_t addr, uint64_t size)
{
	if (size == 0 || addr == 0)
		return;

	static struct acpi_hest_generic_v2 sources[MAX_ERR_SRCS];
	/* Allocate memory for GHES entries */
	err_sources = &sources[0];
	if (err_sources == NULL)
		return;

	/* Initialize GHES memory pool */
	memset(&ghes_mem, 0, sizeof(ghes_mem));
	memset(&sources[0], 0, sizeof(sources));
	ghes_mem.ghes_err_addr_curr = ghes_mem.ghes_err_addr = addr;
	ghes_mem.ghes_err_mem_sz = size;
	ghes_mem.ghes_err_end_addr = (addr + (ROUNDUP_2_64B(GHES_BLOCK_STATUS_SZ) * MAX_ERR_SRCS + 64));

	/* Initialize the generic address structure pool */
	ghes_mem.ghes_gas_reg_mem_sz = sizeof(u64) * MAX_ERR_SRCS;
	ghes_mem.ghes_gas_reg_addr = ghes_mem.ghes_err_end_addr + 64;
	ghes_mem.ghes_gas_reg_addr_curr = ghes_mem.ghes_gas_reg_addr;
	ghes_mem.ghes_gas_reg_end_addr = (ghes_mem.ghes_gas_reg_addr + ghes_mem.ghes_gas_reg_mem_sz);
	ghes_mem.init_done = 1;
}

/*
 * Each status block contains address to a register which then points to
 * the status block. Allocate memory for u64* pointer which will contain
 * pointer to status block. Address of this register is stored in GAS entry
 */
static u64 *acpi_ghes_gas_register_alloc(void)
{
	u64 addr;

	if (ghes_mem.ghes_gas_reg_addr_curr >= ghes_mem.ghes_gas_reg_end_addr)
		return NULL;

	addr = ghes_mem.ghes_gas_reg_addr_curr;
	ghes_mem.ghes_gas_reg_addr_curr += sizeof(u64);

	gas_register_allocs[gai] = (u64 *)(ulong)addr;
	gai++;

	return (u64 *)(ulong)addr;
}

static void *acpi_ghes_alloc(uint64_t size)
{
	uint64_t naddr;
	uint64_t nsz;

	if (!ghes_mem.init_done)
		return NULL;

	/* if not multiple of 64-bytes */
	if (size & (0x40 - 1))
		/* round up to next 64 bytes */
		nsz = ROUNDUP_2_64B(size);
	else
		nsz = size;

	if (ghes_mem.ghes_err_addr_curr + nsz >= ghes_mem.ghes_err_end_addr)
		return NULL;


	naddr = ghes_mem.ghes_err_addr_curr;
	ghes_mem.ghes_err_addr_curr = ghes_mem.ghes_err_addr_curr + nsz;

	return ((void *)(ulong)naddr);
}

int acpi_ghes_new_error_source(uint64_t err_src_id, uint64_t sse_v)
{
	struct acpi_hest_generic_v2 *err_src;
	acpi_ghes_status_block *sblock;
	u64 rar_addr; /* read ack register address */
	u64 *baddr;

	if (err_src_alloc_idx >= MAX_ERR_SRCS)
		return SBI_EINVAL;

	if (!ghes_mem.init_done)
		return SBI_EINVAL;

	/* allocate generic address structure register */
	baddr = acpi_ghes_gas_register_alloc();
	if (baddr == NULL)
		return SBI_EINVAL;

	/* Allocate GHESv2 for source */
	err_src = (err_sources + err_src_alloc_idx);

	err_src->header.type = ACPI_GHES_SOURCE_GENERIC_ERROR_V2;
	err_src->header.source_id = err_src_id;
	err_src->enabled = 1;

	err_src->records_to_preallocate = MAX_ERR_RECS;
	err_src->max_sections_per_record = MAX_SECS_PER_REC;

	/* Allocate GHESv2 status block 8*/
	sblock = acpi_ghes_alloc(GHES_BLOCK_STATUS_SZ);
	if (sblock == NULL)
		return -1;

	err_src->error_block_length = sizeof(*sblock);

	/* OSPM Acknowledgement address (end of status block) */
	rar_addr = (ulong)((ulong)sblock + sizeof(acpi_ghes_status_block));
	/* Store status block pointer to GAS register address */
	*baddr = (ulong)sblock;

	/* Initialize GAS */
	err_src->error_status_address.space_id = AML_AS_SYSTEM_MEMORY;
	err_src->error_status_address.bit_width = sizeof(u64) * 8; /* 64-bit wide access */
	err_src->error_status_address.bit_offset = 0x0;
	err_src->error_status_address.access_width = 4; /* quad word access */
	err_src->error_status_address.address = (ulong)baddr; /* GAS register address */

	/* Initialize read ack register */
	err_src->read_ack_register.address = rar_addr;
	err_src->read_ack_register.bit_width = sizeof(read_ack_reg_t) * 8;
	err_src->read_ack_register.bit_offset = 0;
	err_src->read_ack_register.access_width = 4;
	err_src->read_ack_preserve = ~(1UL << sse_v);
	err_src->read_ack_write = (1UL << sse_v);

	/* Initialize notification structure */
	err_src->notify.type = ACPI_HEST_NOTIFY_SSE;
	err_src->notify.length = 28;
	err_src->notify.config_write_enable = 0;
	err_src->notify.poll_interval = 0;
	err_src->notify.vector = sse_v;
	err_src->notify.polling_threshold_value = 0;
	err_src->notify.polling_threshold_window = 0;
	err_src->notify.error_threshold_value = 0;
	err_src->notify.error_threshold_window = 0;

	err_src_alloc_idx++;

	return 0;
}

static struct acpi_hest_generic_v2 *find_error_source_by_id(uint8_t src_id)
{
	int i;
	struct acpi_hest_generic_v2 *err_src;

	for (i = 0; i < err_src_alloc_idx; i++) {
		err_src = &err_sources[i];
		if (err_src->header.source_id == src_id)
			return err_src;
	}

	return NULL;
}

static int ospm_acked_prev_err(struct acpi_generic_address *read_ack_register,
			       uint64_t ack_preserve, uint64_t ack_write)
{
	uint64_t resp, tmp;

	/* If there is no ack register, assume the previous error ack'ed */
	if (!read_ack_register->address)
		return 1;

	memcpy(&tmp, (void *)(uintptr_t)read_ack_register->address, sizeof(tmp));
	resp = tmp;

	/* If register contains zero, assume its acked */
	if (!resp)
		return 1;

	resp &= ack_preserve;
	resp |= ack_write;

	return !!resp;
}

static void ghes_record_mem_error(acpi_ghes_status_block *error_block,
				  acpi_ghes_error_info *einfo)
{
	uint32_t data_length;
	struct acpi_hest_generic_data *dentry;
	struct cper_sec_mem_err *msec;

	/* This is the length if adding a new generic error data entry*/
	data_length = ACPI_GHES_DATA_LENGTH + ACPI_GHES_MEM_CPER_LENGTH;

	/* Build the new generic error status block header */
	error_block->status.block_status = ACPI_HEST_UNCORRECTABLE;
	error_block->status.raw_data_offset = 0;
	error_block->status.raw_data_length = 0;
	error_block->status.data_length =  data_length;
	error_block->status.error_severity = einfo->info.me.err_sev;

	/* Build generic data entry header */
	dentry = &error_block->data;
	memcpy(dentry->section_type, &CPER_SEC_PLATFORM_MEM, sizeof(dentry->section_type));
	dentry->error_severity = einfo->info.me.err_sev;
	dentry->validation_bits = 0;
	dentry->flags = 0;
	dentry->error_data_length = ACPI_GHES_MEM_CPER_LENGTH;
	memset(dentry->fru_id, 0, sizeof(dentry->fru_id));

	msec = &error_block->cpers[0].sections[0].ms;
	memset(msec, 0, sizeof(*msec));

	msec->validation_bits = einfo->info.me.validation_bits;
	if (einfo->info.me.validation_bits & CPER_MEM_VALID_ERROR_STATUS)
		msec->error_status = einfo->info.me.err_status;
	if (einfo->info.me.validation_bits & CPER_MEM_VALID_PA)
		msec->physical_addr = einfo->info.me.physical_address;
	if (einfo->info.me.validation_bits & CPER_MEM_VALID_PA_MASK)
		msec->physical_addr_mask = einfo->info.me.physical_address_mask;
	if (einfo->info.me.validation_bits & CPER_MEM_VALID_NODE)
		msec->node = einfo->info.me.node;
	if (einfo->info.me.validation_bits & CPER_MEM_VALID_CARD)
		msec->card = einfo->info.me.card;
	if (einfo->info.me.validation_bits & CPER_MEM_VALID_MODULE)
		msec->module = einfo->info.me.module;
	if (einfo->info.me.validation_bits & CPER_MEM_VALID_BANK)
		msec->bank = einfo->info.me.bank;
	if (einfo->info.me.validation_bits & CPER_MEM_VALID_DEVICE)
		msec->device = einfo->info.me.device;
	if (einfo->info.me.validation_bits & CPER_MEM_VALID_ROW)
		msec->row = einfo->info.me.row;
	if (einfo->info.me.validation_bits & CPER_MEM_VALID_COLUMN)
		msec->column = einfo->info.me.column;
	if (einfo->info.me.validation_bits & CPER_MEM_VALID_BIT_POSITION)
		msec->bit_pos = einfo->info.me.bit_pos;
	if (einfo->info.me.validation_bits & CPER_MEM_VALID_REQUESTOR_ID)
		msec->requestor_id = einfo->info.me.req_id;
	if (einfo->info.me.validation_bits & CPER_MEM_VALID_RESPONDER_ID)
		msec->responder_id = einfo->info.me.resp_id;
	if (einfo->info.me.validation_bits & CPER_MEM_VALID_TARGET_ID)
		msec->target_id = einfo->info.me.target_id;
	if (einfo->info.me.validation_bits & CPER_MEM_VALID_ERROR_TYPE)
		msec->error_type = einfo->info.me.err_type;
	if (einfo->info.me.validation_bits & CPER_MEM_VALID_RANK_NUMBER)
		msec->rank = einfo->info.me.rank;
	if (einfo->info.me.validation_bits & CPER_MEM_VALID_ROW_EXT)
		msec->extended = einfo->info.me.extended;
}

static void ghes_record_generic_cpu_error(acpi_ghes_status_block *error_block,
					  acpi_ghes_error_info *einfo)
{
	struct acpi_hest_generic_data *dentry;
	struct cper_sec_proc_generic *psec;
	uint32_t data_length;

	/* This is the length if adding a new generic error data entry */
	data_length = ACPI_GHES_DATA_LENGTH + ACPI_GHES_GENERIC_CPU_CPER_LENGTH;

	/* Build the generic error status block */
	error_block->status.block_status = ACPI_HEST_UNCORRECTABLE;
	error_block->status.raw_data_offset = 0;
	error_block->status.raw_data_length = 0;
	error_block->status.data_length = data_length;
	error_block->status.error_severity = einfo->info.gpe.sev;

	/* Build generic data entry header */
	dentry = &error_block->data;
	memcpy(dentry->section_type, &CPER_SEC_PROC_GENERIC, sizeof(dentry->section_type));
	dentry->error_severity = einfo->info.gpe.sev;
	dentry->validation_bits = 0;
	dentry->flags = 0;
	dentry->error_data_length = ACPI_GHES_GENERIC_CPU_CPER_LENGTH;
	memset(dentry->fru_id, 0, sizeof(dentry->fru_id));

	/* generi processor error section */
	psec = &error_block->cpers[0].sections[0].ps;
	psec->validation_bits = einfo->info.gpe.validation_bits;

	/* Processor Type */
	if (einfo->info.gpe.validation_bits & CPER_PROC_VALID_TYPE)
		psec->proc_type = einfo->info.gpe.proc_type;
	/* ISA */
	if (einfo->info.gpe.validation_bits & CPER_PROC_VALID_ISA)
		psec->proc_isa = einfo->info.gpe.proc_isa;
	/* Error Type */
	if (einfo->info.gpe.validation_bits & CPER_PROC_VALID_ERROR_TYPE)
		psec->proc_error_type = einfo->info.gpe.proc_err_type;
	/* Operation */
	if (einfo->info.gpe.validation_bits & CPER_PROC_VALID_OPERATION)
		psec->operation = einfo->info.gpe.operation;
	/* Flags */
	if (einfo->info.gpe.validation_bits & CPER_PROC_VALID_FLAGS)
		psec->flags = einfo->info.gpe.flags;
	/* Level */
	if (einfo->info.gpe.validation_bits & CPER_PROC_VALID_LEVEL)
		psec->level = einfo->info.gpe.level;

	/* Reserved field - must always be zero */
	psec->reserved = 0;

	/* CPU version */
	if (einfo->info.gpe.validation_bits & CPER_PROC_VALID_VERSION)
		psec->cpu_version = einfo->info.gpe.cpu_version;

	if (einfo->info.gpe.validation_bits & CPER_PROC_VALID_ID)
		psec->proc_id = einfo->info.gpe.cpu_id;

	if (einfo->info.gpe.validation_bits & CPER_PROC_VALID_TARGET_ADDRESS)
		psec->target_addr = einfo->info.gpe.target_addr;

	if (einfo->info.gpe.validation_bits & CPER_PROC_VALID_REQUESTOR_ID)
		psec->requestor_id = einfo->info.gpe.req_ident;

	if (einfo->info.gpe.validation_bits & CPER_PROC_VALID_RESPONDER_ID)
		psec->responder_id = einfo->info.gpe.resp_ident;

	if (einfo->info.gpe.validation_bits & CPER_PROC_VALID_IP)
		psec->ip = einfo->info.gpe.ip;
}

static void ghes_record_pcie_error(acpi_ghes_status_block *error_block,
					  acpi_ghes_error_info *einfo)
{
	struct acpi_hest_generic_data *dentry;
	struct cper_sec_pcie *psec;
	uint32_t data_length;

	/* This is the length if adding a new generic error data entry */
	data_length = ACPI_GHES_DATA_LENGTH + ACPI_GHES_PCIE_CPER_LENGTH;

	/* Build the generic error status block */
	error_block->status.block_status = ACPI_HEST_UNCORRECTABLE;
	error_block->status.raw_data_offset = 0;
	error_block->status.raw_data_length = 0;
	error_block->status.data_length = data_length;
	error_block->status.error_severity = einfo->info.pcie.err_sev;

	/* Build generic data entry header */
	dentry = &error_block->data;
	memcpy(dentry->section_type, &CPER_SEC_PCIE, sizeof(dentry->section_type));
	dentry->error_severity = einfo->info.pcie.err_sev;
	dentry->validation_bits = 0;
	dentry->flags = 0;
	dentry->error_data_length = ACPI_GHES_PCIE_CPER_LENGTH;
	memset(dentry->fru_id, 0, sizeof(dentry->fru_id));

	/* PCIe error section */
	psec = &error_block->cpers[0].sections[0].pcie_sec;
	psec->validation_bits = einfo->info.pcie.validation_bits;

	if (einfo->info.pcie.validation_bits & CPER_PCIE_VALID_PORT_TYPE)
		psec->port_type = einfo->info.pcie.port_type;
	if (einfo->info.pcie.validation_bits & CPER_PCIE_VALID_VERSION) {
		psec->version.major = einfo->info.pcie.version_major;
		psec->version.minor = einfo->info.pcie.version_minor;
	}
	if (einfo->info.pcie.validation_bits & CPER_PCIE_VALID_COMMAND_STATUS) {
		psec->command = einfo->info.pcie.command;
		psec->status = einfo->info.pcie.status;
	}
	if (einfo->info.pcie.validation_bits & CPER_PCIE_VALID_DEVICE_ID)
		psec->device_id.device_id = einfo->info.pcie.device_id;
	if (einfo->info.pcie.validation_bits & CPER_PCIE_VALID_SERIAL_NUMBER) {
		psec->serial_number.lower = einfo->info.pcie.serial_number_lower;
		psec->serial_number.upper = einfo->info.pcie.serial_number_upper;
	}
	if (einfo->info.pcie.validation_bits & CPER_PCIE_VALID_BRIDGE_CONTROL_STATUS) {
		psec->bridge.secondary_status = einfo->info.pcie.bridge_secondary_status;
		psec->bridge.control = einfo->info.pcie.bridge_control;
	}
	if (einfo->info.pcie.validation_bits & CPER_PCIE_VALID_CAPABILITY)
		memcpy(psec->capability, einfo->info.pcie.capability, sizeof(psec->capability));
	if (einfo->info.pcie.validation_bits & CPER_PCIE_VALID_AER_INFO)
		memcpy(psec->aer_info, einfo->info.pcie.aer_info, sizeof(psec->aer_info));
	psec->device_id.vendor_id = einfo->info.pcie.vendor_id;
	psec->device_id.class_code[0] = einfo->info.pcie.class_code[0];
	psec->device_id.class_code[1] = einfo->info.pcie.class_code[1];
	psec->device_id.class_code[2] = einfo->info.pcie.class_code[2];
	psec->device_id.function = einfo->info.pcie.function;
	psec->device_id.device = einfo->info.pcie.device;
	psec->device_id.segment = einfo->info.pcie.segment;
	psec->device_id.bus = einfo->info.pcie.bus;
	psec->device_id.secondary_bus = einfo->info.pcie.secondary_bus;
	psec->device_id.slot = einfo->info.pcie.slot;
	psec->reserved = 0;
	memset(psec->version.reserved, 0, sizeof(psec->version.reserved));
	psec->device_id.reserved = 0;
}

void acpi_ghes_record_errors(uint8_t source_id, acpi_ghes_error_info *einfo)
{
	struct acpi_hest_generic_v2 *err_src;
	acpi_ghes_status_block *sblock;
	u64 *gas;

	err_src = find_error_source_by_id(source_id);
	if (!err_src)
		return;

	if (!ospm_acked_prev_err(&err_src->read_ack_register, err_src->read_ack_preserve, err_src->read_ack_write)) {
		printf("OSPM hasn't acknowledged the previous error. New "
			   "error record cannot be created.\n");
		return;
	}

	/*
	 * FIXME: Read gas address via a function that respects the
	 * gas parameters. Don't read directly after typecast.
	 */
	gas = (u64 *)(ulong)err_src->error_status_address.address;
	sblock = (acpi_ghes_status_block *)(ulong)(*gas);

	if (einfo->etype == ERROR_TYPE_MEM) {
		ghes_record_mem_error(sblock, einfo);
	} else if (einfo->etype == ERROR_TYPE_GENERIC_CPU) {
		ghes_record_generic_cpu_error(sblock, einfo);
	} else if (einfo->etype == ERROR_TYPE_PCIE) {
		ghes_record_pcie_error(sblock, einfo);
	} else {
		printf("%s: Unknown error type %u\n", __func__, einfo->etype);
	}
}

/*
 * Functions to provide error source information over
 * mpxy or other transport.
 */
int acpi_ghes_get_num_err_srcs(void)
{
	return err_src_alloc_idx;
}

int acpi_ghes_get_err_srcs_list(uint32_t *src_ids, uint32_t sz)
{
	int i;
	struct acpi_hest_generic_v2 *src;

	src = &err_sources[0];

	for (i = 0; i < err_src_alloc_idx; i++) {
		src_ids[i] = src->header.source_id;
		src++;
	}

	return err_src_alloc_idx;
}

int acpi_ghes_get_err_src_desc(uint32_t src_id, struct acpi_hest_generic_v2 *ghes)
{
	struct acpi_hest_generic_v2 *g;

	g = find_error_source_by_id(src_id);

	if (g == NULL)
		return -SBI_ENOENT;

	memcpy(ghes, g, sizeof(struct acpi_hest_generic_v2));

	return 0;
}

