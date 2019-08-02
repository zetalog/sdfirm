/*!****************************************************************************
* @file ddr_driver.c
*
 * SMBus wrapper functions for supporting a single instance DDR driver.
*
*
*
******************************************************************************/
/******************************************************************************
* Included Headers
******************************************************************************/
#include <stdio.h>
#include <stdint.h>
#include  "mddr_init.h"
#include  "spd_smbus.h"
#include "ddr_driver.h"

/******************************************************************************
* Defined Constants
******************************************************************************/
#define print_level 4

/******************************************************************************
* Derived data types
******************************************************************************/

/******************************************************************************
* Global Variables
******************************************************************************/
static dimm_slot_smbus_info_t smbus_default_val[DDR_MAX_NUM_CHANS][DDR_MAX_NUM_SLOTS_PER_CHAN]=
{
    {
        {SPD_BLSP1_I2C0,SPD_EEPROM_SMBUS_HDDRSS0_SLOT0_ADDR,SPD_TS_SMBUS_HDDRSS0_SLOT0_ADDR,SPD_CMD_SMBUS_HDDRSS0_SLOT0_ADDR,RCD_SMBUS_HDDRSS0_SLOT0_ADDR},
        {SPD_BLSP1_I2C0,SPD_EEPROM_SMBUS_HDDRSS0_SLOT1_ADDR,SPD_TS_SMBUS_HDDRSS0_SLOT1_ADDR,SPD_CMD_SMBUS_HDDRSS0_SLOT1_ADDR,RCD_SMBUS_HDDRSS0_SLOT1_ADDR}
    },
    {
        {SPD_BLSP1_I2C0,SPD_EEPROM_SMBUS_HDDRSS1_SLOT0_ADDR,SPD_TS_SMBUS_HDDRSS1_SLOT0_ADDR,SPD_CMD_SMBUS_HDDRSS1_SLOT0_ADDR,RCD_SMBUS_HDDRSS1_SLOT0_ADDR},
        {SPD_BLSP1_I2C0,SPD_EEPROM_SMBUS_HDDRSS1_SLOT1_ADDR,SPD_TS_SMBUS_HDDRSS1_SLOT1_ADDR,SPD_CMD_SMBUS_HDDRSS1_SLOT1_ADDR,RCD_SMBUS_HDDRSS1_SLOT1_ADDR}
    }
};

ddr_func_tbl ddr_function_table;
ddr_driver_init_t   ddr_driver_init;

/******************************************************************************
* Macros
******************************************************************************/

/******************************************************************************
* Function Prototypes
******************************************************************************/
int smbus_write_data_byte(uint32_t smbus_id, uint32_t smb_address, uint16_t write_offset, uint8_t value);
int smbus_write_data_word(uint32_t smbus_id, uint32_t smb_address, uint16_t write_offset, uint16_t value);
int smbus_read_generic(uint32_t smbus_id, uint32_t smb_addrss, uint16_t write_offset, void* in_buffer, size_t buffer_size);
void smbus_configuration(dimm_slot_smbus_info_t *smbus_info_p);

/******************************************************************************
* External Functions
******************************************************************************/
extern unsigned mddr_init(target_mode_e mode,  mddr_settings_t *mddr_settings_ptr);

/*===========================================================================
                      FUNCTION DECLARATIONS
===========================================================================*/

/*!****************************************************************************
*
* @par Description:
*    This function gets the DIMM serial EEPROM size from the indicated DIMM.
*    Size of the serial EEPROM in bytes.
*
*
******************************************************************************/
int ddr_driver_init_test(void)
{
    memset(&ddr_driver_init, 0, sizeof(ddr_driver_init_t));
    ddr_driver_init.log_level = INFORMATIONAL;

    ddr_function_table.smbus.write_data_byte = &smbus_write_data_byte;
    ddr_function_table.smbus.write_data_word = &smbus_write_data_word;
    ddr_function_table.smbus.block_read = &smbus_read_generic;

    smbus_configuration((dimm_slot_smbus_info_t*)ddr_driver_init.spd_smbus_info);

    ddr_driver_init.mode=SILICON;


    spd_init(ddr_driver_init.mode, ddr_driver_init.spd_smbus_info);


     switch (ddr_driver_init.mode)
    {
        case SILICON:
             // If the user hasn't specified a clock speed, find an appropriate default for the platform
            if(ddr_driver_init.mddr_settings.mem_freq_mhz[DDR_FREQ_1] ==0)
            {
                ddr_driver_init.mddr_settings.mem_freq_mhz[DDR_FREQ_1] = DDR_MAX_SPEED_RECOVERY;
            }
            if( ddr_driver_init.mddr_settings.max_1dimm_freq_mhz ==0)
            {
                ddr_driver_init.mddr_settings.max_1dimm_freq_mhz = DDR_MAX_SPEED_1DPC;
            }
            if( ddr_driver_init.mddr_settings.max_2dimm_freq_mhz ==0)
            {
                ddr_driver_init.mddr_settings.max_2dimm_freq_mhz = DDR_MAX_SPEED_2DPC;
            }
            break;

        case PALLADIUM_Z1:
            ddr_driver_init.mddr_settings.mem_freq_mhz[DDR_FREQ_1] = DDR_MAX_SPEED_EMU;
            ddr_driver_init.mddr_settings.mddr_xo_freq_khz = 332;
            //ddr_driver_init.mddr_settings.disable_ecc = false;
            ddr_driver_init.mddr_settings.disable_ecc = true;
            ddr_driver_init.mddr_settings.prop_delay = 0x1F;
            break;

         case CHIPSIM:
            ddr_driver_init.mddr_settings.mem_freq_mhz[DDR_FREQ_1] = DDR_MAX_SPEED_EMU;
            ddr_driver_init.mddr_settings.mddr_xo_freq_khz = 332;
            //ddr_driver_init.mddr_settings.disable_ecc = false;
            ddr_driver_init.mddr_settings.disable_ecc = true;
            ddr_driver_init.mddr_settings.prop_delay = 0x1F;
            break;

        default:
            ddr_driver_init.mddr_settings.mem_freq_mhz[DDR_FREQ_1] = DDR_MAX_SPEED_EMU;
            ddr_driver_init.mddr_settings.mddr_xo_freq_khz = 332;
            ddr_driver_init.mddr_settings.disable_ecc = false;
            ddr_driver_init.mddr_settings.prop_delay = 0x1F;

            break;
    }

    if (mddr_init(ddr_driver_init.mode, &(ddr_driver_init.mddr_settings)) != COM_OK)
    {
        mddr_log(CRITICAL, "  DDR Driver Fail to Init\n");
        return COM_ERROR;
    }

    mddr_log(LOW,      " =============================================\n");
    mddr_log(CRITICAL, "  DDR Driver Init Complete\n");
    mddr_log(LOW,      " =============================================\n");

}

/* ============================================================================
**  Function : ddr_smbus_read_data_byte
** ===========================================================================*/
int ddr_smbus_read_data_byte(uint32_t smbus_id, uint32_t smb_address, uint16_t read_offset, uint8_t* out_data)
{
    if(ddr_function_table.smbus.read_data_byte != NULL)
    {
        return ddr_function_table.smbus.read_data_byte(smbus_id, smb_address>>1, read_offset, out_data);
    }
    else
    {
        return COM_ERROR;
    }
}

/* ============================================================================
**  Function : ddr_smbus_read_data_word
** ===========================================================================*/
int ddr_smbus_read_data_word(uint32_t smbus_id, uint32_t smb_address, uint16_t read_offset, uint16_t* out_data)
{
    int status = 0;
    uint16_t output_v;

    if(ddr_function_table.smbus.read_data_word != NULL)
    {
        status=ddr_function_table.smbus.read_data_word(smbus_id, smb_address>>1, read_offset, &output_v);
        // Byte swapping since I2C driver returns read data as n byte streaming
        *out_data = ((output_v&0xFF00)>>8) | ((output_v&0xFF) <<8);

        return status;
    }
    else
    {
        return COM_ERROR;
    }
}

/* ============================================================================
**  Function : ddr_smbus_block_read
** ===========================================================================*/
int ddr_smbus_block_read(uint32_t smbus_id, uint32_t smb_address, uint8_t spd_bank_num, uint16_t spd_reg_id, void* out_buffer, uint32_t buffer_size)
{
    uint16_t read_offset;

    if(ddr_function_table.smbus.block_read != NULL)
    {
        read_offset = (uint16_t)spd_bank_num*SPD_PAGE_SIZE + spd_reg_id;
        return ddr_function_table.smbus.block_read(smbus_id, smb_address>>1, read_offset, out_buffer, buffer_size);
    }
    else
    {
        return COM_ERROR;
    }
}

/* ============================================================================
**  Function : ddr_smbus_write_data_byte
** ===========================================================================*/
int ddr_smbus_write_data_byte(uint32_t smbus_id, uint32_t smb_address, uint16_t write_offset, uint8_t value)
{
    if(ddr_function_table.smbus.write_data_byte != NULL)
    {
        return ddr_function_table.smbus.write_data_byte(smbus_id, smb_address>>1, write_offset, value);
    }
    else
    {
        return COM_ERROR;
    }
}

/* ============================================================================
**  Function : ddr_smbus_write_data_word
** ===========================================================================*/
int ddr_smbus_write_data_word(uint32_t smbus_id, uint32_t smb_address, uint16_t write_offset, uint16_t value)
{
    uint16_t output_v;

    if(ddr_function_table.smbus.write_data_word != NULL)
    {
        output_v = ((value&0xFF00)>>8) | ((value&0xFF) <<8);
        return ddr_function_table.smbus.write_data_word(smbus_id, smb_address>>1, write_offset, output_v);
    }
    else
    {
        return COM_ERROR;
    }
}

/* ============================================================================
**  Function : ddr_smbus_block_write
** ===========================================================================*/
int ddr_smbus_block_write(uint32_t smbus_id, uint32_t smb_address, uint8_t spd_bank_num, uint16_t spd_reg_id, void* in_buffer, uint32_t buffer_size)
{
    uint16_t write_offset;

    if(ddr_function_table.smbus.block_write != NULL)
    {
        write_offset = (uint16_t)spd_bank_num*SPD_PAGE_SIZE + spd_reg_id;
        return ddr_function_table.smbus.block_write(smbus_id, smb_address>>1, write_offset, in_buffer, buffer_size);
    }
    else
    {
        return COM_ERROR;
    }
}

/* ============================================================================
**  Function : smbus_write_generic
** ===========================================================================*/
int smbus_write_generic(uint32_t smbus_id, uint32_t smb_address, uint16_t write_offset, void* in_buffer, size_t buffer_size)
{

}

/* ============================================================================
**  Function : smbus_write_generic
** ===========================================================================*/
int smbus_read_generic(uint32_t smbus_id, uint32_t smb_address, uint16_t write_offset, void* in_buffer, size_t buffer_size)
{

}

/* ============================================================================
**  Function : smbus_write_data_byte
** ===========================================================================*/
int smbus_write_data_byte(uint32_t smbus_id, uint32_t smb_address, uint16_t write_offset, uint8_t value)
{
    uint8_t val = value;
    return smbus_write_generic(smbus_id, smb_address, write_offset, &val, sizeof(val));
}

/* ============================================================================
**  Function : smbus_write_data_word
** ===========================================================================*/
int smbus_write_data_word(uint32_t smbus_id, uint32_t smb_address, uint16_t write_offset, uint16_t value)
{
    uint16_t val = value;
    return smbus_write_generic(smbus_id, smb_address, write_offset, &val, sizeof(val));
}


void smbus_configuration(dimm_slot_smbus_info_t *smbus_info_p)
{
    dimm_slot_smbus_info_t *hobData;
    hobData = &smbus_default_val[0][0];
    memcpy(smbus_info_p, hobData, sizeof(dimm_slot_smbus_info_t)* DDR_MAX_NUM_CHANS*DDR_MAX_NUM_SLOTS_PER_CHAN );
}

/* ============================================================================
**  Function : mddr_log
** ===========================================================================*/
int mddr_log(int importance, const char * fmt, ...)
{
    va_list vargs;
    int ret_val = 0;

    if(importance >= print_level)
    {
        va_start(vargs, fmt);
        ret_val = vprintf(fmt, vargs);
        va_end(vargs);
    }

    return ret_val;
}
