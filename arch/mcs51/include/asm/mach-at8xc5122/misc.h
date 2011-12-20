#ifndef __MISC_AT8XC5122_H_INCLUDE__
#define __MISC_AT8XC5122_H_INCLUDE__

/* PCMCIA */
Sfr(PCMADDR,	0xE4);		/* PCMCIA Interface Address */
Sfr(PCMDATA,	0xE5);		/* PCMCIA Interface Data */
Sfr(PCMTYPE,	0xE6);		/* PCMCIA Interface Configuration */
Sfr(PCMINT,	0xE7);		/* PCMCIA Interface Interrupt enable */

/* DECODER */
Sfr(DECEN,	0xE4);		/* DECODER Interface enable */
Sfr(DECPAG,	0xE5);		/* DECODER Interface Page   */
Sfr(DECONF,	0xE6);		/* DECODER Interface Configuration */

/* EMULATION INTERFACE */
Sfr(EMUCON,	0xDF);		/* EMULATION Interface Control */
Sfr(BKPCL,	0xEF);		/* Breakpoint PC Address Low   */
Sfr(BKPCH,	0xF7);		/* Breakpoint PC Address High  */
Sfr(ICON,	0xFF);		/* ICE Control Register	       */

#endif /* __MISC_AT8XC5122_H_INCLUDE__ */
