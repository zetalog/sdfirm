#ifndef __ARCH_AT8XC5122_H_INCLUDE__
#define __ARCH_AT8XC5122_H_INCLUDE__

#include <target/config.h>

#include <asm/mach/clk.h>
#include <asm/mach/mem.h>
#include <asm/mach/pm.h>
#include <asm/mach/irq.h>
#include <asm/mach/gpio.h>

/* general purpose timer */
#include <driver/gpt.h>
/* serial port */
#include <driver/uart.h>
/* keyboard */
#include <driver/kbd.h>
/* display */
#include <driver/lcd.h>
/* USB device */
#include <driver/usb.h>
/* interface device */
#include <driver/ifd.h>

#define BOARD_HW_MAX_POWER	100

#endif /* __ARCH_AT8XC5122_H_INCLUDE__ */
