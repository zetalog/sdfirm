#ifndef __REG_VEXPRESSA9_H_INCLUDE__
#define __REG_VEXPRESSA9_H_INCLUDE__

/* CoreTile Express A9x4 Daughterboard Memory Map */
#define CE9X4_DDR_SIZE	0x20000000
#define CE9X4_DDR2	(CE9X4_DDR1 + CE9X4_DDR_SIZE)
#define CE9X4_DDR1	0x60000000

#define CE9X4_CS0	0x40000000
#define CE9X4_CS1	0x44000000
#define CE9X4_CS2	0x48000000
#define CE9X4_CS3	0x4C000000
#define CE9X4_CS4	0x50000000
#define CE9X4_CS5	0x54000000
#define CE9X4_CS6	0x58000000
#define CE9X4_CS7	0x10000000

#include <asm/mach/reg-daughter.h>
#include <asm/mach/reg-mother.h>

#define ROM_BASE	CE9X4_NOR1
#define RAM_BASE	CE9X4_DDR1
#define RAMEND		(CE9X4_DDR2 + CE9X4_DDR_SIZE)

#endif /* __REG_VEXPRESSA9_H_INCLUDE__ */
