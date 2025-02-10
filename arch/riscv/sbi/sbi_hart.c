/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#include <target/sbi.h>
#include <target/stream.h>
#include <sbi/riscv_asm.h>
#include <sbi/sbi_math.h>

static __unused uint8_t sbi_mode_switched = PRV_M;

static unsigned long trap_info_offset;
static unsigned long hart_features_offset;

/**
 * Return HART ID of the caller.
 */
unsigned int sbi_current_hartid()
{
	return (uint32_t)csr_read(CSR_MHARTID);
}

static void menvcfg_init(struct sbi_scratch *scratch, uint32_t hartid)
{
	unsigned long menvcfg_val;

	if (sbi_hart_priv_version(scratch) >= SBI_HART_PRIV_VER_1_12) {
		menvcfg_val = csr_read(CSR_MENVCFG);

		/*
		 * Set menvcfg.CBZE == 1
		 *
		 * If Zicboz extension is not available then writes to
		 * menvcfg.CBZE will be ignored because it is a WARL field.
		 */
		menvcfg_val |= ECR_CBZE;

		/*
		 * Set menvcfg.CBCFE == 1
		 *
		 * If Zicbom extension is not available then writes to
		 * menvcfg.CBCFE will be ignored because it is a WARL field.
		 */
		menvcfg_val |= ECR_CBCFE;

		/*
		 * Set menvcfg.CBIE == 3
		 *
		 * If Zicbom extension is not available then writes to
		 * menvcfg.CBIE will be ignored because it is a WARL field.
		 */
		menvcfg_val |= ECR_CBIE;

		/*
		 * Set menvcfg.PBMTE == 1 for RV64 or RV128
		 *
		 * If Svpbmt extension is not available then menvcfg.PBMTE
		 * will be read-only zero.
		 */
#if __riscv_xlen > 32
		menvcfg_val |= ECR_PBMTE;
#endif

		/*
		 * The spec doesn't explicitly describe the reset value of menvcfg.
		 * Enable access to stimecmp if sstc extension is present in the
		 * hardware.
		 */
		if (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSTC)) {
#if __riscv_xlen == 32
			unsigned long menvcfgh_val;
			menvcfgh_val = csr_read(CSR_MENVCFGH);
			menvcfgh_val |= ECR_STCE;
			csr_write(CSR_MENVCFGH, menvcfgh_val);
#else
			menvcfg_val |= ECR_STCE;
#endif
		}

		csr_write(CSR_MENVCFG, menvcfg_val);
	}

}

#ifdef CONFIG_RISCV_SMSTATEEN
static void mstateen_init(struct sbi_scratch *scratch, uint32_t hartid)
{
	uint64_t mstateen_val;

	if (sbi_hart_has_extension(scratch, SBI_HART_EXT_SMSTATEEN)) {
		mstateen_val = csr_read(CSR_MSTATEEN(0));
#if __riscv_xlen == 32
		mstateen_val |= ((uint64_t)csr_read(CSR_MSTATEENH(0))) << 32;
#endif
		mstateen_val |= SER_STATEEN;
		mstateen_val |= SER_CONTEXT;
		mstateen_val |= SER_HSENVCFG;

		if (sbi_hart_has_extension(scratch, SBI_HART_EXT_SMAIA))
			mstateen_val |= (SER_AIA | SER_SVSLCT | SER_IMSIC);
		else
			mstateen_val &= ~(SER_AIA | SER_SVSLCT | SER_IMSIC);
		csr_write(CSR_MSTATEEN(0), mstateen_val);
#if __riscv_xlen == 32
		csr_write(CSR_MSTATEENH(0), mstateen_val >> 32);
#endif
	}
}
#else
static void mstateen_init(struct sbi_scratch *scratch, uint32_t hartid)
{
}
#endif

static void mstatus_init(struct sbi_scratch *scratch, uint32_t hartid)
{
	/* Enable FPU */
#if defined(CONFIG_CPU_F) || defined(CONFIG_CPU_D)
	if (misa_extension('D') || misa_extension('F'))
		csr_set(CSR_MSTATUS, SR_FS);
#endif

#ifdef CONFIG_CPU_V
	if (misa_extension('V'))
		csr_set(CSR_MSTATUS, SR_VS);
#endif

#ifdef CONFIG_CPU_S
	/* Disable user mode usage of all perf counters except default ones (CY, TM, IR) */
	if (misa_extension('S') &&
	    sbi_hart_priv_version(scratch) >= SBI_HART_PRIV_VER_1_10)
		csr_set(CSR_SCOUNTEREN, 7);
#endif
	/**
	 * OpenSBI doesn't use any PMU counters in M-mode.
	 * Supervisor mode usage for all counters are enabled by default
	 * But counters will not run until mcountinhibit is set.
	 */
	if (sbi_hart_priv_version(scratch) >= SBI_HART_PRIV_VER_1_10)
		csr_write(CSR_MCOUNTEREN, -1);

	/* All programmable counters will start running at runtime after S-mode request */
	if (sbi_hart_priv_version(scratch) >= SBI_HART_PRIV_VER_1_11)
		csr_write(CSR_MCOUNTINHIBIT, 0xFFFFFFF8);

	/* Disable all interrupts */
	csr_write(CSR_MIE, 0);

#ifdef CONFIG_CPU_S
	/* Disable S-mode paging */
	if (misa_extension('S'))
		csr_write(CSR_SATP, 0);
#endif
}

#if defined(CONFIG_SBI_RISCV_F) || defined(CONFIG_SBI_RISCV_D)
static int fp_init(uint32_t hartid)
{
	int i;

	if (!misa_extension('D') && !misa_extension('F'))
		return 0;

	if (!(csr_read(CSR_MSTATUS) & SR_FS))
		return -EINVAL;

	for (i = 0; i < 32; i++)
		init_fp_reg(i);
	csr_write(CSR_FCSR, 0);
	return 0;
}
#else
static int fp_init(uint32_t hartid)
{
	unsigned long fd_mask;

	if (!misa_extension('D') && !misa_extension('F'))
		return 0;

	if (!(csr_read(CSR_MSTATUS) & SR_FS))
		return -EINVAL;

	fd_mask = (1 << ('F' - 'A')) | (1 << ('D' - 'A'));
	csr_clear(CSR_MISA, fd_mask);
	if (csr_read(CSR_MISA) & fd_mask)
		return -ENOTSUP;
	return 0;
}
#endif

#ifdef CONFIG_SBI_LDST_MISALIGNED_DELEG
#define EXC_LDST_MISALIGNED_DELEG	((1U << EXC_LOAD_MISALIGNED) | \
					 (1U << EXC_STORE_MISALIGNED))
#else
#define EXC_LDST_MISALIGNED_DELEG	0
#endif

static int delegate_traps(struct sbi_scratch *scratch, uint32_t hartid)
{
	const struct sbi_platform *plat = sbi_platform_ptr(scratch);
	unsigned long interrupts, exceptions;

	if (!misa_extension('S'))
		/* No delegation possible as mideleg does not exist*/
		return 0;

	/* Send M-mode interrupts and most exceptions to S-mode */
	interrupts = IR_SSI | IR_STI | IR_SEI;
	if (sbi_hart_has_extension(scratch, SBI_HART_EXT_SSCOFPMF))
		interrupts |= MIP_LCOFIP;

	exceptions = (1U << EXC_INSN_MISALIGNED) | (1U << EXC_BREAKPOINT) |
		     (1U << EXC_ECALL_U) | EXC_LDST_MISALIGNED_DELEG;
	if (sbi_platform_has_mfaults_delegation(plat))
		exceptions |= (1U << EXC_INSN_PAGE_FAULT) |
			      (1U << EXC_LOAD_PAGE_FAULT) |
			      (1U << EXC_STORE_PAGE_FAULT);

	/*
	 * If hypervisor extension available then we only handle hypervisor
	 * calls (i.e. ecalls from HS-mode) in M-mode.
	 *
	 * The HS-mode will additionally handle supervisor calls (i.e. ecalls
	 * from VS-mode), Guest page faults and Virtual interrupts.
	 */
	if (misa_extension('H')) {
		exceptions |= (1U << CAUSE_VIRTUAL_SUPERVISOR_ECALL);
		exceptions |= (1U << CAUSE_FETCH_GUEST_PAGE_FAULT);
		exceptions |= (1U << CAUSE_LOAD_GUEST_PAGE_FAULT);
		exceptions |= (1U << CAUSE_VIRTUAL_INST_FAULT);
		exceptions |= (1U << CAUSE_STORE_GUEST_PAGE_FAULT);
	}

	csr_write(CSR_MIDELEG, interrupts);
	csr_write(CSR_MEDELEG, exceptions);

	if ((csr_read(CSR_MIDELEG) & interrupts) != interrupts)
		return -ENODEV;
	if ((csr_read(CSR_MEDELEG) & exceptions) != exceptions)
		return -ENODEV;
	return 0;
}

unsigned int sbi_hart_mhpm_count(struct sbi_scratch *scratch)
{
	struct sbi_hart_features *hfeatures =
			sbi_scratch_offset_ptr(scratch, hart_features_offset);

	return hfeatures->mhpm_count;
}

unsigned int sbi_hart_mhpm_bits(struct sbi_scratch *scratch)
{
	struct sbi_hart_features *hfeatures =
		sbi_scratch_offset_ptr(scratch, hart_features_offset);

	return hfeatures->mhpm_bits;
}

int sbi_hart_pmp_check_addr(struct sbi_scratch *scratch, unsigned long addr,
			    unsigned long attr)
{
	unsigned long prot, size, l2l, i;
	phys_addr_t tempaddr;
	const struct sbi_platform *plat = sbi_platform_ptr(scratch);

	if (!sbi_platform_has_pmp(plat))
		return SBI_OK;

	for (i = 0; i < PMP_COUNT; i++) {
		pmp_get(i, &prot, &tempaddr, &l2l);
		if (!(prot & PMP_A))
			continue;
		if (l2l < __riscv_xlen)
			size = (1UL << l2l);
		else
			size = 0;
		if (tempaddr <= addr && addr <= tempaddr + size)
			if (!(prot & attr))
				return SBI_EINVALID_ADDR;
	}

	return SBI_OK;
}

#ifdef CONFIG_RISCV_PMP
static int pmp_init(struct sbi_scratch *scratch, uint32_t hartid)
{
	uint32_t i, count;
	unsigned long prot, log2size;
	phys_addr_t addr;
	__unused unsigned long fw_start;
	const struct sbi_platform *plat = sbi_platform_ptr(scratch);

#define PMP_PROT_FW	(PMP_W | PMP_R | PMP_X)

	log2size = __ilog2_u64(__roundup64(scratch->fw_size));
	fw_start = scratch->fw_start & ~((UL(1) << log2size) - 1);

	pmp_set(0, PMP_PROT_FW, fw_start, log2size);

	count = sbi_platform_pmp_region_count(plat, hartid);
	if ((PMP_COUNT - 1) < count)
		count = (PMP_COUNT - 1);

	for (i = 0; i < count; i++) {
		if (sbi_platform_pmp_region_info(plat, hartid, i, &prot,
						 (unsigned long *)&addr,
						 &log2size))
			continue;
		pmp_set(i + 1, prot, addr, log2size);
	}
	return 0;
}
#else
static int pmp_init(struct sbi_scratch *scratch, uint32_t hartid)
{
	return 0;
}
#endif

#define SBI_SMEPMP_RESV_ENTRY           0

unsigned int sbi_hart_pmp_log2gran(struct sbi_scratch *scratch)
{
	struct sbi_hart_features *hfeatures =
			sbi_scratch_offset_ptr(scratch, hart_features_offset);

	return hfeatures->pmp_log2gran;
}

int sbi_hart_map_saddr(unsigned long addr, unsigned long size)
{
	/* shared R/W access for M and S/U mode */
	unsigned int pmp_flags = (PMP_W | PMP_X);
	unsigned long order, base = 0;
	struct sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	/* If Smepmp is not supported no special mapping is required */
	if (!sbi_hart_has_extension(scratch, SBI_HART_EXT_SMEPMP))
		return SBI_OK;

	if (is_pmp_entry_mapped(SBI_SMEPMP_RESV_ENTRY))
		return SBI_ENOSPC;

	for (order = MAX(sbi_hart_pmp_log2gran(scratch), log2roundup(size));
	     order <= __riscv_xlen; order++) {
		if (order < __riscv_xlen) {
			base = addr & ~((1UL << order) - 1UL);
			if ((base <= addr) &&
			    (addr < (base + (1UL << order))) &&
			    (base <= (addr + size - 1UL)) &&
			    ((addr + size - 1UL) < (base + (1UL << order))))
				break;
		} else {
			return SBI_EFAIL;
		}
	}

	pmp_set(SBI_SMEPMP_RESV_ENTRY, pmp_flags, base, order);

	return SBI_OK;
}

int sbi_hart_unmap_saddr(void)
{
	struct sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	if (!sbi_hart_has_extension(scratch, SBI_HART_EXT_SMEPMP))
		return SBI_OK;

	return pmp_disable(SBI_SMEPMP_RESV_ENTRY);
}

int sbi_hart_priv_version(struct sbi_scratch *scratch)
{
	struct sbi_hart_features *hfeatures =
			sbi_scratch_offset_ptr(scratch, hart_features_offset);

	return hfeatures->priv_version;
}

void sbi_hart_get_priv_version_str(struct sbi_scratch *scratch,
				   char *version_str, int nvstr)
{
	char *temp;
	struct sbi_hart_features *hfeatures =
			sbi_scratch_offset_ptr(scratch, hart_features_offset);

	switch (hfeatures->priv_version) {
	case SBI_HART_PRIV_VER_1_10:
		temp = "v1.10";
		break;
	case SBI_HART_PRIV_VER_1_11:
		temp = "v1.11";
		break;
	case SBI_HART_PRIV_VER_1_12:
		temp = "v1.12";
		break;
	default:
		temp = "unknown";
		break;
	}

	snprintf(version_str, nvstr, "%s", temp);
}

static inline void __sbi_hart_update_extension(struct sbi_hart_features *hfeatures,
					       enum sbi_hart_extensions ext,
					       bool enable)
{
	if (enable)
		hfeatures->extensions |= _BV(ext);
	else
		hfeatures->extensions &= ~_BV(ext);
}

/**
 * Enable/Disable a particular hart extension
 *
 * @param scratch pointer to the HART scratch space
 * @param ext the extension number to check
 * @param enable new state of hart extension
 */
void sbi_hart_update_extension(struct sbi_scratch *scratch,
			       enum sbi_hart_extensions ext,
			       bool enable)
{
	struct sbi_hart_features *hfeatures =
			sbi_scratch_offset_ptr(scratch, hart_features_offset);

	__sbi_hart_update_extension(hfeatures, ext, enable);
}

/**
 * Check whether a particular hart extension is available
 *
 * @param scratch pointer to the HART scratch space
 * @param ext the extension number to check
 * @returns true (available) or false (not available)
 */
bool sbi_hart_has_extension(struct sbi_scratch *scratch,
			    enum sbi_hart_extensions ext)
{
	struct sbi_hart_features *hfeatures =
			sbi_scratch_offset_ptr(scratch, hart_features_offset);

	if (hfeatures->extensions & _BV(ext))
		return true;
	else
		return false;
}

static inline char *sbi_hart_extension_id2string(int ext)
{
	char *estr = NULL;

	switch (ext) {
	case SBI_HART_EXT_SMAIA:
		estr = "smaia";
		break;
	case SBI_HART_EXT_SMSTATEEN:
		estr = "smstateen";
		break;
	case SBI_HART_EXT_SSCOFPMF:
		estr = "sscofpmf";
		break;
	case SBI_HART_EXT_SSTC:
		estr = "sstc";
		break;
	case SBI_HART_EXT_ZICNTR:
		estr = "zicntr";
		break;
	case SBI_HART_EXT_ZIHPM:
		estr = "zihpm";
		break;
	case SBI_HART_EXT_SMEPMP:
		estr = "smepmp";
		break;
	default:
		break;
	}

	return estr;
}

/**
 * Get the hart extensions in string format
 *
 * @param scratch pointer to the HART scratch space
 * @param extensions_str pointer to a char array where the extensions string
 *			 will be updated
 * @param nestr length of the features_str. The feature string will be
 *		truncated if nestr is not long enough.
 */
void sbi_hart_get_extensions_str(struct sbi_scratch *scratch,
				 char *extensions_str, int nestr)
{
	struct sbi_hart_features *hfeatures =
			sbi_scratch_offset_ptr(scratch, hart_features_offset);
	int offset = 0, ext = 0;
	char *temp;

	if (!extensions_str || nestr <= 0)
		return;
	memory_set((caddr_t)extensions_str, 0, nestr);

	if (!hfeatures->extensions)
		goto done;

	do {
		if (hfeatures->extensions & _BV(ext)) {
			temp = sbi_hart_extension_id2string(ext);
			if (temp) {
				snprintf(extensions_str + offset,
					 nestr - offset, "%s,", temp);
				offset = offset + strlen(temp) + 1;
			}
		}

		ext++;
	} while (ext < SBI_HART_EXT_MAX);

done:
	if (offset)
		extensions_str[offset - 1] = '\0';
	else
		strncpy(extensions_str, "none", nestr);
}


#ifdef CONFIG_RISCV_PMP
static unsigned long hart_pmp_get_allowed_addr(void)
{
	unsigned long val = 0;
	struct csr_trap_info trap = {0};

	csr_write_allowed(CSR_PMPCFG(0), (unsigned long)&trap, 0);
	if (trap.cause)
		return 0;

	csr_write_allowed(CSR_PMPADDR(0), (unsigned long)&trap, PMP_ADDR_MASK);
	if (!trap.cause) {
		val = csr_read_allowed(CSR_PMPADDR(0), (unsigned long)&trap);
		if (trap.cause)
			val = 0;
	}

	return val;
}
#else
static unsigned long hart_pmp_get_allowed_addr(void)
{
	return 0;
}
#endif

static int hart_mhpm_get_allowed_bits(void)
{
	int num_bits = 0;
	unsigned long val = ~(UL(0));
	struct csr_trap_info trap = {0};

	/**
	 * It is assumed that platforms will implement same number of bits for
	 * all the performance counters including mcycle/minstret.
	 */
	csr_write_allowed(CSR_MHPMCOUNTER(3), (unsigned long)&trap, val);
	if (!trap.cause) {
		val = csr_read_allowed(CSR_MHPMCOUNTER(3), (unsigned long)&trap);
		if (trap.cause)
			return 0;
	}
	num_bits = __fls_unit(val) + 1;
#if __riscv_xlen == 32
	csr_write_allowed(CSR_MHPMCOUNTERH(3), (unsigned long)&trap, val);
	if (!trap.cause) {
		val = csr_read_allowed(CSR_MHPMCOUNTERH(3), (unsigned long)&trap);
		if (trap.cause)
			return num_bits;
	}
	num_bits += __fls_unit(val) + 1;

#endif

	return num_bits;
}

int hart_detect_features(struct sbi_scratch *scratch)
{
	struct csr_trap_info trap = {0};
	struct sbi_hart_features *hfeatures =
		sbi_scratch_offset_ptr(scratch, hart_features_offset);
	unsigned long val, oldval;
	__unused int rc;

	/* If hart features already detected then do nothing */
	if (hfeatures->detected)
		return 0;

	/* Clear hart features */
	hfeatures->extensions = 0;
	hfeatures->pmp_count = 0;
	hfeatures->mhpm_count = 0;

#define __check_csr(__csr, __rdonly, __wrval, __field, __skip)			\
	oldval = csr_read_allowed(__csr, (unsigned long)&trap);			\
	if (!trap.cause) {							\
		if (__rdonly) {							\
			(hfeatures->__field)++;					\
		} else {							\
			csr_write_allowed(__csr, (unsigned long)&trap, __wrval);\
			if (!trap.cause) {					\
				if (csr_swap(__csr, oldval) == __wrval)		\
					(hfeatures->__field)++;			\
				else						\
					goto __skip;				\
			} else {						\
				goto __skip;					\
			}							\
		}								\
	} else {								\
		goto __skip;							\
	}
#define __check_csr_2(__csr, __rdonly, __wrval, __field, __skip)	\
	__check_csr(__csr + 0, __rdonly, __wrval, __field, __skip)	\
	__check_csr(__csr + 1, __rdonly, __wrval, __field, __skip)
#define __check_csr_4(__csr, __rdonly, __wrval, __field, __skip)	\
	__check_csr_2(__csr + 0, __rdonly, __wrval, __field, __skip)	\
	__check_csr_2(__csr + 2, __rdonly, __wrval, __field, __skip)
#define __check_csr_8(__csr, __rdonly, __wrval, __field, __skip)	\
	__check_csr_4(__csr + 0, __rdonly, __wrval, __field, __skip)	\
	__check_csr_4(__csr + 4, __rdonly, __wrval, __field, __skip)
#define __check_csr_16(__csr, __rdonly, __wrval, __field, __skip)	\
	__check_csr_8(__csr + 0, __rdonly, __wrval, __field, __skip)	\
	__check_csr_8(__csr + 8, __rdonly, __wrval, __field, __skip)
#define __check_csr_32(__csr, __rdonly, __wrval, __field, __skip)	\
	__check_csr_16(__csr + 0, __rdonly, __wrval, __field, __skip)	\
	__check_csr_16(__csr + 16, __rdonly, __wrval, __field, __skip)
#define __check_csr_64(__csr, __rdonly, __wrval, __field, __skip)	\
	__check_csr_32(__csr + 0, __rdonly, __wrval, __field, __skip)	\
	__check_csr_32(__csr + 32, __rdonly, __wrval, __field, __skip)

	/**
	 * Detect the allowed address bits & granularity. At least PMPADDR0
	 * should be implemented.
	 */
	val = hart_pmp_get_allowed_addr();
	if (val) {
		hfeatures->pmp_gran =  1 << (__ffs_unit(val) + 2);
		hfeatures->pmp_addr_bits = __fls_unit(val) + 1;
		/* Detect number of PMP regions. At least PMPADDR0 should be implemented*/
		__check_csr_64(CSR_PMPADDR(0), 0, val, pmp_count, __pmp_skip);
	}
__pmp_skip:

	/* Detect number of MHPM counters */
	__check_csr(CSR_MHPMCOUNTER(3), 0, 1UL, mhpm_count, __mhpm_skip);
	hfeatures->mhpm_bits = hart_mhpm_get_allowed_bits();

	__check_csr_4(CSR_MHPMCOUNTER(4), 0, 1UL, mhpm_count, __mhpm_skip);
	__check_csr_8(CSR_MHPMCOUNTER(8), 0, 1UL, mhpm_count, __mhpm_skip);
	__check_csr_16(CSR_MHPMCOUNTER(16), 0, 1UL, mhpm_count, __mhpm_skip);

	/**
	 * No need to check for MHPMCOUNTERH for RV32 as they are expected to be
	 * implemented if MHPMCOUNTER is implemented.
	 */

__mhpm_skip:

#undef __check_csr_64
#undef __check_csr_32
#undef __check_csr_16
#undef __check_csr_8
#undef __check_csr_4
#undef __check_csr_2
#undef __check_csr

	/* Detect if hart supports Priv v1.10 */
	val = csr_read_allowed(CSR_MCOUNTEREN, (unsigned long)&trap);
	if (!trap.cause)
		hfeatures->priv_version = SBI_HART_PRIV_VER_1_10;

	/* Detect if hart supports Priv v1.11 */
	val = csr_read_allowed(CSR_MCOUNTINHIBIT, (unsigned long)&trap);
	if (!trap.cause &&
	    (hfeatures->priv_version >= SBI_HART_PRIV_VER_1_10))
		hfeatures->priv_version = SBI_HART_PRIV_VER_1_11;

	/* Detect if hart supports Priv v1.12 */
	csr_read_allowed(CSR_MENVCFG, (unsigned long)&trap);
	if (!trap.cause &&
	    (hfeatures->priv_version >= SBI_HART_PRIV_VER_1_11))
		hfeatures->priv_version = SBI_HART_PRIV_VER_1_12;

#ifdef CONFIG_RISCV_SSCOFPMF
	/* Counter overflow/filtering is not useful without mcounter/inhibit */
	if (hfeatures->priv_version >= SBI_HART_PRIV_VER_1_11) {
		/* Detect if hart supports sscofpmf */
		csr_read_allowed(CSR_SCOUNTOVF, (unsigned long)&trap);
		if (!trap.cause)
			__sbi_hart_update_extension(hfeatures,
					SBI_HART_EXT_SSCOFPMF, true);
	}
#endif

	/* Detect if hart supports time CSR */
	csr_read_allowed(CSR_TIME, (unsigned long)&trap);
	if (!trap.cause)
		__sbi_hart_update_extension(hfeatures,
					SBI_HART_EXT_ZICNTR, true);

#ifdef CONFIG_RISCV_SMAIA
	/* Detect if hart has AIA local interrupt CSRs */
	csr_read_allowed(CSR_MTOPI, (unsigned long)&trap);
	if (!trap.cause)
		__sbi_hart_update_extension(hfeatures,
					SBI_HART_EXT_SMAIA, true);
#endif

#ifdef CONFIG_RISCV_SSTC
	/* Detect if hart supports stimecmp CSR(Sstc extension) */
	if (hfeatures->priv_version >= SBI_HART_PRIV_VER_1_12) {
		csr_read_allowed(CSR_STIMECMP, (unsigned long)&trap);
		if (!trap.cause) {
			__sbi_hart_update_extension(hfeatures,
					SBI_HART_EXT_SSTC, true);
		}
	}
#endif

#ifdef CONFIG_RISCV_SMSTATEEN
	/* Detect if hart supports mstateen CSRs */
	if (hfeatures->priv_version >= SBI_HART_PRIV_VER_1_12) {
		val = csr_read_allowed(CSR_MSTATEEN(0), (unsigned long)&trap);
		if (!trap.cause)
			__sbi_hart_update_extension(hfeatures,
					SBI_HART_EXT_SMSTATEEN, true);
	}
#endif

#if 0
	/* Let platform populate extensions */
	rc = sbi_platform_extensions_init(sbi_platform_thishart_ptr(),
					  hfeatures);
	if (rc)
		return rc;
#endif

	/* Extensions implied by other extensions and features */
	if (hfeatures->mhpm_count)
		__sbi_hart_update_extension(hfeatures,
					SBI_HART_EXT_ZIHPM, true);

	/* Mark hart feature detection done */
	hfeatures->detected = true;

	return 0;
}

int sbi_hart_init(struct sbi_scratch *scratch, uint32_t hartid, bool cold_boot)
{
	int rc;

	if (cold_boot) {
		csr_init();

		trap_info_offset = sbi_scratch_alloc_offset(__SIZEOF_POINTER__,
							    "HART_TRAP_INFO");
		if (!trap_info_offset)
			return -ENOMEM;

		hart_features_offset = sbi_scratch_alloc_offset(
			sizeof(struct sbi_hart_features), "HART_FEATURES");
		if (!hart_features_offset)
			return -ENOMEM;
	}

	rc = hart_detect_features(scratch);
	if (rc)
		return rc;

	mstatus_init(scratch, hartid);
	mstateen_init(scratch, hartid);
	menvcfg_init(scratch, hartid);

	rc = fp_init(hartid);
	if (rc)
		return rc;

	rc = delegate_traps(scratch, hartid);
	if (rc)
		return rc;

	return pmp_init(scratch, hartid);
}

void *sbi_hart_get_trap_info(struct sbi_scratch *scratch)
{
	unsigned long *trap_info;

	if (!trap_info_offset)
		return NULL;

	trap_info = sbi_scratch_offset_ptr(scratch, trap_info_offset);

	return (void *)(*trap_info);
}

void sbi_hart_set_trap_info(struct sbi_scratch *scratch, void *data)
{
	unsigned long *trap_info;

	if (!trap_info_offset)
		return;

	trap_info = sbi_scratch_offset_ptr(scratch, trap_info_offset);
	*trap_info = (unsigned long)data;
}

#ifdef CONFIG_SBI_RISCV_S
static void sbi_switch_s_mode(unsigned long next_addr)
{
	irq_set_stvec(next_addr);
	csr_write(CSR_SSCRATCH, 0);
	csr_write(CSR_SIE, 0);
	csr_write(CSR_SATP, 0);
	sbi_mode_switched = PRV_S;
}
#else
#define sbi_switch_s_mode(next_addr)	do { } while (0)
#endif

#ifdef CONFIG_SBI_RISCV_U
static void sbi_switch_u_mode(unsigned long next_addr)
{
	irq_set_utvec(next_addr);
	csr_write(CSR_USCRATCH, 0);
	csr_write(CSR_UIE, 0);
	sbi_mode_switched = PRV_U;
}
#else
#define sbi_switch_u_mode(next_addr)	do { } while (0)
#endif

__noreturn void
sbi_hart_switch_mode(unsigned long arg0, unsigned long arg1,
		     unsigned long next_addr, unsigned long next_mode)
{
	unsigned long val;

	switch (next_mode) {
	case PRV_M:
		break;
	case PRV_S:
		if (!misa_extension('S'))
			bh_panic();
		break;
	case PRV_U:
		if (!misa_extension('U'))
			bh_panic();
		break;
	default:
		bh_panic();
	}

	val = csr_read(CSR_MSTATUS);
	val = INSERT_FIELD(val, SR_MPP, next_mode);
	val = INSERT_FIELD(val, SR_MPIE, 0);

	csr_write(CSR_MSTATUS, val);
	csr_write(CSR_MEPC, next_addr);

	if (next_mode == PRV_S)
		sbi_switch_s_mode(next_addr);
	else if (next_mode == PRV_U)
		sbi_switch_u_mode(next_addr);

	register unsigned long a0 asm("a0") = arg0;
	register unsigned long a1 asm("a1") = arg1;
	__asm__ __volatile__("mret" : : "r"(a0), "r"(a1));
	__builtin_unreachable();
}

DEFINE_SPINLOCK(avail_hart_mask_lock);
static volatile unsigned long avail_hart_mask = 0;

void sbi_hart_mark_available(uint32_t hartid)
{
	spin_lock(&avail_hart_mask_lock);
	avail_hart_mask |= (1UL << hartid);
	spin_unlock(&avail_hart_mask_lock);
}

void sbi_hart_unmark_available(uint32_t hartid)
{
	spin_lock(&avail_hart_mask_lock);
	avail_hart_mask &= ~(1UL << hartid);
	spin_unlock(&avail_hart_mask_lock);
}

unsigned long sbi_hart_available_mask(void)
{
	unsigned long ret;

	spin_lock(&avail_hart_mask_lock);
	ret = avail_hart_mask;
	spin_unlock(&avail_hart_mask_lock);

	return ret;
}

typedef struct sbi_scratch *(*h2s)(unsigned long hartid);

struct sbi_scratch *sbi_hart_id_to_scratch(struct sbi_scratch *scratch,
					   uint32_t hartid)
{
	return ((h2s)scratch->hartid_to_scratch)(hartid);
}

struct sbi_scratch *sbi_hartid_to_scratch(uint32_t hartid)
{
	extern struct sbi_scratch * _hartid_to_scratch(uint32_t hartid);
	return _hartid_to_scratch(hartid);
}
