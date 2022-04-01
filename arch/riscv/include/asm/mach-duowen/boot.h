#ifndef __BOOT_DUOWEN_H_INCLUDE__
#define __BOOT_DUOWEN_H_INCLUDE__

#ifndef __ASSEMBLY__
#include <target/arch.h>
#include <target/uart.h>

#ifdef CONFIG_DUOWEN_BOOT_PROT_STRONG
#undef SOC_BASE
#define SOC_BASE	(imc_socket_id() ? SOC1_BASE : SOC0_BASE)
#endif

#if defined(CONFIG_CONSOLE_OUTPUT) || defined(CONFIG_DUOWEN_BOOT_DEBUG)
static __always_inline void __boot_dbg(uint8_t byte)
{
	while (!dw_uart_write_poll(UART_CON_ID));
	dw_uart_write_byte(UART_CON_ID, byte);
}
#else
#define __boot_dbg(byte)			do { } while (0)
#endif

#ifdef CONFIG_DUOWEN_BOOT_PROT_STRONG
/* RAMEND in the current sdfirm is not used, sdfirm uses lower RAM storage
 * as .data section and stackes. And CONFIG_DUOWEN_BOOT_PROT_STRONG is not
 * dependent on HEAP.
 */
#define DUOWEN_BOOT_PROT_FUNC_DEFINE(size)			\
	uint8_t *boot_from_stack = ((uint8_t *)RAMEND - (size));

static __always_inline void __boot_copy(uint8_t *dst, void *src,
					size_t size)
{
	con_log("boot(stack): src=%016llx: dst=%016llx\n",
		(uintptr_t)src, (uintptr_t)dst);
	memcpy(dst, src, size);
	local_flush_icache_all();
}

#define DUOWEN_BOOT_PROT_FUNC_ASSIGN(type, func, ptr, size)	\
	do {							\
		__boot_copy(boot_from_stack, func, size);	\
		ptr = (type)boot_from_stack;			\
	} while (0)
#else
#define DUOWEN_BOOT_PROT_FUNC_DEFINE(size)
#define DUOWEN_BOOT_PROT_FUNC_ASSIGN(type, func, ptr, size)	\
	do {							\
		ptr = (type)func;				\
	} while (0)
#endif

#ifdef CONFIG_DUOWEN_BOOT_DEBUG
static __always_inline void __boot_dump8(uint8_t byte, bool last)
{
	if (HIHALF(byte) > 9)
		__boot_dbg('a' + HIHALF(byte) - 10);
	else
		__boot_dbg('0' + HIHALF(byte));
	if (LOHALF(byte) > 9)
		__boot_dbg('a' + LOHALF(byte) - 10);
	else
		__boot_dbg('0' + LOHALF(byte));
	if (last)
		__boot_dbg('\n');
	else
		__boot_dbg(' ');
}

static __always_inline void __boot_dump32(uint32_t dword, bool last)
{
	uint8_t *ptr = (uint8_t *)&dword;
	int i;

	for (i = 0; i < 4; i++) {
		__boot_dump8(ptr[i], last && i == 3);
	}
}
#else
#define __boot_dump8(byte, last)		do { } while (0)
#define __boot_dump32(dword, last)		do { } while (0)
#endif

static __always_inline void __boot_jump(void *boot)
{
	void (*boot_entry)(void) = boot;

	__boot_dbg('B');
	__boot_dbg('o');
	__boot_dbg('o');
	__boot_dbg('t');
	__boot_dbg('\n');
	local_flush_icache_all();
	boot_entry();
	unreachable();
}

#ifdef CONFIG_DUOWEN_BOOT_PROT_TEST
#define DUOWEN_BOOT_PROT_TEST_HELP			\
	"boot [addr] [size]\n"				\
	"    - test stack boot function\n"
#define DUOWEN_BOOT_PROT_TEST_FUNC(func, boot)		\
static int func(int argc, char *argv[])			\
{							\
	uint32_t addr = 516;				\
	size_t size = 8;				\
	if (argc > 2)					\
		addr = strtoul(argv[2], NULL, 0);	\
	if (argc > 3)					\
		size = strtoul(argv[3], NULL, 0);	\
	boot((void *)SFAB_RAM_BASE, addr, size, false);	\
	hexdump(addr, (void *)SFAB_RAM_BASE, 1, size);	\
	return 0;					\
}
#define DUOWEN_BOOT_PROT_TEST_EXEC(func)		\
	do {						\
		if (strcmp(argv[1], "boot") == 0)	\
			return func(argc, argv);	\
	} while (0)
#else
#define DUOWEN_BOOT_PROT_TEST_HELP
#define DUOWEN_BOOT_PROT_TEST_FUNC(func, boot)
#define DUOWEN_BOOT_PROT_TEST_EXEC(func)
#endif
#endif /* __ASSEMBLY__ */

#endif /* __BOOT_DUOWEN_H_INCLUDE__ */
