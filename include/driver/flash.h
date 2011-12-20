#ifndef __FLASH_DRIVER_H_INCLUDE__
#define __FLASH_DRIVER_H_INCLUDE__

#include <target/config.h>

#ifdef CONFIG_ARCH_HAS_FLASH
#include <asm/mach/flash.h>
#endif

#ifndef ARCH_HAVE_FLASH
typedef uint32_t flash_word_t;
#define FLASH_HW_PAGE_SIZE		0
#define FLASH_HW_WORD_SIZE		4
#define flash_hw_ctrl_init()
#define flash_hw_erase_page(addr)
#define flash_hw_program_words(addr, pdata, size)
#endif

#endif /* __FLASH_DRIVER_H_INCLUDE__ */
