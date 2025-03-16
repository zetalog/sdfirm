/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems, Inc.
 *
 * Author(s):
 *   Himanshu Chauhan <hchauhan@ventanamicro.com>
 */

#ifndef __APEI_TABLES_H
#define __APEI_TABLES_H

#define MAX_ERR_SRCS		32
#define MAX_ERR_RECS		1
#define MAX_SECS_PER_REC	1

enum acpi_ghes_notification_type {
	/* Polled */
	ACPI_GHES_NOTIFY_POLLED = 0,
	/* External Interrupt */
	ACPI_GHES_NOTIFY_EXTERNAL = 1,
	/* Local Interrupt */
	ACPI_GHES_NOTIFY_LOCAL = 2,
	/* SCI */
	ACPI_GHES_NOTIFY_SCI = 3,
	/* NMI */
	ACPI_GHES_NOTIFY_NMI = 4,
	/* CMCI, ACPI 5.0: 18.3.2.7, Table 18-290 */
	ACPI_GHES_NOTIFY_CMCI = 5,
	/* MCE, ACPI 5.0: 18.3.2.7, Table 18-290 */
	ACPI_GHES_NOTIFY_MCE = 6,
	/* GPIO-Signal, ACPI 6.0: 18.3.2.7, Table 18-332 */
	ACPI_GHES_NOTIFY_GPIO = 7,
	/* ARMv8 SEA, ACPI 6.1: 18.3.2.9, Table 18-345 */
	ACPI_GHES_NOTIFY_SEA = 8,
	/* ARMv8 SEI, ACPI 6.1: 18.3.2.9, Table 18-345 */
	ACPI_GHES_NOTIFY_SEI = 9,
	/* External Interrupt - GSIV, ACPI 6.1: 18.3.2.9, Table 18-345 */
	ACPI_GHES_NOTIFY_GSIV = 10,
	/* Software Delegated Exception, ACPI 6.2: 18.3.2.9, Table 18-383 */
	ACPI_GHES_NOTIFY_SDEI = 11,
	/* RISCV Supervisor Software Event */
	ACPI_GHES_NOTIFY_SSE = 12,
	/* 12 and greater are reserved */
	ACPI_GHES_NOTIFY_RESERVED = 13
};

/*
 * Error Source IDs for GHES. These are just place holders
 * and each platform can define its own source ID for
 * each error source.
 */
enum {
	ACPI_GHES_DRAM_ERROR_SOURCE_ID,
	ACPI_GHES_GENERIC_CPU_ERROR_SOURCE_ID,
	ACPI_GHES_SOURCE_ID_MAX,
};

typedef struct {
	uint64_t ghes_addr_le;
	bool present; /* True if GHES is present at all on this board */
} acpi_ghes_state;

enum {
	ERROR_TYPE_MEM,
	ERROR_TYPE_GENERIC_CPU,
	ERROR_TYPE_MAX,
};

enum {
	GPE_PROC_TYPE_VALID_BIT,
	GPE_PROC_ISA_VALID_BIT,
	GPE_PROC_ERR_TYPE_VALID_BIT,
	GPE_OP_VALID_BIT,
	GPE_FLAGS_VALID_BIT,
	GPE_LEVEL_VALID_BIT,
	GPE_CPU_VERSION_VALID_BIT,
	GPE_CPU_BRAND_STRING_VALID_BIT,
	GPE_CPU_ID_VALID_BIT,
	GPE_TARGET_ADDR_VALID_BIT,
	GPE_REQ_IDENT_VALID_BIT,
	GPE_RESP_IDENT_VALID_BIT,
	GPE_IP_VALID_BIT,
	GPE_BIT_RESERVED_BITS,
};

#define GPE_PROC_TYPE_VALID             (1ul << GPE_PROC_TYPE_VALID_BIT)
#define GPE_PROC_ISA_VALID              (1ul << GPE_PROC_ISA_VALID_BIT)
#define GPE_PROC_ERR_TYPE_VALID         (1ul << GPE_PROC_ERR_TYPE_VALID_BIT)
#define GPE_OP_VALID                    (1ul << GPE_OP_VALID_BIT)
#define GPE_FLAGS_VALID                 (1ul << GPE_FLAGS_VALID_BIT)
#define GPE_LEVEL_VALID                 (1ul << GPE_LEVEL_VALID_BIT)
#define GPE_CPU_VERSION_VALID           (1ul << GPE_CPU_VERSION_VALID_BIT)
#define GPE_CPU_BRAND_STRING_VALID      (1ul << GPE_CPU_BRAND_STRING_VALID_BIT)
#define GPE_CPU_ID_VALID                (1ul << GPE_CPU_ID_VALID_BIT)
#define GPE_TARGET_ADDR_VALID           (1ul << GPE_TARGET_ADDR_VALID_BIT)
#define GPE_REQ_IDENT_VALID             (1ul << GPE_REQ_IDENT_VALID_BIT)
#define GPE_RESP_IDENT_VALID            (1ul << GPE_RESP_IDENT_VALID_BIT)
#define GPE_IP_VALID                    (1ul << GPE_IP_VALID_BIT)

enum {
	GHES_PROC_TYPE_IA32X64,
	GHES_PROC_TYPE_IA64,
	GHES_PROC_TYPE_ARM,
	GHES_PROC_TYPE_RISCV,
};

enum {
	GHES_PROC_ISA_IA32,
	GHES_PROC_ISA_IA64,
	GHES_PROC_ISA_X64,
	GHES_PROC_ISA_ARM_A32,
	GHES_PROC_ISA_ARM_A64,
	GHES_PROC_ISA_RISCV32,
	GHES_PROC_ISA_RISCV64,
};

enum {
	AML_AS_SYSTEM_MEMORY = 0X00,
	AML_AS_SYSTEM_IO = 0X01,
	AML_AS_PCI_CONFIG = 0X02,
	AML_AS_EMBEDDED_CTRL = 0X03,
	AML_AS_SMBUS = 0X04,
	AML_AS_FFH = 0X7F,
};

typedef struct __packed {
	uint8_t asid;
	uint8_t reg_bwidth;
	uint8_t reg_boffs;
	uint8_t access_sz;
	uint64_t address;
} acpi_gas;

typedef struct __packed {
	uint8_t type;
	uint8_t length;
	uint16_t config_we;
	uint32_t poll_interval;
	uint32_t vector;
	uint32_t poll_switch_thresh;
	uint32_t poll_switch_thresh_win;
	uint32_t err_thresh;
	uint32_t err_thresh_win;
} acpi_ghes_notif;

typedef struct __packed {
	uint16_t type;
	uint16_t src_id;
	uint16_t rel_id;
	uint8_t flags;
	uint8_t enabled;
	uint32_t num_rec_pre_alloc;
	uint32_t max_sec_per_rec;
	uint32_t max_raw_dlen;
	acpi_gas gas;
	acpi_ghes_notif notif;
	uint32_t err_status_block_len;
} acpi_ghes;

typedef struct __packed {
	acpi_ghes ghes;
	acpi_gas ack_reg;
	uint64_t ack_preserve;
	uint64_t ack_write;
} acpi_ghesv2;

typedef struct __packed {
	uint8_t type[16];
} acpi_ghes_section_type;

/*
 * CPER Format
 *
 * +------------------------------------+
 * |  Record Header                     |
 * +------------------------------------+
 * |  Section Descriptor                |
 * +------------------------------------+
 * |  Section Descriptor (N)            |
 * +------------------------------------+
 * |  Section                           |
 * +------------------------------------+
 * |  Section (N)                       |
 * +------------------------------------+
 */
typedef struct __packed {
	uint8_t sign[4];
	uint16_t rev;
	uint32_t sign_end;
	uint16_t section_count;
	uint32_t err_sev;
	uint32_t vbits; /* validation bits */
	uint32_t rec_len;
	uint64_t timestamp;
	uint8_t plat_id[16];
	uint8_t part_id[48];
	uint8_t creator_id[16];
	uint8_t notif_type[16];
	uint64_t rec_id; /* record id */
	uint32_t flags;
	uint64_t persistence_info;
	uint8_t resvd[12];
} cper_header;

typedef struct __packed {
	uint32_t section_offs;
	uint32_t section_len;
	uint16_t rev;
	uint8_t vbits; /* validation bits */
	uint8_t resvd;
	uint32_t flags;
	acpi_ghes_section_type section_type;
	uint8_t fru_id[16];
	uint32_t section_sev; /* Severity */
	uint8_t fru_text[20];
} cper_section_desc;

typedef struct __packed {
	uint64_t vbits; /* validation bits */
	uint8_t proc_type;
	uint8_t proc_isa;
	uint8_t proc_err_type;
	uint8_t operation;
	uint8_t flags;
	uint8_t level;
	uint16_t resvd;
	uint64_t cpu_version_info;
	uint8_t cpu_brand_string[128];
	uint64_t proc_id;
	uint64_t target_addr;
	uint64_t requestor_id;
	uint64_t responder_id;
	uint64_t ins_ip; /* Instruction IP */
} cper_gen_proc_sec;

typedef struct __packed {
	uint64_t vbits;
	uint64_t err_status;
	uint64_t phys_addr;
	uint64_t phys_addr_mask;
	uint16_t node;
	uint16_t card;
	uint16_t module;
	uint16_t bank;
	uint32_t device;
	uint32_t row;
	uint32_t column;
	uint32_t rank;
	uint32_t bit_pos;
	uint8_t chip_id;
	uint8_t err_type;
	uint8_t status;
	uint8_t resvd;
	uint64_t requestor_id;
	uint64_t responder_id;
	uint64_t target_id;
	uint32_t card_handle;
	uint32_t module_handle;
} cper_mem2_sec;

typedef union __packed {
	cper_gen_proc_sec ps;
	cper_mem2_sec ms;
} cper_section;

/* CPER Record */
typedef struct __packed {
	cper_section sections[MAX_SECS_PER_REC];
} acpi_ghes_cper;

typedef struct __packed {
	acpi_ghes_section_type type;
	uint32_t err_sev;
	uint16_t rev;
	uint8_t vbits;
	uint8_t flags;
	uint32_t err_dlen;
	uint8_t fru_id[16];
	uint8_t fru_text[20];
	acpi_ghes_cper cpers[MAX_ERR_RECS];
} acpi_ghes_data_entry;

typedef struct __packed {
	uint32_t block_status;
	uint32_t raw_doffs;
	uint32_t raw_dlen;
	uint32_t data_len;
	uint32_t err_sev;
	acpi_ghes_data_entry entry;
} acpi_ghes_status_block;

#define KiB	1024

/* The max size in bytes for one error block */
#define ACPI_GHES_MAX_RAW_DATA_LENGTH   (1 * KiB)

/* Generic Hardware Error Source version 2 */
#define ACPI_GHES_SOURCE_GENERIC_ERROR_V2   10

/* Address offset in Generic Address Structure(GAS) */
#define GAS_ADDR_OFFSET 4

/*
 * The total size of Generic Error Data Entry
 * ACPI 6.1/6.2: 18.3.2.7.1 Generic Error Data,
 * Table 18-343 Generic Error Data Entry
 */
#define ACPI_GHES_DATA_LENGTH               64

/* The memory section CPER size, UEFI 2.6: N.2.5 Memory Error Section */
#define ACPI_GHES_MEM_CPER_LENGTH           80

/* The generic cpu CPER size, UEFI 2.10 2.4.1 Generic processor error */
#define ACPI_GHES_GENERIC_CPU_CPER_LENGTH   192

/* Masks for block_status flags */
#define ACPI_GEBS_UNCORRECTABLE         (0x1UL << 0)
#define ACPI_GEBS_CORRECTABLE           (0x1UL << 1)
#define ACPI_GEBS_MULTI_UNCORRECTABLE   (0x1UL << 2)
#define ACPI_GEBS_MULTI_CORRECTABLE     (0x1UL << 3)

/*
 * Total size for Generic Error Status Block except Generic Error Data Entries
 * ACPI 6.2: 18.3.2.7.1 Generic Error Data,
 * Table 18-380 Generic Error Status Block
 */
#define ACPI_GHES_GESB_SIZE                 20

/*
 * Values for error_severity field
 */
enum acpi_generic_error_severity {
    ACPI_CPER_SEV_RECOVERABLE = 0,
    ACPI_CPER_SEV_FATAL = 1,
    ACPI_CPER_SEV_CORRECTED = 2,
    ACPI_CPER_SEV_NONE = 3,
};

typedef struct {
	uint8_t b[16];
} uuid_le;

#define UUID_LE(a, b, c, d0, d1, d2, d3, d4, d5, d6, d7)		\
((uuid_le)							\
{{ (a) & 0xff, ((a) >> 8) & 0xff, ((a) >> 16) & 0xff, ((a) >> 24) & 0xff, \
   (b) & 0xff, ((b) >> 8) & 0xff,					\
   (c) & 0xff, ((c) >> 8) & 0xff,					\
   (d0), (d1), (d2), (d3), (d4), (d5), (d6), (d7) }})

#endif /* __APEI_TABLES_H */
