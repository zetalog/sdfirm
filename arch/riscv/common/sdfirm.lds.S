#include <target/compiler.h>
#include <target/arch.h>
#include <target/percpu.h>
#include <target/cmdline.h>
#include <target/paging.h>
#include <target/bench.h>
#include <target/sbi.h>
#include <target/acpi.h>

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

	TAIL_TEXT_SECTION(WORD_SIZE)

	. = PAGE_OFFSET + TEXT_OFFSET;
	_fw_start = .;				/* begin of firmware */
	_start = .;				/* begin of sdfirm */
	_stext = .;				/* begin of text and rodata sections */

	__stext = .;				/* begin of text sections */
	__init_text_begin = .;
	HEAD_TEXT_SECTION(WORD_SIZE)
	.init : {
		INIT_TEXT
		INIT_CALLS
	}
	. = ALIGN(WORD_SIZE);
	__init_text_end = .;
	_text = .;
	.text : {
		TEXT_TEXT
		IDMAP_TEXT
	}
	. = ALIGN(PAGE_SIZE);
	__etext = .;				/* end of text sections */

	__srodata = .;
	.rodata : {
		RODATA_RODATA
		CMDLINE_INPUT(WORD_SIZE)
		BENCH_INPUT(WORD_SIZE)
		SBIECALL_INPUT(WORD_SIZE)
		ACPIBIOS_INPUT(WORD_SIZE)
	}
	SRODATA(WORD_SIZE)
	. = ALIGN(PAGE_SIZE);
	__erodata = .;

	_etext = .;				/* end of text and rodata sections */

#ifndef CONFIG_MAXPHYSMEM_128GB
	__text_loc = CONFIG_BOOT_BASE;		/* text boot location */
	_etext_loc = __text_loc + _etext - _stext;
#endif

#if defined(CONFIG_XIP) && (CONFIG_LOAD_BASE != CONFIG_BOOT_BASE)
	. = CONFIG_LOAD_BASE;			/* ensure addresses followups are load addresses */
#endif

	. = ALIGN(PAGE_SIZE);
	_sdata = .;				/* begin of data and bss sections */
	_data = .;

	__sdata = .;				/* begin of data sections */
#ifdef CONFIG_LOAD_DATA
	.data : AT(_etext) {
#else
	.data : {
#endif
		__init_data_begin = .;
		INIT_DATA
		__init_data_end = .;
		DATA_DATA
		PERCPU_INPUT(SMP_CACHE_BYTES)
	}
	SDATA_SECTION(WORD_SIZE);
	__edata = .;				/* end of data sections */

	__bss_start = .;			/* begin of bss sections */
	SBSS(WORD_SIZE)
	BSS(WORD_SIZE)
#ifdef CONFIG_ARCH_HAS_HTIF
	HTIF_SECTION(PAGE_SIZE)
#endif
	. = ALIGN(WORD_SIZE);
	__bss_stop = .;				/* end of bss sections */

	. = ALIGN(PAGE_SIZE);
	_edata = .;				/* end of data and bss sections */

#ifndef CONFIG_CC_NO_RELAX
	/* Cover as much of sdata as possible if it exists.  If sdata+bss
	 * is smaller than 0x1000 then start from bss end to cover as much
	 * of the program as possible.  But rodata is not allowed to be
	 * referenced by gp, as the address of variables in rodata may
	 * change during relaxation, so start from data in that case.
	 */
	__global_pointer$ = MIN(__sdata_start + 0x800,
				MAX(_sdata + 0x800, _edata - 0x800));
#endif

#ifndef CONFIG_MAXPHYSMEM_128GB
	__data_loc = _etext_loc;		/* data boot location */
	_edata_loc = __data_loc + _edata - _sdata;
#endif

	. += PERCPU_STACKS_SIZE;
#ifdef CONFIG_SBI
	. += SBI_PERCPU_STACKS_SIZE;
	__sbi_heap_start = .;
#ifdef CONFIG_SBI_HEAP
	. += SBI_PERCPU_HEAPS_SIZE;
#endif
#endif
	__sbi_heap_end = .;
	__end = .;				/* end of sdfirm */
	_end = .;				/* end of sdfirm */

	. = ALIGN(PAGE_SIZE);
#ifdef CONFIG_SBI_PAYLOAD
#ifdef CONFIG_SBI_PAYLOAD_OFFSET
	. = PAGE_OFFSET + TEXT_OFFSET + CONFIG_SBI_PAYLOAD_OFFSET;
#endif
#endif
	_payload_start = .;
#ifdef CONFIG_SBI_PAYLOAD
	.payload : {
		*(.payload)
		. = ALIGN(WORD_SIZE);
	}
#endif /* CONFIG_SBI_PAYLOAD */
	_payload_end = .;
	. = ALIGN(PAGE_SIZE);
	_efi_start = .;
#ifdef CONFIG_EFI_BUILTIN
	.efi : {
		EFI_TABLE
		. = ALIGN(WORD_SIZE);
	}
#endif
	_efi_end = .;
	. = ALIGN(PAGE_SIZE);
	_fdt_start = .;
#ifdef CONFIG_FDT_BUILTIN
	.fdt : {
		*(.fdt)
		. = ALIGN(WORD_SIZE);
	}
#endif
	_fdt_end = .;
	_fw_end = .;				/* end of firmware */
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
ASSERT((ABI_PERCPU_STACKS_END <= RAMEND), "bad stack size, please tune CONFIG_STACK_SHIFT")
#ifdef CONFIG_SBI
ASSERT((SBI_PERCPU_STACKS_END <= RAMEND), "bad stack size, please tune CONFIG_STACK_SHIFT")
#endif
#ifdef CONFIG_XIP
#ifdef CONFIG_BFM
ASSERT((TEXT_OFFSET >= RAM_BASE) && (TEXT_OFFSET < RAMEND), "Wrong CONFIG_BOOT_BASE is specified against RAM_BASE!")
#else /* CONFIG_BFM */
ASSERT((TEXT_OFFSET >= ROM_BASE) && (TEXT_OFFSET < ROMEND), "Wrong CONFIG_BOOT_BASE is specified against ROM_BASE!")
#endif /* CONFIG_BFM */
#else /* CONFIG_XIP */
ASSERT((TEXT_OFFSET >= RAM_BASE) && (TEXT_OFFSET < RAMEND), "Wrong CONFIG_LOAD_BASE is specified against RAM_BASE!")
#endif /* CONFIG_XIP */
