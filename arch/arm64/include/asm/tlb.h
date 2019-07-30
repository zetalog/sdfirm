#ifndef __ARM64_TLB_H_INCLUDE__
#define __ARM64_TLB_H_INCLUDE__

#ifndef __ASSEMBLY__
#define __TLBI_0(op, arg) 	asm ("tlbi " #op);
#define __TLBI_1(op, arg)	asm ("tlbi " #op ", %0" : : "r" (arg))
#define __TLBI_N(op, arg, n, ...)	\
	__TLBI_##n(op, arg)
#define __tlbi(op, ...)			\
	__TLBI_N(op, ##__VA_ARGS__, 1, 0)

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
	__tlbi(vmallelis);
	dsb(ish);
	isb();
}
#endif /* !__ASSEMBLY__ */

#endif /* __ARM64_TLB_H_INCLUDE__ */
