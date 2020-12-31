#include <target/efi.h>
#include <target/spi.h>
#include <target/mmc.h>

void duowen_gpt_copy(void *ptr, uint32_t addr, uint32_t size)
{
	__unused uint8_t load_sel = imc_load_from();

	if (load_sel == IMC_BOOT_SD)
		duowen_sd_copy(ptr, addr, size);
	if (load_sel == IMC_BOOT_SSI)
		duowen_ssi_flash_copy(ptr, addr, size);
}
