#include <target/config.h>
#include <target/const.h>
#include <target/cmdline.h>
#include <target/arch.h>
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
	_stext = .;				/* begin of text and rodata sections */
	__stext = .;				/* begin of text sections */
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
		. = ALIGN((8));
		__init_text_end = .;
	}
	CMD_LINE_SECTION(8)

	_text = .;				/* address in RAM or ROM */
	.text : {
		TEXT_TEXT
		IDMAP_TEXT
	}
	__etext = .;				/* end of text sections */

	RO_DATA_SECTION(8)

	. = ALIGN((8));
	_etext = .;				/* end of text and rodata sections */

	__text_loc = CONFIG_TEXT_ROM_BASE;	/* text location in ROM */
	_etext_loc = __text_loc + _etext - _stext;

#if defined(CONFIG_XIP) && (CONFIG_TEXT_RAM_BASE != CONFIG_TEXT_ROM_BASE)
	. = CONFIG_TEXT_RAM_BASE;		/* ensure address followups in RAM */
#endif

	_sdata = .;				/* begin of data and bss sections */
	__sdata = .;				/* begin of data sections */
	.data : AT(_etext) {
		_data = .;			/* address in RAM */
#ifdef CONFIG_XIP
		__init_data_begin = .;
		INIT_DATA
		__init_data_end = .;
#endif
		DATA_DATA
	}
	__edata = .;				/* end of data sections */

	__data_loc = _etext_loc;		/* data location in ROM */
	_edata_loc = __data_loc + _edata - _sdata;

	.bss (NOLOAD) : ALIGN(8) {
		__bss_start = .;		/* begin of bss sections */
		*(.bss .bss.*)
	}
	. = ALIGN(8);
	__bss_stop = .;				/* end of bss sections */
	_edata = .;				/* end of data and bss sections */

#ifdef CONFIG_MMU
	. = ALIGN(PAGE_SIZE);
#ifdef CONFIG_CPU_64v8_2_LPA
	mmu_id_map = .;
	. += IDMAP_DIR_SIZE;
#endif /* CONFIG_CPU_64v8_2_LPA */
	mmu_pg_dir = .;
	. += BPGT_DIR_SIZE;
#endif

	. = ALIGN(PAGE_SIZE);
	__end = .;
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
