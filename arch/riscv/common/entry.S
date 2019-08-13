#include <target/config.h>
#include <target/linkage.h>
#include <target/init.h>
#include <target/arch.h>
#include <asm/assembler.h>
#include <asm/asm-offsets.h>
#include <target/paging.h>

#define EXCEPTION_STACK_SIZE 0x58

enter_exception:
	sw	x3, 0x00(x2)	// gp
	sw	x4, 0x04(x2)	// tp
	sw	x5, 0x08(x2)	// t0
	sw	x6, 0x0c(x2)	// t1
	sw	x7, 0x10(x2)	// t2
	sw	x10, 0x14(x2)	// a0
	sw	x11, 0x18(x2)	// a1
	sw	x12, 0x1c(x2)	// a2
	sw	x13, 0x20(x2)	// a3
	sw	x14, 0x24(x2)	// a4
	sw	x15, 0x28(x2)	// a5
	sw	x16, 0x2c(x2)	// a6
	sw	x17, 0x30(x2)	// a7

	csrr	a0, 0x7B0
	csrr	a1, 0x7B1
	csrr	a2, 0x7B2
	sw	a0, 0x34(x2)	// lpstart[0]
	sw	a1, 0x38(x2)	// lpend[0]
	sw	a2, 0x3c(x2)	// lpcount[0]
	csrr	a0, 0x7B4
	csrr	a1, 0x7B5
	csrr	a2, 0x7B6
	sw	a0, 0x40(x2)	// lpstart[1]
	sw	a1, 0x44(x2)	// lpend[1]
	sw	a2, 0x48(x2)	// lpcount[1]

	csrr	a0, 0x341
	sw	a0, 0x4c(x2)	// mepc
	csrr	a1, 0x300
	sw	a1, 0x50(x2)	// mstatus
	jalr	x0, x1

exit_exception:
	lw	a1, 0x50(x2)	// mstatus
	csrrw	x0, 0x300, a1
	lw	a0, 0x4c(x2)	// mepc
	csrrw	x0, 0x341, a0

	lw	a0, 0x40(x2)	// lpstart[1]
	lw	a1, 0x44(x2)	// lpend[1]
	lw	a2, 0x48(x2)	// lpcount[1]
	csrrw	x0, 0x7B4, a0
	csrrw	x0, 0x7B5, a1
	csrrw	x0, 0x7B6, a2
	lw	a0, 0x34(x2)	// lpstart[0]
	lw	a1, 0x38(x2)	// lpend[0]
	lw	a2, 0x3c(x2)	// lpcount[0]
	csrrw	x0, 0x7B0, a0
	csrrw	x0, 0x7B1, a1
	csrrw	x0, 0x7B2, a2

	lw	x3, 0x00(x2)	// gp
	lw	x4, 0x04(x2)	// tp
	lw	x5, 0x08(x2)	// t0
	lw	x6, 0x0c(x2)	// t1
	lw	x7, 0x10(x2)	// t2
	lw	x10, 0x14(x2)	// a0
	lw	x11, 0x18(x2)	// a1
	lw	x12, 0x1c(x2)	// a2
	lw	x13, 0x20(x2)	// a3
	lw	x14, 0x24(x2)	// a4
	lw	x15, 0x28(x2)	// a5
	lw	x16, 0x2c(x2)	// a6
	lw	x17, 0x30(x2)	// a7

	lw	x1, 0x54(x2)
	addi	x2, x2, EXCEPTION_STACK_SIZE
	mret

ENTRY(__handle_irq)
	addi	x2, x2, -EXCEPTION_STACK_SIZE
	sw	x1, 0x54(x2)
	jal	x1, enter_exception
	la	x1, exit_exception
	jal	x0, irqc_hw_handle_irq
ENDPROC(__handle_irq)

ENTRY(__unhandled_except)
	j	.
ENDPROC(__unhandled_except)

	.macro define_exception_entry entry_name handler_name
	.weak \entry_name
\entry_name:
	addi	x2, x2, -EXCEPTION_STACK_SIZE
	sw	x1, 0x54(x2)
	jal	x1, enter_exception
	la	x1, exit_exception
	jal	x0, \handler_name
	.endm

define_exception_entry __handle_illegal_instr __unhandled_except
define_exception_entry __handle_ecall __unhandled_except
define_exception_entry __handle_lsu __unhandled_except
