#include <target/config.h>
#include "macros.inc"

#ifdef CONFIG_CC_ISR_VECTOR
	.macro	vector name
	.if (. - __vectors < _VECTORS_SIZE)
	.weak	\name
	.set	\name, __bad_interrupt
	XJMP	\name
	.endif
	.endm
#else
	.macro	vector nr=1
	.if (. - __vectors < _VECTORS_SIZE)
	XJMP	__do_common_intr\nr
	.endif
	.endm
#endif

	.section .vectors,"ax",@progbits
	.global	__vectors
	.func	__vectors
__vectors:
#ifdef CONFIG_CC_ISR_VECTOR
	XJMP	__init
	vector	__vector_1
	vector	__vector_2
	vector	__vector_3
	vector	__vector_4
	vector	__vector_5
	vector	__vector_6
	vector	__vector_7
	vector	__vector_8
	vector	__vector_9
	vector	__vector_10
	vector	__vector_11
	vector	__vector_12
	vector	__vector_13
	vector	__vector_14
	vector	__vector_15
	vector	__vector_16
	vector	__vector_17
	vector	__vector_18
	vector	__vector_19
	vector	__vector_20
	vector	__vector_21
	vector	__vector_22
	vector	__vector_23
	vector	__vector_24
	vector	__vector_25
	vector	__vector_26
	vector	__vector_27
	vector	__vector_28
	vector	__vector_29
	vector	__vector_30
	vector	__vector_31
	vector	__vector_32
	vector	__vector_33
	vector	__vector_34
	vector	__vector_35
	vector	__vector_36
	vector	__vector_37
	vector	__vector_38
	vector	__vector_39
	vector	__vector_40
#else
	XJMP	__init
	vector	1
	vector	2
	vector	3
	vector	4
	vector	5
	vector	6
	vector	7
	vector	8
	vector	9
	vector	10
	vector	11
	vector	12
	vector	13
	vector	14
	vector	15
	vector	16
	vector	17
	vector	18
	vector	19
	vector	20
	vector	21
	vector	22
	vector	23
	vector	24
	vector	25
	vector	26
	vector	27
	vector	28
	vector	29
	vector	30
	vector	31
	vector	32
	vector	33
	vector	34
	vector	35
	vector	36
	vector	37
	vector	38
	vector	39
	vector	40
#endif
	.endfunc

	/* Handle unexpected interrupts (enabled and no handler), which
	   usually indicate a bug.  Jump to the __vector_default function
	   if defined by the user, otherwise jump to the reset address.

	   This must be in a different section, otherwise the assembler
	   will resolve "rjmp" offsets and there will be no relocs.  */

	.text
	.global	__bad_interrupt
	.func	__bad_interrupt
__bad_interrupt:
	.weak	__vector_default
	.set	__vector_default, __vectors
	XJMP	__vector_default
	.endfunc

	.section .text,"ax",@progbits
	.weak	__init
	;.func	__init
__init:
	.weak	__stack
	.set	__stack, RAMEND

	clr	__zero_reg__
	out	_SFR_IO_ADDR(SREG), __zero_reg__
	ldi	r28,lo8(__stack)
#ifdef SPH
	ldi	r29,hi8(__stack)
	out	_SFR_IO_ADDR(SPH), r29
#endif	/* _HAVE_AVR_STACK_POINTER_HI */
	out	_SFR_IO_ADDR(SPL), r28

#ifdef __AVR_3_BYTE_PC__
	ldi	r16, hh8(pm(__vectors))
	out	_SFR_IO_ADDR(EIND), r16
#endif	/* __AVR_3_BYTE_PC__ */

#ifdef __AVR_HAVE_RAMPD__
	out	AVR_RAMPD_ADDR, __zero_reg__
	out	AVR_RAMPX_ADDR, __zero_reg__
	out	AVR_RAMPY_ADDR, __zero_reg__
	out	_SFR_IO_ADDR(RAMPZ), __zero_reg__
#endif

#if BIG_CODE
	/* Only for >64K devices with RAMPZ, replaces the default code
	   provided by libgcc.S which is only linked in if necessary.  */

	.global	do_copy_idata
do_copy_idata:
	ldi	r17, hi8(__idata_end)
	ldi	r26, lo8(__data_start)
	ldi	r27, hi8(__data_start)
	ldi	r30, lo8(__data_load_start)
	ldi	r31, hi8(__data_load_start)

	/* On the enhanced core, "elpm" with post-increment updates RAMPZ
	   automatically.  Otherwise we have to handle it ourselves.  */

#ifdef __AVR_ENHANCED__
	ldi	r16, hh8(__data_load_start)
#else
	ldi	r16, hh8(__data_load_start - 0x10000)
do_copy_idata_carry:
	inc	r16
#endif
	out	_SFR_IO_ADDR(RAMPZ), r16
	rjmp	do_copy_idata_start

do_copy_idata_loop:
#ifdef __AVR_ENHANCED__
	elpm	r0, Z+
#else
	elpm
#endif
	st	X+, r0
#ifndef __AVR_ENHANCED__
	adiw	r30, 1
	brcs	do_copy_idata_carry
#endif

do_copy_idata_start:
	cpi	r26, lo8(__idata_end)
	cpc	r27, r17
	brne	do_copy_idata_loop
#ifdef __AVR_HAVE_RAMPD__
	out	_SFR_IO_ADDR(RAMPZ), __zero_reg__
#endif /* __AVR_HAVE_RAMPD__*/

	.global	do_copy_xdata
do_copy_xdata:
	ldi	r17, hi8(__data_end)
	ldi	r26, lo8(__xdata_start)
	ldi	r27, hi8(__xdata_start)
	ldi	r30, lo8(__xdata_load_start)
	ldi	r31, hi8(__xdata_load_start)

	/* On the enhanced core, "elpm" with post-increment updates RAMPZ
	   automatically.  Otherwise we have to handle it ourselves.  */

#ifdef __AVR_ENHANCED__
	ldi	r16, hh8(__xdata_load_start)
#else
	ldi	r16, hh8(__xdata_load_start - 0x10000)
do_copy_xdata_carry:
	inc	r16
#endif
	out	_SFR_IO_ADDR(RAMPZ), r16
	rjmp	do_copy_xdata_start

do_copy_xdata_loop:
#ifdef __AVR_ENHANCED__
	elpm	r0, Z+
#else
	elpm
#endif
	st	X+, r0
#ifndef __AVR_ENHANCED__
	adiw	r30, 1
	brcs	do_copy_xdata_carry
#endif

do_copy_xdata_start:
	cpi	r26, lo8(__data_end)
	cpc	r27, r17
	brne	do_copy_xdata_loop
#ifdef __AVR_HAVE_RAMPD__
	out	_SFR_IO_ADDR(RAMPZ), __zero_reg__
#endif /* __AVR_HAVE_RAMPD__*/

#else /* for < 64K ROM device */

	.global	do_copy_idata
do_copy_idata:
	# XXX: Provided MACROS
	# AVR toolchain will use following variables as macros if we have
	# provided them in the linker script.
	ldi	r17, hi8(__idata_end)
	ldi	r26, lo8(__data_start)
	ldi	r27, hi8(__data_start)
	ldi	r30, lo8(__data_load_start)
	ldi	r31, hi8(__data_load_start)
	rjmp	.do_copy_idata_start
.do_copy_idata_loop:
#if defined (__AVR_HAVE_LPMX__)
	lpm	r0, Z+
#else
	lpm
	adiw	r30, 1
#endif
	st	X+, r0
.do_copy_idata_start:
	cpi	r26, lo8(__idata_end)
	cpc	r27, r17
	brne	.do_copy_idata_loop

	.global	do_copy_xdata
do_copy_xdata:
	ldi	r17, hi8(__data_end)
	ldi	r26, lo8(__xdata_start)
	ldi	r27, hi8(__xdata_start)
	ldi	r30, lo8(__xdata_load_start)
	ldi	r31, hi8(__xdata_load_start)
	rjmp	.do_copy_xdata_start
.do_copy_xdata_loop:
#if defined (__AVR_HAVE_LPMX__)
	lpm	r0, Z+
#else
	lpm
	adiw	r30, 1
#endif
	st	X+, r0
.do_copy_xdata_start:
	cpi	r26, lo8(__data_end)
	cpc	r27, r17
	brne	.do_copy_xdata_loop

#endif /* BIG_CODE */

	.global	do_clear_bss
do_clear_bss:
	ldi	r17, hi8(__bss_end)
	ldi	r26, lo8(__bss_start)
	ldi	r27, hi8(__bss_start)
	rjmp	do_clear_bss_start

do_clear_bss_loop:
	st	X+, r1

do_clear_bss_start:
	cpi	r26, lo8(__bss_end)
	cpc	r27, r17
	brne	do_clear_bss_loop
	XJMP	system_init

;	.endfunc

