#ifndef __UEFI_DUOWEN_H_INCLUDE__
#define __UEFI_DUOWEN_H_INCLUDE__

#include <target/spi.h>
#include <target/mmc.h>

typedef void (*duowen_boot_cb)(void *, uint32_t, uint32_t);

#ifdef CONFIG_UEFI
#define ARCH_HAVE_UEFI		1

#define uefi_hw_gpt_copy(ptr, addr, size)	\
	duowen_gpt_copy(ptr, addr, size)

void duowen_gpt_copy(void *ptr, uint32_t addr, uint32_t size);
#else
#define uefi_hw_gpt_copy(ptr, addr, size)	do { } while (0)
#endif

#endif /* __UEFI_DUOWEN_H_INCLUDE__ */
