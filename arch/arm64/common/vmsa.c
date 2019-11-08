#include <target/paging.h>

extern void cpu_do_switch_pgd(phys_addr_t pgd_phys, unsigned long asid);

static void cpu_set_reserved_ttbr_user(void)
{
	phys_addr_t ttbr = phys_to_ttbr(__pa_symbol(empty_zero_page));
	write_sysreg(ttbr, TTBR_USER);
	isb();
}

static void cpu_switch_pgd(pgd_t *pgd, unsigned long asid)
{
	BUG_ON(pgd == mmu_pg_dir);
	cpu_set_reserved_ttbr_user();
	cpu_do_switch_pgd(__pa_symbol(pgd), asid);
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
	       (void *)__pa_symbol(idmap_cpu_replace_ttbr1);

	cpu_install_idmap();
	replace_phys(phys_to_ttbr(virt_to_phys(ttbr)));
	cpu_uninstall_idmap();
}
