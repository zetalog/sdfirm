/*!****************************************************************************
* @file mddr_info.c
*
* @par Description:
* this module is used to get the system configuration information for the DDR
*
******************************************************************************/
/******************************************************************************
* Included Headers
******************************************************************************/
#include "mddr.h"
#include "mddr_init.h"
#include "mddr_defs.h"
#include "ddr_driver.h"

/******************************************************************************
* Defined Constants
******************************************************************************/


/******************************************************************************
* Derived data types
******************************************************************************/



/******************************************************************************
* Global Variables
******************************************************************************/

extern mddr_settings_t mddr_settings;
extern spd_dimm_config_t mddr_dimm_configs[DDR_MAX_NUM_CHANS][DDR_MAX_NUM_SLOTS_PER_CHAN];


/******************************************************************************
* Macros
******************************************************************************/



/******************************************************************************
* Function Prototypes
******************************************************************************/

void mddr_copy_dimm_spd_info(spd_dimm_config_t *src_spd_info_ptr, spd_dimm_config_t *dst_spd_info_ptr);
void mddr_copy_init_setting(mddr_settings_t *settings_ptr);
void mddr_get_cfg_info(mddr_cfg_t *mddr_cfg_ptr);
void mddr_get_chan_cfg_info(uint8_t chan_num, mddr_chan_cfg_t *mddr_chan_cfg_ptr);
void mddr_get_spd_info(mddr_spd_info_t *spd_info_ptr);
void mddr_get_dimm_spd_info(uint8_t chan_num, uint8_t slot_num, spd_dimm_config_t *spd_info_ptr);
void mddr_get_therm_info(mddr_dimm_therm_t *dimm_therm_ptr);
int16_t mddr_get_dimm_temp(uint8_t chan_num, uint8_t slot_num);
bool mddr_get_dimm_eventn_asserted(uint8_t chan_num, uint8_t slot_num);
void get_ddr_state(mddr_state_info_t *mddr_state_info_ptr);

/******************************************************************************
* External Functions
******************************************************************************/

/*!****************************************************************************
*
* @par Description:
*    The mddr_get_cfg_info() function is used to get the configuration
*     information for all DDR channels. The only parameter is a pointer to a
*     mddr_cfg_t structure. The structure, provided by the caller, will be
*     populated by the function. A channel that does not have any DIMMs will
*     have a count of 0 in the field num_slots_populated found in the channels
*     specific entry in the chan_cfg[] array.
*
*                                  @param
*    mddr_cfg_ptr        Pointer to the MDDR configuration information
*                         structure populated by this function.
*
* @return
*    None
*
* @par Notes:
*    The call should be made only after the driver has been initialized
*    successfully.
*
******************************************************************************/
void mddr_get_cfg_info(mddr_cfg_t *mddr_cfg_ptr)
{
    uint8_t chan_num;
    uint8_t num_chans_active = 0;
    uint8_t chan_map = 0x0;


    for(chan_num = 0; chan_num < DDR_MAX_NUM_CHANS; ++chan_num)
    {
        mddr_get_chan_cfg_info(chan_num, &(mddr_cfg_ptr->chan_cfg[chan_num]));

        if(mddr_cfg_ptr->chan_cfg[chan_num].num_slots_populated != 0)
        {
            ++num_chans_active;
            chan_map |= 0x1 << chan_num;
        }
    }

    mddr_cfg_ptr->num_chans_supported = DDR_MAX_NUM_CHANS;
    mddr_cfg_ptr->chan_map = chan_map;
    mddr_cfg_ptr->num_chans_active = num_chans_active;
    mddr_copy_init_setting(&(mddr_cfg_ptr->mddr_settings));
}


/*!****************************************************************************
*
* @par Description:
*    This function is used to get the configuration information for a specific
*     DDR channel. The parameters are the number of the channel and a pointer
*     to a mddr_chan_cfg_t structure. The structure, provided by the caller,
*     will be populated by the function. A channel that does not have any DIMMs
*     will have a count of 0 in the field num_slots_populated.
*
*                                  @param
*    chan_num            The DDR channel to inspect.
*                                  @param
*    mddr_chan_cfg_ptr   Pointer to the channel configuration information
*                         structure populated by this function.
*
* @return
*    None
*
* @par Notes:
*    The call should be made only after the driver has been initialized
*    successfully.
*
******************************************************************************/
void mddr_get_chan_cfg_info(uint8_t chan_num, mddr_chan_cfg_t *mddr_chan_cfg_ptr)
{
    uint8_t slot_num;
    uint8_t num_slots_populated = 0;
    uint8_t slot_map = 0x0;
    uint32_t ddr_size_mb = 0;


    for(slot_num = 0; slot_num < DDR_MAX_NUM_SLOTS_PER_CHAN; ++slot_num)
    {
        if(mddr_dimm_configs[chan_num][slot_num].dev_type != SPD_DEVICE_TYPE_UNDEFINED)
        {
            ddr_size_mb += mddr_dimm_configs[chan_num][slot_num].dimm_size_mb;
            ++num_slots_populated;
            slot_map |= 0x1 << slot_num;
        }
    }

    mddr_chan_cfg_ptr->num_slots_supported = mddr_settings.num_dimm_slots;
    mddr_chan_cfg_ptr->num_slots_populated = num_slots_populated;
    mddr_chan_cfg_ptr->slot_map = slot_map;
    mddr_chan_cfg_ptr->mem_size_gb = ddr_size_mb / MB_PER_GB;
}



/*!****************************************************************************
*
* @par Description:
*    This function gets the translated SPD information for all populated
*     DIMMs.
*
*                                  @param
*    spd_info_ptr        Pointer to the SPD information structure populated
*                         by this function.
*
* @return
*    None
*
* @par Notes:
*    The call should be made only after the driver has been initialized
*    successfully.
*
******************************************************************************/
void mddr_get_spd_info(mddr_spd_info_t *spd_info_ptr)
{
    uint8_t chan_num;
    uint8_t slot_num;

    for(chan_num = 0; chan_num < DDR_MAX_NUM_CHANS; ++chan_num)
    {
        for(slot_num = 0; slot_num < DDR_MAX_NUM_SLOTS_PER_CHAN; ++slot_num)
        {
            mddr_copy_dimm_spd_info(&(mddr_dimm_configs[chan_num][slot_num]),
                                    &(spd_info_ptr->dimm_spd[chan_num][slot_num]));
        }
    }
}



/*!****************************************************************************
*
* @par Description:
*    This function gets the translated SPD information for the indicated
*     DIMM.
*
*                                  @param
*    chan_num           The DDR channel number of the DIMM.
*                                  @param
*    slot_num           The slot number in the DDR channel of the DIMM.
*                                  @param
*    spd_info_ptr       Pointer to the DIMM SPD configuration structure
*                         populated by this function.
*
* @return
*    None
*
* @par Notes:
*    The call should be made only after the driver has been initialized
*    successfully.
*
******************************************************************************/
void mddr_get_dimm_spd_info
(
    uint8_t chan_num,
    uint8_t slot_num,
    spd_dimm_config_t *spd_info_ptr
)
{
    mddr_copy_dimm_spd_info(&(mddr_dimm_configs[chan_num][slot_num]), spd_info_ptr);
}



/*!****************************************************************************
*
* @par Description:
*    This function collects the DIMM temperatures from all populated DIMMs.
*
*                                  @param
*    dimm_therm_ptr     Pointer to the DIMM thermal information structure
*                        populated by this function.
*
* @return
*    None
*
*
* @par Notes:
*    The DIMM temps for slots that are unpopulated, or which lack a temperature
*    sensor, will be MDDR_DIMM_TEMP_INVALID and the DIMM event asserted status
*    will be false.
*
*    The call should be made only after the driver has been initialized
*    successfully.
*
******************************************************************************/
void mddr_get_therm_info(mddr_dimm_therm_t *dimm_therm_ptr)
{
    uint8_t chan_num;
    uint8_t slot_num;

    for(chan_num = 0; chan_num < DDR_MAX_NUM_CHANS; ++chan_num)
    {
        for(slot_num = 0; slot_num < DDR_MAX_NUM_SLOTS_PER_CHAN; ++slot_num)
        {
            dimm_therm_ptr->dimm_temps[chan_num][slot_num] = mddr_get_dimm_temp(chan_num, slot_num);
            dimm_therm_ptr->dimm_eventn_asserted[chan_num][slot_num] = mddr_get_dimm_eventn_asserted(chan_num, slot_num);
        }
    }
}



/*!****************************************************************************
*
* @par Description:
*    This function gets the DIMM temperature from the indicated DIMM.
*
*                                  @param
*    chan_num           The DDR channel number of the DIMM.
*                                  @param
*    slot_num           The slot number in the DDR channel of the DIMM.
*
*
* @return
*    The temperature from a specific DIMM in degrees C. DIMM temperatureis a
*     signed 13 bit 2's complement integer value where the low order 4 bits are
*     the fractional degree componentas defined by JEDEC Standard No. 21-C
*     Release 24, Page 4.1.6 – 23 section 2.22. Where bit-0 => 0.0625;
*     bit-1=> 0.125; bit-2=> 0.25; bit-3> 0.5. The value will be
*     MDDR_DIMM_TEMP_INVALID for slots that are unpopulated or whose
*     DIMM lacks a thermal sensor.
*
* @par Notes:
*    The call should be made only after the driver has been initialized
*    successfully.
*
******************************************************************************/
int16_t mddr_get_dimm_temp(uint8_t chan_num, uint8_t slot_num)
{
    int16_t dimm_temp = MDDR_DIMM_TEMP_INVALID;

    // Read the temp if the DIMM has a thermal sensor.
    if((mddr_dimm_configs[chan_num][slot_num].dev_type != SPD_DEVICE_TYPE_UNDEFINED) &&
       (mddr_dimm_configs[chan_num][slot_num].has_therm_sensor))
    {
        dimm_temp = spd_get_dimm_temp(chan_num, slot_num);
    }

    return dimm_temp;
}



/*!****************************************************************************
*
* @par Description:
*    This function gets the DIMM EVENT_n signal from the indicated DIMM.
*
*                                  @param
*    chan_num           The DDR channel number of the DIMM.
*                                  @param
*    slot_num           The slot number in the DDR channel of the DIMM.
*
*
* @return
*    The status from the DIMM Temperature Sensor (TS) indicating whether the
*     EVENT_n signal is asserted.  Use of this signal is described by JEDEC
*     Standard No. 21-C Release 26, Page 4.1.6 – 16 section 2.17.  The value
*     will be false for slots that are unpopulated or whose DIMM lacks a thermal
*     sensor.
*
* @par Notes:
*    The call should be made only after the driver has been initialized
*    successfully.
*
******************************************************************************/
bool mddr_get_dimm_eventn_asserted(uint8_t chan_num, uint8_t slot_num)
{
    bool eventn_status = false;

    // Read the event_n status if the DIMM has a thermal sensor.
    if((mddr_dimm_configs[chan_num][slot_num].dev_type != SPD_DEVICE_TYPE_UNDEFINED) &&
       (mddr_dimm_configs[chan_num][slot_num].has_therm_sensor))
    {
        eventn_status = spd_get_dimm_eventn_asserted(chan_num, slot_num);
    }

    return eventn_status;
}




/*!****************************************************************************
*
* @par Description:
*    This function collects DDR sub-system state information, including
*     Power-Down and Self-Refresh.
*
*                                  @param
*    mddr_state_info_ptr  Pointer to the DDR State Information structure
*                          populated by this function.  The array is populated
*                          based on physical channel number.
*
* @return
*    None
*
* @par Notes:
*    The call should be made only after the driver has been initialized
*    successfully.
*
******************************************************************************/
void get_ddr_state(mddr_state_info_t *mddr_state_info_ptr)
{
}


/******************************************************************************
* Local Functions
******************************************************************************/
/*!****************************************************************************
*
* @par Description:
*    This function does a byte wise block copy.
*
*                                  @param
*    src_ptr        Pointer to the source data to copy.
*                                  @param
*    dst_ptr        Pointer to the destination to receive the copied data.
*                                  @param
*    size_bytes     Size of the block to copy in bytes.
*
*
* @return
*    COM_OK if the copy was successful, COM_ERROR otherwise.
*
******************************************************************************/
unsigned com_blk_copy(uint8_t *src_ptr, uint8_t *dst_ptr, uint16_t size_bytes)
{
    unsigned status = COM_ERROR;
    uint16_t byte_num;


    if((src_ptr != NULL) && (dst_ptr != NULL))
    {
        status = COM_OK;

        for(byte_num = 0; byte_num < size_bytes; ++byte_num)
        {
            *dst_ptr = *src_ptr;
            ++dst_ptr;
            ++src_ptr;
        }
    }

    return status;
}


/*!****************************************************************************
*
* @par Description:
*    This function copies a DIMM SPD information structure, spd_dimm_config_t,
*     to another DIMM SPD information structure. Used to get a copy of internal
*     data and make it available to higher level / application software.
*
*                                  @param
*    src_spd_info_ptr     Pointer to DIMM SPD information structure that is
*                          the source of the copy.
*                                  @param
*    dst_spd_info_ptr     Pointer to DIMM SPD information structure that is
*                          the destination of the copy.
*
* @return
*    None
*
******************************************************************************/
void mddr_copy_dimm_spd_info(spd_dimm_config_t *src_spd_info_ptr, spd_dimm_config_t *dst_spd_info_ptr)
{
    uint16_t size_bytes;
    uint8_t *src_ptr;
    uint8_t *dst_ptr;

    size_bytes = sizeof(spd_dimm_config_t);
    src_ptr = (uint8_t *)src_spd_info_ptr;
    dst_ptr = (uint8_t *)dst_spd_info_ptr;

    com_blk_copy(src_ptr, dst_ptr, size_bytes);
}



/*!****************************************************************************
*
* @par Description:
*    This function copies the contents of the internal MDDR settings to the
*     provided MDDR settings structure.
*
*                                  @param
*    settings_ptr         Pointer to MDDR settings structure that is
*                          the destination of the copy.
*
* @return
*    None
*
*
******************************************************************************/
void mddr_copy_init_setting(mddr_settings_t *settings_ptr)
{
    uint16_t size_bytes;
    uint8_t *src_ptr;
    uint8_t *dst_ptr;

    size_bytes = sizeof(mddr_settings_t);
    src_ptr = (uint8_t *)&mddr_settings;
    dst_ptr = (uint8_t *)settings_ptr;

    com_blk_copy(src_ptr, dst_ptr, size_bytes);
}
