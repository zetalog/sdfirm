#ifndef __VIC_DRIVER_H_INCLUDE__
#define __VIC_DRIVER_H_INCLUDE__

#include <target/config.h>

#ifdef CONFIG_ARCH_HAS_VIC
#include <asm/mach/vic.h>
#endif

#ifndef ARCH_HAVE_VIC
typedef void (*irq_handler)(void);

#define vic_hw_vectors_init()
#define vic_hw_register_irq(nr, h)
void do_IRQ(irq_t nr);
#endif

#endif /* __VIC_DRIVER_H_INCLUDE__ */
