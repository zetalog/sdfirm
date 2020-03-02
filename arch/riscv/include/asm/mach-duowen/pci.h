#ifndef __PCI_DUOWEN_H_INCLUDE__
#define __PCI_DUOWEN_H_INCLUDE__

#include <target/arch.h>

#if defined(CONFIG_DUOWEN_PCIE)
#include <driver/dw_pcie.h>
#ifndef ARCH_HAVE_PCI
#define ARCH_HAVE_PCI		1
#else
#error "Multiple PCI controller defined"
#endif
#endif

#endif /* __PCI_DUOWEN_H_INCLUDE__ */
