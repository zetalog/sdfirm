#ifndef __PWM_AT8XC5122_H_INCLUDE__
#define __PWM_AT8XC5122_H_INCLUDE__

#include <target/config.h>
#include <target/generic.h>

#include <asm/reg.h>

Sfr(PCON,	0x87);		/* Power Control */

#define MSK_PCON_SMOD1		0x80	/* PCON */
#define MSK_PCON_SMOD0		0x40
#define MSK_PCON_POF		0x10
#define MSK_PCON_GF1		0x08
#define MSK_PCON_GF0		0x04
#define MSK_PCON_PD		0x02
#define MSK_PCON_IDL		0x01

#define set_idle_mode()		(PCON |= MSK_PCON_IDL)
#define set_powerdown_mode()	(PCON |= MSK_PCON_PD)

#endif /* __PWM_AT8XC5122_H_INCLUDE__ */
