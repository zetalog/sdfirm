#include <target/compiler.h>
#include <target/cmdline.h>
#include <target/arch.h>
#include <target/percpu.h>
#include <asm-generic/sdfirm.lds.h>

#define IDMAP_TEXT			\
	. = ALIGN(SZ_4K);		\
	__idmap_text_start = .;		\
	*(.idmap.text)			\
	__idmap_text_end = .;

OUTPUT_FORMAT("elf64-littleaarch64", "elf64-bigaarch64", "elf64-littleaarch64")
OUTPUT_ARCH(aarch64)
ENTRY(__start)

SECTIONS
{
	. = PAGE_OFFSET + TEXT_OFFSET;
	_start = .;				/* begin of sdfirm */
	_stext = .;				/* begin of text and rodata sections */
	__stext = .;				/* begin of text sections */
	.init : {
		HEAD_TEXT
		__init_text_begin = .;
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
		. = ALIGN((8));
		__init_text_end = .;
	}

	CMD_LINE_SECTION(8)
	.test_array ALIGN(8) : {
		__testfn_start = .;
		KEEP (*(.testfn))
		__testfn_end = .;
	}
	.text : {
		TEXT_TEXT
		IDMAP_TEXT
	}
	. = ALIGN(PAGE_SIZE);
	__etext = .;				/* end of text sections */

	RO_DATA_SECTION(PAGE_SIZE)
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
	}
	_edata = .;
	__edata = .;				/* end of data sections */

	__data_loc = _etext_loc;		/* data boot location */
	_edata_loc = __data_loc + _edata - _sdata;

	.percpu : {
		PERCPU_INPUT(SMP_CACHE_BYTES)
	}

	.bss (NOLOAD) : ALIGN(8) {
		__bss_start = .;		/* begin of bss sections */
		*(.bss .bss.*)
	}
	. = ALIGN(8);
	__bss_stop = .;				/* end of bss sections */
	. = ALIGN(PAGE_SIZE);
	_edata = .;				/* end of data and bss sections */

	. += PERCPU_STACKS_SIZE;
	__end = .;				/* end of sdfirm */
	_end = .;				/* end of sdfirm */
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
