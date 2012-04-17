#ifndef __KBD_LM3S3826_H_INCLUDE__
#define __KBD_LM3S3826_H_INCLUDE__

#include <target/config.h>
#include <target/generic.h>

#include <asm/reg.h>
#include <asm/mach/gpio.h>
#include <asm/mach/irq.h>

#ifndef ARCH_HAVE_KBD
#define ARCH_HAVE_KBD			1
#else
#error "Multiple KBD controller defined"
#endif

#define __KBD_HW_MAX_SCANS		4
#define __kbd_hw_read_port(port, mask)		\
	gpio_hw_read_port(port, mask)
#define __kbd_hw_port_irq(n)		(IRQ_GPIOA + n)

void kbd_hw_ctrl_init(void);
void kbd_hw_scan_sync(uint8_t sec);


#endif /* __KBD_LM3S3826_H_INCLUDE__ */
