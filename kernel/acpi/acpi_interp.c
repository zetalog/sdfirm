#include "acpi_int.h"

struct acpi_operand *acpi_operand_get(struct acpi_operand *operand,
				      const char *hint)
{
	if (operand) {
		acpi_dbg("[OP-%p-%d] INC(%s)", operand,
			 operand->common.reference_count.count+1, hint);
	}
	acpi_object_get(ACPI_CAST_PTR(struct acpi_object, operand));
	return operand;
}

void acpi_operand_put(struct acpi_operand *operand, const char *hint)
{
	if (!operand)
		return;
	acpi_dbg("[OP-%p-%d] DEC(%s)", operand,
		 operand->common.reference_count.count-1, hint);
	acpi_object_put(ACPI_CAST_PTR(struct acpi_object, operand));
}

static void __acpi_operand_exit(struct acpi_object *object)
{
	struct acpi_operand *operand =
		ACPI_CAST_PTR(struct acpi_operand, object);

	if (operand->release)
		operand->release(object);
}

struct acpi_operand *acpi_operand_open(acpi_type_t object_type,
				       acpi_size_t size,
				       acpi_release_cb release_operand)
{
	struct acpi_operand *operand;
	struct acpi_object *object;

	BUG_ON(size < sizeof (struct acpi_operand));

	object = acpi_object_open(ACPI_DESC_TYPE_OPERAND, size,
				  __acpi_operand_exit);
	operand = ACPI_CAST_PTR(struct acpi_operand, object);
	if (operand) {
		operand->object_type = object_type;
		operand->release = release_operand;
	}

	return acpi_operand_get(operand, "interp");
}

static void __acpi_method_exit(struct acpi_object *object)
{
	struct acpi_method *method = ACPI_CAST_PTR(struct acpi_method, object);

	if (method)
		acpi_table_decrement(method->ddb);
}

struct acpi_method *acpi_method_open(acpi_ddb_t ddb, uint8_t *aml,
				     uint32_t length, uint8_t flags)
{
	struct acpi_method *method;
	struct acpi_operand *operand;

	operand = acpi_operand_open(ACPI_TYPE_METHOD,
				    sizeof (struct acpi_method),
				    __acpi_method_exit);
	if (!operand)
		return NULL;

	method = ACPI_CAST_PTR(struct acpi_method, operand);
	method->method_flags = flags;
	acpi_table_increment(ddb);
	method->ddb = ddb;
	method->aml_start = aml;
	method->aml_length = length;

	return method;
}

struct acpi_integer *acpi_integer_open(uint64_t value)
{
	struct acpi_integer *integer;
	struct acpi_operand *operand;

	operand = acpi_operand_open(ACPI_TYPE_INTEGER,
				    sizeof (struct acpi_integer),
				    NULL);
	if (!operand)
		return NULL;

	integer = ACPI_CAST_PTR(struct acpi_integer, operand);
	integer->value = value;

	return integer;
}

static void __acpi_string_exit(struct acpi_object *object)
{
	struct acpi_string *string =
		ACPI_CAST_PTR(struct acpi_string, object);

	if (string->value) {
		acpi_os_free(string->value);
		string->value = NULL;
	}
}

struct acpi_string *acpi_string_open(const char *value)
{
	struct acpi_string *string;
	struct acpi_operand *operand;

	operand = acpi_operand_open(ACPI_TYPE_STRING,
				    sizeof (struct acpi_string),
				    __acpi_string_exit);
	if (!operand)
		return NULL;

	string = ACPI_CAST_PTR(struct acpi_string, operand);
	if (value)
		string->value = strdup(value);
	else
		string->value = NULL;

	return string;
}

static void __acpi_buffer_exit(struct acpi_object *object)
{
	struct acpi_buffer *buffer =
		ACPI_CAST_PTR(struct acpi_buffer, object);

	if (buffer->value) {
		acpi_os_free(buffer->value);
		buffer->value = NULL;
	}
}

struct acpi_buffer *acpi_buffer_open(const uint8_t *value, acpi_size_t length)
{
	struct acpi_buffer *buffer;
	struct acpi_operand *operand;

	operand = acpi_operand_open(ACPI_TYPE_BUFFER,
				    sizeof (struct acpi_string),
				    __acpi_buffer_exit);
	if (!operand)
		return NULL;

	buffer = ACPI_CAST_PTR(struct acpi_buffer, operand);
	if (value) {
		buffer->value = acpi_os_allocate(length);
		if (buffer->value) {
			memcpy(buffer->value, value, (size_t)length);
			buffer->length = length;
		}
	} else {
		buffer->value = NULL;
	}

	return buffer;
}

void acpi_operand_close(struct acpi_operand *operand)
{
	struct acpi_object *object = ACPI_CAST_PTR(struct acpi_object, operand);

	acpi_object_close(object);
	if (operand->flags & ACPI_OPERAND_NAMED)
		acpi_operand_put(operand, "named");
	else
		acpi_operand_put(operand, "interp");
}

void acpi_operand_close_stacked(struct acpi_operand *operand)
{
	if (operand) {
		if (operand->flags & ACPI_OPERAND_NAMED)
			acpi_operand_put(operand, "interp");
		else
			acpi_operand_close(operand);
	}
}

static acpi_status_t acpi_interpret_open(struct acpi_interp *interp,
					 struct acpi_environ *environ)
{
	uint16_t opcode = environ->opcode;
	const struct acpi_opcode_info *op_info = environ->op_info;
	struct acpi_namespace_node *node;
	struct acpi_term *namearg;
	acpi_status_t status = AE_OK;

	acpi_debug_opcode_info(op_info, "Open:");

	switch (opcode) {
	case AML_SCOPE_OP:
		namearg = acpi_term_get_arg(environ->term, 0);
		if (!namearg || namearg->aml_opcode != AML_NAMESTRING_OP)
			return AE_AML_OPERAND_TYPE;
		node = acpi_space_open_exist(acpi_interp_scope(interp),
					     namearg->value.string,
					     namearg->aml_length);
		if (!node)
			return AE_NOT_FOUND;
		status = acpi_scope_push(&interp->scope, node);
		acpi_space_close_exist(node);
		break;
	case AML_DEVICE_OP:
		namearg = acpi_term_get_arg(environ->term, 0);
		if (!namearg || namearg->aml_opcode != AML_NAMESTRING_OP)
			return AE_AML_OPERAND_TYPE;
		node = acpi_space_open(interp->ddb,
				       acpi_interp_scope(interp),
				       namearg->value.string,
				       namearg->aml_length,
				       ACPI_TYPE_DEVICE,
				       ACPI_SPACE_OPEN_CREATE);
		if (!node)
			return AE_NO_MEMORY;
		status = acpi_scope_push(&interp->scope, node);
		acpi_space_close(node, false);
		break;
	case AML_IF_OP:
		break;
	case AML_ELSE_OP:
		break;
	case AML_WHILE_OP:
		break;
	}
	return AE_OK;
}

static acpi_status_t acpi_interpret_close_Name(struct acpi_interp *interp,
					       struct acpi_environ *environ)
{
	struct acpi_term *namearg;
	struct acpi_namespace_node *node;
	struct acpi_operand *operand;
	struct acpi_parser *parser = acpi_interp_parser(interp);

	namearg = acpi_term_get_arg(environ->term, 0);
	operand = acpi_operand_get(parser->arguments[0], "interp");
	if (!namearg || !operand || namearg->aml_opcode != AML_NAMESTRING_OP)
		return AE_AML_OPERAND_TYPE;
	node = acpi_space_open(interp->ddb,
			       acpi_interp_scope(interp),
			       namearg->value.string,
			       namearg->aml_length,
			       operand->object_type,
			       ACPI_SPACE_OPEN_CREATE);
	if (!node) {
		acpi_operand_put(operand, "named");
		return AE_NO_MEMORY;
	}
	acpi_space_assign_operand(node, operand);
	acpi_operand_close_stacked(operand);
	acpi_space_close(node, false);
	return AE_OK;
}

static acpi_status_t acpi_interpret_close_Method(struct acpi_interp *interp,
						 struct acpi_environ *environ)
{
	struct acpi_term *namearg;
	struct acpi_term *valuearg;
	struct acpi_term *amlarg;
	struct acpi_namespace_node *node;
	struct acpi_method *method;
	struct acpi_operand *operand;

	namearg = acpi_term_get_arg(environ->term, 0);
	valuearg = acpi_term_get_arg(environ->term, 1);
	amlarg = acpi_term_get_arg(environ->term, 2);
	if (!namearg || !amlarg || !valuearg ||
	    namearg->aml_opcode != AML_NAMESTRING_OP ||
	    valuearg->aml_opcode != AML_BYTE_PFX ||
	    amlarg->aml_opcode != AML_UNKNOWN_OP)
		return AE_AML_OPERAND_TYPE;
	node = acpi_space_open(interp->ddb,
			       acpi_interp_scope(interp),
			       namearg->value.string,
			       namearg->aml_length,
			       ACPI_TYPE_METHOD,
			       ACPI_SPACE_OPEN_CREATE);
	if (!node)
		return AE_NO_MEMORY;
	method = acpi_method_open(interp->ddb,
				  amlarg->aml_offset,
				  amlarg->aml_length,
				  (uint8_t)valuearg->value.integer);
	if (!method) {
		acpi_space_close(node, true);
		return AE_NO_MEMORY;
	}
	operand = ACPI_CAST_PTR(struct acpi_operand, method);
	acpi_space_assign_operand(node, operand);
	acpi_operand_close_stacked(operand);
	acpi_space_close(node, false);
	return AE_OK;
}

static acpi_status_t __acpi_interpret_close_integer(struct acpi_interp *interp,
						    struct acpi_environ *environ,
						    uint64_t integer_value)
{
	struct acpi_integer *integer;
	struct acpi_operand *operand;
	struct acpi_parser *parser = acpi_interp_parser(interp);

	BUG_ON(interp->nr_targets > 0 || interp->result);

	integer = acpi_integer_open(integer_value);
	if (!integer)
		return AE_NO_MEMORY;
	operand = ACPI_CAST_PTR(struct acpi_operand, integer);
	interp->result = operand;
	interp->targets[interp->nr_targets++] = acpi_operand_get(operand, "interp");
	return AE_OK;
}

static acpi_status_t acpi_interpret_close_integer(struct acpi_interp *interp,
						  struct acpi_environ *environ)
{
	struct acpi_term *valuearg;

	valuearg = acpi_term_get_arg(environ->term, 0);
	if (!valuearg)
		return AE_AML_OPERAND_TYPE;
	return __acpi_interpret_close_integer(interp, environ,
					      valuearg->value.integer);
}

static acpi_status_t __acpi_interpret_close_string(struct acpi_interp *interp,
						   struct acpi_environ *environ,
						   const char *string_value)
{
	struct acpi_string *string;
	struct acpi_operand *operand;
	struct acpi_parser *parser = acpi_interp_parser(interp);

	BUG_ON(interp->nr_targets > 0 || interp->result);

	string = acpi_string_open(string_value);
	if (!string)
		return AE_NO_MEMORY;
	operand = ACPI_CAST_PTR(struct acpi_operand, string);
	interp->result = operand;
	interp->targets[interp->nr_targets++] = acpi_operand_get(operand, "interp");
	return AE_OK;
}

static acpi_status_t acpi_interpret_close_string(struct acpi_interp *interp,
						 struct acpi_environ *environ)
{
	struct acpi_term *valuearg;

	valuearg = acpi_term_get_arg(environ->term, 0);
	if (!valuearg)
		return AE_AML_OPERAND_TYPE;
	return __acpi_interpret_close_string(interp, environ,
					     valuearg->value.string);
}

static acpi_status_t __acpi_interpret_close_buffer(struct acpi_interp *interp,
						   struct acpi_environ *environ,
						   const uint8_t *buffer_value,
						   acpi_size_t buffer_size)
{
	struct acpi_buffer *buffer;
	struct acpi_operand *operand;
	struct acpi_parser *parser = acpi_interp_parser(interp);

	BUG_ON(interp->nr_targets > 0 || interp->result);

	buffer = acpi_buffer_open(buffer_value, buffer_size);
	if (!buffer)
		return AE_NO_MEMORY;
	operand = ACPI_CAST_PTR(struct acpi_operand, buffer);
	interp->result = operand;
	interp->targets[interp->nr_targets++] = acpi_operand_get(operand, "interp");
	return AE_OK;
}

static acpi_status_t acpi_interpret_close_buffer(struct acpi_interp *interp,
						 struct acpi_environ *environ)
{
	struct acpi_term *valuearg;

	valuearg = acpi_term_get_arg(environ->term, 0);
	if (!valuearg)
		return AE_AML_OPERAND_TYPE;
	return __acpi_interpret_close_buffer(interp, environ,
					     valuearg->value.buffer.ptr,
					     valuearg->value.buffer.len);
}

static acpi_status_t acpi_interpret_close_Return(struct acpi_interp *interp,
						 struct acpi_environ *environ)
{
	struct acpi_operand *operand;
	struct acpi_parser *parser = acpi_interp_parser(interp);

	operand = acpi_operand_get(parser->arguments[0], "return");
	if (!operand)
		return AE_AML_OPERAND_TYPE;
	interp->result = operand;
	return AE_OK;
}

static acpi_status_t acpi_interpret_close(struct acpi_interp *interp,
					  struct acpi_environ *environ)
{
	uint16_t opcode = environ->opcode;
	const struct acpi_opcode_info *op_info = environ->op_info;
	acpi_status_t status = AE_OK;

	acpi_debug_opcode_info(op_info, "Close:");

	switch (opcode) {
	/* Named objects */
	case AML_SCOPE_OP:
	case AML_DEVICE_OP:
		acpi_scope_pop(&interp->scope);
		break;
	case AML_NAME_OP:
		status = acpi_interpret_close_Name(interp, environ);
		break;
	case AML_METHOD_OP:
		status = acpi_interpret_close_Method(interp, environ);
		break;

	/* Computational data */
	case AML_BYTE_PFX:
	case AML_WORD_PFX:
	case AML_DWORD_PFX:
	case AML_QWORD_PFX:
		status = acpi_interpret_close_integer(interp, environ);
		break;
	case AML_ZERO_OP:
		status = __acpi_interpret_close_integer(interp, environ, 0);
		break;
	case AML_ONE_OP:
		status = __acpi_interpret_close_integer(interp, environ, 1);
		break;
	case AML_ONES_OP:
		status = __acpi_interpret_close_integer(interp, environ, (uint64_t)-1);
		break;
	case AML_REVISION_OP:
		status = __acpi_interpret_close_integer(interp, environ, ACPI_REVISION);
		break;
	case AML_STRING_PFX:
		status = acpi_interpret_close_string(interp, environ);
		break;
	case AML_BUFFER_OP:
		status = acpi_interpret_close_buffer(interp, environ);
		break;

	/* Return values */
	case AML_AMLCODE_OP:
	case AML_RETURN_OP:
		status = acpi_interpret_close_Return(interp, environ);
		break;
	}

	return status;
}

acpi_status_t acpi_interpret_exec(struct acpi_interp *interp,
				  struct acpi_environ *environ,
				  uint8_t type)
{
	if (type == ACPI_AML_OPEN)
		return acpi_interpret_open(interp, environ);
	else
		return acpi_interpret_close(interp, environ);
}

static void __acpi_interpret_init(struct acpi_interp *interp,
				  acpi_ddb_t ddb,
				  struct acpi_namespace_node *node,
				  acpi_term_cb callback)
{
	int i;

	acpi_scope_init(&interp->scope, node);
	if (ddb != ACPI_DDB_HANDLE_INVALID)
		acpi_table_increment(ddb);
	interp->ddb = ddb;
	interp->callback = callback;
	interp->nr_targets = 0;
	for (i = 0; i < AML_MAX_TARGETS; i++)
		interp->targets[i] = NULL;
	interp->result = NULL;
}

static void __acpi_interpret_exit(struct acpi_interp *interp)
{
	int i;

	if (interp->ddb != ACPI_DDB_HANDLE_INVALID) {
		acpi_table_decrement(interp->ddb);
		interp->ddb = ACPI_DDB_HANDLE_INVALID;
	}
	acpi_scope_exit(&interp->scope);
	for (i = 0; i < interp->nr_targets; i++) {
		if (interp->targets[i]) {
			acpi_operand_close_stacked(interp->targets[i]);
			interp->targets[i] = NULL;
		}
	}
	if (interp->result) {
		acpi_operand_close_stacked(interp->result);
		interp->result = NULL;
	}
}

acpi_status_t acpi_interpret_aml(acpi_ddb_t ddb, acpi_tag_t tag,
				 uint8_t *aml_begin,
				 uint32_t aml_length,
				 uint8_t nr_arguments,
				 struct acpi_operand **arguments,
				 acpi_term_cb callback,
				 struct acpi_namespace_node *start_node,
				 struct acpi_operand **result)
{
	struct acpi_interp interp;
	struct acpi_term_list *start_term = NULL;
	acpi_status_t status;
	uint8_t *aml_end = aml_begin + aml_length;

	__acpi_interpret_init(&interp, ddb, start_node, callback);
	status = acpi_parse_aml(&interp, tag, aml_begin, aml_end,
				start_node,
				nr_arguments, arguments);
	if (ACPI_SUCCESS(status) && result)
		*result = acpi_operand_get(interp.result, "interp");
	__acpi_interpret_exit(&interp);

	return status;
}

acpi_status_t acpi_interpret_table(acpi_ddb_t ddb,
				   struct acpi_table_header *table,
				   struct acpi_namespace_node *start_node)
{
	acpi_status_t status;
	uint32_t aml_length;
	uint8_t *aml_start;

	BUG_ON(table->length < sizeof (struct acpi_table_header));

	aml_start = (uint8_t *)table + sizeof (struct acpi_table_header);
	aml_length = table->length - sizeof (struct acpi_table_header);

	status = acpi_interpret_aml(ddb, ACPI_ROOT_TAG,
				    aml_start, aml_length,
				    0, NULL,
				    acpi_interpret_exec,
				    start_node, NULL);
	if (ACPI_FAILURE(status))
		goto err_ref;

err_ref:
	/* Ignore module level execution failure */
	status = AE_OK;
	if (ACPI_FAILURE(status))
		acpi_uninterpret_table(ddb, table, start_node);
	return status;
}

static boolean acpi_uninterpret_table_node(struct acpi_namespace_node *node,
					   void *pddb)
{
	acpi_ddb_t ddb = *(acpi_ddb_t *)pddb;

	if (node->ddb == ddb)
		acpi_space_close_node(node);
	return false;
}

void acpi_uninterpret_table(acpi_ddb_t ddb,
			    struct acpi_table_header *table,
			    struct acpi_namespace_node *start_node)
{
	acpi_space_walk_depth_first(NULL, ACPI_TYPE_ANY, 3, NULL,
				    acpi_uninterpret_table_node,
				    (void *)&ddb);
}

acpi_status_t acpi_evaluate_object(acpi_handle_t handle, char *name,
				   uint8_t nr_arguments,
				   struct acpi_operand **arguments,
				   struct acpi_operand **result)
{
	acpi_status_t status = AE_OK;
	struct acpi_namespace_node *scope = NULL;
	struct acpi_namespace_node *node = NULL;
	acpi_path_len_t len;
	acpi_path_t path = { 0, NULL };
	struct acpi_operand *return_value = NULL;
	struct acpi_operand *operand = NULL;
	struct acpi_method *method = NULL;

	len = acpi_path_encode_alloc(name, &path);
	if (len == 0) {
		status = AE_BAD_PARAMETER;
		goto err_exit;
	}
	if (!path.names) {
		status = AE_NO_MEMORY;
		goto err_exit;
	}

	node = acpi_space_open_exist(handle, path.names, path.length);
	if (!node) {
		status = AE_NOT_EXIST;
		goto err_exit;
	}

	operand = acpi_operand_get(node->operand, "evaluate");
	if (node->object_type != ACPI_TYPE_METHOD) {
		return_value = operand;
		operand = NULL;
		goto err_exit;
	}

	method = ACPI_CAST_PTR(struct acpi_method, operand);
	scope = acpi_node_get(node->parent, "evaluate");

	if (!method || !scope) {
		status = AE_NOT_EXIST;
		goto err_exit;
	}

	status = acpi_interpret_aml(ACPI_DDB_HANDLE_INVALID, node->tag,
				    method->aml_start,
				    method->aml_length,
				    nr_arguments, arguments,
				    acpi_interpret_exec,
				    scope, &return_value);

err_exit:
	if (result && return_value)
		*result = return_value;
	else
		acpi_operand_close_stacked(operand);
	if (method)
		acpi_operand_put(operand, "evaluate");
	if (node)
		acpi_space_close_exist(node);
	if (scope)
		acpi_node_put(scope, "evaluate");
	if (path.names)
		acpi_os_free(path.names);
	return status;
}
