#include <target/paging.h>
#include <target/panic.h>

bool mmu_hw_pgattr_safe(pteval_t old, pteval_t new)
{
	/* The following mapping attributes may be updated in live
	 * kernel mappings without the need for break-before-mak.
	 */
	static const pteval_t mask = PTE_PXN | PTE_RDONLY | PTE_WRITE | PTE_NG;
	if (old == 0 || new == 0)
		return true;
	if ((old | new) & PTE_CONT)
		return false;
	if (old & ~new & PTE_NG)
		return false;
	return ((old ^ new) & ~mask) == 0;
}

extern void cpu_do_switch_pgd(phys_addr_t pgd_phys, unsigned long asid);

static void cpu_set_reserved_ttbr_user(void)
{
	phys_addr_t ttbr = phys_to_ttbr(__pa(empty_zero_page));
	write_sysreg(ttbr, TTBR_USER);
	isb();
}

static void cpu_switch_pgd(pgd_t *pgd, unsigned long asid)
{
	BUG_ON(pgd == mmu_pg_dir);
	cpu_set_reserved_ttbr_user();
	cpu_do_switch_pgd(__pa(pgd), asid);
}

static void cpu_uninstall_idmap(void)
{
	cpu_set_reserved_ttbr_user();
	local_flush_tlb_all();
}

static void cpu_install_idmap(void)
{
	cpu_set_reserved_ttbr_user();
	local_flush_tlb_all();
	cpu_switch_pgd(mmu_id_map, 0);
}

void cpu_replace_ttbr1(caddr_t ttbr)
{
	ttbr_replace_func *replace_phys =
	       (void *)__pa(idmap_cpu_replace_ttbr1);

	cpu_install_idmap();
	replace_phys(phys_to_ttbr(virt_to_phys(ttbr)));
	cpu_uninstall_idmap();
}
