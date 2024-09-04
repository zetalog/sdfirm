#ifndef __LPC_DRIVER_H_INCLUDE__
#define __LPC_DRIVER_H_INCLUDE__

#ifdef CONFIG_ARCH_HAS_LPC
#include <asm/mach/lpc.h>
#endif

#ifndef ARCH_HAVE_LPC
#define lpc_hw_ctrl_init()		do { } while (0)
#define lpc_hw_io_read8(a)		0
#define lpc_hw_io_write8(v, a)		do { } while (0)
#define lpc_hw_mem_read8(a)		0
#define lpc_hw_mem_write8(v, a)		do { } while (0)
#define lpc_hw_mem_read16(a)		0
#define lpc_hw_mem_write16(v, a)	do { } while (0)
#define lpc_hw_mem_read32(a)		0
#define lpc_hw_mem_write32(v, a)	do { } while (0)
#define lpc_hw_irq_status(irq)		0
#define lpc_hw_irq_clear(irq)		do { } while (0)
#define lpc_hw_irq_unmask(irq)		do { } while (0)
#define lpc_hw_irq_mask(irq)		do { } while (0)
#endif

#endif /* __LPC_DRIVER_H_INCLUDE__ */
