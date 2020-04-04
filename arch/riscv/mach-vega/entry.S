#include <target/init.h>
#include <target/arch.h>
#include <target/paging.h>

	__VEC

ENTRY(__vectors)
	jal	x0, __handle_traps
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

	/* Illegal instrution exception */
	jal	x0, __handle_illegal_instr

	/* ecall handler */
	jal	x0, __handle_ecall

	/* LSU error */
	jal	x0, __handle_lsu
END(__vectors)
