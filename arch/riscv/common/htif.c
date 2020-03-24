/*
 * ZETALOG's Personal COPYRIGHT
 *
 * Copyright (c) 2019
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
 * @(#)htif.c: host target interface (HTIF) implementation
 * $Id: htif.c,v 1.1 2020-01-15 13:48:00 zhenglv Exp $
 */

#include <target/arch.h>
#include <target/spinlock.h>
#include <target/string.h>
#include <target/panic.h>

extern uint64_t __htif_base;
volatile uint64_t tohost __attribute__((section(".htif")));
volatile uint64_t fromhost __attribute__((section(".htif")));
volatile int htif_console_buf = -1;
DEFINE_SPINLOCK(htif_lock);

#define TOHOST(base_int)	(uint64_t *)(base_int + TOHOST_OFFSET)
#define FROMHOST(base_int)	(uint64_t *)(base_int + FROMHOST_OFFSET)

#define TOHOST_OFFSET		((uintptr_t)tohost - (uintptr_t)__htif_base)
#define FROMHOST_OFFSET		((uintptr_t)fromhost - (uintptr_t)__htif_base)

#define HTIF_DEV_SYSCALL_PROXY		0
#define HTIF_DEV_BCD			1

#define HTIF_SYSCALL_PROXY_CMD_SYSCALL	0

#define HTIF_BCD_CMD_READ		0
#define HTIF_BCD_CMD_WRITE		1

#define TOHOST_CMD(dev, cmd, payload)			\
	(((uint64_t)(dev) << 56) |			\
	 ((uint64_t)(cmd) << 48) | (uint64_t)(payload))
#define FROMHOST_DEV(fromhost_value)	\
	((uint64_t)(fromhost_value) >> 56)
#define FROMHOST_CMD(fromhost_value)	\
	((uint64_t)(fromhost_value) << 8 >> 56)
#define FROMHOST_DATA(fromhost_value)	\
	((uint64_t)(fromhost_value) << 16 >> 16)

static void __check_fromhost(void)
{
	uint64_t fh = fromhost;

	if (!fh)
		return;
	fromhost = 0;

	/* this should be from the console */
	BUG_ON(FROMHOST_DEV(fh) != 1);
	switch (FROMHOST_CMD(fh)) {
	case 0:
		htif_console_buf = 1 + (uint8_t)FROMHOST_DATA(fh);
		break;
	case 1:
		break;
	default:
		BUG();
	}
}

static void __set_tohost(uintptr_t dev, uintptr_t cmd, uintptr_t data)
{
	while (tohost)
		__check_fromhost();
	tohost = TOHOST_CMD(dev, cmd, data);
}

#if __riscv_xlen == 32
bool htif_console_poll(void)
{
	return false;
}

int htif_console_read(void)
{
	/* HTIF devices are not supported on RV32 */
	return -1;
}
#else
bool htif_con_pending = false;
bool htif_console_poll(void)
{
	int ch;

	spin_lock(&htif_lock);
	ch = htif_console_buf;
	if (ch >= 0)
		goto exit_succ;
	__check_fromhost();
	ch = htif_console_buf;
	if (ch >= 0)
		goto exit_succ;
	if (!htif_con_pending) {
		__set_tohost(HTIF_DEV_BCD, HTIF_BCD_CMD_READ, 0);
		htif_con_pending = true;
	}
exit_succ:
	spin_unlock(&htif_lock);
	return ch >= 0;
}

int htif_console_read(void)
{
	int ch = 0;

	spin_lock(&htif_lock);
	if (htif_console_buf >= 0) {
		ch = htif_console_buf;
		htif_console_buf = -1;
		htif_con_pending = false;
	}
	spin_unlock(&htif_lock);
	return ch - 1;
}
#endif

void htif_console_write(uint8_t ch)
{
	spin_lock(&htif_lock);
	__set_tohost(HTIF_DEV_BCD, HTIF_BCD_CMD_WRITE, ch);
	spin_unlock(&htif_lock);
}

static void do_tohost_fromhost(uintptr_t dev, uintptr_t cmd, uintptr_t data)
{
	spin_lock(&htif_lock);
	__set_tohost(dev, cmd, data);

	while (1) {
		uint64_t fh = fromhost;
		if (fh) {
			if (FROMHOST_DEV(fh) == dev &&
			    FROMHOST_CMD(fh) == cmd) {
				fromhost = 0;
				break;
			}
			__check_fromhost();
		}
	}
	spin_unlock(&htif_lock);
}

void htif_syscall(uintptr_t arg)
{
	do_tohost_fromhost(HTIF_DEV_SYSCALL_PROXY,
			   HTIF_SYSCALL_PROXY_CMD_SYSCALL, arg);
}

__noreturn void htif_poweroff()
{
	while (1) {
		fromhost = 0;
		tohost = 1;
	}
}
