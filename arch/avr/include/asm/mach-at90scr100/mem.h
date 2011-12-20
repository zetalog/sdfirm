#ifndef __MEM_AT90SCR100_H_INCLUDE__
#define __MEM_AT90SCR100_H_INCLUDE__

#include <target/config.h>
#include <asm/reg.h>

/* 6-char sequence denoting where to find the EEPROM registers in memory space.
   Adresses denoted in hex syntax with uppercase letters. Used by the EEPROM
   subroutines.
   First two letters:  EECR address.
   Second two letters: EEDR address.
   Last two letters:   EEAR address.  */
#define __EEPROM_REG_LOCATIONS__ 1F2021

/*
 * EEPROM Control Register and bits
 */
#define EECR    _SFR_IO8(0x1F)

/*
 * EEPROM Data Register
 */
#define EEDR    _SFR_IO8(0x20)

/*
 * EEPROM Address Register
 */
#define EEAR    _SFR_IO16(0x21)
#define EEARL   _SFR_IO8(0x21)
#define EEARH   _SFR_IO8(0x22)

/*
 * Store Program Memory Control and Status Register
 */
#define SPMCSR  _SFR_IO8(0x37)

#endif /* __MEM_AT90SCR100_H_INCLUDE__ */
