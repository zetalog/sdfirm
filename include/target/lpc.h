#ifndef __LPC_H_INCLUDE__
#define __LPC_H_INCLUDE__

#include <target/generic.h>
#include <driver/lpc.h>

#define lpc_io_read8(a)		lpc_hw_io_read8(a)
#define lpc_io_write8(v, a)	lpc_hw_io_write8(v, a)
#define lpc_mem_read8(a)	lpc_hw_mem_read8(a)
#define lpc_mem_write8(v, a)	lpc_hw_mem_write8(v, a)
#define lpc_mem_read16(a)	lpc_hw_mem_read16(a)
#define lpc_mem_write16(v, a)	lpc_hw_mem_write16(v, a)
#define lpc_mem_read32(a)	lpc_hw_mem_read32(a)
#define lpc_mem_write32(v, a)	lpc_hw_mem_write32(v, a)

#define lpc_sirq_config(irq, pol)	lpc_hw_irq_config(irq, pol)
#define lpc_sirq_clear(irq)		lpc_hw_irq_clear(irq)
#define lpc_sirq_mask(irq)		lpc_hw_irq_mask(irq)
#define lpc_sirq_unmask(irq)		lpc_hw_irq_unmask(irq)
#define lpc_sirq_status(irq)		lpc_hw_irq_status(irq)

#endif /* __LPC_H_INCLUDE__ */
