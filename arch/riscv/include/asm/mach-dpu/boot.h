#ifndef __BOOT_DPU_H_INCLUDE__
#define __BOOT_DPU_H_INCLUDE__

#ifndef __ASSEMBLY__
#include <target/arch.h>
#include <target/uart.h>

#ifdef CONFIG_DPU_BOOT_DEBUG
static __always_inline void __boot_dbg(uint8_t byte)
{
	while (!dw_uart_write_poll(UART_CON_ID));
	dw_uart_write_byte(UART_CON_ID, byte);
}

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
#define __boot_dbg(byte)			do { } while (0)
#define __boot_dump8(byte, last)		do { } while (0)
#define __boot_dump32(dword, last)		do { } while (0)
#endif

#endif /* __ASSEMBLY__ */

#endif /* __BOOT_DPU_H_INCLUDE__ */
