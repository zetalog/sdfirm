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

#define ACPI_TERM_OBJ_HEADER			\
	ACPI_OBJECT_HEADER			\
	union acpi_term *parent;		\
	union acpi_term *sibling;		\
	uint16_t aml_opcode;			\
	uint8_t *aml_offset;			\
	uint8_t nr_args;

/* Opcode object flags */
#define ACPI_NAMED_OBJ		0x01
#define ACPI_TERM_USER		0x02

struct acpi_term_obj {
	ACPI_TERM_OBJ_HEADER
};

struct acpi_user_term {
	ACPI_TERM_OBJ_HEADER
	acpi_tag_t name; /* 4-byte name or zero if no name */
};

struct acpi_named_obj {
	ACPI_TERM_OBJ_HEADER
	acpi_tag_t name; /* 4-byte name or zero if no name */
};

union acpi_term {
	struct acpi_term_obj common;
	struct acpi_user_term user_term;
	struct acpi_named_obj named_obj;
};

struct acpi_opcode_info {
	char *name;
	uint32_t args;
	uint16_t flags;
};

struct acpi_interp {
	uint8_t *aml_begin;	/* first AML byte */
	uint8_t *aml_end;	/* (last + 1) AML byte */
	struct acpi_namespace_node *start_node;
	union acpi_term *start_term;

	/* Walking state */
	uint8_t *aml;		/* current AML byte */
	union acpi_term *parent_term;
	uint16_t opcode;
	const struct acpi_opcode_info *op_info;
	uint8_t arg_count;
	uint8_t arg_index;
};

/*=========================================================================
 * Table internals
 *=======================================================================*/
/* exported for internal table installation */
acpi_status_t acpi_install_table(acpi_addr_t address, acpi_tag_t signature,
				 acpi_table_flags_t flags,
				 boolean override, boolean versioning,
				 acpi_ddb_t *ddb_handle);
acpi_status_t acpi_validate_table(acpi_ddb_t ddb);
/* exported for interfacing with event callbacks */
void acpi_table_notify_existing(void);
/* exported for internal table initialization */
acpi_status_t acpi_rsdp_parse(acpi_addr_t rsdp_address);
acpi_status_t acpi_xsdt_parse(acpi_addr_t xsdt_address, uint32_t table_entry_size);
acpi_status_t acpi_xsdt_verify(acpi_addr_t xsdt_address);
void acpi_fadt_parse(struct acpi_table_header *table);
/* references without mappings */
acpi_ddb_t acpi_table_increment(acpi_ddb_t ddb);
void acpi_table_decrement(acpi_ddb_t ddb);
acpi_status_t acpi_table_increment_validated(acpi_ddb_t ddb,
					     struct acpi_table_header **out_table);
int acpi_compare_name(acpi_name_t name1, acpi_name_t name2);

/*=========================================================================
 * Parser internals
 *=======================================================================*/
union acpi_term *acpi_term_alloc(uint16_t opcode);
void acpi_term_free(union acpi_term *op);

const struct acpi_opcode_info *acpi_opcode_get_info(uint16_t opcode);
acpi_status_t acpi_parse_aml(uint8_t *aml_start,
			     uint32_t aml_length,
			     struct acpi_namespace_node *start_node);
acpi_status_t acpi_parse_table(struct acpi_table_header *table,
			       struct acpi_namespace_node *start_node);
void acpi_unparse_table(struct acpi_table_header *table,
			struct acpi_namespace_node *start_node);

const struct acpi_opcode_info *acpi_opcode_get_info(uint16_t opcode);
boolean acpi_opcode_is_opcode(uint16_t opcode);
boolean acpi_opcode_is_namestring(uint16_t opcode);
boolean acpi_opcode_is_namespacemodifierobj(uint16_t opcode);
boolean acpi_opcode_is_namedobj(uint16_t opcode);
boolean acpi_opcode_is_computationaldata(uint16_t opcode);
boolean acpi_opcode_is_dataobject(uint16_t opcode);
boolean acpi_opcode_is_debugobj(uint16_t opcode);
boolean acpi_opcode_is_argobj(uint16_t opcode);
boolean acpi_opcode_is_localobj(uint16_t opcode);
boolean acpi_opcode_is_userterm(uint16_t opcode);
boolean acpi_opcode_is_type1opcode(uint16_t opcode);
boolean acpi_opcode_is_type2opcode(uint16_t opcode);
boolean acpi_opcode_is_type6opcode(uint16_t opcode);
boolean acpi_opcode_is_simplename(uint16_t opcode);
boolean acpi_opcode_is_termarg(uint16_t opcode);
boolean acpi_opcode_is_termobj(uint16_t opcode);
boolean acpi_opcode_is_supername(uint16_t opcode);

/*=========================================================================
 * Namespace internals
 *=======================================================================*/
struct acpi_namespace_node *acpi_space_get_node(const char *name);
void acpi_space_put_node(struct acpi_namespace_node *node);

#endif /* __ACPI_INT_H_INCLUDE__ */
