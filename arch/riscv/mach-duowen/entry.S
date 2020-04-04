#include <target/init.h>
#include <target/arch.h>
#include <target/paging.h>
#include <target/irq.h>
#include <target/task.h>
#include <asm/asm-offsets.h>

	__VEC

ENTRY(__vectors)
	jal	x0, __handle_traps
	jal	x0, __handle_bad_virq
	jal	x0, __handle_bad_virq
	jal	x0, __handle_bad_virq
	jal	x0, __handle_bad_virq
	jal	x0, __handle_bad_virq
	jal	x0, __handle_bad_virq
	jal	x0, __handle_tmr_virq
	jal	x0, __handle_bad_virq
	jal	x0, __handle_bad_virq
	jal	x0, __handle_bad_virq
	jal	x0, __handle_ext_virq
	jal	x0, __handle_bad_virq
	jal	x0, __handle_bad_virq
	jal	x0, __handle_bad_virq
	jal	x0, __handle_bad_virq
	jal	x0, __handle_bad_virq
	jal	x0, __handle_bad_virq
	jal	x0, __handle_bad_virq
	jal	x0, __handle_bad_virq
	jal	x0, __handle_bad_virq
	jal	x0, __handle_bad_virq
	jal	x0, __handle_bad_virq
	jal	x0, __handle_bad_virq
	jal	x0, __handle_bad_virq
	jal	x0, __handle_bad_virq
	jal	x0, __handle_bad_virq
	jal	x0, __handle_bad_virq
	jal	x0, __handle_bad_virq
	jal	x0, __handle_bad_virq
	jal	x0, __handle_bad_virq
	jal	x0, __handle_bad_virq
	/* reset vector */
	jal	x0, __start
END(__vectors)
