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

typedef void (*acpi_release_cb)(struct acpi_object_header *);

#define ACPI_DESC_TYPE_NAMED			0x01
#define ACPI_DESC_TYPE_TERM			0x02
#define ACPI_DESC_TYPE_STATE			0x03

#define ACPI_OBJECT_HEADER			\
	uint8_t descriptor_type;		\
	boolean closing;			\
	struct acpi_reference reference_count;	\
	acpi_release_cb release;

struct acpi_object_header {
	ACPI_OBJECT_HEADER
};

struct acpi_data {
	struct acpi_object_header common;
	acpi_object_type object_type;
	acpi_size_t size;
	union acpi_value default_val;
	union acpi_value *current_val;
};

struct acpi_namespace_node {
	struct acpi_object_header common;
	acpi_tag_t tag;
	acpi_ddb_t ddb;
	acpi_object_type object_type;
	struct acpi_namespace_node *parent;
	struct list_head children;
	struct list_head sibling;
	union acpi_operand *operand;
};

#define ACPI_AML_TERMOBJ		0x01
#define ACPI_AML_TERMLIST		0x02
#define ACPI_AML_NAMESTRING		0x03
#define ACPI_AML_SIMPLENAME		0x04
#define ACPI_AML_SUPERNAME		0x05

struct acpi_opcode_info {
	char *name;
	uint64_t args;
	uint16_t flags;
};

#define ACPI_TERMOBJ_HEADER			\
	ACPI_OBJECT_HEADER			\
	union acpi_term *parent;		\
	union acpi_term *children;		\
	union acpi_term *next;			\
	uint16_t aml_opcode;			\
	uint8_t *aml_offset;			\
	uint32_t aml_length;			\
	uint16_t object_type;			\
	union acpi_value value;			\
	uint8_t arg_count;

struct acpi_term_header {
	ACPI_TERMOBJ_HEADER
};

struct acpi_term_obj {
	ACPI_TERMOBJ_HEADER
};

struct acpi_term_list {
	ACPI_TERMOBJ_HEADER
};

#define ACPI_NAMESTRING_HEADER			\
	ACPI_TERMOBJ_HEADER			\
	acpi_name_t name;

#define ACPI_SIMPLENAME_HEADER			\
	ACPI_NAMESTRING_HEADER			\
	struct acpi_namespace_node *node;

struct acpi_name_string {
	ACPI_NAMESTRING_HEADER
};

struct acpi_simple_name {
	ACPI_SIMPLENAME_HEADER
};

struct acpi_super_name {
	ACPI_SIMPLENAME_HEADER
	struct acpi_opcode_info *op_info;
};

union acpi_term {
	struct acpi_term_header common;
	struct acpi_term_obj term_obj;
	struct acpi_term_list term_list;
	struct acpi_name_string name_string;
	struct acpi_simple_name simple_name;
	struct acpi_super_name super_name;
};

#define ACPI_STATE_PARSER		0x01

#define ACPI_STATE_HEADER		\
	ACPI_OBJECT_HEADER		\
	uint8_t object_type;		\
	acpi_release_cb release_state;	\
	void *next;

struct acpi_state_header {
	ACPI_STATE_HEADER
};

struct acpi_environ {
	union acpi_term *parent_term;
	union acpi_term *term;
	uint16_t opcode;
	const struct acpi_opcode_info *op_info;
	uint16_t arg_type;
};

struct acpi_parser {
	ACPI_STATE_HEADER
	uint8_t *aml;
	uint8_t *aml_begin;
	uint8_t *aml_end;
	uint8_t *pkg_end;
	uint8_t *aml_last_while;
	boolean next_opcode;
	uint8_t arg_index;
	uint64_t arg_types;

	struct acpi_interp *interp;
	/* Executable domain built by parser, executed by interpreter */
	struct acpi_environ environ;
};

union acpi_state {
	struct acpi_state_header common;
	struct acpi_parser parser;
};

/*
 * When the TermList is about to be parsed, we invoke acpi_term_cb using
 * this type.
 */
#define ACPI_AML_OPEN	0x01
/*
 * When all child TermObj elements are parsed, we invoke acpi_term_cb using
 * this type.
 */
#define ACPI_AML_CLOSE	0x02

typedef
acpi_status_t (*acpi_term_cb)(struct acpi_interp *interp,
			      struct acpi_environ *exec_environ,
			      uint8_t type);

struct acpi_interp {
	/* Parser state */
	struct acpi_parser *parser;
	/* current scope */
	struct acpi_namespace_node *node;

	/* Executer state */
	acpi_ddb_t ddb;
	acpi_term_cb callback;
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

extern struct acpi_table_fadt acpi_gbl_FADT;

/*=========================================================================
 * Name internals
 *=======================================================================*/
acpi_path_len_t acpi_path_split(acpi_path_t *path,
				acpi_path_t *parent, acpi_name_t name);

/*=========================================================================
 * Parser internals
 *=======================================================================*/
struct acpi_parser *acpi_parser_init(struct acpi_interp *interp,
				     uint8_t *aml_begin,
				     uint8_t *aml_end,
				     struct acpi_namespace_node *node,
				     union acpi_term *term);
void acpi_parser_exit(struct acpi_parser *parser);
acpi_status_t acpi_parser_push(struct acpi_parser *last_parser,
			       struct acpi_parser **next_parser);
acpi_status_t acpi_parser_pop(struct acpi_parser *last_parser,
			      struct acpi_parser **next_parser);

union acpi_term *acpi_term_alloc_op(uint16_t opcode,
				    uint8_t *aml, uint32_t length);
union acpi_term *acpi_term_alloc_aml(acpi_tag_t tag,
				     uint8_t *aml_begin,
				     uint8_t *aml_end);
acpi_status_t acpi_term_alloc_name(struct acpi_parser *parser,
				   uint16_t arg_type, uint8_t *aml,
				   union acpi_term **pterm);
void acpi_term_free(union acpi_term *op);
union acpi_term *acpi_term_get_arg(union acpi_term *term, uint32_t argn);
void acpi_term_add_arg(union acpi_term *term, union acpi_term *arg);
void acpi_term_remove_arg(union acpi_term *arg);

acpi_status_t acpi_parse_aml(struct acpi_interp *interp,
			     uint8_t *aml_begin, uint8_t *aml_end,
			     struct acpi_namespace_node *node,
			     union acpi_term *term);

const struct acpi_opcode_info *acpi_opcode_get_info(uint16_t opcode);
struct acpi_opcode_info *acpi_opcode_alloc_info(acpi_name_t name,
						uint8_t argc);
uint8_t acpi_opcode_num_arguments(uint16_t opcode);
uint8_t acpi_opcode_num_targets(uint16_t opcode);
boolean acpi_opcode_match_type(uint16_t opcode, uint16_t arg_type);
boolean acpi_opcode_is_term(uint16_t opcode);
boolean acpi_opcode_is_namestring(uint16_t opcode);

/*=========================================================================
 * Interpreter internals
 *=======================================================================*/
acpi_status_t acpi_interpret_aml(acpi_ddb_t ddb,
				 uint8_t *aml_begin,
				 uint32_t aml_length,
				 acpi_term_cb callback,
				 struct acpi_namespace_node *start_node);
acpi_status_t acpi_interpret_exec(struct acpi_interp *interp,
				  struct acpi_environ *environ,
				  uint8_t type);
acpi_status_t acpi_parse_table(acpi_ddb_t ddb,
			       struct acpi_table_header *table,
			       struct acpi_namespace_node *start_node);
void acpi_unparse_table(acpi_ddb_t ddb,
			struct acpi_table_header *table,
			struct acpi_namespace_node *start_node);

/*=========================================================================
 * Namespace internals
 *=======================================================================*/
struct acpi_namespace_node *acpi_node_open(acpi_ddb_t ddb,
					   struct acpi_namespace_node *parent,
					   acpi_tag_t tag,
					   acpi_object_type object_type);
void acpi_node_close(struct acpi_namespace_node *parent);
struct acpi_namespace_node *acpi_node_lookup(acpi_ddb_t ddb,
					     struct acpi_namespace_node *scope,
					     acpi_tag_t tag,
					     acpi_object_type object_type,
					     boolean create);
void acpi_node_put(struct acpi_namespace_node *node, const char *hint);
struct acpi_namespace_node *acpi_node_get(struct acpi_namespace_node *node,
					  const char *hint);
struct acpi_namespace_node *acpi_node_get_graceful(struct acpi_namespace_node *node,
						   const char *hint);

struct acpi_namespace_node *acpi_space_get_node(acpi_ddb_t ddb,
						struct acpi_namespace_node *scope,
						const char *name, uint32_t length,
						boolean create, const char *hint);

/*=========================================================================
 * Utility internals
 *=======================================================================*/
struct acpi_object_header *acpi_object_open(uint8_t type,
					    acpi_size_t size,
					    acpi_release_cb release);
void acpi_object_close(struct acpi_object_header *object);
boolean acpi_object_is_closing(struct acpi_object_header *object);
void acpi_object_get(struct acpi_object_header *object);
void acpi_object_put(struct acpi_object_header *object);
struct acpi_object_header *acpi_object_get_graceful(struct acpi_object_header *object);

void acpi_state_push(union acpi_state **head, union acpi_state *state);
union acpi_state *acpi_state_pop(union acpi_state **head);
union acpi_state *acpi_state_open(uint8_t type, acpi_size_t size,
				  acpi_release_cb release);
void acpi_state_close(union acpi_state *state);

#endif /* __ACPI_INT_H_INCLUDE__ */
