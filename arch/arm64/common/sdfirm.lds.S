#include <target/config.h>
#include <target/const.h>
#include <asm-generic/sdfirm.lds.h>

OUTPUT_FORMAT("elf64-littleaarch64", "elf64-bigaarch64", "elf64-littleaarch64")
OUTPUT_ARCH(aarch64)
ENTRY(stext)

SECTIONS
{
#ifdef CONFIG_XIP
	. = CONFIG_TEXT_ROM_BASE;
#else
	. = CONFIG_TEXT_RAM_BASE;
#endif
	_stext = .;				/* begin of text and rodata sections */
	.init : {
		__init_text_begin = .;
		HEAD_TEXT
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
		__init_text_end = .;
	}

	_text = .;			/* address in RAM or ROM */
	.cmd_array ALIGN(8) : {
		__cmd_start = .;
		*(.cmd.text)
		__cmd_end = .;
	}
	.text : {
		TEXT_TEXT
	}

	RO_DATA_SECTION(8)

	. = ALIGN((8));
	_etext = .;				/* end of text and rodata sections */

	__text_loc = CONFIG_TEXT_ROM_BASE;	/* text location in ROM */
	_etext_loc = __text_loc + _etext - _stext;

#ifdef CONFIG_XIP
	. = CONFIG_TEXT_RAM_BASE;		/* ensure address followups in RAM */
#endif

	_sdata = .;				/* begin of data sections */
	.data : AT(_etext) {
		_data = .;			/* address in RAM */
#ifdef CONFIG_XIP
		__init_data_begin = .;
		INIT_DATA
		__init_data_end = .;
#endif
		DATA_DATA
	}
	_edata = .;				/* end of data sections */

	__data_loc = _etext_loc;		/* data location in ROM */
	_edata_loc = __data_loc + _edata - _sdata;

	.bss (NOLOAD) : ALIGN(8) {
		__bss_start = .;
		*(.bss .bss.*)
	}
	. = ALIGN(8);
	__bss_stop = .;
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
