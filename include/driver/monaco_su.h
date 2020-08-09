#ifndef __MONACO_SU_CENTRIQ_H_INCLUDE__
#define __MONACO_SU_CENTRIQ_H_INCLUDE__

#include <target/generic.h>

#define  SOC_HW_MAJOR_VERSION_1        1
#define  SOC_HW_MAJOR_VERSION_2        2

#define  SOC_HW_MINOR_VERSION_0        0
#define  SOC_HW_MINOR_VERSION_1        1
#define  SOC_HW_MINOR_VERSION_2        2

#define  SOC_AUX_VERSION_0             0
#define  SOC_AUX_VERSION_1             1

typedef struct
{
    uint8_t major_version;      //!< Major Version of the SOC.
    uint8_t minor_version;      //!< Minor Version of the SOC.
    uint8_t aux_version;        //!< Auxiliry of the SOC.
} soc_hw_version_t;

/*    SILICON     for a development platform target.
 *    SIM         for a special simulated target.
 *    SIM_RUMI42  for a simulated RUMI42 emulation target.
 *    SIM_RUMI48  for a simulated RUMI48 emulation target.
 *    RUMI42      for a RUMI42 emulation target.
 *    RUMI48      for a RUMI48 emulation target.
 *    CHIPSIM     for a CHIPSIM environment target.
 */
typedef enum
{
    SILICON,
    SIM,
    SIM_RUMI42,
    SIM_RUMI48,
    RUMI42,
    RUMI48,
    PALLADIUM,
    PALLADIUM48,
    CHIPSIM,
    PALLADIUM_Z1
} target_mode_e;


/*    INFORMATIONAL  All priority of messages should be displayed.
 *    LOW            Only low priority messages and above should be displayed.
 *    MEDIUM         Only medium priority messages and above should be displayed.
 *    HIGH           Only high priority messages and above should be displayed.
 *    CRITICAL       Only critical messages should be displayed. Normal release mode.
 *    NO_LOGS        No logging should occur even for critical items. Debug only.
 */
typedef enum _importance_t
{
    INFORMATIONAL = 0,
    LOW = 1,
    MEDIUM = 2,
    HIGH = 3,
    CRITICAL = 4,
    NO_LOGS = 5
} importance_t;

typedef uint64_t  cpucore_t;
typedef uint32_t  l3intlv_t;

typedef struct
{
    cpucore_t hydra_cores;      //!< Each bit represents a single Hydra core.
    l3intlv_t l3_interleaves;   //!< Each bit represents a single L3 cache interleave.
} partial_goods_t;

/*    soc_hw_version         SOC HW version.
 *    goods                  Partial goods indications.
 *    rs_settings            Ring Switch settings.
 *    l3_settings            L3 cache settings.
 *    mddr_settings          MDDR settings.
 *    mode                   Execution target mode.
 *    log_level              Verbosity.
 *    spd_smbus_info         Map of SMBus IDs and addresses.
 */
typedef struct
{
    soc_hw_version_t soc_hw_version;            //!< Indicates the SOC HW Version that has been detected.
    partial_goods_t goods;                      //!< Flags used to indicate which Hydra cores and L3 interleaves yielded.
    rs_settings_t rs_settings;                  //!< Indicates the configurable Ring settings to use.
    l3_settings_t l3_settings;                  //!< Indicates the configurable L3 settings to use.
    mddr_settings_t mddr_settings;              //!< Indicates the configurable DDR settings to use.
    target_mode_e mode;                         //!< Allows adaptation based on the type of execution target.
    importance_t log_level;                     //!< Indication of the level of reporting of initialization details.
    dimm_slot_smbus_info_t spd_smbus_info[DDR_MAX_NUM_CHANS][DDR_MAX_NUM_SLOTS_PER_CHAN];   //!< Provides the map of platform defined SMBus addresses to the specific DIMMs based on chan and slot.
} monaco_init_t;

#define monaco_log(importance, fmt, ...)	do { } while (0)

#define SU_SHIFT			20
#define SU_BASE(id)			\
	(ARCH_SU_BASE + ((id) << SU_SHIFT))
#define SU_ID(base)			\
	((uint8_t)(((base) - ARCH_SU_BASE) >> SU_SHIFT))
#define INVALID_SU_ID			0xFF

/* Monaco Switch Units (SU)
 * 1. Master Bank Switch Unit (MBSU):
 *    This switch supports QSBv3.0 interleaved connection ports to a
 *    Falkor master and a custom interface for L3.
 * 2. Dual Master Switch Unit (DMSU):
 *    This switch supports QSBv3.0 interleaved connection ports to two
 *    Falkor masters.
 * 3. Peripheral Bridge Switch Unit (PBSU):
 *    This switch supports QSBv3.0 interleaved master and slave ports
 *    for connection to the Valencia bridges.
 * 4. Memory Controller Switch Unit (MCSU):
 *    This switch supports a single QSBv3.0 slave connection to the
 *    memory controller. Unlike other switches, this switch only
 *    communicates with one interleaved ring.
 * 5. Triple/Three Ring Switch Unit (TRSU):
 *    This switch connects rings in three directions.
 * 6. DFT Switch Unit (DFTSU):
 *    Design for Trace Switch Unit.
 */
/* 2.15.44.2.3.1. MBSU_HW_ID (0x0FF 8050 0000)
 * The instance ID can be obtained from SU_HW_ID register, used across the
 * entire ring configuration.
 * Maps to {BUS_ID[0], 2'b00, SWCH_ID[4:0]}
 */
#define SU_BUS_ID_OFFSET			7
#define SU_BUS_ID_MASK				REG_1BIT_MASK
#define SU_SWITCH_ID_OFFSET			0
#define SU_SWITCH_ID_MASK			REG_5BIT_MASK
#define SU_BUS_ID(inst)				_GET_FV(SU_BUS_ID, inst)
#define SU_SWITCH_ID(inst)			_GET_FV(SU_SWITCH_ID, inst)

/****************************************************************************
 * Each CPU cluster belongs to one of master switches such as MBSUs and DMSUs
 * but its association between CPU cluster and master switches are not
 * linear. This is to identify the location of each CPU cluster scattered
 * over multiple master switches.
 *   Switch unit type: MBSU_CH or DMSU_CH
 *   FCN index: 0 or 1
 *   Switch unit instance ID: the instance ID of switch unit
 ****************************************************************************/
#define MSTR_SW_COMB_IDX(type,fcn,id)	((((type)&0x7)<<5)|(((fcn)&0x1)<<4)|(((id)&0xF)>>0))
#define MSTR_SW_GET_TYPE(swtch)		(((swtch)>>5)&0x7)
#define MSTR_SW_GET_FIDX(swtch)		(((swtch)>>4)&0x1)
#define MSTR_SW_GET_INST(swtch)		(((swtch)>>0)&0xF)

/* All ring config registers are 4 bytes in size but 8 byte aligned */
#define SU_REG_ALIGN				8

/* The overall silicon contains a specific logic that the address bit 7
 * of the SU registers are taken as interleave ID.
 */
#define SU_INTLV_BIT			7
#define SU_INTLV_LO_MASK		((1 << SU_INTLV_BIT) - 1)
#define SU_INTLV_HI_MASK		(~SU_INTLV_LO_MASK)
#define __su_slave_id(n)				\
	(((((n) * 0x10) & SU_INTLV_LO_MASK) |		\
	 ((((n) * 0x10) & SU_INTLV_HI_MASK) << 1)) / 0x10)
#define SU_MAX_POSSIBLE_SLAVE_SEGMENTS	ALIGN_UP(SU_MAX_SLAVE_SEGMENTS, 8)
/* FATCM is always the last 2 addresses in the slave segment */
#define SU_SLAVE_FATCM			(SU_MAX_POSSIBLE_SLAVE_SEGMENTS - 1)
/* SU_SLAVE_SEGMENT_ADDR/MASK register block size */
#define SU_SLAVE_SEGMENT_REG_SIZE	\
	(SU_MAX_POSSIBLE_SLAVE_SEGMENTS * 4 * SU_REG_ALIGN)

/* Macro used to calculate the base address of the each SU */
#define MBSU_BASE(idx)				SU_BASE(MBSU0 + (idx))
#define DMSU_BASE(idx)				SU_BASE(DMSU0 + (idx))
#define PBSU_BASE(idx)				SU_BASE(PBSU0 + (idx))
#define MCSU_BASE(idx)				SU_BASE(MCSU0 + (idx))
#define TRSU_BASE(idx)				SU_BASE(TRSU0 + (idx))

#define SU_REG(base, off)			((base) + (off))

/****************************************************************************
 * Register address definitions
 ****************************************************************************/
#define SU_HW_ID(base)				SU_REG(base, 0x00000)
#define SU_HW_VERSION(base)			SU_REG(base, 0x00008)
#define SU_CONFIGURATION_REG(base)		SU_REG(base, 0x00010)
#define SU_LFSR_CNTRL_REG(base)			SU_REG(base, 0x00028)
#define SU_LFSR_RST_REG(base)			SU_REG(base, 0x00030)
#define SU_LIMIT_REG(base)			SU_REG(base, 0x00048)
#define SU_BLU_ROUTING_REG_0(base)		SU_REG(base, 0x00160)
#define SU_BLU_ROUTING_REG_1(base)		SU_REG(base, 0x00168)
/* RAS */
#define SU_ERR_STATUS_REG(base)			SU_REG(base, 0x10008)
#define SU_ERR_CLR_REG(base)			SU_REG(base, 0x10010)
#define SU_ERR_CTRL_REG(base)			SU_REG(base, 0x10018)
#define SU_INT_CTRL_REG(base)			SU_REG(base, 0x10020)
#define SU_ERR_CAP_REG_0(base)			SU_REG(base, 0x10040)
#define SU_ERR_CAP_REG_1(base)			SU_REG(base, 0x10048)
#define SU_ERR_CAP_REG_2(base)			SU_REG(base, 0x10050)
#define SU_ERR_CAP_REG_3(base)			SU_REG(base, 0x10058)
#define SU_ERR_INJ_REG(base)			SU_REG(base, 0x10100)
/* CLK */
#define SU_CLK_ON_REQ_REG(base, clk)		\
	SU_REG(base, 0x02000 + (clk) * SU_REG_ALIGN)
/* clk can be the followings */
#define MBSU_LOCL_MSTR_CLK			0
#define PBSU_ICN_CLK				0
#define DMSU_LOCL_MSTR0_CLK			0
#define MCSU_CLK				0
#define DMSU_LOCL_MSTR1_CLK			1
#define PBSU_TN_CLK				1
/* RING_CLK_ON_REQ found in MBSU, DMSU, PBSU, MCSU, TRSU is not useful in
 * HXT1/HXT2 design.
 */
#define SU_RING_CLK				2

/* MBSU, PBSU, DMSU, MCSU */
#define SU_DCVS_CTRL_REG(base)			SU_REG(base, 0x04008)
/* In the HRD, the following registers have n = 1..7, however, the
 * below definition converts it into n = 0..6.
 */
#define SU_DCVS_TX_LO_THRESHOLD_REG(base, n)	\
	SU_REG(base, 0x04100 + SU_REG_ALIGN + (n) * SU_REG_ALIGN)
#define SU_DCVS_TX_HI_THRESHOLD_REG(base, n)	\
	SU_REG(base, 0x04138 + SU_REG_ALIGN + (n) * SU_REG_ALIGN)
#define SU_DCVS_RX_LO_THRESHOLD_REG(base, n)	\
	SU_REG(base, 0x04200 + SU_REG_ALIGN + (n) * SU_REG_ALIGN)
#define SU_DCVS_RX_HI_THRESHOLD_REG(base, n)	\
	SU_REG(base, 0x04238 + SU_REG_ALIGN + (n) * SU_REG_ALIGN)
#define SU_DCVS_INTF0_RX_SHADOW_CNTR_REG(base)	SU_REG(base, 0x04430)
#define SU_DCVS_INTF0_TX_SHADOW_CNTR_REG(base)	SU_REG(base, 0x04438)
#define SU_DCVS_INTF1_RX_SHADOW_CNTR_REG(base)	SU_REG(base, 0x04530)
#define SU_DCVS_INTF1_TX_SHADOW_CNTR_REG(base)	SU_REG(base, 0x04538)

/* MBSU, DMSU, PBSU, i.e., switches connected to FCN, ICN */
#define SU_DVM_POS_REG(base)			SU_REG(base, 0x00038)
#define SU_BAR_POS_REG(base)			SU_REG(base, 0x00040)
#define SU_L3_DDR_CNT_REG(base)			SU_REG(base, 0x00068)
#define SU_EVEN_INTLV_L3_INST_REG(base, n)	\
	SU_REG(base, 0x00200 + SU_REG_ALIGN * ((n) >> 1))
#define SU_ODD_INTLV_L3_INST_REG(base, n)	\
	SU_REG(base, 0x00250 + SU_REG_ALIGN * ((n) >> 1))
#define SU_DDR_DECODE_MODE_REG(base)		SU_REG(base, 0x00300)
#define SU_EVEN_INTLV_DDR_INST_REG(base)	SU_REG(base, 0x00308)
#define SU_ODD_INTLV_DDR_INST_REG(base)		SU_REG(base, 0x00310)
#define SU_DDR_SEGMENT_ADDR_REG(base, n)	\
	SU_REG(base, 0x00400 + (2 * SU_REG_ALIGN) * (n))
#define SU_DDR_SEGMENT_MASK_REG(base, n)		\
	SU_REG(base, 0x00408 + (2 * SU_REG_ALIGN) * (n))
#define SU_SLAVE_SEGMENT_ADDR_A_REG(base, n)	\
		SU_REG(base, 0x00500 + (2 * SU_REG_ALIGN) * __su_slave_id(n))
#define SU_SLAVE_SEGMENT_MASK_A_REG(base, n)	\
		SU_REG(base, 0x00508 + (2 * SU_REG_ALIGN) * __su_slave_id(n))
#define SU_FATCM_ADDR_A_REG(base)		\
		SU_SLAVE_SEGMENT_ADDR_A_REG(base, SU_SLAVE_FATCM)
#define SU_FATCM_MASK_A_REG(base)		\
		SU_SLAVE_SEGMENT_MASK_A_REG(base, SU_SLAVE_FATCM)
#define SU_SLAVE_SEGMENT_ADDR_B_REG(base, n)	\
		SU_REG(base, 0x00500 +			\
			   SU_SLAVE_SEGMENT_REG_SIZE +	\
			   (2 * SU_REG_ALIGN) * __su_slave_id(n))
#define SU_SLAVE_SEGMENT_MASK_B_REG(base, n)	\
		SU_REG(base, 0x00508 +			\
			   SU_SLAVE_SEGMENT_REG_SIZE +	\
			   (2 * SU_REG_ALIGN) * __su_slave_id(n))
#define SU_FATCM_ADDR_B_REG(base)		\
		SU_SLAVE_SEGMENT_ADDR_B_REG(base, SU_SLAVE_FATCM)
#define SU_FATCM_MASK_B_REG(base)		\
		SU_SLAVE_SEGMENT_MASK_B_REG(base, SU_SLAVE_FATCM)
#define SU_INTLV_BIK_REG_OFFSET			\
		(0x00500 +				\
		 2 * SU_SLAVE_SEGMENT_REG_SIZE)
#define SU_INTLV_BIK_REG(base, offset, n)	\
		SU_REG(base, SU_INTLV_BIK_REG_OFFSET +	\
			   (offset) + SU_REG_ALIGN * (n))
#define SU_EVEN_INTLV_BIK_INST_0_REG(base, n)	\
		SU_INTLV_BIK_REG(base, 0x00000, n)
#define SU_EVEN_INTLV_BIK_INST_1_REG(base, n)	\
		SU_INTLV_BIK_REG(base, 0x00100, n)
#define SU_ODD_INTLV_BIK_INST_0_REG(base, n)	\
		SU_INTLV_BIK_REG(base, 0x00200, n)
#define SU_ODD_INTLV_BIK_INST_1_REG(base, n)	\
		SU_INTLV_BIK_REG(base, 0x00300, n)

/* MBSU Specific Registers */
/****************************************************************************
 * Chapter 2.4.8 FCN/Hydra Partial Goods, as of 10/07/2015 Monaco 2.0.x
 * core HPG
 * Num of L3s  Programming SFT_PID#
 *   4 to 12     Supports upto 24 FCN's
 *               Each SFT_PID# field maps to a single FCN Port ID
 *               If fewer than 24 FCN's exist, unused SFT_PID's must be set
 *               to "3F"
 *   2,3         Supports upto 12 FCN's
 *               SFT_PIDn and SFT_PIDn+1 fields must be programmed with same
 *               FCN Port ID
 *               If fewer than 12 FCN's exist, unused SFT_PID's must be set
 *               to "3F"
 *   1           Supports upto 6 FCN's
 *               SFT_PIDn, SFT_PIDn+1, SFT_PIDn+2 and SFT_PIDn+3 fields must
 *               be programmed with same FCN Port ID
 *               If fewer than 6 FCN's exist, unused SFT_PID's must be set to
 *               "3F"
 *   SFT_PID_REG
 *     [22:20] SFT_BIDn+1
 *     [17:12] SFT_PIDn+1
 *     [10:8]  SFT_BIDn
 *     [5:0]   SFT_PIDn
 ****************************************************************************/
#define MBSU_MNC_NODE_TYPE_CNT_REG(base)	SU_REG(base, 0x00060)
#define MBSU_SFT_PID_REG_BASE_OFFSET		0x00100
#define MBSU_SFT_PID_REG_N_ENTRIES		2
#define MBSU_SFT_PID_REG_N_COUNT		(MAX_CLUSTERS/MBSU_SFT_PID_REG_N_ENTRIES)
#define MBSU_SFT_PID_REG(base, n)		\
	SU_REG(base, 0x00100 + SU_REG_ALIGN * ((n) >> 1))

/****************************************************************************
 * Register field definitions
 ****************************************************************************/
/* SU_HW_ID */
#define SU_HW_ID_INSTANCE_OFFSET		0
#define SU_HW_ID_INSTANCE_MASK			REG_8BIT_MASK
#define SU_HW_ID_INSTANCE(value)		_GET_FV(SU_HW_ID_INSTANCE, value)

#define SU_INSTANCE(bus, swch)			\
	(_SET_FV(SU_BUS_ID, bus) | _SET_FV(SU_SWITCH_ID, swch))

/* SU_CONFIGURATION_REG */
/* MBSU, DMSU, PBSU, MCSU */
#define SU_DLB_DAT_THROTTLE_EN			_BV(31)
#define SU_DLB_CMD_THROTTLE_EN			_BV(30)
#define SU_DLB_SNPRSLT_THROTTLE_EN		_BV(29)
#define SU_DLB_RSP_THROTTLE_EN			_BV(28)
#define SU_DLB_THROTTLE_EN			\
	(SU_DLB_DAT_THROTTLE_EN |		\
	 SU_DLB_CMD_THROTTLE_EN |		\
	 SU_DLB_SNPRSLT_THROTTLE_EN |		\
	 SU_DLB_RSP_THROTTLE_EN)
/* MBSU, PBSU, DMSU */
#define SU_L3_AFTY				_BV(12)
/* MBSU */
#define MBSU_DLB_DET_MODE_SEL			_BV(25)
#define MBSU_L3_DATA_INJ_MODE			_BV(24)
#define MBSU_DLB_REC_MODE_SEL			_BV(23)
#define MBSU_FRR				_BV(22)
#define MBSU_FDR				_BV(21)
#define MBSU_HIGH_PRI_CMD_QUEUE_EN		_BV(20)
#define MBSU_FSYNC_CFG				_BV(19)
#define MBSU_EN_L3_CMD_BYPASS			_BV(18)
#define MBSU_EN_CFG_LOOP_BACK_PATH		_BV(17)
#define MBSU_L3_QUIESCE_EN			_BV(16)
#define MBSU_DLB_CFG_OFFSET			13
#define MBSU_DLB_CFG_MASK			REG_3BIT_MASK
#define MBSU_DLB_CFG(value)			_SET_FV(SU_DLB_CFG, value)
#define MBSU_LCL_L3_AVAIL_O			_BV(9)
#define MBSU_LCL_L3_AVAIL_E			_BV(8)
#define MBSU_FLT_ICN_SNP			_BV(7)
#define MBSU_SRC_OOO_MODE			_BV(6)
#define MBSU_DLB_EN				_BV(5)
#define MBSU_LCL_MSTR_AVAIL			_BV(4)
#define MBSU_DLB_CHAN_SEL_OFFSET		0
#define MBSU_DLB_CHAN_SEL_MASK			REG_4BIT_MASK
#define MBSU_DLB_CHAN_SEL(value)		_SET_FV(MBSU_DLB_CHAN_SEL, value)
#define MBSU_DLB_CHAN_DATA			MBSU_DLB_CHAN_SEL(_BV(0))
#define MBSU_DLB_CHAN_COMMAND			MBSU_DLB_CHAN_SEL(_BV(1))
#define MBSU_DLB_CHAN_SNOOP_RESULT		MBSU_DLB_CHAN_SEL(_BV(2))
#define MBSU_DLB_CHAN_RESPONSE			MBSU_DLB_CHAN_SEL(_BV(3))
#define MBSU_DLB_CHAN_ALL			\
	(MBSU_DLB_CHAN_DATA |			\
	 MBSU_DLB_CHAN_COMMAND |		\
	 MBSU_DLB_CHAN_SNOOP_RESULT |		\
	 MBSU_DLB_CHAN_RESPONSE)
/* DMSU */
#define DMSU_FRR				_BV(21)
#define DMSU_FDR				_BV(20)
#define DMSU_EN_CFG_LOOP_BACK_PATH		_BV(17)
#define DMSU_LCL_MSTR1_AVAIL			_BV(5)
#define DMSU_LCL_MSTR0_AVAIL			_BV(4)
#define DMSU_FSYNC_CFG				_BV(0)
/* MCSU */
#define MCSU_FDR				_BV(23)
#define MCSU_MIN_HIGH_PRI_CMDS_OFFSET		18
#define MCSU_MIN_HIGH_PRI_CMDS_MASK		REG_2BIT_MASK
#define MCSU_MIN_HIGH_PRI_CMDS(value)		_SET_FV(MCSU_MIN_HIGH_PRI_CMDS, value)
#define MCSU_HIGH_PRI_CMD_QUEUE_EN		_BV(16)
#define MCSU_FLOP_SYNC_MODE			_BV(8)
#define MCSU_SNP_RSP_EN				_BV(0)
/* PBSU */
#define PBSU_FRR				_BV(21)
#define PBSU_FDR				_BV(20)
#define PBSU_FSYNC_CFG				_BV(2)
#define PBSU_FLT_ICN_SNP			_BV(1)
#define PBSU_BLK_ALL_SNP_TO_ICN			_BV(0)
/* TRSU */
#define TRSU_DLB_DAT_THROTTLE_EN		_BV(31)
#define TRSU_DLB_SNPRSLT_THROTTLE_EN		_BV(30)
#define TRSU_DLB_THROTTLE_EN			\
	(TRSU_DLB_DAT_THROTTLE_EN |		\
	 TRSU_DLB_SNPRSLT_THROTTLE_EN)
#define TRSU_FRR				_BV(21)
#define TRSU_FDR				_BV(20)

/* SU_LFSR_CNTRL_REG, all */
#define SU_LFSR_EN				_BV(0)
/* SU_LFSR_CNTRL_REG, MBSU, PBSU, DMSU, TRSU */
#define SU_LFSR_DRRR				_BV(4)
#define SU_LFSR_DRRD				_BV(3)
#define SU_LFSR_DRRS				_BV(2)
#define SU_LFSR_DRRC				_BV(1)
/* SU_LFSR_CNTRL_REG, MBSU, PBSU, DMSU */
#define SU_LFSR_LRBSM				_BV(12)
#define SU_LFSR_RTV_OFFSET			6
#define SU_LFSR_RTV_MASK			REG_6BIT_MASK
#define SU_LFSR_RTV(value)			_SET_FV(SU_LFSR_RTV, value)
#define SU_LFSR_BRCE				_BV(5)
/* SU_LFSR_CNTRL_REG, MBSU, DMSU */
#define SU_LFSR_BIT_MASK_OFFSET			26
#define SU_LFSR_BIT_MASK_MASK			REG_6BIT_MASK
#define SU_LFSR_BIT_MASK(value)			_SET_FV(SU_LFSR_BIT_MASK, value)
#define SU_LFSR_LLB_LVL_OFFSET			22
#define SU_LFSR_LLB_LVL_MASK			REG_4BIT_MASK
#define SU_LFSR_LLB_LVL(value)			_SET_FV(SU_LFSR_LLB_LVL, value)
#define SU_LFSR_MASK_LLB_LVL_OFFSET		18
#define SU_LFSR_MASK_LLB_LVL_MASK		REG_4BIT_MASK
#define SU_LFSR_MASK_LLB_LVL(value)		_SET_FV(SU_LFSR_MASK_LLB_LVL, value)
#define SU_LFSR_SBRCE_MASK			\
	(SU_LFSR_BIT_MASK(SU_LFSR_BIT_MASK_MASK) | \
	 SU_LFSR_LLB_LVL(SU_LFSR_LLB_LVL_MASK) | \
	 SU_LFSR_MASK_LLB_LVL(SU_LFSR_MASK_LLB_LVL_MASK))
#define SU_LFSR_SBRCE				_BV(17)
/* SU_LFSR_CNTRL_REG, MBSU */
#define MBSU_LFSR_PSBVS_OFFSET			14
#define MBSU_LFSR_PSBVS_MASK			REG_3BIT_MASK
#define MBSU_LFSR_PSBVS(value)			_SET_FV(SU_LFSR_PSBVS, value)
#define MBSU_LFSR_PSBE				_BV(13)
/* SU_LFSR_CNTRL_REG, MCSU */
#define MCSU_LFSR_DRLNR				_BV(4)
#define MCSU_LFSR_DRLND				_BV(3)
#define MCSU_LFSR_DRLNS				_BV(2)
/* SU_LFSR_CNTRL_REG, TRSU */
#define TRSU_LFSR_SPSM_OFFSET			5
#define TRSU_LFSR_SPSM_MASK			REG_2BIT_MASK
#define TRSU_LFSR_SPSM(value)			_SET_FV(SU_LFSR_SPSM, value)

/* MBSU_CH:
 * DMSU_CH:
 * PBSU_CH: 0x01E
 *   bit[12] LRBSM = 0 - Select 3bit backoff value
 *   bit[11:6] RTV = 0 - Retry Threshold Value at the local master interface
 *   bit[5]   BRCE = 0 - Disable retry based backoff
 *   bit[4]   DRRR = 1 - Enable random ring delays for response
 *   bit[3]   DRRD = 1 - Enable random ring delays for data
 *   bit[2]   DRRS = 1 - Enable random ring delays for snoops/results
 *   bit[1]   DRRC = 1 - Enable random ring delays for commands/requests
 *   bit[0] LFSREN
 * MCSU_CH: 0x01C
 *   bit[4]  DRLNR = 1 - Enable random local node delays for response
 *   bit[3]  DRLND = 1 - Enable random local node delays for data
 *   bit[2]  DRLNS = 1 - Enable random local node delays for snoops/results
 *   bit[0] LFSREN
 * TRSU_CH: 0x01E
 *   bit[6:5] SPSM = 0
 *   bit[4]   DRRR = 1 - Enable random ring delays for response
 *   bit[3]   DRRD = 1 - Enable random ring delays for data
 *   bit[2]   DRRS = 1 - Enable random ring delays for snoops/results
 *   bit[1]   DRRC = 1 - Enable random ring delays for commands/requests
 *   bit[0] LFSREN
 */
#define SU_LFSR_DEFAULT_NON_MCSU	\
	(SU_LFSR_DRRC | SU_LFSR_DRRS | SU_LFSR_DRRD | SU_LFSR_DRRR)
#define SU_LFSR_DEFAULT_MCSU		\
	(MCSU_LFSR_DRLNR | MCSU_LFSR_DRLND | MCSU_LFSR_DRLNS)
#define SU_LFSR_DEFAULT			\
	(SU_LFSR_DEFAULT_NON_MCSU | SU_LFSR_DEFAULT_MCSU)

/* SU_DVM/BAR_POS_REG */
#define SU_POS_INTLV_OFFSET(intlv)	(12 * (intlv))
#define SU_POS_INTLV_MASK(intlv)				\
	((SU_POS_INTLV_BID(SU_POS_INTLV_BID_MASK) |		\
	  SU_POS_INTLV_SWCH_ID(SU_POS_INTLV_SWCH_ID_MASK)) <<	\
	 SU_POS_INTLV_OFFSET(intlv))
#define SU_POS_INTLV_BID_OFFSET		8
#define SU_POS_INTLV_BID_MASK		REG_3BIT_MASK
#define SU_POS_INTLV_BID(value)		_SET_FV(SU_POS_INTLV_BID, value)
#define SU_POS_INTLV_SWCH_ID_OFFSET	0
#define SU_POS_INTLV_SWCH_ID_MASK	REG_5BIT_MASK
#define SU_POS_INTLV_SWCH_ID(value)	_SET_FV(SU_POS_INTLV_SWCH_ID, value)

/* MBSU_MNC_NODE_TYPE_CNT_REG */
#define MBSU_FCN_CNT_OFFSET		0
#define MBSU_FCN_CNT_MASK		REG_5BIT_MASK
#define MBSU_FCN_CNT(value)		_SET_FV(MBSU_FCN_CNT, value)
#define MBSU_ICN_CNT_OFFSET		8
#define MBSU_ICN_CNT_MASK		REG_5BIT_MASK
#define MBSU_ICN_CNT(value)		_SET_FV(MBSU_ICN_CNT, value)
#define MBSU_TN_CNT_OFFSET		16
#define MBSU_TN_CNT_MASK		REG_5BIT_MASK
#define MBSU_TN_CNT(value)		_SET_FV(MBSU_TN_CNT, value)

/* SU_L3_DDR_CNT_REG */
#define SU_DDR_CH_CNT_OFFSET		16
#define SU_DDR_CH_CNT_MASK		REG_4BIT_MASK
#define SU_DDR_CH_CNT(value)		_SET_FV(SU_DDR_CH_CNT, value)
#define SU_L3_ODD_INTLV_CNT_OFFSET	8
#define SU_L3_ODD_INTLV_CNT_MASK	REG_4BIT_MASK
#define SU_L3_ODD_INTLV_CNT(value)	_SET_FV(SU_L3_ODD_INTLV_CNT, value)
#define SU_L3_EVEN_INTLV_CNT_OFFSET	0
#define SU_L3_EVEN_INTLV_CNT_MASK	REG_4BIT_MASK
#define SU_L3_EVEN_INTLV_CNT(value)	_SET_FV(SU_L3_EVEN_INTLV_CNT, value)

/* MBSU_SFT_PID_REG */
#define MBSU_SFT_OFFSET(n)		(12 * ((n) & 0x1))
#define MBSU_SFT_PID_OFFSET		0
#define MBSU_SFT_PID_MASK		REG_5BIT_MASK
#define MBSU_SFT_PID(value)		_SET_FV(MBSU_SFT_PID, value)
#define MBSU_SFT_BID_OFFSET		8
#define MBSU_SFT_BID_MASK		REG_3BIT_MASK
#define MBSU_SFT_BID(value)		_SET_FV(MBSU_SFT_BID, value)
#define MBSU_SFT_MASK(n)			\
	((MBSU_SFT_PID(MBSU_SFT_PID_MASK) |	\
	  MBSU_SFT_BID(MBSU_SFT_BID_MASK)) <<	\
	 MBSU_SFT_OFFSET(n))
#define MBSU_SFT_PID_UNUSED		0x3F

/* SU_EVEN/ODD_INTLV_L3_INST_REG */
#define SU_INTLV_L3_INST_OFFSET(n)		(12 * ((n) & 0x01))
#define SU_INTLV_L3_INST_L3U_BUS_ID_OFFSET(n)	(SU_INTLV_L3_INST_OFFSET(n) + 8)
#define SU_INTLV_L3_INST_L3U_BUS_ID_MASK	REG_3BIT_MASK
#define SU_INTLV_L3_INST_L3U_BUS_ID(n, value)			\
	(((value) & SU_INTLV_L3_INST_L3U_BUS_ID_MASK) <<	\
	 SU_INTLV_L3_INST_L3U_BUS_ID_OFFSET(n))
#define SU_INTLV_L3_INST_L3U_SWCH_ID_OFFSET(n)	(SU_INTLV_L3_INST_OFFSET(n) + 0)
#define SU_INTLV_L3_INST_L3U_SWCH_ID_MASK	REG_5BIT_MASK
#define SU_INTLV_L3_INST_L3U_SWCH_ID(n, value)			\
	(((value) & SU_INTLV_L3_INST_L3U_SWCH_ID_MASK) <<	\
	 SU_INTLV_L3_INST_L3U_SWCH_ID_OFFSET(n))
#define SU_INTLV_L3_INST_MASK(n)			\
	(SU_INTLV_L3_INST_L3U_BUS_ID(n,			\
	 SU_INTLV_L3_INST_L3U_BUS_ID_MASK) |		\
	 SU_INTLV_L3_INST_L3U_SWCH_ID(n,		\
	 SU_INTLV_L3_INST_L3U_SWCH_ID_MASK))

/* SU_DDR_DECODE_MODE_REG */
/* Address space distribution at 256B, 1K, 4K, 8K boundary.
 * Valid only when available DDR instances is 2.
 */
#define SU_DDR_DIST_CNTL_OFFSET		8
#define SU_DDR_DIST_CNTL_MASK		REG_5BIT_MASK
#define SU_DDR_DIST_CNTL(value)		_SET_FV(SU_DDR_DIST_CNTL, value)
#define SU_DDR_DIST_GRANULE_OF_256	1
#define SU_DDR_DIST_GRANULE_OF_4K	8
#define SU_DDR_DIST_GRANULE_OF_8K	16

/* SU_EVEN/ODD_INTLV_DDR_INST_REG */
#define SU_INTLV_DDR_INST_OFFSET(n)		(8 * ((n) & 0x03))
#define SU_INTLV_DDR_INST_DDR_SWCH_ID_OFFSET(n)	(SU_INTLV_DDR_INST_OFFSET(n) + 0)
#define SU_INTLV_DDR_INST_DDR_SWCH_ID_MASK	REG_5BIT_MASK
#define SU_INTLV_DDR_INST_DDR_SWCH_ID(n, value)			\
	(((value) & SU_INTLV_DDR_INST_DDR_SWCH_ID_MASK) <<	\
	 SU_INTLV_DDR_INST_DDR_SWCH_ID_OFFSET(n))
#define SU_INTLV_DDR_INST_MASK(n)				\
	(SU_INTLV_DDR_INST_DDR_SWCH_ID(n,			\
	 SU_INTLV_DDR_INST_DDR_SWCH_ID_MASK))

/* SU_DDR/SLAVE_SEGMENT_ADDR/MASK_A/B_REG, SU_FATCM_ADDR_A/B_REG */
/* DDR/SLAVE/FATCM ADDR_A/B */
#define SU_SEG_EN			_BV(31)
/* DDR ADDR_A/B */
#define SU_PA_43_31_DDR_BASE_OFFSET	0
#define SU_PA_43_31_DDR_BASE_MASK	REG_13BIT_MASK
#define SU_PA_43_31_DDR_BASE(value)	_SET_FV(SU_PA_43_31_DDR_BASE, value)
/* DDR MASK_A/B */
#define SU_PA_43_31_SEG_MASK_OFFSET	0
#define SU_PA_43_31_SEG_MASK_MASK	REG_13BIT_MASK
#define SU_PA_43_31_SEG_MASK(value)	_SET_FV(SU_PA_43_31_SEG_MASK, value)
/* SLAVE/FATCM ADDR_A/B */
#define SU_PA_43_27_SEG_BASE_OFFSET	0
#define SU_PA_43_27_SEG_BASE_MASK	REG_17BIT_MASK
#define SU_PA_43_27_SEG_BASE(value)	_SET_FV(SU_PA_43_27_SEG_BASE, value)
/* SLAVE/FATCM MASK_A/B */
#define SU_PA_43_27_SEG_MASK_OFFSET	0
#define SU_PA_43_27_SEG_MASK_MASK	REG_17BIT_MASK
#define SU_PA_43_27_SEG_MASK(value)	_SET_FV(SU_PA_43_27_SEG_MASK, value)
/* DDR/SLAVE ADDR_A */
#define MBSU_SEG_CACHE_EN		_BV(30)
/* SLAVE ADDR_A */
#define SU_SEG_SWCH_ID_OFFSET		20
#define SU_SEG_SWCH_ID_MASK		REG_5BIT_MASK
#define SU_SEG_SWCH_ID(value)		_SET_FV(SU_SEG_SWCH_ID, value)
/* MSB and LSB used to manipulate the contents of the
 * SU_DDR_SEGMENT_ADDR_REG and SU_DDR_SEGMENT_MASK_REG
 * PA_43_31 for bit[12:0]
 */
#define DDR_SEG_PA_MSB			43
#define DDR_SEG_PA_LSB			31
/* MSB and LSB used to manipulate the contents of the
 * SU_SLAVE_SEGMENT_ADDR_REG and SU_SLAVE_SEGMENT_MASK_REG
 * PA_43_27 for bit[16:0]
 */
#define SLV_SEG_PA_MSB			43
#define SLV_SEG_PA_LSB			27
#define SU_SEG_ADDR(msb, lsb, addr)	\
	((addr) & (1 << (uint32_t)((msb) - (lsb) + 1)) - 1)
/* Macro used in master_list_slv_seg to combine SEG_EN and its SWID */
#define SEG_EN_ID(en, id)		(((en)<<16)|((id)&0xFF))
/* Macro used to take SEG_EN out of the given value */
#define GET_SEG_EN(n)			(((n)>>16)&0x1)
/* Macro used to take SEG_ID out of the given value */
#define GET_SEG_ID(n)			(((n)&0xFF))
/* MSB and LSB used to manipulate the contents of the SU_FATCM_ADDR_REG
 * and SU_FATCM_MASK_REG PA_43_27 for bit[16:0]
 */
#define FATCM_PA_MSB		43
#define FATCM_PA_LSB		27

/* SU_EVEN/ODD_INTLV_BIK_INST_0/1_REG */
/* INST_0 */
#define __SU_BIK0_PRIM_OFFSET		28
#define __SU_BIK0_PRIM_MASK		REG_4BIT_MASK
#define SU_BIK0_PRIM(value)		_SET_FV(__SU_BIK0_PRIM, value)
#define __SU_BIK0_PRIM_MASK_OFFSET	24
#define __SU_BIK0_PRIM_MASK_MASK	REG_4BIT_MASK
#define SU_BIK0_PRIM_MASK(value)	_SET_FV(__SU_BIK0_PRIM_MASK, value)
/* INST_1 */
#define SU_BIK1_EN			_BV(26)
#define __SU_BIK1_PRIM_OFFSET		22
#define __SU_BIK1_PRIM_MASK		REG_4BIT_MASK
#define SU_BIK1_PRIM(value)		_SET_FV(__SU_BIK1_PRIM, value)
#define __SU_BIK1_SEC_OFFSET		18
#define __SU_BIK1_SEC_MASK		REG_4BIT_MASK
#define SU_BIK1_SEC(value)		_SET_FV(__SU_BIK1_SEC, value)
#define __SU_BIK1_SEC_MASK_OFFSET	14
#define __SU_BIK1_SEC_MASK_MASK		REG_4BIT_MASK
#define SU_BIK1_SEC_MASK(value)		_SET_FV(__SU_BIK1_SEC_MASK, value)
#define SU_BIK2_EN			_BV(12)
#define __SU_BIK2_PRIM_OFFSET		8
#define __SU_BIK2_PRIM_MASK		REG_4BIT_MASK
#define SU_BIK2_PRIM(value)		_SET_FV(__SU_BIK2_PRIM, value)
#define __SU_BIK2_SEC_OFFSET		4
#define __SU_BIK2_SEC_MASK		REG_4BIT_MASK
#define SU_BIK2_SEC(value)		_SET_FV(__SU_BIK2_SEC, value)
#define __SU_BIK2_SEC_MASK_OFFSET	0
#define __SU_BIK2_SEC_MASK_MASK		REG_4BIT_MASK
#define SU_BIK2_SEC_MASK(value)		_SET_FV(__SU_BIK2_SEC_MASK, value)

/* SU_ERR_CLR_REG */
#define SU_CLR_ERR			_BV(0)

/* SU_ERR_CTRL_REG/SU_INT_CTRL_REG */
#define SU_ERR_CFG_PW			_BV(2)
#define SU_ERR_CFG_XT			_BV(1)
#define SU_ERR_CFG_DEC			_BV(0)
#define SU_ERR_CFG_ALL			\
	(SU_ERR_CFG_PW | SU_ERR_CFG_XT | SU_ERR_CFG_DEC | 0x18)

/* SU_CLK_ON_REQ_REGs */
#define SU_CODTMR_RST_VAL_OFFSET	21
#define SU_CODTMR_RST_VAL_MASK		REG_5BIT_MASK
#define SU_CODTMR_RST_VAL(value)	_SET_FV(SU_CODTMR_RST_VAL, value)
#define SU_COHTMR_RST_VAL_OFFSET	1
#define SU_COHTMR_RST_VAL_MASK		REG_10BIT_MASK
#define SU_COHTMR_RST_VAL(value)	_SET_FV(SU_COHTMR_RST_VAL, value)
#define SU_CLK_ON_REQ_EN		_BV(0)
#define SU_CODTMR_RST_VAL_MIN		5   /* clk_on_delay_timer min 5 */
#define SU_COHTMR_RST_VAL_MIN		30  /* clk_on_hysteresis_timer min 30 */

/* SU_DCVS_CTRL_REG */
#define SU_DCVS_EN			_BV(31)
#define SU_CLR_SHD_CNT			_BV(30)
#define SU_INTF1_TXC_MASK_OFFSET	12
#define SU_INTF1_TXC_MASK_MASK		REG_2BIT_MASK
#define SU_INTF1_TXC_MASK(value)	_SET_FV(SU_INTF1_TXC_MASK, value)
#define SU_INTF1_RXC_MASK_OFFSET	8
#define SU_INTF1_RXC_MASK_MASK		REG_4BIT_MASK
#define SU_INTF1_RXC_MASK(value)	_SET_FV(SU_INTF1_RXC_MASK, value)
#define SU_INTF0_TXC_MASK_OFFSET	4
#define SU_INTF0_TXC_MASK_MASK		REG_2BIT_MASK
#define SU_INTF0_TXC_MASK(value)	_SET_FV(SU_INTF0_TXC_MASK, value)
#define SU_INTF0_RXC_MASK_OFFSET	0
#define SU_INTF0_RXC_MASK_MASK		REG_4BIT_MASK
#define SU_INTF0_RXC_MASK(value)	_SET_FV(SU_INTF0_RXC_MASK, value)
#define SU_INTF_RXC_TXC_MASKS(i0r, i0t, i1r, i1t)	\
	(SU_INTF0_RXC_MASK(i0r) |			\
	 SU_INTF0_TXC_MASK(i0t) |			\
	 SU_INTF1_RXC_MASK(i1r) |			\
	 SU_INTF1_TXC_MASK(i1t))
#define SU_DCVS_INTF_MASKS				\
	(SU_INTF0_RXC_MASK(SU_INTF0_RXC_MASK_MASK) |	\
	 SU_INTF0_TXC_MASK(SU_INTF0_TXC_MASK_MASK) |	\
	 SU_INTF1_RXC_MASK(SU_INTF1_RXC_MASK_MASK) |	\
	 SU_INTF1_TXC_MASK(SU_INTF1_TXC_MASK_MASK))

/* This function determines the register value that should be written into
 * a single [SWCH]_L3_DDR_CNT_REG register.
 * Change the address decoding PA[15:12] and PA[11:8]
 * 1. Spread mode (default mode) when l3 affinity = 0
 *    to spread the cache line selection every 256B across L3 instances.
 *    PA[15:8] are used as the primary L3 instance selection bits.
 * 2. Affinity mode when l3 affinity = 1
 *    to sustain a certain L3 instance selected for the memory use up to
 *    4KB.
 *    PA bits [15:12] are used for the instance selection and PA [11:8]
 *    are used as secondary bits for instance selection in the case of
 *    partial good or non-power of two L3 instance selection.
 */
#define __su_set_affinity_mode(base)				\
	__raw_setl(SU_L3_AFTY, SU_CONFIGURATION_REG(base))
#define __su_set_spread_mode(base)				\
	__raw_clearl(SU_L3_AFTY, SU_CONFIGURATION_REG(base))

/*  This macro populates the values for SEGMENT_ADDR and SEGMENT_MASK with
 *  the given switch ID, the base address and the size of the region
 *  according to chapter 2.4.2 TN Address decoding in HPG 80-Q1234-1H Rev. A.
 */
#define __su_seg_addr_decode(base, size, msb, lsb, seg_addr, seg_mask)	\
	do {								\
		uint8_t __mbit = (uint8_t)(msb - lsb + 1);		\
		uint32_t __mask = (1 << (uint32_t)__mbit) - 1;		\
		uint32_t __size = size & __mask;			\
		seg_addr = base & __mask;				\
		seg_mask = (__mask + 1 - __size) & (__mask);		\
	} while (0)


/* Macro used to check the system constraints if they're valid
 *   Max APC count is 6 when a valid l3 count is 1
 *   Max APC count is 12 when a valid l3 count is 2 or 3
 */
#define MAX_APC_CNT_L3_1		6
#define MAX_APC_CNT_L3_23		12

#define NUM_INTERLEAVES			2 /* Even and odd interleaves */
#define NUM_APC_TYPE			2 /* MBSU(0) and DMSU(1) */
#define NUM_FCN_TYPE			2 /* FCN0 and FCN1, max number of FCN within one master switch */

#define RS_BUM_PERFNDX_MAX		7 /* n=1..7 */

#define is_pcie_pbsu(pbsu_id)	(pbsu_id == 0 ? true :	\
				 pbsu_id == 1 ? true :	\
				 pbsu_id == 5 ? true :	\
				 false)
/* Switch type values used to indicate a particular switch type when the
 * distinction is required.
 */
typedef enum {
	MBSU_CH  = 0x0, /* minor=1 */
	DMSU_CH  = 0x1, /* minor=3 */
	PBSU_CH  = 0x2, /* minor=4 */
	MCSU_CH  = 0x3,	/* minor=2 */
	TRSU_CH  = 0x4,	/* minor=6 */
} rs_type_e;

typedef struct {
	uint8_t su_type: 4;
	uint8_t su_inst: 4;
} rs_list_t;

/* This type is to determine if the partial goods in the system is capable
 * to support the Monaco system.
 *  apc_cnt     Number of the valid APC in partial goods
 *  ddr_cnt0    Number of the valid ddr channel on even interleave
 *  ddr_cnt1    Number of the valid ddr channel on odd interleave
 *  l3u_cnt0    Number of the valid l3 instance on even interleave
 *  l3u_cnt1    Number of the valid l3 instance on odd interleave
 */
typedef struct {
	uint8_t apc_cnt;
	uint8_t ddr_cnt0;
	uint8_t ddr_cnt1;
	uint8_t l3u_cnt0;
	uint8_t l3u_cnt1;
} rs_goods_t;

/* This type contains the information to handle the Ring BUM in each
 * switch type.
 */
typedef struct {
	uint32_t rs_mask_mbsu;
	uint32_t rs_mask_dmsu;
	uint32_t rs_mask_pbsu;
	uint32_t rs_mask_mcsu;
} rs_mask_su_t;

/* This type defines the connections for Ring BUM used in each switch.
 *   MBSU - Intf0(QSB3M FCN) Intf1(L3 POS/Master)
 *   DMSU - Intf0(QSB3M FCN) Intf1(QSB3M FCN)
 *   PBSU - Intf0(QSB3M ICN) Intf1(QSB3S TN)
 *   MCSU - Intf0(QSB3S TN)  Intf1(NA)
 * The enumeration represent the bit masks used to enable the channels
 * of the Rx/Tx interface0/1 and are used in SU_DCVS_CTRL_REG.
 *       SU_DCVS_CTRL_REG[INTF0_RXC_MASK and INTF0_TXC_MASK]
 *       SU_DCVS_CTRL_REG[INTF1_RXC_MASK and INTF1_TXC_MASK]
 */
typedef enum {
	/* QSB3 Master FCN/ICN
	 * Valid for MBSU Intf0, DMSU Intf0/1, PBSU Intf0
	 */
	Q3MRxCmdCh = 0x01, /* QSB3 Master Rx Command Channel */
	Q3MRxDatCh = 0x02, /* QSB3 Master Rx Data Channel */
	Q3MRxRspCh = 0x04, /* QSB3 Master Rx Response Channel */
	Q3MTxSnpRsltCh = 0x10, /* QSB3 Master Tx Snoop Result Channel */
	Q3MTxDatCh = 0x20, /* QSB3 Master Tx Data Channel */
	/* L3 POS/Master
	 * Valid for MBSU Intf1
	 */
	L3RxCmdCh = 0x01, /* L3 Rx Command Channel */
	L3RxDatCh0Ch = 0x02, /* L3 Rx Data Channel0 Channel */
	L3RxDatCh1Ch = 0x04, /* L3 Rx Data Channel1 Channel */
	L3RxBusSnpRsltCh = 0x08, /* L3 Tx Bus Snoop Result Channel */
	L3TxBusCmdCh = 0x10, /* L3 Tx Bus Command Channel */
	/* QSB3 Slave TN
	 * Valid for PBSU Intf1, MCSU Intf0
	 */
	Q3SRxDatCh = 0x02, /* QSB3 Slave Rx Data Channel */
	Q3SRxSnpRsltCh = 0x08, /* QSB3 Slave Rx Snoop Result Channel */
	Q3STxCmdCh = 0x10, /* QSB3 Slave Tx Command Channel */
	Q3STxDatCh = 0x20, /* QSB3 Slave Tx Data Channel */
} rs_bum_intf_e;

typedef struct
{
	uint32_t tx_lo_thresh[RS_BUM_PERFNDX_MAX];
	uint32_t tx_hi_thresh[RS_BUM_PERFNDX_MAX];
	uint32_t rx_lo_thresh[RS_BUM_PERFNDX_MAX];
	uint32_t rx_hi_thresh[RS_BUM_PERFNDX_MAX];
} rs_bum_thresh_t;

/* This type is to return all the shadow counter register contents.
 *   rs_intf0_rx SU_DCVS_INTF0_RX_SHADOW_CNTR_REG
 *   rs_intf0_tx SU_DCVS_INTF0_TX_SHADOW_CNTR_REG
 *   rs_intf1_rx SU_DCVS_INTF1_RX_SHADOW_CNTR_REG
 *   rs_intf1_tx SU_DCVS_INTF1_TX_SHADOW_CNTR_REG
 */
typedef struct {
	uint32_t rs_intf0_rx;
	uint32_t rs_intf0_tx;
	uint32_t rs_intf1_rx;
	uint32_t rs_intf1_tx;
} rs_bum_cntr_t;

/* This type defines the level for the Secondary Backoff Request. */
typedef enum {
	RS_SBRLVL_0,  /* Don't enter llMode but stay in regularMode */
	RS_SBRLVL_1,
	RS_SBRLVL_2,
	RS_SBRLVL_3,
	RS_SBRLVL_4,
	RS_SBRLVL_5,
	RS_SBRLVL_6,
	RS_SBRLVL_7,
	RS_SBRLVL_8,
	RS_SBRLVL_9,
	RS_SBRLVL_10,
	RS_SBRLVL_11,
	RS_SBRLVL_12,
	RS_SBRLVL_13,
	RS_SBRLVL_14,
	RS_SBRLVL_15,
	RS_SBRLVL_MAX = RS_SBRLVL_15,
} rs_sbrlvl_e;

/* This type provides a container to group all for the high-level
 * configuration controls available to adjust the Ring initialization.
 * Include both the hardware dependent settings and the selectable /
 * configurable options.
 *    rs_sbrce_ena         Boolean to enable/disable Secondary Backoff Request
 *                         Channel Enable on MBSU and DMSU
 *    rs_sbrlvl            Livelock Backoff Request Level, valid in 0 to 15
 *                         SBRCE will stay in the regular mode when set to 0
 *    rs_sbrmsk            Backoff Request Mask, valid in 6 bit mask
 *                         Correspond LFSR bit is not considered by SBRCE
 *    rs_dlbu_ena          Boolean to enable/disable Dead Lock Breaker Unit
 *    rs_eroute_ena        Boolean to enable/disable Erouting scheme
 *    rs_lfsr_ena          Boolean to enable/disable LFSR control
 *    rs_clkon_req_ena     Boolean to enable/disable the ClkOnReq function
 *                         on the master/slave in each switch
 *    rs_clkon_delay       Delay time after ClkOnReq is asserted before
 *                         sending the first transfer out onto the interface
 *    rs_clkon_hysteresis  Hysteresis time before ClkOnReq gets de-asserted
 *    rs_bum_ena           Boolean to disable and enable Ring BUM
 *    rs_perfndx_max       Max index of performance index, max index is 7
 *    rs_bum_thresh        Structure to setup ring bum in each ring switch type
 */
typedef struct {
	/* Ring Switch SBRCE (Secondary Backoff Request Channel Enable) */
	bool rs_sbrce_ena;
	rs_sbrlvl_e rs_sbrlvl;
	uint32_t rs_sbrmsk;
	/* Ring Switch DLBU (Dead Lock Breaker Unit) */
	bool rs_dlbu_ena;
	/* Ring Switch Erouting scheme */
	bool rs_eroute_ena;
	/* Ring Switch LFSR Control */
	bool rs_lfsr_ena;
	/* Ring Switch ClkOn Request feature */
	bool rs_clkon_req_ena;
	uint32_t rs_clkon_delay;
	uint32_t rs_clkon_hysteresis;
	/* Ring Switch BUM feature */
	bool rs_bum_ena;
	uint32_t rs_perfndx_max;
	rs_bum_thresh_t rs_bum_thresh[4]; /* MBSU, DMSU, PBSU, MCSU */
} rs_settings_t;

#include <asm/mach/monaco_su.h>

extern rs_mask_su_t rs_active_mask;
void rs_config_fatcm_segment(uint64_t base, const uint32_t **seg_lists);
void rs_read_bum_counter(rs_type_e su_type, uint32_t su_id,
			 rs_bum_cntr_t *bum_cnt);
unsigned int rs_init(monaco_init_t *init_parms_ptr);

#endif /* __MONACO_SU_CENTRIQ_H_INCLUDE__ */
