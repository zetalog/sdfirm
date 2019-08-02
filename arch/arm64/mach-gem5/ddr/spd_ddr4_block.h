/*!****************************************************************************
* Filename: spd_ddr4_block.h
*
* Description: Provides the DDR4 specific offsets / structure of an SPD block as
*               defined for DDR4 DIMMs.
*
******************************************************************************/
#ifndef _SPD_DDR4_BLOCK_H
#define _SPD_DDR4_BLOCK_H

/******************************************************************************
* Defined Constants and Macros
******************************************************************************/
// SPD EEPROM Field Byte Offsets/IDs DDR4
#define SPD_OFFSET_DDR4_SDRAM_PACKAGE_TYPE                0x06
#define SPD_OFFSET_DDR4_MODULE_NOM_VOLTAGE                0x0B
#define SPD_OFFSET_DDR4_MODULE_ORGANIZATION               0x0C
#define SPD_OFFSET_DDR4_MODULE_MEMORY_BUS_WIDTH           0x0D
#define SPD_OFFSET_DDR4_MODULE_THERMAL_SENSOR             0x0E
#define SPD_OFFSET_DDR4_TIMEBASES                         0x11
#define SPD_OFFSET_DDR4_TCKAVGMIN_MTB                     0x12
#define SPD_OFFSET_DDR4_TCKAVGMIN_FTB                     0x7D
#define SPD_OFFSET_DDR4_TCKAVGMAX_MTB                     0x13
#define SPD_OFFSET_DDR4_TCKAVGMAX_FTB                     0x7C
#define SPD_OFFSET_DDR4_CAS_LAT_1ST_BYTE                  0x14
#define SPD_OFFSET_DDR4_CAS_LAT_2ND_BYTE                  0x15
#define SPD_OFFSET_DDR4_CAS_LAT_3RD_BYTE                  0x16
#define SPD_OFFSET_DDR4_CAS_LAT_4TH_BYTE                  0x17
#define SPD_OFFSET_DDR4_TAAMIN_MTB                        0x18
#define SPD_OFFSET_DDR4_TAAMIN_FTB                        0x7B
#define SPD_OFFSET_DDR4_TRCDMIN_MTB                       0x19
#define SPD_OFFSET_DDR4_TRCDMIN_FTB                       0x7A
#define SPD_OFFSET_DDR4_TRPMIN_MTB                        0x1A
#define SPD_OFFSET_DDR4_TRPMIN_FTB                        0x79
#define SPD_OFFSET_DDR4_TRC_TRAS_MSB                      0x1B
#define SPD_OFFSET_DDR4_TRAS_LSB                          0x1C
#define SPD_OFFSET_DDR4_TRC_LSB                           0x1D
#define SPD_OFFSET_DDR4_TRC_FTB                           0x78
#define SPD_OFFSET_DDR4_TRFC1MIN_LSB                      0x1E
#define SPD_OFFSET_DDR4_TRFC1MIN_MSB                      0x1F
#define SPD_OFFSET_DDR4_TRFC2MIN_LSB                      0x20
#define SPD_OFFSET_DDR4_TRFC2MIN_MSB                      0x21
#define SPD_OFFSET_DDR4_TRFC4MIN_LSB                      0x22
#define SPD_OFFSET_DDR4_TRFC4MIN_MSB                      0x23
#define SPD_OFFSET_DDR4_TFAW_MSB                          0x24
#define SPD_OFFSET_DDR4_TFAW_LSB                          0x25
#define SPD_OFFSET_DDR4_TRRDSMIN_MTB                      0x26
#define SPD_OFFSET_DDR4_TRRDSMIN_FTB                      0x77
#define SPD_OFFSET_DDR4_TRRDLMIN_MTB                      0x27
#define SPD_OFFSET_DDR4_TRRDLMIN_FTB                      0x76
#define SPD_OFFSET_DDR4_TCCDLMIN_MTB                      0x28
#define SPD_OFFSET_DDR4_TCCDLMIN_FTB                      0x75
#define SPD_OFFSET_DDR4_TWRMIN_MSB                        0x29
#define SPD_OFFSET_DDR4_TWRMIN_LSB                        0x2A
#define SPD_OFFSET_DDR4_TWTRMIN_MSB                       0x2B
#define SPD_OFFSET_DDR4_TWTRSMIN_LSB                      0x2C
#define SPD_OFFSET_DDR4_TWTRLMIN_LSB                      0x2D
#define SPD_OFFSET_DDR4_UDIMM_MIRROR                      0x83


// DDR4 RDIMM and LRDIMM specific
#define SPD_OFFSET_DDR4_RDIMM_LRDIMM_MIRROR               0x88
#define SPD_OFFSET_DDR4_RDIMM_LRDIMM_OUTPUT_DRIVE_CTL     0x89
#define SPD_OFFSET_DDR4_RDIMM_LRDIMM_OUTPUT_DRIVE_CK      0x8A


// DDR4 LRDIMM specific (bytes 134-156) 0x86-0x9C
#define SPD_OFFSET_DDR4_LRDIMM_DB_REV                     0x8B
#define SPD_OFFSET_DDR4_LRDIMM_VREFDQ_R0                  0x8C
#define SPD_OFFSET_DDR4_LRDIMM_VREFDQ_R1                  0x8D
#define SPD_OFFSET_DDR4_LRDIMM_VREFDQ_R2                  0x8E
#define SPD_OFFSET_DDR4_LRDIMM_VREFDQ_R3                  0x8F
#define SPD_OFFSET_DDR4_LRDIMM_DB_VREFDQ                  0x90
#define SPD_OFFSET_DDR4_LRDIMM_DB_RON_RTT_1866            0x91
#define SPD_OFFSET_DDR4_LRDIMM_DB_RON_RTT_2400            0x92
#define SPD_OFFSET_DDR4_LRDIMM_DB_RON_RTT_3200            0x93
#define SPD_OFFSET_DDR4_LRDIMM_RON                        0x94
#define SPD_OFFSET_DDR4_LRDIMM_RTT_WR_NOM_1866            0x95
#define SPD_OFFSET_DDR4_LRDIMM_RTT_WR_NOM_2400            0x96
#define SPD_OFFSET_DDR4_LRDIMM_RTT_WR_NOM_3200            0x97
#define SPD_OFFSET_DDR4_LRDIMM_RTT_PARK_1866              0x98
#define SPD_OFFSET_DDR4_LRDIMM_RTT_PARK_2400              0x99
#define SPD_OFFSET_DDR4_LRDIMM_RTT_PARK_3200              0x9A
#define SPD_OFFSET_DDR4_LRDIMM_DB_DRAM_VREFDQ_RANGE       0x9B
#define SPD_OFFSET_DDR4_LRDIMM_DB_DQ_DFE                  0x9C


// DDR4 Manufacturing Information
#define SPD_OFFSET_DDR4_MFG_ID_LSB                        0x140
#define SPD_OFFSET_DDR4_MFG_ID_MSB                        0x141
#define SPD_OFFSET_DDR4_MFG_SN_0                          0x145
#define SPD_OFFSET_DDR4_MFG_SN_1                          0x146
#define SPD_OFFSET_DDR4_MFG_SN_2                          0x147
#define SPD_OFFSET_DDR4_MFG_SN_3                          0x148
#define SPD_OFFSET_DDR4_PART_NUM_LSB                      0x149 // 20 contigious bytes for part number.


// EEPROM Field / subfield access constants.
//
// Note: Some fields are stored as a power of 2 and are scaled to save bits.
//  I.e. Eliminating values that are smaller than the minimum value.
//
#define SPD_DDR4_DIE_COUNT___M                            0x00000007
#define SPD_DDR4_DIE_COUNT___S                            0x4
#define SPD_DDR4_DIE_COUNT_SCALE_OFFSET                   0x1

#define SPD_DDR4_SIGNAL_LOAD___M                          0x00000003
#define SPD_DDR4_SIGNAL_LOAD___S                          0x0
#define SPD_DDR4_SIGNAL_LOAD_SINGLE                       0x2

#define SPD_DDR4_SDRAM_PACKAGE_TYPE___M                   0x00000001
#define SPD_DDR4_SDRAM_PACKAGE_TYPE___S                   0x7
#define SPD_DDR4_SDRAM_PACKAGE_TYPE_MONO                  0x1

#define SPD_DDR4_MODULE_VDD_1_2_V___M                     0x00000003
#define SPD_DDR4_MODULE_VDD_1_2_V___S                     0x0
#define SPD_DDR4_MODULE_VDD_1_2_V_OPERABLE                0x1
#define SPD_DDR4_MODULE_VDD_1_2_V_ENDURANT                0x2

#define SPD_DDR4_FTB___M                                  0x00000003
#define SPD_DDR4_FTB___S                                  0x0
#define SPD_DDR4_FTB_VAL                                  0x0
#define SPD_DDR4_FTB_PS                                   1

#define SPD_DDR4_MTB___M                                  0x00000003
#define SPD_DDR4_MTB___S                                  0x2
#define SPD_DDR4_MTB_VAL                                  0x0
#define SPD_DDR4_MTB_PS                                   125

#define SPD_DDR4_CAL_LAT_1ST___M                          0x000000FF
#define SPD_DDR4_CAL_LAT_1ST___S                          0x0
#define SPD_DDR4_CAL_LAT_2ND___M                          0x000000FF
#define SPD_DDR4_CAL_LAT_2ND___S                          0x0
#define SPD_DDR4_CAL_LAT_3RD___M                          0x000000FF
#define SPD_DDR4_CAL_LAT_3RD___S                          0x0
#define SPD_DDR4_CAL_LAT_4TH___M                          0x0000007F
#define SPD_DDR4_CAL_LAT_4TH___S                          0x0

#define SPD_DDR4_TXX_MTB___M                              0x000000FF
#define SPD_DDR4_TXX_MTB___S                              0x0

#define SPD_DDR4_TXX_FTB___M                              0x000000FF
#define SPD_DDR4_TXX_FTB___S                              0x0

#define SPD_DDR4_TRAS_MTB_MSN___M                         0x0000000F
#define SPD_DDR4_TRAS_MTB_MSN___S                         0x0

#define SPD_DDR4_TRC_MTB_MSN___M                          0x0000000F
#define SPD_DDR4_TRC_MTB_MSN___S                          0x4

#define SPD_DDR4_OUT_DRV_STRENGTH___M                     0x00000003

#define SPD_DDR4_OUT_DRV_CK_Y13___S                       0x2
#define SPD_DDR4_OUT_DRV_CK_Y02___S                       0x0

#define SPD_DDR4_OUT_DRV_CTL_CKE___S                      0x0
#define SPD_DDR4_OUT_DRV_CTL_ODT___S                      0x2
#define SPD_DDR4_OUT_DRV_CTL_CA___S                       0x4
#define SPD_DDR4_OUT_DRV_CTL_CS___S                       0x6

#define SPD_DDR4_TWR_MSB___M                              0x0000000F
#define SPD_DDR4_TWR_MSB___S                              0x0
#define SPD_DDR4_TWR_LSB___M                              0x000000FF
#define SPD_DDR4_TWR_LSB___S                              0x0

#define SPD_DDR4_TWTRS_MSB_MSN___M                        0x0000000F
#define SPD_DDR4_TWTRS_MSB_MSN___S                        0x0
#define SPD_DDR4_TWTRS_LSB___M                            0x000000FF
#define SPD_DDR4_TWTRS_LSB___S                            0x0

#define SPD_DDR4_TWTRL_MSB_MSN___M                        0x0000000F
#define SPD_DDR4_TWTRL_MSB_MSN___S                        0x4
#define SPD_DDR4_TWTRL_LSB___M                            0x000000FF
#define SPD_DDR4_TWTRL_LSB___S                            0x0

#define SPD_DDR4_RANK_MIX___M                             0x00000001
#define SPD_DDR4_RANK_MIX___S                             0x6

#define SPD_DDR4_LR_DB_DQ_DFE___M                         0x00000001
#define SPD_DDR4_LR_DB_DQ_DFE___S                         0x0
#define SPD_DDR4_LR_DB_DQ_GAIN_ADJ___S                    0x1

#define SPD_DDR4_LR_DB_MDQ_RON_RTT___M                    0x00000007
#define SPD_DDR4_LR_DB_MDQ_TERM___S                       0x0
#define SPD_DDR4_LR_DB_MDQ_RON___S                        0x4

#define SPD_DDR4_LR_DB_MDQ_VREFDQ___M                     0x0000003F
#define SPD_DDR4_LR_DB_MDQ_VREFDQ___S                     0x0

#define SPD_DDR4_LR_DB_VREFDQ_RANGE___M                   0x00000001
#define SPD_DDR4_LR_DB_MDQ_VREFDQ_RANGE___S               0x4

#define SPD_DDR4_LR_DB_VREFDQ___M                         0x000000FF

#define SPD_DDR4_LR_DB_RTT_PARK_RANK_0_1                  0x0
#define SPD_DDR4_LR_DB_RTT_PARK_RANK_2_3                  0x1
#define SPD_DDR4_LR_DB_RTT_PARK___M                       0x00000007
#define SPD_DDR4_LR_DB_RTT_PARK01___S                     0x0
#define SPD_DDR4_LR_DB_RTT_PARK23___S                     0x3

#define SPD_DDR4_LR_DB_RTT_WR_NOM___M                     0x00000007
#define SPD_DDR4_LR_DB_RTT_NOM___S                        0x0
#define SPD_DDR4_LR_DB_RTT_WR___S                         0x3

#define SPD_DDR4_LR_DB_RON___M                            0x00000003
#define SPD_DDR4_LR_DB_RON_1866___S                       0x0
#define SPD_DDR4_LR_DB_RON_2400___S                       0x2
#define SPD_DDR4_LR_DB_RON_3200___S                       0x4

#endif
