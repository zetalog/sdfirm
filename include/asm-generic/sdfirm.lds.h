/* Align . to a 8 byte boundary equals to maximum function alignment. */
#define ALIGN_FUNCTION() . = ALIGN(8)

#ifdef CONFIG_ARCH_HAS_LOVEC
#define LOVEC_TEXT						\
	__vectors = .;						\
	__svector = .;						\
	*(.lovec.text)						\
	__evector = .;
#else
#define LOVEC_TEXT
#endif
#ifdef CONFIG_ARCH_HAS_HIVEC
#define HIVEC_TEXT						\
	__vectors = .;						\
	__svector = .;						\
	*(.hivec.text)						\
	__evector = .;
#else
#define HIVEC_TEXT
#endif

#define HEAD_TEXT						\
	*(.head.text)
#define HEAD_TEXT_SECTION(align)				\
	.head.text : AT(ADDR(.head.text)) ALIGN(align) {	\
		LOVEC_TEXT					\
		HEAD_TEXT					\
	}
#define TAIL_TEXT_SECTION(align)				\
	.tail.text : ALIGN(align) {				\
		HIVEC_TEXT					\
	}
#define INIT_DATA						\
	*(.init.data)						\
	*(.init.rodata)
#define INIT_TEXT						\
	*(.init.text)
#define INITCALLS						\
  	*(.initcall0.init)					\
  	*(.initcall0s.init)					\
  	*(.initcall1.init)					\
  	*(.initcall1s.init)					\
  	*(.initcall2.init)					\
  	*(.initcall2s.init)					\
  	*(.initcall3.init)					\
  	*(.initcall3s.init)					\
  	*(.initcall4.init)					\
  	*(.initcall4s.init)					\
  	*(.initcall5.init)					\
  	*(.initcall5s.init)
#define INIT_CALLS						\
	__initcall_start = .;					\
	INITCALLS						\
	__initcall_end = .;
#define TEXT_TEXT						\
	ALIGN_FUNCTION();					\
	*(.text)						\
	*(.text.*)						\
	*(.ref.text)
#define DATA_DATA						\
	*(.data)						\
	*(.ref.data)
#define RODATA_RODATA						\
	*(.rodata) *(.rodata.*)					\
	*(.ref.rodata)

/*
 * Read only Data
 */
#define RO_DATA_SECTION(align)					\
	.rodata : AT(ADDR(.rodata)) ALIGN(align) {		\
		RODATA_RODATA					\
	}
/*
 * bss (Block Started by Symbol) - uninitialized data
 * zeroed during startup
 */
#define BSS(bss_align)						\
	.bss (NOLOAD) : ALIGN(bss_align) {			\
		*(.bss) *(.bss.*)				\
	}
#define BSS_SECTION(bss_align, stop_align)			\
	__bss_start = .;					\
	BSS(bss_align)						\
	. = ALIGN(stop_align);					\
	__bss_stop = .;

#define EFI_TABLE						\
	*(.efi.init)						\
	*(.efi.table)
