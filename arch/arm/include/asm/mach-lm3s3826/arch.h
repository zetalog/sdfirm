#ifndef __ARCH_LM3S3826_H_INCLUDE__
#define __ARCH_LM3S3826_H_INCLUDE__

#include <target/config.h>

#include <asm/mach/mem.h>
#include <asm/mach/cpu.h>
#include <asm/mach/pm.h>
#include <asm/mach/clk.h>
#include <asm/mach/irq.h>

/* general purpose input/output */
#include <target/gpio.h>
/* general purpose timer */
#include <target/gpt.h>
/* serial port */
#include <target/uart.h>
/* USB device */
#include <target/usb.h>
/* flash memory */
#include <target/flash.h>

#define BOARD_HW_MAX_POWER	500

#endif /* __ARCH_LM3S3826_H_INCLUDE__ */
