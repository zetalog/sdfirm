#include <target/irq.h>
#include <target/cmdline.h>
#include <target/console.h>
#include <target/arch.h>
#include <target/ras.h>
#include <target/ghes.h>
#include <target/cper.h>
#include <target/acpi.h>

#define DDR_MODNAME	"ddr_ras"

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

struct ddrsub_baseaddr ddrsub_addr[] = {
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
#define FUNC_INTR_EN			_BV(0)
#define CH0_RAS_INTR_EN			_BV(1)
#define CH1_RAS_INTR_EN			_BV(2)

#define PAR_POISON_EN			0x28
#define WRB_P0R0_PAR_POIS_EN		_BV(0)
#define WRB_P0R1_PAR_POIS_EN		_BV(1)
#define WRB_P1R0_PAR_POIS_EN		_BV(2)
#define WRB_P1R1_PAR_POIS_EN		_BV(3)
#define RDB_P0R0_PAR_POIS_EN		_BV(4)
#define RDB_P0R1_PAR_POIS_EN		_BV(5)
#define RDB_P1R0_PAR_POIS_EN		_BV(6)
#define RDB_P1R1_PAR_POIS_EN		_BV(7)

#define FUNC_INTR_STAT_OFFSET		0x30
#define DFI_ALERT_ERR_INTR		_BV(0)
#define SWCMD_ERR_INTR			_BV(1)
#define DUCMD_ERR_INTR			_BV(2)
#define LCCMD_ERR_INTR			_BV(3)
#define CTRLUPD_ERR_INTR		_BV(4)
#define RFM_ALERT_INTR			_BV(5)
#define DERATE_TEMP_LIMIT_INTR		_BV(6)
#define SBR_DONE_INTR			_BV(7)
#define DFI_ALERT_ERR_INTR_DCH1		_BV(8)
#define SWCMD_ERR_INTR_DCH1		_BV(9)
#define DUCMD_ERR_INTR_DCH1		_BV(10)
#define LCCMD_ERR_INTR_DCH1		_BV(11)
#define CTRLUPD_ERR_INTR_DCH1		_BV(12)
#define RFM_ALERT_INTR_DCH1		_BV(13)
#define DERATE_TEMP_LIMIT_INTR_DCH1	_BV(14)
#define SBR_DONE_INTR_DCH1		_BV(15)
#define DPM_INTR			_BV(16)
#define DDRPHY_INTR			_BV(17)

#define RAS_INTR_STAT_OFFSET		0x34
#define ECC_CORRECTED_ERR_INTR		_BV(0)
#define ECC_UNCORRECTED_ERR_INTR	_BV(1)
#define WR_CRC_ERR_INTR			_BV(2)
#define WR_CRC_ERR_MAX_REACHED_INTR	_BV(3)
#define CAPAR_ERR_INTR			_BV(4)
#define CAPAR_ERR_MAX_REACHED_INTR	_BV(5)
#define CAPAR_FATL_ERR_INTR		_BV(6)
#define CAPAR_RETRY_LIMIT_REACHED_INTR	_BV(7)
#define CAPARCMD_ERR_INTR		_BV(8)
#define WR_CRC_RETRY_LIMIT_INTR		_BV(9)
#define RD_CRC_ERR_MAX_REACHED_INTR	_BV(10)
#define RD_RETRY_LIMIT_INTR		_BV(11)
#define ECC_CORRECTED_ERR_INTR_DCH1	_BV(12)
#define ECC_UNCORRECTED_ERR_INTR_DCH1	_BV(13)
#define WR_CRC_ERR_INTR_DCH1		_BV(14)
#define WR_CRC_ERR_MAX_REACHED_INTR_DCH1	_BV(15)
#define CAPAR_ERR_INTR_DCH1		_BV(16)
#define CAPAR_ERR_MAX_REACHED_INTR_DCH1	_BV(17)
#define CAPAR_FATL_ERR_INTR_DCH1		_BV(18)
#define CAPAR_RETRY_LIMIT_REACHED_INTR_DCH1	_BV(19)
#define CAPARCMD_ERR_INTR_DCH1		_BV(20)
#define WR_CRC_RETRY_LIMIT_INTR_DCH1	_BV(21)
#define RD_CRC_ERR_MAX_REACHED_INTR_DCH1	_BV(22)
#define RD_RETRY_LIMIT_INTR_DCH1		_BV(23)

#define WRB_RDB_PAR_INT_STAT_OFFSET	0xC8
#define WRB_P0R0_PAR_ERR_INTR		_BV(0)
#define WRB_P0R1_PAR_ERR_INTR		_BV(1)
#define WRB_P1R0_PAR_ERR_INTR		_BV(2)
#define WRB_P1R1_PAR_ERR_INTR		_BV(3)
#define RDB_P0R0_PAR_ERR_INTR		_BV(4)
#define RDB_P0R1_PAR_ERR_INTR		_BV(5)
#define RDB_P1R0_PAR_ERR_INTR		_BV(6)
#define RDB_P1R1_PAR_ERR_INTR		_BV(7)

#define WRB_P0R0_PAR_ERR_CNT_OFFSET	0xCC
#define WRB_P0R1_PAR_ERR_CNT_OFFSET	0xD0
#define WRB_P1R0_PAR_ERR_CNT_OFFSET	0xD4
#define WRB_P1R1_PAR_ERR_CNT_OFFSET	0xD8
#define RDB_P0R0_PAR_ERR_CNT_OFFSET	0xDC
#define RDB_P0R1_PAR_ERR_CNT_OFFSET	0xE0
#define RDB_P1R0_PAR_ERR_CNT_OFFSET	0xE4
#define RDB_P1R1_PAR_ERR_CNT_OFFSET	0xE8

/* REGB_DDRC_CH(ch) Registers */
#define REGB_DDRC_CH_OFFSET(ch)	((ch) * 0x10000)

/* ECC Registers */
#define ECCCFG0				0x600
#define ECCCFG0_ECC_MODE_MASK		GENMASK(2, 0)
#define DISABLE_ECC_MODE		0x0
#define SECDED_ECC_MODE		0x4
#define ADVECC_ECC_MODE		0x5

#define ECCCFG0_TEST_MODE		_BV(3)
#define ECCCFG0_ECC_TYPE_MASK		GENMASK(5, 4)
#define ECCCFG0_ECC_REGION_REMAP_EN	_BV(7)
#define ECCCFG0_ECC_REGION_MAP_MASK	GENMASK(14, 8)
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
#define ECCSTAT_ECC_CORRECTED_BIT_NUM_MASK	GENMASK(6, 0)  /* Bit number corrected by single-bit ECC error */
#define ECCSTAT_ECC_CORRECTED_ERR_MASK		GENMASK(15, 8)  /* Single-bit error */
#define ECCSTAT_ECC_UNCORRECTED_ERR_MASK	GENMASK(23, 16) /* Double-bit error */
#define ECCSTAT_SBR_READ_ECC_CE			_BV(24) /* sideband SECDED ECC correctable error by scrubber */
#define ECCSTAT_SBR_READ_ECC_UE			_BV(25) /* sideband SECDED ECC uncorrectable error by scrubber */

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
#define ECCERRCNT_ECC_CORR_ERR_CNT_MASK		GENMASK(15, 0)  /* Number of correctable ECC errors */
#define ECCERRCNT_ECC_UNCORR_ERR_CNT_MASK	GENMASK(31, 16) /* Number of uncorrectable ECC errors */

#define ECCCADDR0			0x614
#define ECCCADDR0_ECC_CORR_ROW_OFFSET	0
#define ECCCADDR0_ECC_CORR_ROW_MASK	GENMASK(23, 0)  /* Row number of corrected ECC error */
#define ECCCADDR0_ECC_CORR_RANK_OFFSET	24
#define ECCCADDR0_ECC_CORR_RANK_MASK	GENMASK(31, 24) /* Rank number of corrected ECC error */

#define ECCCADDR1			0x618
#define ECCCADDR1_ECC_CORR_COL_OFFSET	0
#define ECCCADDR1_ECC_CORR_COL_MASK	GENMASK(10, 0)  /* Block number of corrected ECC error */
#define ECCCADDR1_ECC_CORR_BANK_OFFSET	16
#define ECCCADDR1_ECC_CORR_BANK_MASK	GENMASK(23, 16) /* Bank number of corrected ECC error */
#define ECCCADDR1_ECC_CORR_BG_OFFSET	24
#define ECCCADDR1_ECC_CORR_BG_MASK	GENMASK(27, 24) /* Bank Group number of corrected ECC error */
#define ECCCADDR1_ECC_CORR_CID_OFFSET	28
#define ECCCADDR1_ECC_CORR_CID_MASK	GENMASK(31, 28) /* CID number of corrected ECC error */

#define ECCCSYN0			0x61C
#define ECCCSYN0_ECC_CORR_SYNDROME_31_0_MASK	GENMASK(31, 0)

#define ECCCSYN1			0x620
#define ECCCSYN1_ECC_CORR_SYNDROME_63_32_MASK	GENMASK(31, 0)

#define ECCCSYN2			0x624
#define ECCCSYN2_CB_CORR_SYNDROME_MASK	GENMASK(23, 16) /* Checkbit corrected error syndrome */
#define ECCCSYN2_ECC_CORR_SYNDROME_71_64_MASK	GENMASK(7, 0)

#define ECCBITMASK0			0x628
#define ECCBITMASK0_ECC_CORR_BIT_MASK_31_0	GENMASK(31, 0)

#define ECCBITMASK1			0x62C
#define ECCBITMASK1_ECC_CORR_BIT_MASK_63_32	GENMASK(31, 0)

#define ECCBITMASK2			0x630
#define ECCBITMASK2_ECC_CORR_BIT_MASK_71_64	GENMASK(7, 0)

#define ECCUADDR0			0x634
#define ECCUADDR0_ECC_UNCORR_ROW_MASK	GENMASK(23, 0)
#define ECCUADDR0_ECC_UNCORR_RANK_MASK	GENMASK(31, 24)

#define ECCUADDR1			0x638
#define ECCUADDR1_ECC_UNCORR_COL_MASK	GENMASK(10, 0)
#define ECCUADDR1_ECC_UNCORR_BANK_MASK	GENMASK(23, 16)
#define ECCUADDR1_ECC_UNCORR_BG_MASK	GENMASK(27, 24)
#define ECCUADDR1_ECC_UNCORR_CID_MASK	GENMASK(31, 28)


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
#define ADVECCSTAT_ADVECC_CORRECTED_ERR	0
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
#define ECCPOISONPAT0_ECC_POISON_DATA_31_0_MASK		GENMASK(31, 0)

#define ECCPOISONPAT1			0x65C
#define ECCPOISONPAT1_ECC_POISON_DATA_63_32_MASK	GENMASK(31, 0)

#define ECCPOISONPAT2			0x660
#define ECCPOISONPAT2_ECC_POISON_DATA_71_64_MASK	GENMASK(7, 0)

#define ECCCFG2				0x668
#define ECCCFG2_BYPASS_INTERNAL_ECC	_BV(0)
#define ECCCFG2_KBD_EN			_BV(1)
#define ECCCFG2_DIS_RMW_UE_PROPAGATION	_BV(4)
#define ECCCFG2_EAPAR_EN		_BV(8)
#define ECCCFG2_FLIP_BIT_POS0_MASK	GENMASK(22, 16)
#define ECCCFG2_FLIP_BIT_POS1_MASK	GENMASK(30, 24)

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
#define CRCPARCTL0_CAPAR_ERR_INTR_FORCE			_BV(2)
#define CRCPARCTL0_CAPAR_ERR_MAX_REACHED_INTR_CLR	_BV(5)
#define CRCPARCTL0_CAPAR_ERR_MAX_REACHED_INTR_FORCE	_BV(6)
#define CRCPARCTL0_CAPAR_ERR_INTR_CLR			_BV(7)
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
#define PASINTCTL_SWCMD_ERR_INTR_CLR	_BV(1)
#define PASINTCTL_SWCMD_ERR_INTR_FORCE	_BV(2)
#define PASINTCTL_DUCMD_ERR_INTR_CLR	_BV(5)
#define PASINTCTL_DUCMD_ERR_INTR_FORCE	_BV(6)
#define PASINTCTL_LCCMD_ERR_INTR_CLR	_BV(9)
#define PASINTCTL_LCCMD_ERR_INTR_FORCE	_BV(10)
#define PASINTCTL_CTRLUPD_ERR_INTR_CLR	_BV(13)
#define PASINTCTL_CTRLUPD_ERR_INTR_FORCE	_BV(14)
#define PASINTCTL_RFM_ALERT_INTR_CLR	_BV(17)
#define PASINTCTL_RFM_ALERT_INTR_FORCE	_BV(18)
#define PASINTCTL_CAPARCMD_ERR_INTR_CLR	_BV(21)
#define PASINTCTL_CAPARCMD_ERR_INTR_FORCE	_BV(22)

bh_t ddr_bh;

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

uint32_t ras_irq[RAS_INTR_STAT_OFFSET] = {
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

static void ddr_func_intr_handle_irq(void)
{
	uint32_t status;
	int i;
	caddr_t ctrl_base, reg_base;

	for (i = 0; i < ARRAY_SIZE(ddrsub_addr); i++) {
		ctrl_base = ddrsub_addr[i].ctrl;
		reg_base = ddrsub_addr[i].reg;
		status = __raw_readl(reg_base + FUNC_INTR_STAT_OFFSET);

		if (status != 0) {
			con_dbg(DDR_MODNAME ": FUNC INT STATUS:0x%08x, channel: %d\n", status, i);

			if (status & DFI_ALERT_ERR_INTR)
				/* DDRC doesnot support */;
			if (status & SWCMD_ERR_INTR)
				ddr_set(PASINTCTL_SWCMD_ERR_INTR_CLR, ctrl_base + REGB_DDRC_CH_OFFSET(0) + PASINTCTL);
			if (status & DUCMD_ERR_INTR)
				ddr_set(PASINTCTL_DUCMD_ERR_INTR_CLR, ctrl_base + REGB_DDRC_CH_OFFSET(0) + PASINTCTL);
			if (status & LCCMD_ERR_INTR)
				ddr_set(PASINTCTL_LCCMD_ERR_INTR_CLR, ctrl_base + REGB_DDRC_CH_OFFSET(0) + PASINTCTL);
			if (status & CTRLUPD_ERR_INTR)
				ddr_set(PASINTCTL_CTRLUPD_ERR_INTR_CLR, ctrl_base + REGB_DDRC_CH_OFFSET(0) + PASINTCTL);
			if (status & RFM_ALERT_INTR)
				ddr_set(PASINTCTL_RFM_ALERT_INTR_CLR, ctrl_base + REGB_DDRC_CH_OFFSET(0) + PASINTCTL);
			if (status & DERATE_TEMP_LIMIT_INTR)
				ddr_set(DERATECTL5_DERATE_TEMP_LIMIT_INTR_CLR, ctrl_base + REGB_DDRC_CH_OFFSET(0) + DERATECTL5);
			if (status & SBR_DONE_INTR)
				/* DDRC doesnot support */;
			if (status & DFI_ALERT_ERR_INTR_DCH1)
				/* DDRC doesnot support */;
			if (status & SWCMD_ERR_INTR_DCH1)
				ddr_set(PASINTCTL_SWCMD_ERR_INTR_CLR, ctrl_base + REGB_DDRC_CH_OFFSET(1) + PASINTCTL);
			if (status & DUCMD_ERR_INTR_DCH1)
				ddr_set(PASINTCTL_DUCMD_ERR_INTR_CLR, ctrl_base + REGB_DDRC_CH_OFFSET(1) + PASINTCTL);
			if (status & LCCMD_ERR_INTR_DCH1)
				ddr_set(PASINTCTL_LCCMD_ERR_INTR_CLR, ctrl_base + REGB_DDRC_CH_OFFSET(1) + PASINTCTL);
			if (status & CTRLUPD_ERR_INTR_DCH1)
				ddr_set(PASINTCTL_CTRLUPD_ERR_INTR_CLR, ctrl_base + REGB_DDRC_CH_OFFSET(1) + PASINTCTL);
			if (status & RFM_ALERT_INTR_DCH1)
				ddr_set(PASINTCTL_RFM_ALERT_INTR_CLR, ctrl_base + REGB_DDRC_CH_OFFSET(1) + PASINTCTL);
			if (status & DERATE_TEMP_LIMIT_INTR_DCH1)
				ddr_set(DERATECTL5_DERATE_TEMP_LIMIT_INTR_CLR, ctrl_base + REGB_DDRC_CH_OFFSET(1) + DERATECTL5);
			if (status & SBR_DONE_INTR_DCH1)
				/* DDRC doesnot support */;
			if (status & DPM_INTR)
				/* DDRC doesnot support */;
			if (status & DDRPHY_INTR)
				/* DDRC doesnot support */;
		}
	}
}

static void ddr_ras_intr_handle_irq(void)
{
	uint32_t status, par_status;
	int i;
	caddr_t ctrl_base, reg_base;

	for (i = 0; i < ARRAY_SIZE(ddrsub_addr); i++) {
		ctrl_base = ddrsub_addr[i].ctrl;
		reg_base = ddrsub_addr[i].reg;
		status = __raw_readl(reg_base + RAS_INTR_STAT_OFFSET);
		par_status = __raw_readl(reg_base + WRB_RDB_PAR_INT_STAT_OFFSET);

		if (status != 0 || par_status != 0) {
			con_dbg(DDR_MODNAME ": RAS INT STATUS:0x%08x, PAR INT STATUS:0x%08x, channel: %d\n", status, par_status, i);

			/* process error interrupt */
			if (status & ECC_CORRECTED_ERR_INTR)
				ddr_set(ECCCTL_ECC_CORRECTED_ERR_CLR, ctrl_base + REGB_DDRC_CH_OFFSET(0) + ECCCTL);
			if (status & ECC_UNCORRECTED_ERR_INTR)
				ddr_set(ECCCTL_ECC_UNCORRECTED_ERR_CLR, ctrl_base + REGB_DDRC_CH_OFFSET(0) + ECCCTL);
			if (status & WR_CRC_ERR_INTR)
				ddr_set(CRCPARCTL0_WR_CRC_ERR_INTR_CLR, ctrl_base + REGB_DDRC_CH_OFFSET(0) + CRCPARCTL0);
			if (status & WR_CRC_ERR_MAX_REACHED_INTR)
				ddr_set(CRCPARCTL0_WR_CRC_ERR_MAX_REACHED_INTR_CLR, ctrl_base + REGB_DDRC_CH_OFFSET(0) + CRCPARCTL0);
			if (status & CAPAR_ERR_INTR)
				ddr_set(CRCPARCTL0_CAPAR_ERR_INTR_CLR, ctrl_base + REGB_DDRC_CH_OFFSET(0) + CRCPARCTL0);
			if (status & CAPAR_ERR_MAX_REACHED_INTR)
				ddr_set(CRCPARCTL0_CAPAR_ERR_MAX_REACHED_INTR_CLR, ctrl_base + REGB_DDRC_CH_OFFSET(0) + CRCPARCTL0);
			if (status & CAPAR_FATL_ERR_INTR)
				ddr_set(CRCPARCTL0_CAPAR_FATL_ERR_INTR_CLR, ctrl_base + REGB_DDRC_CH_OFFSET(0) + CRCPARCTL0);
			if (status & CAPAR_RETRY_LIMIT_REACHED_INTR)
				ddr_set(RETRYCTL0_CAPAR_RETRY_LIMIT_INTR_CLR, ctrl_base + REGB_DDRC_CH_OFFSET(0) + RETRYCTL0);
			if (status & CAPARCMD_ERR_INTR)
				ddr_set(PASINTCTL_CAPARCMD_ERR_INTR_CLR, ctrl_base + REGB_DDRC_CH_OFFSET(0) + PASINTCTL);
			if (status & WR_CRC_RETRY_LIMIT_INTR)
				ddr_set(RETRYCTL0_WR_CRC_RETRY_LIMIT_INTR_CLR, ctrl_base + REGB_DDRC_CH_OFFSET(0) + RETRYCTL0);
			if (status & RD_CRC_ERR_MAX_REACHED_INTR)
				ddr_set(CRCPARCTL0_RD_CRC_ERR_MAX_REACHED_INT_CLR, ctrl_base + REGB_DDRC_CH_OFFSET(0) + CRCPARCTL0);
			if (status & RD_RETRY_LIMIT_INTR)
				ddr_set(RETRYCTL0_RD_RETRY_LIMIT_INTR_CLR, ctrl_base + REGB_DDRC_CH_OFFSET(0) + RETRYCTL0);

			/* process parity error */
			if (par_status & WRB_P0R0_PAR_ERR_INTR) {
				con_dbg(DDR_MODNAME ": WRB P0R0 PAR ERR CNT:0x%08x, channel: %d\n", __raw_readl(reg_base + WRB_P0R0_PAR_ERR_CNT_OFFSET), i);
				ddr_clear(WRB_P0R0_PAR_ERR_INTR, reg_base + WRB_RDB_PAR_INT_STAT_OFFSET);
			}
			if (par_status & WRB_P0R1_PAR_ERR_INTR) {
				con_dbg(DDR_MODNAME ": WRB P0R1 PAR ERR CNT:0x%08x, channel: %d\n", __raw_readl(reg_base + WRB_P0R1_PAR_ERR_CNT_OFFSET), i);
				ddr_clear(WRB_P0R1_PAR_ERR_INTR, reg_base + WRB_RDB_PAR_INT_STAT_OFFSET);
			}
			if (par_status & WRB_P1R0_PAR_ERR_INTR) {
				con_dbg(DDR_MODNAME ": WRB P1R0 PAR ERR CNT:0x%08x, channel: %d\n", __raw_readl(reg_base + WRB_P1R0_PAR_ERR_CNT_OFFSET), i);
				ddr_clear(WRB_P1R0_PAR_ERR_INTR, reg_base + WRB_RDB_PAR_INT_STAT_OFFSET);
			}
			if (par_status & WRB_P1R1_PAR_ERR_INTR) {
				con_dbg(DDR_MODNAME ": WRB P1R1 PAR ERR CNT:0x%08x, channel: %d\n", __raw_readl(reg_base + WRB_P1R1_PAR_ERR_CNT_OFFSET), i);
				ddr_clear(WRB_P1R1_PAR_ERR_INTR, reg_base + WRB_RDB_PAR_INT_STAT_OFFSET);
			}
			if (par_status & RDB_P0R0_PAR_ERR_INTR) {
				con_dbg(DDR_MODNAME ": RDB P0R0 PAR ERR CNT:0x%08x, channel: %d\n", __raw_readl(reg_base + RDB_P0R0_PAR_ERR_CNT_OFFSET), i);
				ddr_clear(RDB_P0R0_PAR_ERR_INTR, reg_base + WRB_RDB_PAR_INT_STAT_OFFSET);
			}
			if (par_status & RDB_P0R1_PAR_ERR_INTR) {
				con_dbg(DDR_MODNAME ": RDB P0R1 PAR ERR CNT:0x%08x, channel: %d\n", __raw_readl(reg_base + RDB_P0R1_PAR_ERR_CNT_OFFSET), i);
				ddr_clear(RDB_P0R1_PAR_ERR_INTR, reg_base + WRB_RDB_PAR_INT_STAT_OFFSET);
			}
			if (par_status & RDB_P1R0_PAR_ERR_INTR) {
				con_dbg(DDR_MODNAME ": RDB P1R0 PAR ERR CNT:0x%08x, channel: %d\n", __raw_readl(reg_base + RDB_P1R0_PAR_ERR_CNT_OFFSET), i);
				ddr_clear(RDB_P1R0_PAR_ERR_INTR, reg_base + WRB_RDB_PAR_INT_STAT_OFFSET);
			}
			if (par_status & RDB_P1R1_PAR_ERR_INTR) {
				con_dbg(DDR_MODNAME ": RDB P1R1 PAR ERR CNT:0x%08x, channel: %d\n", __raw_readl(reg_base + RDB_P1R1_PAR_ERR_CNT_OFFSET), i);
				ddr_clear(RDB_P1R1_PAR_ERR_INTR, reg_base + WRB_RDB_PAR_INT_STAT_OFFSET);
			}
		}
	}
}

static void ddr_bh_handler(uint8_t events)
{
	if (events == BH_POLLIRQ) {
		ddr_ras_intr_handle_irq();
		ddr_func_intr_handle_irq();
		return;
	}
}

#ifdef SYS_REALTIME
static void ddr_poll_init(void)
{
	irq_register_poller(ddr_bh);
}
#define ddr_irq_init()	do {} while (0)
#else
static void ddr_irq_init(void)
{
	uint8_t i;

	/* Initialize RAS interrupt info for all DDR controllers */
	for (i = 0; i < ARRAY_SIZE(func_irq); i++) {
		irqc_configure_irq(func_irq[i], 0, IRQ_LEVEL_TRIGGERED);
		irq_register_vector(func_irq[i], ddr_func_intr_handle_irq);
		irqc_enable_irq(func_irq[i]);
	}

	for (i = 0; i < ARRAY_SIZE(ras_irq); i++) {
		irqc_configure_irq(ras_irq[i], 0, IRQ_LEVEL_TRIGGERED);
		irq_register_vector(ras_irq[i], ddr_ras_intr_handle_irq);
		irqc_enable_irq(ras_irq[i]);
	}
}
#define ddr_poll_init()	do {} while (0)
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

	/* Initialize FUNC interrupt info for all DDR controllers */
	for (i = 0; i < ARRAY_SIZE(func_irq_info); i++) {
		func_irq_info[i].irq = func_irq[i];
		func_irq_info[i].ctrl_base_addr = ddrsub_addr[i].ctrl;
		func_irq_info[i].reg_base_addr = ddrsub_addr[i].reg;
		ddr_set(CH0_RAS_INTR_EN | CH1_RAS_INTR_EN | FUNC_INTR_EN,
			func_irq_info[i].reg_base_addr + DDR_INTERRUPT_CTRL_REG);
	}

	/* Initialize RAS interrupt info for all DDR controllers */
	for (i = 0; i < ARRAY_SIZE(ras_irq_info); i++) {
		ras_irq_info[i].irq = ras_irq[i];
		ras_irq_info[i].ctrl_base_addr = ddrsub_addr[i >> 1].ctrl;
		ras_irq_info[i].reg_base_addr = ddrsub_addr[i >> 1].reg;
	}

	/* Register and enable all RAS interrupts */
	ddr_bh = bh_register_handler(ddr_bh_handler);
	ddr_irq_init();
	ddr_poll_init();
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
	/* 1. Disable test_mode, set ECC mode */
	ddr_write_mask(ecc_mode, ECCCFG0_TEST_MODE | ECCCFG0_ECC_MODE_MASK,
		       ddrc_base + REGB_DDRC_CH_OFFSET(ch_index) + ECCCFG0);

	/* 2. Enable data_poison_en, set data_poison_bit */
	ddr_write_mask(BIT(0), ECCCFG1_DATA_POISON_EN,
		       ddrc_base + REGB_DDRC_CH_OFFSET(ch_index) + ECCCFG1);
	if (inject_type == 1)
		ddr_write_mask(BIT(1), ECCCFG1_DATA_POISON_BIT,
			       ddrc_base + REGB_DDRC_CH_OFFSET(ch_index) + ECCCFG1);
	else
		ddr_write_mask(~BIT(1), ECCCFG1_DATA_POISON_BIT,
			       ddrc_base + REGB_DDRC_CH_OFFSET(ch_index) + ECCCFG1);

	/* 3. Set target address registers */
	/* ECCPOISONADDR0: [31:24] rank, [11:0] col */
	ddr_write_mask((rank << ECCPOISONADDR0_ECC_POISON_RANK_OFFSET) | (col & ECCPOISONADDR0_ECC_POISON_COL_MASK),
		       ECCPOISONADDR0_ECC_POISON_RANK_MASK | ECCPOISONADDR0_ECC_POISON_COL_MASK,
		       ddrc_base + REGB_DDRC_CH_OFFSET(ch_index) + ECCPOISONADDR0);
	/* ECCPOISONADDR1: [31:28] bg, [27:24] bank, [23:0] row */
	ddr_write_mask((bg << ECCPOISONADDR1_ECC_POISON_BG_OFFSET) | (bank << ECCPOISONADDR1_ECC_POISON_BANK_OFFSET) | (row & ECCPOISONADDR1_ECC_POISON_ROW_MASK),
		       ECCPOISONADDR1_ECC_POISON_BG_MASK | ECCPOISONADDR1_ECC_POISON_BANK_MASK | ECCPOISONADDR1_ECC_POISON_ROW_MASK,
		       ddrc_base + REGB_DDRC_CH_OFFSET(ch_index) + ECCPOISONADDR1);

	con_log(DDR_MODNAME ": %s ECC error injection: %s, ch=%u, rank=0x%x, bankgroup=0x%x, bank=0x%x, row=0x%x, col=0x%x\n",
		ecc_mode == SECDED_ECC_MODE ? "SECDED" : "Advanced",
		inject_type == 1 ? "correctable" : "uncorrectable",
		ch_index, rank, bg, bank, row, col);
}

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

#define UNKNOWN_ERROR        0x0000
#define SINGLE_BIT_ERROR     0x0002
#define MULTIPLE_BIT_ERROR   0x0003

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
 * ddr_fill_cper_mem - Fill UEFI CPER memory error section
 * @info: Pointer to DDR error info structure
 * @mem: Pointer to CPER memory error section
 *
 * Fill DDR error information into UEFI CPER structure
 */
static void ddr_fill_cper_mem(struct ddr_error_info *info,
			     struct cper_sec_mem_err *mem)
{
	/* Set validation bits */
	mem->validation_bits = CPER_MEM_VALID_PA | CPER_MEM_VALID_ERROR_TYPE
	| CPER_MEM_VALID_BANK | CPER_MEM_VALID_BANK_GROUP | CPER_MEM_VALID_RANK_NUMBER
	| CPER_MEM_VALID_ROW | CPER_MEM_VALID_COLUMN;

	/* Set physical address */
	mem->physical_addr = info->error_address;
	mem->physical_addr_mask = ~0ULL;

	/* Set other fields */
	mem->node = 0;
	mem->card = 0;
	mem->module = 0;
	mem->bank = info->error_bank;
	mem->device = 0;
	mem->row = info->error_row;
	mem->column = info->error_col;
	mem->rank = info->error_rank;
	mem->bit_pos = 0;
	mem->requestor_id = 0;
	mem->responder_id = 0;
	mem->target_id = 0;
	mem->error_type = info->error_type;
	mem->mem_array_handle = 0;
	mem->mem_dev_handle = 0;
	mem->extended = 0;
}
#define MAX_IOMMU_NUM			1
#define MAX_IOMMU_ERROR_BANKS		64 // IOATS + 63 * IOATC
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

	/* 1. Collect error information */
	ddr_collect_error_info(ddrc_base, ch_index, &info, ecc_mode);

	/* 2. Initialize error info structure */
	memset(&einfo, 0, sizeof(einfo));
	einfo.etype = ERROR_TYPE_MEM;

	/* 3. Set error severity */
	if (info.error_type == SINGLE_BIT_ERROR)
		einfo.info.me.err_sev = GHES_SEV_CORRECTED;
	else
		einfo.info.me.err_sev = GHES_SEV_PANIC;

	/* 4. Set validation bits */
	einfo.info.me.validation_bits = GPE_TARGET_ADDR_VALID;

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
	einfo.info.me.chip_id = 0;
	einfo.info.me.err_type = info.error_type;
	einfo.info.me.status = 0;
	einfo.info.me.resvd = 0;
	/* 6. Set source ID (DDR channel) */
	acpi_ghes_source_id = NR_CPUS/4 + MAX_IOMMU_NUM * MAX_IOMMU_ERROR_BANKS + ch_index;

	/* 7. Report error */
	acpi_ghes_record_errors(acpi_ghes_source_id, &einfo);
}

static int do_ddr_intr_test(int argc, char **argv)
{
	uint8_t sub_index = 0, ch_index = 0, intr_index = 0;
	caddr_t ddrc_base, reg_base;
	int mode = 0; // 0: FUNC, 1: RAS
	int i;
	int ecc_inject = 0; // 0: no inject, 1: correctable, 2: uncorrectable
	uint32_t rank = 0, bank = 0, bg = 0, row = 0, col = 0;
	uint32_t ecc_mode = SECDED_ECC_MODE; // Default to SECDED mode

	if (argc < 2)
		return -EINVAL;

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
			sub_index = (uint8_t)strtoull(argv[i + 1], NULL, 16);
			i++;
		} else if (strcmp(argv[i], "-c") == 0) {
			if (i + 1 >= argc)
				return -EINVAL;
			ch_index = (uint8_t)strtoull(argv[i + 1], NULL, 16);
			i++;
		} else if (strcmp(argv[i], "-i") == 0) {
			if (i + 1 >= argc)
				return -EINVAL;
			intr_index = (uint8_t)strtoull(argv[i + 1], NULL, 16);
			i++;
		} else if (strcmp(argv[i], "-inject") == 0) {
			if (i + 1 >= argc)
				return -EINVAL;
			if (strcmp(argv[i + 1], "corr") == 0)
				ecc_inject = 1;
			else if (strcmp(argv[i + 1], "uncorr") == 0)
				ecc_inject = 2;
			i++;
		} else if (strcmp(argv[i], "-ecc") == 0) {
			if (i + 1 >= argc)
				return -EINVAL;
			if (strcmp(argv[i + 1], "secded") == 0)
				ecc_mode = SECDED_ECC_MODE;
			else if (strcmp(argv[i + 1], "adv") == 0)
				ecc_mode = ADVECC_ECC_MODE;
			i++;
		} else if (strcmp(argv[i], "-rank") == 0) {
			if (i + 1 >= argc)
				return -EINVAL;
			rank = (uint32_t)strtoull(argv[i + 1], NULL, 16);
			i++;
		} else if (strcmp(argv[i], "-bank") == 0) {
			if (i + 1 >= argc)
				return -EINVAL;
			bank = (uint32_t)strtoull(argv[i + 1], NULL, 16);
			i++;
		} else if (strcmp(argv[i], "-bg") == 0) {
			if (i + 1 >= argc)
				return -EINVAL;
			bg = (uint32_t)strtoull(argv[i + 1], NULL, 16);
			i++;
		} else if (strcmp(argv[i], "-row") == 0) {
			if (i + 1 >= argc)
				return -EINVAL;
			row = (uint32_t)strtoull(argv[i + 1], NULL, 16);
			i++;
		} else if (strcmp(argv[i], "-col") == 0) {
			if (i + 1 >= argc)
				return -EINVAL;
			col = (uint32_t)strtoull(argv[i + 1], NULL, 16);
			i++;
		} else {
			con_err(DDR_MODNAME ": Invalid option: %s\n", argv[i]);
			return -EINVAL;
		}
	}

	if (ecc_inject) {
		ddrc_base = ddrsub_addr[sub_index].ctrl;
		reg_base = ddrsub_addr[sub_index].reg;
		ddr_ecc_inject(ddrc_base, reg_base, ch_index, ecc_inject, ecc_mode, rank, bank, bg, row, col);
		ddr_report_error(ddrc_base, ch_index, ecc_mode);
		return 0;
	}

	if (mode == 0) {
		/* FUNC interrupt test */
		ddrc_base = ddrsub_addr[sub_index].ctrl;
		reg_base = ddrsub_addr[sub_index].reg;
		con_dbg(DDR_MODNAME ": [default]FUNC INT STATUS:0x%08x\n",
		       __raw_readl(reg_base + FUNC_INTR_STAT_OFFSET));
		switch (intr_index) {
		case 0: /* DFI alert error interrupt */
			break;
		case 1: /* SWCMD error interrupt */
			ddr_set(PASINTCTL_SWCMD_ERR_INTR_FORCE, ddrc_base + REGB_DDRC_CH_OFFSET(ch_index) + PASINTCTL);
			break;
		case 2: /* DUCMD error interrupt */
			ddr_set(PASINTCTL_DUCMD_ERR_INTR_FORCE, ddrc_base + REGB_DDRC_CH_OFFSET(ch_index) + PASINTCTL);
			break;
		case 3: /* LCCMD error interrupt */
			ddr_set(PASINTCTL_LCCMD_ERR_INTR_FORCE, ddrc_base + REGB_DDRC_CH_OFFSET(ch_index) + PASINTCTL);
			break;
		case 4: /* CTRLUPD error interrupt */
			ddr_set(PASINTCTL_CTRLUPD_ERR_INTR_FORCE, ddrc_base + REGB_DDRC_CH_OFFSET(ch_index) + PASINTCTL);
			break;
		case 5: /* RFM alert interrupt */
			ddr_set(PASINTCTL_RFM_ALERT_INTR_FORCE, ddrc_base + REGB_DDRC_CH_OFFSET(ch_index) + PASINTCTL);
			break;
		case 6: /* Derate temp limit interrupt */
			ddr_set(DERATECTL5_DERATE_TEMP_LIMIT_INTR_FORCE, ddrc_base + REGB_DDRC_CH_OFFSET(ch_index) + DERATECTL5);
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
	} else {
		/* RAS interrupt test */
		ddrc_base = ddrsub_addr[sub_index].ctrl;
		reg_base = ddrsub_addr[sub_index].reg;
		con_dbg(DDR_MODNAME ": [default]RAS INT STATUS:0x%08x\n",
		       __raw_readl(reg_base + RAS_INTR_STAT_OFFSET));
		con_dbg(DDR_MODNAME ": [default]PAR INT STATUS:0x%08x\n",
		       __raw_readl(reg_base + WRB_RDB_PAR_INT_STAT_OFFSET));
		switch (intr_index) {
		case 0: /* ECC corrected error interrupt */
			ddr_set(ECCCTL_ECC_CORRECTED_ERR_INTR_FORCE, ddrc_base + REGB_DDRC_CH_OFFSET(ch_index) + ECCCTL);
			ddr_report_error(ddrc_base, ch_index, ecc_mode);
			break;
		case 1: /* ECC uncorrected error interrupt */
			ddr_set(ECCCTL_ECC_UNCORRECTED_ERR_INTR_FORCE, ddrc_base + REGB_DDRC_CH_OFFSET(ch_index) + ECCCTL);
			ddr_report_error(ddrc_base, ch_index, ecc_mode);
			break;
		case 2: /* Write CRC error interrupt */
			ddr_set(CRCPARCTL0_WR_CRC_ERR_INTR_FORCE, ddrc_base + REGB_DDRC_CH_OFFSET(ch_index) + CRCPARCTL0);
			break;
		case 3: /* Write CRC error max reached interrupt */
			ddr_set(CRCPARCTL0_WR_CRC_ERR_MAX_REACHED_INTR_FORCE, ddrc_base + REGB_DDRC_CH_OFFSET(ch_index) + CRCPARCTL0);
			break;
		case 4: /* CAPAR error interrupt */
			ddr_set(CRCPARCTL0_CAPAR_ERR_INTR_FORCE, ddrc_base + REGB_DDRC_CH_OFFSET(ch_index) + CRCPARCTL0);
			break;
		case 5: /* CAPAR error max reached interrupt */
			ddr_set(CRCPARCTL0_CAPAR_ERR_MAX_REACHED_INTR_FORCE, ddrc_base + REGB_DDRC_CH_OFFSET(ch_index) + CRCPARCTL0);
			break;
		case 6: /* CAPAR fatal error interrupt */
			ddr_set(CRCPARCTL0_CAPAR_FATL_ERR_INTR_FORCE, ddrc_base + REGB_DDRC_CH_OFFSET(ch_index) + CRCPARCTL0);
			break;
		case 7: /* CAPAR retry limit reached interrupt */
			ddr_set(RETRYCTL0_CAPAR_RETRY_LIMIT_INTR_FORCE, ddrc_base + REGB_DDRC_CH_OFFSET(ch_index) + RETRYCTL0);
			break;
		case 8: /* CAPAR command error interrupt */
			ddr_set(PASINTCTL_CAPARCMD_ERR_INTR_FORCE, ddrc_base + REGB_DDRC_CH_OFFSET(ch_index) + PASINTCTL);
			break;
		case 9: /* Write CRC retry limit interrupt */
			ddr_set(RETRYCTL0_WR_CRC_RETRY_LIMIT_INTR_FORCE, ddrc_base + REGB_DDRC_CH_OFFSET(ch_index) + RETRYCTL0);
			break;
		case 10: /* Read CRC error max reached interrupt */
			ddr_set(CRCPARCTL0_RD_CRC_ERR_MAX_REACHED_INTR_FORCE, ddrc_base + REGB_DDRC_CH_OFFSET(ch_index) + CRCPARCTL0);
			break;
		case 11: /* Read retry limit interrupt */
			ddr_set(RETRYCTL0_RD_RETRY_LIMIT_INTR_FORCE, ddrc_base + REGB_DDRC_CH_OFFSET(ch_index) + RETRYCTL0);
			break;
		case 12: /* Write buffer P0R0 parity poison enable */
			ddr_set(WRB_P0R0_PAR_POIS_EN, reg_base + PAR_POISON_EN);
			break;
		case 13: /* Write buffer P0R1 parity poison enable */
			ddr_set(WRB_P0R1_PAR_POIS_EN, reg_base + PAR_POISON_EN);
			break;
		case 14: /* Write buffer P1R0 parity poison enable */
			ddr_set(WRB_P1R0_PAR_POIS_EN, reg_base + PAR_POISON_EN);
			break;
		case 15: /* Write buffer P1R1 parity poison enable */
			ddr_set(WRB_P1R1_PAR_POIS_EN, reg_base + PAR_POISON_EN);
			break;
		case 16: /* Read buffer P0R0 parity poison enable */
			ddr_set(RDB_P0R0_PAR_POIS_EN, reg_base + PAR_POISON_EN);
			break;
		case 17: /* Read buffer P0R1 parity poison enable */
			ddr_set(RDB_P0R1_PAR_POIS_EN, reg_base + PAR_POISON_EN);
			break;
		case 18: /* Read buffer P1R0 parity poison enable */
			ddr_set(RDB_P1R0_PAR_POIS_EN, reg_base + PAR_POISON_EN);
			break;
		case 19: /* Read buffer P1R1 parity poison enable */
			ddr_set(RDB_P1R1_PAR_POIS_EN, reg_base + PAR_POISON_EN);
			break;
		default:
			break;
		}
	}
	return 0;
}

DEFINE_COMMAND(ddr_intr, do_ddr_intr_test, "DDR interrupt test commands",
	"ddr_intr -m <mode> [-s sub_index] [-c ch_index] [-i intr_index]\n"
	"  -m: mode, 'func' for FUNC interrupts, 'ras' for RAS interrupts\n"
	"  -s: DDR controller index (0-7)\n"
	"  -c: DDR channel index (0-1)\n"
	"  -i: Interrupt type index\n"
	"  -inject <corr/uncorr>: ECC error injection, correctable or uncorrectable\n"
	"  -ecc <secded/adv>: ECC mode, 'secded' for SECDED ECC, 'adv' for Advanced ECC\n"
	"  -rank <val>: ECC error injection rank\n"
	"  -bank <val>: ECC error injection bank\n"
	"  -bg <val>: ECC error injection bankgroup\n"
	"  -row <val>: ECC error injection row\n"
	"  -col <val>: ECC error injection col\n"
	"  FUNC supported types:\n"
	"     0: DFI alert error interrupt\n"
	"     1: SWCMD error interrupt\n"
	"     2: DUCMD error interrupt\n"
	"     3: LCCMD error interrupt\n"
	"     4: CTRLUPD error interrupt\n"
	"     5: RFM alert interrupt\n"
	"     6: Derate temp limit interrupt\n"
	"     7: SBR done interrupt\n"
	"     8: DPM interrupt\n"
	"     9: DDRPHY interrupt\n"
	"  RAS supported types:\n"
	"     0: ECC corrected error interrupt\n"
	"     1: ECC uncorrected error interrupt\n"
	"     2: Write CRC error interrupt\n"
	"     3: Write CRC error max reached interrupt\n"
	"     4: CAPAR error interrupt\n"
	"     5: CAPAR error max reached interrupt\n"
	"     6: CAPAR fatal error interrupt\n"
	"     7: CAPAR retry limit reached interrupt\n"
	"     8: CAPAR command error interrupt\n"
	"     9: Write CRC retry limit interrupt\n"
	"    10: Read CRC error max reached interrupt\n"
	"    11: Read retry limit interrupt\n"
	"    12: Write buffer P0R0 parity poison enable\n"
	"    13: Write buffer P0R1 parity poison enable\n"
	"    14: Write buffer P1R0 parity poison enable\n"
	"    15: Write buffer P1R1 parity poison enable\n"
	"    16: Read buffer P0R0 parity poison enable\n"
	"    17: Read buffer P0R1 parity poison enable\n"
	"    18: Read buffer P1R0 parity poison enable\n"
	"    19: Read buffer P1R1 parity poison enable\n"
);
