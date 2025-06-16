
#include <target/acpi.h>
#include <target/sbi.h>

struct acpi_table acpi_rhct_desc;

#define RHCT_NODE_HDR_SIZE	sizeof(struct acpi_rhct_node_header)
#define RHCT_HART_INFO_SIZE	sizeof(struct acpi_rhct_hart_info)

#define rhct_foreach_hart(rhct, node)								\
	for (node = ACPI_ADD_PTR(struct acpi_rhct_node_header, rhct, rhct->node_offset);	\
	     node < ACPI_ADD_PTR(struct acpi_rhct_node_header, rhct, rhct->header.length);	\
	     node = ACPI_ADD_PTR(struct acpi_rhct_node_header, node, node->length))		\
		if (node->type == ACPI_RHCT_NODE_TYPE_HART_INFO)

static struct acpi_table_rhct *acpi_get_rhct(void)
{
	return ACPI_CAST_PTR(struct acpi_table_rhct, acpi_rhct_desc.pointer);
}

static void acpi_rhct_parse_isa(struct acpi_table_rhct *rhct,
				struct acpi_rhct_hart_info *hart_info,
				const char **isa)
{
	struct acpi_rhct_node_header *ref_node;
	struct acpi_rhct_isa_string *isa_node;
	uint32_t *hart_info_node_offset;
	int i;

	*isa = NULL;
	hart_info_node_offset = ACPI_ADD_PTR(uint32_t, hart_info, RHCT_HART_INFO_SIZE);
	for (i = 0; i < hart_info->num_offsets; i++) {
		ref_node = ACPI_ADD_PTR(struct acpi_rhct_node_header,
					rhct, hart_info_node_offset[i]);
		if (ref_node->type == ACPI_RHCT_NODE_TYPE_ISA_STRING) {
			isa_node = ACPI_ADD_PTR(struct acpi_rhct_isa_string,
						ref_node, RHCT_NODE_HDR_SIZE);
			*isa = isa_node->isa;
			return;
		}
	}
}

static void acpi_rhct_parse_cmo(struct acpi_table_rhct *rhct,
				struct acpi_rhct_hart_info *hart_info,
				uint32_t *cbom_size, uint32_t *cboz_size,
				uint32_t *cbop_size)
{
	struct acpi_rhct_node_header *ref_node;
	struct acpi_rhct_cmo_node *cmo_node;
	uint32_t *hart_info_node_offset;
	int i;

	*cbom_size = 0;
	*cboz_size = 0;
	*cbop_size = 0;
	hart_info_node_offset = ACPI_ADD_PTR(uint32_t, hart_info, RHCT_HART_INFO_SIZE);
	for (i = 0; i < hart_info->num_offsets; i++) {
		ref_node = ACPI_ADD_PTR(struct acpi_rhct_node_header,
					rhct, hart_info_node_offset[i]);
		if (ref_node->type == ACPI_RHCT_NODE_TYPE_CMO) {
			cmo_node = ACPI_ADD_PTR(struct acpi_rhct_cmo_node,
						ref_node, RHCT_NODE_HDR_SIZE);
			if (cbom_size && cmo_node->cbom_size <= 30)
				*cbom_size = BIT(cmo_node->cbom_size);
			if (cboz_size && cmo_node->cboz_size <= 30)
				*cboz_size = BIT(cmo_node->cboz_size);
			if (cbop_size && cmo_node->cbop_size <= 30)
				*cbop_size = BIT(cmo_node->cbop_size);
		}
	}
}

void acpi_fixup_rhct(void)
{
	acpi_status_t status;
	struct acpi_rhct_hart_info *hart_info;
	struct acpi_rhct_node_header *node;
	struct acpi_table_rhct *rhct;
	uint32_t cbom, cboz, cbop;
	const char *isa;

	status = acpi_get_table_by_name(ACPI_NAME2TAG(ACPI_SIG_RHCT),
					acpi_get_vid(), acpi_get_pid(),
					&acpi_rhct_desc);
	if (ACPI_FAILURE(status)) {
		sbi_printf("rhct: No RHCT table found!\n");
		return;
	}
	rhct = acpi_get_rhct();
	rhct_foreach_hart(rhct, node) {
		hart_info = ACPI_ADD_PTR(struct acpi_rhct_hart_info,
			node, sizeof(struct acpi_rhct_node_header));

		acpi_rhct_parse_isa(rhct, hart_info, &isa);
		acpi_rhct_parse_cmo(rhct, hart_info, &cbom, &cboz, &cbop);
		sbi_printf("rhct(%d): %s cbom=%d, cboz=%d, cbop=%d\n",
			   hart_info->uid, isa, cbom, cboz, cbop);
	}
}
