#ifndef __ARCH_AT90SCR100_H_INCLUDE__
#define __ARCH_AT90SCR100_H_INCLUDE__

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
/* serial port */
#include <target/uart.h>

#define BOARD_HW_MAX_POWER	100

#endif /* __ARCH_AT90SCR100_H_INCLUDE__ */
