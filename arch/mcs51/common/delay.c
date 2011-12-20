#include <target/delay.h>

#ifdef CONFIG_LPS_32BITS
#error "32-bit lps_t is not supported"
#endif

void __delay(volatile lps_t loops)
{
#if 0
	while (loops--);
#else
	__asm
	mov	r2,dpl
	mov	r3,dph
	; while (loops--);
00001$:
	dec	r2
	cjne	r2,#0xff,00002$
	dec	r3
00002$:
	mov	a,r2
	orl	a,r3
	jnz	00001$
	__endasm
	;
#endif
}
