#include <target/init.h>
#include <target/arch.h>

ENTRY(duowen_load_sram)
	la	a5, load_addr
	ld	a5, 0(a5)
	jalr	a5
ENDPROC(duowen_load_sram)

load_addr:
	.dword	SFAB_RAM_BASE
