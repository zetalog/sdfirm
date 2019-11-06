#include <target/raven.h>

ENTRY(GetCPUID)
	mrs	x0, MPIDR_EL1
	LSR	w1,w0,#8	// get aff1 level1 & others
	BFI	w0,w1,#1,#8	// merge aff0 level0 and aff1 level1 & others
	AND	w0,w0,#0x1ff	// clear other bits except the cpu id
	ret
ENDPROC(GetCPUID)
