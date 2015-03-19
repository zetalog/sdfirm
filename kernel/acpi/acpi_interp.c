#include "acpi_int.h"

acpi_status_t acpi_interpret_exec(struct acpi_interp *interp,
				  struct acpi_environ *environ,
				  uint8_t type)
{
	uint16_t opcode = environ->opcode;
	const struct acpi_opcode_info *op_info = environ->op_info;

	if (type == ACPI_AML_OPEN)
		acpi_debug_opcode_info(op_info, "Open:");
	else
		acpi_debug_opcode_info(op_info, "Close:");

	switch (opcode) {
	case AML_SCOPE_OP:
		break;
	case AML_NAME_OP:
		break;
	case AML_METHOD_OP:
		break;
	}

	return AE_OK;
}

static void __acpi_interpret_init(struct acpi_interp *interp,
				  acpi_ddb_t ddb,
				  struct acpi_namespace_node *node,
				  acpi_term_cb callback)
{
	if (ddb != ACPI_DDB_HANDLE_INVALID)
		acpi_table_increment(ddb);
	interp->ddb = ddb;
	interp->node = acpi_node_get(node, "interp");
	interp->callback = callback;
}

static void __acpi_interpret_exit(struct acpi_interp *interp)
{
	if (interp->ddb != ACPI_DDB_HANDLE_INVALID) {
		acpi_table_decrement(interp->ddb);
		interp->ddb = ACPI_DDB_HANDLE_INVALID;
	}
	if (interp->node) {
		acpi_node_put(interp->node, "interp");
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
	union acpi_term *start_term = NULL;
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
	return status;
}

void acpi_unparse_table(acpi_ddb_t ddb,
			struct acpi_table_header *table,
			struct acpi_namespace_node *start_node)
{
}
