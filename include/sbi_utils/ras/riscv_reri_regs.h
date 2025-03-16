/*
 * RISC-V RERI Registers Definitions
 *
 * Copyright (c) 2024 Ventana Micro Systems, Inc.
 *
 * Author(s):
 * Himanshu Chauhan <hchauhan@ventanamicro.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 or
 * (at your option) any later version.
 */

#ifndef __RISCV_RERI_REGS_
#define __RISCV_RERI_REGS_

#define MAX_ERROR_RECORDS       63

/* RERI Error Codes ras-reri-v0.3 (Section 2.7) */
enum {
	RERI_EC_NONE,
	RERI_EC_OUE, /* Other unspecified error */
	RERI_EC_CDA, /* Corrupted data access */
	RERI_EC_CBA, /* Cache block data error */
	RERI_EC_CSD, /* Cache scrubbing detected */
	RERI_EC_CAS, /* Cache address/state error */
	RERI_EC_CUE, /* Cache unspecified error */
	RERI_EC_SDC, /* Snoop-filter/directory address/ control state */
	RERI_EC_SUE, /* Snoop-filter/directory unspecified error */
	RERI_EC_TPD, /* TLB/Page-walk cache data */
	RERI_EC_TPA, /* TLB/Page-walk address control state */
	RERI_EC_TPU, /* TLB/Page-walk unknown error */
	RERI_EC_HSE, /* Hart state error */
	RERI_EC_ICS, /* Interrupt controller state */
	RERI_EC_ITD, /* Interconnect data error */
	RERI_EC_ITO, /* Interconnection other error */
	RERI_EC_IWE, /* Internal watchdog error */
	RERI_EC_IDE, /* Internal datapath/memory or execution unit error */
	RERI_EC_SBE, /* System memory command or address bus error */
	RERI_EC_SMU, /* System memory unspecified error */
	RERI_EC_SMD, /* System memory data error */
	RERI_EC_SMS, /* System memory scrubbing detected error */
	RERI_EC_PIO, /* Protocol error illegal IO */
	RERI_EC_PUS, /* Protocol error unexpected state */
	RERI_EC_PTO, /* Protocol error timeout error */
	RERI_EC_SIC, /* System internal controller error */
	RERI_EC_DPU, /* Deferred error passthrough not supported */
	RERI_EC_PCX, /* PCI/CXL detected error */
	RERI_EC_RES, /* Reserved errors start */
	RERI_EC_REE = 63, /* Reserved errors end */
	RERI_EC_CES = 64, /* Custom error start */
	RERI_EC_CEE = 255, /* Custom error end */
	RERI_EC_INVALID,
};

enum {
	RERI_TT_UNSPECIFIED,
	RERI_TT_CUSTOM,
	RERI_TT_RES1,
	RERI_TT_RES2,
	RERI_TT_EXPLICIT_READ,
	RERI_TT_EXPLICIT_WRITE,
	RERI_TT_IMPLICIT_READ,
	RERI_TT_IMPLICIT_WRITE,
	RERI_TT_INVALID,
};

typedef union riscv_reri_control {
	struct __attribute__((__packed__)) {
		uint16_t ele:1;
		uint16_t cece:1;
		uint16_t sinv:1;
		uint16_t rsvd0:1;
		uint16_t ces:2;
		uint16_t udes:2;
		uint16_t uues:2;
		uint16_t rsvd1:6;

		uint16_t rsvd2;

		uint16_t eid;

		uint16_t rsvd:8;
		uint16_t cust:8;
	};
	uint64_t value;
} riscv_reri_control;

#define RERI_CTRL_MASK 0xFFFF000001FDull

typedef union riscv_reri_status {
	struct __attribute__((__packed__)) {
		uint16_t v:1;
		uint16_t ce:1;
		uint16_t de:1;
		uint16_t ue:1;
		uint16_t pri:2;
		uint16_t mo:1;
		uint16_t c:1;
		uint16_t tt:3;
		uint16_t iv:1;
		uint16_t at:4;

		uint16_t siv:1;
		uint16_t tsv:1;
		uint16_t rsvd0:2;
		uint16_t scrub:1;
		uint16_t ceco:1;
		uint16_t rsvd1:2;
		uint16_t ec:8;

		uint16_t rsvd2;

		uint16_t cec:16;
	};
	uint64_t value;
} riscv_reri_status;

#define RERI_STS_MASK 0x7800FF3FFFFEull

typedef struct __packed riscv_reri_error_record {
	riscv_reri_control control_i;
	riscv_reri_status status_i;
	uint64_t addr_i;
	uint64_t info_i;
	uint64_t suppl_info_i;
	uint64_t timestamp_i;
	uint64_t reserved;
	uint64_t custom;
} riscv_reri_error_record;

typedef union riscv_reri_bank_info {
	struct __packed {
		uint16_t inst_id;
		uint16_t n_err_recs;
		uint64_t reserved0:24;
		uint8_t version:8;
	};
	uint64_t value;
} riscv_reri_bank_info;

typedef union riscv_reri_vendor_imp_id {
	struct __packed {
		uint32_t vendor_id;
		uint16_t imp_id;
		uint16_t reserved;
	};
	uint64_t value;
} riscv_reri_vendor_imp_id;

typedef struct __packed riscv_reri_error_bank {
	riscv_reri_vendor_imp_id vendor_n_imp_id;
	riscv_reri_bank_info bank_info;
	uint64_t valid_summary;
	uint64_t reserved[2];
	uint64_t custom[3];
	riscv_reri_error_record records[MAX_ERROR_RECORDS];
} riscv_reri_error_bank;

#endif /* __RISCV_RERI_REGS_ */
