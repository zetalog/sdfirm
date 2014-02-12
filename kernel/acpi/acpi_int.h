/*
 * ZETALOG's Personal COPYRIGHT
 *
 * Copyright (c) 2009 - 2014
 *    ZETALOG - "Lv ZHENG".  All rights reserved.
 *    Author: Lv "Zetalog" Zheng
 *    Internet: zetalog@gmail.com
 *
 * This COPYRIGHT used to protect Personal Intelligence Rights.
 * Redistribution and use in source and binary forms with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *    This product includes software developed by the Lv "Zetalog" ZHENG.
 * 3. Neither the name of this software nor the names of its developers may
 *    be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 * 4. Permission of redistribution and/or reuse of souce code partially only
 *    granted to the developer(s) in the companies ZETALOG worked.
 * 5. Any modification of this software should be published to ZETALOG unless
 *    the above copyright notice is no longer declaimed.
 *
 * THIS SOFTWARE IS PROVIDED BY THE ZETALOG AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE ZETALOG OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * @(#)acpi_int.h: ACPI internal interfaces
 * $Id: acpi_int.h,v 1.279 2011-10-19 10:19:18 zhenglv Exp $
 */

#ifndef __ACPI_INT_H_INCLUDE__
#define __ACPI_INT_H_INCLUDE__

#include <target/acpi.h>
#include "acpi_aml.h"

/* Structure for Resource Tag information */
struct acpi_resource_tag {
	uint32_t bit_offset;
	uint32_t bit_length;
};

union acpi_parser_value {
	uint64_t integer; /* Integer constant (Up to 64 bits) */
	uint32_t size;	/* bytelist or field size */
	char *string; /* NULL terminated string */
	uint8_t *buffer; /* buffer or string */
	char *name; /* NULL terminated string */
	union acpi_parse_object *arg; /* arguments and contained ops */
	struct acpi_resource_tag tag; /* Resource descriptor tag info  */
};

#define ACPI_PARSER_COMMON \
	union acpi_opcode *parent; /* Parent op */\
	uint8_t descriptor_type; /* To differentiate various internal objs */\
	union acpi_opcode *sibling; /* Next op */\
	uint8_t flags; /* Type of Op */\
	uint16_t aml_opcode; /* AML opcode */\
	uint32_t aml_offset; /* Offset of declaration in AML */\
	struct acpi_namespace_node *node; /* For use by interpreter */\
	union acpi_parser_value value; /* Value or args associated with the opcode */\
	uint8_t nr_args; /* Number of elements in the arg list */

struct acpi_opcode_common {
	ACPI_PARSER_COMMON
};

struct acpi_opcode_named {
	ACPI_PARSER_COMMON
	acpi_tag_t name; /* 4-byte name or zero if no name */
};

struct acpi_opcode_asl {
	ACPI_PARSER_COMMON
};

union acpi_opcode {
	struct acpi_opcode_common common;
	struct acpi_opcode_named named;
};

struct acpi_parser_state {
    uint8_t *aml_begin; /* First AML byte */
    uint8_t *aml_end; /* (last + 1) AML byte */
    uint8_t *aml; /* Next AML byte */
    union acpi_opcode *start_op; /* Root of parse tree */
    struct acpi_namespace_node *start_node;
    union acpi_parse_object *start_scope;
};

/*=========================================================================
 * Table internals
 *=======================================================================*/
/* exported for internal table installation */
acpi_status_t acpi_table_install(acpi_addr_t address, acpi_tag_t signature,
				 acpi_table_flags_t flags,
				 boolean override, boolean versioning,
				 acpi_ddb_t *ddb_handle);
/* exported for interfacing with event callbacks */
void acpi_table_notify_existing(void);
/* exported for internal table initialization */
acpi_status_t acpi_rsdp_parse(acpi_addr_t rsdp_address);
acpi_status_t acpi_xsdt_parse(acpi_addr_t xsdt_address, uint32_t table_entry_size);
acpi_status_t acpi_xsdt_verify(acpi_addr_t xsdt_address);
void acpi_fadt_parse(struct acpi_table_header *table);

int acpi_compare_name(acpi_name_t name1, acpi_name_t name2);

/*=========================================================================
 * Parser internals
 *=======================================================================*/
union acpi_opcode *acpi_opcode_alloc(uint16_t opcode);
void acpi_opcode_free(union acpi_opcode *op);
const struct acpi_opcode_info *acpi_opcode_get_info(uint16_t opcode);
acpi_status_t acpi_parse_aml(uint8_t *aml_start,
			     uint32_t aml_length,
			     struct acpi_namespace_node *start_node);

#endif /* __ACPI_INT_H_INCLUDE__ */
