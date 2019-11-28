#ifndef __VIC_DRIVER_H_INCLUDE__
#define __VIC_DRIVER_H_INCLUDE__

#ifdef CONFIG_ARCH_HAS_VIC
#include <asm/mach/vic.h>
#endif

#ifndef ARCH_HAVE_VIC
#define vic_hw_vectors_init()
#define vic_hw_register_irq(nr, h)
#endif

#endif /* __VIC_DRIVER_H_INCLUDE__ */
