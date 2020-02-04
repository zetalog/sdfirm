#ifndef __IRQC_SPIKE_H_INCLUDE__
#define __IRQC_SPIKE_H_INCLUDE__

#ifndef ARCH_HAVE_IRQC_
#define ARCH_HAVE_IRQC		1
#else
#error "Multiple IRQ controller defined"
#endif

#include <asm/plic.h>

#endif /* __IRQC_SPIKE_H_INCLUDE__ */
