#ifndef __ARCH_VIRT_H_INCLUDE__
#define __ARCH_VIRT_H_INCLUDE__

/* This file is intended to be used for implementing SoC specific
 * instructions, registers.
 */

#ifndef __ASSEMBLY__
void board_reset(void);
void board_suspend(void);
void board_hibernate(void);
#endif /* __ASSEMBLY__ */

#endif /* __ARCH_VIRT_H_INCLUDE__ */
