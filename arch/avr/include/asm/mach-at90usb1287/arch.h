#ifndef __ARCH_AT90USB1287_H_INCLUDE__
#define __ARCH_AT90USB1287_H_INCLUDE__

#include <target/config.h>

#include <asm/mach/clk.h>
#include <asm/mach/mem.h>
#include <asm/mach/pm.h>
#include <asm/mach/irq.h>
#include <asm/mach/gpio.h>

/* watch-dog timer */
#include <target/wdt.h>
/* timestamp counter */
#include <target/tsc.h>
/* general purpose timer */
#include <target/gpt.h>
/* serial port */
#include <target/uart.h>
/* spi bus controller */
#include <target/spi.h>
/* light-emitting device */
#include <target/led.h>
/* usb device/host controller */
#include <target/usb.h>

#define BOARD_HW_MAX_POWER	100

#endif /* __ARCH_AT90USB1287_H_INCLUDE__ */
