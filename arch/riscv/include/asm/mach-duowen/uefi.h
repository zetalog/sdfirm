#ifndef __UEFI_DUOWEN_H_INCLUDE__
#define __UEFI_DUOWEN_H_INCLUDE__

#include <target/spi.h>
#include <target/mmc.h>

typedef void (*duowen_boot_cb)(void *, uint32_t, uint32_t);

#ifdef CONFIG_UEFI
#define ARCH_HAVE_UEFI		1
#endif

#endif /* __UEFI_DUOWEN_H_INCLUDE__ */
