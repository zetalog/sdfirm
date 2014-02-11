#include "acpi_int.h"

acpi_status_t acpi_parse_once(acpi_interpreter_mode pass_number,
			      uint32_t table_index,
			      struct acpi_namespace_node *start_node)
{
	acpi_status_t status = AE_OK;
#if 0
	uint32_t AmlLength;
	uint8_t *AmlStart;
	struct acpi_table_header *table;
	uint32_t table_length;
	ACPI_PARSE_OBJECT *ParseRoot;
	ACPI_WALK_STATE *walk_state;

#if 0
	status = AcpiTbGetOwnerId(table_index, &owner_id);
	if (ACPI_FAILURE(status))
		return status;
#endif

	ParseRoot = AcpiPsCreateScopeOp();
	if (!ParseRoot)
		return AE_NO_MEMORY;

	WalkState = AcpiDsCreateWalkState(owner_id, NULL, NULL, NULL);
	if (!WalkState) {
		AcpiPsFreeOp(ParseRoot);
		return AE_NO_MEMORY;
	}

	status = AcpiGetTableByIndex(table_index, &table);
	if (ACPI_FAILURE(status)) {
		AcpiDsDeleteWalkState(WalkState);
		AcpiPsFreeOp(ParseRoot);
		return status;
	}

	table_length = ACPI_DECODE32(&table->length);
	if (table_length < sizeof (struct acpi_table_header))
		status = AE_BAD_HEADER;
	else {
		AmlStart = (uint8_t *)table + sizeof (struct acpi_table_header);
		AmlLength = table_length - sizeof (struct acpi_table_header);
		status = AcpiDsInitAmlWalk(WalkState, ParseRoot, NULL,
					   AmlStart, AmlLength,
					   NULL, (uint8_t)pass_number);
	}
	if (ACPI_FAILURE(status)) {
		AcpiDsDeleteWalkState(WalkState);
		goto err_parse;
	}
	if (start_node && start_node != acpi_gbl_root_node) {
		status = AcpiDsScopeStackPush(start_node, ACPI_TYPE_METHOD, WalkState);
		if (ACPI_FAILURE(status)) {
			AcpiDsDeleteWalkState(WalkState);
			goto err_parse;
		}
	}
	status = AcpiPsParseAml(WalkState);

err_parse:
	AcpiPsDeleteParseTree(ParseRoot);
#endif
	return status;
}
