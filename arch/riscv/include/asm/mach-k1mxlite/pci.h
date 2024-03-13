#ifndef __PCI_K1MXLITE_H_INCLUDE__
#define __PCI_K1MXLITE_H_INCLUDE__

#include <driver/pcie_designware.h>

#define PCIE_LINK_MODE PORT_LINK_MODE_4_LANES

#ifdef CONFIG_K1MXLITE_PCIE
void pcie_ccix_linkup(void);
#else
#define pcie_ccix_linkup()	do { } while (0)
#endif

#endif /* __PCI_K1MXLITE_H_INCLUDE__ */
