#include <target/init.h>
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
#define bpgt_value(phys, attr)		((phys) | (attr))
#define set_bpgt_dir(tbl, type, virt, phys, shift, ptrs)	\
	(((type *)tbl)[bpgt_index(virt, shift, ptrs)] =		\
	 bpgt_value(phys, BPGT_PMD_FLAGS))

__init void bpgt_create_idmap_dir(void *tbl, caddr_t virt)
{
	phys_addr_t phys = __pa_symbol(virt);

	phys += PAGE_SIZE;
	set_bpgt_dir(tbl, pgd_t, virt, phys, PGDIR_SHIFT, PTRS_PER_PGD);
#if BPGT_PGTABLE_LEVELS > 3
	tbl += PAGE_SIZE;
	phys += PAGE_SIZE;
	set_bpgt_dir(tbl, pud_t, virt, phys, PUD_SHIFT, PTRS_PER_PUD);
#endif
#if BPGT_PGTABLE_LEVELS > 2
	tbl += PAGE_SIZE;
	phys += PAGE_SIZE;
	set_bpgt_dir(tbl, pmd_t, virt, phys, PMD_SHIFT, PTRS_PER_PMD);
#endif
}

__init void bpgt_create_idmap_blk(void *tbl, phys_addr_t phys,
				  caddr_t start, caddr_t end)
{
	int start_pfn = bpgt_index(start, BPGT_BLOCK_SHIFT, PTRS_PER_PTE);
	int end_pfn = bpgt_index(end, BPGT_BLOCK_SHIFT, PTRS_PER_PTE);
	pte_t pte = bpgt_value(phys, BPGT_MM_MMUFLAGS);

	while (start_pfn < end_pfn) {
		((pte_t *)tbl)[start_pfn] = pte;
		start_pfn++;
		phys += BPGT_BLOCK_SIZE;
	}
}

void __init mmu_hw_boot_init(void)
{
	uintptr_t load_pa = (uintptr_t)(&_start);
	uintptr_t load_sz = (uintptr_t)(&_end) - load_pa;

	bpgt_create_idmap_dir(mmu_id_map, PAGE_OFFSET);
	bpgt_create_idmap_blk(mmu_id_map, SDFIRM_START, load_pa, load_sz);
}

void mmu_hw_ctrl_init(void)
{
}
