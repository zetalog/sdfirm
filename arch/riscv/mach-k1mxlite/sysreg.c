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
 * @(#)sysreg.c: K1MXLite system registers implementation
 * $Id: sysreg.c,v 1.1 2023-09-06 10:48:00 zhenglv Exp $
 */

#include <target/arch.h>

void k1mxlite_cpu_reset(void)
{
	cpu_t cpu, hart;
	cpu_t boot_hart = sysreg_boot_cpu();

	if (boot_hart == csr_read(CSR_MHARTID) || 32 == csr_read(CSR_MHARTID)) {
		for (cpu = 0; cpu < MAX_CPU_NUM; cpu++) {
			hart = smp_hw_cpu_hart(cpu);
			if (hart == boot_hart || hart == 32)
				continue;
			if (_BV(hart) & acpu_get_cpu_map())
				sysreg_soft_reset_cpu(cpu);
		}
	}
}

uint32_t acpu_get_cpu_map(void)
{
	return sysreg_cpu_mask();
}

static uint8_t acpu_contract_cpu_map(uint32_t map)
{
	uint32_t mask = map;

	mask = ((mask & 0xaaaaaaaa) >>  1) | (mask & 0x55555555);
	mask = ((mask & 0x44444444) >>  2) | (mask & 0x11111111);
	mask = ((mask & 0x10101010) >>  3) | (mask & 0x01010101);
	mask = ((mask & 0x03000300) >>  6) | (mask & 0x00030003);
	mask = ((mask & 0x000f0000) >> 12) | (mask & 0x0000000f);
	return (uint8_t)mask;
}

uint8_t acpu_get_cluster_map(void)
{
	return acpu_contract_cpu_map(acpu_get_cpu_map());
}
