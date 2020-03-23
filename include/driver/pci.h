#ifndef __PCI_DRIVER_H_INCLUDE__
#define __PCI_DRIVER_H_INCLUDE__

#ifdef CONFIG_ARCH_HAS_PCI
#include <asm/mach/pci.h>
#endif

#ifndef ARCH_HAVE_PCI
#define pci_platform_init()		do { } while (0)
#endif

#endif /* __PCI_DRIVER_H_INCLUDE__ */
