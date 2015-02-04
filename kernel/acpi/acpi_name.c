#include "acpi_int.h"

int acpi_compare_name(acpi_name_t name1, acpi_name_t name2)
{
	return ACPI_NAME2TAG(name2) - ACPI_NAME2TAG(name2);
}

int acpi_compare_sig_name(acpi_tag_t sig, acpi_name_t name)
{
	return ACPI_NAME2TAG(name) - sig;
}

void aml_decode_last_nameseg(acpi_name_t name, char *pathname, uint32_t length)
{
	char *path = pathname;
	int nr_carats, nr_segs;

	if (!pathname) {
		nr_segs = 0;
		path = "";
	} else {
		if (*path == AML_ROOT_PFX)
			path++;
		else {
			nr_carats = 0;
			while (*path == AML_PARENT_PFX) {
				path++;
				nr_carats++;
			}
		}
		switch (*path) {
		case 0:
			nr_segs = 0;
			break;
		case AML_DUAL_NAME_PFX:
			nr_segs = 2;
			path++;
			break;
		case AML_MULTI_NAME_PFX:
			path++;
			nr_segs = (uint32_t)(uint8_t)*path;
			path++;
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
	        path += ACPI_NAME_SIZE;
        }
	ACPI_NAMECPY(ACPI_NAME2TAG(path), name);
}
