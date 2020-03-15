#include <target/init.h>
#include <target/arch.h>
#include <target/paging.h>
#include <target/console.h>

pgd_t mmu_id_map[IDMAP_DIR_SIZE / sizeof (pgd_t)] __page_aligned_bss;

/* bpgt_init() is called from head.S with MMU-off.
 * Following requirements should be honoured for bpgt_init() to work
 * correctly:
 * 1) It should use PC-relative addressing for accessing kernel symbols.
 *    To achieve this we always use GCC cmodel=medany.
 * 2) The compiler instrumentation for FTRACE will not work for
 *    bpgt_init() so disable compiler instrumentation when FTRACE is
 *    enabled.
 * Currently, the above requirements are honoured by using custom CFLAGS
 * for init.o in mm/Makefile.
 */
#ifndef __riscv_cmodel_medany
#error "bpgt_init() called before relocation must be PIC!"
#endif

#define bpgt_index(virt, shift, ptrs)	(((virt) >> (shift)) & ((ptrs) - 1))
#define bpgt_value(phys, attr)						\
	(((phys) >> (PAGE_SHIFT)) << (_PAGE_PFN_SHIFT) | (attr))
#define set_bpgt_dir(tbl, type, virt, phys, shift, ptrs)		\
	do {								\
		type pte = bpgt_value(phys, BPGT_PMD_FLAGS);		\
		int index = bpgt_index((virt), (shift),			\
				       PTRS_PER_##ptrs);		\
		type *ptr = (type *)(tbl) + index;			\
		mmu_dbg_tbl("Early  " #ptrs ": %016llx : %016llx\n",	\
			    ptr, pte);					\
		*ptr = pte;						\
	} while (0)

__init static void *bpgt_create_dir(void *tbl, caddr_t virt)
{
	phys_addr_t phys = __pa(tbl);

	phys += PAGE_SIZE;
	set_bpgt_dir(tbl, pgd_t, virt, phys, PGDIR_SHIFT, PGD);
#if BPGT_PGTABLE_LEVELS > 2
	tbl += PAGE_SIZE;
	phys += PAGE_SIZE;
	set_bpgt_dir(tbl, pud_t, virt, phys, PUD_SHIFT, PUD);
#endif
#if BPGT_PGTABLE_LEVELS > 3
	tbl += PAGE_SIZE;
	phys += PAGE_SIZE;
	set_bpgt_dir(tbl, pmd_t, virt, phys, PMD_SHIFT, PMD);
#endif
	tbl += PAGE_SIZE;
	return tbl;
}

__init static void bpgt_create_blk(void *tbl, caddr_t start, caddr_t end)
{
	phys_addr_t phys = __pa(start);
	int start_pfn = bpgt_index(start, BPGT_BLOCK_SHIFT, PTRS_PER_PTE);
	int end_pfn = bpgt_index(end + BPGT_BLOCK_SIZE - 1,
				 BPGT_BLOCK_SHIFT, PTRS_PER_PTE);
	pte_t pte;

	while (start_pfn < end_pfn) {
		pte = bpgt_value(phys, BPGT_MM_MMUFLAGS);
		mmu_dbg_tbl("Early  PTE: %016llx : %016llx\n",
			    &(((pte_t *)tbl)[start_pfn]), pte);
		((pte_t *)tbl)[start_pfn] = pte;
		start_pfn++;
		phys += BPGT_BLOCK_SIZE;
	}
}

__init static void sv_satp_switch(caddr_t map, const char *hint)
{
	unsigned long satp;

	satp = PFN_DOWN(__pa(map)) | SATP_MODE;
	printf("%s: %016llx\n", hint, satp);
	csr_write(CSR_SATP, satp);
}

__init void bpgt_init(void)
{
	void *tbl = mmu_id_map;

	printf("Early  MAP: %016llx - %016llx\n", SDFIRM_START, SDFIRM_END);
	tbl = bpgt_create_dir(tbl, SDFIRM_START);
	bpgt_create_blk(tbl, SDFIRM_START, SDFIRM_END);
#if defined(CONFIG_MMU_IDMAP_DEVICE) && defined(IDMAP_DEV_BASE)
	/* TODO: Map identity device area for early console */
#endif
}

bool mmu_hw_pgattr_safe(pteval_t old, pteval_t new)
{
	/* The following mapping attributes may be updated in live
	 * kernel mappings without the need for break-before-mak.
	 */
	static const pteval_t mask = _PAGE_READ | _PAGE_WRITE | _PAGE_EXEC;
	if (old == 0 || new == 0)
		return true;
	return ((old ^ new) & ~mask) == 0;
}

__init void mmu_hw_ctrl_init(void)
{
	/* TODO: relocate to PAGE_OFFSET */
	sv_satp_switch((caddr_t)mmu_pg_dir, "SATP");
}
