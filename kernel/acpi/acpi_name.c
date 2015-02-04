#include "acpi_int.h"

int acpi_compare_name(acpi_name_t name1, acpi_name_t name2)
{
	return ACPI_NAME2TAG(name2) - ACPI_NAME2TAG(name2);
}

int acpi_compare_sig_name(acpi_tag_t sig, acpi_name_t name)
{
	return ACPI_NAME2TAG(name) - sig;
}

void acpi_path_split(acpi_path_t path, acpi_path_t *parent, acpi_name_t name)
{
	char *iter = path.names;
	int nr_carats, nr_segs;

	if (!path.names) {
		nr_segs = 0;
		iter = "";
	} else {
		if (*iter == AML_ROOT_PFX)
			iter++;
		else {
			nr_carats = 0;
			while (*iter == AML_PARENT_PFX) {
				iter++;
				nr_carats++;
			}
		}
		switch (*iter) {
		case 0:
			nr_segs = 0;
			break;
		case AML_DUAL_NAME_PFX:
			nr_segs = 2;
			iter++;
			break;
		case AML_MULTI_NAME_PFX:
			iter++;
			nr_segs = (uint32_t)(uint8_t)*iter;
			iter++;
			break;
		default:
			nr_segs = 1;
			break;
		}
	}
	while (nr_segs) {
		if (nr_segs == 1)
			break;
		nr_segs--;
	        iter += ACPI_NAME_SIZE;
        }
	if (name)
		ACPI_NAMECPY(ACPI_NAME2TAG(iter), name);
	if (parent) {
		parent->names = path.names;
		parent->length = iter - path.names;
	}
}
