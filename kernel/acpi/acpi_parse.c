#include "acpi_int.h"

acpi_status_t acpi_parse_aml(uint8_t *aml_start,
			     uint32_t aml_length,
			     struct acpi_namespace_node *start_node)
{
	acpi_status_t status = AE_OK;
	struct acpi_interp interp;

	interp.aml = interp.aml_begin = aml_start;
	interp.aml_end = aml_start + aml_length;

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

	status = acpi_parse_aml(aml_start, aml_length, start_node);
	if (ACPI_FAILURE(status))
		goto err_ref;

err_ref:
	return status;
}

void acpi_unparse_table(struct acpi_table_header *table,
			struct acpi_namespace_node *start_node)
{
}
