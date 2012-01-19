#ifndef __MEM_AT8XC5122_H_INCLUDE__
#define __MEM_AT8XC5122_H_INCLUDE__

#include <target/config.h>
#include <target/generic.h>

#include <asm/reg.h>

Sfr(AUXR,	0x8E);		/* Auxilary Register 0 */
Sfr(AUXR1,	0xA2);		/* Auxilary Register 1 */
Sfr(RCON,	0xD1);		/* Memory Selection */

/* AUXR */
#define MSK_AUXR_DPU		0x80	/* AUXR */
#define MSK_AUXR_XRS0		0x04
#define MSK_AUXR_EXTRAM		0x02
#define MSK_AUXR_AO		0x01
#define MSK_AUXR_ENBOOT		0x20	/* AUXR1 */
#define MSK_AUXR_GF3		0x08
#define MSK_AUXR_DPS		0x01
#define MSK_RCON_RPS		0x08	/* RCON */

/* This for CRAM version only */
#define xram_internal_on()	(AUXR &= ~MSK_AUXR_EXTRAM)
#define xram_external_on()	(AUXR |= MSK_AUXR_EXTRAM)
#define xram_internal_256()	(AUXR &= ~MSK_AUXR_XRS0)
#define xram_internal_512()	(AUXR |= MSK_AUXR_XRS0)
#define xram_external_ale_on()	(AUXR &= ~MSK_AUXR_AO)
#define xram_external_ale_off()	(AUXR |= MSK_AUXR_AO)

/* for GPIO quasi bi-direction mode */
#define gpio_weak_pullup_on()	(AUXR &= ~MSK_AUXR_DPU)
#define gpio_weak_pullup_off()	(AUXR |= MSK_AUXR_DPU)

#endif /* __MEM_AT8XC5122_H_INCLUDE__ */
