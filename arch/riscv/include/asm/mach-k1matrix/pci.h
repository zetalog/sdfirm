#ifndef __PCI_K1MATRIX_H_INCLUDE__
#define __PCI_K1MATRIX_H_INCLUDE__

#ifdef CONFIG_K1MATRIX_PCIE
void pcie_ccix_linkup(void);
#else
#define pcie_ccix_linkup()	do { } while (0)
#endif

#endif /* __PCI_K1MATRIX_H_INCLUDE__ */
