#include <target/paging.h>

uint64_t idmap_t0sz = TCR_T0SZ(64 - VA_BITS);

static void cpu_set_reserved_ttbr0(void)
{
	phys_addr_t ttbr = phys_to_ttbr(__pa_symbol(empty_zero_page));
	write_sysreg(ttbr, ttbr0_el1);
	isb();
}

static void cpu_switch_pgd(pgd_t *pgd)
{
	BUG_ON(pgd == mmu_pg_dir);
	cpu_set_reserved_ttbr0();
#if 0
	cpu_do_switch_pgd(virt_to_phys(pgd));
#endif
}

#if VA_BITS == 52
static inline bool __cpu_uses_extended_idmap(void)
{
	return false;
}
#else
static inline bool __cpu_uses_extended_idmap(void)
{
	return idmap_t0sz != TCR_T0SZ(VA_BITS);
}
#endif

static void __cpu_set_tcr_t0sz(unsigned long t0sz)
{
	unsigned long tcr;

	if (!__cpu_uses_extended_idmap())
		return;

	tcr = read_sysreg(tcr_el1);
	tcr &= ~TCR_T0SZ_MASK;
	tcr |= t0sz << TCR_T0SZ_OFFSET;
	write_sysreg(tcr, tcr_el1);
	isb();
}

#define cpu_set_default_tcr_t0sz()	__cpu_set_tcr_t0sz(TCR_T0SZ(VA_BITS))
#define cpu_set_idmap_tcr_t0sz()	__cpu_set_tcr_t0sz(idmap_t0sz)

static void cpu_uninstall_idmap(void)
{
	cpu_set_reserved_ttbr0();
	local_flush_tlb_all();
	cpu_set_default_tcr_t0sz();
}

static void cpu_install_idmap(void)
{
	cpu_set_reserved_ttbr0();
	local_flush_tlb_all();
	cpu_set_idmap_tcr_t0sz();

	cpu_switch_pgd((pgd_t *)mmu_id_map);
}

void cpu_replace_ttbr1(caddr_t ttbr)
{
	pgd_t *pgdp = (pgd_t *)ttbr;
	phys_addr_t ttbr1 = phys_to_ttbr(virt_to_phys(pgdp));
	ttbr_replace_func *replace_phys =
	       (void *)__pa_symbol(idmap_cpu_replace_ttbr1);

	cpu_install_idmap();
	replace_phys(ttbr1);
	cpu_uninstall_idmap();
}
