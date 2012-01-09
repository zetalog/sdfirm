#ifndef __BOOT_ATC8X5122_H_INCLUDE__
#define __BOOT_ATC8X5122_H_INCLUDE__

#include <target/config.h>
#include <target/generic.h>

#include <asm/reg.h>
#include <asm/mach/gpio.h>

#ifndef ARCH_HAVE_BOOT
#define ARCH_HAVE_BOOT			1
#else
#error "Multiple bootloader defined"
#endif

/* PLLDIV config byte */
#define BOOT_HW_PLLDIV_CFG	0x7FFB
/* PLL/XTAL config byte */
#define BOOT_HW_PLLXTAL_CFG	0x7FFC
/* software identification byte */
#define BOOT_HW_SOFTWARE_ID	0x7FFD
#define BOOT_HW_CODE_LENGTH	0x7FFE
#define BOOT_HW_USB_ID		0x7F00
#define CRAM_SIZE		0x7F00

#ifdef CONFIG_DSU_AT8XC5122
uint8_t boot_hw_cram_readb(text_byte_t *addr);
uint16_t boot_hw_cram_readw(text_word_t *addr);
uint32_t boot_hw_cram_readl(text_quad_t *addr);
void boot_hw_cram_writeb(text_byte_t *addr, uint8_t val);
uint16_t boot_hw_get_id(void);
#else
#define boot_hw_cram_readb(addr)	0
#define boot_hw_cram_readw(addr)	0
#define boot_hw_cram_readl(addr)	0
#define boot_hw_cram_writeb(addr, byte)
#define boot_hw_get_id()		0
#endif

#define board_hw_firmware_boot()

#endif /* __BOOT_ATC8X5122_H_INCLUDE__ */
