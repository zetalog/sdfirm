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

void acpi_operand_close(struct acpi_operand *operand)
{
	struct acpi_object *object = ACPI_CAST_PTR(struct acpi_object, operand);

	acpi_object_close(object);
	acpi_operand_put(operand, "interp");
}

static acpi_status_t acpi_interpret_open(struct acpi_interp *interp,
					 struct acpi_environ *environ)
{
	uint16_t opcode = environ->opcode;
	const struct acpi_opcode_info *op_info = environ->op_info;
	struct acpi_namespace_node *node;
	struct acpi_term *namearg;
	struct acpi_namespace_node *curr_scope;

	acpi_debug_opcode_info(op_info, "Open:");

	switch (opcode) {
	case AML_SCOPE_OP:
		namearg = acpi_term_get_arg(environ->term, 0);
		if (!namearg || namearg->aml_opcode != AML_NAMESTRING_OP)
			return AE_AML_OPERAND_TYPE;
		node = acpi_space_open(interp->ddb,
				       interp->node,
				       namearg->value.string,
				       namearg->aml_length,
				       ACPI_TYPE_DEVICE, true);
		curr_scope = interp->node;
		interp->node = acpi_node_get(node, "scope");
		acpi_node_put(curr_scope, "scope");
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

static acpi_status_t acpi_interpret_close(struct acpi_interp *interp,
					  struct acpi_environ *environ)
{
	struct acpi_term *namearg;
	struct acpi_term *valuearg;
	struct acpi_term *amlarg;
	uint16_t opcode = environ->opcode;
	const struct acpi_opcode_info *op_info = environ->op_info;
	struct acpi_namespace_node *node;
	struct acpi_namespace_node *curr_scope;
	acpi_status_t status = AE_OK;
	acpi_type_t object_type = ACPI_TYPE_ANY;

	acpi_debug_opcode_info(op_info, "Close:");

	switch (opcode) {
	case AML_SCOPE_OP:
		curr_scope = interp->node;
		interp->node = acpi_node_get(curr_scope->parent, "scope");
		acpi_node_put(curr_scope, "scope");
		break;
	case AML_NAME_OP:
	case AML_METHOD_OP:
		if (opcode == AML_METHOD_OP)
			object_type = ACPI_TYPE_METHOD;
		else {
			/* TODO: obtain the object type from the argument type */
		}
		namearg = acpi_term_get_arg(environ->term, 0);
		if (!namearg || namearg->aml_opcode != AML_NAMESTRING_OP) {
			status = AE_AML_OPERAND_TYPE;
			break;
		}
		node = acpi_space_open(interp->ddb,
				       interp->node,
				       namearg->value.string,
				       namearg->aml_length,
				       object_type, true);
		valuearg = acpi_term_get_arg(environ->term, 1);
		if (opcode == AML_METHOD_OP)
			amlarg = acpi_term_get_arg(environ->term, 2);
		if ((opcode == AML_METHOD_OP &&
		     (!amlarg || !valuearg ||
		      valuearg->aml_opcode != AML_BYTE_PFX ||
		      amlarg->aml_opcode != AML_UNKNOWN_OP))) {
			status = AE_AML_OPERAND_TYPE;
		} else {
			if (opcode == AML_METHOD_OP) {
				struct acpi_method *method;

				method = acpi_method_open(interp->ddb,
							  amlarg->aml_offset,
							  amlarg->aml_length,
							  (uint8_t)valuearg->value.integer);
				node->operand = ACPI_CAST_PTR(struct acpi_operand, method);
			}
		}

		acpi_space_close(node, false);
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
	if (ddb != ACPI_DDB_HANDLE_INVALID)
		acpi_table_increment(ddb);
	interp->ddb = ddb;
	interp->node = acpi_node_get(node, "scope");
	interp->callback = callback;
}

static void __acpi_interpret_exit(struct acpi_interp *interp)
{
	if (interp->ddb != ACPI_DDB_HANDLE_INVALID) {
		acpi_table_decrement(interp->ddb);
		interp->ddb = ACPI_DDB_HANDLE_INVALID;
	}
	if (interp->node) {
		acpi_node_put(interp->node, "scope");
		interp->node = NULL;
	}
}

acpi_status_t acpi_interpret_aml(acpi_ddb_t ddb,
				 uint8_t *aml_begin,
				 uint32_t aml_length,
				 acpi_term_cb callback,
				 struct acpi_namespace_node *start_node)
{
	struct acpi_interp interp;
	struct acpi_term_list *start_term = NULL;
	acpi_status_t status;
	uint8_t *aml_end = aml_begin + aml_length;

	/* AML is a TermList */
	__acpi_interpret_init(&interp, ddb, start_node, callback);

	start_term = acpi_term_alloc_aml(ACPI_ROOT_TAG, aml_begin, aml_end);
	if (!start_term) {
		status = AE_NO_MEMORY;
		goto err_ref;
	}

	status = acpi_parse_aml(&interp, aml_begin, aml_end, start_node, start_term);

err_ref:
	__acpi_interpret_exit(&interp);
	return status;
}

acpi_status_t acpi_parse_table(acpi_ddb_t ddb,
			       struct acpi_table_header *table,
			       struct acpi_namespace_node *start_node)
{
	acpi_status_t status;
	uint32_t aml_length;
	uint8_t *aml_start;

	BUG_ON(table->length < sizeof (struct acpi_table_header));

	aml_start = (uint8_t *)table + sizeof (struct acpi_table_header);
	aml_length = table->length - sizeof (struct acpi_table_header);

	status = acpi_interpret_aml(ddb, aml_start, aml_length,
				    acpi_interpret_exec, start_node);
	if (ACPI_FAILURE(status))
		goto err_ref;

err_ref:
	/* Ignore module level execution failure */
	status = AE_OK;
	if (ACPI_FAILURE(status))
		acpi_unparse_table(ddb, table, start_node);
	return status;
}

static boolean acpi_unparse_table_node(struct acpi_namespace_node *node,
				       void *pddb)
{
	acpi_ddb_t ddb = *(acpi_ddb_t *)pddb;

	if (node->ddb == ddb)
		acpi_space_close_node(node);
	return false;
}

void acpi_unparse_table(acpi_ddb_t ddb,
			struct acpi_table_header *table,
			struct acpi_namespace_node *start_node)
{
	acpi_space_walk_depth_first(NULL, ACPI_TYPE_ANY, 3, NULL,
				    acpi_unparse_table_node,
				    (void *)&ddb);
}
