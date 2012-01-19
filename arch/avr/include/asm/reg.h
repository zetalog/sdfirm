#ifndef __REG_AVR_H_INCLUDE__
#define __REG_AVR_H_INCLUDE__

#include <target/config.h>

#define __SFR_MEM_BASE		0x20

#ifndef __ASSEMBLER__
#include <target/types.h>
#define _MMIO_BYTE(mem_addr)	(*(volatile uint8_t *)(mem_addr))
#define _MMIO_WORD(mem_addr)	(*(volatile uint16_t *)(mem_addr))
#define _MMIO_DWORD(mem_addr)	(*(volatile uint32_t *)(mem_addr))
#endif

#ifdef __ASSEMBLER__
#define _SFR_MEM8(mem_addr)	(mem_addr)
#define _SFR_MEM16(mem_addr)	(mem_addr)
#define _SFR_MEM32(mem_addr)	(mem_addr)
#define _SFR_IO8(io_addr)	((io_addr) + __SFR_MEM_BASE)
#define _SFR_IO16(io_addr)	((io_addr) + __SFR_MEM_BASE)
#define _SFR_MEM_ADDR(sfr)	(sfr)
#else
#define _SFR_MEM8(mem_addr)	_MMIO_BYTE(mem_addr)
#define _SFR_MEM16(mem_addr)	_MMIO_WORD(mem_addr)
#define _SFR_MEM32(mem_addr)	_MMIO_DWORD(mem_addr)
#define _SFR_IO8(io_addr)	_MMIO_WORD((io_addr) + __SFR_MEM_BASE)
#define _SFR_IO16(io_addr)	_MMIO_WORD((io_addr) + __SFR_MEM_BASE)
#define _SFR_MEM_ADDR(sfr)	((uint16_t)&(sfr))
#endif

#define _SFR_IO_ADDR(sfr)	(_SFR_MEM_ADDR(sfr) - __SFR_MEM_BASE)

/*
 * MCU Status, Control Register
 */
#define MCUSR   _SFR_IO8(0x34)
#define MCUCR   _SFR_IO8(0x35)

/*
 * On-chip Debug Reigster
 */
#define MONDR   _SFR_IO8(0x31)
#define OCDR    _SFR_IO8(0x31)

#include <asm/mach/reg.h>

#endif /* __REG_AVR_H_INCLUDE__ */
