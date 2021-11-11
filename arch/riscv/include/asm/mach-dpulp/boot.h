/*
 * ZETALOG's Personal COPYRIGHT
 *
 * Copyright (c) 2021
 *    ZETALOG - "Lv ZHENG".  All rights reserved.
 *    Author: Lv "Zetalog" Zheng
 *    Internet: zhenglv@hotmail.com
 *
 * This COPYRIGHT used to protect Personal Intelligence Rights.
 * Redistribution and use in source and binary forms with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *    This product includes software developed by the Lv "Zetalog" ZHENG.
 * 3. Neither the name of this software nor the names of its developers may
 *    be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 * 4. Permission of redistribution and/or reuse of souce code partially only
 *    granted to the developer(s) in the companies ZETALOG worked.
 * 5. Any modification of this software should be published to ZETALOG unless
 *    the above copyright notice is no longer declaimed.
 *
 * THIS SOFTWARE IS PROVIDED BY THE ZETALOG AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE ZETALOG OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * @(#)boot.h: DPU-LP specific xSBL bootloader interface
 * $Id: boot.h,v 1.1 2021-11-11 17:16:00 zhenglv Exp $
 */

#ifndef __BOOT_DPULP_H_INCLUDE__
#define __BOOT_DPULP_H_INCLUDE__

#ifndef __ASSEMBLY__
#include <target/arch.h>
#include <target/uart.h>

#ifdef CONFIG_DPULP_BOOT_PROT_STRONG
#define DPULP_BOOT_PROT_FUNC_DEFINE(size)		\
	__align(4) uint8_t boot_from_stack[size];

static __always_inline void __boot_copy(uint8_t *dst, void *src,
					size_t size)
{
	printf("src=%016llx: dst=%016llx\n", (uintptr_t)src, (uintptr_t)dst);
	memcpy(dst, src, size);
}

#define DPULP_BOOT_PROT_FUNC_ASSIGN(type, func, ptr)	\
	do {						\
		__boot_copy(boot_from_stack, func,	\
			    sizeof(boot_from_stack));	\
		ptr = (type)boot_from_stack;		\
	} while (0)
#else
#define DPULP_BOOT_PROT_FUNC_DEFINE(size)
#define DPULP_BOOT_PROT_FUNC_ASSIGN(type, func, ptr)	\
	do {						\
		ptr = (type)func;			\
	} while (0)
#endif

#ifdef CONFIG_DPULP_BOOT_DEBUG
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

static __always_inline void __boot_jump(void *boot)
{
	void (*boot_entry)(void) = boot;

	__boot_dbg('B');
	__boot_dbg('o');
	__boot_dbg('o');
	__boot_dbg('t');
	__boot_dbg('\n');
	boot_entry();
	unreachable();
}

#ifdef CONFIG_DPULP_BOOT_PROT_TEST
#define DPULP_BOOT_PROT_TEST_HELP			\
	"boot [addr] [size]\n"				\
	"    - test stack boot function\n"
#define DPULP_BOOT_PROT_TEST_FUNC(func, boot)		\
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
#define DPULP_BOOT_PROT_TEST_EXEC(func)		\
	do {						\
		if (strcmp(argv[1], "boot") == 0)	\
			return func(argc, argv);	\
	} while (0)
#else
#define DPULP_BOOT_PROT_TEST_HELP
#define DPULP_BOOT_PROT_TEST_FUNC(func, boot)
#define DPULP_BOOT_PROT_TEST_EXEC(func)
#endif
#endif /* __ASSEMBLY__ */

#endif /* __BOOT_DPULP_H_INCLUDE__ */
