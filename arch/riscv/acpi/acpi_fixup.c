#include <target/acpi.h>
#include <target/sbi.h>

static char *acpi_vid;
static char *acpi_pid;

#ifndef CONFIG_ACPI_RHCT
extern void acpi_fixup_rhct(void);
#else
#define acpi_fixup_rhct()		do { } while (0)
#endif

char *acpi_get_vid(void)
{
	return acpi_vid;
}

char *acpi_get_pid(void)
{
	return acpi_pid;
}

void acpi_fixups(char *oem, char *oem_table)
{
	acpi_vid = oem;
	acpi_pid = oem_table;
	acpi_fixup_rhct();
}
