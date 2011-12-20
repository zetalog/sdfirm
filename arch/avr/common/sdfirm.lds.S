#include <target/config.h>
#include <asm/reg.h>

OUTPUT_FORMAT("elf32-avr","elf32-avr","elf32-avr")
OUTPUT_ARCH(avr)

#ifdef XRAMEND
#define DATAEND		XRAMEND
#else
#define DATAEND		RAMEND
#endif

MEMORY
{
	text      (rx)   : ORIGIN = 0, LENGTH = FLASHEND+1
	data      (rw!x) : ORIGIN = 0x800100, LENGTH = DATAEND+1-0x100
#ifdef E2END
	eeprom    (rw!x) : ORIGIN = 0x810000, LENGTH = E2END+1
#endif
	fuse      (rw!x) : ORIGIN = 0x820000, LENGTH = 1K
	lock      (rw!x) : ORIGIN = 0x830000, LENGTH = 1K
	signature (rw!x) : ORIGIN = 0x840000, LENGTH = 1K
}

SECTIONS
{
	/* internal text space or external memory */
	.text : {
		*(.vectors)
		KEEP(*(.vectors))
		*(.progmem*)
		. = ALIGN(2);
		/*
		 * For future tablejump instruction arrays for 3 byte pc devices.
		 * We don't relax jump/call instructions within these sections.
		 */
		*(.jumptables)
		*(.jumptables*)
		/* For code that needs to reside in the lower 128k progmem. */
		*(.lowtext)
		*(.lowtext*)
		*(.text)
		. = ALIGN(2);
		*(.text.*)
		. = ALIGN(2);
		_etext = .;
	} > text
	.data : AT (ADDR(.text) + SIZEOF(.text))
	{
		PROVIDE(__data_start = .);
		*(.idata.data)
		*(.idata.data*)
		PROVIDE(__idata_end = .);
#ifdef XRAMEND
		/* reserved for stack usage */
		. = RAMEND - 0xFF;
#endif
		PROVIDE(__xdata_start = .);
		*(.xdata.data)
		*(.xdata.data*)
		*(.data)
		*(.data*)
		*(.rodata)  /* We need to include .rodata here if gcc is used */
		*(.rodata*) /* with -fdata-sections.  */
		. = ALIGN(2);
		_edata = .;
		PROVIDE(__data_end = .);
	} > data
	.bss SIZEOF(.data) + ADDR(.data) : {
		PROVIDE(__bss_start = .);
		*(.bss)
		*(.bss*)
		*(COMMON)
		PROVIDE(__bss_end = .);
	} > data
	__data_load_start = LOADADDR(.data);
	__data_load_end = __data_load_start + SIZEOF(.data);
	__xdata_load_start = __data_load_start + __xdata_start - __data_start;
#ifdef E2END
	.eeprom : {
		*(.eeprom*)
		__eeprom_end = .;
	}  > eeprom
#endif
	.fuse : {
		KEEP(*(.fuse))
		KEEP(*(.lfuse))
		KEEP(*(.hfuse))
		KEEP(*(.efuse))
	} > fuse
	.lock : {
		KEEP(*(.lock*))
	} > lock
	.signature : {
		KEEP(*(.signature*))
	} > signature
}
