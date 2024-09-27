/*
 * ZETALOG's Personal COPYRIGHT
 *
 * Copyright (c) 2022
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
 * @(#)mach.c: K1MAX specific board initialization
 * $Id: mach.c,v 1.1 2022-10-15 14:36:00 zhenglv Exp $
 */

#include <target/arch.h>
#include <target/bench.h>
#include <target/smp.h>
#include <target/paging.h>
#include <target/barrier.h>
#include <target/percpu.h>
#include <target/cmdline.h>
#include <target/sbi.h>

void board_early_init(void)
{
	DEVICE_ARCH(DEVICE_ARCH_RISCV);
}

#ifdef CONFIG_K1M_BOOT_DDR
void board_boot_ddr(void)
{
	void (*boot_entry)(void);

	boot_entry = (void *)DRAM_BASE;
	printf("B(D)\n");
	smp_boot_secondary_cpus((caddr_t)boot_entry);
	local_flush_icache_all();
	boot_entry();
}
#else
#define board_boot_ddr()	do { } while (0)
#endif

#ifdef CONFIG_K1M_BOOT
void board_boot(void)
{
	board_boot_ddr();
}
#else
#define board_boot()		do { } while (0)
#endif

void board_late_init(void)
{
	board_boot();
}

#ifdef CONFIG_SMP
void board_smp_init(void)
{
}
#endif /* CONFIG_SMP */

#ifdef CONFIG_FINISH
#ifdef CONFIG_SBI
void board_finish(int code)
{
	sbi_finish(code);
}
#else /* CONFIG_SBI */
void board_finish(int code)
{
	if (code)
		k1max_finish(1);
	else
		k1max_finish(0);
}
#endif /* CONFIG_SBI */
#endif /* CONFIG_FINISH */

#ifdef CONFIG_SHUTDOWN
void board_shutdown(void)
{
	k1max_finish(0);
}
#endif

static int do_k1max_aia_msi(int argc, char *argv[])
{
#ifdef CONFIG_CPU_SMAIA_IMSIC
	int i, msi;
	uint64_t inc = 0x0101010101010101;
	uint64_t val;
	uint64_t eidelivery[] = {
		0x00000000, 0x00000001, 0x00000002, 0x00000003,
		0x40000000, 0x40000001, 0x40000002, 0x40000003,
	};

	csr_write(CSR_MISELECT, AIA_EIDELIVERY);
	val = csr_read(CSR_MIREG);
	printf("EIDELIVERY : %016llx\n", val);
	for (i = 0; i < ARRAY_SIZE(eidelivery); i++) {
		csr_write(CSR_MIREG, eidelivery[i]);
		val = csr_read(CSR_MIREG);
		printf("EIDELIVERY : %016llx\n", val);
	}

	val = 0x3020100030201000;
	csr_write(CSR_MISELECT, AIA_EITHRESHOLD);
	csr_write(CSR_MIREG, val);
	csr_write(CSR_MISELECT, AIA_EITHRESHOLD);
	val = csr_read(CSR_MIREG);
	printf("EITHRESHOLD: %016llx\n", val);

	for (msi = 0; msi < 128; msi += 16) {
		csr_write(CSR_MISELECT, AIA_SETEIPNUM);
		csr_write(CSR_MIREG, msi);
		for (i = 0; i < 4; i += 2) {
			csr_write(CSR_MISELECT, AIA_EIP(i));
			val = csr_read(CSR_MIREG);
			printf("EIP%02d      : %016llx\n", i, val);
		}
	}

	val = 0x3020100030201000;
	for (i = 30; i >= 0; i -= 2) {
		csr_write(CSR_MISELECT, AIA_EIP(i));
		csr_write(CSR_MIREG, val);
		csr_write(CSR_MISELECT, AIA_EIE(i));
		csr_write(CSR_MIREG, val);
		val += inc;
		printf("MTOPEI     : %016llx\n",
		       (uint64_t)csr_read(CSR_MTOPEI));
	}
	for (i = 0; i < 32; i += 2) {
		csr_write(CSR_MISELECT, AIA_EIP(i));
		val = csr_read(CSR_MIREG);
		printf("EIP%02d      : %016llx\n", i, val);
		csr_write(CSR_MISELECT, AIA_EIE(i));
		val = csr_read(CSR_MIREG);
		printf("EIE%02d      : %016llx\n", i, val);
	}
	return 0;
#else
	return -1;
#endif
}

static int do_k1max_aia_prio(int argc, char *argv[])
{
#ifdef CONFIG_CPU_SMAIA_IMSIC
	int i;
	uint64_t inc = 0x0101010101010101;
	uint64_t val = 0x3020100030201000;

	for (i = 0; i < 16; i += 2) {
		csr_write(CSR_MISELECT, AIA_IPRIO(i));
		csr_write(CSR_MIREG, val);
		val += inc;
	}
	for (i = 0; i < 16; i += 2) {
		csr_write(CSR_MISELECT, AIA_IPRIO(i));
		val = csr_read(CSR_MIREG);
		printf("MIPRIO%02d   : %016llx\n", i, val);
	}
	return 0;
#else
	return -1;
#endif
}

static int do_k1max_aia(int argc, char *argv[])
{
	if (argc < 3)
		return -EINVAL;

	if (strcmp(argv[2], "prio") == 0)
		return do_k1max_aia_prio(argc, argv);
	if (strcmp(argv[2], "msi") == 0)
		return do_k1max_aia_msi(argc, argv);
	return -EINVAL;
}

static int do_k1max(int argc, char *argv[])
{
	if (argc < 2)
		return -EINVAL;

	if (strcmp(argv[1], "aia") == 0)
		return do_k1max_aia(argc, argv);
	return -EINVAL;
}

DEFINE_COMMAND(k1max, do_k1max, "K1MAX SoC global commands",
	"k1max aia prio\n"
	"	-AIA MIPRIO register test\n"
	"k1max aia msi\n"
	"	-AIA IMSIC register test\n"
);
