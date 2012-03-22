#ifndef __REG_MCS51_H_INCLUDE__
#define __REG_MCS51_H_INCLUDE__

#ifdef SDCC
#define Sfr(x, y)			__sfr __at y		x
#define Sbit(x, y, z)			__sbit __at ((y) ^ (z))	x
#define Sfr16(x, y)			Sfr(x, y)
#elif __C51__
#define Sfr(x, y)			sfr x = y
#define Sbit(x, y, z)			sbit x = y ^ z
#define Sfr16(x, y)			sfr16 x = y
#endif

#define __raw_readb(a)			(*(volatile __near__ unsigned char *)(a))
#define __raw_writeb(v, a)		(*(volatile __near__ unsigned char *)(a) = (v))
#define __raw_setb_atomic(b, a)				\
	do {						\
		unsigned char __v = __raw_readb(a);	\
		__v |= _BV(b);				\
		__raw_writeb(__v, (a));			\
	} while (0)
#define __raw_clearb_atomic(b, a)			\
	do {						\
		unsigned char __v = __raw_readb(a);	\
		__v &= ~_BV(b);				\
		__raw_writeb(__v, (a));			\
	} while(0)
#define __raw_testb_atomic(b, a)			\
	((__raw_readb(a) >> b) & 0x01)

/* 80C51/80C52 Special Function Registers */

/* Non-Bit Addressible */
/* Stack Pointer */
Sfr(SP,		0x81);
/* Data Pointer */
Sfr16(DPTR,	0x82);		/* 2 Bytes */
Sfr(DPL,	0x82);		/* Low byte */
Sfr(DPH,	0x83);		/* High Byte */

/* Bit Addressible */
/* Accumulator */
Sfr(A,		0xE0);
Sfr(ACC,	0xE0);
/* B Register */
Sfr(B,		0xF0);
/* Program Status Word */
Sfr(PSW,	0xD0);
Sbit(CY,	0xD0, 7);	/* Carry Flag */
Sbit(AC,	0xD0, 6);	/* Auxilary Carry Flag */
Sbit(F0,	0xD0, 5);	/* Flag 0 available to the user for general purpose */
Sbit(RS1,	0xD0, 4);	/* Register Bank Selector bit 1 */
Sbit(RS0,	0xD0, 3);	/* Register Bank Selector bit 0 */
Sbit(OV,	0xD0, 2);	/* Overflow Flag */
Sbit(F1,	0xD0, 1);	/* User Definable Flag */
Sbit(P	,	0xD0, 0);	/* Parity Flag */

/* Interrupt Enable Control */
Sfr(IE,		0xA8);
Sbit(EA,	0xA8, 7);	/* IRQ enabling all */

#endif /* __REG_MCS51_H_INCLUDE__ */
