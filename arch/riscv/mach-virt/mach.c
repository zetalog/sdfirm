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
 * @(#)mach.c: SPIKE specific board initialization
 * $Id: mach.c,v 1.1 2019-09-05 15:41:00 zhenglv Exp $
 */

#include <target/arch.h>
#include <target/sbi.h>
#include <target/cmdline.h>

#ifdef CONFIG_SHUTDOWN
#ifdef CONFIG_SBI
void board_shutdown(void)
{
	sbi_shutdown();
}
#else
void board_shutdown(void)
{
	virt_finish_pass();
}
#endif
#endif

#ifdef CONFIG_FINISH
void board_finish(int code)
{
	if (code) {
		printf("Test failure.\n\n");
		virt_finish_fail();
	} else {
		printf("Test success.\n\n");
		virt_finish_pass();
	}
}
#endif

void board_early_init(void)
{
	DEVICE_ARCH(DEVICE_ARCH_RISCV);
}

void board_late_init(void)
{
}

#ifdef CONFIG_SMP
void board_smp_init(void)
{
}
#endif /* CONFIG_SMP */

static int do_qemu_shutdown(int argc, char *argv[])
{
	board_shutdown();
	return 0;
}

static int do_qemu(int argc, char *argv[])
{
	if (argc < 2)
		return -EINVAL;

	if (strcmp(argv[1], "shutdown") == 0)
		return do_qemu_shutdown(argc, argv);
	return -EINVAL;
}

DEFINE_COMMAND(qemu, do_qemu, "QEMU virtio (virt) simulator global commands",
	"qemu shutdown\n"
	"    -shutdown board\n"
);
