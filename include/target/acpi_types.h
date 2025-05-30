#ifndef __ACPI_TYPES_H_INCLUDE__
#define __ACPI_TYPES_H_INCLUDE__

#include <target/generic.h>

#define IORESOURCE_BITS		0x000000ff	/* Bus-specific bits */

#define IORESOURCE_IO		0x00000100	/* Resource type */
#define IORESOURCE_MEM		0x00000200
#define IORESOURCE_IRQ		0x00000400
#define IORESOURCE_DRQ		0x00000800

#define IORESOURCE_TYPE_MASK	(IORESOURCE_IO | IORESOURCE_MEM \
				| IORESOURCE_IRQ | IORESOURCE_DRQ)

#define IORESOURCE_PREFETCH	0x00001000	/* No side effects */
#define IORESOURCE_READONLY	0x00002000
#define IORESOURCE_CACHEABLE	0x00004000
/* This resource filters all of the unclaimed transactions to the bus below. */
#define IORESOURCE_SUBTRACTIVE  0x00040000
/* The IO resource has a bus below it. */
#define IORESOURCE_BRIDGE	0x00080000
/* This is a request to allocate resource about 4G boundary. */
#define IORESOURCE_ABOVE_4G	0x00100000
/* The resource needs to be soft reserved in the coreboot table */
#define IORESOURCE_SOFT_RESERVE	0x00200000
/* The resource needs to be reserved in the coreboot table */
#define IORESOURCE_RESERVE	0x10000000
/* The IO resource assignment has been stored in the device */
#define IORESOURCE_STORED	0x20000000
/* An IO resource that has been assigned a value */
#define IORESOURCE_ASSIGNED	0x40000000
/* An IO resource the allocator must not change */
#define IORESOURCE_FIXED	0x80000000

/* PCI specific resource bits (IORESOURCE_BITS) */
#define IORESOURCE_PCI64		(1<<0)	/* 64bit long pci resource */
#define IORESOURCE_PCI_BRIDGE		(1<<1)  /* A bridge pci resource */
#define IORESOURCE_PCIE_RESIZABLE_BAR	(1<<2)  /* A Resizable BAR */

typedef uint64_t resource_t;
struct resource {
	resource_t base;	/* Base address of the resource */
	resource_t size;	/* Size of the resource */
	resource_t limit;	/* Largest valid value base + size -1 */
	const struct resource *next;	/* Next resource in the list */
	unsigned long flags;	/* Descriptions of the kind of resource */
	unsigned long index;	/* Bus specific per device resource id */
	unsigned char align;	/* Required alignment (log 2) of the resource */
	unsigned char gran;	/* Granularity (log 2) of the resource */
	/* Alignment must be >= the granularity of the resource */
};

#define CONFIG_ACPI_CPU_STRING	"CPU%-3d"

/*
 * The assigned ACPI ID for the coreboot project is 'BOOT'
 * http://www.uefi.org/acpi_id_list
 */
#define COREBOOT_ACPI_ID	"BOOT"      /* ACPI ID for coreboot HIDs */

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
#define ACPI_ADDRESS_SPACE_PCI			2	/* PCI config space */
#define ACPI_ADDRESS_SPACE_EC			3	/* Embedded controller */
#define ACPI_ADDRESS_SPACE_SMBUS		4	/* SMBus */
#define ACPI_ADDRESS_SPACE_CMOS			5	/* SystemCMOS */
#define ACPI_ADDRESS_SPACE_PCI_BAR_TARGET	6	/* PciBarTarget */
#define ACPI_ADDRESS_SPACE_IPMI			7	/* IPMI */
#define ACPI_ADDRESS_SPACE_GENERAL_PURPOSE_IO	8	/* GeneralPurposeIO */
#define ACPI_ADDRESS_SPACE_GENERIC_SERIAL_BUS	9	/* GenericSerialBus  */
#define ACPI_ADDRESS_SPACE_PCC			0x0A	/* Platform Comm. Channel */
#define ACPI_ADDRESS_SPACE_FIXED		0x7f	/* Functional fixed hardware */
#define  ACPI_FFIXEDHW_VENDOR_INTEL		1	/* Intel */
#define  ACPI_FFIXEDHW_CLASS_HLT		0	/* C1 Halt */
#define  ACPI_FFIXEDHW_CLASS_IO_HLT		1	/* C1 I/O then Halt */
#define  ACPI_FFIXEDHW_CLASS_MWAIT		2	/* MWAIT Native C-state */
#define  ACPI_FFIXEDHW_FLAG_HW_COORD		1	/* Hardware Coordination bit */
#define  ACPI_FFIXEDHW_FLAG_BM_STS		2	/* BM_STS avoidance bit */
/* 0x80-0xbf: Reserved */
/* 0xc0-0xff: OEM defined */

/* Access size definitions for Generic address structure */
#define ACPI_ACCESS_SIZE_UNDEFINED	0	/* Undefined (legacy reasons) */
#define ACPI_ACCESS_SIZE_BYTE_ACCESS	1
#define ACPI_ACCESS_SIZE_WORD_ACCESS	2
#define ACPI_ACCESS_SIZE_DWORD_ACCESS	3
#define ACPI_ACCESS_SIZE_QWORD_ACCESS	4

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
