/*!****************************************************************************
* @file spd.c
*
* @par Description:
*    This module provides the functionality to load and extract the SPD
*    information for a DIMM slot in the Monaco DDR subsystem.
*
* @par Notes:
*    - The provided spd_base address points to a reserved block of memory
*      large enough to hold the SPD information block as defined by
*      spd_block_size. If SPD simulation is used the amount of reserved
*      space must be doubled to contain the raw simulation data.
*    - Relies on the global verbose_mode to drive its level of reporting.
*
******************************************************************************/

/******************************************************************************
* Included Headers
******************************************************************************/
#include  "ddr_driver.h"
#include "mddr.h"
#include "mddr_init.h"
#include "spd.h"
#include  "spd_smbus.h"
#include  "mddr_defs.h"
#include  "spd_simm.h"
#include  "spd_block.h"

/******************************************************************************
* Global Variables
******************************************************************************/

uint32_t medium_time_base_ps = 0;
uint32_t fine_time_base_fs = 0;
bool sign_determined[DDR_MAX_NUM_CHANS] = {false, false};
int16_t dimm_capabilities[DDR_MAX_NUM_CHANS];

/******************************************************************************
* External Variables
******************************************************************************/
extern dimm_slot_smbus_info_t spd_smbus_info[DDR_MAX_NUM_CHANS][DDR_MAX_NUM_SLOTS_PER_CHAN];
extern bool spd_use_simulated_spd ;
extern bool spd_use_simm_in_sil ;
extern mddr_lr_db_si_chan_t lr_db_si_data[DDR_MAX_NUM_CHANS];
extern uint8_t *spd_sim_array_ptr;

/******************************************************************************
* Function Prototypes
******************************************************************************/

uint8_t spd_get_ddr_device_type(uint64_t spd_base);
uint8_t spd_get_ddr_module_type(uint64_t spd_base);
uint8_t spd_get_spd_revision(uint64_t spd_base);
uint8_t spd_get_num_ranks(uint64_t spd_base, uint8_t dev_type);
uint8_t spd_get_sdram_width(uint64_t spd_base, uint8_t dev_type);
bool spd_get_has_ecc(uint64_t spd_base, uint8_t dev_type);
uint8_t spd_get_dimm_module_width(uint64_t spd_base, uint8_t dev_type);
uint8_t spd_get_num_bank_groups(uint64_t spd_base);
uint16_t spd_get_device_capacity(uint64_t spd_base);
uint8_t spd_get_num_rows(uint64_t spd_base);
uint8_t spd_get_num_columns(uint64_t spd_base);
uint8_t spd_get_die_count(uint64_t spd_base, uint8_t dev_type);
bool spd_get_is_3DS(uint64_t spd_base, uint8_t dev_type, uint8_t die_count);
bool spd_get_ca_mirrored(uint64_t spd_base, uint8_t dev_type, uint8_t mod_type);
unsigned spd_calc_dimm_size(uint16_t device_capacity, uint8_t sdram_width,
        uint8_t num_ranks, uint8_t dimm_module_width, bool is_3DS, uint8_t die_count);
bool spd_get_has_therm_sensor(uint64_t spd_base, uint8_t dev_type);
bool spd_get_is_voltage_ok(uint64_t spd_base, uint8_t dev_type);
uint32_t spd_get_ftb_fs(uint64_t spd_base, uint8_t dev_type);
uint32_t spd_get_mtb_ps(uint64_t spd_base, uint8_t dev_type);
uint32_t spd_get_cas_lat_sup(uint64_t spd_base, uint8_t dev_type);
uint32_t spd_get_taa_min_ps(uint64_t spd_base, uint8_t dev_type);
uint32_t spd_get_tck_min_ps(uint64_t spd_base, uint8_t dev_type);
uint32_t spd_get_tck_max_ps(uint64_t spd_base, uint8_t dev_type);

// Functions to Get Timing and Signal Strength Values

// Common
uint32_t spd_get_trcd_min_ps(uint64_t spd_base, uint8_t dev_type);
uint32_t spd_get_trp_min_ps(uint64_t spd_base, uint8_t dev_type);
uint32_t spd_get_tras_min_ps(uint64_t spd_base, uint8_t dev_type);
uint32_t spd_get_trc_min_ps(uint64_t spd_base, uint8_t dev_type);
uint32_t spd_get_tfaw_min_ps(uint64_t spd_base, uint8_t dev_type);
uint32_t spd_get_twr_min_ps(uint64_t spd_base, uint8_t dev_type, uint8_t revision);
uint32_t spd_get_twtr_min_ps(uint64_t spd_base, uint8_t dev_type, uint8_t revision);
uint8_t spd_get_out_drv_ck_y13(uint64_t spd_base, uint8_t dev_type, uint8_t mod_type);
uint8_t spd_get_out_drv_ck_y02(uint64_t spd_base, uint8_t dev_type, uint8_t mod_type);

// DDR4 Only
uint32_t spd_get_trrdl_min_ps(uint64_t spd_base, uint8_t dev_type);
uint32_t spd_get_trrds_min_ps(uint64_t spd_base, uint8_t dev_type);
uint32_t spd_get_tccdl_min_ps(uint64_t spd_base, uint8_t dev_type);
uint32_t spd_get_trfc1_min_ps(uint64_t spd_base, uint8_t dev_type);
uint32_t spd_get_trfc2_min_ps(uint64_t spd_base, uint8_t dev_type);
uint32_t spd_get_trfc4_min_ps(uint64_t spd_base, uint8_t dev_type);
uint32_t spd_get_twtrs_min_ps(uint64_t spd_base, uint8_t dev_type, uint8_t revision);

bool spd_get_rank_mix(uint64_t spd_base, uint8_t revision);

uint8_t spd_get_out_drv_ctl_cke(uint64_t spd_base, uint8_t dev_type, uint8_t mod_type);
uint8_t spd_get_out_drv_ctl_odt(uint64_t spd_base, uint8_t dev_type, uint8_t mod_type);
uint8_t spd_get_out_drv_ctl_ca(uint64_t spd_base, uint8_t dev_type, uint8_t mod_type);
uint8_t spd_get_out_drv_ctl_cs(uint64_t spd_base, uint8_t dev_type, uint8_t mod_type);

uint16_t spd_get_type_detail(UNUSED_PARAM uint8_t dev_type, uint8_t mod_type);
uint16_t spd_get_max_clk_speed(uint32_t tck_min_ps);
uint16_t spd_get_manufacturer_id(uint64_t spd_base, uint8_t dev_type);
uint32_t spd_get_serial_number(uint64_t spd_base, uint8_t dev_type);
void spd_get_part_number(uint64_t spd_base, uint8_t dev_type, uint8_t part_number[]);

uint32_t spd_get_txx_ps(uint16_t txx_mtb, uint16_t txx_ftb);

// DDR4 LRDIMM Only
uint8_t spd_get_lrdm_db_drv_ck_bcom_bodt_bcke(uint64_t spd_base, uint8_t dev_type, uint8_t mod_type);
uint8_t spd_get_lrdm_db_drv_ck_bck(uint64_t spd_base, uint8_t dev_type, uint8_t mod_type);
void spd_get_lrdimm_config(uint8_t ddr_chan, uint8_t dimm_slot, uint64_t spd_base, uint8_t dev_type, uint8_t mod_type);
uint8_t spd_get_lr_db_dq_dfe(uint64_t spd_base);
uint8_t spd_get_lr_db_dq_gain_adj(uint64_t spd_base);
uint8_t spd_get_lr_db_mdq_ron(uint64_t spd_base, uint8_t ddr_freq_idx);
uint8_t spd_get_lr_db_mdq_term(uint64_t spd_base, uint8_t ddr_freq_idx);
uint8_t spd_get_lr_db_mdq_vrefdq(uint64_t spd_base);
uint8_t spd_get_lr_db_mdq_vrefdq_range(uint64_t spd_base);
uint8_t spd_get_lr_db_rtt_park(uint64_t spd_base, uint8_t rank_config, uint8_t ddr_freq_idx);
uint8_t spd_get_lr_db_rtt_wr(uint64_t spd_base, uint8_t ddr_freq_idx);
uint8_t spd_get_lr_db_rtt_nom(uint64_t spd_base, uint8_t ddr_freq_idx);
uint8_t spd_get_lr_db_ron(uint64_t spd_base, uint8_t ddr_freq_idx);
uint8_t spd_get_lr_db_vrefdq(uint64_t spd_base, uint8_t rank);
uint8_t spd_get_lr_db_vrefdq_range(uint64_t spd_base, uint8_t rank);





/*!****************************************************************************
*
* @par Description:
*    This function initializes access to DIMM SPD when running in the Enablement
*     bare-metal environment.
*
*                                  @param
*    mode               Mode of the target being executed on.
*                        [SILICON|RUMI42|RUMI48|SIM|SIM_RUMI42|SIM_RUMI48]
*                                  @param
*    smbus_info         Array providing the map of platform defined SMBus
*                        addresses to the specific DIMMs based on chan and slot.
*
*
* @return
*    An indication of the level of success.
*
*
******************************************************************************/
void spd_init
(
    target_mode_e mode,
    dimm_slot_smbus_info_t smbus_info[DDR_MAX_NUM_CHANS][DDR_MAX_NUM_SLOTS_PER_CHAN]
)
{
    uint8_t ddr_chan;
    uint8_t dimm_slot;


    // Record SMBus Address mappings
    for(ddr_chan = 0; ddr_chan < DDR_MAX_NUM_CHANS; ++ddr_chan)
    {
        for(dimm_slot = 0; dimm_slot < DDR_MAX_NUM_SLOTS_PER_CHAN; ++dimm_slot)
        {
            spd_smbus_info[ddr_chan][dimm_slot].smbus_id = smbus_info[ddr_chan][dimm_slot].smbus_id;
            spd_smbus_info[ddr_chan][dimm_slot].spd_eeprom_bus_addr = smbus_info[ddr_chan][dimm_slot].spd_eeprom_bus_addr;
            spd_smbus_info[ddr_chan][dimm_slot].spd_ts_bus_addr = smbus_info[ddr_chan][dimm_slot].spd_ts_bus_addr;
            spd_smbus_info[ddr_chan][dimm_slot].spd_cmd_bus_addr = smbus_info[ddr_chan][dimm_slot].spd_cmd_bus_addr;
            spd_smbus_info[ddr_chan][dimm_slot].rcd_bus_addr = smbus_info[ddr_chan][dimm_slot].rcd_bus_addr;
        }
    }


    // SPD is simulated in all modes except silicon since the SPD is not
    //  available on RUMI42 or RUMI48.

    if (mode != SILICON)
    {
        // Init simulation.
        //  Raw simulation data will be stored directly into the saved
        //  SPD info storage block and the call to spd_fetch_ddr_spd_info()
        //  will only indicate the condition of the slot as populated or
        //  unpopulated without actually changing the contents of the buffer.

        // Simulation init that loads the sim data is run once for Monaco and
        //  once for DDR based on the flag spd_use_simulated_spd: by default this
        //  flag is reset in spd_simm.h; mddr_init.c then resets it to ensure
        //  simm init is run.
        if (!spd_use_simulated_spd)
        {
            spd_simm_init(mode);
        }
    }
    else   // Mode is SILICON
    {
        if (spd_use_simm_in_sil && (!spd_use_simulated_spd)) // SILICON: SPD Simm
        {
            spd_simm_init(mode);
        }
        else if (!spd_use_simm_in_sil)
        {
            spd_smbus_init();
        }
    }
}



/*!****************************************************************************
*
* @par Description:
*    This function gets the DIMM serial EEPROM size from the indicated DIMM.
*
*                                  @param
*    chan_num       The DDR channel of the DIMM to inspect.
*                                  @param
*    slot_num       The slot in the DDR channel of the DIMM to inspect.
*
*
* @return
*    Size of the serial EEPROM in bytes.
*
*
******************************************************************************/
unsigned spd_read_dimm_spd_bytes_total(uint8_t chan_num, uint8_t slot_num)
{
    uint8_t spd_dev_info;
    uint8_t bytes_total_indicator;
    unsigned bytes_total;
    unsigned status;
    dimm_slot_smbus_info_t spd_smbus_info;

    // CB: can't use spd_fetch_ddr_spd_info() because that gets the first N bytes
    // Find the SMBus information for the DIMM slot.
    status = spd_smbus_get_dimm_slot_info(chan_num, slot_num, &spd_smbus_info);

    if (status == COM_OK)   // valid chan & slot combination
    {
        status = spd_smbus_get_byte(spd_smbus_info.smbus_id,
                                spd_smbus_info.spd_eeprom_bus_addr,
                                SPD_OFFSET_SPD_DEVICE_INFO, &spd_dev_info);
    }

    if(status == COM_OK)
    {
        // Extract the sign field
        bytes_total_indicator = (spd_dev_info >> SPD_BYTES_TOTAL___S) & SPD_BYTES_TOTAL___M;


        if(bytes_total_indicator == SPD_BYTES_TOTAL_256)
        {
            bytes_total = 256;
        }
        else if(bytes_total_indicator == SPD_BYTES_TOTAL_512)
        {
            bytes_total = 512;
        }
        else
        {
            // Return the undefined value if the value is either undefined or invalid.
            bytes_total = 0;
        }
    }
    else
    {
        // Return 0 since there is no serial EEPROM at the given SMBus address.
        bytes_total = 0;
    }

    return bytes_total;
}



/*!****************************************************************************
*
* @par Description:
*    This function read the first N bytes of SPD from the indicated DIMM.
*
*                                  @param
*    ddr_chan       DDR channel of the DIMM whose SPD is to be read.
*                                  @param
*    dimm_slot      Slot in the DDR channel of the DIMM whose SPD is to be
*                   read.
*                                  @param
*    spd_base       The base address of a data buffer to fill with the requested
*                   SPD data.
*                                  @param
*    load_size      The number of bytes of SPD data to load.
*
*
* @return
*    An indication of the level of success.
*
*
******************************************************************************/
unsigned spd_read_dimm_nbytes
(
    unsigned ddr_chan,
    unsigned dimm_slot,
    uint64_t spd_base,
    unsigned load_size
)
{
    unsigned status;

    // Load the requested SPD data into memory
    status = spd_fetch_ddr_spd_info(ddr_chan, dimm_slot, spd_base, load_size);

    if (status == COM_OK)
    {
        mddr_log(HIGH, "    ** Found DIMM on Chan %u  Slot %u \n", hddrss[ddr_chan], dimm_slot);
    }
    else    // COM_ERROR
    {
        mddr_log(INFORMATIONAL, "    ** No DIMM on Chan %u  Slot %u - Unpopulated\n", hddrss[ddr_chan], dimm_slot);
    }

    return status;
}



/*!****************************************************************************
*
* @par Description:
*    This function gets a SPD block for a specific DIMM slot.
*
*                                  @param
*    ddr_chan       The DDR channel of the DIMM to inspect.
*                                  @param
*    dimm_slot      The slot in the DDR channel of the DIMM to inspect.
*                                  @param
*    spd_base       The base address of a data buffer to fill with the requested
*                   SPD data.
*                                  @param
*    load_size      The number of bytes of SPD data to load.
*
*
* @return
*    An indication of the level of success.
*
*
******************************************************************************/
unsigned spd_fetch_ddr_spd_info
(
    unsigned ddr_chan,
    unsigned dimm_slot,
    uint64_t spd_base,
    unsigned load_size
)
{
    unsigned status;
    dimm_slot_smbus_info_t smbus_info;


    // Find the SMBus information for the DIMM slot.
    status = spd_smbus_get_dimm_slot_info(ddr_chan, dimm_slot, &smbus_info);

    if (status == COM_OK)   // valid chan & slot combination
    {
        // Get DIMM spd block from SMBus
        status = spd_smbus_get_spd_block(smbus_info.smbus_id, smbus_info.spd_eeprom_bus_addr, load_size, spd_base);
    }

    return status;
}



/*!****************************************************************************
*
* @par Description:
*    This function gets the set of DIMM configuration information from the
*    specified block of SPD data.
*
*                                  @param
*    ddr_chan         DDR channel of the DIMM whose SPD DIMM configuration info
*                     is to be obtained.
*                                  @param
*    dimm_slot        Slot in the DDR channel of the DIMM whose SPD DIMM
*                     configuration info is to be obtained.
*                                  @param
*    spd_base         The base address of SPD data buffer to inspect.
*                                  @param
*    dimm_config_ptr  Pointer to the structure to be updated with the
*                     configuration information.
*
*
* @return
*    None
*
*
******************************************************************************/
void spd_get_dimm_config(uint8_t ddr_chan, uint8_t dimm_slot, uint64_t spd_base, spd_dimm_config_t *dimm_config_ptr)
{

    dimm_config_ptr->revision =             spd_get_spd_revision(spd_base);
    dimm_config_ptr->dev_type =             spd_get_ddr_device_type(spd_base);
    dimm_config_ptr->mod_type =             spd_get_ddr_module_type(spd_base);
    dimm_config_ptr->num_ranks =            spd_get_num_ranks(spd_base, dimm_config_ptr->dev_type);
    dimm_config_ptr->sdram_width =          spd_get_sdram_width(spd_base, dimm_config_ptr->dev_type);
    dimm_config_ptr->has_ecc =              spd_get_has_ecc(spd_base, dimm_config_ptr->dev_type);
    dimm_config_ptr->dimm_module_width =    spd_get_dimm_module_width(spd_base, dimm_config_ptr->dev_type);
    dimm_config_ptr->num_bank_groups =      spd_get_num_bank_groups(spd_base);
    dimm_config_ptr->device_capacity =      spd_get_device_capacity(spd_base);
    dimm_config_ptr->num_rows =             spd_get_num_rows(spd_base);
    dimm_config_ptr->num_columns =          spd_get_num_columns(spd_base);
    dimm_config_ptr->die_count =            spd_get_die_count(spd_base, dimm_config_ptr->dev_type);
    dimm_config_ptr->is_3DS =               spd_get_is_3DS(spd_base, dimm_config_ptr->dev_type, dimm_config_ptr->die_count);
    dimm_config_ptr->ca_mirrored =          spd_get_ca_mirrored(spd_base, dimm_config_ptr->dev_type, dimm_config_ptr->mod_type);
    dimm_config_ptr->dimm_size_mb =         spd_calc_dimm_size(dimm_config_ptr->device_capacity, dimm_config_ptr->sdram_width,
                                                               dimm_config_ptr->num_ranks, dimm_config_ptr->dimm_module_width,
                                                               dimm_config_ptr->is_3DS, dimm_config_ptr->die_count);
    dimm_config_ptr->has_therm_sensor =     spd_get_has_therm_sensor(spd_base, dimm_config_ptr->dev_type);
    dimm_config_ptr->is_voltage_ok =        spd_get_is_voltage_ok(spd_base, dimm_config_ptr->dev_type);
    dimm_config_ptr->ftb_fs =               spd_get_ftb_fs(spd_base, dimm_config_ptr->dev_type);
    dimm_config_ptr->mtb_ps =               spd_get_mtb_ps(spd_base, dimm_config_ptr->dev_type);
    dimm_config_ptr->cas_lat_sup =          spd_get_cas_lat_sup(spd_base, dimm_config_ptr->dev_type);

    // Timing Values

    // Common
    dimm_config_ptr->taa_min_ps =           spd_get_taa_min_ps(spd_base, dimm_config_ptr->dev_type);
    dimm_config_ptr->tck_min_ps =           spd_get_tck_min_ps(spd_base, dimm_config_ptr->dev_type);
    dimm_config_ptr->tck_max_ps =           spd_get_tck_max_ps(spd_base, dimm_config_ptr->dev_type);
    dimm_config_ptr->trcd_min_ps =          spd_get_trcd_min_ps(spd_base, dimm_config_ptr->dev_type);
    dimm_config_ptr->trp_min_ps =           spd_get_trp_min_ps(spd_base, dimm_config_ptr->dev_type);
    dimm_config_ptr->trc_min_ps =           spd_get_trc_min_ps(spd_base, dimm_config_ptr->dev_type);
    dimm_config_ptr->tras_min_ps =          spd_get_tras_min_ps(spd_base, dimm_config_ptr->dev_type);
 //   dimm_config_ptr->trfc_min_ps =          spd_get_trfc_min_ps(spd_base, dimm_config_ptr->dev_type);
   dimm_config_ptr->trfc_min_ps =          0;
    dimm_config_ptr->tfaw_min_ps =          spd_get_tfaw_min_ps(spd_base, dimm_config_ptr->dev_type);
 //   dimm_config_ptr->trrd_min_ps =          spd_get_trrd_min_ps(spd_base, dimm_config_ptr->dev_type);
  dimm_config_ptr->trrd_min_ps =         0;
    dimm_config_ptr->twr_min_ps =           spd_get_twr_min_ps(spd_base, dimm_config_ptr->dev_type, dimm_config_ptr->revision);
    dimm_config_ptr->twtr_min_ps =          spd_get_twtr_min_ps(spd_base, dimm_config_ptr->dev_type, dimm_config_ptr->revision);

    dimm_config_ptr->out_drv_ck_y13 =       spd_get_out_drv_ck_y13(spd_base, dimm_config_ptr->dev_type, dimm_config_ptr->mod_type);
    dimm_config_ptr->out_drv_ck_y02 =       spd_get_out_drv_ck_y02(spd_base, dimm_config_ptr->dev_type, dimm_config_ptr->mod_type);

    if (dimm_config_ptr->dev_type == SPD_DEVICE_TYPE_DDR4)
    {
        // DDR4 Only
        dimm_config_ptr->trrds_min_ps =         spd_get_trrds_min_ps(spd_base, dimm_config_ptr->dev_type);
        dimm_config_ptr->tccdl_min_ps =         spd_get_tccdl_min_ps(spd_base, dimm_config_ptr->dev_type);
        dimm_config_ptr->trrdl_min_ps =         spd_get_trrdl_min_ps(spd_base, dimm_config_ptr->dev_type);
        dimm_config_ptr->trfc1_min_ps =         spd_get_trfc1_min_ps(spd_base, dimm_config_ptr->dev_type);
        dimm_config_ptr->trfc2_min_ps =         spd_get_trfc2_min_ps(spd_base, dimm_config_ptr->dev_type);
        dimm_config_ptr->trfc4_min_ps =         spd_get_trfc4_min_ps(spd_base, dimm_config_ptr->dev_type);
        dimm_config_ptr->twtrs_min_ps =         spd_get_twtrs_min_ps(spd_base, dimm_config_ptr->dev_type, dimm_config_ptr->revision);

        dimm_config_ptr->is_rank_mix  =         spd_get_rank_mix(spd_base, dimm_config_ptr->revision);

        if ((dimm_config_ptr->mod_type == SPD_MODULE_TYPE_RDIMM) || (dimm_config_ptr->mod_type == SPD_MODULE_TYPE_DDR4_LRDIMM))
        {
            // DDR4 Only RDIMM and LRDIMM Values
            dimm_config_ptr->out_drv_ctl_cke =  spd_get_out_drv_ctl_cke(spd_base, dimm_config_ptr->dev_type, dimm_config_ptr->mod_type);
            dimm_config_ptr->out_drv_ctl_odt =  spd_get_out_drv_ctl_odt(spd_base, dimm_config_ptr->dev_type, dimm_config_ptr->mod_type);
            dimm_config_ptr->out_drv_ctl_ca =   spd_get_out_drv_ctl_ca(spd_base, dimm_config_ptr->dev_type, dimm_config_ptr->mod_type);
            dimm_config_ptr->out_drv_ctl_cs =   spd_get_out_drv_ctl_cs(spd_base, dimm_config_ptr->dev_type, dimm_config_ptr->mod_type);
        }

        // DDR4 Only LRDIMM Values
        if (dimm_config_ptr->mod_type == SPD_MODULE_TYPE_DDR4_LRDIMM)
        {
            dimm_config_ptr->lrdm_db_drv_ck_bcom_bodt_bcke = spd_get_lrdm_db_drv_ck_bcom_bodt_bcke(spd_base, dimm_config_ptr->dev_type, dimm_config_ptr->mod_type);
            dimm_config_ptr->lrdm_db_drv_ck_bck = spd_get_lrdm_db_drv_ck_bck(spd_base, dimm_config_ptr->dev_type, dimm_config_ptr->mod_type);

            spd_get_lrdimm_config(ddr_chan, dimm_slot, spd_base, dimm_config_ptr->dev_type, dimm_config_ptr->mod_type);
        }
    }


    // DIMM Identification
    dimm_config_ptr->type_detail = spd_get_type_detail(dimm_config_ptr->dev_type, dimm_config_ptr->mod_type);
    dimm_config_ptr->max_clk_speed = spd_get_max_clk_speed(dimm_config_ptr->tck_min_ps);
    dimm_config_ptr->manufacturer_id = spd_get_manufacturer_id(spd_base, dimm_config_ptr->dev_type);
    dimm_config_ptr->serial_number = spd_get_serial_number(spd_base, dimm_config_ptr->dev_type);
    spd_get_part_number(spd_base, dimm_config_ptr->dev_type, dimm_config_ptr->part_number);
}



/*!****************************************************************************
*
* @par Description:
*    This function gets the DIMM temperature from the indicated DIMM.
*
*                                  @param
*    chan_num       The DDR channel of the DIMM to inspect.
*                                  @param
*    slot_num       The slot in the DDR channel of the DIMM to inspect.
*
*
* @return
*    The temperature from a specific DIMM in degrees C. DIMM temperatureis a
*    signed 13 bit 2's complement integer value where the low order 4 bits are
*    the fractional degree component as defined by JEDEC Standard No. 21-C
*     Release 24, Page 4.1.6 – 23 section 2.22. Where bit-0 => 0.0625;
*     bit-1=> 0.125; bit-2=> 0.25; bit-3> 0.5
*
*
******************************************************************************/
int16_t spd_get_dimm_temp(uint8_t ddr_chan, uint8_t slot_num)
{
    unsigned status = COM_OK;
    int16_t range;
    int16_t dimm_temp;
    int16_t sign;


    // Perf improvement to only read the capabilities accross the SMBus once for each channel.
    if(sign_determined[ddr_chan] == false)
    {
        sign_determined[ddr_chan] = true;

        status = spd_smbus_get_word(spd_smbus_info[ddr_chan][slot_num].smbus_id,
                                    spd_smbus_info[ddr_chan][slot_num].spd_ts_bus_addr,
                                    SPD_TS_CAPABILITIES,
                                    (uint16_t *)&(dimm_capabilities[ddr_chan]));
    }

    status |= spd_smbus_get_word(spd_smbus_info[ddr_chan][slot_num].smbus_id,
                                spd_smbus_info[ddr_chan][slot_num].spd_ts_bus_addr,
                                SPD_TS_AMBIENT_TEMPERATURE,
                                (uint16_t *)&dimm_temp);


    if(status == COM_OK)
    {
        // Extract the range to determine if the sign bit is valid.
        range = (dimm_capabilities[ddr_chan] >> SPD_TS_TEMP_RANGE___M) & SPD_TS_TEMP_RANGE___M;

        // Extract the temperature. Note: Bits for unsupported resolutions are required to be 0
        //  per spec so no special treatment is required to adapt to differing resolutions.
        dimm_temp = (dimm_temp >> SPD_TS_TEMP___S) & SPD_TS_TEMP___M;

        // The JEDEC spec is vague on the state of the sign bit if the range is not set, so clear it to be safe.
        if(range != SPD_TS_TEMP_RANGE___M)
        {
            // Clear the sign bit since it is invalid.
            dimm_temp &= ~(SPD_TS_TEMP_SIGN___M << SPD_TS_TEMP_SIGN___S);
        }

        // Extract the sign bit.
        sign = (dimm_temp >> SPD_TS_TEMP_SIGN___S) & SPD_TS_TEMP_SIGN___M;

        if(sign == SPD_TS_TEMP_SIGN___M)
        {
            // Sign bit set so sign extend the value.
            //  Note: This overwrites the sign bit.
            dimm_temp |= 0xFFFF << SPD_TS_TEMP_SIGN___S;
        }
    }
    else // if read failed from existing DIMM, return INVALID marker
    {
        dimm_temp = MDDR_DIMM_TEMP_INVALID;
    }

    mddr_log(LOW, "Processed DIMM temp from Channel %hhu, Slot %hhu, Value: 0x%hx\n", hddrss[ddr_chan], slot_num, dimm_temp);

    return dimm_temp;
}


/*!****************************************************************************
*
* @par Description:
*    This function gets the DIMM EVENT_n assertion status from the indicated DIMM.
*
*                                  @param
*    chan_num       The DDR channel of the DIMM to inspect.
*                                  @param
*    slot_num       The slot in the DDR channel of the DIMM to inspect.
*
*
* @return
*    An indication of whether the EVENT_n pin is asserted from a specific DIMM
*     as defined by JEDEC Standard No. 21-C Release 26, Page 4.1.6 – 21 section 2.17.
*
*
******************************************************************************/
bool spd_get_dimm_eventn_asserted(uint8_t ddr_chan, uint8_t slot_num)
{
    unsigned status = COM_OK;
    uint16_t configuration;
    bool eventn_status;

    status |= spd_smbus_get_word(spd_smbus_info[ddr_chan][slot_num].smbus_id,
                                 spd_smbus_info[ddr_chan][slot_num].spd_ts_bus_addr,
                                 SPD_TS_CONFIGURATION,
                                 &configuration);

    if(status == COM_OK)
    {
        eventn_status = (configuration >> SPD_TS_CONFIGURATION_EVENT_STS___S) & SPD_TS_CONFIGURATION_EVENT_STS___M;
    }
    else
    {
        eventn_status = false;
    }

    mddr_log(LOW, "Processed DIMM eventn status from Channel %hhu, Slot %hhu, Value: 0x%hx\n", hddrss[ddr_chan], slot_num, eventn_status);

    return eventn_status;
}


/******************************************************************************
* Local Functions
******************************************************************************/


/*!****************************************************************************
*
* @par Description:
*    This function gets the DDR DRAM device type from the specified block of SPD
*    data.
*
*                                  @param
*    spd_base       The base address of SPD data buffer to inspect.
*
*
* @return
*    The device type of the DIMM found in the SPD information.
*
*
******************************************************************************/
uint8_t spd_get_ddr_device_type(uint64_t spd_base)
{
    uint64_t byte_addr;
    uint8_t dev_type;


    // Get DIMM device type from the SPD data.
    byte_addr = spd_base + SPD_OFFSET_DRAM_DEVICE_TYPE;
    dev_type = *(uint8_t *)byte_addr;

    return dev_type;
}



/*!****************************************************************************
*
* @par Description:
*    This function gets the DDR module type from the specified block of SPD
*    data.
*
*                                  @param
*    spd_base       The base address of SPD data buffer to inspect.
*
*
* @return
*    The module type of the DIMM found in the SPD information.
*
*
******************************************************************************/
uint8_t spd_get_ddr_module_type(uint64_t spd_base)
{
    uint64_t byte_addr;
    uint8_t mod_type;


    // Get DIMM module type from the SPD data.
    byte_addr = spd_base + SPD_OFFSET_DRAM_MODULE_TYPE;
    mod_type = *(uint8_t *)byte_addr;

    return mod_type;
}


/*!****************************************************************************
*
* @par Description:
*    This function gets the SPD revision from the specified block of SPD
*    data.
*
*                                  @param
*    spd_base       The base address of SPD data buffer to inspect.
*
*
* @return
*    The SPD revision of the DIMM found in the SPD information.
*
* @author
*    lchavani
*
******************************************************************************/
uint8_t spd_get_spd_revision(uint64_t spd_base)
{
    uint64_t byte_addr;
    uint8_t spd_rev;


    // Get DIMM SPD revision from the SPD data.
    byte_addr = spd_base + SPD_OFFSET_SPD_REVISION;
    spd_rev = *(uint8_t *)byte_addr;

    return spd_rev;
}


/*!****************************************************************************
*
* @par Description:
*    This function gets the number of ranks in the DIMM from the specified
*    block of SPD data.
*
*                                  @param
*    spd_base        The base address of SPD data buffer to inspect.
*                                  @param
*    dev_type        Device type, only DDR4.
*
*
* @return
*    The number of ranks present in the DIMM.
*
*
******************************************************************************/
uint8_t spd_get_num_ranks(uint64_t spd_base, uint8_t dev_type)
{
    uint64_t byte_addr=0;
    uint8_t mod_org;
    uint8_t num_ranks;


    if (dev_type == SPD_DEVICE_TYPE_DDR4)
    {
        byte_addr = spd_base + SPD_OFFSET_DDR4_MODULE_ORGANIZATION;
    }

    // Get number of ranks in the DIMM from the SPD data.
    mod_org = *(uint8_t *)byte_addr;
    num_ranks = (mod_org >> SPD_NUM_RANK___S) & (SPD_NUM_RANK___M);
    num_ranks = num_ranks + SPD_NUM_RANKS_SCALE_OFFSET;


    return num_ranks;
}

/*!****************************************************************************
*
* @par Description:
*    This function gets the SDRAM device width from the specified
*    block of SPD data.
*
*                                  @param
*    spd_base        The base address of SPD data buffer to inspect.
*                                  @param
*    dev_type        Device type, DDR3 or DDR4.
*
*
* @return
*    SDRAM device width in bits.
*
*
******************************************************************************/
uint8_t spd_get_sdram_width(uint64_t spd_base, uint8_t dev_type)
{
    uint64_t byte_addr=0;
    uint8_t mod_org;
    uint8_t sdram_width;


    if (dev_type == SPD_DEVICE_TYPE_DDR4)
    {
        byte_addr = spd_base + SPD_OFFSET_DDR4_MODULE_ORGANIZATION;
    }

    // Get DIMM device width from the SPD data.
    mod_org = *(uint8_t *)byte_addr;
    sdram_width = (mod_org >> SPD_DEVICE_WIDTH___S) & SPD_DEVICE_WIDTH___M;
    sdram_width = sdram_width + SPD_DEVICE_WIDTH_SCALE_OFFSET;
    sdram_width = 1 << sdram_width;


    return sdram_width;
}



/*!****************************************************************************
*
* @par Description:
*    This function gets an ECC indication from the specified  block of
*    SPD data.
*
*                                  @param
*    spd_base        The base address of SPD data buffer to inspect.
*                                  @param
*    dev_type        Device type, DDR3 or DDR4.
*
*
* @return
*    Boolean indicating if the SPD indicates the DIMM is ECC.
*
*
******************************************************************************/
bool spd_get_has_ecc(uint64_t spd_base, uint8_t dev_type)
{
    bool has_ecc = false;
    uint64_t byte_addr=0;
    uint8_t module_bus_width;
    uint8_t mod_ecc;


    if (dev_type == SPD_DEVICE_TYPE_DDR4)
    {
        byte_addr = spd_base + SPD_OFFSET_DDR4_MODULE_MEMORY_BUS_WIDTH;
    }

    // Get DIMM bus width information from the SPD data.
    module_bus_width = *(uint8_t *)byte_addr;
    mod_ecc = (module_bus_width >> SPD_MOD_ECC___S) & SPD_MOD_ECC___M;

    if(mod_ecc == SPD_MOD_ECC___M)
    {
        has_ecc = true;
    }

    return has_ecc;
}



/*!****************************************************************************
*
* @par Description:
*    This function gets the DIMM module width from the specified block of
*    SPD data.
*
*                                  @param
*    spd_base        The base address of SPD data buffer to inspect.
*                                  @param
*    dev_type        Device type, DDR3 or DDR4.
*
*
* @return
*    DIMM module width in bits excluding ECC.
*
*
******************************************************************************/
uint8_t spd_get_dimm_module_width(uint64_t spd_base, uint8_t dev_type)
{
    uint64_t byte_addr=0;
    uint8_t module_bus_width;
    uint8_t mod_width;


    if (dev_type == SPD_DEVICE_TYPE_DDR4)
    {
        byte_addr = spd_base + SPD_OFFSET_DDR4_MODULE_MEMORY_BUS_WIDTH;
    }

    // Get DIMM bus width information from the SPD data.
    module_bus_width = *(uint8_t *)byte_addr;
    mod_width = (module_bus_width >> SPD_MOD_WIDTH___S) & SPD_MOD_WIDTH___M;
    mod_width = mod_width + SPD_MOD_WIDTH_SCALE_OFFSET;
    mod_width = 1 << mod_width;


    return mod_width;
}


/*!****************************************************************************
*
* @par Description:
*    This function gets the number of bank groups on the DIMM from the
*    specified block of SPD data. DDR4 only.
*
*                                  @param
*    spd_base        The base address of SPD data buffer to inspect.
*
*
* @return
*    Count of the number of bank groups on the DIMM.
*
* @author
*    gkowis
*
******************************************************************************/
uint8_t spd_get_num_bank_groups(uint64_t spd_base)
{
    uint64_t byte_addr=0;
    uint8_t dens_n_banks;
    uint8_t num_bank_groups;

    byte_addr = spd_base + SPD_OFFSET_SDRAM_DENS_N_BANKS;

    // Get DIMM density and bank information from the SPD data.
    dens_n_banks = *(uint8_t *)byte_addr;
    num_bank_groups = (dens_n_banks >> SPD_BANK_GROUP___S) & SPD_BANK_GROUP___M;

    if(num_bank_groups != 0)
    {
        // 0 is the special case where no there are no bank groups.
        num_bank_groups = 1 << num_bank_groups;
    }

    return num_bank_groups;
}



/*!****************************************************************************
*
* @par Description:
*    This function gets the number of bank groups on the DIMM from the
*    specified block of SPD data. DDR4 only.
*
*                                  @param
*    spd_base        The base address of SPD data buffer to inspect.
*
*
* @return
*    Count of the number of bank groups on the DIMM.
*
*
******************************************************************************/
uint16_t spd_get_device_capacity(uint64_t spd_base)
{
    uint64_t byte_addr;
    uint8_t dens_n_banks;
    uint16_t device_capacity;


    byte_addr = spd_base + SPD_OFFSET_SDRAM_DENS_N_BANKS;


    // Get DIMM density and bank information from the SPD data.
    dens_n_banks = *(uint8_t *)byte_addr;
    device_capacity = (dens_n_banks >> SPD_DEVICE_CAPACITY___S) & SPD_DEVICE_CAPACITY___M;
    device_capacity = device_capacity + SPD_DEVICE_CAPACITY_SCALE_OFFSET;
    device_capacity = 1 << device_capacity;


    return device_capacity;
}



/*!****************************************************************************
*
* @par Description:
*    This function gets the number of rows, per device, on the DIMM from
*    the specified block of SPD data.
*
*                                  @param
*    spd_base        The base address of SPD data buffer to inspect.
*
*
* @return
*    Count of the number of rows in a DIMM SDRAM.
*
*
******************************************************************************/
uint8_t spd_get_num_rows(uint64_t spd_base)
{
    uint64_t byte_addr;
    uint8_t sdram_addressing;
    uint8_t num_rows;


    byte_addr = spd_base + SPD_OFFSET_SDRAM_ADDRESSING;


    // Get device addressing information from the SPD data.
    sdram_addressing = *(uint8_t *)byte_addr;
    num_rows = (sdram_addressing >> SPD_ROWS___S) & SPD_ROWS___M;
    num_rows = num_rows + SPD_ROWS_SCALE_OFFSET;


    return num_rows;
}



/*!****************************************************************************
*
* @par Description:
*    This function gets the number of columns, per device, on the DIMM
*    from the specified block of SPD data.
*
*                                  @param
*    spd_base        The base address of SPD data buffer to inspect.
*
*
* @return
*    Count of the number of columns in a DIMM SDRAM.
*
*
******************************************************************************/
uint8_t spd_get_num_columns(uint64_t spd_base)
{
    uint64_t byte_addr;
    uint8_t sdram_addressing;
    uint8_t num_columns;


    byte_addr = spd_base + SPD_OFFSET_SDRAM_ADDRESSING;


    // Get device addressing information from the SPD data.
    sdram_addressing = *(uint8_t *)byte_addr;
    num_columns = (sdram_addressing >> SPD_COLS___S) & SPD_COLS___M;
    num_columns = num_columns + SPD_COLS_SCALE_OFFSET;

    return num_columns;
}


/*!****************************************************************************
*
* @par Description:
*    This function gets DIMM die count from the specified block of SPD data.
*
*                                  @param
*    spd_base        The base address of SPD data buffer to inspect.
*                                  @param
*    dev_type        Device type, DDR3 or DDR4.
*
*
* @return
*    Count of the number of die on the DIMM.
*
*
******************************************************************************/
uint8_t spd_get_die_count(uint64_t spd_base, uint8_t dev_type)
{
    uint64_t byte_addr=0;
    uint8_t die_count=0;


    if (dev_type == SPD_DEVICE_TYPE_DDR4)
    {
        byte_addr = spd_base + SPD_OFFSET_DDR4_SDRAM_PACKAGE_TYPE;
        die_count = *(uint8_t *)byte_addr;
        die_count = (die_count >> SPD_DDR4_DIE_COUNT___S) & SPD_DDR4_DIE_COUNT___M;
        die_count = die_count + SPD_DDR4_DIE_COUNT_SCALE_OFFSET;
    }

    return die_count;
}



/*!****************************************************************************
*
* @par Description:
*    This function gets an indication of 3DS from the specified block of SPD
*    data.
*
*                                  @param
*    spd_base        The base address of SPD data buffer to inspect.
*                                  @param
*    dev_type        Device type, DDR3 or DDR4.
*                                  @param
*    die_count       DIMM die count.
*
*
* @return
*    Boolean indicating if the SPD indicates the DIMM is 3DS.
*
*
******************************************************************************/
bool spd_get_is_3DS(uint64_t spd_base, uint8_t dev_type, uint8_t die_count)
{
    bool is_3DS = false;
    uint64_t byte_addr;
    uint8_t signal_load;
    uint8_t sdram_package;


    // Get SDRAM device type from the SPD data.
    if (dev_type == SPD_DEVICE_TYPE_DDR4)
    {
        byte_addr = spd_base + SPD_OFFSET_DDR4_SDRAM_PACKAGE_TYPE;
        signal_load = *(uint8_t *)byte_addr;
        signal_load = (signal_load >> SPD_DDR4_SIGNAL_LOAD___S) & SPD_DDR4_SIGNAL_LOAD___M;

        sdram_package = *(uint8_t *)byte_addr;
        sdram_package = (sdram_package >> SPD_DDR4_SDRAM_PACKAGE_TYPE___S) & SPD_DDR4_SDRAM_PACKAGE_TYPE___M;

        if((signal_load == SPD_DDR4_SIGNAL_LOAD_SINGLE) && (sdram_package == SPD_DDR4_SDRAM_PACKAGE_TYPE_MONO) && (die_count > 1))
        {
            is_3DS = true;
        }
    }

    return is_3DS;
}



/*!****************************************************************************
*
* @par Description:
*    This function gets an indication of CA mirroring from the  specified block
*    of SPD data.
*
*                                  @param
*    spd_base        The base address of SPD data buffer to inspect.
*                                  @param
*    dev_type        Device type, DDR3 or DDR4.
*                                  @param
*    mod_type        DIMM module type. RDIMM, UDIMM, ...
*
*
* @return
*    Boolean indicating if the SPD indicates the DIMM is CA mirrored.
*
*
******************************************************************************/
bool spd_get_ca_mirrored(uint64_t spd_base, uint8_t dev_type, uint8_t mod_type)
{
    bool ca_mirrored = false;
    uint64_t byte_addr=0;
    uint8_t ca_mirror;


    // Get CA mirroring information from the SPD data.
    if (dev_type == SPD_DEVICE_TYPE_DDR4)
    {
         if (mod_type == SPD_MODULE_TYPE_UDIMM)
        {
            byte_addr = spd_base + SPD_OFFSET_DDR4_UDIMM_MIRROR;
        }
        else
        {
           // DDR4 RDIMM or LRDIMM
           byte_addr = spd_base + SPD_OFFSET_DDR4_RDIMM_LRDIMM_MIRROR;
        }
    }

    ca_mirror = *(uint8_t *)byte_addr;
    ca_mirror = (ca_mirror >> SPD_CA_MIRROR___S) & SPD_CA_MIRROR___M;

    if(ca_mirror == SPD_CA_MIRRORED)
    {
        ca_mirrored = true;
    }


    return ca_mirrored;
}



/*!****************************************************************************
*
* @par Description:
*    This function calculates the size of the DIMM as described by its' SPD
*    information.
*
*                                  @param
*    device_capacity    Capacity of a single device on the DIMM.
*                                  @param
*    sdram_width        SDRAM device width indicator.
*                                  @param
*    num_ranks          Number of ranks on the the DIMM.
*                                  @param
*    dimm_module_width  Width in bits of the DIMM module, excluding ECC bits.
*
*
* @return
*    The calculated size of the DIMM in MB.
*
*
*
*
******************************************************************************/
uint32_t spd_calc_dimm_size
(
    uint16_t device_capacity,
    uint8_t sdram_width,
    uint8_t num_ranks,
    uint8_t dimm_module_width,
    bool is_3DS,
    uint8_t die_count
)
{
    unsigned dimm_size_mb;


    dimm_size_mb = (dimm_module_width / sdram_width) * num_ranks * device_capacity;

    if (is_3DS)
        dimm_size_mb = dimm_size_mb * die_count;

    return dimm_size_mb;
}



/*!****************************************************************************
*
* @par Description:
*    This function gets an thermal sensor indication from the specified  block
*    of SPD data.
*
*                                  @param
*    spd_base        The base address of SPD data buffer to inspect.
*                                  @param
*    dev_type        Device type, DDR3 or DDR4.
*
*
* @return
*    Boolean indicating if the SPD indicates the DIMM has a thermal sensor.
*
*
******************************************************************************/
bool spd_get_has_therm_sensor(uint64_t spd_base, uint8_t dev_type)
{
    bool has_therm_sensor = false;
    uint64_t byte_addr=0;
    uint8_t mod_therm_sensor;


    if (dev_type == SPD_DEVICE_TYPE_DDR4)
    {
        byte_addr = spd_base + SPD_OFFSET_DDR4_MODULE_THERMAL_SENSOR;
    }

    // Get DIMM thermal sensor information from the SPD data.
    mod_therm_sensor = *(uint8_t *)byte_addr;
    mod_therm_sensor = (mod_therm_sensor >> SPD_TS___S) & SPD_TS___M;

    if(mod_therm_sensor == SPD_TS___M)
    {
        has_therm_sensor = true;
    }

    return has_therm_sensor;
}



/*!****************************************************************************
*
* @par Description:
*    This function gets an indication of DIMM operability at the standard
*    voltage for the DIMM type from the specified block of SPD data.
*
*                                  @param
*    spd_base        The base address of SPD data buffer to inspect.
*                                  @param
*    dev_type        Device type, DDR3 or DDR4.
*
*
* @return
*    Boolean indicating if the SPD indicates the DIMM is operable at the standard
*    voltage for the given DIMM type.
*
*
******************************************************************************/
bool spd_get_is_voltage_ok(uint64_t spd_base, uint8_t dev_type)
{
    bool is_voltage_ok = false;
    uint64_t byte_addr;
    uint8_t vdd_flags;


    if (dev_type == SPD_DEVICE_TYPE_DDR4)
    {
        byte_addr = spd_base + SPD_OFFSET_DDR4_MODULE_NOM_VOLTAGE;
        vdd_flags = *(uint8_t *)byte_addr;
        vdd_flags = (vdd_flags >> SPD_DDR4_MODULE_VDD_1_2_V___S) & SPD_DDR4_MODULE_VDD_1_2_V___M;
        vdd_flags = vdd_flags & SPD_DDR4_MODULE_VDD_1_2_V_OPERABLE;  // Get just the operable flag

        if(vdd_flags == SPD_DDR4_MODULE_VDD_1_2_V_OPERABLE)
        {
            is_voltage_ok = true;
        }
    }

    return is_voltage_ok;
}



/*!****************************************************************************
*
* @par Description:
*    This function gets the DIMMs fine timebase from the specified
*    block of SPD data.
*
*                                  @param
*    spd_base        The base address of SPD data buffer to inspect.
*                                  @param
*    dev_type        Device type, DDR3 or DDR4.
*
*
* @return
*    Fine timebase in fs. Femtosecond are used to eliminate the use of
*     fractional values which are are possible per DDR3 spec.
*
*
******************************************************************************/
uint32_t spd_get_ftb_fs(uint64_t spd_base, uint8_t dev_type)
{
    uint64_t byte_addr;
    uint8_t ftb;
    uint32_t ftb_fs;


    if (dev_type == SPD_DEVICE_TYPE_DDR4)
    {
        byte_addr = spd_base + SPD_OFFSET_DDR4_TIMEBASES;
        ftb = *(uint8_t *)byte_addr;
        ftb = (ftb >> SPD_DDR4_FTB___S) & SPD_DDR4_FTB___M;

        if (ftb != SPD_DDR4_FTB_VAL)
        {
            mddr_log(MEDIUM, "ERROR DDR4 timebases byte NOT equal to %u.  data = %u\n", SPD_DDR4_FTB_VAL, ftb);
        }

        ftb_fs = SPD_DDR4_FTB_PS * FEMTO_SEC_PER_PICO_SEC;
    }

    // Set the global so it can be used in time base translations.
    fine_time_base_fs = ftb_fs;

    return ftb_fs;
}



/*!****************************************************************************
*
* @par Description:
*    This function gets the DIMMs medium timebase from the specified
*    block of SPD data.
*
*                                  @param
*    spd_base        The base address of SPD data buffer to inspect.
*                                  @param
*    dev_type        Device type, DDR3 or DDR4.
*
*
* @return
*    Medium timebase in ps. Fractional values are possible.
*
*
******************************************************************************/
uint32_t spd_get_mtb_ps(uint64_t spd_base, uint8_t dev_type)
{
    uint64_t byte_addr;
    uint8_t mtb;
    uint32_t mtb_ps;


    if (dev_type == SPD_DEVICE_TYPE_DDR4)
    {
        byte_addr = spd_base + SPD_OFFSET_DDR4_TIMEBASES;
        mtb = *(uint8_t *)byte_addr;
        mtb = (mtb >> SPD_DDR4_MTB___S) & SPD_DDR4_MTB___M;

        if (mtb != SPD_DDR4_MTB_VAL)
        {
            mddr_log(MEDIUM, "ERROR DDR4 timebases byte NOT equal to %u.  data = %u\n", SPD_DDR4_MTB_VAL, mtb);
        }

        mtb_ps = SPD_DDR4_MTB_PS;
    }

    // Set the global so it can be used in time base translations.
    medium_time_base_ps = mtb_ps;

    return mtb_ps;
}



/*!****************************************************************************
*
* @par Description:
*    This function gets the CAS latencies supported flags from the specified
*    block of SPD data.
*
*                                  @param
*    spd_base        The base address of SPD data buffer to inspect.
*                                  @param
*    dev_type        Device type, DDR3 or DDR4.
*
*
* @return
*    CAS latencies supported flags.
*
*
******************************************************************************/
uint32_t spd_get_cas_lat_sup(uint64_t spd_base, uint8_t dev_type)
{
    uint64_t byte_addr;
    uint8_t cas_lat_1st;
    uint8_t cas_lat_2nd;
    uint8_t cas_lat_3rd;
    uint8_t cas_lat_4th;
    uint32_t cas_lat=0;


    if (dev_type == SPD_DEVICE_TYPE_DDR4)
    {
        byte_addr = spd_base + SPD_OFFSET_DDR4_CAS_LAT_1ST_BYTE;
        cas_lat_1st = *(uint8_t *)byte_addr;
        cas_lat_1st = (cas_lat_1st >> SPD_DDR4_CAL_LAT_1ST___S) & SPD_DDR4_CAL_LAT_1ST___M;

        byte_addr = spd_base + SPD_OFFSET_DDR4_CAS_LAT_2ND_BYTE;
        cas_lat_2nd = *(uint8_t *)byte_addr;
        cas_lat_2nd = (cas_lat_2nd >> SPD_DDR4_CAL_LAT_2ND___S) & SPD_DDR4_CAL_LAT_2ND___M;

        byte_addr = spd_base + SPD_OFFSET_DDR4_CAS_LAT_3RD_BYTE;
        cas_lat_3rd = *(uint8_t *)byte_addr;
        cas_lat_3rd = (cas_lat_3rd >> SPD_DDR4_CAL_LAT_3RD___S) & SPD_DDR4_CAL_LAT_3RD___M;

        byte_addr = spd_base + SPD_OFFSET_DDR4_CAS_LAT_4TH_BYTE;
        cas_lat_4th = *(uint8_t *)byte_addr;
        cas_lat_4th = (cas_lat_4th >> SPD_DDR4_CAL_LAT_4TH___S) & SPD_DDR4_CAL_LAT_4TH___M;

        cas_lat = (cas_lat_4th << 24) | (cas_lat_3rd << 16) | (cas_lat_2nd << 8) | cas_lat_1st;
    }

    return cas_lat;
}



/*!****************************************************************************
*
* @par Description:
*    This function gets the minimum CAS latency time in ps from the specified
*    block of SPD data.
*
*                                  @param
*    spd_base        The base address of SPD data buffer to inspect.
*                                  @param
*    dev_type        Device type, DDR3 or DDR4.
*
*
* @return
*    The minimum CAS latency time in ps. Fractional values are possible.
*
*
******************************************************************************/
uint32_t spd_get_taa_min_ps(uint64_t spd_base, uint8_t dev_type)
{
    uint64_t byte_addr;
    uint8_t taamin_mtb=0;
    uint8_t taamin_ftb=0;
    uint32_t taa_min_ps;


    if (dev_type == SPD_DEVICE_TYPE_DDR4)
    {
        byte_addr = spd_base + SPD_OFFSET_DDR4_TAAMIN_MTB;
        taamin_mtb = *(uint8_t *)byte_addr;
        taamin_mtb = (taamin_mtb >> SPD_DDR4_TXX_MTB___S) & SPD_DDR4_TXX_MTB___M;

        byte_addr = spd_base + SPD_OFFSET_DDR4_TAAMIN_FTB;
        taamin_ftb = *(uint8_t *)byte_addr;
        taamin_ftb = (taamin_ftb >> SPD_DDR4_TXX_FTB___S) & SPD_DDR4_TXX_FTB___M;
    }

    taa_min_ps = spd_get_txx_ps(taamin_mtb, taamin_ftb);

    return taa_min_ps;
}



/*!****************************************************************************
*
* @par Description:
*    This function gets the SDRAM minimum cycle time in ps from the specified
*    block of SPD data.
*
*                                  @param
*    spd_base        The base address of SPD data buffer to inspect.
*                                  @param
*    dev_type        Device type, DDR3 or DDR4.
*
*
* @return
*    The SDRAM minimum cycle time in ps. Fractional values are possible.
*
*
******************************************************************************/
uint32_t spd_get_tck_min_ps(uint64_t spd_base, uint8_t dev_type)
{
    uint64_t byte_addr;
    uint8_t tckmin_mtb=0;
    uint8_t tckmin_ftb=0;
    uint32_t tck_min_ps;


    if (dev_type == SPD_DEVICE_TYPE_DDR4)
    {
        byte_addr = spd_base + SPD_OFFSET_DDR4_TCKAVGMIN_MTB;
        tckmin_mtb = *(uint8_t *)byte_addr;
        tckmin_mtb = (tckmin_mtb >> SPD_DDR4_TXX_MTB___S) & SPD_DDR4_TXX_MTB___M;

        byte_addr = spd_base + SPD_OFFSET_DDR4_TCKAVGMIN_FTB;
        tckmin_ftb = *(uint8_t *)byte_addr;
        tckmin_ftb = (tckmin_ftb >> SPD_DDR4_TXX_FTB___S) & SPD_DDR4_TXX_FTB___M;
    }

    tck_min_ps = spd_get_txx_ps(tckmin_mtb, tckmin_ftb);

    return tck_min_ps;
}



/*!****************************************************************************
*
* @par Description:
*    This function gets the SDRAM maximum cycle time in ps from the specified
*    block of SPD data.
*
*                                  @param
*    spd_base        The base address of SPD data buffer to inspect.
*                                  @param
*    dev_type        Device type, DDR3 or DDR4.
*
*
* @return
*    The SDRAM maximum cycle time in ps. Fractional values are possible.
*
*
******************************************************************************/
uint32_t spd_get_tck_max_ps(uint64_t spd_base, uint8_t dev_type)
{
    uint64_t byte_addr;
    uint8_t tckmax_mtb=0;
    uint8_t tckmax_ftb=0;
    uint32_t tck_max_ps;


    if (dev_type == SPD_DEVICE_TYPE_DDR4)
    {
        byte_addr = spd_base + SPD_OFFSET_DDR4_TCKAVGMAX_MTB;
        tckmax_mtb = *(uint8_t *)byte_addr;
        tckmax_mtb = (tckmax_mtb >> SPD_DDR4_TXX_MTB___S) & SPD_DDR4_TXX_MTB___M;

        byte_addr = spd_base + SPD_OFFSET_DDR4_TCKAVGMAX_FTB;
        tckmax_ftb = *(uint8_t *)byte_addr;
        tckmax_ftb = (tckmax_ftb >> SPD_DDR4_TXX_FTB___S) & SPD_DDR4_TXX_FTB___M;
    }

    tck_max_ps = spd_get_txx_ps(tckmax_mtb, tckmax_ftb);

    return tck_max_ps;
}



// Functions to Get Timing Values

/*!****************************************************************************
*
* @par Description:
*    This function gets the minimum RAS to CAS delay time in ps from the
*    specified block of SPD data.
*
*                                  @param
*    spd_base        The base address of SPD data buffer to inspect.
*                                  @param
*    dev_type        Device type, DDR3 or DDR4.
*
*
* @return
*    The minimum RAS to CAS delay time in ps. Fractional values are possible.
*
*
******************************************************************************/
uint32_t spd_get_trcd_min_ps(uint64_t spd_base, uint8_t dev_type)
{
    uint64_t byte_addr;
    uint8_t trcdmin_mtb=0;
    uint8_t trcdmin_ftb=0;
    uint32_t trcd_min_ps;


    if (dev_type == SPD_DEVICE_TYPE_DDR4)
    {
        byte_addr = spd_base + SPD_OFFSET_DDR4_TRCDMIN_MTB;
        trcdmin_mtb = *(uint8_t *)byte_addr;
        trcdmin_mtb = (trcdmin_mtb >> SPD_DDR4_TXX_MTB___S) & SPD_DDR4_TXX_MTB___M;

        byte_addr = spd_base + SPD_OFFSET_DDR4_TRCDMIN_FTB;
        trcdmin_ftb = *(uint8_t *)byte_addr;
        trcdmin_ftb = (trcdmin_ftb >> SPD_DDR4_TXX_FTB___S) & SPD_DDR4_TXX_FTB___M;
    }

    trcd_min_ps = spd_get_txx_ps(trcdmin_mtb, trcdmin_ftb);

    return trcd_min_ps;
}



/*!****************************************************************************
*
* @par Description:
*    This function gets the minimum row precharge delay time in ps from the
*    specified block of SPD data.
*
*                                  @param
*    spd_base        The base address of SPD data buffer to inspect.
*                                  @param
*    dev_type        Device type, DDR3 or DDR4.
*
*
* @return
*    The minimum row precharge delay time in ps. Fractional values are possible.
*
*
******************************************************************************/
uint32_t spd_get_trp_min_ps(uint64_t spd_base, uint8_t dev_type)
{
    uint64_t byte_addr;
    uint8_t trpmin_mtb=0;
    uint8_t trpmin_ftb=0;
    uint32_t trp_min_ps;


    if (dev_type == SPD_DEVICE_TYPE_DDR4)
    {
        byte_addr = spd_base + SPD_OFFSET_DDR4_TRPMIN_MTB;
        trpmin_mtb = *(uint8_t *)byte_addr;
        trpmin_mtb = (trpmin_mtb >> SPD_DDR4_TXX_MTB___S) & SPD_DDR4_TXX_MTB___M;

        byte_addr = spd_base + SPD_OFFSET_DDR4_TRPMIN_FTB;
        trpmin_ftb = *(uint8_t *)byte_addr;
        trpmin_ftb = (trpmin_ftb >> SPD_DDR4_TXX_FTB___S) & SPD_DDR4_TXX_FTB___M;
    }

    trp_min_ps = spd_get_txx_ps(trpmin_mtb, trpmin_ftb);

    return trp_min_ps;
}


/*!****************************************************************************
*
* @par Description:
*    This function gets the minimum active to precharge delay time in ps from
*    the specified block of SPD data.
*
*                                  @param
*    spd_base        The base address of SPD data buffer to inspect.
*                                  @param
*    dev_type        Device type, DDR3 or DDR4.
*
*
* @return
*    The minimum active to precharge delay time in ps. Fractional values are
*    possible.
*
*
******************************************************************************/
uint32_t spd_get_tras_min_ps(uint64_t spd_base, uint8_t dev_type)
{
    uint64_t byte_addr;
    uint16_t tras_msn;
    uint16_t tras_mtb=0;
    uint32_t tras_ps;


    if (dev_type == SPD_DEVICE_TYPE_DDR4)
    {
        // MSN - Most Significant Nibble
        byte_addr = spd_base + SPD_OFFSET_DDR4_TRC_TRAS_MSB;
        tras_msn = *(uint8_t *)byte_addr;
        tras_msn = (tras_msn >> SPD_DDR4_TRAS_MTB_MSN___S) & SPD_DDR4_TRAS_MTB_MSN___M;

        byte_addr = spd_base + SPD_OFFSET_DDR4_TRAS_LSB;
        tras_mtb = *(uint8_t *)byte_addr;
        tras_mtb = (tras_mtb >> SPD_DDR4_TXX_MTB___S) & SPD_DDR4_TXX_MTB___M;

        tras_mtb = (tras_msn << 0x8) | tras_mtb;
    }

    tras_ps = spd_get_txx_ps(tras_mtb, 0);

    return tras_ps;
}



/*!****************************************************************************
*
* @par Description:
*    This function gets the minimum active to precharge delay time in ps from
*    the specified block of SPD data.
*
*                                  @param
*    spd_base        The base address of SPD data buffer to inspect.
*                                  @param
*    dev_type        Device type, DDR3 or DDR4.
*
*
* @return
*    The minimum active to precharge delay time in ps. Fractional values are
*    possible.
*
*
******************************************************************************/
uint32_t spd_get_trc_min_ps(uint64_t spd_base, uint8_t dev_type)
{
    uint64_t byte_addr;
    uint16_t trc_msn;
    uint16_t trc_mtb=0;
    uint16_t trc_ftb=0;
    uint32_t trc_ps;


    if (dev_type == SPD_DEVICE_TYPE_DDR4)
    {
        // MSN - Most Significant Nibble
        byte_addr = spd_base + SPD_OFFSET_DDR4_TRC_TRAS_MSB;
        trc_msn = *(uint8_t *)byte_addr;
        trc_msn = (trc_msn >> SPD_DDR4_TRC_MTB_MSN___S) & SPD_DDR4_TRC_MTB_MSN___M;

        byte_addr = spd_base + SPD_OFFSET_DDR4_TRC_LSB;
        trc_mtb = *(uint8_t *)byte_addr;
        trc_mtb = (trc_mtb >> SPD_DDR4_TXX_MTB___S) & SPD_DDR4_TXX_MTB___M;
        trc_mtb = (trc_msn << 0x8) | trc_mtb;

        byte_addr = spd_base + SPD_OFFSET_DDR4_TRC_FTB;
        trc_ftb = *(uint8_t *)byte_addr;
        trc_ftb = (trc_ftb >> SPD_DDR4_TXX_FTB___S) & SPD_DDR4_TXX_FTB___M;
    }

    trc_ps = spd_get_txx_ps(trc_mtb, trc_ftb);

    return trc_ps;
}



/*!****************************************************************************
*
* @par Description:
*    This function gets the minimum four activate window time in ps from
*    the specified block of SPD data.
*
*                                  @param
*    spd_base        The base address of SPD data buffer to inspect.
*                                  @param
*    dev_type        Device type, DDR3 or DDR4.
*
*
* @return
*    The minimum four activate window time in ps. Fractional values are
*    possible.
*
*
******************************************************************************/

uint32_t spd_get_tfaw_min_ps(uint64_t spd_base, uint8_t dev_type)
{
    uint64_t byte_addr;
    uint16_t tfaw_msb=0;
    uint16_t tfaw_lsb=0;
    uint16_t tfaw_mtb=0;
    uint32_t tfaw_ps;


    if (dev_type == SPD_DEVICE_TYPE_DDR4)
    {
        byte_addr = spd_base + SPD_OFFSET_DDR4_TFAW_MSB;
        tfaw_msb = *(uint8_t *)byte_addr;
        tfaw_msb = (tfaw_msb >> SPD_DDR4_TXX_MTB___S) & SPD_DDR4_TXX_MTB___M;

        byte_addr = spd_base + SPD_OFFSET_DDR4_TFAW_LSB;
        tfaw_lsb = *(uint8_t *)byte_addr;
        tfaw_lsb = (tfaw_lsb >> SPD_DDR4_TXX_MTB___S) & SPD_DDR4_TXX_MTB___M;
    }

    tfaw_mtb = (tfaw_msb << 0x8) | tfaw_lsb;
    tfaw_ps = spd_get_txx_ps(tfaw_mtb, 0);

    return tfaw_ps;
}



/*!****************************************************************************
*
* @par Description:
*    This function gets the RDIMM / LRDIMM clock driver Y13 signal output strength from
*    the specified block of SPD data.
*
*                                  @param
*    spd_base        The base address of SPD data buffer to inspect.
*                                  @param
*    dev_type        Device type, DDR3 or DDR4.
*
*
* @return
*    The strength indicator value for the RDIMM / LRDIMM clock driver Y13 signal output
*    strength indicator.
*
*
******************************************************************************/
uint8_t spd_get_out_drv_ck_y13(uint64_t spd_base, uint8_t dev_type, uint8_t mod_type)
{
    uint64_t byte_addr;
    uint8_t out_drv_ck_y13 = 0;


 if ((dev_type == SPD_DEVICE_TYPE_DDR4) && ((mod_type == SPD_MODULE_TYPE_RDIMM) || (mod_type == SPD_MODULE_TYPE_DDR4_LRDIMM)))
    {
        byte_addr = spd_base + SPD_OFFSET_DDR4_RDIMM_LRDIMM_OUTPUT_DRIVE_CK;
        out_drv_ck_y13 = *(uint8_t *)byte_addr;
        out_drv_ck_y13 = (out_drv_ck_y13 >> SPD_DDR4_OUT_DRV_CK_Y13___S) & SPD_DDR4_OUT_DRV_STRENGTH___M;
    }

    return out_drv_ck_y13;
}


/*!****************************************************************************
*
* @par Description:
*    This function gets the RDIMM / LRDIMM clock driver Y02 signal output strength from
*    the specified block of SPD data.
*
*                                  @param
*    spd_base        The base address of SPD data buffer to inspect.
*                                  @param
*    dev_type        Device type, DDR3 or DDR4.
*
*
* @return
*    The strength indicator value for the RDIMM / LRDIMM clock driver Y02 signal output
*    strength indicator.
*
*
******************************************************************************/
uint8_t spd_get_out_drv_ck_y02(uint64_t spd_base, uint8_t dev_type, uint8_t mod_type)
{
    uint64_t byte_addr;
    uint8_t out_drv_ck_y02 = 0;


 if ((dev_type == SPD_DEVICE_TYPE_DDR4) && ((mod_type == SPD_MODULE_TYPE_RDIMM) || (mod_type == SPD_MODULE_TYPE_DDR4_LRDIMM)))
    {
        byte_addr = spd_base + SPD_OFFSET_DDR4_RDIMM_LRDIMM_OUTPUT_DRIVE_CK;
        out_drv_ck_y02 = *(uint8_t *)byte_addr;
        out_drv_ck_y02 = (out_drv_ck_y02 >> SPD_DDR4_OUT_DRV_CK_Y02___S) & SPD_DDR4_OUT_DRV_STRENGTH___M;
    }

    return out_drv_ck_y02;
}


/*!****************************************************************************
*
* @par Description:
*    This function gets the minimum row active to row active delay time in ps
*    from the specified block of SPD data. DDR3 Only!
*
*                                  @param
*    spd_base        The base address of SPD data buffer to inspect.
*                                  @param
*    dev_type        Device type, DDR3 or DDR4.
*
*
* @return
*    The minimum row active to row active delay time in ps. Fractional values
*    are possible. DDR3 Only! 0 for DDR4.
*
*
******************************************************************************/
uint32_t spd_get_trrd_min_ps(uint64_t spd_base, uint8_t dev_type)
{
    uint32_t trrd_ps = 0;

    return trrd_ps;
}



/*!****************************************************************************
*
* @par Description:
*    This function gets the minimum write recovery time in ps from the
*    specified block of SPD data.
*
*                                  @param
*    spd_base        The base address of SPD data buffer to inspect.
*                                  @param
*    dev_type        Device type, DDR3 or DDR4.
*                                  @param
*    revision        The revision of the SPD.
*
*
* @return
*    The minimum write recovery time in ps. Fractional values are possible.
*    DDR3 Only! 0 for DDR4.
*
*
******************************************************************************/
uint32_t spd_get_twr_min_ps(uint64_t spd_base, uint8_t dev_type, uint8_t revision)
{
    uint64_t byte_addr;
    uint8_t twr_msb;
    uint8_t twr_lsb;
    uint8_t twr_mtb=0;
    uint32_t twr_ps = 0;


     if (dev_type == SPD_DEVICE_TYPE_DDR4)
    {
        if (revision > SPD_REVISION_1_0)
        {
            byte_addr = spd_base + SPD_OFFSET_DDR4_TWRMIN_MSB;
            twr_msb = *(uint8_t *)byte_addr;
            twr_msb = (twr_msb >> SPD_DDR4_TWR_MSB___S) & SPD_DDR4_TWR_MSB___M;

            byte_addr = spd_base + SPD_OFFSET_DDR4_TWRMIN_LSB;
            twr_lsb = *(uint8_t *)byte_addr;
            twr_lsb = (twr_lsb >> SPD_DDR4_TWR_LSB___S) & SPD_DDR4_TWR_LSB___M;

            twr_mtb = (twr_msb << 0x8) | twr_lsb;

            twr_ps = spd_get_txx_ps(twr_mtb, 0);
        }
        else
        {
            // tWR is 15ns for all frequencies
            twr_ps = 0x3A98;
        }
    }

    return twr_ps;
}



/*!****************************************************************************
*
* @par Description:
*    This function gets the minimum internal write to read command delay time
*    in ps from the specified block of SPD data.
*
*                                  @param
*    spd_base        The base address of SPD data buffer to inspect.
*                                  @param
*    dev_type        Device type, DDR3 or DDR4.
*                                  @param
*    revision        The revision of the SPD.
*
*
* @return
*    The minimum internal write to read command delay time in ps. Fractional
*    values are possible. DDR3 Only! 0 for DDR4.
*
*
******************************************************************************/
uint32_t spd_get_twtr_min_ps(uint64_t spd_base, uint8_t dev_type, uint8_t revision)
{
    uint64_t byte_addr;
    uint8_t twtr_mtb=0;
    uint8_t twtr_msb;
    uint8_t twtrl_lsb;
    uint32_t twtr_ps = 0;


    if (dev_type == SPD_DEVICE_TYPE_DDR4)
    {
        // tWTR_L
        if (revision > SPD_REVISION_1_0)
        {
            byte_addr = spd_base + SPD_OFFSET_DDR4_TWTRMIN_MSB;
            twtr_msb = *(uint8_t *)byte_addr;
            twtr_msb = (twtr_msb >> SPD_DDR4_TWTRL_MSB_MSN___S) & SPD_DDR4_TWTRL_MSB_MSN___M;

            byte_addr = spd_base + SPD_OFFSET_DDR4_TWTRLMIN_LSB;
            twtrl_lsb = *(uint8_t *)byte_addr;
            twtrl_lsb = (twtrl_lsb >> SPD_DDR4_TWTRL_LSB___S) & SPD_DDR4_TWTRL_LSB___M;

            twtr_mtb = (twtr_msb << 0x8) | twtrl_lsb;

            twtr_ps = spd_get_txx_ps(twtr_mtb, 0);
        }
        else
        {
            // twtr_l=max(4nCK, 7.5ns) up to 2400, rest is TBD
            twtr_ps = 0x1D4C;
        }
    }

    return twtr_ps;
}


/*!****************************************************************************
*
* @par Description:
*    This function gets the minimum delay from start of internal write trans-action
*     to internal read command for dif-ferent bank group.DDR4 Only!
*
*                                  @param
*    spd_base        The base address of SPD data buffer to inspect.
*                                  @param
*    dev_type        Device type, DDR3 or DDR4.
*                                  @param
*    revision        The revision of the SPD.
*
*
* @return
*    The minimum delay time from start of internal write trans-action
*     to internal read command for dif-ferent bank group in ps. Fractional
*     values are possible. DDR4 Only!
*
*
******************************************************************************/
uint32_t spd_get_twtrs_min_ps(uint64_t spd_base, uint8_t dev_type, uint8_t revision)
{
    uint64_t byte_addr;
    uint8_t twtrs_mtb=0;
    uint8_t twtr_msb;
    uint8_t twtrs_lsb;
    uint32_t twtrs_ps = 0;


    if (dev_type == SPD_DEVICE_TYPE_DDR4)
    {
        // tWTR_S
        if (revision > SPD_REVISION_1_0)
        {
            byte_addr = spd_base + SPD_OFFSET_DDR4_TWTRMIN_MSB;
            twtr_msb = *(uint8_t *)byte_addr;
            twtr_msb = (twtr_msb >> SPD_DDR4_TWTRS_MSB_MSN___S) & SPD_DDR4_TWTRS_MSB_MSN___M;

            byte_addr = spd_base + SPD_OFFSET_DDR4_TWTRSMIN_LSB;
            twtrs_lsb = *(uint8_t *)byte_addr;
            twtrs_lsb = (twtrs_lsb >> SPD_DDR4_TWTRS_LSB___S) & SPD_DDR4_TWTRS_LSB___M;

            twtrs_mtb = (twtr_msb << 0x8) | twtrs_lsb;

            twtrs_ps = spd_get_txx_ps(twtrs_mtb, 0);
        }
        else
        {
            // twtr_s=max(2nCK, 2.5ns) up to 2400, rest is TBD
            twtrs_ps = 0x9C4;
        }
    }

    return twtrs_ps;
}


/*!****************************************************************************
*
* @par Description:
*    This function gets the bit to indicate if the ranks are asymmetrical.
*    For DDR4 only, revision > 1.0
*
*                                  @param
*    spd_base        The base address of SPD data buffer to inspect.
*                                  @param
*    revision        The revision of the SPD.
*
*
* @return
*    True if asymmetrical ranks (mixed), else returns false (symmetrical).
*     DDR4 Only!
*
*
******************************************************************************/
bool spd_get_rank_mix(uint64_t spd_base, uint8_t revision)
{
    uint64_t byte_addr;
    bool rank_mix = false;
    uint8_t mod_org;

    if (revision > SPD_REVISION_1_0)
    {
        byte_addr = spd_base + SPD_OFFSET_DDR4_MODULE_ORGANIZATION;

        mod_org = *(uint8_t *)byte_addr;

        if (((mod_org >> SPD_DDR4_RANK_MIX___S) & SPD_DDR4_RANK_MIX___M) > 0)
        {
            rank_mix = true;
        }
    }

    return rank_mix;
}


/*!****************************************************************************
*
* @par Description:
*    This function gets the minimum internal read to precharge command delay
*    time in ps from the specified block of SPD data. DDR3 Only!
*
*                                  @param
*    spd_base        The base address of SPD data buffer to inspect.
*                                  @param
*    dev_type        Device type, DDR3 or DDR4.
*
*
* @return
*    The minimum internal read to precharge command delay time in ps.
*    Fractional values are possible. DDR3 Only! 0 for DDR4.
*
******************************************************************************/
uint32_t spd_get_trtp_min_ps(uint64_t spd_base, uint8_t dev_type)
{
    uint32_t trtp_ps = 0;

    return trtp_ps;
}



/*!****************************************************************************
*
* @par Description:
*    This function gets the minimum refresh recovery delay time in ps from
*    the specified block of SPD data. DDR3 Only!
*
*                                  @param
*    spd_base        The base address of SPD data buffer to inspect.
*                                  @param
*    dev_type        Device type, DDR3 or DDR4.
*
*
* @return
*    The minimum refresh recovery delay time in ps. Fractional values are
*    possible. DDR3 Only! 0 for DDR4.
*
*
******************************************************************************/
uint32_t spd_get_trfc_min_ps(uint64_t spd_base, uint8_t dev_type)
{
    uint32_t trfc_ps = 0;

    return trfc_ps;
}



/*!****************************************************************************
*
* @par Description:
*    This function gets the RDIMM driver output strength for control signal A
*    from the specified block of SPD data.
*
*                                  @param
*    spd_base        The base address of SPD data buffer to inspect.
*                                  @param
*    dev_type        Device type, DDR3 or DDR4.
*                                  @param
*    mod_type        Module type type, RDIMM, UDIMM ....
*
*
* @return
*    The strength indicator value for the RDIMM driver output strength for
*    control signal A. DDR3 Only! RDIMM_INVALID_DRIVE for DDR4.
*
*
******************************************************************************/
uint8_t spd_get_rdm_out_drv_ctl_a(uint64_t spd_base, uint8_t dev_type, uint8_t mod_type)
{
    uint8_t rdm_out_drv_ctl_a = RDIMM_INVALID_DRIVE;
    return rdm_out_drv_ctl_a;
}



/*!****************************************************************************
*
* @par Description:
*    This function gets the RDIMM driver output strength for control signal B
*    from the specified block of SPD data.
*
*                                  @param
*    spd_base        The base address of SPD data buffer to inspect.
*                                  @param
*    dev_type        Device type, DDR3 or DDR4.
*                                  @param
*    mod_type        Module type type, RDIMM, UDIMM ....
*
*
* @return
*    The strength indicator value for the RDIMM driver output strength for
*    control signal B. DDR3 Only! RDIMM_INVALID_DRIVE for DDR4.
*
******************************************************************************/
uint8_t spd_get_rdm_out_drv_ctl_b(uint64_t spd_base, uint8_t dev_type, uint8_t mod_type)
{
    uint8_t rdm_out_drv_ctl_b = RDIMM_INVALID_DRIVE;

    return rdm_out_drv_ctl_b;
}



/*!****************************************************************************
*
* @par Description:
*    This function gets the RDIMM driver output strength for commands/addresses
*    signal A from the specified block of SPD data.
*
*                                  @param
*    spd_base        The base address of SPD data buffer to inspect.
*                                  @param
*    dev_type        Device type, DDR3 or DDR4.
*                                  @param
*    mod_type        Module type type, RDIMM, UDIMM ....
*
*
* @return
*    The strength indicator value for the RDIMM driver output strength for
*    commands/addresses signal A. DDR3 Only! RDIMM_INVALID_DRIVE for DDR4.
*
*
******************************************************************************/
uint8_t spd_get_rdm_out_drv_ca_a(uint64_t spd_base, uint8_t dev_type, uint8_t mod_type)
{
    uint8_t rdm_out_drv_ca_a = RDIMM_INVALID_DRIVE;

    return rdm_out_drv_ca_a;
}



/*!****************************************************************************
*
* @par Description:
*    This function gets the RDIMM driver output strength for commands/addresses
*    signal B from the specified block of SPD data.
*
*                                  @param
*    spd_base        The base address of SPD data buffer to inspect.
*                                  @param
*    dev_type        Device type, DDR3 or DDR4.
*                                  @param
*    mod_type        Module type type, RDIMM, UDIMM ....
*
*
* @return
*    The strength indicator value for the RDIMM driver output strength for
*    commands/addresses signal B. DDR3 Only! RDIMM_INVALID_DRIVE for DDR4.
*
*
******************************************************************************/
uint8_t spd_get_rdm_out_drv_ca_b(uint64_t spd_base, uint8_t dev_type, uint8_t mod_type)
{
    uint8_t rdm_out_drv_ca_b = RDIMM_INVALID_DRIVE;
    return rdm_out_drv_ca_b;
}



/*!****************************************************************************
*
* @par Description:
*    This function gets the 16 RDIMM configuration words.
*
*                                  @param
*    spd_base        The base address of SPD data buffer to inspect.
*                                  @param
*    dev_type        Device type, DDR3 or DDR4.
*                                  @param
*    mod_type        Module type type, RDIMM, UDIMM ....
*
*
* @return
*    The 16 RDIMM configuration words. DDR3 Only! RDIMM_INVALID_CWS for DDR4.
*
*
******************************************************************************/
uint64_t spd_get_rdm_cw15_0(uint64_t spd_base, uint8_t dev_type, uint8_t mod_type)
{
    uint64_t rdm_cw15_0 = RDIMM_INVALID_CWS;
    return rdm_cw15_0;
}

/*!****************************************************************************
*
* @par Description:
*    This function gets the minimum activate to activate delay time, same bank
*    group, in ps from the specified block of SPD data.
*
*                                  @param
*    spd_base        The base address of SPD data buffer to inspect.
*                                  @param
*    dev_type        Device type, DDR3 or DDR4.
*
*
* @return
*    The minimum activate to activate delay time, same bank group, in ps.
*    Fractional values are possible. DDR4 Only! 0 for DDR3.
*
*
******************************************************************************/
uint32_t spd_get_trrdl_min_ps(uint64_t spd_base, uint8_t dev_type)
{
    uint64_t byte_addr;
    uint8_t trrdlmin_mtb;
    uint8_t trrdlmin_ftb;
    uint32_t trrdl_min_ps = 0;


    if (dev_type == SPD_DEVICE_TYPE_DDR4)
    {
        byte_addr = spd_base + SPD_OFFSET_DDR4_TRRDLMIN_MTB;
        trrdlmin_mtb = *(uint8_t *)byte_addr;
        trrdlmin_mtb = (trrdlmin_mtb >> SPD_DDR4_TXX_MTB___S) & SPD_DDR4_TXX_MTB___M;

        byte_addr = spd_base + SPD_OFFSET_DDR4_TRRDLMIN_FTB;
        trrdlmin_ftb = *(uint8_t *)byte_addr;
        trrdlmin_ftb = (trrdlmin_ftb >> SPD_DDR4_TXX_FTB___S) & SPD_DDR4_TXX_FTB___M;

        trrdl_min_ps = spd_get_txx_ps(trrdlmin_mtb, trrdlmin_ftb);
    }
    return trrdl_min_ps;
}



/*!****************************************************************************
*
* @par Description:
*    This function gets the minimum activate to activate delay time, different
*    bank group, in ps from the specified block of SPD data.
*
*                                  @param
*    spd_base        The base address of SPD data buffer to inspect.
*                                  @param
*    dev_type        Device type, DDR3 or DDR4.
*
*
* @return
*    The minimum activate to activate delay time, different bank group, in ps.
*    Fractional values are possible. DDR4 Only! 0 for DDR3.
*
*
******************************************************************************/
uint32_t spd_get_trrds_min_ps(uint64_t spd_base, uint8_t dev_type)
{
    uint64_t byte_addr;
    uint8_t trrdsmin_mtb;
    uint8_t trrdsmin_ftb;
    uint32_t trrds_min_ps = 0;


    if (dev_type == SPD_DEVICE_TYPE_DDR4)
    {
        byte_addr = spd_base + SPD_OFFSET_DDR4_TRRDSMIN_MTB;
        trrdsmin_mtb = *(uint8_t *)byte_addr;
        trrdsmin_mtb = (trrdsmin_mtb >> SPD_DDR4_TXX_MTB___S) & SPD_DDR4_TXX_MTB___M;

        byte_addr = spd_base + SPD_OFFSET_DDR4_TRRDSMIN_FTB;
        trrdsmin_ftb = *(uint8_t *)byte_addr;
        trrdsmin_ftb = (trrdsmin_ftb >> SPD_DDR4_TXX_FTB___S) & SPD_DDR4_TXX_FTB___M;

        trrds_min_ps = spd_get_txx_ps(trrdsmin_mtb, trrdsmin_ftb);
    }


    return trrds_min_ps;
}



/*!****************************************************************************
*
* @par Description:
*    This function gets the minimum CAS to CAS delay time, same bank group,
*    in ps from the specified block of SPD data.
*
*                                  @param
*    spd_base        The base address of SPD data buffer to inspect.
*                                  @param
*    dev_type        Device type, DDR3 or DDR4.
*
*
* @return
*    The minimum CAS to CAS delay time, same bank group, in ps. Fractional
*    values are possible. DDR4 Only! 0 for DDR3.
*
*
******************************************************************************/
uint32_t spd_get_tccdl_min_ps(uint64_t spd_base, uint8_t dev_type)
{
    uint64_t byte_addr;
    uint8_t tccdlmin_mtb;
    uint8_t tccdlmin_ftb;
    uint32_t tccdl_min_ps = 0;


    if (dev_type == SPD_DEVICE_TYPE_DDR4)
    {
        byte_addr = spd_base + SPD_OFFSET_DDR4_TCCDLMIN_MTB;
        tccdlmin_mtb = *(uint8_t *)byte_addr;
        tccdlmin_mtb = (tccdlmin_mtb >> SPD_DDR4_TXX_MTB___S) & SPD_DDR4_TXX_MTB___M;

        byte_addr = spd_base + SPD_OFFSET_DDR4_TCCDLMIN_FTB;
        tccdlmin_ftb = *(uint8_t *)byte_addr;
        tccdlmin_ftb = (tccdlmin_ftb >> SPD_DDR4_TXX_FTB___S) & SPD_DDR4_TXX_FTB___M;

        tccdl_min_ps = spd_get_txx_ps(tccdlmin_mtb, tccdlmin_ftb);
    }


    return tccdl_min_ps;
}



/*!****************************************************************************
*
* @par Description:
*    This function gets the minimum RFC1 refresh recovery delay time in ps from
*    the specified block of SPD data.
*
*                                  @param
*    spd_base        The base address of SPD data buffer to inspect.
*                                  @param
*    dev_type        Device type, DDR3 or DDR4.
*
*
* @return
*    The minimum RFC1 refresh recovery delay time in ps. Fractional values are
*    possible. DDR4 Only! 0 for DDR3.
*
*
******************************************************************************/
uint32_t spd_get_trfc1_min_ps(uint64_t spd_base, uint8_t dev_type)
{
    uint64_t byte_addr;
    uint16_t trfc1_msb;
    uint16_t trfc1_lsb;
    uint16_t trfc1_mtb;
    uint32_t trfc1_ps = 0;


    if (dev_type == SPD_DEVICE_TYPE_DDR4)
    {
        byte_addr = spd_base + SPD_OFFSET_DDR4_TRFC1MIN_MSB;
        trfc1_msb = *(uint8_t *)byte_addr;
        trfc1_msb = (trfc1_msb >> SPD_DDR4_TXX_MTB___S) & SPD_DDR4_TXX_MTB___M;

        byte_addr = spd_base + SPD_OFFSET_DDR4_TRFC1MIN_LSB;
        trfc1_lsb = *(uint8_t *)byte_addr;
        trfc1_lsb = (trfc1_lsb >> SPD_DDR4_TXX_MTB___S) & SPD_DDR4_TXX_MTB___M;

        trfc1_mtb = (trfc1_msb << 0x8) | trfc1_lsb;
        trfc1_ps = spd_get_txx_ps(trfc1_mtb, 0);
    }


    return trfc1_ps;
}



/*!****************************************************************************
*
* @par Description:
*    This function gets the minimum RFC2 refresh recovery delay time in ps from
*    the specified block of SPD data.
*
*                                  @param
*    spd_base        The base address of SPD data buffer to inspect.
*                                  @param
*    dev_type        Device type, DDR3 or DDR4.
*
*
* @return
*    The minimum RFC2 refresh recovery delay time in ps. Fractional values are
*    possible. DDR4 Only! 0 for DDR3.
*
*
******************************************************************************/
uint32_t spd_get_trfc2_min_ps(uint64_t spd_base, uint8_t dev_type)
{
    uint64_t byte_addr;
    uint16_t trfc2_msb;
    uint16_t trfc2_lsb;
    uint16_t trfc2_mtb;
    uint32_t trfc2_ps = 0;


    if (dev_type == SPD_DEVICE_TYPE_DDR4)
    {
        byte_addr = spd_base + SPD_OFFSET_DDR4_TRFC2MIN_MSB;
        trfc2_msb = *(uint8_t *)byte_addr;
        trfc2_msb = (trfc2_msb >> SPD_DDR4_TXX_MTB___S) & SPD_DDR4_TXX_MTB___M;

        byte_addr = spd_base + SPD_OFFSET_DDR4_TRFC2MIN_LSB;
        trfc2_lsb = *(uint8_t *)byte_addr;
        trfc2_lsb = (trfc2_lsb >> SPD_DDR4_TXX_MTB___S) & SPD_DDR4_TXX_MTB___M;

        trfc2_mtb = (trfc2_msb << 0x8) | trfc2_lsb;
        trfc2_ps = spd_get_txx_ps(trfc2_mtb, 0);
    }


    return trfc2_ps;
}



/*!****************************************************************************
*
* @par Description:
*    This function gets the minimum RFC4 refresh recovery delay time in ps from
*    the specified block of SPD data.
*
*                                  @param
*    spd_base        The base address of SPD data buffer to inspect.
*                                  @param
*    dev_type        Device type, DDR3 or DDR4.
*
*
* @return
*    The minimum RFC4 refresh recovery delay time in ps. Fractional values are
*    possible. DDR4 Only! 0 for DDR3.
*
*
******************************************************************************/
uint32_t spd_get_trfc4_min_ps(uint64_t spd_base, uint8_t dev_type)
{
    uint64_t byte_addr;
    uint16_t trfc4_msb;
    uint16_t trfc4_lsb;
    uint16_t trfc4_mtb;
    uint32_t trfc4_ps = 0;


    if (dev_type == SPD_DEVICE_TYPE_DDR4)
    {
        // MSB - Most Significant Nibble
        byte_addr = spd_base + SPD_OFFSET_DDR4_TRFC4MIN_MSB;
        trfc4_msb = *(uint8_t *)byte_addr;
        trfc4_msb = (trfc4_msb >> SPD_DDR4_TXX_MTB___S) & SPD_DDR4_TXX_MTB___M;

        byte_addr = spd_base + SPD_OFFSET_DDR4_TRFC4MIN_LSB;
        trfc4_lsb = *(uint8_t *)byte_addr;
        trfc4_lsb = (trfc4_lsb >> SPD_DDR4_TXX_MTB___S) & SPD_DDR4_TXX_MTB___M;

        trfc4_mtb = (trfc4_msb << 0x8) | trfc4_lsb;
        trfc4_ps = spd_get_txx_ps(trfc4_mtb, 0);
    }


    return trfc4_ps;
}



/*!****************************************************************************
*
* @par Description:
*    This function gets the RDIMM / LRDIMM driver output strength for the CKE signal
*    from the specified block of SPD data.
*
*                                  @param
*    spd_base        The base address of SPD data buffer to inspect.
*                                  @param
*    dev_type        Device type, DDR3 or DDR4.
*                                  @param
*    mod_type        Module type, RDIMM, UDIMM ....
*
*
* @return
*    The strength indicator value for the RDIMM / LRDIMM driver output strength for
*    the CKE signal. DDR4 Only! RDIMM_INVALID_DRIVE for DDR3.
*
******************************************************************************/
uint8_t spd_get_out_drv_ctl_cke(uint64_t spd_base, uint8_t dev_type, uint8_t mod_type)
{
    uint64_t byte_addr;
    uint8_t out_drv_ctl_cke = RDIMM_INVALID_DRIVE;


    if ((dev_type == SPD_DEVICE_TYPE_DDR4) &&
        ((mod_type == SPD_MODULE_TYPE_RDIMM) || (mod_type == SPD_MODULE_TYPE_DDR4_LRDIMM)))
    {
        byte_addr = spd_base + SPD_OFFSET_DDR4_RDIMM_LRDIMM_OUTPUT_DRIVE_CTL;
        out_drv_ctl_cke = *(uint8_t *)byte_addr;
        out_drv_ctl_cke = (out_drv_ctl_cke >> SPD_DDR4_OUT_DRV_CTL_CKE___S) & SPD_DDR4_OUT_DRV_STRENGTH___M;
    }

    return out_drv_ctl_cke;
}



/*!****************************************************************************
*
* @par Description:
*    This function gets the RDIMM / LRDIMM driver output strength for the ODT signal
*    from the specified block of SPD data.
*
*                                  @param
*    spd_base        The base address of SPD data buffer to inspect.
*                                  @param
*    dev_type        Device type, DDR3 or DDR4.
*                                  @param
*    mod_type        Module type, RDIMM, UDIMM ....
*
*
* @return
*    The strength indicator value for the RDIMM / LRDIMM driver output strength for
*    the ODT signal. DDR4 Only! RDIMM_INVALID_DRIVE for DDR3.
*
*
******************************************************************************/
uint8_t spd_get_out_drv_ctl_odt(uint64_t spd_base, uint8_t dev_type, uint8_t mod_type)
{
    uint64_t byte_addr;
    uint8_t out_drv_ctl_odt = RDIMM_INVALID_DRIVE;


    if ((dev_type == SPD_DEVICE_TYPE_DDR4) &&
        ((mod_type == SPD_MODULE_TYPE_RDIMM) || (mod_type == SPD_MODULE_TYPE_DDR4_LRDIMM)))
    {
        byte_addr = spd_base + SPD_OFFSET_DDR4_RDIMM_LRDIMM_OUTPUT_DRIVE_CTL;
        out_drv_ctl_odt = *(uint8_t *)byte_addr;
        out_drv_ctl_odt = (out_drv_ctl_odt >> SPD_DDR4_OUT_DRV_CTL_ODT___S) & SPD_DDR4_OUT_DRV_STRENGTH___M;
    }

    return out_drv_ctl_odt;
}



/*!****************************************************************************
*
* @par Description:
*    This function gets the RDIMM / LRDIMM driver output strength for the commands/addresses
*    signal from the specified block of SPD data.
*
*                                  @param
*    spd_base        The base address of SPD data buffer to inspect.
*                                  @param
*    dev_type        Device type, DDR3 or DDR4.
*                                  @param
*    mod_type        Module type type, RDIMM, UDIMM ....
*
*
* @return
*    The strength indicator value for the RDIMM / LRDIMM driver output strength for
*    the commands/addresses signal. DDR4 Only! RDIMM_INVALID_DRIVE for DDR3.
*
*
******************************************************************************/
uint8_t spd_get_out_drv_ctl_ca(uint64_t spd_base, uint8_t dev_type, uint8_t mod_type)
{
    uint64_t byte_addr;
    uint8_t out_drv_ctl_ca = RDIMM_INVALID_DRIVE;


    if ((dev_type == SPD_DEVICE_TYPE_DDR4) &&
        ((mod_type == SPD_MODULE_TYPE_RDIMM) || (mod_type == SPD_MODULE_TYPE_DDR4_LRDIMM)))
    {
        byte_addr = spd_base + SPD_OFFSET_DDR4_RDIMM_LRDIMM_OUTPUT_DRIVE_CTL;
        out_drv_ctl_ca = *(uint8_t *)byte_addr;
        out_drv_ctl_ca = (out_drv_ctl_ca >> SPD_DDR4_OUT_DRV_CTL_CA___S) & SPD_DDR4_OUT_DRV_STRENGTH___M;
    }

    return out_drv_ctl_ca;
}



/*!****************************************************************************
*
* @par Description:
*    This function gets the RDIMM / LRDIMM driver output strength for the chip select
*    signal from the specified block of SPD data.
*
*                                  @param
*    spd_base        The base address of SPD data buffer to inspect.
*                                  @param
*    dev_type        Device type, DDR3 or DDR4.
*                                  @param
*    mod_type        Module type type, RDIMM, UDIMM ....
*
*
* @return
*    The strength indicator value for the RDIMM / LRDIMM driver output strength for
*    the chip select signal. DDR4 Only! RDIMM_INVALID_DRIVE for DDR3.
*
*
******************************************************************************/
uint8_t spd_get_out_drv_ctl_cs(uint64_t spd_base, uint8_t dev_type, uint8_t mod_type)
{
    uint64_t byte_addr;
    uint8_t out_drv_ctl_cs = RDIMM_INVALID_DRIVE;


    if ((dev_type == SPD_DEVICE_TYPE_DDR4) &&
        ((mod_type == SPD_MODULE_TYPE_RDIMM) || (mod_type == SPD_MODULE_TYPE_DDR4_LRDIMM)))
    {
        byte_addr = spd_base + SPD_OFFSET_DDR4_RDIMM_LRDIMM_OUTPUT_DRIVE_CTL;
        out_drv_ctl_cs = *(uint8_t *)byte_addr;
        out_drv_ctl_cs = (out_drv_ctl_cs >> SPD_DDR4_OUT_DRV_CTL_CS___S) & SPD_DDR4_OUT_DRV_STRENGTH___M;
    }

    return out_drv_ctl_cs;
}



/*!****************************************************************************
*
* @par Description:
*    This function gets the memory type details based on information from the
*     SPD data.
*
*                                  @param
*    dev_type        Device type, DDR3 or DDR4. Unused for now but will likely
*                     be needed for future memory types.
*                                  @param
*    mod_type        Module type type, RDIMM, UDIMM ....
*
*
* @return
*    The device details bit-field.
*
*
******************************************************************************/
uint16_t spd_get_type_detail(UNUSED_PARAM uint8_t dev_type, uint8_t mod_type)
{
    uint16_t type_detail = MEM_ATTR_TYPE_DETAIL_SYNCHRONOUS;


    // UDIMMs and RDIMMs Both DIMM types would be Synchronous; RDIMMs would be registered and UDIMMs would be unregistered
    if (mod_type == SPD_MODULE_TYPE_RDIMM)
    {
        type_detail |= MEM_ATTR_TYPE_DETAIL_REGISTERED;
    }
    else if (mod_type == SPD_MODULE_TYPE_UDIMM)
    {
        type_detail |= MEM_ATTR_TYPE_DETAIL_UNBUFFERED;
    }
    else
    {
        type_detail |= MEM_ATTR_TYPE_DETAIL_UNKNOWN;
    }

    return type_detail;
}



/*!****************************************************************************
*
* @par Description:
*    This function gets the maximum clock speed, in MHz, supported by the DIMM
*     based on information from the SPD data. I.e. It takes the tck min psec
*     and converts that to clock rate.
*
*                                  @param
*    tck_min_ps      The SDRAM minimum cycle time in ps.
*
*
* @return
*    The maximum clock speed supported by the DIMM in MHz.
*
*
******************************************************************************/
uint16_t spd_get_max_clk_speed(uint32_t tck_min_ps)
{
    uint16_t max_clk_speed;


    max_clk_speed = SPD_PS_TO_MHZ(tck_min_ps);

    return max_clk_speed;
}



/*!****************************************************************************
*
* @par Description:
*    This function gets the DIMM manufacturer ID from the specified block of
*     SPD data.
*
*                                  @param
*    spd_base        The base address of SPD data buffer to inspect.
*                                  @param
*    dev_type        Device type, DDR3 or DDR4.
*
*
* @return
*    The manufacturer ID of the DIMM.
*
******************************************************************************/
uint16_t spd_get_manufacturer_id(uint64_t spd_base, uint8_t dev_type)
{
    uint8_t  manufacturer_id_msb=0;
    uint8_t  manufacturer_id_lsb=0;
    uint16_t manufacturer_id;


    if (dev_type == SPD_DEVICE_TYPE_DDR4)
    {
        manufacturer_id_msb = *(uint8_t *)(spd_base + SPD_OFFSET_DDR4_MFG_ID_MSB);
        manufacturer_id_lsb = *(uint8_t *)(spd_base + SPD_OFFSET_DDR4_MFG_ID_LSB);
    }

    manufacturer_id = (manufacturer_id_msb << 0x8) | manufacturer_id_lsb;

    return manufacturer_id;
}



/*!****************************************************************************
*
* @par Description:
*    This function gets the DIMM serial number from the specified block of
*     SPD data.
*
*                                  @param
*    spd_base        The base address of SPD data buffer to inspect.
*                                  @param
*    dev_type        Device type, DDR3 or DDR4.
*
*
* @return
*    The serial number of the DIMM.
*
******************************************************************************/
uint32_t spd_get_serial_number(uint64_t spd_base, uint8_t dev_type)
{
    uint8_t  serial_number_b0=0;
    uint8_t  serial_number_b1=0;
    uint8_t  serial_number_b2=0;
    uint8_t  serial_number_b3=0;
    uint32_t serial_number;


    if (dev_type == SPD_DEVICE_TYPE_DDR4)
    {
        serial_number_b0 = *(uint8_t *)(spd_base + SPD_OFFSET_DDR4_MFG_SN_0);
        serial_number_b1 = *(uint8_t *)(spd_base + SPD_OFFSET_DDR4_MFG_SN_1);
        serial_number_b2 = *(uint8_t *)(spd_base + SPD_OFFSET_DDR4_MFG_SN_2);
        serial_number_b3 = *(uint8_t *)(spd_base + SPD_OFFSET_DDR4_MFG_SN_3);
    }

    serial_number = (serial_number_b0 << 0x18) | (serial_number_b1 << 0x10) | (serial_number_b2 << 0x8) | serial_number_b3;

    return serial_number;
}



/*!****************************************************************************
*
* @par Description:
*    This function gets the DIMM part number from the specified block of
*     SPD data.
*
*                                  @param
*    spd_base        The base address of SPD data buffer to inspect.
*                                  @param
*    dev_type        Device type, DDR3 or DDR4.
*                                  @param
*    part_number     Part number byte array populated by this function.
*
*
* @return
*    None.
*
******************************************************************************/
void spd_get_part_number(uint64_t spd_base, uint8_t dev_type, uint8_t part_number[])
{
    uint64_t byte_addr=0;
    uint8_t byte_num;
    uint8_t byte_count=0;
    uint8_t space = 0x20;



    if (dev_type == SPD_DEVICE_TYPE_DDR4)
    {
        byte_count = SPD_MODULE_PART_NUM_LEN_DDR4;
        byte_addr = spd_base + SPD_OFFSET_DDR4_PART_NUM_LSB;
    }

    // Copy the bytes that make up the part number putting a space character
    //  in any unused locations in the destination array. DDR3 and DDR4 have
    //  different part lengths.
    for(byte_num = 0; byte_num < SPD_MODULE_PART_NUM_LEN; ++byte_num)
    {
        if(byte_num < byte_count)
        {
            part_number[byte_num] = *((uint8_t *)byte_addr + byte_num);
        }
        else
        {
            // Put a space character in any unused locations.
            part_number[byte_num] = space;
        }
    }

    // Add string termination.
    part_number[SPD_MODULE_PART_NUM_LEN_STR - 1] = 0;
}

/*!****************************************************************************
*
* @par Description:
*    This function calculates the a timing parameter in ps from a timing
*    parameter expressed in timing base form (MTB & FTB).
*
*                                  @param
*    txx_mtb        The MTB part of the SPD timing value.
*                                  @param
*    txx_ftb        The FTB part of the SPD timing value.
*
*
* @return
*    The txx timing value in ps.
*
******************************************************************************/
uint32_t spd_get_txx_ps(uint16_t txx_mtb, uint16_t txx_ftb)
{
    uint32_t txx_ps;
    int32_t txx_ftb_ps;


    // Use time base conversion derived from MTB and FTB common conversion
    //  factors extracted from the SPD.

    txx_ps = txx_mtb * medium_time_base_ps;

    // txx_ftb is in 2s compliment so just treat as signed.
    //  ftb is in femtoseconds to allow use of integers with little loss of accuracy.
    //   This is necessary since DDR3 supports fractional PS for ftb.

    if ((txx_ftb & 0x80) > 0)
    {
        txx_ftb_ps = -1 * ((((~txx_ftb & 0xFF) + 1) * fine_time_base_fs) / FEMTO_SEC_PER_PICO_SEC);
    }
    else
    {
        txx_ftb_ps = ((txx_ftb * fine_time_base_fs) / FEMTO_SEC_PER_PICO_SEC);
    }

    txx_ps = txx_ps + txx_ftb_ps;

    return txx_ps;
}


/*!****************************************************************************
*
* @par Description:
*    This function gets the LRDIMM clock driver & DB BCOM, BODT, BCKE signal output strength from
*    the specified block of SPD data.
*
*                                  @param
*    spd_base        The base address of SPD data buffer to inspect.
*                                  @param
*    dev_type        Device type, DDR3 or DDR4.
*                                  @param
*    mod_type        Module type, RDIMM, UDIMM ....
*
*
* @return
*    The strength indicator value for the LRDIMM clock driver & DB BCOM, BODT, BCKE signal output
*    strength indicator.
*
* @author
*    lchavani
*
******************************************************************************/
uint8_t spd_get_lrdm_db_drv_ck_bcom_bodt_bcke(uint64_t spd_base, uint8_t dev_type, uint8_t mod_type)
{
    uint64_t byte_addr;
    uint8_t lrdm_db_drv_ck_bcom_bodt_bcke = 0;

    if ((dev_type == SPD_DEVICE_TYPE_DDR4) && (mod_type == SPD_MODULE_TYPE_DDR4_LRDIMM))
    {
        byte_addr = spd_base + SPD_OFFSET_DDR4_RDIMM_LRDIMM_OUTPUT_DRIVE_CK;
        lrdm_db_drv_ck_bcom_bodt_bcke = *(uint8_t *)byte_addr;
        lrdm_db_drv_ck_bcom_bodt_bcke = (lrdm_db_drv_ck_bcom_bodt_bcke >> 4) & 0x01;
    }

    return lrdm_db_drv_ck_bcom_bodt_bcke;
}


/*!****************************************************************************
*
* @par Description:
*    This function gets the LRDIMM clock driver & DB BCK signal output strength from
*    the specified block of SPD data.
*
*                                  @param
*    spd_base        The base address of SPD data buffer to inspect.
*                                  @param
*    dev_type        Device type, DDR3 or DDR4.
*                                  @param
*    mod_type        Module type, RDIMM, UDIMM ....
*
*
* @return
*    The strength indicator value for the LRDIMM clock driver & DB BCK signal output
*    strength indicator.
*
******************************************************************************/
uint8_t spd_get_lrdm_db_drv_ck_bck(uint64_t spd_base, uint8_t dev_type, uint8_t mod_type)
{
    uint64_t byte_addr;
    uint8_t lrdm_db_drv_ck_bck = 0;

    if ((dev_type == SPD_DEVICE_TYPE_DDR4) && (mod_type == SPD_MODULE_TYPE_DDR4_LRDIMM))
    {
        byte_addr = spd_base + SPD_OFFSET_DDR4_RDIMM_LRDIMM_OUTPUT_DRIVE_CK;
        lrdm_db_drv_ck_bck = *(uint8_t *)byte_addr;
        lrdm_db_drv_ck_bck = (lrdm_db_drv_ck_bck >> 5) & 0x01;
    }

    return lrdm_db_drv_ck_bck;
}


/*!****************************************************************************
*
* @par Description:
*    This function gets the LRDIMM configuration information from the
*    specified block of SPD data.
*
*                                  @param
*    ddr_chan         DDR channel of the DIMM whose SPD DIMM configuration info
*                     is to be obtained.
*                                  @param
*    dimm_slot        Slot in the DDR channel of the DIMM whose SPD DIMM
*                     configuration info is to be obtained.
*                                  @param
*    spd_base         The base address of SPD data buffer to inspect.
*                                  @param
*    dev_type        Device type, DDR3 or DDR4.
*                                  @param
*    mod_type        Module type, RDIMM, UDIMM ....
*
*
* @return
*    None
*
******************************************************************************/
void spd_get_lrdimm_config(uint8_t ddr_chan, uint8_t dimm_slot, uint64_t spd_base, uint8_t dev_type, uint8_t mod_type)
{
    uint8_t rank;
    uint8_t rank_config;

    if ((dev_type == SPD_DEVICE_TYPE_DDR4) && (mod_type == SPD_MODULE_TYPE_DDR4_LRDIMM))
    {
        // mdq dfe
        lr_db_si_data[ddr_chan].lr_spd_db_si[dimm_slot].lr_spd_db_dq_dfe = spd_get_lr_db_dq_dfe(spd_base);

        // mdq gain adj
        lr_db_si_data[ddr_chan].lr_spd_db_si[dimm_slot].lr_spd_db_dq_gain_adj = spd_get_lr_db_dq_gain_adj(spd_base);

        // mdq ron
        lr_db_si_data[ddr_chan].lr_spd_db_si[dimm_slot].lr_spd_db_mdq_ron[0] = spd_get_lr_db_mdq_ron(spd_base, DDR_FREQ_IDX_933_MHZ);
        lr_db_si_data[ddr_chan].lr_spd_db_si[dimm_slot].lr_spd_db_mdq_ron[1] = spd_get_lr_db_mdq_ron(spd_base, DDR_FREQ_IDX_1200_MHZ);
        lr_db_si_data[ddr_chan].lr_spd_db_si[dimm_slot].lr_spd_db_mdq_ron[2] = spd_get_lr_db_mdq_ron(spd_base, DDR_FREQ_IDX_1600_MHZ);

        // mdq term (rtt)
        lr_db_si_data[ddr_chan].lr_spd_db_si[dimm_slot].lr_spd_db_mdq_term[0] = spd_get_lr_db_mdq_term(spd_base, DDR_FREQ_IDX_933_MHZ);
        lr_db_si_data[ddr_chan].lr_spd_db_si[dimm_slot].lr_spd_db_mdq_term[1] = spd_get_lr_db_mdq_term(spd_base, DDR_FREQ_IDX_1200_MHZ);
        lr_db_si_data[ddr_chan].lr_spd_db_si[dimm_slot].lr_spd_db_mdq_term[2] = spd_get_lr_db_mdq_term(spd_base, DDR_FREQ_IDX_1600_MHZ);

        // mdq vrefdq
        lr_db_si_data[ddr_chan].lr_spd_db_si[dimm_slot].lr_spd_db_mdq_vrefdq = spd_get_lr_db_mdq_vrefdq(spd_base);

        // mdq vrefdq range
        lr_db_si_data[ddr_chan].lr_spd_db_si[dimm_slot].lr_spd_db_mdq_vrefdq_range = spd_get_lr_db_mdq_vrefdq_range(spd_base);

        // rtt_park; R2/3 (b'5:3), R0/1 (b'2:0)
        rank_config = SPD_DDR4_LR_DB_RTT_PARK_RANK_0_1;
        lr_db_si_data[ddr_chan].lr_spd_db_si[dimm_slot].lr_spd_db_rtt_park01[0] = spd_get_lr_db_rtt_park(spd_base, rank_config, DDR_FREQ_IDX_933_MHZ);
        lr_db_si_data[ddr_chan].lr_spd_db_si[dimm_slot].lr_spd_db_rtt_park01[1] = spd_get_lr_db_rtt_park(spd_base, rank_config, DDR_FREQ_IDX_1200_MHZ);
        lr_db_si_data[ddr_chan].lr_spd_db_si[dimm_slot].lr_spd_db_rtt_park01[2] = spd_get_lr_db_rtt_park(spd_base, rank_config, DDR_FREQ_IDX_1600_MHZ);

        rank_config = SPD_DDR4_LR_DB_RTT_PARK_RANK_2_3;
        lr_db_si_data[ddr_chan].lr_spd_db_si[dimm_slot].lr_spd_db_rtt_park23[0] = spd_get_lr_db_rtt_park(spd_base, rank_config, DDR_FREQ_IDX_933_MHZ);
        lr_db_si_data[ddr_chan].lr_spd_db_si[dimm_slot].lr_spd_db_rtt_park23[1] = spd_get_lr_db_rtt_park(spd_base, rank_config, DDR_FREQ_IDX_1200_MHZ);
        lr_db_si_data[ddr_chan].lr_spd_db_si[dimm_slot].lr_spd_db_rtt_park23[2] = spd_get_lr_db_rtt_park(spd_base, rank_config, DDR_FREQ_IDX_1600_MHZ);

        // rtt_wr (b'5:3)
        lr_db_si_data[ddr_chan].lr_spd_db_si[dimm_slot].lr_spd_db_rtt_wr[0] = spd_get_lr_db_rtt_wr(spd_base, DDR_FREQ_IDX_933_MHZ);
        lr_db_si_data[ddr_chan].lr_spd_db_si[dimm_slot].lr_spd_db_rtt_wr[1] = spd_get_lr_db_rtt_wr(spd_base, DDR_FREQ_IDX_1200_MHZ);
        lr_db_si_data[ddr_chan].lr_spd_db_si[dimm_slot].lr_spd_db_rtt_wr[2] = spd_get_lr_db_rtt_wr(spd_base, DDR_FREQ_IDX_1600_MHZ);

        // rtt_nom (b'2:0)
        lr_db_si_data[ddr_chan].lr_spd_db_si[dimm_slot].lr_spd_db_rtt_nom[0] = spd_get_lr_db_rtt_nom(spd_base, DDR_FREQ_IDX_933_MHZ);
        lr_db_si_data[ddr_chan].lr_spd_db_si[dimm_slot].lr_spd_db_rtt_nom[1] = spd_get_lr_db_rtt_nom(spd_base, DDR_FREQ_IDX_1200_MHZ);
        lr_db_si_data[ddr_chan].lr_spd_db_si[dimm_slot].lr_spd_db_rtt_nom[2] = spd_get_lr_db_rtt_nom(spd_base, DDR_FREQ_IDX_1600_MHZ);

        // ron
        lr_db_si_data[ddr_chan].lr_spd_db_si[dimm_slot].lr_spd_db_ron[0] = spd_get_lr_db_ron(spd_base, DDR_FREQ_IDX_933_MHZ);
        lr_db_si_data[ddr_chan].lr_spd_db_si[dimm_slot].lr_spd_db_ron[1] = spd_get_lr_db_ron(spd_base, DDR_FREQ_IDX_1200_MHZ);
        lr_db_si_data[ddr_chan].lr_spd_db_si[dimm_slot].lr_spd_db_ron[2] = spd_get_lr_db_ron(spd_base, DDR_FREQ_IDX_1600_MHZ);

        // dram vrefdq and vrefdq range
        for (rank = 0; rank < DDR_MAX_RANKS_PER_SLOT; ++rank)
        {
            lr_db_si_data[ddr_chan].lr_spd_db_si[dimm_slot].lr_spd_db_vrefdq[rank] = spd_get_lr_db_vrefdq(spd_base, rank);
            lr_db_si_data[ddr_chan].lr_spd_db_si[dimm_slot].lr_spd_db_vrefdq_range[rank] = spd_get_lr_db_vrefdq_range(spd_base, rank);
        }

    }
}


/*!****************************************************************************
*
* @par Description:
*    This function gets the LRDIMM DB DQ DFE capabilities - feedback equalization support
*    from the specified block of SPD data.
*
*                                  @param
*    spd_base        The base address of SPD data buffer to inspect.
*
*
* @return
*    LRDIMM DB DQ DFE - feedback equalization support
*
******************************************************************************/
uint8_t spd_get_lr_db_dq_dfe(uint64_t spd_base)
{
    uint64_t byte_addr;
    uint8_t lr_spd_db_dq_dfe;

    byte_addr = spd_base + SPD_OFFSET_DDR4_LRDIMM_DB_DQ_DFE;
    lr_spd_db_dq_dfe = *(uint8_t *)byte_addr;
    lr_spd_db_dq_dfe = (lr_spd_db_dq_dfe >> SPD_DDR4_LR_DB_DQ_DFE___S) & SPD_DDR4_LR_DB_DQ_DFE___M;

    return lr_spd_db_dq_dfe;
}


/*!****************************************************************************
*
* @par Description:
*    This function gets the LRDIMM DB DQ DFE capabilities - gain adjustment support
*    from the specified block of SPD data.
*
*                                  @param
*    spd_base        The base address of SPD data buffer to inspect.
*
*
* @return
*    LRDIMM DB DQ DFE - gain adjustment support
*
******************************************************************************/
uint8_t spd_get_lr_db_dq_gain_adj(uint64_t spd_base)
{
    uint64_t byte_addr;
    uint8_t lr_spd_db_dq_gain_adj;

    byte_addr = spd_base + SPD_OFFSET_DDR4_LRDIMM_DB_DQ_DFE;
    lr_spd_db_dq_gain_adj = *(uint8_t *)byte_addr;
    lr_spd_db_dq_gain_adj = (lr_spd_db_dq_gain_adj >> SPD_DDR4_LR_DB_DQ_GAIN_ADJ___S) & SPD_DDR4_LR_DB_DQ_DFE___M;

    return lr_spd_db_dq_gain_adj;
}


/*!****************************************************************************
*
* @par Description:
*    This function gets the LRDIMM DB drive strength for MDQ/MDQS outputs
*    from the specified block of SPD data.
*
*                                  @param
*    spd_base        The base address of SPD data buffer to inspect.
*
*
* @return
*    LRDIMM DB drive strength for MDQ/MDQS outputs
*
******************************************************************************/
uint8_t spd_get_lr_db_mdq_ron(uint64_t spd_base, uint8_t ddr_freq_idx)
{
    uint64_t byte_addr = 0x0;
    uint8_t lr_db_mdq_ron;

    if (ddr_freq_idx == DDR_FREQ_IDX_933_MHZ)
    {
        byte_addr = spd_base + SPD_OFFSET_DDR4_LRDIMM_DB_RON_RTT_1866;
    }
    else if (ddr_freq_idx == DDR_FREQ_IDX_1200_MHZ)
    {
        byte_addr = spd_base + SPD_OFFSET_DDR4_LRDIMM_DB_RON_RTT_2400;
    }
    else // if (ddr_freq_idx == DDR_FREQ_IDX_1600_MHZ)
    {
        byte_addr = spd_base + SPD_OFFSET_DDR4_LRDIMM_DB_RON_RTT_3200;
    }

    lr_db_mdq_ron = *(uint8_t *)byte_addr;
    lr_db_mdq_ron = (lr_db_mdq_ron >> SPD_DDR4_LR_DB_MDQ_RON___S) & SPD_DDR4_LR_DB_MDQ_RON_RTT___M;

    return lr_db_mdq_ron;
}


/*!****************************************************************************
*
* @par Description:
*    This function gets the LRDIMM DB Read RTT termination strength
*    from the specified block of SPD data.
*
*                                  @param
*    spd_base        The base address of SPD data buffer to inspect.
*
*
* @return
*    LRDIMM DB Read RTT termination strength
*
******************************************************************************/
uint8_t spd_get_lr_db_mdq_term(uint64_t spd_base, uint8_t ddr_freq_idx)
{
    uint64_t byte_addr = 0x0;
    uint8_t lr_db_mdq_term;

    if (ddr_freq_idx == DDR_FREQ_IDX_933_MHZ)
    {
        byte_addr = spd_base + SPD_OFFSET_DDR4_LRDIMM_DB_RON_RTT_1866;
    }
    else if (ddr_freq_idx == DDR_FREQ_IDX_1200_MHZ)
    {
        byte_addr = spd_base + SPD_OFFSET_DDR4_LRDIMM_DB_RON_RTT_2400;
    }
    else // if (ddr_freq_idx == DDR_FREQ_IDX_1600_MHZ)
    {
        byte_addr = spd_base + SPD_OFFSET_DDR4_LRDIMM_DB_RON_RTT_3200;
    }

    lr_db_mdq_term = *(uint8_t *)byte_addr;
    lr_db_mdq_term = (lr_db_mdq_term >> SPD_DDR4_LR_DB_MDQ_TERM___S) & SPD_DDR4_LR_DB_MDQ_RON_RTT___M;

    return lr_db_mdq_term;
}


/*!****************************************************************************
*
* @par Description:
*    This function gets the LRDIMM DB VrefDQ
*    from the specified block of SPD data.
*
*                                  @param
*    spd_base        The base address of SPD data buffer to inspect.
*
*
* @return
*    LRDIMM DB VrefDQ
*
******************************************************************************/
uint8_t spd_get_lr_db_mdq_vrefdq(uint64_t spd_base)
{
    uint64_t byte_addr;
    uint8_t lr_spd_db_mdq_vrefdq;

    byte_addr = spd_base + SPD_OFFSET_DDR4_LRDIMM_DB_VREFDQ;
    lr_spd_db_mdq_vrefdq = *(uint8_t *)byte_addr;
    lr_spd_db_mdq_vrefdq = (lr_spd_db_mdq_vrefdq >> SPD_DDR4_LR_DB_MDQ_VREFDQ___S) & SPD_DDR4_LR_DB_MDQ_VREFDQ___M;

    return lr_spd_db_mdq_vrefdq;
}


/*!****************************************************************************
*
* @par Description:
*    This function gets the LRDIMM DB VrefDQ range
*    from the specified block of SPD data.
*
*                                  @param
*    spd_base        The base address of SPD data buffer to inspect.
*
*
* @return
*    LRDIMM DB VrefDQ range
*
******************************************************************************/
uint8_t spd_get_lr_db_mdq_vrefdq_range(uint64_t spd_base)
{
    uint64_t byte_addr;
    uint8_t lr_spd_db_mdq_vrefdq_range;

    byte_addr = spd_base + SPD_OFFSET_DDR4_LRDIMM_DB_DRAM_VREFDQ_RANGE;
    lr_spd_db_mdq_vrefdq_range = *(uint8_t *)byte_addr;
    lr_spd_db_mdq_vrefdq_range = (lr_spd_db_mdq_vrefdq_range >> SPD_DDR4_LR_DB_MDQ_VREFDQ_RANGE___S) &
                                     SPD_DDR4_LR_DB_VREFDQ_RANGE___M;

    return lr_spd_db_mdq_vrefdq_range;
}


/*!****************************************************************************
*
* @par Description:
*    This function gets the LRDIMM DRAM ODT termination strength (rtt_park)
*    from the specified block of SPD data.
*
*                                  @param
*    spd_base        The base address of SPD data buffer to inspect.
*                                  @param
*    rank_config     Ranks 0,1 or Ranks 2,3.
*                                  @param
*    ddr_freq_idx    Frequency index based on DDR operating speed SPD variants.
*
*
* @return
*    LRDIMM DRAM ODT termination strength (rtt_park)
*
******************************************************************************/
uint8_t spd_get_lr_db_rtt_park(uint64_t spd_base, uint8_t rank_config, uint8_t ddr_freq_idx)
{
    uint64_t byte_addr = 0x0;
    uint8_t lr_db_rtt_park;

    if (ddr_freq_idx == DDR_FREQ_IDX_933_MHZ)
    {
        byte_addr = spd_base + SPD_OFFSET_DDR4_LRDIMM_RTT_PARK_1866;
    }
    else if (ddr_freq_idx == DDR_FREQ_IDX_1200_MHZ)
    {
        byte_addr = spd_base + SPD_OFFSET_DDR4_LRDIMM_RTT_PARK_2400;
    }
    else // if (ddr_freq_idx == DDR_FREQ_IDX_1600_MHZ)
    {
        byte_addr = spd_base + SPD_OFFSET_DDR4_LRDIMM_RTT_PARK_3200;
    }

    lr_db_rtt_park = *(uint8_t *)byte_addr;

    if (rank_config == SPD_DDR4_LR_DB_RTT_PARK_RANK_0_1)
    {
        lr_db_rtt_park = (lr_db_rtt_park >> SPD_DDR4_LR_DB_RTT_PARK01___S) & SPD_DDR4_LR_DB_RTT_PARK___M;
    }
    else if (rank_config == SPD_DDR4_LR_DB_RTT_PARK_RANK_2_3)
    {
        lr_db_rtt_park = (lr_db_rtt_park >> SPD_DDR4_LR_DB_RTT_PARK23___S) & SPD_DDR4_LR_DB_RTT_PARK___M;
    }

    return lr_db_rtt_park;
}


/*!****************************************************************************
*
* @par Description:
*    This function gets the LRDIMM DRAM ODT termination strength (rtt_wr)
*    from the specified block of SPD data.
*
*                                  @param
*    spd_base        The base address of SPD data buffer to inspect.
*                                  @param
*    ddr_freq_idx    Frequency index based on DDR operating speed SPD variants.
*
*
* @return
*    LRDIMM DRAM ODT termination strength (rtt_wr)
*
******************************************************************************/
uint8_t spd_get_lr_db_rtt_wr(uint64_t spd_base, uint8_t ddr_freq_idx)
{
    uint64_t byte_addr = 0x0;
    uint8_t lr_db_rtt_wr;

    if (ddr_freq_idx == DDR_FREQ_IDX_933_MHZ)
    {
        byte_addr = spd_base + SPD_OFFSET_DDR4_LRDIMM_RTT_WR_NOM_1866;
    }
    else if (ddr_freq_idx == DDR_FREQ_IDX_1200_MHZ)
    {
        byte_addr = spd_base + SPD_OFFSET_DDR4_LRDIMM_RTT_WR_NOM_2400;
    }
    else // if (ddr_freq_idx == DDR_FREQ_IDX_1600_MHZ)
    {
        byte_addr = spd_base + SPD_OFFSET_DDR4_LRDIMM_RTT_WR_NOM_3200;
    }

    lr_db_rtt_wr = *(uint8_t *)byte_addr;
    lr_db_rtt_wr = (lr_db_rtt_wr >> SPD_DDR4_LR_DB_RTT_WR___S) & SPD_DDR4_LR_DB_RTT_WR_NOM___M;

    return lr_db_rtt_wr;
}


/*!****************************************************************************
*
* @par Description:
*    This function gets the LRDIMM DRAM ODT termination strength (rtt_nom)
*    from the specified block of SPD data.
*
*                                  @param
*    spd_base        The base address of SPD data buffer to inspect.
*                                  @param
*    ddr_freq_idx    Frequency index based on DDR operating speed SPD variants.
*
*
* @return
*    LRDIMM DRAM ODT termination strength (rtt_nom)
*
******************************************************************************/
uint8_t spd_get_lr_db_rtt_nom(uint64_t spd_base, uint8_t ddr_freq_idx)
{
    uint64_t byte_addr = 0x0;
    uint8_t lr_db_rtt_nom;

    if (ddr_freq_idx == DDR_FREQ_IDX_933_MHZ)
    {
        byte_addr = spd_base + SPD_OFFSET_DDR4_LRDIMM_RTT_WR_NOM_1866;
    }
    else if (ddr_freq_idx == DDR_FREQ_IDX_1200_MHZ)
    {
        byte_addr = spd_base + SPD_OFFSET_DDR4_LRDIMM_RTT_WR_NOM_2400;
    }
    else // if (ddr_freq_idx == DDR_FREQ_IDX_1600_MHZ)
    {
        byte_addr = spd_base + SPD_OFFSET_DDR4_LRDIMM_RTT_WR_NOM_3200;
    }

    lr_db_rtt_nom = *(uint8_t *)byte_addr;
    lr_db_rtt_nom = (lr_db_rtt_nom >> SPD_DDR4_LR_DB_RTT_NOM___S) & SPD_DDR4_LR_DB_RTT_WR_NOM___M;

    return lr_db_rtt_nom;
}


/*!****************************************************************************
*
* @par Description:
*    This function gets the LRDIMM DRAM output buffer drive strength
*    from the specified block of SPD data.
*
*                                  @param
*    spd_base        The base address of SPD data buffer to inspect.
*                                  @param
*    ddr_freq_idx    Frequency index based on DDR operating speed SPD variants.
*
*
* @return
*    LRDIMM DRAM output buffer drive strength
*
******************************************************************************/
uint8_t spd_get_lr_db_ron(uint64_t spd_base, uint8_t ddr_freq_idx)
{
    uint64_t byte_addr = 0x0;
    uint8_t lr_db_ron;

    byte_addr = spd_base + SPD_OFFSET_DDR4_LRDIMM_RON;
    lr_db_ron = *(uint8_t *)byte_addr;

    if (ddr_freq_idx == DDR_FREQ_IDX_933_MHZ)
    {
        lr_db_ron = (lr_db_ron >> SPD_DDR4_LR_DB_RON_1866___S) & SPD_DDR4_LR_DB_RON___M;
    }
    else if (ddr_freq_idx == DDR_FREQ_IDX_1200_MHZ)
    {
        lr_db_ron = (lr_db_ron >> SPD_DDR4_LR_DB_RON_2400___S) & SPD_DDR4_LR_DB_RON___M;
    }
    else if (ddr_freq_idx == DDR_FREQ_IDX_1600_MHZ)
    {
        lr_db_ron = (lr_db_ron >> SPD_DDR4_LR_DB_RON_3200___S) & SPD_DDR4_LR_DB_RON___M;
    }

    return lr_db_ron;
}


/*!****************************************************************************
*
* @par Description:
*    This function gets the LRDIMM DRAM VrefDQ for package rank 0-3
*    from the specified block of SPD data.
*
*                                  @param
*    spd_base        The base address of SPD data buffer to inspect.
*                                  @param
*    rank            Rank for which the VrefDQ SPD info needs to be obtained.
*
*
* @return
*    LRDIMM DRAM VrefDQ for package rank 0-3
*
******************************************************************************/
uint8_t spd_get_lr_db_vrefdq(uint64_t spd_base, uint8_t rank)
{
    uint64_t byte_addr;
    uint8_t spd_offset;
    uint8_t lr_spd_db_vrefdq;

    spd_offset = SPD_OFFSET_DDR4_LRDIMM_VREFDQ_R0 + rank;
    byte_addr = spd_base + spd_offset;
    lr_spd_db_vrefdq = *(uint8_t *)byte_addr;
    lr_spd_db_vrefdq = lr_spd_db_vrefdq & SPD_DDR4_LR_DB_VREFDQ___M;

    return lr_spd_db_vrefdq;
}


/*!****************************************************************************
*
* @par Description:
*    This function gets the LRDIMM DRAM VrefDQ range
*    from the specified block of SPD data.
*
*                                  @param
*    spd_base        The base address of SPD data buffer to inspect.
*                                  @param
*    rank            Rank for which the VrefDQ SPD info needs to be obtained.
*
*
* @return
*    LRDIMM DRAM VrefDQ range
*
******************************************************************************/
uint8_t spd_get_lr_db_vrefdq_range(uint64_t spd_base, uint8_t rank)
{
    uint64_t byte_addr;
    uint8_t lr_spd_db_vrefdq_range;

    byte_addr = spd_base + SPD_OFFSET_DDR4_LRDIMM_DB_DRAM_VREFDQ_RANGE;
    lr_spd_db_vrefdq_range = *(uint8_t *)byte_addr;
    lr_spd_db_vrefdq_range = (lr_spd_db_vrefdq_range >> rank) & SPD_DDR4_LR_DB_VREFDQ_RANGE___M;

    return lr_spd_db_vrefdq_range;
}
