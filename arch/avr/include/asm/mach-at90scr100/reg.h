#ifndef __REG_AT90SCR100_H_INCLUDE__
#define __REG_AT90SCR100_H_INCLUDE__

/* Constants */
#define SPM_PAGESIZE 256
#define RAMEND		0x10FF
#ifdef CONFIG_AT90SCR100_EEPROM
#define E2END		0xFFF
#define E2PAGESIZE	8
#endif
#define FLASHEND	0x7FFF

/* Fuses */
#define FUSE_MEMORY_SIZE 3

/* Low Fuse Byte */
#define FUSE_CKSEL0	(unsigned char)~_BV(0)
#define FUSE_CKSEL1	(unsigned char)~_BV(1)
#define FUSE_CKSEL2	(unsigned char)~_BV(2)
#define FUSE_CKSEL3	(unsigned char)~_BV(3)
#define FUSE_SUT0	(unsigned char)~_BV(4)
#define FUSE_SUT1	(unsigned char)~_BV(5)
#define FUSE_CKOUT	(unsigned char)~_BV(6)
#define FUSE_CKDIV8	(unsigned char)~_BV(7)
#define LFUSE_DEFAULT	(FUSE_CKSEL1 &  & FUSE_CKSEL2 & FUSE_SUT0)

/* High Fuse Byte */
#define FUSE_BOOTRST	(unsigned char)~_BV(0)
#define FUSE_BOOTSZ0	(unsigned char)~_BV(1)
#define FUSE_BOOTSZ1	(unsigned char)~_BV(2)
#define FUSE_EESAVE	(unsigned char)~_BV(3)
#define FUSE_WDTON	(unsigned char)~_BV(4)
#define FUSE_SPIEN	(unsigned char)~_BV(5)
#define FUSE_JTAGEN	(unsigned char)~_BV(6)
#define FUSE_OCDEN	(unsigned char)~_BV(7)
#define HFUSE_DEFAULT	(FUSE_BOOTSZ0 & FUSE_BOOTSZ1 & FUSE_SPIEN & FUSE_JTAGEN)

/* Extended Fuse Byte */
#define FUSE_BODLEVEL0	(unsigned char)~_BV(0)
#define FUSE_BODLEVEL1	(unsigned char)~_BV(1)
#define FUSE_BODLEVEL2	(unsigned char)~_BV(2)
#define FUSE_HWBE	(unsigned char)~_BV(3)
#define EFUSE_DEFAULT	(0xFF)


/* Lock Bits */
#define __LOCK_BITS_EXIST
#define __BOOT_LOCK_BITS_0_EXIST
#define __BOOT_LOCK_BITS_1_EXIST 

/* Signature */
#define SIGNATURE_0 0x1E
#define SIGNATURE_1 0x96
#define SIGNATURE_2 0x09

#define _VECTORS_SIZE 152
#define _VECTORS_CUNT 38

/* 
 * Generic definitions for registers that are common across
 * multiple AVR devices and families.
 */

/*
 * Stack pointer register.
 */
#define SPL	_SFR_IO8(0x3D)
#define SPH	_SFR_IO8(0x3E)
#define SP	_SFR_IO16(0x3D)

/*
 * Status Register and bits
 */
#define SREG	_SFR_IO8(0x3F)
#define SREG_C	0
#define SREG_Z	1
#define SREG_N	2
#define SREG_V	3
#define SREG_S	4
#define SREG_H	5
#define SREG_T	6
#define SREG_I	7

/*
 * Pointer registers definitions
 */
#define XL	r26
#define XH	r27
#define YL	r28
#define YH	r29
#define ZL	r30
#define ZH	r31

/* No RAMPZ in this chip */

#endif /* __REG_AT90SCR100_H_INCLUDE__ */
