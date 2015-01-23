#include "acpi_int.h"

#define AML_PARSER_GET_ARG_TYPE(parser)				\
	AML_GET_ARG_TYPE(parser->arg_types, parser->arg_index)
#define AML_PARSER_IS_VAR_ARG(parser)				\
	AML_IS_VARTYPE(AML_GET_ARG_TYPE(parser->arg_types,	\
					parser->arg_index - 1))

static void __acpi_parser_init(struct acpi_parser *parser,
			       struct acpi_interp *interp,
			       uint8_t *aml_begin,
			       uint8_t *aml_end,
			       struct acpi_namespace_node *node,
			       union acpi_term *term)
{
	parser->interp = interp;
	parser->aml_begin = aml_begin;
	parser->aml_end = aml_end;
	parser->parent_node = /* acpi_namespace_get( */node/* ) */;
	parser->parent_term = term;
}

static void acpi_parser_release(struct acpi_object_header *object)
{
	/* acpi_namespace_put(parser->parent_node); */
}

static struct acpi_parser *acpi_parser_create(struct acpi_interp *interp,
					      uint8_t *aml_begin,
					      uint8_t *aml_end,
					      struct acpi_namespace_node *node,
					      union acpi_term *term)
{
	union acpi_state *state;
	struct acpi_parser *parser = NULL;

	state = acpi_state_create(ACPI_STATE_PARSER,
				  sizeof (struct acpi_parser),
				  acpi_parser_release);
	parser = ACPI_CAST_PTR(struct acpi_parser, state);
	if (parser)
		__acpi_parser_init(parser, interp, aml_begin, aml_end,
				   node, term);

	return parser;
}

static void acpi_parser_delete(struct acpi_parser *parser)
{
	acpi_object_delete(ACPI_CAST_PTR(struct acpi_object_header, parser));
}

struct acpi_parser *acpi_parser_init(struct acpi_interp *interp,
				     uint8_t *aml_begin,
				     uint8_t *aml_end,
				     struct acpi_namespace_node *node,
				     union acpi_term *term)
{
	struct acpi_parser *parser = acpi_parser_create(interp, aml_begin, aml_end,
							NULL, NULL);
	if (parser) {
		parser->aml = parser->aml_begin;
		parser->pkg_end = parser->aml_end;
		parser->node = node;
		parser->term = term;
		parser->op_info = acpi_opcode_get_info(AML_UNKNOWN_OP);
		parser->arg_types = parser->op_info->args;
		parser->next_opcode = false;
	}

	return parser;
}

void acpi_parser_exit(struct acpi_parser *parser)
{
	acpi_parser_delete(parser);
}

acpi_status_t acpi_parser_push(struct acpi_parser *last_parser,
			       struct acpi_parser **next_parser)
{
	struct acpi_interp *interp = last_parser->interp;
	struct acpi_parser *next_state;
	uint8_t *aml_end;

	BUG_ON(last_parser != interp->parser);

	if (last_parser->pkg_end < last_parser->aml_end)
		aml_end = last_parser->pkg_end;
	else
		aml_end = last_parser->pkg_end;

	next_state = acpi_parser_create(interp,
					last_parser->aml, aml_end,
					last_parser->node, last_parser->term);
	if (!next_state) {
		*next_parser = last_parser;
		return AE_NO_MEMORY;
	}

	acpi_state_push(&ACPI_CAST_PTR(union acpi_state, interp->parser),
			ACPI_CAST_PTR(union acpi_state, next_state));
	BUG_ON(next_state != interp->parser);

	next_state->aml = last_parser->aml;
	next_state->pkg_end = last_parser->aml_end;

	BUG_ON(next_state->aml > next_state->aml_end);

	next_state->parent_node = last_parser->node;
	next_state->parent_term = last_parser->term;

	*next_parser = interp->parser;

	return AE_OK;
}

acpi_status_t acpi_parser_pop(struct acpi_parser *last_parser,
			      struct acpi_parser **next_parser)
{
	struct acpi_interp *interp = last_parser->interp;
	union acpi_state *last_state;
	struct acpi_parser *next_state;

	BUG_ON(last_parser != interp->parser);

	last_state = acpi_state_pop(&ACPI_CAST_PTR(union acpi_state,
				    interp->parser));

	next_state = interp->parser;
	if (next_state) {
		next_state->aml = last_parser->aml;
		if (next_state->aml == next_state->pkg_end)
			next_state->pkg_end = next_state->aml_end;
		next_state->next_opcode = false;
		BUG_ON(next_state->aml > next_state->aml_end);
	}

	acpi_parser_delete(last_parser);
	*next_parser = next_state;

	return AE_OK;
}

uint16_t aml_opcode_peek(uint8_t *aml)
{
	uint16_t opcode;

	opcode = (uint16_t)ACPI_DECODE8(aml);
	if (opcode == AML_EXTENDED_OP_PFX)
		opcode = ACPI_DECODE16(aml);
	if (acpi_opcode_is_namestring(opcode))
		opcode = AML_NAMESTRING_OP;
	else if (!acpi_opcode_is_opcode(opcode))
		opcode = AML_UNKNOWN_OP;

	return opcode;
}

uint32_t aml_opcode_size(uint8_t *aml, uint16_t opcode)
{
	return (opcode > 0x00FF) ? 2 : 1;
}

void aml_decode_integer(union acpi_term *term, uint8_t *aml,
			uint16_t arg_type, uint32_t *value_len)
{
	uint32_t length;
	uint16_t opcode;

	switch (arg_type) {
	case AML_BYTEDATA:
		opcode = AML_BYTE_PFX;
		term->common.value.integer = (uint64_t)ACPI_DECODE8(aml);
		length = 1;
		break;
	case AML_WORDDATA:
		opcode = AML_WORD_PFX;
		term->common.value.integer = (uint64_t)ACPI_DECODE16(aml);
		length = 2;
		break;
	case AML_DWORDDATA:
		opcode = AML_DWORD_PFX;
		term->common.value.integer = (uint64_t)ACPI_DECODE32(aml);
		length = 4;
		break;
	case AML_QWORDDATA:
		opcode = AML_QWORD_PFX;
		term->common.value.integer = (uint64_t)ACPI_DECODE64(aml);
		length = 8;
		break;
	default:
		BUG();
		break;
	}
	if (value_len)
		*value_len = length;
	term->common.aml_length += length;
}

void aml_decode_string(union acpi_term *term, uint8_t *aml, uint32_t *str_len)
{
	uint16_t opcode = AML_STRING_PFX;
	uint32_t length;

	term->common.value.string = ACPI_CAST_PTR(char, aml);
	length = 0;
	while (aml[length])
		length++;
	length++;
	if (str_len)
		*str_len = length;
	term->common.aml_length += length;
}

uint32_t aml_decode_pkg_length(uint8_t *aml, uint32_t *length)
{
	uint32_t pkg_length = 0;
	uint32_t byte_cnt;
	uint8_t zero_mask = 0x3F; /* Default [0:5] */

	/*
	 * Byte 0 bits [6:7] contain the number of additional bytes
	 * used to encode the package length, either 0,1,2, or 3
	 */
	byte_cnt = (ACPI_DECODE8(aml) >> 6);
	if (length)
		*length = byte_cnt + 1;

	/* Get bytes 3, 2, 1 as needed */
	while (byte_cnt) {
		/*
		 * Final bit positions for the package length bytes:
		 *      Byte3->[20:27]
		 *      Byte2->[12:19]
		 *      Byte1->[04:11]
		 *      Byte0->[00:03]
		 */
		pkg_length |= (ACPI_DECODE8(aml+byte_cnt) << ((byte_cnt << 3) - 4));
		zero_mask = 0x0F; /* Use bits [0:3] of byte 0 */
		byte_cnt--;
	}

	/* Byte 0 is a special case, either bits [0:3] or [0:5] are used */
	pkg_length |= (ACPI_DECODE8(aml) & zero_mask);
	return pkg_length;
}

void aml_decode_byte_list(union acpi_term *term,
			  uint8_t *aml, uint8_t *aml_end,
			  uint32_t *buf_len)
{
	uint8_t length = ACPI_PTR_DIFF(aml_end, aml);

	term->common.value.buffer.len = length;
	term->common.value.buffer.ptr = aml;
	term->common.aml_length = length;
}

void aml_decode_namestring(union acpi_term *term, uint8_t *aml,
			   uint32_t *name_len)
{
	uint8_t *begin = aml;
	uint8_t *end = aml;
	uint32_t length;

	while (*end == AML_ROOT_PFX || *end == AML_PARENT_PFX)
		end++;

	switch (*end) {
	case 0:
		if (end == begin)
			begin = NULL;
		end++;
		break;
	case AML_DUAL_NAME_PFX:
		end += 1 + (2 * ACPI_NAME_SIZE);
		break;
	case AML_MULTI_NAME_PFX:
		end += 2 + (*(end + 1) * ACPI_NAME_SIZE);
		break;
	default:
		end += ACPI_NAME_SIZE;
		break;
	}
	length = end - aml;
	if (name_len)
		*name_len = length;

	term->common.value.string = ACPI_CAST_PTR(char, begin);
	term->common.aml_length = length;
}

void aml_decode_computation_data(union acpi_term *term, uint8_t *aml,
				 uint16_t opcode, uint32_t *length)
{
	*length = 0;

	switch (opcode) {
	case AML_BYTE_PFX:
		aml_decode_integer(term, aml, AML_BYTEDATA, length);
		break;
	case AML_WORD_PFX:
		aml_decode_integer(term, aml, AML_WORDDATA, length);
		break;
	case AML_DWORD_PFX:
		aml_decode_integer(term, aml, AML_DWORDDATA, length);
		break;
	case AML_QWORD_PFX:
		aml_decode_integer(term, aml, AML_QWORDDATA, length);
		break;
	case AML_STRING_PFX:
		aml_decode_string(term, aml, length);
		break;
	}
}

boolean acpi_parser_completed(struct acpi_parser *parser)
{
	return (boolean)((parser->aml >= parser->pkg_end ||
			 !AML_PARSER_GET_ARG_TYPE(parser)));
}

union acpi_term *acpi_term_get_arg(union acpi_term *term, uint32_t argn)
{
	union acpi_term *arg = NULL;

	if (!acpi_opcode_num_arguments(term->common.aml_opcode) &&
	    !acpi_opcode_num_targets(term->common.aml_opcode))
		return NULL;

	arg = term->common.children;
	while (arg && argn) {
		argn--;
		arg = arg->common.next;
	}

	return arg;
}

void acpi_term_add_arg(union acpi_term *term, union acpi_term *arg)
{
	union acpi_term *prev_arg;

	if (!term)
		return;

	if (!acpi_opcode_num_arguments(term->common.aml_opcode) &&
	    !acpi_opcode_num_targets(term->common.aml_opcode))
		return;

	if (term->common.children) {
		prev_arg = term->common.children;
		while (prev_arg->common.next)
			prev_arg = prev_arg->common.next;
		prev_arg->common.next = arg;
	} else {
		term->common.children = arg;
	}

	while (arg) {
		arg->common.parent = term;
		arg = arg->common.next;
		term->common.arg_count++;
	}
}

static void acpi_parser_delete_tree(union acpi_term *subtree_root)
{
	union acpi_term *term = subtree_root;
	union acpi_term *next = NULL;
	union acpi_term *parent = NULL;

	while (term) {
		if (term != parent) {
			next = acpi_term_get_arg(term, 0);
			if (next) {
				term = next;
				continue;
			}
		}
		next = term->common.next;
		parent = term->common.parent;
		acpi_term_free(term);
		if (term == subtree_root)
			return;
		if (next)
			term = next;
		else
			term = parent;
	}
}

acpi_status_t acpi_complete_term(struct acpi_parser *parser,
				 union acpi_term *term)
{
	union acpi_term *prev, *next;
	const struct acpi_opcode_info *parent_info;
	acpi_status_t status = AE_OK;

	if (!term)
		return AE_OK;

	if (term->common.parent) {
		prev = term->common.parent->common.children;
		if (!prev)
			goto cleanup;
		parent_info = acpi_opcode_get_info(term->common.parent->common.aml_opcode);
		if (parent_info->flags & AML_NAMESPACE_MODIFIER_OBJ) {
		} else if (parent_info->flags & AML_NAMED_OBJ) {
		} else {
		}

		if (prev == term) {
			term->common.parent->common.children = term->common.next;
		} else while (prev) {
			next = prev->common.next;
			if (next == term) {
				prev->common.next = term->common.next;
				next = NULL;
			}
			prev = next;
		}
	}

cleanup:
	acpi_parser_delete_tree(term);
	return status;
}

static acpi_status_t acpi_parser_begin_term(struct acpi_parser *parser)
{
	acpi_status_t status = AE_OK;
	union acpi_term *term;
	uint8_t *aml = parser->aml;
	uint16_t opcode;
	uint32_t length;

	opcode = aml_opcode_peek(aml);
	length = aml_opcode_size(aml, opcode);
	term = acpi_term_alloc(opcode, aml, length);
	if (!term)
		return AE_NO_MEMORY;

	/* Consume opcode */
	parser->opcode = opcode;
	parser->aml += length;

#ifdef CONFIG_ACPI_AML_PREFIX_SIMPLE
	aml_decode_computation_data(term, parser->aml, opcode, &length);
	/* Consume computational data value */
	parser->aml += length;
#endif

	if (opcode == AML_UNKNOWN_OP)
		return AE_CTRL_PARSE_CONTINUE;

	acpi_term_add_arg(parser->parent_term, term);

	parser->op_info = acpi_opcode_get_info(parser->opcode);
	parser->arg_types = parser->op_info->args;
	parser->next_opcode = false;
	parser->term = term;

	return status;
}

static acpi_status_t acpi_parser_end_term(struct acpi_parser *parser,
					  acpi_status_t parser_status)
{
	acpi_status_t status;
	union acpi_term *term = parser->term;
	struct acpi_interp *interp = parser->interp;

	status = acpi_complete_term(parser, term);
	if (ACPI_FAILURE(status))
		return status;
	parser->term = NULL;
	switch (parser_status) {
	case AE_OK:
		break;
	default:
		do {
			if (term) {
				status = acpi_complete_term(parser, term);
				if (ACPI_FAILURE(status))
					return status;
			}
			acpi_parser_pop(parser, &parser);
		} while (term);
		return status;
	}

	return AE_OK;
}

acpi_status_t acpi_parser_get_simple_arg(struct acpi_parser *parser,
					 uint16_t arg_type)
{
	union acpi_term *arg = NULL;
	uint32_t length = 0;
	uint8_t *aml = parser->aml;

	switch (arg_type) {
	case AML_BYTEDATA:
		arg = acpi_term_alloc(AML_BYTE_PFX, aml, 1);
		if (!arg)
			return AE_NO_MEMORY;
		aml_decode_computation_data(arg, aml, AML_BYTE_PFX, &length);
		break;
	case AML_WORDDATA:
		arg = acpi_term_alloc(AML_WORD_PFX, aml, 1);
		if (!arg)
			return AE_NO_MEMORY;
		aml_decode_computation_data(arg, aml, AML_WORD_PFX, &length);
		break;
	case AML_DWORDDATA:
		arg = acpi_term_alloc(AML_DWORD_PFX, aml, 1);
		if (!arg)
			return AE_NO_MEMORY;
		aml_decode_computation_data(arg, aml, AML_DWORD_PFX, &length);
		break;
	case AML_QWORDDATA:
		arg = acpi_term_alloc(AML_QWORD_PFX, aml, length);
		if (!arg)
			return AE_NO_MEMORY;
		aml_decode_computation_data(arg, aml, AML_QWORD_PFX, &length);
		break;
	case AML_ASCIICHARLIST:
		arg = acpi_term_alloc(AML_STRING_PFX, aml, 1);
		if (!arg)
			return AE_NO_MEMORY;
		aml_decode_computation_data(arg, aml, AML_STRING_PFX, &length);
		break;
	case AML_BYTELIST:
		arg = acpi_term_alloc(AML_BUFFER_OP, aml, 0);
		if (!arg)
			return AE_NO_MEMORY;
		aml_decode_byte_list(arg, aml, parser->pkg_end, &length);
		break;
	}

	/* Consume opcode */
	parser->aml += (arg->common.aml_length + length);
	acpi_term_add_arg(parser->term, arg);

	return AE_OK;
}

acpi_status_t acpi_parser_get_simple_name(struct acpi_parser *parser,
					  uint16_t arg_type)
{
	uint16_t opcode;
	uint32_t length;
	union acpi_term *arg;
	uint8_t *aml = parser->aml;

	opcode = AML_NAMESTRING_OP;
	arg = acpi_term_alloc(opcode, aml, 0);
	if (!arg)
		return AE_NO_MEMORY;
	aml_decode_namestring(arg, aml, &length);

	/* Consume opcode */
	parser->aml += length;
	acpi_term_add_arg(parser->term, arg);

	return AE_OK;
}

acpi_status_t acpi_parser_get_pkg_length(struct acpi_parser *parser)
{
	uint32_t length;
	uint32_t pkg_length;

	pkg_length = aml_decode_pkg_length(parser->aml, &length);
	parser->pkg_end = parser->aml + pkg_length;

	/* Consume opcode */
	parser->aml += length;

	return AE_OK;
}

acpi_status_t acpi_parser_get_term_list(struct acpi_parser *parser)
{
	uint32_t length;
	union acpi_term *namearg;
	union acpi_term *arg;
	acpi_tag_t tag;

	switch (parser->opcode) {
	case AML_WHILE_OP:
	case AML_ELSE_OP:
	case AML_IF_OP:
		/* TODO: find a suitable tag to search conditions */
		tag = ACPI_ROOT_TAG;
		break;
	case AML_METHOD_OP:
	case AML_SCOPE_OP:
		namearg = acpi_term_get_arg(parser->term, 0);
		if (!namearg || namearg->common.aml_opcode != AML_NAMESTRING_OP)
			return AE_AML_OPERAND_TYPE;
		tag = ACPI_NAME2TAG(namearg->common.value.string);
		break;
	default:
		tag = ACPI_ROOT_TAG;
		break;
	}

	arg = acpi_term_alloc_aml(tag, parser->aml, parser->pkg_end);
	if (!arg)
		return AE_NO_MEMORY;
	length = parser->pkg_end - parser->aml;

	/* Consume opcode */
	parser->aml += length;
	acpi_term_add_arg(parser->term, arg);

	return AE_OK;
}

acpi_status_t acpi_parser_get_argument(struct acpi_parser *parser,
				       uint16_t arg_type)
{
	union acpi_term *arg = NULL;
	uint8_t *aml = parser->aml;
	uint16_t opcode = AML_UNKNOWN_OP;
	uint32_t length = 0;

	opcode = aml_opcode_peek(aml);
	if (!acpi_opcode_match_type(opcode, arg_type))
		return AE_AML_BAD_OPCODE;

	switch (arg_type) {
	case AML_BYTEDATA:
	case AML_WORDDATA:
	case AML_DWORDDATA:
	case AML_QWORDDATA:
	case AML_ASCIICHARLIST:
	case AML_BYTELIST:
		return acpi_parser_get_simple_arg(parser, arg_type);
	case AML_PKGLENGTH:
		return acpi_parser_get_pkg_length(parser);
	case AML_NAMESTRING:
		return acpi_parser_get_simple_name(parser, arg_type);
	case AML_SIMPLENAME:
		opcode = aml_opcode_peek(aml);
		if (opcode == AML_NAMESTRING_OP)
			return acpi_parser_get_simple_name(parser, AML_SIMPLENAME);
		else
			parser->next_opcode = true;
		return AE_OK;
	case AML_DATAREFOBJECT:
	case AML_OBJECTREFERENCE:
		/* SuperName */
	case AML_SUPERNAME(ANY):
	case AML_TARGET:
	case AML_DEVICE:
		/* TermArg */
	case AML_TERMARG(ANY):
	case AML_INTEGERARG:
	case AML_BUFFERARG:
	case AML_PACKAGEARG:
	case AML_OBJECTARG:
	case AML_DATA:
	case AML_BUFFSTR:
	case AML_BUFFPKGSTR:
	case AML_BYTEARG:
		parser->next_opcode = true;
		return AE_OK;
	case AML_OBJECTLIST:
	case AML_FIELDLIST:
	case AML_PACKAGELEMENTLIST:
	case AML_TERMARGLIST:
		if (parser->aml < parser->pkg_end)
			parser->next_opcode = true;
		return AE_OK;
	case AML_TERMLIST:
		if (parser->aml < parser->pkg_end) {
			if (!parser->parent_term) {
				/* Execute the start term. */
				parser->next_opcode = true;
			} else {
				/* Do not execute the non start term. */
				return acpi_parser_get_term_list(parser);
			}
		}
		return AE_OK;
	default:
		acpi_err("Invalid argument type: 0x%X", arg_type);
		return AE_AML_OPERAND_TYPE;
	}

	if (opcode == AML_UNKNOWN_OP)
		return AE_CTRL_PARSE_CONTINUE;

	/* Consume opcode */
	parser->aml += arg->common.aml_length;
	acpi_term_add_arg(parser->term, arg);

	return AE_OK;
}

static acpi_status_t acpi_parser_get_arguments(struct acpi_parser *parser)
{
	acpi_status_t status;
	union acpi_term *term = parser->term;
	uint8_t *aml = parser->aml;
	uint16_t opcode = term->common.aml_opcode;
	uint32_t length;

	switch (opcode) {
#ifndef CONFIG_ACPI_AML_PREFIX_SIMPLE
	case AML_BYTE_PFX:
	case AML_WORD_PFX:
	case AML_DWORD_PFX:
	case AML_QWORD_PFX:
	case AML_STRING_PFX:
		aml_decode_computation_data(term, aml, opcode, &length);
		parser->aml += length;
		break;
#endif
	default:
		while (AML_PARSER_GET_ARG_TYPE(parser) && !parser->next_opcode) {
			status = acpi_parser_get_argument(parser,
							  AML_PARSER_GET_ARG_TYPE(parser));
			if (ACPI_FAILURE(status))
				return status;
			parser->arg_index++;
		}
		break;
	}

	return AE_OK;
}

acpi_status_t acpi_parse_aml(struct acpi_interp *interp,
			     struct acpi_namespace_node *node,
			     union acpi_term *term)
{
	acpi_status_t status = AE_OK;
	struct acpi_parser *parser = interp->parser;
	uint8_t *aml_end = interp->aml_end;
	uint16_t curr_arg, last_arg;

	parser = acpi_parser_init(interp,
				  interp->aml_begin, interp->aml_end,
				  node, term);
	if (!parser)
		return AE_NO_MEMORY;
	interp->parser = parser;

	while (parser && ((parser->aml < aml_end) || parser->term)) {
		if (!parser->term) {
			status = acpi_parser_begin_term(parser);
			if (ACPI_FAILURE(status)) {
				if (status == AE_CTRL_PARSE_CONTINUE)
					continue;
				if (status == AE_CTRL_PARSE_PENDING)
					status = AE_OK;
				if (status == AE_CTRL_TERMINATE)
					return status;
				status = acpi_parser_end_term(parser, status);
				if (ACPI_FAILURE(status))
					return status;
				goto next_parser;
			}
		}

		if (AML_PARSER_GET_ARG_TYPE(parser)) {
			status = acpi_parser_get_arguments(parser);
			if (ACPI_FAILURE(status)) {
				status = acpi_parser_end_term(parser, status);
				if (ACPI_FAILURE(status))
					return status;
				goto next_parser;
			}
		}

		if (parser->next_opcode) {
			status = acpi_parser_push(parser, &parser);
			if (ACPI_FAILURE(status)) {
				status = acpi_parser_end_term(parser, status);
				if (ACPI_FAILURE(status))
					return status;
				goto next_parser;
			}
			continue;
		}
#if 0
		status = parser->execute(parser);
#endif
		status = acpi_parser_end_term(parser, status);
		if (ACPI_FAILURE(status))
			return status;

next_parser:
		if (acpi_parser_completed(parser))
			acpi_parser_pop(parser, &parser);

		if (parser) {
			curr_arg = AML_GET_ARG_TYPE(parser->arg_types, parser->arg_index);
			last_arg = AML_GET_ARG_TYPE(parser->arg_types, parser->arg_index - 1);
		}

		if (parser && AML_PARSER_IS_VAR_ARG(parser)) {
			if (parser->aml < parser->pkg_end) {
				parser->arg_index--;
				status = acpi_parser_push(parser, &parser);
				if (ACPI_FAILURE(status)) {
					status = acpi_parser_end_term(parser, status);
					if (ACPI_FAILURE(status))
						return status;
				}
			}
		}
	}

	BUG_ON(parser);

	return status;
}
