#ifndef __REG_VIRT_H_INCLUDE__
#define __REG_VIRT_H_INCLUDE__

#include <target/config.h>
#include <target/sizes.h>

#define RAM_BASE		UL(0x80000000)
#define RAMEND			UL(0x80000000)
#define PLIC_BASE		UL(0x0C000000)
#define CLINT_BASE		UL(0x2000000)

#endif /* __REG_VIRT_H_INCLUDE__ */
