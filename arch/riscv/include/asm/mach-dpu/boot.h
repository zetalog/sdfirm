#ifndef __BOOT_DPU_H_INCLUDE__
#define __BOOT_DPU_H_INCLUDE__

#ifndef __ASSEMBLY__
#include <target/arch.h>
#include <target/uart.h>
#include <target/smp.h>

#define BOOT_ERROR_INIT			(NR_CPUS + 1)
#define BOOT_ERROR_FIND			(NR_CPUS + 2)

#ifdef CONFIG_DPU_APC_INIT_MSG
#define __boot_msg(msg)			dpu_uart_putc((msg) + '0')
#define __boot_init()			\
	do {				\
		dpu_uart_init();	\
		dpu_uart_putc('\n');	\
		dpu_uart_putc('-');	\
	} while (0)
#define __boot_fini()			\
	do {				\
		dpu_uart_putc('-');	\
		dpu_uart_putc('\n');	\
	} while (0)
#else /* CONFIG_DPU_APC_INIT_MSG */
#define __boot_msg(msg)			do { } while (0)
#define __boot_init()			do { } while (0)
#define __boot_fini()			do { } while (0)
#endif /* CONFIG_DPU_APC_INIT_MSG */

#ifdef CONFIG_DPU_BOOT_DEBUG
#define __boot_dbg(byte)		dpu_uart_putc(byte)

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
#else /* CONFIG_DPU_BOOT_DEBUG */
#define __boot_dbg(byte)			do { } while (0)
#define __boot_dump8(byte, last)		do { } while (0)
#define __boot_dump32(dword, last)		do { } while (0)
#endif /* CONFIG_DPU_BOOT_DEBUG */

#endif /* __ASSEMBLY__ */

#endif /* __BOOT_DPU_H_INCLUDE__ */
