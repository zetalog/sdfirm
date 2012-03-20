#ifndef __ARCH_AT8XC5122_H_INCLUDE__
#define __ARCH_AT8XC5122_H_INCLUDE__

#include <target/config.h>

#include <asm/mach/clk.h>
#include <asm/mach/mem.h>
#include <asm/mach/pm.h>
#include <asm/mach/irq.h>

/* generic purpose input/output */
#include <target/gpio.h>
/* general purpose timer */
#include <target/gpt.h>
/* serial port */
#include <target/uart.h>
/* keyboard */
#include <target/kbd.h>
/* display */
#include <target/lcd.h>
/* USB device */
#include <target/usb.h>
/* interface device */
#include <target/ifd.h>

#define BOARD_HW_MAX_POWER	100

#endif /* __ARCH_AT8XC5122_H_INCLUDE__ */
