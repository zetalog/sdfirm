#include <target/irq.h>
#include <target/cmdline.h>
#include <target/console.h>
#include <target/arch.h>
#include <target/ras.h>
#include <target/ghes.h>
#include <target/cper.h>
#include <target/acpi.h>
#include <target/ras.h>

#define DDR_MODNAME	"ddr_ras"

#ifdef CONFIG_DDR_RAS_DEBUG
#define ddr_dbg(...)	con_log(__VA_ARGS__)
#else
#define ddr_dbg(...)	do { } while (0)
#endif

/* DDR register operation macros */
#define ddr_set(v, a)					\
	do {						\
		uint32_t __v = __raw_readl(a);		\
		__v |= (v);				\
		__raw_writel(__v, (a));			\
	} while (0)

#define ddr_clear(v, a)					\
	do {						\
		uint32_t __v = __raw_readl(a);		\
		__v &= ~(v);				\
		__raw_writel(__v, (a));			\
	} while (0)

#define ddr_write_mask(v, m, a)				\
	do {						\
		uint32_t __v = __raw_readl(a);		\
		__v &= ~(m);				\
		__v |= (v);				\
		__raw_writel(__v, a);			\
	} while (0)

#define ddr_write_field(v, b, m, a)			\
	do {						\
		uint32_t __v = __raw_readl(a);		\
		__v &= ~(m);				\
		__v |= ((v) << (b)) & (m);		\
		__raw_writel(__v, a);			\
	} while (0)

#define ddr_read_field(b, m, a)				\
	({						\
		uint32_t __v = __raw_readl(a);		\
		__v &= (m);				\
		__v >>= (b);				\
		__v;					\
	})

#define FUNC_INTR_NUM 8
#define RAS_INTR_NUM  16

/**
 * struct ddrsub_baseaddr - DDR subsystem base address structure
 * @ctrl: DDR controller base address
 * @reg: DDR register base address
 */
struct ddrsub_baseaddr {
	caddr_t ctrl;
	caddr_t reg;
};

struct ddrsub_baseaddr ddrc_addr[] = {
	{.ctrl = __DDR_CH_A_CRTL_BASE, .reg = __DDR_CH_A_REG_BASE},  // DDR A
	{.ctrl = __DDR_CH_B_CRTL_BASE, .reg = __DDR_CH_B_REG_BASE},  // DDR B
	{.ctrl = __DDR_CH_C_CRTL_BASE, .reg = __DDR_CH_C_REG_BASE},  // DDR C
	{.ctrl = __DDR_CH_D_CRTL_BASE, .reg = __DDR_CH_D_REG_BASE},  // DDR D
	{.ctrl = __DDR_CH_E_CRTL_BASE, .reg = __DDR_CH_E_REG_BASE},  // DDR E
	{.ctrl = __DDR_CH_F_CRTL_BASE, .reg = __DDR_CH_F_REG_BASE},  // DDR F
	{.ctrl = __DDR_CH_G_CRTL_BASE, .reg = __DDR_CH_G_REG_BASE},  // DDR G
	{.ctrl = __DDR_CH_H_CRTL_BASE, .reg = __DDR_CH_H_REG_BASE},  // DDR H
};

/* DDR SYSREG */
#define DDR_INTERRUPT_CTRL_REG		0x10
#define FUNC_INTR_EN				_BV(0)
#define CH0_RAS_INTR_EN				_BV(1)
#define CH1_RAS_INTR_EN				_BV(2)

#define PAR_POISON_EN			0x28
#define WRB_P0R0_PAR_POIS_EN			_BV(0)
#define WRB_P0R1_PAR_POIS_EN			_BV(1)
#define WRB_P1R0_PAR_POIS_EN			_BV(2)
#define WRB_P1R1_PAR_POIS_EN			_BV(3)
#define RDB_P0R0_PAR_POIS_EN			_BV(4)
#define RDB_P0R1_PAR_POIS_EN			_BV(5)
#define RDB_P1R0_PAR_POIS_EN			_BV(6)
#define RDB_P1R1_PAR_POIS_EN			_BV(7)

#define FUNC_INTR_STAT			0x30
#define DFI_ALERT_ERR_INTR			_BV(0)
#define SWCMD_ERR_INTR				_BV(1)
#define DUCMD_ERR_INTR				_BV(2)
#define LCCMD_ERR_INTR				_BV(3)
#define CTRLUPD_ERR_INTR			_BV(4)
#define RFM_ALERT_INTR				_BV(5)
#define DERATE_TEMP_LIMIT_INTR			_BV(6)
#define SBR_DONE_INTR				_BV(7)
#define DFI_ALERT_ERR_INTR_DCH1			_BV(8)
#define SWCMD_ERR_INTR_DCH1			_BV(9)
#define DUCMD_ERR_INTR_DCH1			_BV(10)
#define LCCMD_ERR_INTR_DCH1			_BV(11)
#define CTRLUPD_ERR_INTR_DCH1			_BV(12)
#define RFM_ALERT_INTR_DCH1			_BV(13)
#define DERATE_TEMP_LIMIT_INTR_DCH1		_BV(14)
#define SBR_DONE_INTR_DCH1			_BV(15)
#define DPM_INTR				_BV(16)
#define DDRPHY_INTR				_BV(17)

#define RAS_INTR_STAT			0x34
#define ECC_CORRECTED_ERR_INTR			_BV(0)
#define ECC_UNCORRECTED_ERR_INTR		_BV(1)
#define WR_CRC_ERR_INTR				_BV(2)
#define WR_CRC_ERR_MAX_REACHED_INTR		_BV(3)
#define CAPAR_ERR_INTR				_BV(4)
#define CAPAR_ERR_MAX_REACHED_INTR		_BV(5)
#define CAPAR_FATL_ERR_INTR			_BV(6)
#define CAPAR_RETRY_LIMIT_REACHED_INTR		_BV(7)
#define CAPARCMD_ERR_INTR			_BV(8)
#define WR_CRC_RETRY_LIMIT_INTR			_BV(9)
#define RD_CRC_ERR_MAX_REACHED_INTR		_BV(10)
#define RD_RETRY_LIMIT_INTR			_BV(11)
#define ECC_CORRECTED_ERR_INTR_DCH1		_BV(12)
#define ECC_UNCORRECTED_ERR_INTR_DCH1		_BV(13)
#define WR_CRC_ERR_INTR_DCH1			_BV(14)
#define WR_CRC_ERR_MAX_REACHED_INTR_DCH1	_BV(15)
#define CAPAR_ERR_INTR_DCH1			_BV(16)
#define CAPAR_ERR_MAX_REACHED_INTR_DCH1		_BV(17)
#define CAPAR_FATL_ERR_INTR_DCH1		_BV(18)
#define CAPAR_RETRY_LIMIT_REACHED_INTR_DCH1	_BV(19)
#define CAPARCMD_ERR_INTR_DCH1			_BV(20)
#define WR_CRC_RETRY_LIMIT_INTR_DCH1		_BV(21)
#define RD_CRC_ERR_MAX_REACHED_INTR_DCH1	_BV(22)
#define RD_RETRY_LIMIT_INTR_DCH1		_BV(23)

#define WRB_RDB_PRR_CTRL		0xC4
#define WRB_P0R0_PAR_ERR_INTR_EN		_BV(0)
#define WRB_P0R1_PAR_ERR_INTR_EN		_BV(1)
#define WRB_P1R0_PAR_ERR_INTR_EN		_BV(2)
#define WRB_P1R1_PAR_ERR_INTR_EN		_BV(3)
#define RDB_P0R0_PAR_ERR_INTR_EN		_BV(4)
#define RDB_P0R1_PAR_ERR_INTR_EN		_BV(5)
#define RDB_P1R0_PAR_ERR_INTR_EN		_BV(6)
#define RDB_P1R1_PAR_ERR_INTR_EN		_BV(7)
#define WRB_P0R0_PAR_ERR_CNT_EN			_BV(8)
#define WRB_P0R1_PAR_ERR_CNT_EN			_BV(9)
#define WRB_P1R0_PAR_ERR_CNT_EN			_BV(10)
#define WRB_P1R1_PAR_ERR_CNT_EN			_BV(11)
#define RDB_P0R0_PAR_ERR_CNT_EN			_BV(12)
#define RDB_P0R1_PAR_ERR_CNT_EN			_BV(13)
#define RDB_P1R0_PAR_ERR_CNT_EN			_BV(14)
#define RDB_P1R1_PAR_ERR_CNT_EN			_BV(15)

#define WRB_RDB_PAR_INT_STAT		0xC8
#define WRB_P0R0_PAR_ERR_INTR			_BV(0)
#define WRB_P0R1_PAR_ERR_INTR			_BV(1)
#define WRB_P1R0_PAR_ERR_INTR			_BV(2)
#define WRB_P1R1_PAR_ERR_INTR			_BV(3)
#define RDB_P0R0_PAR_ERR_INTR			_BV(4)
#define RDB_P0R1_PAR_ERR_INTR			_BV(5)
#define RDB_P1R0_PAR_ERR_INTR			_BV(6)
#define RDB_P1R1_PAR_ERR_INTR			_BV(7)

#define WRB_P0R0_PAR_ERR_CNT		0xCC
#define WRB_P0R1_PAR_ERR_CNT		0xD0
#define WRB_P1R0_PAR_ERR_CNT		0xD4
#define WRB_P1R1_PAR_ERR_CNT		0xD8
#define RDB_P0R0_PAR_ERR_CNT		0xDC
#define RDB_P0R1_PAR_ERR_CNT		0xE0
#define RDB_P1R0_PAR_ERR_CNT		0xE4
#define RDB_P1R1_PAR_ERR_CNT		0xE8

/* REGB_DDRC_CH(ch) Registers */
#define REG_GROUP_DDRC_CH0		0x10000
#define REGB_DDRC_CH_OFFSET(ch)		((REG_GROUP_DDRC_CH0) + ((ch) * 0x10000))

/* ECC Registers */
#define ECCCFG0				0x600
#define ECCCFG0_ECC_MODE_MASK			GENMASK(2, 0)
#define DISABLE_ECC_MODE			0x0
#define SECDED_ECC_MODE				0x4
#define ADVECC_ECC_MODE				0x5

#define ECCCFG0_TEST_MODE			_BV(3)
#define ECCCFG0_ECC_TYPE_MASK			GENMASK(5, 4)
#define ECCCFG0_ECC_REGION_REMAP_EN		_BV(7)
#define ECCCFG0_ECC_REGION_MAP_MASK		GENMASK(14, 8)
#define ECCCFG0_BLK_CHANNEL_IDLE_TIME_X32_MASK	GENMASK(21, 16)
#define ECCCFG0_BLK_CHANNEL_IDLE_TIME_X32_OFFSET	16
#define ECCCFG0_ECC_REGION_MAP_OTHER	29
#define ECCCFG0_ECC_REGION_MAP_GRANU_MASK	GENMASK(31, 30)

#define ECCCFG1				0x604
#define ECCCFG1_DATA_POISON_EN			_BV(0)
#define ECCCFG1_DATA_POISON_BIT			_BV(1)
#define ECCCFG1_POISON_CHIP_EN			_BV(2)
#define ECCCFG1_ECC_REGION_PARITY_LOCK		_BV(4)
#define ECCCFG1_ECC_REGION_WASTE_LOCK		_BV(5)
#define ECCCFG1_BLK_CHANNEL_ACTIVE_TERM		_BV(7)
#define ECCCFG1_ACTIVE_BLK_CHANNEL_MASK		GENMASK(12, 8)
#define ECCCFG1_POISON_ADVECC_KBD		_BV(15)
#define ECCCFG1_POISON_NUM_DFI_BEAT_OFFSET	16
#define ECCCFG1_POISON_NUM_DFI_BEAT_MASK	GENMASK(19, 16)
#define ECCCFG1_PROP_RD_ECC_ERR_MASK		GENMASK(21, 20)

#define ECCSTAT				0x608
#define ECCSTAT_ECC_CORRECTED_BIT_NUM_MASK	GENMASK(6, 0)
#define ECCSTAT_ECC_CORRECTED_ERR_MASK		GENMASK(15, 8)
#define ECCSTAT_ECC_UNCORRECTED_ERR_MASK	GENMASK(23, 16)
#define ECCSTAT_SBR_READ_ECC_CE			_BV(24)
#define ECCSTAT_SBR_READ_ECC_UE			_BV(25)

#define ECCCTL				0x60C
#define ECCCTL_ECC_CORRECTED_ERR_CLR		_BV(0)
#define ECCCTL_ECC_UNCORRECTED_ERR_CLR		_BV(1)
#define ECCCTL_ECC_CORRECTED_ERR_CNT_CLR	_BV(2)
#define ECCCTL_ECC_UNCORRECTED_ERR_CNT_CLR	_BV(3)
#define ECCCTL_ECC_CORRECTED_ERR_INTR_EN	_BV(8)
#define ECCCTL_ECC_UNCORRECTED_ERR_INTR_EN	_BV(9)
#define ECCCTL_ECC_CORRECTED_ERR_INTR_FORCE	_BV(16)
#define ECCCTL_ECC_UNCORRECTED_ERR_INTR_FORCE	_BV(17)

#define ECCERRCNT			0x610
#define ECCERRCNT_ECC_CORR_ERR_CNT_MASK		GENMASK(15, 0)
#define ECCERRCNT_ECC_UNCORR_ERR_CNT_MASK	GENMASK(31, 16)

#define ECCCADDR0			0x614
#define ECCCADDR0_ECC_CORR_ROW_OFFSET		0
#define ECCCADDR0_ECC_CORR_ROW_MASK		GENMASK(23, 0)
#define ECCCADDR0_ECC_CORR_RANK_OFFSET		24
#define ECCCADDR0_ECC_CORR_RANK_MASK		GENMASK(31, 24)

#define ECCCADDR1			0x618
#define ECCCADDR1_ECC_CORR_COL_OFFSET		0
#define ECCCADDR1_ECC_CORR_COL_MASK		GENMASK(10, 0)
#define ECCCADDR1_ECC_CORR_BANK_OFFSET		16
#define ECCCADDR1_ECC_CORR_BANK_MASK		GENMASK(23, 16)
#define ECCCADDR1_ECC_CORR_BG_OFFSET		24
#define ECCCADDR1_ECC_CORR_BG_MASK		GENMASK(27, 24)
#define ECCCADDR1_ECC_CORR_CID_OFFSET		28
#define ECCCADDR1_ECC_CORR_CID_MASK		GENMASK(31, 28)

#define ECCCSYN0			0x61C
#define ECCCSYN0_ECC_CORR_SYNDROME_31_0_MASK	GENMASK(31, 0)

#define ECCCSYN1			0x620
#define ECCCSYN1_ECC_CORR_SYNDROME_63_32_MASK	GENMASK(31, 0)

#define ECCCSYN2			0x624
#define ECCCSYN2_CB_CORR_SYNDROME_MASK		GENMASK(23, 16)
#define ECCCSYN2_ECC_CORR_SYNDROME_71_64_MASK	GENMASK(7, 0)

#define ECCBITMASK0			0x628
#define ECCBITMASK0_ECC_CORR_BIT_MASK_31_0	GENMASK(31, 0)

#define ECCBITMASK1			0x62C
#define ECCBITMASK1_ECC_CORR_BIT_MASK_63_32	GENMASK(31, 0)

#define ECCBITMASK2			0x630
#define ECCBITMASK2_ECC_CORR_BIT_MASK_71_64	GENMASK(7, 0)

#define ECCUADDR0			0x634
#define ECCUADDR0_ECC_UNCORR_ROW_MASK		GENMASK(23, 0)
#define ECCUADDR0_ECC_UNCORR_RANK_MASK		GENMASK(31, 24)

#define ECCUADDR1			0x638
#define ECCUADDR1_ECC_UNCORR_COL_MASK		GENMASK(10, 0)
#define ECCUADDR1_ECC_UNCORR_BANK_MASK		GENMASK(23, 16)
#define ECCUADDR1_ECC_UNCORR_BG_MASK		GENMASK(27, 24)
#define ECCUADDR1_ECC_UNCORR_CID_MASK		GENMASK(31, 28)


#define ECCUSYN0			0x63C
#define ECCUSYN0_ECC_UNCORR_SYNDROME_31_0_MASK	GENMASK(31, 0)

#define ECCUSYN1			0x640
#define ECCUSYN1_ECC_UNCORR_SYNDROME_63_32_MASK	GENMASK(31, 0)

#define ECCUSYN2			0x644
#define ECCUSYN2_CB_UNCORR_SYNDROME_MASK	GENMASK(23, 16)
#define ECCUSYN2_ECC_UNCORR_SYNDROME_71_64_MASK	GENMASK(7, 0)

#define ECCPOISONADDR0			0x648
#define ECCPOISONADDR0_ECC_POISON_COL_OFFSET	0
#define ECCPOISONADDR0_ECC_POISON_COL_MASK	GENMASK(11, 0)
#define ECCPOISONADDR0_ECC_POISON_CID_OFFSET	16
#define ECCPOISONADDR0_ECC_POISON_CID_MASK	GENMASK(23, 16)
#define ECCPOISONADDR0_ECC_POISON_RANK_OFFSET	24
#define ECCPOISONADDR0_ECC_POISON_RANK_MASK	GENMASK(31, 24)

#define ECCPOISONADDR1			0x64C
#define ECCPOISONADDR1_ECC_POISON_ROW_OFFSET	0
#define ECCPOISONADDR1_ECC_POISON_ROW_MASK	GENMASK(23, 0)
#define ECCPOISONADDR1_ECC_POISON_BANK_OFFSET	24
#define ECCPOISONADDR1_ECC_POISON_BANK_MASK	GENMASK(27, 24)
#define ECCPOISONADDR1_ECC_POISON_BG_OFFSET	28
#define ECCPOISONADDR1_ECC_POISON_BG_MASK	GENMASK(31, 28)

/* Advanced ECC Registers */
#define ADVECCINDEX			0x650
#define ADVECCINDEX_ECC_SYNDROME_SEL_OFFSET	0
#define ADVECCINDEX_ECC_SYNDROME_SEL_MASK	GENMASK(2, 0)
#define ADVECCINDEX_ECC_ERR_SYMBOL_SEL_OFFSET	3
#define ADVECCINDEX_ECC_ERR_SYMBOL_SEL_MASK	GENMASK(4, 3)
#define ADVECCINDEX_ECC_POISON_BEATS_SEL_OFFSET	5
#define ADVECCINDEX_ECC_POISON_BEATS_SEL_MASK	GENMASK(8, 5)

#define ADVECCSTAT			0x654
#define ADVECCSTAT_ADVECC_CORRECTED_ERR		0
#define ADVECCSTAT_ADVECC_UNCORRECTED_ERR	1
#define ADVECCSTAT_ADVECC_NUM_ERR_SYMBOL_OFFSET	2
#define ADVECCSTAT_ADVECC_NUM_ERR_SYMBOL_MASK	GENMASK(4, 2)
#define ADVECCSTAT_ADVECC_ERR_SYMBOL_POS_OFFSET	5
#define ADVECCSTAT_ADVECC_ERR_SYMBOL_POS_MASK	GENMASK(15, 5)
#define ADVECCSTAT_ADVECC_ERR_SYMBOL_BITS_OFFSET	16
#define ADVECCSTAT_ADVECC_ERR_SYMBOL_BITS_MASK	GENMASK(23, 16)
#define ADVECCSTAT_ADVECC_CE_KBD_STATE_OFFSET	24
#define ADVECCSTAT_ADVECC_CE_KBD_STATE_MASK	GENMASK(27, 24)
#define ADVECCSTAT_ADVECC_UE_KBD_STATE_OFFSET	28
#define ADVECCSTAT_ADVECC_UE_KBD_STATE_MASK	GENMASK(29, 28)
#define ADVECCSTAT_ADVECC_SBR_READ_ADVECC_CE	_BV(30)
#define ADVECCSTAT_ADVECC_SBR_READ_ADVECC_UE	_BV(31)

#define ECCPOISONPAT0			0x658
#define ECCPOISONPAT0_ECC_POISON_DATA_31_0_MASK	GENMASK(31, 0)

#define ECCPOISONPAT1			0x65C
#define ECCPOISONPAT1_ECC_POISON_DATA_63_32_MASK	GENMASK(31, 0)

#define ECCPOISONPAT2			0x660
#define ECCPOISONPAT2_ECC_POISON_DATA_71_64_MASK	GENMASK(7, 0)

#define ECCCFG2				0x668
#define ECCCFG2_BYPASS_INTERNAL_ECC		_BV(0)
#define ECCCFG2_KBD_EN				_BV(1)
#define ECCCFG2_DIS_RMW_UE_PROPAGATION		_BV(4)
#define ECCCFG2_EAPAR_EN			_BV(8)
#define ECCCFG2_FLIP_BIT_POS0_MASK		GENMASK(22, 16)
#define ECCCFG2_FLIP_BIT_POS1_MASK		GENMASK(30, 24)

#define ECCCDATA0			0x66C
#define ECCCDATA0_ECC_CORR_DATA_31_0_MASK	GENMASK(31, 0)

#define ECCCDATA1			0x670
#define ECCCDATA1_ECC_CORR_DATA_63_32_MASK	GENMASK(31, 0)

#define ECCUDATA0			0x674
#define ECCUDATA0_ECC_UNCORR_DATA_31_0_MASK	GENMASK(31, 0)

#define ECCUDATA1			0x678
#define ECCUDATA1_ECC_UNCORR_DATA_63_32_MASK	GENMASK(31, 0)

#define ECCSYMBOL			0x67C
#define ECCSYMBOL_ECC_CORR_SYM_71_64_MASK	GENMASK(7, 0)
#define ECCSYMBOL_ECC_UNCORR_SYM_71_64_MASK	GENMASK(23, 16)

/* Derate Registers */
#define DERATECTL5			0x114
#define DERATECTL5_DERATE_TEMP_LIMIT_INTR_CLR	_BV(1)
#define DERATECTL5_DERATE_TEMP_LIMIT_INTR_FORCE	_BV(2)

#define CRCPARCTL0			0x800
#define CRCPARCTL0_CAPAR_ERR_INTR_CLR			_BV(1)
#define CRCPARCTL0_CAPAR_ERR_INTR_FORCE			_BV(2)
#define CRCPARCTL0_CAPAR_ERR_MAX_REACHED_INTR_CLR	_BV(5)
#define CRCPARCTL0_CAPAR_ERR_MAX_REACHED_INTR_FORCE	_BV(6)
#define CRCPARCTL0_CAPAR_ERR_CNT_CLR			_BV(7)
#define CRCPARCTL0_WR_CRC_ERR_INTR_CLR			_BV(9)
#define CRCPARCTL0_WR_CRC_ERR_INTR_FORCE		_BV(10)
#define CRCPARCTL0_WR_CRC_ERR_MAX_REACHED_INTR_CLR	_BV(13)
#define CRCPARCTL0_WR_CRC_ERR_MAX_REACHED_INTR_FORCE	_BV(14)
#define CRCPARCTL0_RD_CRC_ERR_MAX_REACHED_INT_CLR	_BV(21)
#define CRCPARCTL0_RD_CRC_ERR_MAX_REACHED_INTR_FORCE	_BV(23)
#define CRCPARCTL0_CAPAR_FATL_ERR_INTR_CLR		_BV(25)
#define CRCPARCTL0_CAPAR_FATL_ERR_INTR_FORCE		_BV(26)

#define RETRYCTL0			0x890
#define RETRYCTL0_WR_CRC_RETRY_LIMIT_INTR_CLR		_BV(21)
#define RETRYCTL0_WR_CRC_RETRY_LIMIT_INTR_FORCE		_BV(22)
#define RETRYCTL0_RD_RETRY_LIMIT_INTR_CLR		_BV(24)
#define RETRYCTL0_RD_RETRY_LIMIT_INTR_FORCE		_BV(25)
#define RETRYCTL0_CAPAR_RETRY_LIMIT_INTR_CLR		_BV(27)
#define RETRYCTL0_CAPAR_RETRY_LIMIT_INTR_FORCE		_BV(28)

#define PASINTCTL			0xB1C
#define PASINTCTL_SWCMD_ERR_INTR_CLR		_BV(1)
#define PASINTCTL_SWCMD_ERR_INTR_FORCE		_BV(2)
#define PASINTCTL_DUCMD_ERR_INTR_CLR		_BV(5)
#define PASINTCTL_DUCMD_ERR_INTR_FORCE		_BV(6)
#define PASINTCTL_LCCMD_ERR_INTR_CLR		_BV(9)
#define PASINTCTL_LCCMD_ERR_INTR_FORCE		_BV(10)
#define PASINTCTL_CTRLUPD_ERR_INTR_CLR		_BV(13)
#define PASINTCTL_CTRLUPD_ERR_INTR_FORCE	_BV(14)
#define PASINTCTL_RFM_ALERT_INTR_CLR		_BV(17)
#define PASINTCTL_RFM_ALERT_INTR_FORCE		_BV(18)
#define PASINTCTL_CAPARCMD_ERR_INTR_CLR		_BV(21)
#define PASINTCTL_CAPARCMD_ERR_INTR_FORCE	_BV(22)

#define SWCTL				0xC80
#define SWCTL_SW_DONE				_BV(0)

#define SWSTAT				0xC84
#define SWSTAT_SW_DONE_ACK			_BV(0)
#define SWSTAT_SW_DONE_ACK_OFFSET		0
#define SWSTAT_SW_DONE_ACK_MASK			GENMASK(0, 0)

struct ddr_irq_info {
	irq_t irq;
	caddr_t ctrl_base_addr;
	caddr_t reg_base_addr;
};

struct ddr_irq_info func_irq_info[FUNC_INTR_NUM] = {0};
struct ddr_irq_info ras_irq_info[RAS_INTR_NUM] = {0};
uint32_t func_irq[FUNC_INTR_NUM] = {
	IRQ_DDRA_INT,
	IRQ_DDRB_INT,
	IRQ_DDRC_INT,
	IRQ_DDRD_INT,
	IRQ_DDRE_INT,
	IRQ_DDRF_INT,
	IRQ_DDRG_INT,
	IRQ_DDRH_INT
};

uint32_t ras_irq[RAS_INTR_NUM] = {
	IRQ_DDRA_CH0_RAS,
	IRQ_DDRA_CH1_RAS,
	IRQ_DDRB_CH0_RAS,
	IRQ_DDRB_CH1_RAS,
	IRQ_DDRC_CH0_RAS,
	IRQ_DDRC_CH1_RAS,
	IRQ_DDRD_CH0_RAS,
	IRQ_DDRD_CH1_RAS,
	IRQ_DDRE_CH0_RAS,
	IRQ_DDRE_CH1_RAS,
	IRQ_DDRF_CH0_RAS,
	IRQ_DDRF_CH1_RAS,
	IRQ_DDRG_CH0_RAS,
	IRQ_DDRG_CH1_RAS,
	IRQ_DDRH_CH0_RAS,
	IRQ_DDRH_CH1_RAS
};

/**
 * struct ddr_error_info - DDR error information structure
 * @error_type: Type of error (correctable/uncorrectable)
 * @error_status: Error status register value
 * @error_address: Error address
 * @error_syndrome: Error syndrome
 * @error_count: Error count
 * @error_rank: Error rank
 * @error_bank: Error bank
 * @error_row: Error row
 * @error_col: Error column
 * @error_bg: Error bank group
 * @error_cid: Error chip ID
 * @adv_ecc_num_symbol: Number of error symbols (Advanced ECC only)
 * @adv_ecc_symbol_pos: Error symbol position (Advanced ECC only)
 * @adv_ecc_symbol_bits: Error symbol bits (Advanced ECC only)
 * @adv_ecc_syndrome_sel: Error syndrome selection (Advanced ECC only)
 * @adv_ecc_symbol_sel: Error symbol selection (Advanced ECC only)
 */
struct ddr_error_info {
	uint32_t error_type;
	uint32_t error_status;
	uint64_t error_address;
	uint64_t error_syndrome;
	uint32_t error_count;
	uint32_t error_rank;
	uint32_t error_bank;
	uint32_t error_row;
	uint32_t error_col;
	uint32_t error_bg;
	uint32_t error_cid;
	/* Advanced ECC specific fields */
	uint32_t adv_ecc_num_symbol;
	uint32_t adv_ecc_symbol_pos;
	uint32_t adv_ecc_symbol_bits;
	uint32_t adv_ecc_syndrome_sel;
	uint32_t adv_ecc_symbol_sel;
};

#define UNKNOWN_ERROR		0x0000
#define SINGLE_BIT_ERROR	0x0002
#define MULTIPLE_BIT_ERROR	0x0003

// DDR address mapping configuration
struct ddr_addr_map_config {
	uint8_t col_width;   // Column width (e.g., 10)
	uint8_t row_width;   // Row width (e.g., 18)
	uint8_t bank_width;  // Bank width (e.g., 2)
	uint8_t bg_width;	// Bank Group width (e.g., 3)
	uint8_t rank_width;  // Rank width (e.g., 4)
	uint8_t ch_width;	// Channel width (e.g., 1)
};

// DDR address components
struct ddr_addr_components {
	uint8_t channel;
	uint8_t rank;
	uint8_t bg;
	uint8_t bank;
	uint8_t row;
	uint8_t col;
};

uint8_t ddr_mask = 0;
struct ddr_addr_map_config cfg = {0};
struct ddr_addr_components comp = {0};

/**
 * @brief Calculate physical address from address components
 * @return physical address
 *
 * Bit mapping:
 * 31	 30	  29	 28	 27	 26	 25	 24
 * bg1	bg0	bank1  bank0  row15  row14  row13  row12
 * 23	 22	  21	 20	 19	 18	 17	 16
 * row11  row10  row9   row8   row7   row6   row5   row4
 * 15	  14	 13	 12	 11	 10	 9	  8
 * row3   row2   row1   row0   col9   col8   col7   col6
 * 7	   6	  5	  4	  3	   2	  1	 0
 * col5   col4   col3   col2   col1   col0	 -	 -
 *
 * Note:
 * - col[2:0] are unused
 * - col[9:6] and col[5:4] are valid
 */
uint64_t ddr_components_to_physaddr(void)
{
	uint64_t addr = 0;

	// Bank Group [31:30] - 2 bits (bg1, bg0)
	addr |= (comp.bg & 0x3) << 30;

	// Bank [29:28] - 2 bits (bank1, bank0)
	addr |= (comp.bank & 0x3) << 28;

	// Row [27:12] - 16 bits (row15:row0)
	addr |= (comp.row & 0xFFFF) << 12;

	// Column [11:2] - 10 bits (col9:col0)
	// Note: Last 2 bits (1:0) are unused per comment
	addr |= ((comp.col >> 6) & 0xF) << 8;   // col[9:6]
	addr |= ((comp.col >> 4) & 0x3) << 5;   // col[5:4]

	// Channel and Rank are not shown in the bit mapping comment,
	// so they are omitted here. If they should be included,
	// they would need to be placed in higher bits not shown in the diagram.

	return addr;
}

/**
 * @brief Parse physical address into address components
 * @param phys_addr physical address
 * @return address components
 */
void ddr_physaddr_to_components(uint64_t phys_addr)
{
	/* Clear all components first */
	memset(&comp, 0, sizeof(comp));

	/* Extract Bank Group [31:30] - 2 bits */
	comp.bg = (phys_addr >> 30) & 0x3;

	/* Extract Bank [29:28] - 2 bits */
	comp.bank = (phys_addr >> 28) & 0x3;

	/* Extract Row [27:12] - 16 bits */
	comp.row = (phys_addr >> 12) & 0xFFFF;

	/*
	 * Extract Column (only col[9:4] are valid)
	 * col[9:6] from bits [11:8] -> shift left 6
	 * col[5:4] from bits [6:5]  -> shift left 4
	 */
	comp.col = ((phys_addr >> 8) & 0xF) << 6;   // col[9:6]
	comp.col |= ((phys_addr >> 5) & 0x3) << 4;  // col[5:4]

	/*
	 * Channel and Rank are not shown in the bit mapping,
	 * so they are kept as 0. If needed, they should be
	 * extracted from higher bits (e.g., [63:32])
	 */
	comp.channel = 0;
	comp.rank = 0;
}

static void ddr_rw_test(uint64_t test_addr, uint8_t test_data, int test_times, bool write)
{
	uint8_t read_data;
	int i;

	con_log(DDR_MODNAME ": Starting DDR read/write test for %d times\n", test_times);
	for (i = 0; i < test_times; i++) {
		if (write) {
			*(volatile uint8_t *)(test_addr + i * 0x4) = test_data;
			con_log(DDR_MODNAME ": Test %d: Write 0x%02x at addr 0x%llx\n",
				i, test_data, test_addr + i * 0x4);
		} else {
			read_data = *(volatile uint8_t *)(test_addr + i * 0x4);
			con_log(DDR_MODNAME ": Test %d: Read 0x%02x at addr 0x%llx\n",
				i, read_data, test_addr + i * 0x4);
		}
	}
}

/**
 * @brief method to set swctl.sw_done to 0.
 * @param ch channel number
 * @note channel number if only applicable when DDRCTL_SINGLE_INST_DUALCH is set
 */
void ddrc_pre_qdyn_write(caddr_t ddrc_base)
{
	ddr_clear(SWCTL_SW_DONE, ddrc_base + REGB_DDRC_CH_OFFSET(0) + SWCTL);
}

/**
 * @brief method to set swctl.sw_done and wait for swstat.sw_done_ack.
 * @param ch channel number
 * @note channel number if only applicable when DDRCTL_SINGLE_INST_DUALCH is set
 */
bool ddrc_post_qdyn_write(caddr_t ddrc_base)
{
	uint32_t val;

	ddr_set(SWCTL_SW_DONE, ddrc_base + REGB_DDRC_CH_OFFSET(0) + SWCTL);

	val = ddr_read_field(SWSTAT_SW_DONE_ACK_OFFSET, SWSTAT_SW_DONE_ACK_MASK, ddrc_base + REGB_DDRC_CH_OFFSET(0) + SWSTAT);
	if (val != 0x1) {
		con_log(DDR_MODNAME ": Error in seq_poll_sw_done_ack\n");
		return false;
	}

	return true;
}

/**
 * ddr_collect_error_info - collect DDR error information
 * @ddrc_base: DDR controller base address
 * @ch_index: Channel index
 * @info: Pointer to DDR error info structure
 * @ecc_mode: ECC mode (SECDED_ECC_MODE or ADVECC_ECC_MODE)
 *
 * Collect DDR error information from ECC related registers, including:
 * - Error type (correctable/uncorrectable)
 * - Error Address (rank/bank/row/col)
 * - Error syndrome
 * - Error count
 * For Advanced ECC mode, also collect:
 * - Error symbol position and bits
 * - Error syndrome selection
 * - Error symbol selection
 */
static void ddr_collect_error_info(caddr_t ddrc_base, uint8_t ch_index,
				  struct ddr_error_info *info, uint32_t ecc_mode)
{
	uint32_t ecc_stat;
	uint32_t ecc_addr0, ecc_addr1;
	uint32_t ecc_syn0, ecc_syn1, ecc_syn2;
	uint32_t ecc_err_cnt;
	uint32_t adv_ecc_stat, adv_ecc_index;

	/* 1. Read ECC status register */
	ecc_stat = __raw_readl(ddrc_base + REGB_DDRC_CH_OFFSET(ch_index) + ECCSTAT);
	ecc_err_cnt = __raw_readl(ddrc_base + REGB_DDRC_CH_OFFSET(ch_index) + ECCERRCNT);
	con_log(DDR_MODNAME ": ECCSTAT:0x%08x, ECCERRCNT:0x%08x\n", ecc_stat, ecc_err_cnt);

	/* 2. Determine error type */
	if (ecc_stat & ECCSTAT_ECC_CORRECTED_ERR_MASK) {
		info->error_type = SINGLE_BIT_ERROR;
		/* Read correctable error information */
		ecc_addr0 = __raw_readl(ddrc_base + REGB_DDRC_CH_OFFSET(ch_index) + ECCCADDR0);
		ecc_addr1 = __raw_readl(ddrc_base + REGB_DDRC_CH_OFFSET(ch_index) + ECCCADDR1);
		ecc_syn0 = __raw_readl(ddrc_base + REGB_DDRC_CH_OFFSET(ch_index) + ECCCSYN0);
		ecc_syn1 = __raw_readl(ddrc_base + REGB_DDRC_CH_OFFSET(ch_index) + ECCCSYN1);
		ecc_syn2 = __raw_readl(ddrc_base + REGB_DDRC_CH_OFFSET(ch_index) + ECCCSYN2);

		/* For Advanced ECC, read additional error information */
		if (ecc_mode == ADVECC_ECC_MODE) {
			adv_ecc_stat = __raw_readl(ddrc_base + REGB_DDRC_CH_OFFSET(ch_index) + ADVECCSTAT);
			adv_ecc_index = __raw_readl(ddrc_base + REGB_DDRC_CH_OFFSET(ch_index) + ADVECCINDEX);

			/* Store Advanced ECC specific information */
			info->adv_ecc_num_symbol = (adv_ecc_stat & ADVECCSTAT_ADVECC_NUM_ERR_SYMBOL_MASK) >> ADVECCSTAT_ADVECC_NUM_ERR_SYMBOL_OFFSET;
			info->adv_ecc_symbol_pos = (adv_ecc_stat & ADVECCSTAT_ADVECC_ERR_SYMBOL_POS_MASK) >> ADVECCSTAT_ADVECC_ERR_SYMBOL_POS_OFFSET;
			info->adv_ecc_symbol_bits = (adv_ecc_stat & ADVECCSTAT_ADVECC_ERR_SYMBOL_BITS_MASK) >> ADVECCSTAT_ADVECC_ERR_SYMBOL_BITS_OFFSET;
			info->adv_ecc_syndrome_sel = (adv_ecc_index & ADVECCINDEX_ECC_SYNDROME_SEL_MASK) >> ADVECCINDEX_ECC_SYNDROME_SEL_OFFSET;
			info->adv_ecc_symbol_sel = (adv_ecc_index & ADVECCINDEX_ECC_ERR_SYMBOL_SEL_MASK) >> ADVECCINDEX_ECC_ERR_SYMBOL_SEL_OFFSET;
		}
	} else if (ecc_stat & ECCSTAT_ECC_UNCORRECTED_ERR_MASK) {
		info->error_type = MULTIPLE_BIT_ERROR;
		/* Read uncorrectable error information */
		ecc_addr0 = __raw_readl(ddrc_base + REGB_DDRC_CH_OFFSET(ch_index) + ECCUADDR0);
		ecc_addr1 = __raw_readl(ddrc_base + REGB_DDRC_CH_OFFSET(ch_index) + ECCUADDR1);
		ecc_syn0 = __raw_readl(ddrc_base + REGB_DDRC_CH_OFFSET(ch_index) + ECCUSYN0);
		ecc_syn1 = __raw_readl(ddrc_base + REGB_DDRC_CH_OFFSET(ch_index) + ECCUSYN1);
		ecc_syn2 = __raw_readl(ddrc_base + REGB_DDRC_CH_OFFSET(ch_index) + ECCUSYN2);

		/* For Advanced ECC, read additional error information */
		if (ecc_mode == ADVECC_ECC_MODE) {
			adv_ecc_stat = __raw_readl(ddrc_base + REGB_DDRC_CH_OFFSET(ch_index) + ADVECCSTAT);
			adv_ecc_index = __raw_readl(ddrc_base + REGB_DDRC_CH_OFFSET(ch_index) + ADVECCINDEX);

			/* Store Advanced ECC specific information */
			info->adv_ecc_num_symbol = (adv_ecc_stat & ADVECCSTAT_ADVECC_NUM_ERR_SYMBOL_MASK) >> 2;
			info->adv_ecc_symbol_pos = (adv_ecc_stat & ADVECCSTAT_ADVECC_ERR_SYMBOL_POS_MASK) >> 5;
			info->adv_ecc_symbol_bits = (adv_ecc_stat & ADVECCSTAT_ADVECC_ERR_SYMBOL_BITS_MASK) >> 16;
			info->adv_ecc_syndrome_sel = adv_ecc_index & ADVECCINDEX_ECC_SYNDROME_SEL_MASK;
			info->adv_ecc_symbol_sel = (adv_ecc_index & ADVECCINDEX_ECC_ERR_SYMBOL_SEL_MASK) >> 3;
		}
	} else {
		info->error_type = UNKNOWN_ERROR;
		return;
	}

	/* 3. Fill error info structure */
	info->error_status = ecc_stat;
	info->error_rank = (ecc_addr0 & ECCCADDR0_ECC_CORR_RANK_MASK) >> ECCCADDR0_ECC_CORR_RANK_OFFSET;
	info->error_bank = (ecc_addr1 & ECCCADDR1_ECC_CORR_BANK_MASK) >> ECCCADDR1_ECC_CORR_BANK_OFFSET;
	info->error_row = (ecc_addr0 & ECCCADDR0_ECC_CORR_ROW_MASK) >> ECCCADDR0_ECC_CORR_ROW_OFFSET;
	info->error_col = (ecc_addr1 & ECCCADDR1_ECC_CORR_COL_MASK) >> ECCCADDR1_ECC_CORR_COL_OFFSET;
	info->error_bg = (ecc_addr1 & ECCCADDR1_ECC_CORR_BG_MASK) >> ECCCADDR1_ECC_CORR_BG_OFFSET;
	info->error_cid = (ecc_addr1 & ECCCADDR1_ECC_CORR_CID_MASK) >> ECCCADDR1_ECC_CORR_CID_OFFSET;
	/* Build physical address: rank[31:24] | bank[23:16] | row[15:0] */
	info->error_address = (info->error_rank  << 24) |
				 (info->error_bank<< 16) |
				 (info->error_row << 0);

	/* Build syndrome: syn2[71:64] | syn1[63:32] | syn0[31:0] */
	info->error_syndrome = ((uint64_t)(ecc_syn2 & ECCCSYN2_ECC_CORR_SYNDROME_71_64_MASK) << 56) |
				  ((uint64_t)ecc_syn1 << 32) |
				  (uint64_t)ecc_syn0;

	/* Error count */
	if (info->error_type == SINGLE_BIT_ERROR)
		info->error_count = (ecc_err_cnt & ECCERRCNT_ECC_CORR_ERR_CNT_MASK);
	else
		info->error_count = (ecc_err_cnt & ECCERRCNT_ECC_UNCORR_ERR_CNT_MASK) >> 16;

	/* 4. Clear error status */
	if (info->error_type == SINGLE_BIT_ERROR) {
		ddr_set(ECCCTL_ECC_CORRECTED_ERR_CLR,
			ddrc_base + REGB_DDRC_CH_OFFSET(ch_index) + ECCCTL);
	} else {
		ddr_set(ECCCTL_ECC_UNCORRECTED_ERR_CLR,
			ddrc_base + REGB_DDRC_CH_OFFSET(ch_index) + ECCCTL);
	}
}

/**
 * ddr_report_error - report DDR error
 * @ddrc_base: DDR controller base address
 * @ch_index: Channel index
 * @ecc_mode: ECC mode (SECDED_ECC_MODE or ADVECC_ECC_MODE)
 *
 * Collect DDR error information and report through GHES
 */
void ddr_report_error(caddr_t ddrc_base, uint8_t ch_index, uint32_t ecc_mode)
{
	struct ddr_error_info info;
	acpi_ghes_error_info einfo;
	uint32_t acpi_ghes_source_id;

	memset(&info, 0, sizeof(info));
	memset(&einfo, 0, sizeof(einfo));

	/* 1. Collect error information */
	ddr_collect_error_info(ddrc_base, ch_index, &info, ecc_mode);

	/* 2. Initialize error info structure */
	einfo.etype = ERROR_TYPE_MEM;

	/* 3. Set error severity */
	if (info.error_type == SINGLE_BIT_ERROR)
		einfo.info.me.err_sev = GHES_SEV_CORRECTED;
	else
		einfo.info.me.err_sev = GHES_SEV_PANIC;

	/* 4. Set validation bits */
	einfo.info.me.validation_bits = CPER_MEM_VALID_PA
					| CPER_MEM_VALID_ERROR_TYPE
					| CPER_MEM_VALID_BANK
					| CPER_MEM_VALID_BANK_GROUP
					| CPER_MEM_VALID_RANK_NUMBER
					| CPER_MEM_VALID_ROW
					| CPER_MEM_VALID_COLUMN;

	/* 5. Set physical address */
	einfo.info.me.physical_address = info.error_address;
	einfo.info.me.physical_address_mask = ~0ULL;
	einfo.info.me.node = 0;
	einfo.info.me.card = 0;
	einfo.info.me.module = 0;
	einfo.info.me.bank = info.error_bank;
	einfo.info.me.device = 0;
	einfo.info.me.row = info.error_row;
	einfo.info.me.column = info.error_col;
	einfo.info.me.rank = info.error_rank;
	einfo.info.me.bit_pos = 0;
	// einfo.info.me.chip_id = 0;
	einfo.info.me.err_type = info.error_type;
	einfo.info.me.err_status = 0;
	/* 6. Set source ID (DDR channel) */
	acpi_ghes_source_id = 0x60 + ch_index;
	/* 7. Report error */
	acpi_ghes_record_errors(acpi_ghes_source_id, &einfo);
}

void ddr_func_intr_handle_irq(irq_t irq)
{
	uint32_t status;
	int i;
	caddr_t ctrl_base = 0, reg_base = 0;
	bool found = false;

	for (i = 0; i < ARRAY_SIZE(func_irq_info); i++) {
		if (irq == func_irq_info[i].irq) {
			ctrl_base = ddrc_addr[i].ctrl;
			reg_base = ddrc_addr[i].reg;
			found = true;
			break;
		}
	}

	if (!found) {
		con_log(DDR_MODNAME ": Invalid IRQ %d\n", irq);
		return;
	}

	status = __raw_readl(reg_base + FUNC_INTR_STAT);
	con_log(DDR_MODNAME ": FUNC INT STATUS:0x%08x\n", status);

	if (status == 0)
		return;

	if (status & DFI_ALERT_ERR_INTR)
		/* DDRC doesnot support */;
	if (status & SWCMD_ERR_INTR) {
		con_log(DDR_MODNAME ": process SWCMD error interrupt: reg_value:0x%08x\n", __raw_readl(ctrl_base + REGB_DDRC_CH_OFFSET(0) + PASINTCTL));
		ddr_set(PASINTCTL_SWCMD_ERR_INTR_CLR, ctrl_base + REGB_DDRC_CH_OFFSET(0) + PASINTCTL);
		con_log(DDR_MODNAME ": PASINTCTL:0x%08x\n", __raw_readl(ctrl_base + REGB_DDRC_CH_OFFSET(0) + PASINTCTL));
	}
	if (status & DUCMD_ERR_INTR) {
		con_log(DDR_MODNAME ": process DUCMD error interrupt: reg_value:0x%08x\n", __raw_readl(ctrl_base + REGB_DDRC_CH_OFFSET(0) + PASINTCTL));
		ddr_set(PASINTCTL_DUCMD_ERR_INTR_CLR, ctrl_base + REGB_DDRC_CH_OFFSET(0) + PASINTCTL);
		con_log(DDR_MODNAME ": PASINTCTL:0x%08x\n", __raw_readl(ctrl_base + REGB_DDRC_CH_OFFSET(0) + PASINTCTL));
	}
	if (status & LCCMD_ERR_INTR) {
		con_log(DDR_MODNAME ": process LCCMD error interrupt: reg_value:0x%08x\n", __raw_readl(ctrl_base + REGB_DDRC_CH_OFFSET(0) + PASINTCTL));
		ddr_set(PASINTCTL_LCCMD_ERR_INTR_CLR, ctrl_base + REGB_DDRC_CH_OFFSET(0) + PASINTCTL);
		con_log(DDR_MODNAME ": PASINTCTL:0x%08x\n", __raw_readl(ctrl_base + REGB_DDRC_CH_OFFSET(0) + PASINTCTL));
	}
	if (status & CTRLUPD_ERR_INTR) {
		con_log(DDR_MODNAME ": process CTRLUPD error interrupt: reg_value:0x%08x\n", __raw_readl(ctrl_base + REGB_DDRC_CH_OFFSET(0) + PASINTCTL));
		ddr_set(PASINTCTL_CTRLUPD_ERR_INTR_CLR, ctrl_base + REGB_DDRC_CH_OFFSET(0) + PASINTCTL);
		con_log(DDR_MODNAME ": PASINTCTL:0x%08x\n", __raw_readl(ctrl_base + REGB_DDRC_CH_OFFSET(0) + PASINTCTL));
	}
	if (status & RFM_ALERT_INTR) {
		con_log(DDR_MODNAME ": process RFM alert interrupt: reg_value:0x%08x\n", __raw_readl(ctrl_base + REGB_DDRC_CH_OFFSET(0) + PASINTCTL));
		ddr_set(PASINTCTL_RFM_ALERT_INTR_CLR, ctrl_base + REGB_DDRC_CH_OFFSET(0) + PASINTCTL);
		con_log(DDR_MODNAME ": PASINTCTL:0x%08x\n", __raw_readl(ctrl_base + REGB_DDRC_CH_OFFSET(0) + PASINTCTL));
	}
	if (status & SBR_DONE_INTR)
		/* DDRC doesnot support */;
	if (status & DFI_ALERT_ERR_INTR_DCH1)
		/* DDRC doesnot support */;
	if (status & SWCMD_ERR_INTR_DCH1) {
		con_log(DDR_MODNAME ": process SWCMD error interrupt: reg_value:0x%08x\n", __raw_readl(ctrl_base + REGB_DDRC_CH_OFFSET(1) + PASINTCTL));
		ddr_set(PASINTCTL_SWCMD_ERR_INTR_CLR, ctrl_base + REGB_DDRC_CH_OFFSET(1) + PASINTCTL);
		con_log(DDR_MODNAME ": PASINTCTL:0x%08x\n", __raw_readl(ctrl_base + REGB_DDRC_CH_OFFSET(1) + PASINTCTL));
	}
	if (status & DUCMD_ERR_INTR_DCH1) {
		con_log(DDR_MODNAME ": process DUCMD error interrupt: reg_value:0x%08x\n", __raw_readl(ctrl_base + REGB_DDRC_CH_OFFSET(1) + PASINTCTL));
		ddr_set(PASINTCTL_DUCMD_ERR_INTR_CLR, ctrl_base + REGB_DDRC_CH_OFFSET(1) + PASINTCTL);
		con_log(DDR_MODNAME ": PASINTCTL:0x%08x\n", __raw_readl(ctrl_base + REGB_DDRC_CH_OFFSET(1) + PASINTCTL));
	}
	if (status & LCCMD_ERR_INTR_DCH1) {
		con_log(DDR_MODNAME ": process LCCMD error interrupt: reg_value:0x%08x\n", __raw_readl(ctrl_base + REGB_DDRC_CH_OFFSET(1) + PASINTCTL));
		ddr_set(PASINTCTL_LCCMD_ERR_INTR_CLR, ctrl_base + REGB_DDRC_CH_OFFSET(1) + PASINTCTL);
		con_log(DDR_MODNAME ": PASINTCTL:0x%08x\n", __raw_readl(ctrl_base + REGB_DDRC_CH_OFFSET(1) + PASINTCTL));
	}
	if (status & CTRLUPD_ERR_INTR_DCH1) {
		con_log(DDR_MODNAME ": process CTRLUPD error interrupt: reg_value:0x%08x\n", __raw_readl(ctrl_base + REGB_DDRC_CH_OFFSET(1) + PASINTCTL));
		ddr_set(PASINTCTL_CTRLUPD_ERR_INTR_CLR, ctrl_base + REGB_DDRC_CH_OFFSET(1) + PASINTCTL);
		con_log(DDR_MODNAME ": PASINTCTL:0x%08x\n", __raw_readl(ctrl_base + REGB_DDRC_CH_OFFSET(1) + PASINTCTL));
	}
	if (status & RFM_ALERT_INTR_DCH1) {
		con_log(DDR_MODNAME ": process RFM alert interrupt: reg_value:0x%08x\n", __raw_readl(ctrl_base + REGB_DDRC_CH_OFFSET(1) + PASINTCTL));
		ddr_set(PASINTCTL_RFM_ALERT_INTR_CLR, ctrl_base + REGB_DDRC_CH_OFFSET(1) + PASINTCTL);
		con_log(DDR_MODNAME ": PASINTCTL:0x%08x\n", __raw_readl(ctrl_base + REGB_DDRC_CH_OFFSET(1) + PASINTCTL));
	}
	if (status & DERATE_TEMP_LIMIT_INTR) {
		con_log(DDR_MODNAME ": process Derate temp limit interrupt: reg_value:0x%08x\n", __raw_readl(ctrl_base + REGB_DDRC_CH_OFFSET(0) + DERATECTL5));
		ddr_set(DERATECTL5_DERATE_TEMP_LIMIT_INTR_CLR, ctrl_base + REGB_DDRC_CH_OFFSET(0) + DERATECTL5);
		con_log(DDR_MODNAME ": DERATECTL5:0x%08x\n", __raw_readl(ctrl_base + REGB_DDRC_CH_OFFSET(0) + DERATECTL5));
	}
	if (status & DERATE_TEMP_LIMIT_INTR_DCH1) {
		con_log(DDR_MODNAME ": process Derate temp limit interrupt: reg_value:0x%08x\n", __raw_readl(ctrl_base + REGB_DDRC_CH_OFFSET(1) + DERATECTL5));
		ddr_set(DERATECTL5_DERATE_TEMP_LIMIT_INTR_CLR, ctrl_base + REGB_DDRC_CH_OFFSET(1) + DERATECTL5);
		con_log(DDR_MODNAME ": DERATECTL5:0x%08x\n", __raw_readl(ctrl_base + REGB_DDRC_CH_OFFSET(1) + DERATECTL5));
	}
	if (status & SBR_DONE_INTR_DCH1)
		/* DDRC doesnot support */;
	if (status & DPM_INTR)
		/* DDRC doesnot support */;
	if (status & DDRPHY_INTR)
		/* DDRC doesnot support */;

	irqc_ack_irq(irq);
	con_log(DDR_MODNAME ": ack func irq: %d\n", irq_ext(irq));
}

void ddr_ras_intr_handle_irq(irq_t irq)
{
	uint32_t status,par_status;
	int i;
	caddr_t ctrl_base = 0, reg_base = 0;
	bool found = false;

	for (i = 0; i < ARRAY_SIZE(ras_irq_info); i++) {
		if (irq == ras_irq_info[i].irq) {
			ctrl_base = ddrc_addr[i].ctrl;
			reg_base = ddrc_addr[i].reg;
			found = true;
			break;
		}
	}

	if (!found) {
		con_log(DDR_MODNAME ": Invalid RAS IRQ %d\n", irq);
		return;
	}

	status = __raw_readl(reg_base + RAS_INTR_STAT);
	par_status = __raw_readl(reg_base + WRB_RDB_PAR_INT_STAT);
	con_log(DDR_MODNAME ": RAS INT STATUS:0x%08x, PAR INT STATUS:0x%08x\n", status, par_status);

	if (status == 0 && par_status == 0)
		return;

	/* process error interrupt */
	if (status & ECC_CORRECTED_ERR_INTR) {
		con_log(DDR_MODNAME ": process ECC corrected error interrupt\n");
		con_log(DDR_MODNAME ": ECCCTL:0x%08x\n", __raw_readl(ctrl_base + REGB_DDRC_CH_OFFSET(0) + ECCCTL));
		ddr_set(ECCCTL_ECC_CORRECTED_ERR_CLR, ctrl_base + REGB_DDRC_CH_OFFSET(0) + ECCCTL);
		con_log(DDR_MODNAME ": ECCCTL:0x%08x\n", __raw_readl(ctrl_base + REGB_DDRC_CH_OFFSET(0) + ECCCTL));
		ddr_report_error(ctrl_base, 0, SECDED_ECC_MODE);
	}
	if (status & ECC_UNCORRECTED_ERR_INTR) {
		con_log(DDR_MODNAME ": process ECC uncorrected error interrupt\n");
		con_log(DDR_MODNAME ": ECCCTL:0x%08x\n", __raw_readl(ctrl_base + REGB_DDRC_CH_OFFSET(0) + ECCCTL));
		ddr_set(ECCCTL_ECC_UNCORRECTED_ERR_CLR, ctrl_base + REGB_DDRC_CH_OFFSET(0) + ECCCTL);
		con_log(DDR_MODNAME ": ECCCTL:0x%08x\n", __raw_readl(ctrl_base + REGB_DDRC_CH_OFFSET(0) + ECCCTL));
	}
	if (status & WR_CRC_ERR_INTR) {
		con_log(DDR_MODNAME ": process WR CRC error interrupt\n");
		con_log(DDR_MODNAME ": CRCPARCTL0:0x%08x\n", __raw_readl(ctrl_base + REGB_DDRC_CH_OFFSET(0) + CRCPARCTL0));
		ddr_set(CRCPARCTL0_WR_CRC_ERR_INTR_CLR, ctrl_base + REGB_DDRC_CH_OFFSET(0) + CRCPARCTL0);
		con_log(DDR_MODNAME ": CRCPARCTL0:0x%08x\n", __raw_readl(ctrl_base + REGB_DDRC_CH_OFFSET(0) + CRCPARCTL0));
	}
	if (status & WR_CRC_ERR_MAX_REACHED_INTR) {
		con_log(DDR_MODNAME ": process WR CRC error max reached interrupt\n");
		con_log(DDR_MODNAME ": CRCPARCTL0:0x%08x\n", __raw_readl(ctrl_base + REGB_DDRC_CH_OFFSET(0) + CRCPARCTL0));
		ddr_set(CRCPARCTL0_WR_CRC_ERR_MAX_REACHED_INTR_CLR, ctrl_base + REGB_DDRC_CH_OFFSET(0) + CRCPARCTL0);
		con_log(DDR_MODNAME ": CRCPARCTL0:0x%08x\n", __raw_readl(ctrl_base + REGB_DDRC_CH_OFFSET(0) + CRCPARCTL0));
	}
	if (status & CAPAR_ERR_INTR) {
		con_log(DDR_MODNAME ": process CAPAR error interrupt\n");
		con_log(DDR_MODNAME ": CRCPARCTL0:0x%08x\n", __raw_readl(ctrl_base + REGB_DDRC_CH_OFFSET(0) + CRCPARCTL0));
		ddr_set(CRCPARCTL0_CAPAR_ERR_INTR_CLR, ctrl_base + REGB_DDRC_CH_OFFSET(0) + CRCPARCTL0);
		con_log(DDR_MODNAME ": CRCPARCTL0:0x%08x\n", __raw_readl(ctrl_base + REGB_DDRC_CH_OFFSET(0) + CRCPARCTL0));
	}
	if (status & CAPAR_ERR_MAX_REACHED_INTR) {
		con_log(DDR_MODNAME ": process CAPAR error max reached interrupt\n");
		con_log(DDR_MODNAME ": CRCPARCTL0:0x%08x\n", __raw_readl(ctrl_base + REGB_DDRC_CH_OFFSET(0) + CRCPARCTL0));
		ddr_set(CRCPARCTL0_CAPAR_ERR_MAX_REACHED_INTR_CLR, ctrl_base + REGB_DDRC_CH_OFFSET(0) + CRCPARCTL0);
		con_log(DDR_MODNAME ": CRCPARCTL0:0x%08x\n", __raw_readl(ctrl_base + REGB_DDRC_CH_OFFSET(0) + CRCPARCTL0));
	}
	if (status & CAPAR_FATL_ERR_INTR) {
		con_log(DDR_MODNAME ": process CAPAR fatal error interrupt\n");
		con_log(DDR_MODNAME ": CRCPARCTL0:0x%08x\n", __raw_readl(ctrl_base + REGB_DDRC_CH_OFFSET(0) + CRCPARCTL0));
		ddr_set(CRCPARCTL0_CAPAR_FATL_ERR_INTR_CLR, ctrl_base + REGB_DDRC_CH_OFFSET(0) + CRCPARCTL0);
		con_log(DDR_MODNAME ": CRCPARCTL0:0x%08x\n", __raw_readl(ctrl_base + REGB_DDRC_CH_OFFSET(0) + CRCPARCTL0));
	}
	if (status & CAPAR_RETRY_LIMIT_REACHED_INTR) {
		con_log(DDR_MODNAME ": process CAPAR retry limit reached interrupt\n");
		con_log(DDR_MODNAME ": RETRYCTL0:0x%08x\n", __raw_readl(ctrl_base + REGB_DDRC_CH_OFFSET(0) + RETRYCTL0));
		ddr_set(RETRYCTL0_CAPAR_RETRY_LIMIT_INTR_CLR, ctrl_base + REGB_DDRC_CH_OFFSET(0) + RETRYCTL0);
		con_log(DDR_MODNAME ": RETRYCTL0:0x%08x\n", __raw_readl(ctrl_base + REGB_DDRC_CH_OFFSET(0) + RETRYCTL0));
	}
	if (status & CAPARCMD_ERR_INTR) {
		con_log(DDR_MODNAME ": process CAPAR command error interrupt\n");
		con_log(DDR_MODNAME ": PASINTCTL:0x%08x\n", __raw_readl(ctrl_base + REGB_DDRC_CH_OFFSET(0) + PASINTCTL));
		ddr_set(PASINTCTL_CAPARCMD_ERR_INTR_CLR, ctrl_base + REGB_DDRC_CH_OFFSET(0) + PASINTCTL);
		con_log(DDR_MODNAME ": PASINTCTL:0x%08x\n", __raw_readl(ctrl_base + REGB_DDRC_CH_OFFSET(0) + PASINTCTL));
	}
	if (status & WR_CRC_RETRY_LIMIT_INTR) {
		con_log(DDR_MODNAME ": process WR CRC retry limit interrupt\n");
		con_log(DDR_MODNAME ": RETRYCTL0:0x%08x\n", __raw_readl(ctrl_base + REGB_DDRC_CH_OFFSET(0) + RETRYCTL0));
		ddr_set(RETRYCTL0_WR_CRC_RETRY_LIMIT_INTR_CLR, ctrl_base + REGB_DDRC_CH_OFFSET(0) + RETRYCTL0);
		con_log(DDR_MODNAME ": RETRYCTL0:0x%08x\n", __raw_readl(ctrl_base + REGB_DDRC_CH_OFFSET(0) + RETRYCTL0));
	}
	if (status & RD_CRC_ERR_MAX_REACHED_INTR) {
		con_log(DDR_MODNAME ": process RD CRC error max reached interrupt\n");
		con_log(DDR_MODNAME ": CRCPARCTL0:0x%08x\n", __raw_readl(ctrl_base + REGB_DDRC_CH_OFFSET(0) + CRCPARCTL0));
		ddr_set(CRCPARCTL0_RD_CRC_ERR_MAX_REACHED_INT_CLR, ctrl_base + REGB_DDRC_CH_OFFSET(0) + CRCPARCTL0);
		con_log(DDR_MODNAME ": CRCPARCTL0:0x%08x\n", __raw_readl(ctrl_base + REGB_DDRC_CH_OFFSET(0) + CRCPARCTL0));
	}
	if (status & RD_RETRY_LIMIT_INTR) {
		con_log(DDR_MODNAME ": process RD retry limit interrupt\n");
		con_log(DDR_MODNAME ": RETRYCTL0:0x%08x\n", __raw_readl(ctrl_base + REGB_DDRC_CH_OFFSET(0) + RETRYCTL0));
		ddr_set(RETRYCTL0_RD_RETRY_LIMIT_INTR_CLR, ctrl_base + REGB_DDRC_CH_OFFSET(0) + RETRYCTL0);
		con_log(DDR_MODNAME ": RETRYCTL0:0x%08x\n", __raw_readl(ctrl_base + REGB_DDRC_CH_OFFSET(0) + RETRYCTL0));
	}

	/* process parity error */
	if (par_status & WRB_P0R0_PAR_ERR_INTR) {
		con_log(DDR_MODNAME ": WRB P0R0 PAR ERR CNT:0x%08x, channel: %d\n", __raw_readl(reg_base + WRB_P0R0_PAR_ERR_CNT), i);
		ddr_set(WRB_P0R0_PAR_ERR_INTR, reg_base + WRB_RDB_PAR_INT_STAT);
		ddr_clear(WRB_P0R0_PAR_POIS_EN, reg_base + PAR_POISON_EN);
		con_log(DDR_MODNAME ": WRB_RDB_PAR_INT_STAT:0x%08x\n", __raw_readl(reg_base + WRB_RDB_PAR_INT_STAT));
	}
	if (par_status & WRB_P0R1_PAR_ERR_INTR) {
		con_log(DDR_MODNAME ": WRB P0R1 PAR ERR CNT:0x%08x, channel: %d\n", __raw_readl(reg_base + WRB_P0R1_PAR_ERR_CNT), i);
		ddr_set(WRB_P0R1_PAR_ERR_INTR, reg_base + WRB_RDB_PAR_INT_STAT);
		ddr_clear(WRB_P0R1_PAR_POIS_EN, reg_base + PAR_POISON_EN);
		con_log(DDR_MODNAME ": WRB_RDB_PAR_INT_STAT:0x%08x\n", __raw_readl(reg_base + WRB_RDB_PAR_INT_STAT));
	}
	if (par_status & WRB_P1R0_PAR_ERR_INTR) {
		con_dbg(DDR_MODNAME ": WRB P1R0 PAR ERR CNT:0x%08x, channel: %d\n", __raw_readl(reg_base + WRB_P1R0_PAR_ERR_CNT), i);
		ddr_set(WRB_P1R0_PAR_ERR_INTR, reg_base + WRB_RDB_PAR_INT_STAT);
		ddr_clear(WRB_P1R0_PAR_POIS_EN, reg_base + PAR_POISON_EN);
		con_log(DDR_MODNAME ": WRB_RDB_PAR_INT_STAT:0x%08x\n", __raw_readl(reg_base + WRB_RDB_PAR_INT_STAT));
	}
	if (par_status & WRB_P1R1_PAR_ERR_INTR) {
		con_dbg(DDR_MODNAME ": WRB P1R1 PAR ERR CNT:0x%08x, channel: %d\n", __raw_readl(reg_base + WRB_P1R1_PAR_ERR_CNT), i);
		ddr_set(WRB_P1R1_PAR_ERR_INTR, reg_base + WRB_RDB_PAR_INT_STAT);
		ddr_clear(WRB_P1R1_PAR_POIS_EN, reg_base + PAR_POISON_EN);
		con_log(DDR_MODNAME ": WRB_RDB_PAR_INT_STAT:0x%08x\n", __raw_readl(reg_base + WRB_RDB_PAR_INT_STAT));
	}
	if (par_status & RDB_P0R0_PAR_ERR_INTR) {
		con_dbg(DDR_MODNAME ": RDB P0R0 PAR ERR CNT:0x%08x, channel: %d\n", __raw_readl(reg_base + RDB_P0R0_PAR_ERR_CNT), i);
		ddr_set(RDB_P0R0_PAR_ERR_INTR, reg_base + WRB_RDB_PAR_INT_STAT);
		ddr_clear(RDB_P0R0_PAR_POIS_EN, reg_base + PAR_POISON_EN);
		con_log(DDR_MODNAME ": WRB_RDB_PAR_INT_STAT:0x%08x\n", __raw_readl(reg_base + WRB_RDB_PAR_INT_STAT));
	}
	if (par_status & RDB_P0R1_PAR_ERR_INTR) {
		con_dbg(DDR_MODNAME ": RDB P0R1 PAR ERR CNT:0x%08x, channel: %d\n", __raw_readl(reg_base + RDB_P0R1_PAR_ERR_CNT), i);
		ddr_set(RDB_P0R1_PAR_ERR_INTR, reg_base + WRB_RDB_PAR_INT_STAT);
		ddr_clear(RDB_P0R1_PAR_POIS_EN, reg_base + PAR_POISON_EN);
		con_log(DDR_MODNAME ": WRB_RDB_PAR_INT_STAT:0x%08x\n", __raw_readl(reg_base + WRB_RDB_PAR_INT_STAT));
	}
	if (par_status & RDB_P1R0_PAR_ERR_INTR) {
		con_dbg(DDR_MODNAME ": RDB P1R0 PAR ERR CNT:0x%08x, channel: %d\n", __raw_readl(reg_base + RDB_P1R0_PAR_ERR_CNT), i);
		ddr_set(RDB_P1R0_PAR_ERR_INTR, reg_base + WRB_RDB_PAR_INT_STAT);
		ddr_clear(RDB_P1R0_PAR_POIS_EN, reg_base + PAR_POISON_EN);
		con_log(DDR_MODNAME ": WRB_RDB_PAR_INT_STAT:0x%08x\n", __raw_readl(reg_base + WRB_RDB_PAR_INT_STAT));
	}
	if (par_status & RDB_P1R1_PAR_ERR_INTR) {
		con_dbg(DDR_MODNAME ": RDB P1R1 PAR ERR CNT:0x%08x, channel: %d\n", __raw_readl(reg_base + RDB_P1R1_PAR_ERR_CNT), i);
		ddr_set(RDB_P1R1_PAR_ERR_INTR, reg_base + WRB_RDB_PAR_INT_STAT);
		ddr_clear(RDB_P1R1_PAR_POIS_EN, reg_base + PAR_POISON_EN);
		con_log(DDR_MODNAME ": WRB_RDB_PAR_INT_STAT:0x%08x\n", __raw_readl(reg_base + WRB_RDB_PAR_INT_STAT));
	}

	irqc_ack_irq(irq);
	con_log(DDR_MODNAME ": ack ras irq: %d\n", irq_ext(irq));
}

#ifdef SYS_REALTIME
#define ddr_irq_init()	do {} while (0)
#else
static void ddr_irq_init(void)
{
	uint8_t i;

#if defined(CONFIG_K1MATRIX_PZ1) || defined(CONFIG_K1MATRIX_ASIC)
	/* Initialize RAS interrupt info for all DDR controllers */
	for (i = 0; i < ARRAY_SIZE(func_irq); i++) {
		if (!(ddr_mask & (1 << i)))
			continue;
		irqc_configure_irq(func_irq[i], 0, IRQ_LEVEL_TRIGGERED);
		irq_register_vector(func_irq[i], ddr_func_intr_handle_irq);
		irqc_enable_irq(func_irq[i]);
	}
#endif
	for (i = 0; i < ARRAY_SIZE(ras_irq); i++) {
		if (!(ddr_mask & (1 << (i * 2))))
			continue;
		irqc_configure_irq(ras_irq[i], 0, IRQ_LEVEL_TRIGGERED);
		irq_register_vector(ras_irq[i], ddr_ras_intr_handle_irq);
		irqc_enable_irq(ras_irq[i]);
	}
}
#endif

/**
 * ddr_ras_init - Initialize DDR RAS
 *
 * This function initializes DDR RAS by registering interrupt handlers
 * and enabling interrupts for all DDR controllers.
 */
void ddr_ras_init(void)
{
	int i;

	memset(&cfg, 0, sizeof(cfg));

	ddr_mask = (~__raw_readl(HW_DDR_MASK) & ~__raw_readl(SW_DDR_MASK));
	con_log(DDR_MODNAME ": DDR mask:0x%08x\n", ddr_mask);

	/* Initialize FUNC interrupt info for all DDR controllers */
	for (i = 0; i < ARRAY_SIZE(func_irq_info); i++) {
		if (!(ddr_mask & (1 << i)))
			continue;
		func_irq_info[i].irq = func_irq[i];
		func_irq_info[i].ctrl_base_addr = ddrc_addr[i].ctrl;
		func_irq_info[i].reg_base_addr = ddrc_addr[i].reg;
		ddr_set(CH0_RAS_INTR_EN | CH1_RAS_INTR_EN | FUNC_INTR_EN,
			func_irq_info[i].reg_base_addr + DDR_INTERRUPT_CTRL_REG);
		ddr_set(0xFFFFFFFF, func_irq_info[i].reg_base_addr + WRB_RDB_PRR_CTRL);
		con_log("DDR_MODNAME: func_irq_info[%d].irq = %d\n", i, func_irq_info[i].irq);
		con_log("DDR_MODNAME: func_irq_info[%d].ctrl_base_addr = 0x%lx\n", i, func_irq_info[i].ctrl_base_addr);
		con_log("DDR_MODNAME: func_irq_info[%d].reg_base_addr = 0x%lx\n", i, func_irq_info[i].reg_base_addr);
		con_log("DDR_MODNAME: func_irq_info[%d].int_ctrl = 0x%x\n", i, __raw_readl(func_irq_info[i].reg_base_addr + DDR_INTERRUPT_CTRL_REG));
		con_log("DDR_MODNAME: func_irq_info[%d].wrb_rdb_ctrl = 0x%x\n", i, __raw_readl(func_irq_info[i].reg_base_addr + WRB_RDB_PRR_CTRL));
	}

	/* Initialize RAS interrupt info for all DDR controllers */
	for (i = 0; i < ARRAY_SIZE(ras_irq_info); i++) {
		if (!(ddr_mask & (1 << (i * 2))))
			continue;
		ras_irq_info[i].irq = ras_irq[i];
		ras_irq_info[i].ctrl_base_addr = ddrc_addr[i >> 1].ctrl;
		ras_irq_info[i].reg_base_addr = ddrc_addr[i >> 1].reg;
	}

	/* Register and enable all RAS interrupts */
	ddr_irq_init();

	/* Initialize address mapping configuration */
#if CONFIG_K1MATRIX_PZ1 || CONFIG_K1MATRIX_ASIC
#else
	cfg.ch_width = 1;
	cfg.rank_width = 4;
	cfg.bg_width = 2;
	cfg.bank_width = 2;
	cfg.row_width = 16;
	cfg.col_width = 10;
#endif
}

/*
 * Inject ECC error (correctable or uncorrectable) to DDR
 * @ddrc_base: DDR controller base address
 * @reg_base: DDR register base address
 * @ch_index: channel index
 * @inject_type: 1 for correctable, 2 for uncorrectable
 * @ecc_mode: ECC mode (SECDED_ECC_MODE or ADVECC_ECC_MODE)
 * @rank, bank, row, col: target address
 */
static void ddr_ecc_inject(caddr_t ddrc_base, caddr_t reg_base,
				uint8_t ch_index, int inject_type, uint32_t ecc_mode,
				uint32_t rank, uint32_t bank, uint32_t bg, uint32_t row, uint32_t col)
{
	uint32_t phys_addr;
	volatile uint32_t *addr;
	uint32_t orig_data;

	/* 1. Disable test_mode, set ECC mode */
	con_log(DDR_MODNAME ": ECCCFG0:0x%08x\n", __raw_readl(ddrc_base + REGB_DDRC_CH_OFFSET(ch_index) + ECCCFG0));
	ddr_write_mask(ecc_mode, ECCCFG0_TEST_MODE | ECCCFG0_ECC_MODE_MASK,
			   ddrc_base + REGB_DDRC_CH_OFFSET(ch_index) + ECCCFG0);
	con_log(DDR_MODNAME ": ECCCFG0:0x%08x\n", __raw_readl(ddrc_base + REGB_DDRC_CH_OFFSET(ch_index) + ECCCFG0));

	/* 2. Enable data_poison_en, set data_poison_bit */
	con_log(DDR_MODNAME ": ECCCFG1:0x%08x\n", __raw_readl(ddrc_base + REGB_DDRC_CH_OFFSET(ch_index) + ECCCFG1));
	ddrc_pre_qdyn_write(ddrc_base);
	ddr_write_mask(BIT(0), ECCCFG1_DATA_POISON_EN,
			   ddrc_base + REGB_DDRC_CH_OFFSET(ch_index) + ECCCFG1);
	con_log(DDR_MODNAME ": ECCCFG1:0x%08x\n", __raw_readl(ddrc_base + REGB_DDRC_CH_OFFSET(ch_index) + ECCCFG1));

	if (inject_type == 1)
		ddr_write_mask(BIT(1), ECCCFG1_DATA_POISON_BIT,
				   ddrc_base + REGB_DDRC_CH_OFFSET(ch_index) + ECCCFG1);
	else
		ddr_write_mask(~BIT(1), ECCCFG1_DATA_POISON_BIT,
				   ddrc_base + REGB_DDRC_CH_OFFSET(ch_index) + ECCCFG1);
	con_log(DDR_MODNAME ": ECCCFG1:0x%08x\n", __raw_readl(ddrc_base + REGB_DDRC_CH_OFFSET(ch_index) + ECCCFG1));

	/* 3. Set target address registers */
	/* ECCPOISONADDR0: [31:24] rank, [11:0] col */
	con_log(DDR_MODNAME ": ECCPOISONADDR0:0x%08x\n", __raw_readl(ddrc_base + REGB_DDRC_CH_OFFSET(ch_index) + ECCPOISONADDR0));
	ddr_write_mask((rank << ECCPOISONADDR0_ECC_POISON_RANK_OFFSET) | (col & ECCPOISONADDR0_ECC_POISON_COL_MASK),
			   ECCPOISONADDR0_ECC_POISON_RANK_MASK | ECCPOISONADDR0_ECC_POISON_COL_MASK,
			   ddrc_base + REGB_DDRC_CH_OFFSET(ch_index) + ECCPOISONADDR0);
	con_log(DDR_MODNAME ": ECCPOISONADDR0:0x%08x\n", __raw_readl(ddrc_base + REGB_DDRC_CH_OFFSET(ch_index) + ECCPOISONADDR0));
	/* ECCPOISONADDR1: [31:28] bg, [27:24] bank, [23:0] row */
	con_log(DDR_MODNAME ": ECCPOISONADDR1:0x%08x\n", __raw_readl(ddrc_base + REGB_DDRC_CH_OFFSET(ch_index) + ECCPOISONADDR1));
	ddr_write_mask((bg << ECCPOISONADDR1_ECC_POISON_BG_OFFSET) | (bank << ECCPOISONADDR1_ECC_POISON_BANK_OFFSET) | (row & ECCPOISONADDR1_ECC_POISON_ROW_MASK),
			   ECCPOISONADDR1_ECC_POISON_BG_MASK | ECCPOISONADDR1_ECC_POISON_BANK_MASK | ECCPOISONADDR1_ECC_POISON_ROW_MASK,
			   ddrc_base + REGB_DDRC_CH_OFFSET(ch_index) + ECCPOISONADDR1);
	con_log(DDR_MODNAME ": ECCPOISONADDR1:0x%08x\n", __raw_readl(ddrc_base + REGB_DDRC_CH_OFFSET(ch_index) + ECCPOISONADDR1));
	ddrc_post_qdyn_write(ddrc_base);

	con_log(DDR_MODNAME ": %s ECC error injection: %s, ch=%u, rank=0x%x, bankgroup=0x%x, bank=0x%x, row=0x%x, col=0x%x\n",
		ecc_mode == SECDED_ECC_MODE ? "SECDED" : "Advanced",
		inject_type == 1 ? "correctable" : "uncorrectable",
		ch_index, rank, bg, bank, row, col);

	/* */
	phys_addr = ddr_components_to_physaddr();

	con_log(DDR_MODNAME ": phys_addr:0x%08x\n", phys_addr);
	addr = (volatile uint32_t*)(uintptr_t)phys_addr;

	orig_data = *addr;
	con_log(DDR_MODNAME ": Original data at 0x%08x: 0x%08x\n", phys_addr, orig_data);

	if (inject_type == 1) {
		/* correctable error */
		*addr ^= 0x00000001;
		con_log(DDR_MODNAME ": Injected correctable error at 0x%08x: 0x%08x\n",
			   phys_addr, *addr);
	} else if (inject_type == 2) {
		/* uncorrectable error */
		*addr ^= 0x00000003;
		con_log(DDR_MODNAME ": Injected uncorrectable error at 0x%08x: 0x%08x\n",
			   phys_addr, *addr);
	}

	(void)*addr;
	con_log(DDR_MODNAME ": Triggered ECC error check\n");

	/* restore original data */
	*addr = orig_data;
	con_log(DDR_MODNAME ": Restored original data at 0x%08x: 0x%08x\n", phys_addr, orig_data);

}

static int do_ddr_intr_test(int argc, char **argv)
{
	uint8_t sub_index = 0, ch_index = 0, intr_index = 0;
	caddr_t ddrc_base, reg_base;
	int mode = 0; // 0: FUNC, 1: RAS
	int i;

	if (argc < 4) {
		con_err(DDR_MODNAME ": Usage: ddr_intr -m <func|ras> -s <sub_idx> -c <ch> -i <intr_idx>\n");
		return -EINVAL;
	}

	for (i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-m") == 0) {
			if (i + 1 >= argc)
				return -EINVAL;
			if (strcmp(argv[i + 1], "ras") == 0)
				mode = 1;
			else
				mode = 0;
			i++;
		} else if (strcmp(argv[i], "-s") == 0) {
			if (i + 1 >= argc)
				return -EINVAL;
			sub_index = (uint8_t)strtoull(argv[i + 1], NULL, 10);
			i++;
		} else if (strcmp(argv[i], "-c") == 0) {
			if (i + 1 >= argc)
				return -EINVAL;
			ch_index = (uint8_t)strtoull(argv[i + 1], NULL, 10);
			i++;
		} else if (strcmp(argv[i], "-i") == 0) {
			if (i + 1 >= argc)
				return -EINVAL;
			intr_index = (uint8_t)strtoull(argv[i + 1], NULL, 10);
			i++;
		} else {
			con_err(DDR_MODNAME ": Invalid option: %s\n", argv[i]);
			return -EINVAL;
		}
	}

	if (mode == 0) {
#if defined(CONFIG_K1MATRIX_PZ1) || defined(CONFIG_K1MATRIX_ASIC)
		/* FUNC interrupt test */
		ddrc_base = ddrc_addr[sub_index].ctrl;
		reg_base = ddrc_addr[sub_index].reg;
		con_dbg(DDR_MODNAME ": [default]FUNC INT STATUS:0x%08x\n",
			   __raw_readl(reg_base + FUNC_INTR_STAT));
		switch (intr_index) {
		case 0: /* DFI alert error interrupt */
			break;
		case 1: /* SWCMD error interrupt */
			con_log(DDR_MODNAME ": SWCMD error interrupt\n");
			con_log(DDR_MODNAME ": PASINTCTL:0x%08x\n", __raw_readl(ddrc_base + REGB_DDRC_CH_OFFSET(ch_index) + PASINTCTL));
			ddr_set(PASINTCTL_SWCMD_ERR_INTR_FORCE, ddrc_base + REGB_DDRC_CH_OFFSET(ch_index) + PASINTCTL);
			con_log(DDR_MODNAME ": PASINTCTL:0x%08x\n", __raw_readl(ddrc_base + REGB_DDRC_CH_OFFSET(ch_index) + PASINTCTL));
			break;
		case 2: /* DUCMD error interrupt */
			con_log(DDR_MODNAME ": DUCMD error interrupt\n");
			con_log(DDR_MODNAME ": PASINTCTL:0x%08x\n", __raw_readl(ddrc_base + REGB_DDRC_CH_OFFSET(ch_index) + PASINTCTL));
			ddr_set(PASINTCTL_DUCMD_ERR_INTR_FORCE, ddrc_base + REGB_DDRC_CH_OFFSET(ch_index) + PASINTCTL);
			con_log(DDR_MODNAME ": PASINTCTL:0x%08x\n", __raw_readl(ddrc_base + REGB_DDRC_CH_OFFSET(ch_index) + PASINTCTL));
			break;
		case 3: /* LCCMD error interrupt */
			con_log(DDR_MODNAME ": LCCMD error interrupt\n");
			con_log(DDR_MODNAME ": PASINTCTL:0x%08x\n", __raw_readl(ddrc_base + REGB_DDRC_CH_OFFSET(ch_index) + PASINTCTL));
			ddr_set(PASINTCTL_LCCMD_ERR_INTR_FORCE, ddrc_base + REGB_DDRC_CH_OFFSET(ch_index) + PASINTCTL);
			con_log(DDR_MODNAME ": PASINTCTL:0x%08x\n", __raw_readl(ddrc_base + REGB_DDRC_CH_OFFSET(ch_index) + PASINTCTL));
			break;
		case 4: /* CTRLUPD error interrupt */
			con_log(DDR_MODNAME ": CTRLUPD error interrupt\n");
			con_log(DDR_MODNAME ": PASINTCTL:0x%08x\n", __raw_readl(ddrc_base + REGB_DDRC_CH_OFFSET(ch_index) + PASINTCTL));
			ddr_set(PASINTCTL_CTRLUPD_ERR_INTR_FORCE, ddrc_base + REGB_DDRC_CH_OFFSET(ch_index) + PASINTCTL);
			con_log(DDR_MODNAME ": PASINTCTL:0x%08x\n", __raw_readl(ddrc_base + REGB_DDRC_CH_OFFSET(ch_index) + PASINTCTL));
			break;
		case 5: /* RFM alert interrupt */
			con_log(DDR_MODNAME ": RFM alert interrupt\n");
			con_log(DDR_MODNAME ": PASINTCTL:0x%08x\n", __raw_readl(ddrc_base + REGB_DDRC_CH_OFFSET(ch_index) + PASINTCTL));
			ddr_set(PASINTCTL_RFM_ALERT_INTR_FORCE, ddrc_base + REGB_DDRC_CH_OFFSET(ch_index) + PASINTCTL);
			con_log(DDR_MODNAME ": PASINTCTL:0x%08x\n", __raw_readl(ddrc_base + REGB_DDRC_CH_OFFSET(ch_index) + PASINTCTL));
			break;
		case 6: /* Derate temp limit interrupt */
			con_log(DDR_MODNAME ": Derate temp limit interrupt\n");
			con_log(DDR_MODNAME ": DERATECTL5:0x%08x\n", __raw_readl(ddrc_base + REGB_DDRC_CH_OFFSET(ch_index) + DERATECTL5));
			ddr_set(DERATECTL5_DERATE_TEMP_LIMIT_INTR_FORCE, ddrc_base + REGB_DDRC_CH_OFFSET(ch_index) + DERATECTL5);
			con_log(DDR_MODNAME ": DERATECTL5:0x%08x\n", __raw_readl(ddrc_base + REGB_DDRC_CH_OFFSET(ch_index) + DERATECTL5));
			break;
		case 7: /* SBR done interrupt */
			break;
		case 8: /* DPM interrupt */
			break;
		case 9: /* DDRPHY interrupt */
			break;
		default:
			break;
		}
#else
		con_err(DDR_MODNAME ": ProFPGA does not support func interrupt test, because derate temp limit interrupt is always be triggered\n");
		return -EINVAL;
#endif
	} else {
		/* RAS interrupt test */
		ddrc_base = ddrc_addr[sub_index].ctrl;
		reg_base = ddrc_addr[sub_index].reg;
		con_dbg(DDR_MODNAME ": [default]RAS INT STATUS:0x%08x\n",
			__raw_readl(reg_base + RAS_INTR_STAT));
		con_dbg(DDR_MODNAME ": [default]PAR INT STATUS:0x%08x\n",
			__raw_readl(reg_base + WRB_RDB_PAR_INT_STAT));
		switch (intr_index) {
		case 0: /* ECC corrected error interrupt */
			con_log(DDR_MODNAME ": set ECC corrected error interrupt\n");
			con_log(DDR_MODNAME ": ECCCTL:0x%08x\n", __raw_readl(ddrc_base + REGB_DDRC_CH_OFFSET(ch_index) + ECCCTL));
			ddr_set(ECCCTL_ECC_CORRECTED_ERR_INTR_FORCE, ddrc_base + REGB_DDRC_CH_OFFSET(ch_index) + ECCCTL);
			con_log(DDR_MODNAME ": ECCCTL:0x%08x\n", __raw_readl(ddrc_base + REGB_DDRC_CH_OFFSET(ch_index) + ECCCTL));
			break;
		case 1: /* ECC uncorrected error interrupt */
			con_log(DDR_MODNAME ": ECC uncorrected error interrupt\n");
			con_log(DDR_MODNAME ": ECCCTL:0x%08x\n", __raw_readl(ddrc_base + REGB_DDRC_CH_OFFSET(ch_index) + ECCCTL));
			ddr_set(ECCCTL_ECC_UNCORRECTED_ERR_INTR_FORCE, ddrc_base + REGB_DDRC_CH_OFFSET(ch_index) + ECCCTL);
			con_log(DDR_MODNAME ": ECCCTL:0x%08x\n", __raw_readl(ddrc_base + REGB_DDRC_CH_OFFSET(ch_index) + ECCCTL));
			break;
		case 2: /* Write CRC error interrupt */
			con_log(DDR_MODNAME ": Write CRC error interrupt\n");
			con_log(DDR_MODNAME ": CRCPARCTL0:0x%08x\n", __raw_readl(ddrc_base + REGB_DDRC_CH_OFFSET(ch_index) + CRCPARCTL0));
			ddr_set(CRCPARCTL0_WR_CRC_ERR_INTR_FORCE, ddrc_base + REGB_DDRC_CH_OFFSET(ch_index) + CRCPARCTL0);
			con_log(DDR_MODNAME ": CRCPARCTL0:0x%08x\n", __raw_readl(ddrc_base + REGB_DDRC_CH_OFFSET(ch_index) + CRCPARCTL0));
			break;
		case 3: /* Write CRC error max reached interrupt */
			con_log(DDR_MODNAME ": Write CRC error max reached interrupt\n");
			con_log(DDR_MODNAME ": CRCPARCTL0:0x%08x\n", __raw_readl(ddrc_base + REGB_DDRC_CH_OFFSET(ch_index) + CRCPARCTL0));
			ddr_set(CRCPARCTL0_WR_CRC_ERR_MAX_REACHED_INTR_FORCE, ddrc_base + REGB_DDRC_CH_OFFSET(ch_index) + CRCPARCTL0);
			con_log(DDR_MODNAME ": CRCPARCTL0:0x%08x\n", __raw_readl(ddrc_base + REGB_DDRC_CH_OFFSET(ch_index) + CRCPARCTL0));
			break;
		case 4: /* CAPAR error interrupt */
			con_log(DDR_MODNAME ": CAPAR error interrupt\n");
			con_log(DDR_MODNAME ": CRCPARCTL0:0x%08x\n", __raw_readl(ddrc_base + REGB_DDRC_CH_OFFSET(ch_index) + CRCPARCTL0));
			ddr_set(CRCPARCTL0_CAPAR_ERR_INTR_FORCE, ddrc_base + REGB_DDRC_CH_OFFSET(ch_index) + CRCPARCTL0);
			con_log(DDR_MODNAME ": CRCPARCTL0:0x%08x\n", __raw_readl(ddrc_base + REGB_DDRC_CH_OFFSET(ch_index) + CRCPARCTL0));
			break;
		case 5: /* CAPAR error max reached interrupt */
			con_log(DDR_MODNAME ": CAPAR error max reached interrupt\n");
			con_log(DDR_MODNAME ": CRCPARCTL0:0x%08x\n", __raw_readl(ddrc_base + REGB_DDRC_CH_OFFSET(ch_index) + CRCPARCTL0));
			ddr_set(CRCPARCTL0_CAPAR_ERR_MAX_REACHED_INTR_FORCE, ddrc_base + REGB_DDRC_CH_OFFSET(ch_index) + CRCPARCTL0);
			con_log(DDR_MODNAME ": CRCPARCTL0:0x%08x\n", __raw_readl(ddrc_base + REGB_DDRC_CH_OFFSET(ch_index) + CRCPARCTL0));
			break;
		case 6: /* CAPAR fatal error interrupt */
			con_log(DDR_MODNAME ": CAPAR fatal error interrupt\n");
			con_log(DDR_MODNAME ": CRCPARCTL0:0x%08x\n", __raw_readl(ddrc_base + REGB_DDRC_CH_OFFSET(ch_index) + CRCPARCTL0));
			ddr_set(CRCPARCTL0_CAPAR_FATL_ERR_INTR_FORCE, ddrc_base + REGB_DDRC_CH_OFFSET(ch_index) + CRCPARCTL0);
			con_log(DDR_MODNAME ": CRCPARCTL0:0x%08x\n", __raw_readl(ddrc_base + REGB_DDRC_CH_OFFSET(ch_index) + CRCPARCTL0));
			break;
		case 7: /* CAPAR retry limit reached interrupt */
			con_log(DDR_MODNAME ": CAPAR retry limit reached interrupt\n");
			con_log(DDR_MODNAME ": RETRYCTL0:0x%08x\n", __raw_readl(ddrc_base + REGB_DDRC_CH_OFFSET(ch_index) + RETRYCTL0));
			ddr_set(RETRYCTL0_CAPAR_RETRY_LIMIT_INTR_FORCE, ddrc_base + REGB_DDRC_CH_OFFSET(ch_index) + RETRYCTL0);
			con_log(DDR_MODNAME ": RETRYCTL0:0x%08x\n", __raw_readl(ddrc_base + REGB_DDRC_CH_OFFSET(ch_index) + RETRYCTL0));
			break;
		case 8: /* CAPAR command error interrupt */
			con_log(DDR_MODNAME ": CAPAR command error interrupt\n");
			con_log(DDR_MODNAME ": PASINTCTL:0x%08x\n", __raw_readl(ddrc_base + REGB_DDRC_CH_OFFSET(ch_index) + PASINTCTL));
			ddr_set(PASINTCTL_CAPARCMD_ERR_INTR_FORCE, ddrc_base + REGB_DDRC_CH_OFFSET(ch_index) + PASINTCTL);
			con_log(DDR_MODNAME ": PASINTCTL:0x%08x\n", __raw_readl(ddrc_base + REGB_DDRC_CH_OFFSET(ch_index) + PASINTCTL));
			break;
		case 9: /* Write CRC retry limit interrupt */
			con_log(DDR_MODNAME ": Write CRC retry limit interrupt\n");
			con_log(DDR_MODNAME ": RETRYCTL0:0x%08x\n", __raw_readl(ddrc_base + REGB_DDRC_CH_OFFSET(ch_index) + RETRYCTL0));
			ddr_set(RETRYCTL0_WR_CRC_RETRY_LIMIT_INTR_FORCE, ddrc_base + REGB_DDRC_CH_OFFSET(ch_index) + RETRYCTL0);
			con_log(DDR_MODNAME ": RETRYCTL0:0x%08x\n", __raw_readl(ddrc_base + REGB_DDRC_CH_OFFSET(ch_index) + RETRYCTL0));
			break;
		case 10: /* Read CRC error max reached interrupt */
			con_log(DDR_MODNAME ": Read CRC error max reached interrupt\n");
			con_log(DDR_MODNAME ": CRCPARCTL0:0x%08x\n", __raw_readl(ddrc_base + REGB_DDRC_CH_OFFSET(ch_index) + CRCPARCTL0));
			ddr_set(CRCPARCTL0_RD_CRC_ERR_MAX_REACHED_INTR_FORCE, ddrc_base + REGB_DDRC_CH_OFFSET(ch_index) + CRCPARCTL0);
			con_log(DDR_MODNAME ": CRCPARCTL0:0x%08x\n", __raw_readl(ddrc_base + REGB_DDRC_CH_OFFSET(ch_index) + CRCPARCTL0));
			break;
		case 11: /* Read retry limit interrupt */
			con_log(DDR_MODNAME ": Read retry limit interrupt\n");
			con_log(DDR_MODNAME ": RETRYCTL0:0x%08x\n", __raw_readl(ddrc_base + REGB_DDRC_CH_OFFSET(ch_index) + RETRYCTL0));
			ddr_set(RETRYCTL0_RD_RETRY_LIMIT_INTR_FORCE, ddrc_base + REGB_DDRC_CH_OFFSET(ch_index) + RETRYCTL0);
			con_log(DDR_MODNAME ": RETRYCTL0:0x%08x\n", __raw_readl(ddrc_base + REGB_DDRC_CH_OFFSET(ch_index) + RETRYCTL0));
			break;
		case 12: /* Write buffer P0R0 parity poison enable */
			con_log(DDR_MODNAME ": Write buffer P0R0 parity poison enable\n");
			con_log(DDR_MODNAME ": PAR_POISON_EN:0x%08x\n", __raw_readl(reg_base + PAR_POISON_EN));
			ddr_set(WRB_P0R0_PAR_POIS_EN, reg_base + PAR_POISON_EN);
			con_log(DDR_MODNAME ": PAR_POISON_EN:0x%08x\n", __raw_readl(reg_base + PAR_POISON_EN));
			break;
		case 13: /* Write buffer P0R1 parity poison enable */
			con_log(DDR_MODNAME ": Write buffer P0R1 parity poison enable\n");
			con_log(DDR_MODNAME ": PAR_POISON_EN:0x%08x\n", __raw_readl(reg_base + PAR_POISON_EN));
			ddr_set(WRB_P0R1_PAR_POIS_EN, reg_base + PAR_POISON_EN);
			con_log(DDR_MODNAME ": PAR_POISON_EN:0x%08x\n", __raw_readl(reg_base + PAR_POISON_EN));
			break;
		case 14: /* Write buffer P1R0 parity poison enable */
			con_log(DDR_MODNAME ": Write buffer P1R0 parity poison enable\n");
			con_log(DDR_MODNAME ": PAR_POISON_EN:0x%08x\n", __raw_readl(reg_base + PAR_POISON_EN));
			ddr_set(WRB_P1R0_PAR_POIS_EN, reg_base + PAR_POISON_EN);
			con_log(DDR_MODNAME ": PAR_POISON_EN:0x%08x\n", __raw_readl(reg_base + PAR_POISON_EN));
			break;
		case 15: /* Write buffer P1R1 parity poison enable */
			con_log(DDR_MODNAME ": Write buffer P1R1 parity poison enable\n");
			con_log(DDR_MODNAME ": PAR_POISON_EN:0x%08x\n", __raw_readl(reg_base + PAR_POISON_EN));
			ddr_set(WRB_P1R1_PAR_POIS_EN, reg_base + PAR_POISON_EN);
			con_log(DDR_MODNAME ": PAR_POISON_EN:0x%08x\n", __raw_readl(reg_base + PAR_POISON_EN));
			break;
		case 16: /* Read buffer P0R0 parity poison enable */
			con_log(DDR_MODNAME ": Read buffer P0R0 parity poison enable\n");
			con_log(DDR_MODNAME ": PAR_POISON_EN:0x%08x\n", __raw_readl(reg_base + PAR_POISON_EN));
			ddr_set(RDB_P0R0_PAR_POIS_EN, reg_base + PAR_POISON_EN);
			con_log(DDR_MODNAME ": PAR_POISON_EN:0x%08x\n", __raw_readl(reg_base + PAR_POISON_EN));
			break;
		case 17: /* Read buffer P0R1 parity poison enable */
			con_log(DDR_MODNAME ": Read buffer P0R1 parity poison enable\n");
			con_log(DDR_MODNAME ": PAR_POISON_EN:0x%08x\n", __raw_readl(reg_base + PAR_POISON_EN));
			ddr_set(RDB_P0R1_PAR_POIS_EN, reg_base + PAR_POISON_EN);
			con_log(DDR_MODNAME ": PAR_POISON_EN:0x%08x\n", __raw_readl(reg_base + PAR_POISON_EN));
			break;
		case 18: /* Read buffer P1R0 parity poison enable */
			con_log(DDR_MODNAME ": Read buffer P1R0 parity poison enable\n");
			con_log(DDR_MODNAME ": PAR_POISON_EN:0x%08x\n", __raw_readl(reg_base + PAR_POISON_EN));
			ddr_set(RDB_P1R0_PAR_POIS_EN, reg_base + PAR_POISON_EN);
			con_log(DDR_MODNAME ": PAR_POISON_EN:0x%08x\n", __raw_readl(reg_base + PAR_POISON_EN));
			break;
		case 19: /* Read buffer P1R1 parity poison enable */
			con_log(DDR_MODNAME ": Read buffer P1R1 parity poison enable\n");
			con_log(DDR_MODNAME ": PAR_POISON_EN:0x%08x\n", __raw_readl(reg_base + PAR_POISON_EN));
			ddr_set(RDB_P1R1_PAR_POIS_EN, reg_base + PAR_POISON_EN);
			con_log(DDR_MODNAME ": PAR_POISON_EN:0x%08x\n", __raw_readl(reg_base + PAR_POISON_EN));
			break;
		default:
			break;
		}

		if (intr_index >= 12 && intr_index <= 15)
			ddr_rw_test(0x1000000, 0x55, 20, true);
		else if (intr_index >= 16 && intr_index <= 19)
			ddr_rw_test(0x1000000, 0x55, 20, false);
	}
	return 0;
}

static int do_ddr_ecc_inject_test(int argc, char **argv)
{
	uint8_t sub_index = 0;
	int ecc_type = 0; // 0: none, 1: correctable, 2: uncorrectable
	uint32_t ecc_mode = SECDED_ECC_MODE;
	caddr_t ddrc_base, reg_base;

	if (argc < 4) {
		con_err(DDR_MODNAME ": Usage: ddr_ecc -t <corr|uncorr> -s <sub_idx> -c <ch> [options]\n");
		return -EINVAL;
	}

	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-t") == 0) {
			if (++i >= argc)
				return -EINVAL;
			if (strcmp(argv[i], "corr") == 0)
				ecc_type = 1;
			else if (strcmp(argv[i], "uncorr") == 0)
				ecc_type = 2;
			else
				return -EINVAL;
		} else if (strcmp(argv[i], "-s") == 0) {
			if (++i >= argc)
				return -EINVAL;
			sub_index = (uint8_t)strtoull(argv[i], NULL, 10);
		} else if (strcmp(argv[i], "-c") == 0) {
			if (++i >= argc)
				return -EINVAL;
			comp.channel = (uint8_t)strtoull(argv[i], NULL, 10);
		} else if (strcmp(argv[i], "-ecc") == 0) {
			if (++i >= argc)
				return -EINVAL;
			ecc_mode = (strcmp(argv[i], "adv") == 0) ? ADVECC_ECC_MODE : SECDED_ECC_MODE;
		} else if (strcmp(argv[i], "-rank") == 0) {
			if (++i >= argc)
				return -EINVAL;
			comp.rank = (uint32_t)strtoull(argv[i], NULL, 10);
		} else if (strcmp(argv[i], "-bank") == 0) {
			if (++i >= argc)
				return -EINVAL;
			comp.bank = (uint32_t)strtoull(argv[i], NULL, 10);
		} else if (strcmp(argv[i], "-bg") == 0) {
			if (++i >= argc)
				return -EINVAL;
			comp.bg = (uint32_t)strtoull(argv[i], NULL, 10);
		} else if (strcmp(argv[i], "-row") == 0) {
			if (++i >= argc)
				return -EINVAL;
			comp.row = (uint32_t)strtoull(argv[i], NULL, 10);
		} else if (strcmp(argv[i], "-col") == 0) {
			if (++i >= argc)
				return -EINVAL;
			comp.col = (uint32_t)strtoull(argv[i], NULL, 10);
		}
	}

	if (ecc_type == 0) {
		con_err(DDR_MODNAME ": Must specify error type (-t corr/uncorr)\n");
		return -EINVAL;
	}

	ddrc_base = ddrc_addr[sub_index].ctrl;
	reg_base = ddrc_addr[sub_index].reg;

	con_log(DDR_MODNAME ": Injecting %s ECC error at rank%d/bank%d/bg%d/row%d/col%d\n",
		   ecc_type == 1 ? "correctable" : "uncorrectable", comp.rank, comp.bank, comp.bg, comp.row, comp.col);

	ddr_ecc_inject(ddrc_base, reg_base, comp.channel, ecc_type, ecc_mode,
				  comp.rank, comp.bank, comp.bg, comp.row, comp.col);

	ddr_report_error(ddrc_base, comp.channel, ecc_mode);

	return 0;
}
#if 1
DEFINE_COMMAND(ddr_intr, do_ddr_intr_test, "DDR interrupt test commands",
	"ddr_intr -m <func|ras> -s <sub_idx> -c <ch> -i <intr_idx>\n"
	"  -m: Test mode (func/ras)\n"
	"  -s: Controller index (0-7)\n"
	"  -c: Channel index (0-1)\n"
	"  -i: Interrupt type index\n"
	"  FUNC supported types:\n"
	"	 0: DFI alert error interrupt\n"
	"	 1: SWCMD error interrupt\n"
	"	 2: DUCMD error interrupt\n"
	"	 3: LCCMD error interrupt\n"
	"	 4: CTRLUPD error interrupt\n"
	"	 5: RFM alert interrupt\n"
	"	 6: Derate temp limit interrupt\n"
	"	 7: SBR done interrupt\n"
	"	 8: DPM interrupt\n"
	"	 9: DDRPHY interrupt\n"
	"  RAS supported types:\n"
	"	 0: ECC corrected error interrupt\n"
	"	 1: ECC uncorrected error interrupt\n"
	"	 2: Write CRC error interrupt\n"
	"	 3: Write CRC error max reached interrupt\n"
	"	 4: CAPAR error interrupt\n"
	"	 5: CAPAR error max reached interrupt\n"
	"	 6: CAPAR fatal error interrupt\n"
	"	 7: CAPAR retry limit reached interrupt\n"
	"	 8: CAPAR command error interrupt\n"
	"	 9: Write CRC retry limit interrupt\n"
	"	10: Read CRC error max reached interrupt\n"
	"	11: Read retry limit interrupt\n"
	"	12: Write buffer P0R0 parity poison enable\n"
	"	13: Write buffer P0R1 parity poison enable\n"
	"	14: Write buffer P1R0 parity poison enable\n"
	"	15: Write buffer P1R1 parity poison enable\n"
	"	16: Read buffer P0R0 parity poison enable\n"
	"	17: Read buffer P0R1 parity poison enable\n"
	"	18: Read buffer P1R0 parity poison enable\n"
	"	19: Read buffer P1R1 parity poison enable\n"
);
#endif
DEFINE_COMMAND(ddr_einj, do_ddr_ecc_inject_test, "DDR ECC error injection test commands",
	"ddr_einj -t <corr|uncorr> -s <sub_idx> -c <ch> [options]\n"
	"  -t: Error type (corr/uncorr)\n"
	"  -s: Controller index (0-7)\n"
	"  -c: Channel index (0-1)\n"
	"  -ecc: ECC mode (secded/adv)\n"
	"  -rank: Target rank (0-7)\n"
	"  -bank: Target bank (0-15)\n"
	"  -bg: Bank group (0-3)\n"
	"  -row: Row\n"
	"  -col: Column\n"
);
