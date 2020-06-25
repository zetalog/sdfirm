#include <target/tlb.h>
#include <target/smp.h>
#include <target/page.h>
#include <target/barrier.h>

#ifndef asid2cpu
#define asid2cpu(asid)		((cpu_t)(asid))
#endif

void local_flush_tlb_all(void)
{
	sfence_vma_all();
	fence(rw, rw);
}

void local_flush_tlb_page(caddr_t addr)
{
	sfence_vma_page(addr);
	fence(rw, rw);
}

void local_flush_tlb_asid_all(int asid)
{
	sfence_vma_asid_all(asid);
	fence(rw, rw);
}

void local_flush_tlb_asid_page(int asid, caddr_t addr)
{
	sfence_vma_asid_page(asid, addr);
	fence(rw, rw);
}

void local_flush_tlb_page(caddr_t addr);
#ifdef CONFIG_SMP
void __flush_tlb_all(void)
{
	sbi_remote_sfence_vma(cpumask_bits(&smp_online_cpus), 0, -1);
}

void __flush_tlb_range(cpu_mask_t *cpumask, caddr_t start, caddr_t end)
{
	sbi_remote_sfence_vma(cpumask_bits(cpumask), start, end - start);
}
#else
void __flush_tlb_all(void)
{
	local_flush_tlb_all();
}

void __flush_tlb_range(cpu_mask_t *cpumask, caddr_t start, caddr_t end)
{
	caddr_t __start = ALIGN_DOWN(start, PAGE_SIZE);
	caddr_t __end = ALIGN_UP(end, PAGE_SIZE);
	caddr_t addr;

	for (addr = __start; addr < __end; addr += PAGE_SIZE)
		local_flush_tlb_page(addr);
}
#endif

void flush_tlb_all(void)
{
	__flush_tlb_all();
}

void flush_tlb_page(int asid, caddr_t addr)
{
	cpu_t cpu = asid2cpu(asid);
	cpu_mask_t cpumask;

	if (cpumask_test_cpu(cpu, &smp_online_cpus))
		cpumask_set_cpu(cpu, &smp_online_cpus);
	cpumask_set_cpu(smp_processor_id(), &smp_online_cpus);
	__flush_tlb_range(&cpumask, addr, 0);
}

void flush_tlb_range_user(int asid, caddr_t start, caddr_t end)
{
	cpu_t cpu = asid2cpu(asid);
	cpu_mask_t cpumask;

	if (cpumask_test_cpu(cpu, &smp_online_cpus))
		cpumask_set_cpu(cpu, &smp_online_cpus);
	cpumask_set_cpu(smp_processor_id(), &smp_online_cpus);
	__flush_tlb_range(&cpumask, start, end);
}

void flush_tlb_range_kern(caddr_t start, caddr_t end)
{
	__flush_tlb_range(&smp_online_cpus, start, end);
}
