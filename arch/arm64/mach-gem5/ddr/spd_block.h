/*!****************************************************************************
* Filename: spd_block.h
*
* Description: Provides the offsets / structure of an SPD block as defined
*               for DDR4 DIMMs. Also provides the constants to
*               make use of particular sub-fields of within SPD data.
*
******************************************************************************/
#ifndef _SPD_BLOCK_H
#define _SPD_BLOCK_H

/******************************************************************************
* Included Headers
******************************************************************************/
#include "spd_ddr4_block.h"    // load DDR4 offsets/values


/******************************************************************************
* Defined Constants and Macros
******************************************************************************/

//
// SPD EEPROM Field Byte Offsets/IDs Common to DDR4
//
#define SPD_OFFSET_SPD_DEVICE_INFO              0x00 // Number of Serial PD Bytes Written / SPD Device Size / CRC Coverage
#define SPD_OFFSET_SPD_REVISION                 0x01
#define SPD_OFFSET_DRAM_DEVICE_TYPE             0x02
#define SPD_OFFSET_DRAM_MODULE_TYPE             0x03
#define SPD_OFFSET_SDRAM_DENS_N_BANKS           0x04
#define SPD_OFFSET_SDRAM_ADDRESSING             0x05

// EEPROM Field / subfield access constants.
//
// Note: Some fields are stored as a power of 2 and are scaled to save bits.
//  I.e. Eliminating values that are smaller than the minimum value.
//

#define SPD_BYTES_TOTAL___M                 0x00000007
#define SPD_BYTES_TOTAL___S                 0x4
#define SPD_BYTES_USED___M                  0x00000007
#define SPD_BYTES_USED___S                  0x0

#define SPD_DEVICE_CAPACITY___M             0x0000000F
#define SPD_DEVICE_CAPACITY___S             0x0
#define SPD_DEVICE_CAPACITY_SCALE_OFFSET    0x5 // Scale : 0x20  2 to the 5th

#define SPD_BANK_GROUP___M                  0x00000007
#define SPD_BANK_GROUP___S                  0x6

#define SPD_DEVICE_WIDTH___M                0x00000007
#define SPD_DEVICE_WIDTH___S                0x0
#define SPD_DEVICE_WIDTH_SCALE_OFFSET       0x2 // Scale : 0x04  2 to the 2nd

#define SPD_NUM_RANK___M                    0x00000007
#define SPD_NUM_RANK___S                    0x3
#define SPD_NUM_RANKS_SCALE_OFFSET          0x1 // Scale : 0x02  2 to the 1st

#define SPD_MOD_WIDTH___M                   0x00000007
#define SPD_MOD_WIDTH___S                   0x0
#define SPD_MOD_WIDTH_SCALE_OFFSET          0x3 // Scale : 0x08  2 to the 3rd

#define SPD_MOD_ECC___M                     0x00000001
#define SPD_MOD_ECC___S                     0x3

#define SPD_ROWS___M                        0x00000007
#define SPD_ROWS___S                        0x3
#define SPD_ROWS_SCALE_OFFSET               0xC

#define SPD_COLS___M                        0x00000007
#define SPD_COLS___S                        0x0
#define SPD_COLS_SCALE_OFFSET               0x9

#define SPD_CA_MIRROR___M                   0x00000001
#define SPD_CA_MIRROR___S                   0x0
#define SPD_CA_MIRRORED                     0x1

#define SPD_TS___M                          0x00000001
#define SPD_TS___S                          0x7


//
// SPD Bytes total encodings in Serial EEPROM.
//
#define SPD_BYTES_TOTAL_UNDEFINED           0x00
#define SPD_BYTES_TOTAL_256                 0x01
#define SPD_BYTES_TOTAL_512                 0x02


//
// SPD TS Register IDs. All Thermal Sensor is common between DDR4
//
#define SPD_TS_CAPABILITIES                 0x00
#define SPD_TS_CONFIGURATION                0x01
#define SPD_TS_HIGH_LIMIT                   0x02
#define SPD_TS_LOW_LIMIT                    0x03
#define SPD_TS_TCRIT_LIMIT                  0x04
#define SPD_TS_AMBIENT_TEMPERATURE          0x05
#define SPD_TS_MANUFACTURER ID              0x06
#define SPD_TS_DEVICE_REVISION              0x07


//
// TS Capability register Field / subfield access constants.
//
#define SPD_TS_TEMP_RESOLUTION___M          0x3
#define SPD_TS_TEMP_RESOLUTION___S          3
#define SPD_TS_TEMP_RANGE___M               0x1
#define SPD_TS_TEMP_RANGE___S               2


//
// TS Configuration register Field / subfield access constants.
//
#define SPD_TS_CONFIGURATION_HYST___M       0x3
#define SPD_TS_CONFIGURATION_HYST___S       9
#define SPD_TS_CONFIGURATION_SHDN___M       0x1
#define SPD_TS_CONFIGURATION_SHDN___S       8
#define SPD_TS_CONFIGURATION_TCRIT_LOCK___M 0x1
#define SPD_TS_CONFIGURATION_TCRIT_LOCK___S 7
#define SPD_TS_CONFIGURATION_EVENT_LOCK___M 0x1
#define SPD_TS_CONFIGURATION_EVENT_LOCK___S 6
#define SPD_TS_CONFIGURATION_CLEAR___M      0x1
#define SPD_TS_CONFIGURATION_CLEAR___S      5
#define SPD_TS_CONFIGURATION_EVENT_STS___M  0x1
#define SPD_TS_CONFIGURATION_EVENT_STS___S  4
#define SPD_TS_CONFIGURATION_EVENT_CTRL___M 0x1
#define SPD_TS_CONFIGURATION_EVENT_CTRL___S 3
#define SPD_TS_CONFIGURATION_TCRIT_ONLY___M 0x1
#define SPD_TS_CONFIGURATION_TCRIT_ONLY___S 2
#define SPD_TS_CONFIGURATION_EVENT_POL___M  0x1
#define SPD_TS_CONFIGURATION_EVENT_POL___S  1
#define SPD_TS_CONFIGURATION_EVENT_MODE___M 0x1
#define SPD_TS_CONFIGURATION_EVENT_MODE___S 0


//
// TS TCRIT Limit register Field / subfield access constants.
//
#define SPD_TS_TCRIT_LIMIT_SIGN___M         0x1
#define SPD_TS_TCRIT_LIMIT_SIGN___S         12
#define SPD_TS_TCRIT_LIMIT_TEMP_WHOLE___M   0xFF
#define SPD_TS_TCRIT_LIMIT_TEMP_WHOLE___S   4
#define SPD_TS_TCRIT_LIMIT_TEMP_FRACT___M   0x3
#define SPD_TS_TCRIT_LIMIT_TEMP_FRACT___S   2


//
// TS Ambient Temperature register Field / subfield access constants.
//
#define SPD_TS_TCRIT___M                    0x00000001
#define SPD_TS_TCRIT___S                    15
#define SPD_TS_LOW___M                      0x00000001
#define SPD_TS_LOW___S                      14
#define SPD_TS_HIGH___M                     0x00000001
#define SPD_TS_HIGH___S                     13
#define SPD_TS_TEMP_SIGN___M                0x00000001
#define SPD_TS_TEMP_SIGN___S                12
#define SPD_TS_TEMP___M                     0x00001FFF
#define SPD_TS_TEMP___S                     0


#endif
