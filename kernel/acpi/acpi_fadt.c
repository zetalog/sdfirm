#include <target/acpi.h>

struct acpi_fadt_info {
	char *name;
	uint16_t address64;
	uint16_t address32;
	uint16_t length;
	uint8_t default_bit_width;
	uint8_t type;
	
};

struct acpi_fadt_pm_info {
	struct acpi_generic_address *target;
	uint16_t source;
	uint8_t register_num;
};

#define ACPI_FADT_OPTIONAL		0
#define ACPI_FADT_REQUIRED		1
#define ACPI_FADT_SEPARATE_LENGTH	2

static struct acpi_table_fadt acpi_gbl_FADT;
boolean acpi_gbl_fadt_use_default_register_width = true;
boolean acpi_gbl_fadt_use_32bit_addresses = false;

struct acpi_generic_address acpi_gbl_xpm1a_enable;
struct acpi_generic_address acpi_gbl_xpm1b_enable;
struct acpi_generic_address acpi_gbl_xpm1a_status;
struct acpi_generic_address acpi_gbl_xpm1b_status;

static struct acpi_fadt_info acpi_fadt_info_table[] = {
	{
		"Pm1aEventBlock",
		ACPI_FADT_OFFSET(xpm1a_event_block),
		ACPI_FADT_OFFSET(pm1a_event_block),
		ACPI_FADT_OFFSET(pm1_event_length),
		ACPI_PM1_REGISTER_WIDTH * 2,        /* Enable + Status register */
		ACPI_FADT_REQUIRED
	},
	{
		"Pm1bEventBlock",
		ACPI_FADT_OFFSET(xpm1b_event_block),
		ACPI_FADT_OFFSET(pm1b_event_block),
		ACPI_FADT_OFFSET(pm1_event_length),
		ACPI_PM1_REGISTER_WIDTH * 2,        /* Enable + Status register */
		ACPI_FADT_OPTIONAL
	},
	{
		"Pm1aControlBlock",
		ACPI_FADT_OFFSET(xpm1a_control_block),
		ACPI_FADT_OFFSET(pm1a_control_block),
		ACPI_FADT_OFFSET(pm1_control_length),
		ACPI_PM1_REGISTER_WIDTH,
		ACPI_FADT_REQUIRED
	},
	{
		"Pm1bControlBlock",
		ACPI_FADT_OFFSET(xpm1b_control_block),
		ACPI_FADT_OFFSET(pm1b_control_block),
		ACPI_FADT_OFFSET(pm1_control_length),
		ACPI_PM1_REGISTER_WIDTH,
		ACPI_FADT_OPTIONAL
	},
	{
		"Pm2ControlBlock",
		ACPI_FADT_OFFSET(xpm2_control_block),
		ACPI_FADT_OFFSET(pm2_control_block),
		ACPI_FADT_OFFSET(pm2_control_length),
		ACPI_PM2_REGISTER_WIDTH,
		ACPI_FADT_SEPARATE_LENGTH
	},
	{
		"PmTimerBlock",
		ACPI_FADT_OFFSET(xpm_timer_block),
		ACPI_FADT_OFFSET(pm_timer_block),
		ACPI_FADT_OFFSET(pm_timer_length),
		ACPI_PM_TIMER_WIDTH,
		ACPI_FADT_SEPARATE_LENGTH         /* ACPI 5.0A: Timer is optional */
	},
	{
		"Gpe0Block",
		ACPI_FADT_OFFSET(xgpe0_block),
		ACPI_FADT_OFFSET(gpe0_block),
		ACPI_FADT_OFFSET(gpe0_block_length),
		0,
		ACPI_FADT_SEPARATE_LENGTH
	},
	{
		"Gpe1Block",
		ACPI_FADT_OFFSET(xgpe1_block),
		ACPI_FADT_OFFSET(gpe1_block),
		ACPI_FADT_OFFSET(gpe1_block_length),
		0,
		ACPI_FADT_SEPARATE_LENGTH
	}
};
#define ACPI_FADT_INFO_ENTRIES		ARRAY_SIZE(acpi_fadt_info_table)

static struct acpi_fadt_pm_info acpi_fadt_pm_info_table[] =
{
	{
		&acpi_gbl_xpm1a_status,
		ACPI_FADT_OFFSET(xpm1a_event_block),
		0
	},
	{
		&acpi_gbl_xpm1a_enable,
		ACPI_FADT_OFFSET(xpm1a_event_block),
		1
	},
	{
		&acpi_gbl_xpm1b_status,
		ACPI_FADT_OFFSET(xpm1a_event_block),
		0
	},
	{
		&acpi_gbl_xpm1b_enable,
		ACPI_FADT_OFFSET(xpm1a_event_block),
		1
	},
};
#define ACPI_FADT_PM_INFO_ENTRIES	ARRAY_SIZE(acpi_fadt_pm_info_table)

uint32_t acpi_fadt_flag_is_set(uint32_t mask)
{
	return ACPI_DECODE32(&acpi_gbl_FADT.flags) & mask;
}

static uint64_t acpi_fadt_select_address(char *name,
					 uint32_t address32,
					 uint64_t address64)
{
	if (!address64)
		return (uint64_t)address32;

	if (address32 && (address64 != (uint64_t)address32)) {
		acpi_warn_bios("32/64X %s address mismatch in FADT: "
			       "0x%8.8X/0x%8.8X%8.8X, using %u-bit address",
			       name, address32, ACPI_FORMAT_UINT64(address64),
			       acpi_gbl_fadt_use_32bit_addresses ? 32 : 64);
		if (acpi_gbl_fadt_use_32bit_addresses)
			return (uint64_t)address32;
	}

	return address64;
}

static void acpi_fadt_select_gas(char *name,
				 struct acpi_generic_address *generic_address,
				 uint8_t space_id,
				 uint32_t address32,
				 uint64_t address64,
				 uint16_t bit_width,
				 uint8_t default_bit_width)
{
	if (!address64)
		acpi_encode_generic_address(generic_address,
					    space_id, address32, bit_width);

	if (address32 && (address64 != (uint64_t)address32)) {
		acpi_warn_bios("32/64X %s address mismatch in FADT: "
			       "0x%8.8X/0x%8.8X%8.8X, using %u-bit address",
			       name, address32, ACPI_FORMAT_UINT64(address64),
			       acpi_gbl_fadt_use_32bit_addresses ? 32 : 64);
		if (acpi_gbl_fadt_use_32bit_addresses)
			acpi_encode_generic_address(generic_address,
						    space_id, address32, bit_width);
	}

	if (acpi_gbl_fadt_use_default_register_width && default_bit_width) {
		if (bit_width != default_bit_width) {
			acpi_warn_bios("Invalid length for FADT/%s: %u, using default %u",
				        name, bit_width, default_bit_width);
			ACPI_ENCODE8(&generic_address->bit_width, default_bit_width);
		}
	}
}

static void acpi_fadt_convert(void)
{
	struct acpi_generic_address *generic_address64;
	uint32_t *address32;
	uint64_t *address64;
	uint8_t *length32;
	uint32_t i;

	/*
	 * For ACPI 1.0 FADTs (revision 1 or 2), ensure that reserved
	 * fields which should be zero are indeed zero. This will
	 * workaround BIOSs that inadvertently place values in these
	 * fields.
	 * The ACPI 1.0 reserved fields that will be zeroed are the bytes
	 * located at offset 45, 55, 95, and the word located at offset
	 * 109, 110.
	 */
	if (ACPI_DECODE32(&acpi_gbl_FADT.header.length) <= ACPI_FADT_V2_SIZE) {
		ACPI_ENCODE8(&acpi_gbl_FADT.preferred_profile, 0);
		ACPI_ENCODE8(&acpi_gbl_FADT.pstate_control, 0);
		ACPI_ENCODE8(&acpi_gbl_FADT.cst_control, 0);
		ACPI_ENCODE16(&acpi_gbl_FADT.boot_flags, 0);
	}

	ACPI_ENCODE32(&acpi_gbl_FADT.header.length,
		      sizeof (struct acpi_table_fadt));

	ACPI_ENCODE64(&acpi_gbl_FADT.Xfacs,
		      acpi_fadt_select_address("FACS",
					       ACPI_DECODE32(&acpi_gbl_FADT.facs),
					       ACPI_DECODE64(&acpi_gbl_FADT.Xfacs)));
	ACPI_ENCODE64(&acpi_gbl_FADT.Xdsdt,
		      acpi_fadt_select_address("DSDT",
					       ACPI_DECODE32(&acpi_gbl_FADT.dsdt),
					       ACPI_DECODE64(&acpi_gbl_FADT.Xdsdt)));

	if (acpi_fadt_flag_is_set(ACPI_FADT_HW_REDUCED))
		return;

	for (i = 0; i < ACPI_FADT_INFO_ENTRIES; i++) {
		generic_address64 = ACPI_ADD_PTR(struct acpi_generic_address,
						 &acpi_gbl_FADT,
						 acpi_fadt_info_table[i].address64);
		address32 = ACPI_ADD_PTR(uint32_t, &acpi_gbl_FADT,
					 acpi_fadt_info_table[i].address32);
		length32 = ACPI_ADD_PTR(uint8_t, &acpi_gbl_FADT,
					acpi_fadt_info_table[i].length);
		address64 = ACPI_ADD_PTR(uint64_t, generic_address64,
					 ACPI_OFFSET(struct acpi_generic_address, address));

		acpi_fadt_select_gas(acpi_fadt_info_table[i].name,
				     generic_address64,
				     ACPI_ADR_SPACE_SYSTEM_IO,
				     ACPI_DECODE32(address32),
				     ACPI_DECODE64(address64),
				     (uint16_t)ACPI_MUL_8(ACPI_DECODE8(length32)),
				     acpi_fadt_info_table[i].default_bit_width);
	}
}

static void acpi_fadt_setup_registers(void)
{
	struct acpi_generic_address *source64;
	uint8_t pm1_reg_byte_width;
	uint32_t i;

	pm1_reg_byte_width = (uint8_t)ACPI_DIV_16(ACPI_DECODE8(&acpi_gbl_FADT.xpm1a_event_block.bit_width));
	
	for (i = 0; i < ACPI_FADT_PM_INFO_ENTRIES; i++){
		source64 = ACPI_ADD_PTR(struct acpi_generic_address,
					&acpi_gbl_FADT,
					acpi_fadt_pm_info_table[i].source);
		if (ACPI_DECODE64(&source64->address)) {
			acpi_encode_generic_address(acpi_fadt_pm_info_table[i].target,
						    ACPI_DECODE8(&source64->space_id),
						    ACPI_DECODE64(&source64->address) +
						    (acpi_fadt_pm_info_table[i].register_num *
						    pm1_reg_byte_width),
						    (uint16_t)ACPI_MUL_8(pm1_reg_byte_width));
		}
	}
}

static void acpi_fadt_copy_local(struct acpi_table_header *table)
{
	uint32_t length;

	length = min(ACPI_DECODE32(&table->length),
		     sizeof (struct acpi_table_fadt));

	memcpy((void *)&acpi_gbl_FADT, (void *)table, length);
	memset((void *)&acpi_gbl_FADT, 0, sizeof (struct acpi_table_fadt) - length);

	acpi_fadt_convert();
	acpi_fadt_setup_registers();
}

void acpi_fadt_parse(struct acpi_table_header *table)
{
	uint32_t length;

	if (!table)
		return;
	length = table->length;
	__acpi_table_checksum_valid(table);
	acpi_fadt_copy_local(table);

	/* Obtain the DSDT and FACS tables via their addresses within the FADT */
	acpi_table_install_fixed((acpi_addr_t)ACPI_DECODE64(&acpi_gbl_FADT.Xdsdt),
				 ACPI_SIG_DSDT, ACPI_DDB_HANDLE_DSDT);
	if (acpi_fadt_flag_is_set(ACPI_FADT_HW_REDUCED))
		acpi_table_install_fixed((acpi_addr_t)ACPI_DECODE64(&acpi_gbl_FADT.Xfacs),
					 ACPI_SIG_FACS, ACPI_DDB_HANDLE_FACS);
}
