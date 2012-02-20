#ifndef __ARCH_LM3S9B92_H_INCLUDE__
#define __ARCH_LM3S9B92_H_INCLUDE__

#include <target/config.h>

#include <asm/mach/mem.h>
#include <asm/mach/cpu.h>
#include <asm/mach/pm.h>
#include <asm/mach/clk.h>
#include <asm/mach/irq.h>
#include <asm/mach/gpio.h>

/* general purpose timer */
#include <driver/gpt.h>
/* serial port */
#include <driver/uart.h>
/* USB device */
#include <driver/usb.h>
/* flash memory */
#include <driver/flash.h>

#define PN53X_HW_SPI_CHIP	0x11
#define LCD_HW_SPI_CHIP		0x01
#define	SD_HW_SPI_CHIP		0x02
#define BOARD_HW_MAX_POWER	500

#endif /* __ARCH_LM3S9B92_H_INCLUDE__ */
