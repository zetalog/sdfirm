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

#ifdef CONFIG_SBI
void sbi_process_vaisra_nmi(void)
{
	uint32_t ecr, esr;
	int err;

	sbi_printf("IRQ_M_NMI\n");

	ecr = csr_read(CSR_DCEC);
	esr = csr_read(CSR_DCES);
	for (err = 0; err < MAX_DCES; err++) {
		if (err_indicated(err) && err_nmi_enabled(err))
			printf("%s\n", vaisra_dces[err].verbose);
	}

	ecr = csr_read(CSR_ICEC);
	esr = csr_read(CSR_ICES);
	for (err = 0; err < MAX_DCES; err++) {
		if (err_indicated(err) && err_nmi_enabled(err))
			printf("%s\n", vaisra_ices[err].verbose);
	}
}
#endif /* CONFIG_SBI */
