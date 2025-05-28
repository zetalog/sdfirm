#ifndef __ACPI_TYPES_H_INCLUDE__
#define __ACPI_TYPES_H_INCLUDE__

#include <target/generic.h>

/* List of ACPI HID that use the coreboot ACPI ID */
enum coreboot_acpi_ids {
	COREBOOT_ACPI_ID_CBTABLE	= 0x0000, /* BOOT0000 */
	COREBOOT_ACPI_ID_IGD_GMBUS_ARB  = 0x0001, /* BOOT0001 */
	COREBOOT_ACPI_ID_IGD_GMBUS_LINK = 0x0002, /* BOOT0002 */
	COREBOOT_ACPI_ID_AMDGFX_ACP	= 0x0003, /* BOOT0003 */
	COREBOOT_ACPI_ID_MAX		= 0xFFFF, /* BOOTFFFF */
};

/* RSDP (Root System Description Pointer) */
typedef struct acpi_rsdp {
	char  signature[8];	/* RSDP signature */
	uint8_t    checksum;		/* Checksum of the first 20 bytes */
	char  oem_id[6];	/* OEM ID */
	uint8_t    revision;		/* RSDP revision */
	uint32_t   rsdt_address;	/* Physical address of RSDT (32 bits) */
	uint32_t   length;		/* Total RSDP length (incl. extended part) */
	uint64_t   xsdt_address;	/* Physical address of XSDT (64 bits) */
	uint8_t    ext_checksum;	/* Checksum of the whole table */
	uint8_t    reserved[3];
} __packed acpi_rsdp_t;

/* GAS (Generic Address Structure) */
typedef struct acpi_gen_regaddr {
	uint8_t  space_id;		/* Address space ID */
	uint8_t  bit_width;		/* Register size in bits */
	uint8_t  bit_offset;		/* Register bit offset */
	uint8_t  access_size;	/* Access size since ACPI 2.0c */
	uint32_t addrl;		/* Register address, low 32 bits */
	uint32_t addrh;		/* Register address, high 32 bits */
} __packed acpi_addr_t;

#define ACPI_ADDRESS_SPACE_MEMORY		0	/* System memory */
#define ACPI_ADDRESS_SPACE_IO			1	/* System I/O */

typedef struct acpi_cstate {
	uint8_t  ctype;
	uint16_t latency;
	uint32_t power;
	acpi_addr_t resource;
} __packed acpi_cstate_t;

struct acpi_sw_pstate {
	uint32_t core_freq;
	uint32_t power;
	uint32_t transition_latency;
	uint32_t bus_master_latency;
	uint32_t control_value;
	uint32_t status_value;
} __packed;

struct acpi_xpss_sw_pstate {
	uint64_t core_freq;
	uint64_t power;
	uint64_t transition_latency;
	uint64_t bus_master_latency;
	uint64_t control_value;
	uint64_t status_value;
	uint64_t control_mask;
	uint64_t status_mask;
} __packed;

typedef struct acpi_tstate {
	uint32_t percent;
	uint32_t power;
	uint32_t latency;
	uint32_t control;
	uint32_t status;
} __packed acpi_tstate_t;

enum acpi_lpi_state_flags {
	ACPI_LPI_STATE_DISABLED = 0,
	ACPI_LPI_STATE_ENABLED
};

/* Low Power Idle State */
struct acpi_lpi_state {
	uint32_t min_residency_us;
	uint32_t worst_case_wakeup_latency_us;
	uint32_t flags;
	uint32_t arch_context_lost_flags;
	uint32_t residency_counter_frequency_hz;
	uint32_t enabled_parent_state;
	acpi_addr_t entry_method;
	acpi_addr_t residency_counter_register;
	acpi_addr_t usage_counter_register;
	const char *state_name;
};

/* Port types for ACPI _UPC object */
enum acpi_upc_type {
	UPC_TYPE_A,
	UPC_TYPE_MINI_AB,
	UPC_TYPE_EXPRESSCARD,
	UPC_TYPE_USB3_A,
	UPC_TYPE_USB3_B,
	UPC_TYPE_USB3_MICRO_B,
	UPC_TYPE_USB3_MICRO_AB,
	UPC_TYPE_USB3_POWER_B,
	UPC_TYPE_C_USB2_ONLY,
	UPC_TYPE_C_USB2_SS_SWITCH,
	UPC_TYPE_C_USB2_SS,
	UPC_TYPE_PROPRIETARY = 0xff,
	/*
	 * The following types are not directly defined in the ACPI
	 * spec but are used by coreboot to identify a USB device type.
	 */
	UPC_TYPE_INTERNAL = 0xff,
	UPC_TYPE_UNUSED,
	UPC_TYPE_HUB
};

#include <target/acpi_pld.h>
#include <target/acpi_device.h>

#endif /* __ACPI_TYPES_H_INCLUDE__ */
