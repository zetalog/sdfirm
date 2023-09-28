/*
 * ZETALOG's Personal COPYRIGHT
 *
 * Copyright (c) 2023
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
 * @(#)sysreg.c: K1-max system registers implementation
 * $Id: sysreg.c,v 1.1 2023-05-30 17:48:00 zhenglv Exp $
 */

#include <target/arch.h>

#define BOOT_CLUSTER	CPU_TO_CLUSTER(BOOT_HART)

void k1max_cpu_reset(void)
{
	cpu_t cpu, hart, cluster;
	uint32_t reset = __raw_readl(SYS_CPU_SOFTWARE_RST);

	if (BOOT_HART == csr_read(CSR_MHARTID)) {
		for (cpu = 0; cpu < MAX_CPU_NUM; cpu++) {
			hart = smp_hw_cpu_hart(cpu);
			cluster = CPU_TO_CLUSTER(hart);

			if (hart == BOOT_HART)
				continue;
			if (!(hart & BOOT_MASK))
				continue;
			reset |= SYS_CPU_RST(cpu);
			if (cluster == BOOT_CLUSTER)
				continue;
			reset |= SYS_CLUSTER_RST(cluster);
		}
	}
	__raw_writel(reset, SYS_CPU_SOFTWARE_RST);
}
