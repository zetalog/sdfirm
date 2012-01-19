#ifndef __ARCH_AT90USB1287_H_INCLUDE__
#define __ARCH_AT90USB1287_H_INCLUDE__

#include <target/config.h>

#include <asm/mach/clk.h>
#include <asm/mach/mem.h>
#include <asm/mach/pm.h>
#include <asm/mach/irq.h>
#include <asm/mach/gpio.h>

/* watch-dog timer */
#include <driver/wdt.h>
/* timestamp counter */
#include <driver/tsc.h>
/* general purpose timer */
#include <driver/gpt.h>
/* serial port */
#include <driver/uart.h>
/* spi bus controller */
#include <driver/spi.h>
/* light-emitting device */
#include <driver/led.h>
/* usb device/host controller */
#include <driver/usb.h>

#endif /* __ARCH_AT90USB1287_H_INCLUDE__ */
