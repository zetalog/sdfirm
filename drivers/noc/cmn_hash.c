#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

typedef uint64_t caddr_t;

#ifndef NULL
#define	NULL				((void *)0L)
#endif /* NULL */

#define __AC(X,Y)	(X##Y)
#define _AC(X,Y)	__AC(X,Y)
#define _AT(T,X)	((T)(X))

#define _UL(x)		(_AC(x, UL))
#define _ULL(x)		(_AC(x, ULL))

#define _BITUL(x)	(_UL(1) << (x))
#define _BITULL(x)	(_ULL(1) << (x))

#define UL(x)		(_UL(x))
#define ULL(x)		(_ULL(x))

#define __raw_writeq(v,a)	(*(volatile uint64_t *)(caddr_t)(a) = (v))
#define __raw_readq(a)		(*(volatile uint64_t *)(caddr_t)(a))

#undef offsetof
#define offsetof(TYPE, MEMBER)		((size_t)&((TYPE *)0)->MEMBER)

#define _BV_UL(bit)			(UL(1) << (bit))
#define _BV_ULL(bit)			(ULL(1) << (bit))
#define _BV(bit)			_BV_UL(bit)
#define _SET_FV_UL(name, value)		\
	((UL(0) + ((value) & (name##_MASK))) << (name##_OFFSET))
#define _SET_FV_ULL(name, value)	\
	((ULL(0) + ((value) & (name##_MASK))) << (name##_OFFSET))
#define _SET_FV(name, value)		_SET_FV_UL(name, value)
#define _GET_FV_UL(name, value)		\
	((UL(0) + ((value) >> (name##_OFFSET))) & (name##_MASK))
#define _GET_FV_ULL(name, value)	\
	((ULL(0) + ((value) >> (name##_OFFSET))) & (name##_MASK))
#define _GET_FV(name, value)		_GET_FV_UL(name, value)
/* Default to _SET_FV() as by default _FV() is used to generate field
 * values to be written to the registers.
 */
#define _FV(name, value)		_SET_FV(name, value)
#define _GET_FVn(n, name, value)        \
        (((value) >> (name##_OFFSET(n))) & (name##_MASK))
#define _SET_FVn(n, name, value)        \
        (((value) & (name##_MASK)) << (name##_OFFSET(n)))
#define _GET_FV_ULLn(n, name, value)    \
        ((((uint64_t)(value)) >> (name##_OFFSET(n))) & (name##_MASK))
#define _SET_FV_ULLn(n, name, value)    \
        ((((uint64_t)(value)) & (name##_MASK)) << (name##_OFFSET(n)))

#define REG16_1BIT_INDEX(n)             ((((uint16_t)(n)) & (~UL(15))) >> 4)
#define REG16_2BIT_INDEX(n)             ((((uint16_t)(n)) & (~UL(7 ))) >> 3)
#define REG16_4BIT_INDEX(n)             ((((uint16_t)(n)) & (~UL(3 ))) >> 2)
#define REG16_8BIT_INDEX(n)             ((((uint16_t)(n)) & (~UL(1 ))) >> 1)
#define REG_1BIT_MASK			0x001
#define REG_2BIT_MASK			0x003
#define REG_3BIT_MASK			0x007
#define REG_4BIT_MASK			0x00F
#define REG_5BIT_MASK			0x01F
#define REG_6BIT_MASK			0x03F
#define REG_7BIT_MASK			0x07F
#define REG_8BIT_MASK			0x0FF
#define REG_9BIT_MASK			0x1FF
#define REG_10BIT_MASK			0x3FF
#define REG_11BIT_MASK			0x7FF
#define REG_12BIT_MASK			0xFFF
#define REG_13BIT_MASK			0x1FFF
#define REG_15BIT_MASK			0x7FFF
#define REG_16BIT_MASK			0xFFFF
#define REG_17BIT_MASK			0x1FFFF
#define REG_20BIT_MASK			0xFFFFF
#define REG_23BIT_MASK			0x7FFFFF
#define REG_24BIT_MASK			0xFFFFFF
#define REG_28BIT_MASK			0xFFFFFFF
#define REG_32BIT_MASK			0xFFFFFFFF
#define REG_36BIT_MASK			ULL(0xFFFFFFFFF)
#define REG_40BIT_MASK			ULL(0xFFFFFFFFFF)
#define REG_44BIT_MASK			ULL(0xFFFFFFFFFFF)

#define REG32_1BIT_INDEX(n)		((((uint32_t)(n)) & (~UL(31))) >> 5)
#define REG32_2BIT_INDEX(n)		((((uint32_t)(n)) & (~UL(15))) >> 4)
#define REG32_4BIT_INDEX(n)		((((uint32_t)(n)) & (~UL(7 ))) >> 3)
#define REG32_8BIT_INDEX(n)		((((uint32_t)(n)) & (~UL(3 ))) >> 2)
#define REG32_16BIT_INDEX(n)		((((uint32_t)(n)) & (~UL(1 ))) >> 1)
#define REG64_1BIT_INDEX(n)		((((uint64_t)(n)) & (~ULL(63))) >> 6)
#define REG64_2BIT_INDEX(n)		((((uint64_t)(n)) & (~ULL(31))) >> 5)
#define REG64_4BIT_INDEX(n)		((((uint64_t)(n)) & (~ULL(15))) >> 4)
#define REG64_8BIT_INDEX(n)		((((uint64_t)(n)) & (~ULL(7) )) >> 3)
#define REG64_16BIT_INDEX(n)		((((uint64_t)(n)) & (~ULL(3) )) >> 2)
#define REG64_32BIT_INDEX(n)		((((uint64_t)(n)) & (~ULL(1) )) >> 1)
#define REG_1BIT_INDEX(n)		REG32_1BIT_INDEX(n)
#define REG_2BIT_INDEX(n)		REG32_2BIT_INDEX(n)
#define REG_4BIT_INDEX(n)		REG32_4BIT_INDEX(n)
#define REG_8BIT_INDEX(n)		REG32_8BIT_INDEX(n)
#define REG_16BIT_INDEX(n)		REG32_16BIT_INDEX(n)

#define REG32_1BIT_OFFSET(n)		((((uint32_t)(n)) & UL(31)) << 0)
#define REG32_2BIT_OFFSET(n)		((((uint32_t)(n)) & UL(15)) << 1)
#define REG32_4BIT_OFFSET(n)		((((uint32_t)(n)) & UL( 7)) << 2)
#define REG32_8BIT_OFFSET(n)		((((uint32_t)(n)) & UL( 3)) << 3)
#define REG32_16BIT_OFFSET(n)		((((uint32_t)(n)) & UL( 1)) << 4)
#define REG64_1BIT_OFFSET(n)		((((uint64_t)(n)) & ULL(63)) << 0)
#define REG64_2BIT_OFFSET(n)		((((uint64_t)(n)) & ULL(31)) << 1)
#define REG64_4BIT_OFFSET(n)		((((uint64_t)(n)) & ULL(15)) << 2)
#define REG64_8BIT_OFFSET(n)		((((uint64_t)(n)) & ULL( 7)) << 3)
#define REG64_16BIT_OFFSET(n)		((((uint64_t)(n)) & ULL( 3)) << 4)
#define REG64_32BIT_OFFSET(n)		((((uint64_t)(n)) & ULL( 1)) << 5)
#define REG_1BIT_OFFSET(n)		REG32_1BIT_OFFSET(n)
#define REG_2BIT_OFFSET(n)		REG32_2BIT_OFFSET(n)
#define REG_4BIT_OFFSET(n)		REG32_4BIT_OFFSET(n)
#define REG_8BIT_OFFSET(n)		REG32_8BIT_OFFSET(n)
#define REG_16BIT_OFFSET(n)		REG32_16BIT_OFFSET(n)

#define REG_1BIT_ADDR(base, n)		((base)+(((n) & (~31)) >> 3))
#define REG_2BIT_ADDR(base, n)		((base)+(((n) & (~15)) >> 2))
#define REG_4BIT_ADDR(base, n)		((base)+(((n) & (~7 )) >> 1))
#define REG_8BIT_ADDR(base, n)		((base)+(((n) & (~3 )) >> 0))
#define _GET_FVn(n, name, value)	\
	(((value) >> (name##_OFFSET(n))) & (name##_MASK))
#define _SET_FVn(n, name, value)	\
	(((value) & (name##_MASK)) << (name##_OFFSET(n)))


uint8_t hweight16(uint16_t word)
{
	uint16_t res = word - ((word >> 1) & 0x5555);
	res = (res & 0x3333) + ((res >> 2) & 0x3333);
	res = (res + (res >> 4)) & 0x0F0F;
	return (res + (res >> 8)) & 0x00FF;
}

#define NR_CMN_NODES	256

#if NR_CMN_NODES < 256
typedef uint8_t cmn_id_t;
#elif NR_CMN_NODES < 65536
typedef uint16_t cmn_id_t;
#else
typedef uint32_t cmn_id_t;
#endif

#define CMN_MODNAME	"n100"

#define CMN_REG(base, offset)		(base + offset)
#define CMN_8BIT_REG(base, offset, n)   \
        CMN_REG((base), (offset) + (REG64_8BIT_INDEX(n) << 3))

typedef uint16_t cmn_nid_t;
typedef uint16_t cmn_lid_t;
caddr_t cmn_node_base;

void cmn_writeq(uint64_t v, caddr_t a, const char* n, int i)
{
	if (i < 0)
		printf(CMN_MODNAME ": %016llx=%016llx %s\n",
			(uint64_t)a - cmn_node_base, v, n);
	else
		printf(CMN_MODNAME ": %016llx=%016llx %s%d\n",
			(uint64_t)a - cmn_node_base, v, n, i);
	__raw_writeq(v, a);
}

#define cmn_setq(v,a,n,i)                               \
        do {                                            \
                uint64_t __v = __raw_readq(a);          \
                __v |= (v);                             \
                cmn_writeq(__v, (a), (n), (i));         \
        } while (0)
#define cmn_clearq(v,a,n,i)                             \
        do {                                            \
                uint64_t __v = __raw_readq(a);          \
                __v &= ~(v);                            \
                cmn_writeq(__v, (a), (n), (i));         \
        } while (0)
#define cmn_writeq_mask(v,m,a,n,i)                      \
        do {                                            \
                uint64_t __v = __raw_readq(a);          \
                __v &= ~(m);                            \
                __v |= (v);                             \
                cmn_writeq(__v, (a), (n), (i));         \
        } while (0)

#define CMN_12BIT_INDEX(n)              REG64_16BIT_INDEX(n)
#define CMN_12BIT_OFFSET(n)             \
        (((((uint64_t)(n)) & ULL( 3)) << 4) -   \
         ((((uint64_t)(n)) & ULL( 3)) << 2))

#define CMN_12BIT_REG(base, offset, n)  \
        CMN_REG((base), (offset) + (CMN_12BIT_INDEX(n) << 3))

#define CMN_nodeid_OFFSET(n)            CMN_12BIT_OFFSET(n)
#define CMN_nodeid_MASK                 REG_12BIT_MASK
#define CMN_nodeid(n, value)            _SET_FV_ULLn(n, CMN_nodeid, value)

cmn_id_t cmn_hnf_count;
cmn_id_t cmn_snf_count;
uint8_t cmn_hnf_scgs[16];

cmn_nid_t cmn_snf_table[] = {
	8, /* DDR0 */
	16, /* DDR1 */
	24, /* DDR2 */
	32, /* DDR3 */
	12, /* DDR0 */
	20, /* DDR1 */
	28, /* DDR2 */
	36, /* DDR3 */
	328, /* DDR4 */
	336, /* DDR5 */
	344, /* DDR6 */
	352, /* DDR7 */
	332, /* DDR4 */
	340, /* DDR5 */
	348, /* DDR6 */
	356, /* DDR7 */
};

cmn_nid_t cmn_hnf_table[] = {
	76, /* Cluster0 */
	84, /* Cluster1 */
	92, /* Cluster2 */
	100, /* Cluster3 */
	140, /* Cluster4 */
	148, /* Cluster5 */
	156, /* Cluster6 */
	164, /* Cluster7 */
	204, /* Cluster8 */
	212, /* Cluster9 */
	220, /* Cluster10 */
	228, /* Cluster11 */
	268, /* Cluster12 */
	276, /* Cluster13 */
	284, /* Cluster14 */
	292, /* Cluster15 */
};

#define CMN_rnsam_sys_cache_grp_hn_nodeid(base, n)      \
	CMN_12BIT_REG(base, 0x0C58, n)

uint16_t DDR_MASK;
uint64_t CPU_MASK;
bool CONFIG_K1MATRIX_NINTLV;

uint32_t CMN_rnsam_sys_cache_grp_hn_nodeid_reg[16];

static void cmn600_configure_rn_sam(caddr_t rnsam)
{
	caddr_t base;
	cmn_id_t region_index;
	cmn_lid_t lid;
	cmn_nid_t nid;
	cmn_id_t snf;
	cmn_id_t lid_base;
	uint8_t scg;

	lid_base = 0;
	scg = 0;
#ifdef CONFIG_CMN600_SAM_RANGE_BASED
	for (lid = 0; lid < cmn_hnf_count; lid++) {
		nid = cmn_hnf_scgs[lid_base];
		cmn_writeq_mask(CMN_nodeid(lid_base, nid),
			CMN_nodeid(lid_base, CMN_nodeid_MASK),
			CMN_rnsam_sys_cache_grp_hn_nodeid(rnsam, lid_base),
			"CMN_rnsam_sys_cache_grp_hn_nodeid", lid_base);
		lid_base++;
#ifdef CONFIG_CMN600_DEBUG_CONFIGURE
		con_dbg(CMN_MODNAME ": SCG%d: %d/%d, ID: %d\n",
			scg, lid_base, cmn_hnf_count, nid);
#endif
	}
#else
	while (lid_base < cmn_hnf_count) {
		for (lid = 0; lid < cmn_snf_count && lid_base < cmn_hnf_count; lid++) {
			caddr_t reg = CMN_rnsam_sys_cache_grp_hn_nodeid(rnsam, lid_base);

			nid = cmn_hnf_scgs[lid_base];
			cmn_writeq_mask(CMN_nodeid(lid_base, nid),
				CMN_nodeid(lid_base, CMN_nodeid_MASK),
				reg,
				"CMN_rnsam_sys_cache_grp_hn_nodeid", lid_base);
			lid_base++;
#ifdef CONFIG_CMN600_DEBUG_CONFIGURE
			con_dbg(CMN_MODNAME ": SCG%d: %d/%d, ID: %d\n",
				scg, lid_base, cmn_hnf_count, nid);
#endif
		}
		scg++;
		if (scg == 4) {
			if ((cmn_snf_count * 4) < cmn_hnf_count)
				lid_base += cmn_hnf_count - (cmn_snf_count * 4);
			scg = 0;
		}
	}
#endif
#if 0
	cmn_writeq(cmn_hnf_count, CMN_rnsam_sys_cache_group_hn_count(rnsam),
		"CMN_rnsam_sys_cache_group_hn_count", -1);
#endif
}

uint16_t k1matrix_ddr2snf(uint8_t ddr_mask)
{
	uint16_t snf_mask;

	snf_mask = (ddr_mask & 0xf) | ((ddr_mask & 0xf0) << 4);
	if (!CONFIG_K1MATRIX_NINTLV)
		snf_mask |= (snf_mask & 0xf0f) << 4;
	return snf_mask;
}

uint16_t k1matrix_llc2hnf(uint64_t cluster_mask)
{
	uint64_t hnf_mask;

	hnf_mask = ((cluster_mask & 0xaaaaaaaaaaaaaaaa) >> 1) |
		(cluster_mask & 0x5555555555555555);
	hnf_mask = ((hnf_mask & 0x4444444444444444) >> 2) |
		(hnf_mask & 0x1111111111111111);
	hnf_mask = ((hnf_mask & 0x1010101010101010) >> 3) |
		(hnf_mask & 0x0101010101010101);
	hnf_mask = ((hnf_mask & 0x0300030003000300) >> 6) |
		(hnf_mask & 0x0003000300030003);
	hnf_mask = ((hnf_mask & 0x000f0000000f0000) >> 12) |
		(hnf_mask & 0x0000000f0000000f);
	hnf_mask = ((hnf_mask & 0x000000ff00000000) >> 24) |
		(hnf_mask & 0x00000000000000ff);
	return (uint16_t)hnf_mask;
}

cmn_id_t k1matrix_hnf_index(uint16_t hnf_mask, cmn_nid_t nid)
{
	int i;
	cmn_id_t index = 0;

	for (i = 0; i < 16; i++) {
		if (!(hnf_mask & _BV(i)))
			continue;
		if (nid == cmn_hnf_table[i])
			return index;
		index++;
	}
	return 16;
}

bool cmn600_hw_hnf_masked(cmn_nid_t nid)
{
	return k1matrix_hnf_index(k1matrix_llc2hnf(CPU_MASK), nid) == 16;
}

cmn_id_t cmn600_hw_snf_count(void)
{
	return hweight16(k1matrix_ddr2snf(DDR_MASK));
}

void cmn600_configure(void)
{
	cmn_id_t i;

	for (i = 0; i < 16; i++) {
		if (!cmn600_hw_hnf_masked(cmn_hnf_table[i]))
			cmn_hnf_scgs[cmn_hnf_count++] = cmn_hnf_table[i];
	}
	cmn_snf_count = cmn600_hw_snf_count();
	cmn600_configure_rn_sam(cmn_node_base);
}

void cmn600_hnf_hash_test(uint64_t cpu_mask, uint16_t ddr_mask, bool intlv)
{
	uint8_t i;

	printf("CPU: %016llx, DDR: %02x %s\n",
	       cpu_mask, (uint8_t)ddr_mask, intlv ? "INTLV" : "\0");
	cmn_node_base = (uint8_t *)CMN_rnsam_sys_cache_grp_hn_nodeid_reg - 0x0C58;
	for (i = 0; i < 16; i++)
		CMN_rnsam_sys_cache_grp_hn_nodeid_reg[i] = 0;
	cmn_hnf_count = 0;
	cmn_snf_count = 0;
	CONFIG_K1MATRIX_NINTLV = !intlv;
	DDR_MASK = ddr_mask;
	CPU_MASK = cpu_mask;
	cmn600_configure();
}

void main(void)
{
	/* MODE 1 */
	cmn600_hnf_hash_test(0xffffffffffffffff, 0xff, true);
	/* MODE 2 */
	cmn600_hnf_hash_test(0xffffffff, 0xf, true);
	/* MODE 3 */
	cmn600_hnf_hash_test(0xffff, 0x3, true);
	/* MODE 4 */
	cmn600_hnf_hash_test(0xffffffffffffffff, 0x66, true);
	/* MODE 5 */
	cmn600_hnf_hash_test(0xffffffff, 0x6, true);
	/* MODE 6 */
	cmn600_hnf_hash_test(0xffff, 0x2, true);
	/* MODE 7 */
	cmn600_hnf_hash_test(0xffffffffffffffff, 0x1, true);
	/* MODE 8 */
	cmn600_hnf_hash_test(0xffffffffffffffff, 0x1, false);
}
