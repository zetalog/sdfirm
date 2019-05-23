#ifndef __ARM64_TLB_H_INCLUDE__
#define __ARM64_TLB_H_INCLUDE__

static inline void flush_tlb_all(void)
{
	dsb(ishst);
	asm("tlbi	vmalle1is");
	dsb(ish);
	isb();
}

#endif /* __ARM64_TLB_H_INCLUDE__ */
