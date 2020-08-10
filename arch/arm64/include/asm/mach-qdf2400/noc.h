#ifndef __NOC_QDF2400_H_INCLUDE__
#define __NOC_QDF2400_H_INCLUDE__

#include <target/arch.h>

#include <driver/monaco.h>
#ifndef ARCH_HAVE_NOC
#define ARCH_HAVE_NOC		1
#else
#error "Multiple NOC controller defined"
#endif

/****************************************************************************
 * Monaco Ring Switch Config Base Address
 *   Space offset between adjacent ring switch config blocks
 ****************************************************************************/
#define ARCH_SU_BASE			0x0FF80000000

/* Monaco Switch Unit IDs */
#define PBSU0				0
#define MBSU0				5
#define DMSU0				17
#define MCSU0				23
#define TRSU0				29

#define NR_PBSUS			5
#define NR_MBSUS			12
#define NR_DMSUS			6
#define NR_MCSUS			6
#define NR_TRSUS			2

/* In HXT1, there are 2 DLBUs:
 * 1. One in the alpha ring, ADLBU can be configured in MBSU1;
 * 2. The other is in the gamma ring, GDLBU can be configured in MBSU7.
 *
 * In HXT2, there is 1 DLBU:
 * 1. One is in the alpha ring, ADLBU can be configured in MBSU2.
 */
#define SU_ADLBU_BASE			MBSU_BASE(1)
#define ARCH_HAS_SU_ADLBU		1
#define SU_GDLBU_BASE			MBSU_BASE(7)
#define ARCH_HAS_SU_GDLBU		1
/* In HXT1 and HXT2, PBSU2 is also CFGPOS.
 * When ring configuration faults, errors are reported to the CFGPOS.
 */
#define SU_CFGPOS_BASE			PBSU_BASE(2)

/* The number of ddr segment addr/mask pairs for all A and B set */
#define SU_MAX_DDR_SEGMENTS		2
/* The number of slave segment addr/mask pairs in each A and B set */
#define SU_MAX_SLAVE_SEGMENTS		7
/* The number of fatcm addr/mask register pairs for all A and B set */
#define SU_MAX_FATCMS			2

/* The number of I/O Coherent Nodes in emulation.
 * The number of I/O Coherent Nodes in silicon.
 */
#define NUM_EMULATION_ICN_NODES		4
#define NUM_SILICON_ICN_NODES		5

#define MAX_L3_NODES			12
#define MAX_CLUSTERS			24
#define DDR_MAX_NUM_CHANS		6
#define DDR_CHAN_MASK			0x3F

#define MONACO_HW_RUMI_PBSU_MASK	0x1E /* PCIE1, SYSF, HDMA and SATA */
#define MONACO_HW_PALLADIUM_PBSU_MASK	0x04 /* only SYSF */
#define MONACO_HW_SILICON_PBSU_MASK	0x1F

/*===========================================================================
 * Highlevel Map
 * Note: These are reserved regions in the address map and not sizes.
 *       (Ex. IMEM & DDR memories as smaller than the regions reserved for
 *        them in the memory map.)
 *---------------------------------------------------------------------------
 * Region                  Size        Start Address      End Address
 *---------------------------------------------------------------------------
 * DDR (MDDR)              768 GB      0x000_0000_0000   0x0BF_FFFF_FFFF
 * System IMEM               4 MB      0x0FF_7000_0000   0x0FF_703F_FFFF
 * Message IMEM              4 MB      0x0FF_7040_0000   0x0FF_707F_FFFF
 * Monaco Config           128 MB      0x0FF_8000_0000   0x0FF_87FF_FFFF
 * SATA                    128 MB      0x0FF_8800_0000   0x0FF_8FFF_FFFF
 * HDMA                    128 MB      0x0FF_9800_0000   0x0FF_9FFF_FFFF
 * PCIe A (Hydra View)       4 TB      0x800_0000_0000   0xBFF_FFFF_FFFF
 * PCIe B (Hydra View)       4 TB      0xC00_0000_0000   0xFFF_FFFF_FFFF
 *===========================================================================
 * Config Space Map through PBSU2(CFGPOS)
 *---------------------------------------------------------------------------
 * Region        SubRegion Size        Start Address     End Address
 *---------------------------------------------------------------------------
 * PBSU 0 Cfg    PCIe 0    1 MB        0x0FF_80000000    0x0FF_800FFFFF
 * PBSU 1 Cfg    PCIe 1    1 MB        0x0FF_80100000    0x0FF_801FFFFF
 * PBSU 2 Cfg    SysFab    1 MB        0x0FF_80200000    0x0FF_802FFFFF
 * PBSU 3 Cfg    HDMA      1 MB        0x0FF_80300000    0x0FF_803FFFFF
 * PBSU 4 Cfg    SATA      1 MB        0x0FF_80400000    0x0FF_804FFFFF
 *
 * MBSU 0 Cfgs   MBSU 0    0.25 MB     0x0FF_80500000    0x0FF_8053FFFF
 *               HDC  0    0.25 MB     0x0FF_80540000    0x0FF_8057FFFF
 *               L3   0    0.25 MB     0x0FF_80580000    0x0FF_805BFFFF
 * MBSU 1 Cfgs   MBSU 1    0.25 MB     0x0FF_80600000    0x0FF_8063FFFF
 *               HDC  3    0.25 MB     0x0FF_80640000    0x0FF_8067FFFF
 *               L3   1    0.25 MB     0x0FF_80680000    0x0FF_806BFFFF
 * MBSU 2 Cfgs   MBSU 2    0.25 MB     0x0FF_80700000    0x0FF_8073FFFF
 *               HDC  4    0.25 MB     0x0FF_80740000    0x0FF_8077FFFF
 *               L3   2    0.25 MB     0x0FF_80780000    0x0FF_807BFFFF
 * MBSU 3 Cfgs   MBSU 3    0.25 MB     0x0FF_80800000    0x0FF_8083FFFF
 *               HDC  7    0.25 MB     0x0FF_80840000    0x0FF_8087FFFF
 *               L3   3    0.25 MB     0x0FF_80880000    0x0FF_808BFFFF
 * MBSU 4 Cfgs   MBSU 4    0.25 MB     0x0FF_80900000    0x0FF_8093FFFF
 *               HDC  8    0.25 MB     0x0FF_80940000    0x0FF_8097FFFF
 *               L3   4    0.25 MB     0x0FF_80980000    0x0FF_809BFFFF
 * MBSU 5 Cfgs   MBSU 5    0.25 MB     0x0FF_80A00000    0x0FF_80A3FFFF
 *               HDC 11    0.25 MB     0x0FF_80A40000    0x0FF_80A7FFFF
 *               L3   5    0.25 MB     0x0FF_80A80000    0x0FF_80ABFFFF
 * MBSU 6 Cfgs   MBSU 6    0.25 MB     0x0FF_80B00000    0x0FF_80B3FFFF
 *               HDC 12    0.25 MB     0x0FF_80B40000    0x0FF_80B7FFFF
 *               L3   6    0.25 MB     0x0FF_80B80000    0x0FF_80BBFFFF
 * MBSU 7 Cfgs   MBSU 7    0.25 MB     0x0FF_80C00000    0x0FF_80C3FFFF
 *               HDC 15    0.25 MB     0x0FF_80C40000    0x0FF_80C7FFFF
 *               L3   7    0.25 MB     0x0FF_80C80000    0x0FF_80CBFFFF
 * MBSU 8 Cfgs   MBSU 8    0.25 MB     0x0FF_80D00000    0x0FF_80D3FFFF
 *               HDC 16    0.25 MB     0x0FF_80D40000    0x0FF_80D7FFFF
 *               L3   8    0.25 MB     0x0FF_80D80000    0x0FF_80DBFFFF
 * MBSU 9 Cfgs   MBSU 9    0.25 MB     0x0FF_80E00000    0x0FF_80E3FFFF
 *               HDC 19    0.25 MB     0x0FF_80E40000    0x0FF_80E7FFFF
 *               L3   9    0.25 MB     0x0FF_80E80000    0x0FF_80EBFFFF
 * MBSU10 Cfgs   MBSU10    0.25 MB     0x0FF_80F00000    0x0FF_80F3FFFF
 *               HDC 20    0.25 MB     0x0FF_80F40000    0x0FF_80F7FFFF
 *               L3  10    0.25 MB     0x0FF_80F80000    0x0FF_80FBFFFF
 * MBSU11 Cfgs   MBSU11    0.25 MB     0x0FF_81000000    0x0FF_8103FFFF
 *               HDC 23    0.25 MB     0x0FF_81040000    0x0FF_8107FFFF
 *               L3  11    0.25 MB     0x0FF_81080000    0x0FF_810BFFFF
 * DMSU 0 Cfgs   DMSU 0    0.25 MB     0x0FF_81100000    0x0FF_8113FFFF
 *               HDC 10    0.25 MB     0x0FF_81140000    0x0FF_8117FFFF
 *               HDC 13    0.25 MB     0x0FF_81180000    0x0FF_811BFFFF
 * DMSU 1 Cfgs   DMSU 1    0.25 MB     0x0FF_81200000    0x0FF_8123FFFF
 *               HDC  9    0.25 MB     0x0FF_81240000    0x0FF_8127FFFF
 *               HDC 14    0.25 MB     0x0FF_81280000    0x0FF_812BFFFF
 * DMSU 2 Cfgs   DMSU 2    0.25 MB     0x0FF_81300000    0x0FF_8133FFFF
 *               HDC  6    0.25 MB     0x0FF_81340000    0x0FF_8137FFFF
 *               HDC 17    0.25 MB     0x0FF_81380000    0x0FF_813BFFFF
 * DMSU 3 Cfgs   DMSU 3    0.25 MB     0x0FF_81400000    0x0FF_8143FFFF
 *               HDC  5    0.25 MB     0x0FF_81440000    0x0FF_8147FFFF
 *               HDC 18    0.25 MB     0x0FF_81480000    0x0FF_814BFFFF
 * DMSU 4 Cfgs   DMSU 4    0.25 MB     0x0FF_81500000    0x0FF_8153FFFF
 *               HDC  2    0.25 MB     0x0FF_81540000    0x0FF_8157FFFF
 *               HDC 21    0.25 MB     0x0FF_81580000    0x0FF_815BFFFF
 * DMSU 5 Cfgs   DMSU 5    0.25 MB     0x0FF_81600000    0x0FF_8163FFFF
 *               HDC  1    0.25 MB     0x0FF_81640000    0x0FF_8167FFFF
 *               HDC 22    0.25 MB     0x0FF_81680000    0x0FF_816BFFFF
 * MCSU 0 Cfg              0.25 MB     0x0FF_81700000    0x0FF_817FFFFF
 * MCSU 1 Cfg              0.25 MB     0x0FF_81800000    0x0FF_818FFFFF
 * MCSU 2 Cfg              0.25 MB     0x0FF_81900000    0x0FF_819FFFFF
 * MCSU 3 Cfg              0.25 MB     0x0FF_81A00000    0x0FF_81AFFFFF
 * MCSU 4 Cfg              0.25 MB     0x0FF_81B00000    0x0FF_81BFFFFF
 * MCSU 5 Cfg              0.25 MB     0x0FF_81C00000    0x0FF_81CFFFFF
 *===========================================================================
 * PBSU Assignments PBSU2(CFGPOS)
 *---------------------------------------------------------------------------
 * Switch    Peripherial    Notes:
 *---------------------------------------------------------------------------
 * PBSU 0    PCIe 0
 * PBSU 1    PCIe 1
 * PBSU 2    SysFab        CFGPOS (Monaco Configuration Space PoS)
 * PBSU 3    HDMA
 * PBSU 4    SATA
 *===========================================================================
 * MCSU Address Routing / Associations
 *---------------------------------------------------------------------------
 * Switch    Interleave    Notes:
 *---------------------------------------------------------------------------
 * MCSU 0    Even Intlv
 * MCSU 1    Odd  Intlv
 * MCSU 3    Even Intlv
 * MCSU 2    Odd  Intlv
 * MCSU 4    Even Intlv
 * MCSU 5    Odd  Intlv
 *===========================================================================
 * Extracted from Monaco 2.0.x Core HPG.docm : Dated 09-01-2015
 *===========================================================================
 * 12 MBSUs
 * 6  DMSUs
 * 5  PBSUs
 * 6  MCSUs
 * NORTH RING WRAPPER(BUSID=0,4)
 *   MCSU0(Even)   SWCH_ID=0x00                MC PID=0x20
 *   MBSU0         SWCH_ID=0x01  FCN PID=0x01  L3 PID=0x21
 *   MBSU1         SWCH_ID=0x02  FCN PID=0x02  L3 PID=0x22
 *   MBSU2         SWCH_ID=0x03  FCN PID=0x03  L3 PID=0x23
 *   MCSU1(Odd)    SWCH_ID=0x04                MC PID=0x24
 *   MBSU3         SWCH_ID=0x05  FCN PID=0x05  L3 PID=0x25
 *   MBSU4         SWCH_ID=0x06  FCN PID=0x06  L3 PID=0x26
 *   MBSU5         SWCH_ID=0x07  FCN PID=0x07  L3 PID=0x27
 *   MCSU2(Odd)    SWCH_ID=0x08                MC PID=0x28
 * EAST RING WRAPPER (BUSID=0,4)
 *   PBSU0         SWCH_ID=0x09  ICN PID=0x09  TN PID=0x29
 *   TRSU0         SWCH_ID=0x0A
 *   PBSU1         SWCH_ID=0x0B  ICN PID=0x0B  TN PID=0x2B
 * SOUTH RING WRAPPER (BUSID=0,4)
 *   MCSU3(Even)   SWCH_ID=0x0C                MC PID=0x2C
 *   MBSU6         SWCH_ID=0x0D  FCN PID=0x0D  L3 PID=0X2D
 *   MBSU7         SWCH_ID=0x0E  FCN PID=0x0E  L3 PID=0x2E
 *   MBSU8         SWCH_ID=0x0F  FCN PID=0x0F  L3 PID=0x2F
 *   MCSU4(Even)   SWCH_ID=0x10                MC PID=0x30
 *   MBSU9         SWCH_ID=0x11  FCN PID=0x11  L3 PID=0x31
 *   MBSU10        SWCH_ID=0x12  FCN PID=0x12  L3 PID=0x32
 *   MBSU11        SWCH_ID=0x13  FCN PID=0x13  L3 PID=0x33
 *   MCSU5(Odd)    SWCH_ID=0x14                MC PID=0x34
 * WEST RING WRAPPER (BUSID=0,4)
 *   DFTSU0        SWCH_ID=0x15
 *   PBSU2         SWCH_ID=0x16  ICN PID=0x16  TN PID=0x36
 *   TRSU1         SWCH_ID=0x17
 *   PBSU3         SWCH_ID=0x18  ICN PID=0x18  TN PID=0x38
 *   PBSU4         SWCH_ID=0x19  ICN PID=0x19  TN PID=0x39
 * CENTER SEGMENT WRAPPER (BUSID=1,5)
 *   DMSU0         SWCH_ID=0x00  FCN PID=0x00 FCN PID=0x01
 *   DMSU1         SWCH_ID=0x02  FCN PID=0x02 FCN PID=0x03
 *   DMSU2         SWCH_ID=0x04  FCN PID=0x04 FCN PID=0x05
 *   DMSU3         SWCH_ID=0x06  FCN PID=0x06 FCN PID=0x07
 *   DMSU4         SWCH_ID=0x08  FCN PID=0x08 FCN PID=0x09
 *   DMSU5         SWCH_ID=0x0A  FCN PID=0x0A FCN PID=0x0B
 * Ration between CPU Cluster and L3
 * Limited by minimum number of L3s on an interleave:
 *   24 clusters for minimum of 4 or more L3
 *   12 clusters for minimum of 2 or 3 L3
 *    6 clusters for minimum of 1 L3
 * NOTE: All registers in Monaco Config Space are 32-bit accessible only
 *       All registers are aligned to a 64-bit boundary.
 * Address alignment of TNs (Target Nodes)
 *   Each TN has 2 sets of address ranges
 *   Power of 2 address range
 *   Address range is 128MB in size minimum!
 *   Each address range is aligned to the size of the range
 *===========================================================================*/
#define RS_TOTAL_NUM		32 /* including DFTSU */
#define RS_CH_TOTAL_NUM		31 /* except DFTSU */

extern const uint8_t su_hw_apc_ids[MAX_CLUSTERS];
extern const uint8_t su_hw_l3u_ids[MAX_L3_NODES];
extern const uint8_t su_hw_ddr_ids[DDR_MAX_NUM_CHANS];

extern const uint8_t su_hw_ddrss_insts[DDR_MAX_NUM_CHANS];
extern const uint8_t su_hw_mcsu_insts[DDR_MAX_NUM_CHANS];

extern const uint32_t su_hw_ddr_segs[SU_MAX_DDR_SEGMENTS][2];
extern const uint32_t su_hw_slave_segs[SU_MAX_SLAVE_SEGMENTS][3];

extern const uint8_t su_hw_switch_apc_map[MAX_CLUSTERS];
/* routing */
extern const uint32_t su_hw_erouting[RS_TOTAL_NUM][2];
/* switch units */
extern const rs_list_t su_hw_switch_lists[RS_CH_TOTAL_NUM];

uint8_t su_get_linear_sid(uint8_t inst);

#endif /* __NOC_QDF2400_H_INCLUDE__ */
