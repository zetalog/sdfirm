#include <target/acpi.h>

int acpi_compare_name(acpi_name_t name1, acpi_name_t name2)
{
	return ACPI_NAME2TAG(name2) - ACPI_NAME2TAG(name2);
}

int acpi_compare_sig_name(acpi_tag_t sig, acpi_name_t name)
{
	return ACPI_NAME2TAG(name) - sig;
}

uint8_t acpi_checksum_calc(uint8_t *buffer, uint32_t length)
{
	uint8_t sum = 0;

	while (length--) {
		sum += ACPI_DECODE8(buffer);
		buffer++;
	}

	return -sum;
}

void acpi_encode_generic_address(struct acpi_generic_address *generic_address,
				 uint8_t space_id,
				 uint64_t address64,
				 uint16_t bit_width)
{
	ACPI_ENCODE64(&generic_address->address, address64);
	ACPI_ENCODE8(&generic_address->space_id, space_id);
	ACPI_ENCODE8(&generic_address->bit_width,
		     (uint8_t)(bit_width > 255 ? (255) : bit_width));
	ACPI_ENCODE8(&generic_address->bit_offset, 0);
	ACPI_ENCODE8(&generic_address->access_width, 0); /* Access width ANY */
}
