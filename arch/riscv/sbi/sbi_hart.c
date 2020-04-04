/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#include <target/sbi.h>

static uint8_t sbi_mode_switched = PRV_M;

/**
 * Return HART ID of the caller.
 */
unsigned int sbi_current_hartid()
{
	return (u32)csr_read(CSR_MHARTID);
}

static void mstatus_init(struct sbi_scratch *scratch, u32 hartid)
{
	const struct sbi_platform *plat = sbi_platform_ptr(scratch);

	/* Enable FPU */
	if (misa_extension('D') || misa_extension('F'))
		csr_write(CSR_MSTATUS, SR_FS);

	/* Enable user/supervisor use of perf counters */
#ifndef CONFIG_ARCH_HAS_NOSEE
	if (misa_extension('S') && sbi_platform_has_scounteren(plat))
		csr_write(CSR_SCOUNTEREN, -1);
#endif
	if (sbi_platform_has_mcounteren(plat))
		csr_write(CSR_MCOUNTEREN, -1);

	/* Disable all interrupts */
	csr_write(CSR_MIE, 0);

#ifndef CONFIG_ARCH_HAS_NOSEE
	/* Disable S-mode paging */
	if (misa_extension('S'))
		csr_write(CSR_SATP, 0);
#endif
}

#if defined(CONFIG_RISCV_F) || defined(CONFIG_RISCV_D)
static int fp_init(u32 hartid)
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
static int fp_init(u32 hartid)
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

static int delegate_traps(struct sbi_scratch *scratch, u32 hartid)
{
	const struct sbi_platform *plat = sbi_platform_ptr(scratch);
	unsigned long interrupts, exceptions;

	if (!misa_extension('S'))
		/* No delegation possible as mideleg does not exist*/
		return 0;

	/* Send M-mode interrupts and most exceptions to S-mode */
	interrupts = IR_SSI | IR_STI | IR_SEI;
	exceptions = (1U << EXC_INSN_MISALIGNED) | (1U << EXC_BREAKPOINT) |
		     (1U << EXC_ECALL_U);
	if (sbi_platform_has_mfaults_delegation(plat))
		exceptions |= (1U << EXC_INSN_PAGE_FAULT) |
			      (1U << EXC_LOAD_PAGE_FAULT) |
			      (1U << EXC_STORE_PAGE_FAULT);

	csr_write(CSR_MIDELEG, interrupts);
	csr_write(CSR_MEDELEG, exceptions);

	if (csr_read(CSR_MIDELEG) != interrupts)
		return -ENODEV;
	if (csr_read(CSR_MEDELEG) != exceptions)
		return -ENODEV;
	return 0;
}

#ifdef CONFIG_RISCV_PMP
static int pmp_init(struct sbi_scratch *scratch, u32 hartid)
{
	u32 i, count;
	ulong prot, log2size;
	phys_addr_t addr;
	__unused unsigned long fw_start;
	const struct sbi_platform *plat = sbi_platform_ptr(scratch);

	if (!sbi_platform_has_pmp(plat))
		return 0;

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
static int pmp_init(struct sbi_scratch *scratch, u32 hartid)
{
	return 0;
}
#endif

static unsigned long trap_info_offset;

int sbi_hart_init(struct sbi_scratch *scratch, u32 hartid, bool cold_boot)
{
	int rc;

	if (cold_boot) {
		trap_info_offset = sbi_scratch_alloc_offset(__SIZEOF_POINTER__,
							    "HART_TRAP_INFO");
		if (!trap_info_offset)
			return -ENOMEM;
	}

	mstatus_init(scratch, hartid);

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

#ifndef CONFIG_ARCH_HAS_NOSEE
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

#ifdef CONFIG_RISCV_N
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

void __attribute__((noreturn))
sbi_hart_switch_mode(unsigned long arg0, unsigned long arg1,
		     unsigned long next_addr, unsigned long next_mode)
{
	unsigned long val;

	switch (next_mode) {
	case PRV_M:
		break;
	case PRV_S:
		if (!misa_extension('S'))
			hart_hang();
		break;
	case PRV_U:
		if (!misa_extension('U'))
			hart_hang();
		break;
	default:
		hart_hang();
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

void sbi_hart_mark_available(u32 hartid)
{
	spin_lock(&avail_hart_mask_lock);
	avail_hart_mask |= (1UL << hartid);
	spin_unlock(&avail_hart_mask_lock);
}

void sbi_hart_unmark_available(u32 hartid)
{
	spin_lock(&avail_hart_mask_lock);
	avail_hart_mask &= ~(1UL << hartid);
	spin_unlock(&avail_hart_mask_lock);
}

ulong sbi_hart_available_mask(void)
{
	ulong ret;

	spin_lock(&avail_hart_mask_lock);
	ret = avail_hart_mask;
	spin_unlock(&avail_hart_mask_lock);

	return ret;
}

typedef struct sbi_scratch *(*h2s)(ulong hartid);

struct sbi_scratch *sbi_hart_id_to_scratch(struct sbi_scratch *scratch,
					   u32 hartid)
{
	return ((h2s)scratch->hartid_to_scratch)(hartid);
}

#define COLDBOOT_WAIT_BITMAP_SIZE __riscv_xlen
DEFINE_SPINLOCK(coldboot_wait_bitmap_lock);
static unsigned long coldboot_wait_bitmap   = 0;

void sbi_cpu_wait_for_coldboot(struct sbi_scratch *scratch, u32 cpu)
{
	unsigned long mipval;

	if (cpu >= NR_CPUS ||
	    (COLDBOOT_WAIT_BITMAP_SIZE <= cpu))
		hart_hang();

	/* Set MSIE bit to receive IPI */
	csr_set(CSR_MIE, IR_MSI);

	do {
		spin_lock(&coldboot_wait_bitmap_lock);
		coldboot_wait_bitmap |= (1UL << cpu);
		spin_unlock(&coldboot_wait_bitmap_lock);

		wfi();
		mipval = csr_read(CSR_MIP);

		spin_lock(&coldboot_wait_bitmap_lock);
		coldboot_wait_bitmap &= ~(1UL << cpu);
		spin_unlock(&coldboot_wait_bitmap_lock);
	} while (!(mipval && IR_MSI));

	csr_clear(CSR_MIP, IR_MSI);
}

void sbi_cpu_wake_coldboot_cpus(struct sbi_scratch *scratch, u32 cpu)
{
	const struct sbi_platform *plat = sbi_platform_ptr(scratch);

	for (int i = 0; i < NR_CPUS; i++) {
		/* send an IPI to every other hart */
		spin_lock(&coldboot_wait_bitmap_lock);
		if ((i != cpu) && (coldboot_wait_bitmap & (1UL << i)))
			sbi_platform_ipi_send(plat, i);
		spin_unlock(&coldboot_wait_bitmap_lock);
	}
}
