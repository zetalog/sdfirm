#include <target/efi.h>

__efi efi_system_table_t efi_core_st = {
	.hdr = {
		.signature = EFI_SYSTEM_TABLE_SIGNATURE,
		.revision = EFI_2_00_SYSTEM_TABLE_REVISION,
		.headersize = sizeof(efi_core_st),
		.crc32 = 0,
		.reserved = 0,
	},
};

void uefi_dxe_init(void)
{
}
