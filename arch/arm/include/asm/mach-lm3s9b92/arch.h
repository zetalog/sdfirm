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

#define BOARD_HW_SPI_PN53X	0x01
#define BOARD_HW_MAX_POWER	500

#endif /* __ARCH_LM3S9B92_H_INCLUDE__ */
