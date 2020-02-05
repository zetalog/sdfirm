#include <target/compiler.h>
#include <target/arch.h>
#include <target/cmdline.h>
#include <target/paging.h>

#define IDMAP_TEXT			\
	. = ALIGN(SZ_4K);		\
	__idmap_text_start = .;		\
	*(.idmap.text)			\
	__idmap_text_end = .;

OUTPUT_ARCH(riscv)
ENTRY(__start)

SECTIONS
{
	. = PAGE_OFFSET + VEC_BASE;

#ifdef CONFIG_ARCH_HAS_HIVEC
	__svector = .;
	TAIL_TEXT_SECTION(WORD_SIZE)
	__evector = .;
#endif

	. = PAGE_OFFSET + TEXT_OFFSET;
	PROVIDE(_fw_start = .);
	_start = .;				/* begin of sdfirm */
	_stext = .;				/* begin of text and rodata sections */
	__stext = .;				/* begin of text sections */
	__init_text_begin = .;
	HEAD_TEXT_SECTION(WORD_SIZE)
	.init : {
		INIT_TEXT
		/*
		__proc_info_begin = .;
		*(.proc.info.init)
		__proc_info_end = .;
		__arch_info_begin = .;
		*(.arch.info.init)
		__arch_info_end = .;
		*/
		INIT_CALLS
		INIT_DATA
		. = ALIGN(WORD_SIZE);
		__init_text_end = .;
	}

	_text = .;
	.text : {
		TEXT_TEXT
		IDMAP_TEXT
	}
	. = ALIGN(PAGE_SIZE);
	__etext = .;				/* end of text sections */

	__srodata = .;
	RO_DATA_SECTION(WORD_SIZE)
	CMD_LINE_SECTION(WORD_SIZE)
	SRO_DATA_SECTION(WORD_SIZE)
	. = ALIGN(PAGE_SIZE);
	__erodata = .;

	_etext = .;				/* end of text and rodata sections */

	__text_loc = CONFIG_BOOT_BASE;		/* text boot location */
	_etext_loc = __text_loc + _etext - _stext;

#if defined(CONFIG_XIP) && (CONFIG_LOAD_BASE != CONFIG_BOOT_BASE)
	. = CONFIG_LOAD_BASE;			/* ensure addresses followups are load addresses */
#endif

	. = ALIGN(PAGE_SIZE);

	_sdata = .;				/* begin of data and bss sections */
	__sdata = .;				/* begin of data sections */
#ifdef CONFIG_LOAD_DATA
	.data : AT(_etext) {
#else
	.data : {
#endif
		_data = .;
#ifdef CONFIG_XIP
		__init_data_begin = .;
		INIT_DATA
		__init_data_end = .;
#endif
		DATA_DATA
		. = ALIGN((WORD_SIZE));
	}

	.sdata : {
		__global_pointer$ = . + 0x800;
		*(.sdata .sdata.*)
		. = ALIGN(WORD_SIZE);
	}
	__edata = .;				/* end of data sections */

	__data_loc = _etext_loc;		/* data boot location */
	_edata_loc = __data_loc + _edata - _sdata;

	.bss (NOLOAD) : ALIGN(WORD_SIZE) {
		__bss_start = .;		/* begin of bss sections */
		*(.sbss .sbss.*)
		*(.bss .bss.*)
	}

#ifdef CONFIG_ARCH_HAS_HTIF
	HTIF_SECTION(WORD_SIZE)
#endif
	. = ALIGN(WORD_SIZE);
	__bss_stop = .;				/* end of bss sections */
	. = ALIGN(PAGE_SIZE);
	_edata = .;				/* end of data and bss sections */

	. += PERCPU_STACKS_SIZE;
#ifdef CONFIG_SBI
	PROVIDE(_fw_end = .);
	. += SBI_PERCPU_STACKS_SIZE;
#endif
	__end = .;				/* end of sdfirm */
	_end = .;				/* end of sdfirm */

#ifdef FW_PAYLOAD_OFFSET
	. = PAGE_OFFSET + TEXT_OFFSET + FW_PAYLOAD_OFFSET;
#else
	. = ALIGN(PAGE_SIZE);
#endif
	.payload :
	{
		PROVIDE(_payload_start = .);
		*(.payload)
		. = ALIGN(8);
		PROVIDE(_payload_end = .);
	}
	PROVIDE(_fw_reloc_end = .);
}

/*
 * These must never be empty
 * If you have to comment these two assert statements out, your
 * binutils is too old (for other reasons as well)
 */
/*
ASSERT((__proc_info_end - __proc_info_begin), "missing CPU support")
ASSERT((__arch_info_end - __arch_info_begin), "no machine record defined")
*/
