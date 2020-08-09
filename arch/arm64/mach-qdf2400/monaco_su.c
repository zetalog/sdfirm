#include <driver/monaco_su.h>

/******************************************************************************
 * Master list in the order of the ring switch units to program. This helps
 * rs_init invoke the proper function pointer with the instance id. The order
 * needs to be secured when Erouting is required.
 ******************************************************************************/
const rs_list_t su_hw_switch_lists[RS_CH_TOTAL_NUM] = {
	/* su_type, ins#, swch_id */
	{ MCSU_CH, 0 }, /* 0 */
	{ MBSU_CH, 0 }, /* 1 */
	{ MBSU_CH, 1 }, /* 2 */
	{ MBSU_CH, 2 }, /* 3 */
	{ MCSU_CH, 1 }, /* 4 */
	{ MBSU_CH, 3 }, /* 5 */
	{ MBSU_CH, 4 }, /* 6 */
	{ MBSU_CH, 5 }, /* 7 */
	{ MCSU_CH, 2 }, /* 8 */
	{ PBSU_CH, 0 }, /* 9 */
	{ PBSU_CH, 1 }, /* 11 */
	{ MCSU_CH, 3 }, /* 12 */
	{ MBSU_CH, 6 }, /* 13 */
	{ MBSU_CH, 7 }, /* 14 */
	{ MBSU_CH, 8 }, /* 15 */
	{ MCSU_CH, 4 }, /* 16 */
	{ MBSU_CH, 9 }, /* 17 */
	{ MBSU_CH, 10}, /* 18 */
	{ MBSU_CH, 11}, /* 19 */
	{ MCSU_CH, 5 }, /* 20 */
	{ PBSU_CH, 3 }, /* 24 */
	{ PBSU_CH, 4 }, /* 25 */
	{ DMSU_CH, 0 }, /* 0 */
	{ DMSU_CH, 1 }, /* 2 */
	{ DMSU_CH, 2 }, /* 4 */
	{ DMSU_CH, 3 }, /* 6 */
	{ DMSU_CH, 4 }, /* 8 */
	{ DMSU_CH, 5 }, /* 10 */
	{ TRSU_CH, 0 }, /* 10 */
	{ TRSU_CH, 1 }, /* 23 */
	{ PBSU_CH, 2 }, /* 22 */
};

/******************************************************************************
 * Master list of MBSU switch IDs - ordered by L3 cache nodes 0 to 11.
 *   bit[7] - BID, bit[6:0] - SWID
 ******************************************************************************/
const uint8_t su_hw_l3u_ids[MAX_L3_NODES] = {
	SU_INSTANCE(0, 0x01),
	SU_INSTANCE(0, 0x02),
	SU_INSTANCE(0, 0x03),
	SU_INSTANCE(0, 0x05),
	SU_INSTANCE(0, 0x06),
	SU_INSTANCE(0, 0x07),
	SU_INSTANCE(0, 0x0D),
	SU_INSTANCE(0, 0x0E),
	SU_INSTANCE(0, 0x0F),
	SU_INSTANCE(0, 0x11),
	SU_INSTANCE(0, 0x12),
	SU_INSTANCE(0, 0x13),
};

/******************************************************************************
 * Master list of Hydra Cluster PIDs - ordered by cluster 0 to 23.
 *   bit[7] - BID, bit[6:0] - PID
 ******************************************************************************/
const uint8_t su_hw_apc_ids[MAX_CLUSTERS] = {
	SU_INSTANCE(0, 0x01),
	SU_INSTANCE(1, 0x0A),
	SU_INSTANCE(1, 0x08),
	SU_INSTANCE(0, 0x02),
	SU_INSTANCE(0, 0x03),
	SU_INSTANCE(1, 0x06),
	SU_INSTANCE(1, 0x04),
	SU_INSTANCE(0, 0x05),
	SU_INSTANCE(0, 0x06),
	SU_INSTANCE(1, 0x02),
	SU_INSTANCE(1, 0x00),
	SU_INSTANCE(0, 0x07),
	SU_INSTANCE(0, 0x0D),
	SU_INSTANCE(1, 0x01),
	SU_INSTANCE(1, 0x03),
	SU_INSTANCE(0, 0x0E),
	SU_INSTANCE(0, 0x0F),
	SU_INSTANCE(1, 0x05),
	SU_INSTANCE(1, 0x07),
	SU_INSTANCE(0, 0x11),
	SU_INSTANCE(0, 0x12),
	SU_INSTANCE(1, 0x09),
	SU_INSTANCE(1, 0x0B),
	SU_INSTANCE(0, 0x13),
};

/******************************************************************************
 * Master list of MCSU switch IDs - ordered by logical DDR channel 0 to 5.
 * Notes:
 *   A logical DDR channel reflects the default address walk order found in the
 *   Monaco bus. Each logical channel maps to a single MCSU and HDDRSS.
 *    Logical    MCSU        HDDRSS     Switch
 *    Channel   Instance    Instance      ID      Interleave
 *    -------   --------    --------    ------    ----------
 *      0          0           2         0x00       Even
 *      1          1           3         0x04       Odd
 *      2          3           5         0x0C       Even
 *      3          2           4         0x08       Odd
 *      4          4           0         0x10       Even
 *      5          5           1         0x14       Odd
 * bit[7] - BID, bit[6:0] - SWID
 ******************************************************************************/
const uint8_t su_hw_ddr_ids[DDR_MAX_NUM_CHANS] = {
	SU_INSTANCE(0, 0x00),
	SU_INSTANCE(0, 0x04),
	SU_INSTANCE(0, 0x0C),
	SU_INSTANCE(0, 0x08),
	SU_INSTANCE(0, 0x10),
	SU_INSTANCE(0, 0x14),
};

/******************************************************************************
 * Master list of the base address and its size used in DDR_SEGMENT
 * Notes:
 *   The memory map for DDR memory can be set via two sets of address ranges
 *   (A/B), with each address range defined with a pair of ADDR and MASK
 *   registers.
 *   Each ADDR/MASK register pair defines a power-of-2 address range that can
 *   be at a minimum 2GB in size.
 *    Each address range is aligned to the size of the address range.
 *    If an address range is not aligned to its size, multiple segment register
 *    sets can be used. The programmed memory map is evenly distributed across
 *    the available controllers.
 *    ===============================================================
 *    TN Region         Size     Start Address      End Address
 *    ---------------------------------------------------------------
 *    DDR (MDDR)        768 GB   0x000_0000_0000   0x0BF_FFFF_FFFF
 *    ===============================================================
 *    ADDR PA_43:31 and MASK PA_43:31
 ******************************************************************************/
const uint32_t su_hw_ddr_segs[SU_MAX_DDR_SEGMENTS][2] = {
	/* ADDR_A PA_43:31 and MASK_A PA_43:31 */
	{
		0x000000000000 >> DDR_SEG_PA_LSB,
		0x008000000000 >> DDR_SEG_PA_LSB
	},
	/* ADDR_B PA_43:31 and MASK_B PA_43:31 */
	{
		0x008000000000 >> DDR_SEG_PA_LSB,
		0x004000000000 >> DDR_SEG_PA_LSB
	},
};

/******************************************************************************
 * Master list of the base address and its size used in SLAVE_SEGMENT
 * Notes:
 *   Each TN has two sets of address ranges (A/B), with each address range
 *   defined with a pair of ADDR and MASK registers.
 *   Each ADDR/MASK register pair defines a power-of-2 address range that can
 *   be at a minimum 128MB in size.
 *   Each address range is aligned to the size of the address range.
 *   If an address range is not aligned to its size, multiple segment register
 *   sets can be used. The programmed memory map is evenly distributed across
 *   the available controllers.
 *   ==========================================================================
 *   TN Region     Conn'ed PBSU/SWID      Size   Start Address     End Address
 *   --------------------------------------------------------------------------
 *   SVBU          PBSU2  SW_ID=0x16    2 GB   0x0FF_0000_0000  0x0FF_7FFF_FFFF
 *       SysIMEM                        4 MB   0x0FF_7000_0000  0x0FF_703F_FFFF
 *       MsgIMEM                        4 MB   0x0FF_7040_0000  0x0FF_707F_FFFF
 *   MonacoConfig  Always valid       128 MB   0x0FF_8000_0000  0x0FF_87FF_FFFF
 *   SATA          PBSU4  SW_ID=0x19  128 MB   0x0FF_8800_0000  0x0FF_8FFF_FFFF
 *   HDMA          PBSU3  SW_ID=0x18  128 MB   0x0FF_9800_0000  0x0FF_9FFF_FFFF
 *   PCIe 0        PBSU0  SW_ID=0x09    4 TB   0x800_0000_0000  0xBFF_FFFF_FFFF
 *   PCIe 1        PBSU1  SW_ID=0x0B    4 TB   0xC00_0000_0000  0xFFF_FFFF_FFFF
 *   ==========================================================================
 *   SWID, ADDR PA_43:27 and MASK PA_43:27
 *   Provides only for set A
 ******************************************************************************/
const uint32_t su_hw_slave_segs[SU_MAX_SLAVE_SEGMENTS][3] = {
	/* PBSU0 */
	{
		SEG_EN_ID(1, 0x09),
		0x080000000000 >> SLV_SEG_PA_LSB,
		0x040000000000 >> SLV_SEG_PA_LSB
	},
	/* PBSU1 */
	{
		SEG_EN_ID(1, 0x0B),
		0x0C0000000000 >> SLV_SEG_PA_LSB,
		0x040000000000 >> SLV_SEG_PA_LSB
	},
	/* PBSU2 */
	{
		SEG_EN_ID(1, 0x16),
		0x00FF00000000 >> SLV_SEG_PA_LSB,
		0x000080000000 >> SLV_SEG_PA_LSB
	},
	/* PBSU3 */
	{
		SEG_EN_ID(1, 0x18),
		0x00FF98000000 >> SLV_SEG_PA_LSB,
		0x000008000000 >> SLV_SEG_PA_LSB
	},
	/* PBSU4 */
	{
		SEG_EN_ID(1, 0x19),
		0x00FF88000000 >> SLV_SEG_PA_LSB,
		0x000008000000 >> SLV_SEG_PA_LSB
	},
};

/******************************************************************************
 * Description
 *   Map indicating which switches / switch types connect to which clusters.
 *   Each element is for a single cluster. Switch and cluster IDs are based
 *   on order left to right. The list contains owner switch type keys that
 *   map switches, by type, to the usable clusters they own. Position of a
 *   key within the list indicates both the cluster associated and the
 *   order of the switch within all switches of the particular
 *   type. Derived from HPG Table 2-3
 *   Type : Select one between MBSU_CH or DMSU_CH
 *   ID   : bit[7]=FCN ID and bit[6:0]=SU Instance ID
 * MBSU and DMSU Switch IDs
 *   MBSU0  BID=0,4 SWCH_ID=0x01 FCN PID=0x01  L3 PID=0x21, HDC_0 , L3_0
 *   MBSU1  BID=0,4 SWCH_ID=0x02 FCN PID=0x02  L3 PID=0x22, HDC_3 , L3_1
 *   MBSU2  BID=0,4 SWCH_ID=0x03 FCN PID=0x03  L3 PID=0x23, HDC_4 , L3_2
 *   MBSU3  BID=0,4 SWCH_ID=0x05 FCN PID=0x05  L3 PID=0x25, HDC_7 , L3_3
 *   MBSU4  BID=0,4 SWCH_ID=0x06 FCN PID=0x06  L3 PID=0x26, HDC_8 , L3_4
 *   MBSU5  BID=0,4 SWCH_ID=0x07 FCN PID=0x07  L3 PID=0x27, HDC_11, L3_5
 *   MBSU6  BID=0,4 SWCH_ID=0x0D FCN PID=0x0D  L3 PID=0X2D, HDC_12, L3_6
 *   MBSU7  BID=0,4 SWCH_ID=0x0E FCN PID=0x0E  L3 PID=0x2E, HDC_15, L3_7
 *   MBSU8  BID=0,4 SWCH_ID=0x0F FCN PID=0x0F  L3 PID=0x2F, HDC_16, L3_8
 *   MBSU9  BID=0,4 SWCH_ID=0x11 FCN PID=0x11  L3 PID=0x31, HDC_19, L3_9
 *   MBSU10 BID=0,4 SWCH_ID=0x12 FCN PID=0x12  L3 PID=0x32, HDC_20, L3_10
 *   MBSU11 BID=0,4 SWCH_ID=0x13 FCN PID=0x13  L3 PID=0x33, HDC_23, L3_11
 *   DMSU0  BID=1,5 SWCH_ID=0x00 FCN PID=0x00 FCN PID=0x01, HDC_10, HDC_13
 *   DMSU1  BID=1,5 SWCH_ID=0x02 FCN PID=0x02 FCN PID=0x03, HDC_9 , HDC_14
 *   DMSU2  BID=1,5 SWCH_ID=0x04 FCN PID=0x04 FCN PID=0x05, HDC_6 , HDC_17
 *   DMSU3  BID=1,5 SWCH_ID=0x06 FCN PID=0x06 FCN PID=0x07, HDC_5 , HDC_18
 *   DMSU4  BID=1,5 SWCH_ID=0x08 FCN PID=0x08 FCN PID=0x09, HDC_2 , HDC_21
 *   DMSU5  BID=1,5 SWCH_ID=0x0A FCN PID=0x0A FCN PID=0x0B, HDC_1 , HDC_22
 ******************************************************************************/
const uint8_t su_hw_switch_apc_map[MAX_CLUSTERS] = {
	MSTR_SW_COMB_IDX(MBSU_CH, 0,  0),  /* HDC 0 , MBSU, FCN0, SU InsID=0 */
	MSTR_SW_COMB_IDX(DMSU_CH, 0,  5),  /* HDC 1 , DMSU, FCN0, SU InsID=5 */
	MSTR_SW_COMB_IDX(DMSU_CH, 0,  4),  /* HDC 2 , DMSU, FCN0, SU InsID=4 */
	MSTR_SW_COMB_IDX(MBSU_CH, 0,  1),  /* HDC 3 , MBSU, FCN0, SU InsID=1 */
	MSTR_SW_COMB_IDX(MBSU_CH, 0,  2),  /* HDC 4 , MBSU, FCN0, SU InsID=2 */
	MSTR_SW_COMB_IDX(DMSU_CH, 0,  3),  /* HDC 5 , DMSU, FCN0, SU InsID=3 */
	MSTR_SW_COMB_IDX(DMSU_CH, 0,  2),  /* HDC 6 , DMSU, FCN0, SU InsID=2 */
	MSTR_SW_COMB_IDX(MBSU_CH, 0,  3),  /* HDC 7 , MBSU, FCN0, SU InsID=3 */
	MSTR_SW_COMB_IDX(MBSU_CH, 0,  4),  /* HDC 8 , MBSU, FCN0, SU InsID=4 */
	MSTR_SW_COMB_IDX(DMSU_CH, 0,  1),  /* HDC 9 , DMSU, FCN0, SU InsID=1 */
	MSTR_SW_COMB_IDX(DMSU_CH, 0,  0),  /* HDC 10, DMSU, FCN0, SU InsID=0 */
	MSTR_SW_COMB_IDX(MBSU_CH, 0,  5),  /* HDC 11, MBSU, FCN0, SU InsID=5 */
	MSTR_SW_COMB_IDX(MBSU_CH, 0,  6),  /* HDC 12, MBSU, FCN0, SU InsID=6 */
	MSTR_SW_COMB_IDX(DMSU_CH, 1,  0),  /* HDC 13, DMSU, FCN1, SU InsID=0 */
	MSTR_SW_COMB_IDX(DMSU_CH, 1,  1),  /* HDC 14, DMSU, FCN1, SU InsID=1 */
	MSTR_SW_COMB_IDX(MBSU_CH, 0,  7),  /* HDC 15, MBSU, FCN0, SU InsID=7 */
	MSTR_SW_COMB_IDX(MBSU_CH, 0,  8),  /* HDC 16, MBSU, FCN0, SU InsID=8 */
	MSTR_SW_COMB_IDX(DMSU_CH, 1,  2),  /* HDC 17, DMSU, FCN1, SU InsID=2 */
	MSTR_SW_COMB_IDX(DMSU_CH, 1,  3),  /* HDC 18, DMSU, FCN1, SU InsID=3 */
	MSTR_SW_COMB_IDX(MBSU_CH, 0,  9),  /* HDC 19, MBSU, FCN0, SU InsID=9 */
	MSTR_SW_COMB_IDX(MBSU_CH, 0,  10), /* HDC 20, MBSU, FCN0, SU InsID=10 */
	MSTR_SW_COMB_IDX(DMSU_CH, 1,  4),  /* HDC 21, DMSU, FCN1, SU InsID=4 */
	MSTR_SW_COMB_IDX(DMSU_CH, 1,  5),  /* HDC 22, DMSU, FCN1, SU InsID=5 */
	MSTR_SW_COMB_IDX(MBSU_CH, 0,  11), /* HDC 23, MBSU, FCN0, SU InsID=11 */
};

/******************************************************************************
 * Master list of HDDRSS instance No. - ordered by logical DDR channel 0 to 5.
 * Notes:
 *   A logical DDR channel reflects the default address walk order found in the
 *   Monaco bus. Each logical channel maps to a single MCSU and HDDRSS.
 ******************************************************************************/
const uint8_t su_hw_ddrss_insts[DDR_MAX_NUM_CHANS] = {
	2,
	3,
	5,
	4,
	0,
	1,
};

/******************************************************************************
 * Master list of MSCU instance No.- ordered by logical DDR channel 0 to 5.
 * Notes:
 *   A logical DDR channel reflects the default address walk order found in the
 *   Monaco bus. Each logical channel maps to a single MCSU and HDDRSS.
 ******************************************************************************/
const uint8_t su_hw_mcsu_insts[DDR_MAX_NUM_CHANS] = {
	0,
	1,
	3,
	2,
	4,
	5,
};

/******************************************************************************
 * Erouting scheme in the Monaco Ring.
 * Master list of BLU_ROUTING_0/1 for all the switch units. The order is the
 * SWCH_ID which can be read from HW_ID.
 ******************************************************************************/
const uint32_t su_hw_erouting[RS_TOTAL_NUM][2] = {
	/* routing0   routing1 */
	{ 0x000003FE, 0x00000000 }, /* MCSU0  0   0x00 */
	{ 0x000003FC, 0x00000000 }, /* MBSU0  1   0x01 */
	{ 0x000003F8, 0x00000000 }, /* MBSU1  2   0x02 */
	{ 0x000003F0, 0x00000000 }, /* MBSU2  3   0x03 */
	{ 0x000003E0, 0x00000000 }, /* MCSU1  4   0x04 */
	{ 0x000003C0, 0x00000000 }, /* MBSU3  5   0x05 */
	{ 0x00000380, 0x00000000 }, /* MBSU4  6   0x06 */
	{ 0x00000300, 0x00000000 }, /* MBSU5  7   0x07 */
	{ 0x00000200, 0x00000000 }, /* MCSU2  8   0x08 */
	{ 0x00000000, 0x00000000 }, /* PBSU0  9   0x09 */
	{ 0x00000000, 0x00000000 }, /* TRSU0  10  0x0A */
	{ 0x03FFF3FF, 0x00000FFF }, /* PBSU1  11  0x0B */
	{ 0x03FFE3FF, 0x00000FFF }, /* MCSU3  12  0x0C */
	{ 0x03FFC3FF, 0x00000FFF }, /* MBSU6  13  0x0D */
	{ 0x03FF83FF, 0x00000FFF }, /* MBSU7  14  0x0E */
	{ 0x03FF03FF, 0x00000FFF }, /* MBSU8  15  0x0F */
	{ 0x03FE03FF, 0x00000FFF }, /* MCSU4  16  0x10 */
	{ 0x03FC03FF, 0x00000FFF }, /* MBSU9  17  0x11 */
	{ 0x03F803FF, 0x00000FFF }, /* MBSU10 18  0x12 */
	{ 0x03F003FF, 0x00000FFF }, /* MBSU11 19  0x13 */
	{ 0x03E003FF, 0x00000FFF }, /* MCSU5  20  0x14 */
	{ 0x00000000, 0x00000000 }, /* DFTSU  21  0x15 */
	{ 0x038003FF, 0x00000FFF }, /* PBSU2  22  0x16 */
	{ 0x030003FF, 0x00000000 }, /* TRSU1  23  0x17 */
	{ 0x020003FF, 0x00000000 }, /* PBSU3  24  0x18 */
	{ 0x000003FF, 0x00000000 }, /* PBSU4  25  0x19 */
	{ 0x00000FFC, 0x03FFFFFF }, /* DMSU0  26  0x80 */
	{ 0x00000FF0, 0x03FFFFFF }, /* DMSU1  27  0x82 */
	{ 0x00000FC0, 0x03FFFFFF }, /* DMSU2  28  0x84 */
	{ 0x00000F00, 0x03FFFFFF }, /* DMSU3  29  0x86 */
	{ 0x00000C00, 0x03FFFFFF }, /* DMSU4  30  0x88 */
	{ 0x00000000, 0x03FFFFFF }, /* DMSU5  31  0x8A */
};

/* Convert instance ID into linear array index in su_hw_switch_lists */
uint8_t su_get_linear_sid(uint8_t inst)
{
	if (inst & 0x80)
		return ((inst & 0x1F) >> 1) + 0x1A;
	else
		return inst;
}
