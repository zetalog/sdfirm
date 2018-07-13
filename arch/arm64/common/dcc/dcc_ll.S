#include <target/config.h>
#include <target/linkage.h>
#include <target/init.h>
#include <asm/assembler.h>

ENTRY(__dcc_status)
	mrs	x0, MDCCSR_EL0
	ret
ENDPIPROC(__dcc_status)

ENTRY(__dcc_write)
	msr	DBGDTRTX_EL0, x0
	ret
ENDPIPROC(__dcc_write)

ENTRY(__dcc_read)
	mrs	x0, DBGDTRRX_EL0
	ret
ENDPIPROC(__dcc_read)
