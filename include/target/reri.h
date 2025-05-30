#ifndef __TARGET_RERI_H_INCLUDE__
#define __TARGET_RERI_H_INCLUDE__

#include <target/generic.h>
#include <target/ras.h>
#include <target/actbl1.h>
#include <target/cper.h>
#include <target/io.h>

/* register definitions */
#define RERI_VENDOR_N_IMP_ID		0x0
#define RERI_BANK_INFO			0x8
#define RERI_VALID_SUMMARY		0x10
#define RERI_CONTROL_I(base, i)		((base) + (0x40 + 0x40 * (i)))
#define RERI_STATUS_I(base, i)		((base) + (0x48 + 0x40 * (i)))
#define RERI_ADDR_INFO_I(base, i)	((base) + (0x50 + 0x40 * (i)))
#define RERI_INFO_I(base, i)		((base) + (0x58 + 0x40 * (i)))
#define RERI_SUPPL_INFO_I(base, i)	((base) + (0x60 + 0x40 * (i)))
#define RERI_TIMESTAMP_I(base, i)	((base) + (0x68 + 0x40 * (i)))

/* RERI vendor_n_imp_id fields */
#define RERI_VENDOR_ID(value)		_GET_FV_ULL(RERI_VENDOR_ID, value)

#define RERI_IMP_ID_OFFSET		32
#define RERI_IMP_ID_MASK		REG_32BIT_MASK
#define RERI_IMP_ID(value)		_GET_FV_ULL(RERI_IMP_ID, value)

/* RERI bank_info fields */
#define RERI_INST_ID_OFFSET		0
#define RERI_INST_ID_MASK		REG_16BIT_MASK
#define RERI_INST_ID(value)		_GET_FV_ULL(RERI_INST_ID, value)

#define RERI_N_ERR_RECS_OFFSET		16
#define RERI_N_ERR_RECS_MASK		REG_6BIT_MASK
#define RERI_N_ERR_RECS(value)		_GET_FV_ULL(RERI_N_ERR_RECS, value)

#define RERI_LAYOUT_OFFSET		22
#define RERI_LAYOUT_MASK		REG_2BIT_MASK
#define RERI_LAYOUT(value)		_GET_FV_ULL(RERI_LAYOUT, value)

#define RERI_VERSION_OFFSET		56
#define RERI_VERSION_MASK		REG_8BIT_MASK
#define RERI_VERSION(value)		_GET_FV_ULL(RERI_VERSION, value)

/* RERI valid_summary fields */
#define RERI_SV_EN			_BV_ULL(0)
#define RERI_VALID_BITMAP_MASK		GENMASK(63, 1)

/* RERI control_i fields */
#define RERI_CTRL_ELSE_EN		_BV_ULL(0)

#define RERI_CTRL_CECE_OFFSET		1
#define RERI_CTRL_CECE_MASK		REG_1BIT_MASK
#define RERI_CTRL_GET_CECE(value)	_GET_FV_ULL(RERI_CTRL_CECE, value)
#define RERI_CTRL_SET_CECE(value)	_SET_FV_ULL(RERI_CTRL_CECE, value)

#define RERI_CTRL_CES_OFFSET		2
#define RERI_CTRL_CES_MASK		REG_2BIT_MASK
#define RERI_CTRL_GET_CES(value)	_GET_FV_ULL(RERI_CTRL_CES, value)
#define RERI_CTRL_SET_CES(value)	_SET_FV_ULL(RERI_CTRL_CES, value)

#define RERI_CTRL_UEDS_OFFSET		4
#define RERI_CTRL_UEDS_MASK		REG_2BIT_MASK
#define RERI_CTRL_GET_UEDS(value)	_GET_FV_ULL(RERI_CTRL_UEDS, value)
#define RERI_CTRL_SET_UEDS(value)	_SET_FV_ULL(RERI_CTRL_UEDS, value)

#define RERI_CTRL_UECS_OFFSET		6
#define RERI_CTRL_UECS_MASK		REG_2BIT_MASK
#define RERI_CTRL_GET_UECS(value)	_GET_FV_ULL(RERI_CTRL_UECS, value)
#define RERI_CTRL_SET_UECS(value)	_SET_FV_ULL(RERI_CTRL_UECS, value)

#define RERI_CTRL_EID_OFFSET		32
#define RERI_CTRL_EID_MASK		REG_16BIT_MASK
#define RERI_CTRL_GET_EID(value)	_GET_FV_ULL(RERI_CTRL_EID, value)
#define RERI_CTRL_SET_EID(value)	_SET_FV_ULL(RERI_CTRL_EID, value)

#define RERI_CTRL_SINV_EN		_BV_ULL(48)
#define RERI_CTRL_SRDP_EN		_BV_ULL(49)

#define RERI_CTRL_CUST_OFFSET		60
#define RERI_CTRL_CUST_MASK		REG_4BIT_MASK
#define RERI_CTRL_GET_CUST(value)	_GET_FV_ULL(RERI_CTRL_CUST, value)
#define RERI_CTRL_SET_CUST(value)	_SET_FV_ULL(RERI_CTRL_CUST, value)

/* RERI status_i fields */
#define RERI_STATUS_V_EN		_BV_ULL(0)

#define RERI_STATUS_CE_OFFSET		1
#define RERI_STATUS_CE_MASK		REG_1BIT_MASK
#define RERI_STATUS_GET_CE(value)	_GET_FV_ULL(RERI_STATUS_CE, value)
#define RERI_STATUS_SET_CE(value)	_SET_FV_ULL(RERI_STATUS_CE, value)

#define RERI_STATUS_UED_OFFSET		2
#define RERI_STATUS_UED_MASK		REG_1BIT_MASK
#define RERI_STATUS_GET_UED(value)	_GET_FV_ULL(RERI_STATUS_UED, value)
#define RERI_STATUS_SET_UED(value)	_SET_FV_ULL(RERI_STATUS_UED, value)

#define RERI_STATUS_UEC_OFFSET		3
#define RERI_STATUS_UEC_MASK		REG_1BIT_MASK
#define RERI_STATUS_GET_UEC(value)	_GET_FV_ULL(RERI_STATUS_UEC, value)
#define RERI_STATUS_SET_UEC(value)	_SET_FV_ULL(RERI_STATUS_UEC, value)

#define RERI_STATUS_PRI_OFFSET		4
#define RERI_STATUS_PRI_MASK		REG_2BIT_MASK
#define RERI_STATUS_GET_PRI(value)	_GET_FV_ULL(RERI_STATUS_PRI, value)
#define RERI_STATUS_SET_PRI(value)	_SET_FV_ULL(RERI_STATUS_PRI, value)

#define RERI_STATUS_MO_OFFSET		6
#define RERI_STATUS_MO_MASK		REG_1BIT_MASK
#define RERI_STATUS_GET_MO(value)	_GET_FV_ULL(RERI_STATUS_MO, value)
#define RERI_STATUS_SET_MO(value)	_SET_FV_ULL(RERI_STATUS_MO, value)

#define RERI_STATUS_C_OFFSET		7
#define RERI_STATUS_C_MASK		REG_1BIT_MASK
#define RERI_STATUS_GET_C(value)	_GET_FV_ULL(RERI_STATUS_C, value)
#define RERI_STATUS_SET_C(value)	_SET_FV_ULL(RERI_STATUS_C, value)

#define RERI_STATUS_TT_OFFSET		8
#define RERI_STATUS_TT_MASK		REG_3BIT_MASK
#define RERI_STATUS_GET_TT(value)	_GET_FV_ULL(RERI_STATUS_TT, value)
#define RERI_STATUS_SET_TT(value)	_SET_FV_ULL(RERI_STATUS_TT, value)

#define RERI_STATUS_IV_OFFSET		11
#define RERI_STATUS_IV_MASK		REG_1BIT_MASK
#define RERI_STATUS_GET_IV(value)	_GET_FV_ULL(RERI_STATUS_IV, value)
#define RERI_STATUS_SET_IV(value)	_SET_FV_ULL(RERI_STATUS_IV, value)

#define RERI_STATUS_AIT_OFFSET		12
#define RERI_STATUS_AIT_MASK		REG_4BIT_MASK
#define RERI_STATUS_GET_AIT(value)	_GET_FV_ULL(RERI_STATUS_AIT, value)
#define RERI_STATUS_SET_AIT(value)	_SET_FV_ULL(RERI_STATUS_AIT, value)

#define RERI_STATUS_SIV_OFFSET		16
#define RERI_STATUS_SIV_MASK		REG_1BIT_MASK
#define RERI_STATUS_GET_SIV(value)	_GET_FV_ULL(RERI_STATUS_SIV, value)
#define RERI_STATUS_SET_SIV(value)	_SET_FV_ULL(RERI_STATUS_SIV, value)

#define RERI_STATUS_TSV_OFFSET		17
#define RERI_STATUS_TSV_MASK		REG_1BIT_MASK
#define RERI_STATUS_GET_TSV(value)	_GET_FV_ULL(RERI_STATUS_TSV, value)
#define RERI_STATUS_SET_TSV(value)	_SET_FV_ULL(RERI_STATUS_TSV, value)

#define RERI_STATUS_SCRUB_OFFSET	20
#define RERI_STATUS_SCRUB_MASK		REG_1BIT_MASK
#define RERI_STATUS_GET_SCRUB(value)	_GET_FV_ULL(RERI_STATUS_SCRUB, value)
#define RERI_STATUS_SET_SCRUB(value)	_SET_FV_ULL(RERI_STATUS_SCRUB, value)

#define RERI_STATUS_CECO_OFFSET		21
#define RERI_STATUS_CECO_MASK		REG_1BIT_MASK
#define RERI_STATUS_GET_CECO(value)	_GET_FV_ULL(RERI_STATUS_CECO, value)
#define RERI_STATUS_SET_CECO(value)	_SET_FV_ULL(RERI_STATUS_CECO, value)

#define RERI_STATUS_RDIP_OFFSET		23
#define RERI_STATUS_RDIP_MASK		REG_1BIT_MASK
#define RERI_STATUS_GET_RDIP(value)	_GET_FV_ULL(RERI_STATUS_RDIP, value)
#define RERI_STATUS_SET_RDIP(value)	_SET_FV_ULL(RERI_STATUS_RDIP, value)

#define RERI_STATUS_EC_OFFSET		24
#define RERI_STATUS_EC_MASK		REG_8BIT_MASK
#define RERI_STATUS_GET_EC(value)	_GET_FV_ULL(RERI_STATUS_EC, value)
#define RERI_STATUS_SET_EC(value)	_SET_FV_ULL(RERI_STATUS_EC, value)

#define RERI_STATUS_CEC_OFFSET		48
#define RERI_STATUS_CEC_MASK		REG_16BIT_MASK
#define RERI_STATUS_GET_CEC(value)	_GET_FV_ULL(RERI_STATUS_CEC, value)
#define RERI_STATUS_SET_CEC(value)	_SET_FV_ULL(RERI_STATUS_CEC, value)


#if __riscv_xlen == 64
#ifdef CONFIG_ARCH_IS_MMIO_32BIT
static inline uint64_t reri_read(caddr_t dev_addr)
{
	uint32_t hi, lo;

	hi = __raw_readl(((uintptr_t)(dev_addr) + 4));
	lo = __raw_readl(dev_addr);

	return (uint64_t)hi << 32 | lo;
}

static inline void reri_write(uint64_t val, caddr_t dev_addr)
{
	__raw_writel(HIDWORD(val), ((uintptr_t)(dev_addr) + 4));
	__raw_writel(LODWORD(val), dev_addr);
}
#else
static inline uint64_t reri_read(caddr_t dev_addr)
{
	return __raw_readq(dev_addr);
}

static inline void reri_write(uint64_t val, caddr_t dev_addr)
{
	__raw_writeq(val, dev_addr);
}
#endif

#define reri_set(v, a)					\
	do {						\
		uint64_t __v = reri_read(a);		\
		__v |= (v);				\
		reri_write(__v, (a));			\
	} while (0)
#define reri_clear(v, a)				\
	do {						\
		uint64_t __v = reri_read(a);		\
		__v &= ~(v);				\
		reri_write(__v, (a));			\
	} while (0)
#define reri_write_mask(v, m, a)			\
	do {						\
		uint64_t __v = reri_read(a);		\
		__v &= ~(m);				\
		__v |= (v);				\
		reri_write(__v, a);			\
	} while (0)
#define iommu_set(v, a)					\
	do {						\
		uint32_t __v = __raw_readl(a);		\
		__v |= (v);				\
		__raw_writel(__v, (a));			\
	} while (0)
#endif
struct reri_info {
	uint32_t ec;	// Error Code
	uint32_t tt;	// Transaction Type
	uint32_t ce;	// Corrected Error (CE)
	uint32_t uec;	// Uncorrected Urgent Error (UUE)
	uint32_t ued;	// Uncorrected Deferred Error (UDE)
	const char *desc;
};

typedef enum {
	DIE_0 = 0,
	DIE_1 = 1,
	DIE_COUNT
} die_id_t;


typedef struct {
	const char *name;	// error source name
	source_type_t type;	// error source type
	die_id_t die;		// error source die id
} reri_source_info_t;

#include <asm/mach/reri.h>

#ifdef CONFIG_RERI
void reri_drv_init(void);
int reri_drv_sync_hart_errs(u32 hart_id, u32 *pending_vectors);
#else
#define reri_drv_init()			do { } while (0)
#endif

#endif /* __TARGET_RERI_H_INCLUDE__ */

