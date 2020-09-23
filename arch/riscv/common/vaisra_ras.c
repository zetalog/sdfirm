/*
 * ZETALOG's Personal COPYRIGHT
 *
 * Copyright (c) 2020
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
 * @(#)vaisra_ras.c: VAISRA RAS (reli/avail/service-ability) implementation
 * $Id: vaisra_ras.c,v 1.1 2020-09-15 06:36:00 zhenglv Exp $
 */

#include <target/ras.h>
#include <target/sbi.h>

struct vaisra_err {
	uint8_t err;
	const char *verbose;
};

static struct vaisra_err vaisra_dces[MAX_DCES] = {
	{ DCE_L1VTPE,    "L1-dcache virtual tag parity error" },
	{ DCE_L1PTPE,    "L1-dcache physical tag parity error" },
	{ DCE_L1DPE,     "L1-dcache data parity error" },
	{ DCE_S1FTLBTPE, "Stage-1 final TLB tag parity error" },
	{ DCE_S1FTLBDPE, "Stage-1 final TLB data parity error" },
};

static struct vaisra_err vaisra_ices[MAX_ICES] = {
	{ ICE_L0TPE,     "L0-icache tag parity error" },
	{ ICE_L0DPE,     "L0-icache data parity error" },
	{ ICE_L1TPE,     "L1-icache tag parity error" },
	{ ICE_L1DPE,     "L1-icache data parity error" },
	{ ICE_V0ICPEO,   "Interleave-0 I-cache parity error observed" },
	{ ICE_V1ICPEO,   "Interleave-1 I-cache parity error observed" },
};

static struct vaisra_err vaisra_l2es[MAX_L2ES] = {
	{ L2E_TSBEC0AD,  "Tag single bit error cpu0 abort or discard" },
	{ L2E_TSBEC0NA,  "Tag single bit error cpu0 no-abort" },
	{ L2E_TSBEC1AD,  "Tag single bit error cpu1 abort or discard" },
	{ L2E_TSBEC1NA,  "Tag single bit error cpu1 no-abort" },
	{ L2E_TSBESNP,   "Tag single bit error snoop" },
	{ L2E_TSBEL2I,   "Tag single bit error L2 initiated" },
	{ L2E_TDBEC0AD,  "Tag double bit error cpu0 abort or discard" },
	{ L2E_TDBEC0NA,  "Tag double bit error cpu0 no-abort" },
	{ L2E_TDBEC1AD,  "Tag double bit error cpu1 abort or discard" },
	{ L2E_TDBEC1NA,  "Tag double bit error cpu1 no-abort" },
	{ L2E_TDBESNP,   "Tag double bit error snoop" },
	{ L2E_TDBEL2I,   "Tag double bit error L2 initiated" },
	{ L2E_DSBEC0AD,  "Data single bit error cpu0 abort or discard" },
	{ L2E_DSBEC0NA,  "Data single bit error cpu0 no-abort" },
	{ L2E_DSBEC1AD,  "Data single bit error cpu1 abort or discard" },
	{ L2E_DSBEC1NA,  "Data single bit error cpu1 no-abort" },
	{ L2E_DSBESNP,   "Data single bit error snoop" },
	{ L2E_DSBEL2I,   "Data single bit error L2 initiated" },
	{ L2E_DDBEC0AD,  "Data double bit error cpu0 abort or discard" },
	{ L2E_DDBEC0NA,  "Data double bit error cpu0 no-abort" },
	{ L2E_DDBEC1AD,  "Data double bit error cpu1 abort or discard" },
	{ L2E_DDBEC1NA,  "Data double bit error cpu1 no-abort" },
	{ L2E_DDBESNP,   "Data double bit error snoop" },
	{ L2E_DDBEL2I,   "Data double bit error L2 initiated" },
	{ L2E_BMPCEC0AD, "Bus master port command error cpu0 abort or discard" },
	{ L2E_BMPCEC0NA, "Bus master port command error cpu0 no-abort" },
	{ L2E_BMPCEC1AD, "Bus master port command error cpu1 abort or discard" },
	{ L2E_BMPCEC1NA, "Bus master port command error cpu1 no-abort" },
	{ L2E_BMPCESNP,  "Bus master port command error snoop" },
	{ L2E_BMPCEL2I,  "Bus master port command error L2 initiated" },
	{ L2E_BMPDEC0AD, "Bus master port data error cpu0 abort or discard" },
	{ L2E_BMPDEC0NA, "Bus master port data error cpu0 no-abort" },
	{ L2E_BMPDEC1AD, "Bus master port data error cpu1 abort or discard" },
	{ L2E_BMPDEC1NA, "Bus master port data error cpu1 no-abort" },
};

void l2csr_write(uint16_t csr, uint64_t val)
{
	csr_write(CSR_ML2CSRS, csr);
	csr_write(CSR_ML2CSRD, val);
}

uint64_t l2csr_read(uint16_t csr)
{
	csr_write(CSR_ML2CSRS, csr);
	return csr_read(CSR_ML2CSRD);
}

#ifdef CONFIG_SBI
bool sbi_process_vaisra_nmi(void)
{
	uint64_t ecr0, ecr1, esr;
	int err;

	sbi_printf("IRQ_M_NMI\n");

	ecr0 = csr_read(CSR_MDCEC);
	esr = csr_read(CSR_MDCES);
	for (err = 0; err < MAX_DCES; err++) {
		if (err_indicated(esr, err) && err_nmi_enabled(ecr0, err))
			sbi_printf("%s\n", vaisra_dces[err].verbose);
	}

	ecr0 = csr_read(CSR_MICEC);
	esr = csr_read(CSR_MICES);
	for (err = 0; err < MAX_DCES; err++) {
		if (err_indicated(esr, err) && err_nmi_enabled(ecr0, err))
			sbi_printf("%s\n", vaisra_ices[err].verbose);
	}

	ecr0 = l2csr_read(L2CSR_L2EC0);
	ecr1 = l2csr_read(L2CSR_L2EC1);
	esr = l2csr_read(L2CSR_L2ES_GC);
	for (err = 0; err < MAX_L2ES; err++) {
		if (err_indicated(esr, err) && (err > 31 ?
		    err_nmi_enabled(ecr1, err - 32) :
		    err_nmi_enabled(ecr0, err)))
			sbi_printf("%s\n", vaisra_l2es[err].verbose);
	}

	return false;
}
#endif /* CONFIG_SBI */
