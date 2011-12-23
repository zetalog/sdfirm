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

#define board_hw_firmware_boot()

#endif /* __BOOT_ATC8X5122_H_INCLUDE__ */
