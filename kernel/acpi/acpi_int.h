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

struct acpi_namespace_node {
	struct acpi_object common;
	acpi_tag_t tag;
	acpi_ddb_t ddb;
	acpi_type_t object_type;
	struct acpi_namespace_node *parent;
	struct list_head children;
	struct list_head sibling;
	struct acpi_operand *operand;
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

union acpi_value {
	uint64_t integer;
	char *string;
	struct {
		uint8_t len;
		uint8_t *ptr;
	} buffer;
};
#define ACPI_DEFAULT_OPERAND_SIZE	(sizeof (union acpi_value))

struct acpi_term {
	ACPI_OBJECT_HEADER
	struct acpi_term *parent;
	struct acpi_term *children;
	struct acpi_term *next;
	uint16_t aml_opcode;
	uint8_t *aml_offset;
	uint32_t aml_length;
	uint16_t object_type;
	union acpi_value value;
	uint8_t arg_count;
};

struct acpi_term_obj {
	struct acpi_term common;
};

struct acpi_term_list {
	struct acpi_term common;
	acpi_name_t name;
};

struct acpi_name_string {
	struct acpi_term common;
	acpi_name_t name;
};

struct acpi_simple_name {
	struct acpi_term common;
	acpi_name_t name;
	struct acpi_namespace_node *node;
};

struct acpi_super_name {
	struct acpi_term common;
	acpi_name_t name;
	struct acpi_namespace_node *node;
	struct acpi_opcode_info *op_info;
};

#define ACPI_STATE_PARSER		0x01

struct acpi_state {
	struct acpi_object common;
	uint8_t object_type;
	acpi_release_cb release_state;
	void *next;
};

struct acpi_environ {
	struct acpi_term *parent_term;
	struct acpi_term *term;
	uint16_t opcode;
	const struct acpi_opcode_info *op_info;
	uint16_t arg_type;
};

struct acpi_parser {
	struct acpi_state common;
	uint8_t *aml;
	uint8_t *aml_begin;
	uint8_t *aml_end;
	uint8_t *pkg_begin;
	uint8_t *pkg_end;
	uint8_t *aml_last_while;
	boolean next_opcode;
	uint8_t arg_index;
	uint64_t arg_types;

	struct acpi_operand *arguments[AML_MAX_ARGUMENTS];
	uint8_t nr_arguments;

	struct acpi_interp *interp;
	/* Executable domain built by parser, executed by interpreter */
	struct acpi_environ environ;
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

	struct acpi_operand *result;
	struct acpi_operand *targets[AML_MAX_TARGETS];
	uint8_t nr_targets;

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
 * Parser internals
 *=======================================================================*/
struct acpi_parser *acpi_parser_init(struct acpi_interp *interp,
				     acpi_tag_t tag,
				     uint8_t *aml_begin,
				     uint8_t *aml_end,
				     struct acpi_namespace_node *node,
				     uint8_t nr_arguments,
				     struct acpi_operand **arguments);
void acpi_parser_exit(struct acpi_parser *parser);
acpi_status_t acpi_parser_push(struct acpi_parser *last_parser,
			       struct acpi_parser **next_parser);
acpi_status_t acpi_parser_pop(struct acpi_parser *last_parser,
			      struct acpi_parser **next_parser);

struct acpi_term *acpi_term_alloc_op(uint16_t opcode,
				    uint8_t *aml, uint32_t length);
struct acpi_term_list *acpi_term_alloc_aml(acpi_tag_t tag,
					   uint8_t *aml_begin,
					   uint8_t *aml_end);
acpi_status_t acpi_term_alloc_name(struct acpi_parser *parser,
				   uint16_t arg_type, uint8_t *aml,
				   struct acpi_name_string **pterm);
void acpi_term_free(struct acpi_term *op);
struct acpi_term *acpi_term_get_arg(struct acpi_term *term, uint32_t argn);
void acpi_term_add_arg(struct acpi_term *term, struct acpi_term *arg);
void acpi_term_remove_arg(struct acpi_term *arg);

acpi_status_t acpi_parse_aml(struct acpi_interp *interp, acpi_tag_t tag,
			     uint8_t *aml_begin, uint8_t *aml_end,
			     struct acpi_namespace_node *node,
			     uint8_t nr_arguments,
			     struct acpi_operand **arguments);

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
acpi_status_t acpi_interpret_exec(struct acpi_interp *interp,
				  struct acpi_environ *environ,
				  uint8_t type);
acpi_status_t acpi_interpret_table(acpi_ddb_t ddb,
				   struct acpi_table_header *table,
				   struct acpi_namespace_node *start_node);
void acpi_uninterpret_table(acpi_ddb_t ddb,
			    struct acpi_table_header *table,
			    struct acpi_namespace_node *start_node);

struct acpi_operand *acpi_operand_open(acpi_type_t object_type,
				       acpi_size_t size,
				       acpi_release_cb release_operand);
void acpi_operand_close(struct acpi_operand *operand);
struct acpi_method *acpi_method_open(acpi_ddb_t ddb, uint8_t *aml,
				     uint32_t length, uint8_t flags);
void acpi_operand_close_stacked(struct acpi_operand *operand);

/*=========================================================================
 * Namespace internals
 *=======================================================================*/
/*
 * The following functions MUST be invoked with acpi_gbl_space_mutex
 * acquired.
 */
struct acpi_namespace_node *__acpi_node_open(acpi_ddb_t ddb,
					     struct acpi_namespace_node *parent,
					     acpi_tag_t tag,
					     acpi_type_t object_type);
void __acpi_node_close(struct acpi_namespace_node *parent);
struct acpi_namespace_node *__acpi_node_lookup(acpi_ddb_t ddb,
					       struct acpi_namespace_node *scope,
					       acpi_tag_t tag,
					       acpi_type_t object_type,
					       boolean create);
struct acpi_namespace_node *__acpi_node_get_graceful(struct acpi_namespace_node *node,
						     const char *hint);

/*
 * The following functions MAY be invoked without acpi_gbl_space_mutex
 * acquired.
 * Especially, for acpi_node_put(), if it may lead to a destruction of the
 * node, it MUST be invoked without acpi_gbl_space_mutex acquired.
 */
struct acpi_namespace_node *acpi_node_get(struct acpi_namespace_node *node,
					  const char *hint);
void acpi_node_put(struct acpi_namespace_node *node, const char *hint);

struct acpi_namespace_node *acpi_space_get_node(acpi_ddb_t ddb,
						struct acpi_namespace_node *scope,
						const char *name, uint32_t length,
						acpi_type_t object_type,
						boolean create, const char *hint);
void acpi_space_close_node(acpi_handle_t node);

/*=========================================================================
 * Utility internals
 *=======================================================================*/
struct acpi_object *acpi_object_open(uint8_t type, acpi_size_t size,
				     acpi_release_cb release);
void acpi_object_close(struct acpi_object *object);
boolean acpi_object_is_closing(struct acpi_object *object);
void acpi_object_get(struct acpi_object *object);
void acpi_object_put(struct acpi_object *object);
struct acpi_object *acpi_object_get_graceful(struct acpi_object *object);

void acpi_state_push(struct acpi_state **head, struct acpi_state *state);
struct acpi_state *acpi_state_pop(struct acpi_state **head);
struct acpi_state *acpi_state_open(uint8_t type, acpi_size_t size,
				   acpi_release_cb release);
void acpi_state_close(struct acpi_state *state);

#endif /* __ACPI_INT_H_INCLUDE__ */
