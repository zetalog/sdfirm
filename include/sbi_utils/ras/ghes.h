/*
 * RISC-V RERI Registers Definitions
 *
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Ventana Micro Systems, Inc.
 *
 * Author(s):
 * Himanshu Chauhan <hchauhan@ventanamicro.com>
 *
 */

#ifndef __ACPI_GHES_H
#define __ACPI_GHES_H

typedef struct {
	uint32_t etype;
	union {
		struct {
			uint32_t  validation_bits;
			uint32_t  sev;
			uint8_t   proc_type;
			uint8_t   proc_isa;
			uint8_t   proc_err_type;
			uint8_t   operation;
			uint8_t   flags;
			uint8_t   level;
			uint64_t  cpu_version;
			uint8_t   cpu_brand_string[128];
			uint64_t  cpu_id;
			uint64_t  target_addr;
			uint64_t  req_ident;
			uint64_t  resp_ident;
			uint64_t  ip;
		} gpe; /* generic processor error */

		struct {
			uint64_t  physical_address;
		} me; /* DRAM Error */
	} info;
} acpi_ghes_error_info;

void acpi_ghes_init(uint64_t addr, uint64_t size);
int acpi_ghes_new_error_source(uint64_t err_src_id, uint64_t sse_v);
void acpi_ghes_record_errors(uint8_t source_id, acpi_ghes_error_info *einfo);
int acpi_ghes_get_num_err_srcs(void);
int acpi_ghes_get_err_srcs_list(uint32_t *src_ids, uint32_t sz);
int acpi_ghes_get_err_src_desc(uint32_t src_id, acpi_ghesv2 *ghes);

#endif
