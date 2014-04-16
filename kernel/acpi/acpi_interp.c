#include "acpi_int.h"

acpi_status_t acpi_interpret_aml(uint8_t *aml_begin,
				 uint32_t aml_length,
				 struct acpi_namespace_node *start_node)
{
	struct acpi_interp interp;
	union acpi_term *start_term = NULL;
	acpi_status_t status;
	uint8_t *aml_end = aml_begin + aml_length;

	/* AML is a TermList */
	start_term = acpi_term_alloc_TermList(aml_begin, aml_end);
	if (!start_term)
		return AE_NO_MEMORY;

	interp.aml_begin = aml_begin;
	interp.aml_end = aml_end;

	status = acpi_parse_aml(&interp, start_node, start_term);

	return status;
}

acpi_status_t acpi_parse_table(struct acpi_table_header *table,
			       struct acpi_namespace_node *start_node)
{
	acpi_status_t status;
	uint32_t aml_length;
	uint8_t *aml_start;

	BUG_ON(table->length < sizeof (struct acpi_table_header));

	aml_start = (uint8_t *)table + sizeof (struct acpi_table_header);
	aml_length = table->length - sizeof (struct acpi_table_header);

	status = acpi_interpret_aml(aml_start, aml_length, start_node);
	if (ACPI_FAILURE(status))
		goto err_ref;

err_ref:
	return status;
}

void acpi_unparse_table(struct acpi_table_header *table,
			struct acpi_namespace_node *start_node)
{
}
