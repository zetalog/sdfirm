#ifndef __GPIO_AT8XC5122_H_INCLUDE__
#define __GPIO_AT8XC5122_H_INCLUDE__

/* Ports */
Sfr(P0,		0x80);		/* Port 0 */
Sbit(P0_7,	0x80, 7);
Sbit(P0_6,	0x80, 6);
Sbit(P0_5,	0x80, 5);
Sbit(P0_4,	0x80, 4);
Sbit(P0_3,	0x80, 3);
Sbit(P0_2,	0x80, 2);
Sbit(P0_1,	0x80, 1);
Sbit(P0_0,	0x80, 0);

Sfr(P1,		0x90);		/* Port 1 */
Sbit(P1_7,	0x90, 7);
Sbit(P1_6,	0x90, 6);
Sbit(P1_5,	0x90, 5);
Sbit(P1_4,	0x90, 4);
Sbit(P1_3,	0x90, 3);
Sbit(P1_2,	0x90, 2);
Sbit(P1_1,	0x90, 1);
Sbit(P1_0,	0x90, 0);

Sfr(P2,		0xA0);		/* Port 2 */
Sbit(P2_7,	0xA0, 7);
Sbit(P2_6,	0xA0, 6);
Sbit(P2_5,	0xA0, 5);
Sbit(P2_4,	0xA0, 4);
Sbit(P2_3,	0xA0, 3);
Sbit(P2_2,	0xA0, 2);
Sbit(P2_1,	0xA0, 1);
Sbit(P2_0,	0xA0, 0);

Sfr(P3,	0xB0);			/* Port 3 */
Sbit(P3_7,	0xB0, 7);
Sbit(P3_6,	0xB0, 6);
Sbit(P3_5,	0xB0, 5);
Sbit(P3_4,	0xB0, 4);
Sbit(P3_3,	0xB0, 3);
Sbit(P3_2,	0xB0, 2);
Sbit(P3_1,	0xB0, 1);
Sbit(P3_0,	0xB0, 0);

Sfr(P4,		0xC0);		/* Port 4 */
Sbit(P4_7,	0xC0, 7);
Sbit(P4_6,	0xC0, 6);
Sbit(P4_5,	0xC0, 5);
Sbit(P4_4,	0xC0, 4);
Sbit(P4_3,	0xC0, 3);
Sbit(P4_2,	0xC0, 2);
Sbit(P4_1,	0xC0, 1);
Sbit(P4_0,	0xC0, 0);

Sfr(P5,		0xE8);		/* Port 5 */
Sbit(P5_7,	0xE8, 7);
Sbit(P5_6,	0xE8, 6);
Sbit(P5_5,	0xE8, 5);
Sbit(P5_4,	0xE8, 4);
Sbit(P5_3,	0xE8, 3);
Sbit(P5_2,	0xE8, 2);
Sbit(P5_1,	0xE8, 1);
Sbit(P5_0,	0xE8, 0);

/* Port Mode Register 0 */
Sfr(PMOD0,	0x91);
/* Port Mode Register 1 */
Sfr(PMOD1,	0x84);

/* PMOD */
#define MSK_PMOD_P3C1		0x80	/* PMOD0 */
#define MSK_PMOD_P3C0		0x40
#define MSK_PMOD_P2C1		0x20
#define MSK_PMOD_P2C0		0x10
#define MSK_PMOD_P0C1		0x02
#define MSK_PMOD_P0C0		0x01
#define MSK_PMOD_P5HC1		0x80	/* PMOD1 */
#define MSK_PMOD_P5HC0		0x40
#define MSK_PMOD_P5MC1		0x20
#define MSK_PMOD_P5MC0		0x10
#define MSK_PMOD_P5LC1		0x08
#define MSK_PMOD_P5LC0		0x04
#define MSK_PMOD_P4C1		0x02
#define MSK_PMOD_P4C0		0x01

#endif /* __GPIO_AT8XC5122_H_INCLUDE__ */
