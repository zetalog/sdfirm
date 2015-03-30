#include "acpi_int.h"

#define AML_PARSER_GET_ARG_TYPE(parser)				\
	AML_GET_ARG_TYPE(parser->arg_types, parser->arg_index)
#define AML_PARSER_IS_VAR_ARG(parser)				\
	AML_IS_VARTYPE(AML_GET_ARG_TYPE(parser->arg_types,	\
					parser->arg_index))

static void __acpi_parser_init(struct acpi_parser *parser,
			       struct acpi_interp *interp,
			       uint8_t *aml_begin,
			       uint8_t *aml_end,
			       struct acpi_term *term)
{
	struct acpi_environ *environ = &parser->environ;
	int i;

	parser->interp = interp;
	parser->aml_begin = aml_begin;
	parser->aml_end = aml_end;
	parser->nr_arguments = 0;
	for (i = 0; i < AML_MAX_ARGUMENTS; i++)
		parser->arguments[i] = NULL;

	environ->parent_term = term;
}

static void __acpi_parser_exit(struct acpi_object *object)
{
	struct acpi_parser *parser = ACPI_CAST_PTR(struct acpi_parser, object);
	struct acpi_environ *environ = &parser->environ;
	int i;

	/* Nothing need to be freed currently */
	for (i = 0; i < parser->nr_arguments; i++) {
		acpi_operand_close_stacked(parser->arguments[i]);
		parser->arguments[i] = NULL;
	}
	parser->nr_arguments = 0;
}

static struct acpi_parser *acpi_parser_open(struct acpi_interp *interp,
					    uint8_t *aml_begin, uint8_t *aml_end,
					    struct acpi_term *term)
{
	struct acpi_state *state;
	struct acpi_parser *parser = NULL;

	state = acpi_state_open(ACPI_STATE_PARSER,
				sizeof (struct acpi_parser),
				__acpi_parser_exit);
	parser = ACPI_CAST_PTR(struct acpi_parser, state);
	if (parser)
		__acpi_parser_init(parser, interp, aml_begin, aml_end, term);

	return parser;
}

static void acpi_parser_close(struct acpi_parser *parser)
{
	acpi_state_close(ACPI_CAST_PTR(struct acpi_state, parser));
}

struct acpi_parser *acpi_parser_init(struct acpi_interp *interp,
				     uint8_t *aml_begin,
				     uint8_t *aml_end,
				     struct acpi_namespace_node *node,
				     struct acpi_term_list *term_list)
{
	struct acpi_parser *parser;
	struct acpi_environ *environ;

	parser = acpi_parser_open(interp, aml_begin, aml_end, NULL);
	if (parser) {
		environ = &parser->environ;
		environ->term = ACPI_CAST_PTR(struct acpi_term, term_list);
		environ->op_info = acpi_opcode_get_info(AML_AMLCODE_OP);

		parser->aml = parser->aml_begin;
		parser->pkg_begin = NULL;
		parser->pkg_end = parser->aml_end;
		parser->arg_types = environ->op_info->args;

		/*
		 * Initialize the next_opcode, let it to be determined by
		 * the TermList parsing.
		 */
		parser->next_opcode = false;
	}

	return parser;
}

void acpi_parser_exit(struct acpi_parser *parser)
{
	while (parser)
		acpi_parser_pop(parser, &parser);
}

acpi_status_t acpi_parser_push(struct acpi_parser *last_parser,
			       struct acpi_parser **next_parser)
{
	struct acpi_interp *interp = last_parser->interp;
	struct acpi_parser *next_state;
	struct acpi_environ *last_environ = &last_parser->environ;
	uint8_t *aml_end;

	BUG_ON(last_parser != interp->parser);

	if (last_parser->pkg_end < last_parser->aml_end)
		aml_end = last_parser->pkg_end;
	else
		aml_end = last_parser->pkg_end;

	next_state = acpi_parser_open(interp,
				      last_parser->aml, aml_end,
				      last_environ->term);
	if (!next_state) {
		*next_parser = last_parser;
		return AE_NO_MEMORY;
	}

	acpi_state_push(&ACPI_CAST_PTR(struct acpi_state, interp->parser),
			ACPI_CAST_PTR(struct acpi_state, next_state));
	BUG_ON(next_state != interp->parser);

	next_state->aml = last_parser->aml;
	next_state->pkg_end = last_parser->aml_end;

	BUG_ON(next_state->aml > next_state->aml_end);

	BUG_ON(next_state->environ.parent_term != last_environ->term);
	next_state->environ.arg_type = AML_PARSER_GET_ARG_TYPE(last_parser);

	*next_parser = interp->parser;

	return AE_OK;
}

acpi_status_t acpi_parser_pop(struct acpi_parser *last_parser,
			      struct acpi_parser **next_parser)
{
	struct acpi_interp *interp = last_parser->interp;
	struct acpi_state *last_state;
	struct acpi_parser *next_state;
	int i;

	BUG_ON(last_parser != interp->parser);

	last_state = acpi_state_pop(&ACPI_CAST_PTR(struct acpi_state,
				    interp->parser));

	next_state = interp->parser;
	if (next_state) {
		next_state->aml = last_parser->aml;
		if (next_state->aml == next_state->pkg_end)
			next_state->pkg_end = next_state->aml_end;
		/*
		 * Reinitialize the next_opcode, let it to be determined
		 * by the argument parsing.
		 */
		next_state->next_opcode = false;
		BUG_ON(next_state->aml > next_state->aml_end);
		if (interp->result) {
			next_state->arguments[next_state->nr_arguments++] = interp->result;
			interp->result = NULL;
		}
		for (i = 0; i < interp->nr_targets; i++) {
			acpi_operand_put(interp->targets[i], "target");
			interp->targets[i] = NULL;
		}
	}

	/*
	 * NOTE: Closing Parser Right After Popping
	 *
	 * This implies that the APIs are used in the following way:
	 * acpi_parser_init()
	 * * acpi_parser_push()
	 * * acpi_parser_pop()
	 * acpi_parser_exit()
	 * Where "*" indicates that the API can be invoked for multiple
	 * times.
	 */
	acpi_parser_close(last_parser);
	*next_parser = next_state;

	return AE_OK;
}

uint16_t aml_opcode_peek(uint8_t *aml)
{
	uint16_t opcode;

	opcode = (uint16_t)ACPI_DECODE8(aml);
	if (opcode == AML_EXTENDED_OP_PFX)
		opcode = (uint16_t)((opcode << 8) | (uint16_t)ACPI_DECODE8(aml + 1));
	if (acpi_opcode_is_namestring(opcode))
		opcode = AML_NAMESTRING_OP;

	return opcode;
}

uint32_t aml_opcode_size(uint8_t *aml, uint16_t opcode)
{
	BUG_ON(opcode == AML_UNKNOWN_OP);

	/*
	 * NOTE: Recognized a NameString, returning 0 because it will be
	 *       parsed in acpi_term_alloc_name().
	 */
	if (opcode == AML_NAMESTRING_OP)
		return 0;
	return (opcode > 0x00FF) ? 2 : 1;
}

void aml_decode_integer(struct acpi_term *term, uint8_t *aml,
			uint16_t arg_type, uint32_t *value_len)
{
	uint32_t length;
	uint16_t opcode;

	switch (arg_type) {
	case AML_BYTEDATA:
		opcode = AML_BYTE_PFX;
		term->value.integer = (uint64_t)ACPI_DECODE8(aml);
		length = 1;
		break;
	case AML_WORDDATA:
		opcode = AML_WORD_PFX;
		term->value.integer = (uint64_t)ACPI_DECODE16(aml);
		length = 2;
		break;
	case AML_DWORDDATA:
		opcode = AML_DWORD_PFX;
		term->value.integer = (uint64_t)ACPI_DECODE32(aml);
		length = 4;
		break;
	case AML_QWORDDATA:
		opcode = AML_QWORD_PFX;
		term->value.integer = (uint64_t)ACPI_DECODE64(aml);
		length = 8;
		break;
	default:
		BUG();
		break;
	}
	if (value_len)
		*value_len = length;
	term->aml_length += length;
}

void aml_decode_string(struct acpi_term *term, uint8_t *aml, uint32_t *str_len)
{
	uint16_t opcode = AML_STRING_PFX;
	uint32_t length;

	term->value.string = ACPI_CAST_PTR(char, aml);
	length = 0;
	while (aml[length])
		length++;
	length++;
	if (str_len)
		*str_len = length;
	term->aml_length += length;
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

void aml_decode_byte_list(struct acpi_term *term,
			  uint8_t *aml, uint8_t *aml_end,
			  uint32_t *buf_len)
{
	uint8_t length = ACPI_PTR_DIFF(aml_end, aml);

	term->value.buffer.len = length;
	term->value.buffer.ptr = aml;
	term->aml_length += length;
}

void aml_decode_namestring(struct acpi_term *term, uint8_t *aml,
			   acpi_path_len_t *name_len)
{
	uint8_t *begin = aml;
	uint8_t *end = aml;
	acpi_path_len_t length;

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

	term->value.string = ACPI_CAST_PTR(char, begin);
	term->aml_length += length;
}

void aml_decode_computation_data(struct acpi_term *term, uint8_t *aml,
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
	uint16_t arg_type = AML_PARSER_GET_ARG_TYPE(parser);

	return (boolean)((parser->aml >= parser->pkg_end) || !arg_type);
}

static acpi_status_t acpi_parser_consume_arg(struct acpi_parser *parser,
					     struct acpi_term *term,
					     struct acpi_term *arg)
{
	uint32_t length = arg->aml_length;

	if ((parser->aml + length) > parser->pkg_end) {
		acpi_term_free(arg);
		return AE_AML_INCOMPLETE_TERM;
	}
	/* Consume opcode */
	parser->aml += length;
	acpi_term_add_arg(term, arg);

	return AE_OK;
}

static acpi_status_t acpi_parser_begin_term(struct acpi_parser *parser)
{
	acpi_status_t status = AE_OK;
	struct acpi_term *term;
	uint8_t *aml = parser->aml;
	uint16_t opcode;
	uint32_t length;
	struct acpi_environ *environ = &parser->environ;
	uint16_t arg_type;
	struct acpi_super_name *super_name;

	BUG_ON(environ->term);

	opcode = aml_opcode_peek(aml);
	length = aml_opcode_size(aml, opcode);

	/*
	 * TBD: Parser Continuation
	 * Should we allow some bad AML tables and return
	 * AE_CTRL_PARSE_CONTINUE here?
	 */
	if (!acpi_opcode_is_term(opcode))
		return AE_AML_UNKNOWN_TERM;

	if (opcode == AML_NAMESTRING_OP) {
		arg_type = environ->arg_type;
		status = acpi_term_alloc_name(parser, arg_type, aml,
					      ACPI_CAST_PTR(struct acpi_name_string *, &term));
		if (ACPI_FAILURE(status))
			return status;
	} else {
		term = acpi_term_alloc_op(opcode, aml, length);
		if (!term)
			return AE_NO_MEMORY;
	}
	BUG_ON(!term);

#ifdef CONFIG_ACPI_AML_PREFIX_SIMPLE
	aml_decode_computation_data(term, parser->aml, opcode, &length);
#endif

	status = acpi_parser_consume_arg(parser,
					 environ->parent_term, term);
	if (ACPI_FAILURE(status))
		return status;

	environ->opcode = opcode;
	if (term->object_type == ACPI_AML_SUPERNAME) {
		super_name = ACPI_CAST_PTR(struct acpi_super_name, term);
		environ->op_info = super_name->op_info;
	} else
		environ->op_info = acpi_opcode_get_info(environ->opcode);
	environ->term = term;

	parser->arg_types = environ->op_info->args;
	/*
	 * Reinitialize the next_opcode, let it to be determined by the
	 * argument parsing.
	 */
	parser->next_opcode = false;

	return status;
}

static acpi_status_t acpi_parser_end_term(struct acpi_parser *parser,
					  acpi_status_t parser_status)
{
	struct acpi_environ *environ = &parser->environ;
	struct acpi_term *term = environ->term;

	if (!term)
		return parser_status;
	/*
	 * TODO: we may create a result term to replace the deleted one.
	 */
	acpi_term_remove_arg(term);
	acpi_term_free(term);
	environ->term = NULL;

	return AE_OK;
}

acpi_status_t acpi_parser_get_simple_arg(struct acpi_parser *parser,
					 uint16_t arg_type)
{
	struct acpi_term *arg = NULL;
	uint32_t length = 0;
	uint8_t *aml = parser->aml;
	struct acpi_environ *environ = &parser->environ;

	switch (arg_type) {
	case AML_BYTEDATA:
		arg = acpi_term_alloc_op(AML_BYTE_PFX, aml, 0);
		if (!arg)
			return AE_NO_MEMORY;
		aml_decode_computation_data(arg, aml, AML_BYTE_PFX, &length);
		break;
	case AML_WORDDATA:
		arg = acpi_term_alloc_op(AML_WORD_PFX, aml, 0);
		if (!arg)
			return AE_NO_MEMORY;
		aml_decode_computation_data(arg, aml, AML_WORD_PFX, &length);
		break;
	case AML_DWORDDATA:
		arg = acpi_term_alloc_op(AML_DWORD_PFX, aml, 0);
		if (!arg)
			return AE_NO_MEMORY;
		aml_decode_computation_data(arg, aml, AML_DWORD_PFX, &length);
		break;
	case AML_QWORDDATA:
		arg = acpi_term_alloc_op(AML_QWORD_PFX, aml, 0);
		if (!arg)
			return AE_NO_MEMORY;
		aml_decode_computation_data(arg, aml, AML_QWORD_PFX, &length);
		break;
	case AML_ASCIICHARLIST:
		arg = acpi_term_alloc_op(AML_STRING_PFX, aml, 0);
		if (!arg)
			return AE_NO_MEMORY;
		aml_decode_computation_data(arg, aml, AML_STRING_PFX, &length);
		break;
	case AML_BYTELIST:
		arg = acpi_term_alloc_op(AML_BUFFER_OP, aml, 0);
		if (!arg)
			return AE_NO_MEMORY;
		aml_decode_byte_list(arg, aml, parser->pkg_end, &length);
		break;
	}

	/*
	 * The following length consumption is used for
	 * CONFIG_ACPI_AML_PREFIX_SIMPLE=n environment.
	 *
	 * FIXME: We have problem in dealing with byte_list here.
	 */
	return acpi_parser_consume_arg(parser, environ->term, arg);
}

acpi_status_t acpi_parser_get_name_string(struct acpi_parser *parser,
					  uint16_t arg_type)
{
	struct acpi_name_string *name_string;
	uint8_t *aml = parser->aml;
	struct acpi_environ *environ = &parser->environ;
	acpi_status_t status;

	status = acpi_term_alloc_name(parser, arg_type, aml, &name_string);
	if (ACPI_FAILURE(status))
		return status;

	return acpi_parser_consume_arg(parser, environ->term,
				       ACPI_CAST_PTR(struct acpi_term, name_string));
}

acpi_status_t acpi_parser_get_pkg_length(struct acpi_parser *parser)
{
	uint32_t length;
	uint32_t pkg_length;

	pkg_length = aml_decode_pkg_length(parser->aml, &length);
	parser->pkg_begin = NULL;
	parser->pkg_end = parser->aml + pkg_length;

	/* Consume opcode */
	parser->aml += length;

	return AE_OK;
}

acpi_status_t acpi_parser_get_term_list(struct acpi_parser *parser)
{
	struct acpi_term *namearg;
	struct acpi_term_list *term_list;
	acpi_tag_t tag;
	struct acpi_environ *environ = &parser->environ;

	switch (environ->opcode) {
	case AML_WHILE_OP:
	case AML_ELSE_OP:
	case AML_IF_OP:
		/* TODO: find a suitable tag to search conditions */
		tag = ACPI_ROOT_TAG;
		break;
	case AML_METHOD_OP:
	case AML_SCOPE_OP:
		namearg = acpi_term_get_arg(environ->term, 0);
		if (!namearg || namearg->aml_opcode != AML_NAMESTRING_OP)
			return AE_AML_OPERAND_TYPE;
		tag = ACPI_NAME2TAG(namearg->value.string);
		break;
	default:
		tag = ACPI_ROOT_TAG;
		break;
	}

	term_list = acpi_term_alloc_aml(tag, parser->aml, parser->pkg_end);
	if (!term_list)
		return AE_NO_MEMORY;

	return acpi_parser_consume_arg(parser, environ->term,
				       ACPI_CAST_PTR(struct acpi_term, term_list));
}

acpi_status_t acpi_parser_get_argument(struct acpi_parser *parser,
				       uint16_t arg_type)
{
	struct acpi_term *arg = NULL;
	uint16_t opcode = AML_UNKNOWN_OP;
	struct acpi_environ *environ = &parser->environ;
	acpi_status_t status;

	switch (arg_type) {
	case AML_PKGLENGTH:
		return acpi_parser_get_pkg_length(parser);
	case AML_NAMESTRING:
		return acpi_parser_get_name_string(parser, arg_type);
	case AML_SIMPLENAME:
		opcode = aml_opcode_peek(parser->aml);
		if (opcode == AML_NAMESTRING_OP)
			return acpi_parser_get_name_string(parser, arg_type);
		else {
			/*
			 * TODO: LocalObj and ArgObj can be handled
			 *       directly here.
			 */
			if (AML_IS_LOCAL_OR_ARG(opcode))
				parser->next_opcode = true;
		}
		return AE_OK;
	case AML_OBJECT:
		/*
		 * Object can be NameSpaceModifierObj or NamedObj, which
		 * contains opcode.
		 */
	case AML_DATAREFOBJECT:
		/*
		 * DataRefObject can be DataObject, which contains opcode.
		 */
	case AML_OBJECTREFERENCE:
		/*
		 * This actually should be a SuperName.
		 */
		parser->next_opcode = true;
		return AE_OK;
	case AML_TERMLIST:
		/*
		 * Opcodes containing an element list need to be executed
		 * earlier.
		 */
		if (!parser->pkg_begin && parser->interp->callback) {
			parser->pkg_begin = parser->aml;
			status = parser->interp->callback(parser->interp,
							  &parser->environ,
							  ACPI_AML_OPEN);
			if (ACPI_FAILURE(status))
				return status;
		}
		if (parser->aml < parser->pkg_end) {
			/*
			 * TermObj can be a UserTemObj because of
			 * Type2Opcode.
			 */
			if (!environ->parent_term ||
			    (environ->term->aml_opcode != AML_METHOD_OP)) {
				/*
				 * Evaluate the entrance AMLCode and
				 * none Method opcodes.
				 */
				parser->next_opcode = true;
			} else {
				/*
				 * Do not evaluate the non entrance
				 * AMLCode.
				 */
				return acpi_parser_get_term_list(parser);
			}
		}
		return AE_OK;
	default:
		if (AML_IS_SIMPLEDATA(arg_type))
			return acpi_parser_get_simple_arg(parser, arg_type);
		if (AML_IS_VARTYPE(arg_type)) {
			/*
			 * Opcodes containing an element list need to be
			 * executed earlier.
			 */
			if (!parser->pkg_begin && parser->interp->callback) {
				parser->pkg_begin = parser->aml;
				status = parser->interp->callback(parser->interp,
								  &parser->environ,
								  ACPI_AML_OPEN);
				if (ACPI_FAILURE(status))
					return status;
			}
			/*
			 * All variable argument need to be built with a
			 * sub-arguments list.
			 */
			if (parser->aml < parser->pkg_end)
#if 0
				parser->next_opcode = true;
#else
				/*
				 * NYI: Hacking to make parser running for
				 * AML_OBJECT/AML_FIELDELEMENT/AML_PACKAGEELEMENT
				 */
				return acpi_parser_get_term_list(parser);
#endif
			return AE_OK;
		}
		if (AML_IS_TERMARG(arg_type)) {
			/*
			 * TermArg can be a UserTermObj because of
			 * Type2Opcode.
			 */
			/*
			 * TODO: LocalObj and ArgObj can be handled
			 *       directly here.
			 */
			if (AML_IS_LOCAL_OR_ARG(opcode))
				parser->next_opcode = true;
			else
				parser->next_opcode = true;
			return AE_OK;
		}
		if (AML_IS_SUPERNAME(arg_type)) {
			/*
			 * SuperName can be a UserTermObj because of
			 * Type6Opcode.
			 */
			parser->next_opcode = true;
			return AE_OK;
		}
		acpi_err("Invalid argument type: 0x%X", arg_type);
		return AE_AML_OPERAND_TYPE;
	}

	BUG_ON(opcode == AML_UNKNOWN_OP);

	return acpi_parser_consume_arg(parser, environ->term, arg);
}

static acpi_status_t acpi_parser_get_arguments(struct acpi_parser *parser)
{
	acpi_status_t status;
	uint16_t arg_type;

	arg_type = AML_PARSER_GET_ARG_TYPE(parser);
	while (arg_type && !parser->next_opcode) {
		status = acpi_parser_get_argument(parser, arg_type);
		if (ACPI_FAILURE(status))
			return status;
		if (!AML_PARSER_IS_VAR_ARG(parser) || !parser->next_opcode)
			parser->arg_index++;
		arg_type = AML_PARSER_GET_ARG_TYPE(parser);
	}

	return AE_OK;
}

acpi_status_t acpi_parse_aml(struct acpi_interp *interp,
			     uint8_t *aml_begin, uint8_t *aml_end,
			     struct acpi_namespace_node *node,
			     struct acpi_term_list *term_list)
{
	acpi_status_t status = AE_OK;
	struct acpi_parser *parser = interp->parser;

	parser = acpi_parser_init(interp, aml_begin, aml_end, node, term_list);
	if (!parser)
		return AE_NO_MEMORY;
	interp->parser = parser;

	while (parser && ((parser->aml < aml_end) || parser->environ.term)) {
		if (!parser->environ.term) {
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
					break;
				goto next_parser;
			}
		}

		if (AML_PARSER_GET_ARG_TYPE(parser)) {
			status = acpi_parser_get_arguments(parser);
			if (ACPI_FAILURE(status)) {
				status = acpi_parser_end_term(parser, status);
				if (ACPI_FAILURE(status))
					break;
				goto next_parser;
			}
		}

		if (parser->next_opcode) {
			status = acpi_parser_push(parser, &parser);
			if (ACPI_FAILURE(status)) {
				status = acpi_parser_end_term(parser, status);
				if (ACPI_FAILURE(status))
					break;
				goto next_parser;
			}
			continue;
		}

		if (parser->interp->callback)
			status = parser->interp->callback(parser->interp,
							  &parser->environ,
							  ACPI_AML_CLOSE);
		else
			status = AE_OK;

		status = acpi_parser_end_term(parser, status);
		if (ACPI_FAILURE(status))
			break;

next_parser:
		if (acpi_parser_completed(parser))
			acpi_parser_pop(parser, &parser);
	}

	if (parser && ACPI_SUCCESS(status))
		status = AE_AML_INCOMPLETE_TERM;
	acpi_parser_exit(parser);

	return status;
}
