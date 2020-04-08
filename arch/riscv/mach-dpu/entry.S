#include <target/init.h>
#include <target/arch.h>
#include <target/paging.h>
#include <target/irq.h>
#include <target/task.h>
#include <asm/asm-offsets.h>

	__VEC

__ENTRY(__vectors, 8)
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
#ifdef CONFIG_RI5CY_FIRQ_EXT
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
#endif
END(__vectors)
