/*!****************************************************************************
* @file ddr_driver.h
*
 * SMBus wrapper functions for supporting a single instance DDR driver.
*
*
*
******************************************************************************/
#ifndef _DDR_DRIVER_H_
#define _DDR_DRIVER_H_
/*==========================================================================
                               INCLUDE FILES
===========================================================================*/
#include "mddr.h"
#include "spd.h"

#define SPD_BLSP1_I2C0                          0
#define SPD_BLSP1_I2C1                          1

#define SPD_EEPROM_SMBUS_HDDRSS0_SLOT0_ADDR     0xA8
#define SPD_EEPROM_SMBUS_HDDRSS0_SLOT1_ADDR     0xAA
#define SPD_EEPROM_SMBUS_HDDRSS1_SLOT0_ADDR     0xA4
#define SPD_EEPROM_SMBUS_HDDRSS1_SLOT1_ADDR     0xA6
#define SPD_EEPROM_SMBUS_HDDRSS2_SLOT0_ADDR     0xA4

#define SPD_TS_SMBUS_HDDRSS0_SLOT0_ADDR         0x38
#define SPD_TS_SMBUS_HDDRSS0_SLOT1_ADDR         0x3A
#define SPD_TS_SMBUS_HDDRSS1_SLOT0_ADDR         0x34
#define SPD_TS_SMBUS_HDDRSS1_SLOT1_ADDR         0x36

#define SPD_CMD_SMBUS_HDDRSS0_SLOT0_ADDR         0x68
#define SPD_CMD_SMBUS_HDDRSS0_SLOT1_ADDR         0x6A
#define SPD_CMD_SMBUS_HDDRSS1_SLOT0_ADDR         0x64
#define SPD_CMD_SMBUS_HDDRSS1_SLOT1_ADDR         0x66

#define RCD_SMBUS_HDDRSS0_SLOT0_ADDR         0xB8
#define RCD_SMBUS_HDDRSS0_SLOT1_ADDR         0xBA
#define RCD_SMBUS_HDDRSS1_SLOT0_ADDR         0xB4
#define RCD_SMBUS_HDDRSS1_SLOT1_ADDR         0xB6


#define read8(addr) (*((volatile uint8_t *) (addr)))
#define write8(addr, val) (*((volatile uint8_t *) (addr)) = ((uint8_t) (val)))
#define read16(addr) (*((volatile uint16_t *) (addr)))
#define write16(addr, val) (*((volatile uint16_t *) (addr)) = ((uint16_t) (val)))
#define read32(addr) (*((volatile uint32_t *) (addr)))
#define write32(addr, val) (*((volatile uint32_t *) (addr)) = ((uint32_t) (val)))
#define read64(addr) (*((volatile uint64_t *) (addr)))
#define write64(addr, val) (*((volatile uint64_t *) (addr)) = ((uint64_t) (val)))

/*!****************************************************************************
*
* @defgroup common_status_vals Common Status Values
*
* @par Description
*    Common status indicator values used to a common good bad indication.
*    Generally used to indicate this success / failure for various
*    functions and operations.
*
* @{
******************************************************************************/
#define COM_OK                  0x00
#define COM_ERROR               0xFF


typedef struct
{
    /* SMBUS functions */
    int (*read_data_byte) (uint32_t smbus_id, uint32_t smb_address, uint16_t read_offset, uint8_t* out_data);
    int (*read_data_word) (uint32_t smbus_id, uint32_t smb_address, uint16_t read_offset, uint16_t* out_data);
    int (*block_read) (uint32_t smbus_id, uint32_t smb_address, uint16_t read_offset, void* out_buffer, size_t buffer_size);
    int (*write_data_byte) (uint32_t smbus_id, uint32_t smb_address, uint16_t write_offset, uint8_t value);
    int (*write_data_word) (uint32_t smbus_id, uint32_t smb_address, uint16_t write_offset, uint16_t value);
    int (*block_write) (uint32_t smbus_id, uint32_t smb_address, uint16_t write_offset, void* in_buffer, size_t buffer_size);
} smbus_wrapper;
typedef struct
{
    smbus_wrapper smbus;
} ddr_func_tbl;

/*!****************************************************************************
*
* @par Description
*    Log level used to clasify severity/priority of log messages. Used to
*     limit which mesages are logged/displayed.
*
* @par Members
*                                  <PRE>
*    INFORMATIONAL  All priority of messages should be displayed.
*    LOW            Only low priority messages and above should be displayed.
*    MEDIUM         Only medium priority messages and above should be displayed.
*    HIGH           Only high priority messages and above should be displayed.
*    CRITICAL       Only critical messages should be displayed. Normal release mode.
*    NO_LOGS        No logging should occur even for critical items. Debug only.
*                                  </PRE>
******************************************************************************/
typedef enum _importance_t
{
    INFORMATIONAL = 0,
    LOW = 1,
    MEDIUM = 2,
    HIGH = 3,
    CRITICAL = 4,
    NO_LOGS = 5
} importance_t;


typedef struct
{
    mddr_settings_t mddr_settings;              //!< Indicates the configurable DDR settings to use.
    target_mode_e mode;                         //!< Allows adaptation based on the type of execution target.
    importance_t log_level;                     //!< Indication of the level of reporting of initialization details.
    dimm_slot_smbus_info_t spd_smbus_info[DDR_MAX_NUM_CHANS][DDR_MAX_NUM_SLOTS_PER_CHAN];   //!< Provides the map of platform defined SMBus addresses to the specific DIMMs based on chan and slot.
} ddr_driver_init_t;

int mddr_log(int importance, const char * fmt, ...);
#endif
