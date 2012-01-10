#ifndef __BOOT_AT90USB1287_H_INCLUDE__
#define __BOOT_AT90USB1287_H_INCLUDE__

#include <target/config.h>
#include <target/generic.h>

#include <asm/reg.h>
#include <asm/mach/gpio.h>

#ifndef ARCH_HAVE_BOOT
#define ARCH_HAVE_BOOT			1
#else
#error "Multiple bootloader defined"
#endif

#define BOARD_HW_FIRMWARE_SIZE			0x8000
#define BOARD_HW_TRANSFER_SIZE			0x40

#define board_hw_firmware_size()		BOARD_HW_FIRMWARE_SIZE
#define board_hw_firmware_manifest(size)
#define board_hw_firmware_write(n, block, length)
#define board_hw_firmware_read(n, block)
#define board_hw_firmware_appl()
#define board_hw_firmware_boot()

#endif /* __BOOT_AT90USB1287_H_INCLUDE__ */
