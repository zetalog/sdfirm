#include "acpi_int.h"

union acpi_opcode *acpi_parser_create_scope(void)
{
	union acpi_opcode *scope_op;

	scope_op = acpi_opcode_alloc(AML_SCOPE_OP);
	if (!scope_op)
		return (NULL);
	scope_op->named.name = ACPI_ROOT_TAG;

	return scope_op;
}

acpi_status_t acpi_parse_aml(uint8_t *aml_start,
			     uint32_t aml_length,
			     struct acpi_namespace_node *start_node)
{
	acpi_status_t status = AE_OK;
	union acpi_opcode *parser_root = NULL;
	struct acpi_parser_state parser;

	parser_root = acpi_parser_create_scope();
	if (!parser_root)
		return AE_NO_MEMORY;

	parser.aml = parser.aml_begin = aml_start;
	parser.aml_end = aml_start + aml_length;

	return status;
}
