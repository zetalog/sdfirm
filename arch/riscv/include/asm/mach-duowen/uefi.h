#ifndef __UEFI_DUOWEN_H_INCLUDE__
#define __UEFI_DUOWEN_H_INCLUDE__

#include <target/spi.h>

#ifdef CONFIG_UEFI
#define ARCH_HAVE_UEFI		1

#define uefi_hw_gpt_copy(ptr, addr, size)	\
	duowen_ssi_flash_copy(ptr, addr, size)
#endif

#endif /* __UEFI_DUOWEN_H_INCLUDE__ */
