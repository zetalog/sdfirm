#ifndef __ARM64_TLB_H_INCLUDE__
#define __ARM64_TLB_H_INCLUDE__

#ifndef __ASSEMBLY__
#define __TLBI_0(op, arg) 	asm ("tlbi " #op);
#define __TLBI_1(op, arg)	asm ("tlbi " #op ", %0" : : "r" (arg))
#define __TLBI_N(op, arg, n, ...)	\
	__TLBI_##n(op, arg)
#define __tlbi(op, ...)			\
	__TLBI_N(op, ##__VA_ARGS__, 1, 0)
#define __tlbi_user(op, arg)		\
	__tlbi(op, (arg) | USER_ASID_FLAG)

/* This macro creates a properly formatted VA operand for the TLBI */
#define __TLBI_VADDR(addr, asid)		\
	({					\
		uint64_t __ta = (addr) >> 12;	\
		__ta &= GENMASK_ULL(43, 0);	\
		__ta |= (uint64_t)(asid) << 48;	\
		__ta;				\
	})

static inline void local_flush_tlb_all(void)
{
	dsb(nshst);
	__tlbi(vmalle1);
	dsb(nsh);
	isb();
}

static inline void flush_tlb_all(void)
{
	dsb(ishst);
	__tlbi(vmalle1is);
	dsb(ish);
	isb();
}

static inline void flush_tlb_user(uint16_t asid)
{
	dsb(ishst);
	__tlbi(aside1is, asid);
	__tlbi_user(aside1is, asid);
	dsb(ish);
}
#define flush_tlb_task(pid)	flush_tlb_user(pid)

static inline void flush_tlb_page(uint16_t asid, caddr_t uaddr)
{
	caddr_t addr = __TLBI_VADDR(uaddr, asid);

	dsb(ishst);
	__tlbi(vale1is, addr);
	__tlbi_user(vale1is, addr);
	dsb(ish);
}

#define MAX_TLBI_OPS	PAGE_MAX_TABLE_ENTRIES

static inline void __flush_tlb_range(uint16_t asid,
				     caddr_t start, caddr_t end,
				     caddr_t stride, bool last_level)
{
	caddr_t addr;

	start = round_down(start, stride);
	end = round_up(end, stride);

	if ((end - start) > (MAX_TLBI_OPS * PAGE_SIZE)) {
		flush_tlb_user(asid);
		return;
	}

	/* Convert the stride into units of 4k */
	stride >>= 12;

	start = __TLBI_VADDR(start, asid);
	end = __TLBI_VADDR(end, asid);

	dsb(ishst);
	for (addr = start; addr < end; addr += stride) {
		if (last_level) {
			__tlbi(vale1is, addr);
			__tlbi_user(vale1is, addr);
		} else {
			__tlbi(vae1is, addr);
			__tlbi_user(vae1is, addr);
		}
	}
	dsb(ish);
}

static inline void flush_tlb_range_user(uint16_t asid,
					caddr_t start, caddr_t end)
{
        /* We cannot use leaf-only invalidation here, since we may be
	 * invalidating table entries as part of collapsing hugepages or
	 * moving page tables.
	 */
	__flush_tlb_range(asid, start, end, PAGE_SIZE, false);
}
#define flush_tlb_range(pid, start, end)	\
	flush_tlb_range_user(pid, start, end)

static inline void flush_tlb_range_kern(caddr_t start, caddr_t end)
{
	caddr_t addr;

	if ((end - start) > (MAX_TLBI_OPS * PAGE_SIZE)) {
		flush_tlb_all();
		return;
	}

	start = __TLBI_VADDR(start, 0);
	end = __TLBI_VADDR(end, 0);

	dsb(ishst);
	for (addr = start; addr < end; addr += 1 << (PAGE_SHIFT - 12))
		__tlbi(vaale1is, addr);
	dsb(ish);
	isb();
}

/* Used to invalidate the TLB (walk caches) corresponding to intermediate
 * page table levels (pgd/pud/pmd).
 */
static inline void flush_tlb_kernel_pgtable(caddr_t kaddr)
{
	caddr_t addr = __TLBI_VADDR(kaddr, 0);

	dsb(ishst);
	__tlbi(vaae1is, addr);
	dsb(ish);
}
#endif /* !__ASSEMBLY__ */

#endif /* __ARM64_TLB_H_INCLUDE__ */
