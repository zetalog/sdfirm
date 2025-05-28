#ifndef __TARGET_ECC_SRAM_H_INCLUDE__
#define __TARGET_ECC_SRAM_H_INCLUDE__

#include <target/generic.h>
#include <asm/mach/ecc_sram.h>

#ifdef CONFIG_ECC_SRAM_RAS
void ecc_sram_init(void);
#else
#define ecc_sram_init() do { } while (0)
#endif

#endif /* __TARGET_ECC_SRAM_H_INCLUDE__ */
