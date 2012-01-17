#ifndef __BOOT_H_INCLUDE__
#define __BOOT_H_INCLUDE__

#include <target/generic.h>
#include <asm/mach/boot.h>

/* TODO: translate DFU configurations to bootloader configurations */

#if defined(CONFIG_DFU_RUNTIME) || defined(CONFIG_DFU_FULLCMD)
#define DFU_RUNTIME		1
/* Following interfaces need to be implemented for bootloader enabled
 * DFU with runtime application.
 */
#ifndef ARCH_HAVE_BOOT
#define board_hw_firmware_boot()
#endif
#else
#undef DFU_RUNTIME
#define board_hw_firmware_boot()
#endif

#if defined(CONFIG_DFU_DFUMODE) || defined(CONFIG_DFU_FULLCMD)
#define DFU_DFUMODE		1
/* Following interfaces need to be implemented for bootloader enabled
 * DFU mode.
 */
#ifndef ARCH_HAVE_BOOT
#define BOARD_HW_FIRMWARE_SIZE			0x8000
#define BOARD_HW_TRANSFER_SIZE			200

#define board_hw_firmware_size()		BOARD_HW_FIRMWARE_SIZE
#define board_hw_firmware_manifest(size)
#define board_hw_firmware_write(n, block, length)
#define board_hw_firmware_read(n, block)
#define board_hw_firmware_appl()
#endif
#else
#undef DFU_DFUMODE
#define BOARD_HW_TRANSFER_SIZE			0
#define board_hw_firmware_appl()
#endif

#endif /* __BOOT_H_INCLUDE__ */
