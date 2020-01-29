#include <target/init.h>
#include <target/arch.h>
#include <target/paging.h>

pgd_t mmu_id_map[IDMAP_DIR_SIZE / sizeof (pgd_t)] __page_aligned_bss;

/* mmu_hw_boot_init() is called from head.S with MMU-off.
 * Following requirements should be honoured for setup_vm() to work
 * correctly:
 * 1) It should use PC-relative addressing for accessing kernel symbols.
 *    To achieve this we always use GCC cmodel=medany.
 * 2) The compiler instrumentation for FTRACE will not work for setup_vm()
 *    so disable compiler instrumentation when FTRACE is enabled.
 * Currently, the above requirements are honoured by using custom CFLAGS
 * for init.o in mm/Makefile.
 */
#ifndef __riscv_cmodel_medany
#error "setup_vm() is called from head.S before relocate so it should "
	"not use absolute addressing."
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

__init void *bpgt_create_idmap_dir(void *tbl, caddr_t virt)
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

__init void bpgt_create_idmap_blk(void *tbl, caddr_t start, caddr_t end)
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

void __init mmu_hw_boot_init(void)
{
	void *tbl = mmu_id_map;
	unsigned long satp;

	printf("Early  MAP: %016llx - %016llx\n", SDFIRM_START, SDFIRM_END);
	memset((void *)mmu_id_map, 0, IDMAP_DIR_SIZE);
	tbl = bpgt_create_idmap_dir(tbl, SDFIRM_START);
	bpgt_create_idmap_blk(tbl, SDFIRM_START, SDFIRM_END);
#if defined(CONFIG_MMU_IDMAP_DEVICE) && defined(IDMAP_DEV_BASE)
	/* TODO: Map identity device area for early console */
#endif
	satp = PFN_DOWN(__pa(mmu_id_map)) | SATP_MODE;
	printf("Early SATP: %016llx\n", satp);
	csr_write(CSR_SATP, satp);
	local_flush_tlb_all();
}

void mmu_hw_ctrl_init(void)
{
}
