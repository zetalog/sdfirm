/*!****************************************************************************
* @file mddr_init.c
*
* @par Description:
*    This module provides the top level functionality to initialize the  DDR subsystem.
*    Includes MDDR common utilities that are to be used across MDDR subcomponents.
*
******************************************************************************/
/******************************************************************************
* Included Headers
******************************************************************************/
#include  "mddr.h"
#include  "mddr_init.h"
#include  "spd_smbus.h"
#include  "mddr_defs.h"
#include  "ddr_driver.h"

/******************************************************************************
* Defined Constants
******************************************************************************/


/******************************************************************************
* Derived data types
******************************************************************************/
/******************************************************************************
* Global Variables
******************************************************************************/

bool enhanced_training = true;
mddr_settings_t mddr_settings;
uint8_t spd_scratch_pad[SPD_MAX_SERIAL_EEPROM_SIZE];
spd_dimm_config_t mddr_dimm_configs[DDR_MAX_NUM_CHANS][DDR_MAX_NUM_SLOTS_PER_CHAN];
mddr_common_cfg_t common_cfg;
mddr_timing_values_t mddr_timing_vals;
mddr_latencies_t latencies;
uint8_t num_chans_poped = 0;
uint8_t num_dimms_per_chan_poped = 0;
uint32_t mddr_valid_ranks;
uint32_t mem_freq_mhz;
uint32_t ddr_rate;
uint32_t mem_freq_ps;
uint32_t mddr_xo_freq_khz;

bool reg_report = false;
target_mode_e platform_mode;

mddr_lr_db_si_chan_t lr_db_si_data[DDR_MAX_NUM_CHANS];

/*!****************************************************************************
*
* @par Description
*    Structure used to collect platform specific SI adjustments for each DDR
*    channel. Pre-initialized with data for SOD single DIMM @1866 MTS or less.
*
******************************************************************************/
mddr_si_chan_t si_data[DDR_MAX_NUM_CHANS] =
{
    // Chan 0
    {
        {0, 0}, {34, 34}, 60, 7475,
        {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x2, 0x0, 0x0, 0x0, 0x5, 0x0, 0x10, 0x10},
        {
            // DIMM 0
            {
                {150, 150, 150, 150},     // ibt_ca/cs/cke/odt
                {
                    // DIMM 0 - Rank 0
                    {
                        0, 80, 0, 34, 0, 0,
                        {7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750}
                    },

                    // DIMM 0 - Rank 1
                    {
                        0, 80, 0, 34, 0, 0,
                        {7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750}
                    },

                    // DIMM 0 - Rank 2
                    {
                        0, 80, 0, 34, 0, 0,
                        {7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750}
                    },

                    // DIMM 0 - Rank 3
                    {
                        0, 80, 0, 34, 0, 0,
                        {7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750}
                    }
                }
            },


            // DIMM 1
            {
                {150, 150, 150, 150},     // ibt_ca/cs/cke/odt
                {
                    // DIMM 0 - Rank 0
                    {
                        0, 80, 0, 34, 0, 0,
                        {7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750}
                    },

                    // DIMM 0 - Rank 1
                    {
                        0, 80, 0, 34, 0, 0,
                        {7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750}
                    },

                    // DIMM 0 - Rank 2
                    {
                        0, 80, 0, 34, 0, 0,
                        {7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750}
                    },

                    // DIMM 0 - Rank 3
                    {
                        0, 80, 0, 34, 0, 0,
                        {7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750}
                    }
                }
            }
        }
    },

    // Chan 1
    {
        {0, 0}, {34, 34}, 60, 7475,
        {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x2, 0x0, 0x0, 0x0, 0x5, 0x0, 0x10, 0x10},
        {
            // DIMM 0
            {
                {150, 150, 150, 150},     // ibt_ca/cs/cke/odt
                {
                    // DIMM 0 - Rank 0
                    {
                        0, 80, 0, 34, 0, 0,
                        {7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750}
                    },

                    // DIMM 0 - Rank 1
                    {
                        0, 80, 0, 34, 0, 0,
                        {7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750}
                    },

                    // DIMM 0 - Rank 2
                    {
                        0, 80, 0, 34, 0, 0,
                        {7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750}
                    },

                    // DIMM 0 - Rank 3
                    {
                        0, 80, 0, 34, 0, 0,
                        {7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750}
                    }
                }
            },


            // DIMM 1
            {
                {150, 150, 150, 150},     // ibt_ca/cs/cke/odt
                {
                    // DIMM 0 - Rank 0
                    {
                        0, 80, 0, 34, 0, 0,
                        {7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750}
                    },

                    // DIMM 0 - Rank 1
                    {
                        0, 80, 0, 34, 0, 0,
                        {7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750}
                    },

                    // DIMM 0 - Rank 2
                    {
                        0, 80, 0, 34, 0, 0,
                        {7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750}
                    },

                    // DIMM 0 - Rank 3
                    {
                        0, 80, 0, 34, 0, 0,
                        {7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750,7750}
                    }
                }
            }
        }
    }
};

/*!****************************************************************************
*
* @par Description
*    Array used to associate the HDDRSS base addresses to the DDR channels.
*    The array is indexed by channel number.
*
* @par Notes:
*    A logical DDR channel reflects the default address walk order found
*    in the Monaco bus. Each logical channel maps to a single MCSU and
*    HDDRSS.
*
* @verbatim
*    Logical     MCSU        HDDRSS     Switch
*    Channel    Instance    Instance      ID      Interleave
*    -------    --------    --------    ------    ----------
*      0           0           2         0x00       Even
*      1           1           3         0x04       Odd
* @endverbatim
*
******************************************************************************/
uint64_t mddr_base_addrs[DDR_MAX_NUM_CHANS] = {
                                MDDR_HDDRSS2_ADDR,
                                MDDR_HDDRSS3_ADDR

                           };

uint8_t hddrss[DDR_MAX_NUM_CHANS] = {
                                0,
                                1,
                          };


/******************************************************************************
* Macros
******************************************************************************/

/******************************************************************************
* Function Prototypes
******************************************************************************/

unsigned mddr_validate_input(mddr_settings_t *mddr_settings_ptr);
void mddr_get_dimm_configs(void);
unsigned mddr_verify_dimm_combination_configs(mddr_settings_t *mddr_settings_ptr);
unsigned mddr_verify_dimms(mddr_settings_t *mddr_settings_ptr);
void mddr_get_combined_timing_vals(uint32_t mem_freq_ps);
void mddr_get_latency_vals(mddr_common_cfg_t *common_cfg_ptr, mddr_settings_t *mddr_settings_ptr, mddr_latencies_t *latencies_ptr);
uint32_t mddr_get_clock_period_timing_val(void);
uint32_t mddr_get_derived_txs_timing_val(uint32_t trfc_min_ps);
uint32_t mddr_get_derived_txsdll_timing_val(void);
uint32_t mddr_get_derived_tdllkmin_timing_val(void);
uint32_t mddr_get_derived_tstab_timing_val(void);
uint32_t mddr_get_derived_tckevmin_timing_val(void);
uint16_t mddr_get_dimms_max_clk(void);
void mddr_record_init_parms(mddr_settings_t *settings_ptr);
uint32_t mddr_get_cas_latency(   mddr_common_cfg_t *common_cfg_ptr);
uint32_t max_of_clk_ps(uint32_t i_clks, uint32_t i_ps, uint32_t freq_ps);
uint32_t mddr_get_num_clocks(uint32_t txx_ps, uint32_t freq_ps);
void mddr_set_reg(uint64_t mddr_reg_addr, char *reg_name_str, uint32_t reg_val);
uint32_t mddr_get_reg(uint64_t mddr_reg_addr, char *reg_name_str);
void mddr_rmw_reg(uint64_t mddr_reg_addr, char *reg_name_str, uint32_t mask, uint32_t set_bits);
void mddr_rd_chk_reg(uint64_t mddr_base_addr, uint64_t s_addr_offset, uint64_t mask, uint32_t value, uint32_t to_ms);
void mddr_wait_for_clear(uint64_t mddr_base_addr, uint64_t mddr_reg_addr, uint32_t reg_mask, uint8_t max_loops);
void mddr_wait_for_set(uint64_t mddr_base_addr, uint64_t mddr_reg_addr, uint32_t reg_mask, uint8_t max_loops);
bool mddr_check_chan_populated(uint8_t ddr_chan);
uint8_t mddr_get_dimm_per_chan_cnt(void);


/******************************************************************************
* External Functions
******************************************************************************/




/*!****************************************************************************
*
* @par Description:
*    This function initializes  the memory controllers found in the Monaco
*    DDR subsystem based on SPD information extracted from the populated DIMMs
*    and specified configuration options
*
*                                  @param
*    mode               Mode of the target being executed on. Used to provide
*                        simulated SPD for Emulation and simulation environments.
*                        [SILICON|RUMI42|RUMI48|SIM|SIM_RUMI42|SIM_RUMI48]
*                                  @param
*    soc_hw_version     Provides the HW version of the target SoC.
*                                  @param
*    mddr_settings_ptr  The pointer to the structure containing the
*                        settings information.
*
*
* @return
*    The status indicating the level of success in initializing the memory controllers.
*
*
******************************************************************************/
unsigned mddr_init(target_mode_e mode,  mddr_settings_t *mddr_settings_ptr)
{
    unsigned status = COM_OK;
    uint8_t ddr_chan;
    uint16_t max_1dpc_freq_hw = MAX_1DPC_CLK;    // Max clock for 1DIMM per ch poped supported by hardware
    uint16_t max_2dpc_freq_hw = MAX_2DPC_CLK;    // Max clock for 2DIMMs per ch poped supported by hardware
    uint16_t max_dimm_freq;

    // Check for invalid MDDR settings input arguments.  Invalid combinations that
    //  result in overrides will be checked later.
    status |= mddr_validate_input(mddr_settings_ptr);

    // Get the DIMM configuration info from SPD
    mddr_get_dimm_configs();

    num_dimms_per_chan_poped = mddr_get_dimm_per_chan_cnt();

    // Determine the DDR frequency to be used
    if (num_dimms_per_chan_poped == 1)
    {
        // Choose the minimum of software DDR clock max and hardware supported DDR clock max
        if (mddr_settings_ptr->max_1dimm_freq_mhz > max_1dpc_freq_hw)
        {
            mddr_log(CRITICAL,"\n^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n");
            mddr_log(CRITICAL,"ADJUST: 1 DIMM per channel populated, software max DDR Clock reduced to %d MHz.\n", max_1dpc_freq_hw);
            mddr_log(CRITICAL,"^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n\n");

            mddr_settings_ptr->max_1dimm_freq_mhz = max_1dpc_freq_hw;
        }

        // Choose the minimum of desired DDR clock and software DDR clock max
        if (mddr_settings_ptr->mem_freq_mhz[DDR_FREQ_1] > mddr_settings_ptr->max_1dimm_freq_mhz)
        {
            mddr_log(CRITICAL,"\n^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n");
            mddr_log(CRITICAL,"ADJUST: 1 DIMM per channel populated, desired DDR Clock (%d MHz) exceeds software max DDR Clock (%d MHz) \n",
                                  mddr_settings_ptr->mem_freq_mhz[DDR_FREQ_1], mddr_settings_ptr->max_1dimm_freq_mhz);
            mddr_log(CRITICAL,"DDR Clock reduced to %d MHz.\n", mddr_settings_ptr->max_1dimm_freq_mhz);
            mddr_log(CRITICAL,"^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n\n");

            mddr_settings_ptr->mem_freq_mhz[DDR_FREQ_1] = mddr_settings_ptr->max_1dimm_freq_mhz;
        }
    }
    else if (num_dimms_per_chan_poped > 1)
    {
        // Choose the minimum of software DDR clock max and hardware supported DDR clock max
        if (mddr_settings_ptr->max_2dimm_freq_mhz > max_2dpc_freq_hw)
        {
            mddr_log(CRITICAL,"\n^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n");
            mddr_log(CRITICAL,"ADJUST: 2 DIMMs per channel populated, software max DDR Clock reduced to %d MHz.\n", max_2dpc_freq_hw);
            mddr_log(CRITICAL,"^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n\n");

            mddr_settings_ptr->max_2dimm_freq_mhz = max_2dpc_freq_hw;
        }

        // Choose the minimum of desired DDR clock and software DDR clock max
        if (mddr_settings_ptr->mem_freq_mhz[DDR_FREQ_1] > mddr_settings_ptr->max_2dimm_freq_mhz)
        {
            mddr_log(CRITICAL,"\n^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n");
            mddr_log(CRITICAL,"ADJUST: 2 DIMMs per channel populated, desired DDR Clock (%d MHz) exceeds software max DDR Clock (%d MHz) \n",
                                 mddr_settings_ptr->mem_freq_mhz[DDR_FREQ_1], mddr_settings_ptr->max_2dimm_freq_mhz);
            mddr_log(CRITICAL,"DDR Clock reduced to %d MHz.\n", mddr_settings_ptr->max_2dimm_freq_mhz);
            mddr_log(CRITICAL,"^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n\n");

            mddr_settings_ptr->mem_freq_mhz[DDR_FREQ_1] = mddr_settings_ptr->max_2dimm_freq_mhz;
        }
    }


    // Get the greatest common DIMM clock rate supported.
     max_dimm_freq = mddr_get_dimms_max_clk();

    // Downshift the frequency to the highest common clock rate supported
    //  by the populated DIMMs as long as the overclocking override has not
    //  been enabled.
    if((mddr_settings_ptr->mem_freq_mhz[DDR_FREQ_1] > max_dimm_freq) &&
       (!mddr_settings_ptr->allow_overclocking))
    {
        // The speed grade for the populated DIMMs is below the requested frequency so
        //  drop the overall DDR clock rate to the highest supported by all DIMMs.
        mddr_log(CRITICAL,"\n^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n");
        mddr_log(CRITICAL,"ADJUST: DIMMs speed grade below requested frequency. DDR Clock reduced to %d MHz.\n", max_dimm_freq);
        mddr_log(CRITICAL,"^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n\n");
        mddr_settings_ptr->mem_freq_mhz[DDR_FREQ_1] = max_dimm_freq;
    }

    // Use the first frequency for now since dynamic frequency switch is not supported yet.
    mem_freq_mhz = mddr_settings_ptr->mem_freq_mhz[DDR_FREQ_1];

    // Set up the other top level frequency parameters
    mddr_xo_freq_khz = mddr_settings_ptr->mddr_xo_freq_khz;
    mem_freq_ps = MDDR_MHZ_TO_PS(mem_freq_mhz);
    ddr_rate = mem_freq_mhz * 2;

    status |= mddr_verify_dimm_combination_configs(mddr_settings_ptr);


    // All the 2TCK related overides are to be done here before we obtain the SI since LRDIMM gap spacings
    // are dependant on 2TCK
    if(ddr_rate < 2400)
    {
       if (mddr_settings_ptr->ddr4_2t_preamble_mode != DDR4_2T_DISABLE)
        {
           mddr_log(HIGH, "Ignoring write/read 2T preamble feature since data rate<2400, data rate = %d\n", ddr_rate);
           // write/read 2tCK preamble (0=disable, 1=2T read, 2=2T write, 3=2T write & read)
           mddr_settings_ptr->ddr4_2t_preamble_mode = DDR4_2T_DISABLE;
        }
    }
    else
    {
        if (mddr_settings_ptr->ddr4_2t_preamble_mode == DDR4_2T_DISABLE)
        {
            mddr_log(HIGH,"\nSetting DDR4 2T Preamble to READ configuration since data rate>=2400, data rate = %d\n", ddr_rate);
            mddr_settings_ptr->ddr4_2t_preamble_mode = DDR4_2T_READ;
        }
        else if (mddr_settings_ptr->ddr4_2t_preamble_mode == DDR4_2T_WRITE)
        {
            mddr_log(HIGH,"\nSetting DDR4 2T Preamble to WRITE_READ configuration since data rate>=2400, data rate = %d\n", ddr_rate);
            mddr_settings_ptr->ddr4_2t_preamble_mode = DDR4_2T_WRITE_READ;
        }
     }

    if(status == COM_OK)
    {
        // Force disable DDR4 modes that are frequency dependent.
        //  Table 106, pg 204 JEDEC 79-4A
        if(common_cfg.dev_type == SPD_DEVICE_TYPE_DDR4)
        {
            if(ddr_rate < 2666)
            {
                if (mddr_settings_ptr->ddr4_wr_crc_en)
                {
                    mddr_log(HIGH, "Ignoring write CRC enable feature since data rate<2666, data rate = %d\n", ddr_rate);
                    mddr_settings_ptr->ddr4_wr_crc_en = false;
                }

                if (mddr_settings_ptr->ddr4_geardown_mode_en)
                {
                    mddr_log(HIGH, "Ignoring geardown mode since data rate<2666, data rate = %d\n", ddr_rate);
                    mddr_settings_ptr->ddr4_geardown_mode_en = false;
                }
            }
        }

        if ((!common_cfg.has_ecc) && (mddr_settings_ptr->disable_ecc == false))
        {
            mddr_log(HIGH, "Ignoring ECC feature since DIMMs do not support ECC\n");
            mddr_settings_ptr->disable_ecc = true;
        }

        if((common_cfg.mod_type == SPD_MODULE_TYPE_RDIMM)&&(mddr_settings_ptr->ca_parity == CA_PARITY_DRAM))
        {
            mddr_log(HIGH, "Ignoring DRAM CA Parity feature with DIMM CA Parity disabled for RDIMMs\n");
            mddr_settings_ptr->ca_parity = CA_PARITY_DISABLE;
        }

        // This setting is ignored by hardware whenever 1 pair of channels is
        //  populated, however, we only print a warning and if a setting other
        //  than the default is detected.
        if((num_chans_poped <= 2)&&(mddr_settings_ptr->ddr_granule != DDR_DIST_GRANULE_256B))
        {
            mddr_log(HIGH, "Ignoring channel interleaving granule setting when only one pair of channels is populated\n");
            // Warn and ignore but do not override because the Ring Switches have already been programmed
        }

        // DIMM interleaving is only valid if both slots populated and they have the same amount of memory.
        if ((mddr_settings_ptr->dimm_intlv_en) && (num_dimms_per_chan_poped != DDR_MAX_NUM_SLOTS_PER_CHAN))
        {
            // Override the interleaving setting
            mddr_log(HIGH,"Ignoring DIMM Interleave mode since only 1 DIMM is populated per channel\n");
            mddr_settings_ptr->dimm_intlv_en = false;
        }

        if ((mddr_settings_ptr->dimm_intlv_en) && (common_cfg.slot_config[0].dimm_size_mb != common_cfg.slot_config[1].dimm_size_mb))
        {
            // Override the interleaving setting
            mddr_log(HIGH,"Ignoring DIMM Interleave mode since populated DIMMs have different amounts of memory\n");
            mddr_settings_ptr->dimm_intlv_en = false;
        }
        //dfi_lp_mode is not used in synopsys ddr
        mddr_get_combined_timing_vals(mem_freq_ps);
        mddr_get_latency_vals(&common_cfg, mddr_settings_ptr, &latencies);

        for(ddr_chan = 0; ddr_chan < DDR_MAX_NUM_CHANS; ++ddr_chan)
        {
            if (mddr_check_chan_populated(ddr_chan))
            {
               // ddr channel clock init
               // init ddr controller
               // init ddr phy
               // ddr phy training
            }
            else
            {
              //   mddr_lockdown_unused_chan(ddr_chan);
            }
        }
    }
    else
    {
        mddr_log(CRITICAL, " ERROR : The MDDR Subsystem could not be initialized. \n");
    }

    mddr_record_init_parms(mddr_settings_ptr);

    return status;
}


/******************************************************************************
* Local Functions
******************************************************************************/

/*!****************************************************************************
*
* @par Description:
*    This function validates all MDDR settings arguments passed to the MDDR
*     initialization routine.  Any arguments that are out of range result in an
*     error.  Invalid combinations of values are not checked here, as those
*     checks are performed later in the driver and overridden whenever possible.
*
* @return
*    The status indicating the if the arguments are valid.  COM_OK if the
*     arguments are acceptable, COM_ERROR otherwise.
*
*
******************************************************************************/
unsigned mddr_validate_input(mddr_settings_t *mddr_settings_ptr)
{
    unsigned status = COM_OK;
    return status;

}

/*!****************************************************************************
*
* @par Description:
*    This function collects the SPD based configuration information for all
*    DIMMs populated in the MDDR sub-system.
*
* @return
*    None
*
*
******************************************************************************/
void mddr_get_dimm_configs(void)
{
    unsigned status;
    uint64_t spd_base;
    uint8_t ddr_chan;
    uint8_t dimm_slot;
    unsigned bytes_total;


    spd_base = (uint64_t)spd_scratch_pad;


    // Go through each DIMM SPD and get the associated SPD information.
    for(ddr_chan = 0; ddr_chan < DDR_MAX_NUM_CHANS; ++ddr_chan)
    {
        for(dimm_slot = 0; dimm_slot < DDR_MAX_NUM_SLOTS_PER_CHAN; ++dimm_slot)
        {
            mddr_log(INFORMATIONAL, "Scanning DDR Chan %u  DIMM slot %u\n", hddrss[ddr_chan], dimm_slot);

            // Default to indicating the channel is unpopulated
            mddr_dimm_configs[ddr_chan][dimm_slot].dev_type = SPD_DEVICE_TYPE_UNDEFINED;

            // Need to get number of bytes total in the serial EEPROM for the correct load size.
            //  The SPD field Bytes Total is used instead of Bytes Used because the interpretation
            //  of bytes used differs between DDR3 and DDR4.
            bytes_total = spd_read_dimm_spd_bytes_total(ddr_chan, dimm_slot);

            if((bytes_total > 0) && (bytes_total <= SPD_MAX_SERIAL_EEPROM_SIZE))
            {
                mddr_log(HIGH, "   >> Found DIMM in Channel %u\n", hddrss[ddr_chan]);

                // Read the block of SPD data into memory for the given channel/slot.
                status = spd_read_dimm_nbytes(ddr_chan, dimm_slot, spd_base, bytes_total);

                if(status == COM_OK)
                {
                    spd_get_dimm_config(ddr_chan, dimm_slot, spd_base, &(mddr_dimm_configs[ddr_chan][dimm_slot]));
                }
            }
        }
    }
}

/*!****************************************************************************
*
* @par Description:
*    This function verifies the SPD configuration collected from all
*    populated DIMMs and coalesces the configuration into a common configuration.
*
*                                  @param
*    mddr_settings_ptr  The pointer to the structure containing the
*                        settings information.
*
* @return
*    The status indicating the if the SPD configurations is good or bad. COM_OK
*    if the configurations are acceptable, COM_ERROR otherwise.
*
*
* @par Notes:
*    DIMMs within a channel can differ but all populated channels are expected
*    to be populated identically.
*
******************************************************************************/
unsigned mddr_verify_dimm_combination_configs(mddr_settings_t *mddr_settings_ptr)
{
    unsigned status = COM_ERROR;
    uint8_t ddr_chan;
    uint8_t baseline_chan;
    bool found_baseline_chan = false;
    uint8_t dimm_slot;
    spd_dimm_config_t *cfg_ptr;
    uint8_t max_num_columns = 0;


    // Start with an indication that no DIMMs are present.
    common_cfg.dev_type = SPD_DEVICE_TYPE_UNDEFINED;

    //
    // Establish the common configuration.
    //
    // Find the baseline channel and record the configuration that should be common
    //  to all slots of all populated channels. The first populated channel is
    //  considered the baseline and all other channels should be populated.
    ddr_chan = 0;
    found_baseline_chan = false;
    baseline_chan = 0;    // Set baseline_chan to 0 to start with

    do
    {
        dimm_slot = 0;

        do
        {
            cfg_ptr = &(mddr_dimm_configs[ddr_chan][dimm_slot]);

            if(cfg_ptr->dev_type != SPD_DEVICE_TYPE_UNDEFINED)
            {
                found_baseline_chan = true;
                baseline_chan = ddr_chan;

                // Record the common config for the baseline channel that applies accross
                //  all slots of all channels.
                common_cfg.dev_type = cfg_ptr->dev_type;
                common_cfg.sdram_width = cfg_ptr->sdram_width;
                common_cfg.has_ecc = cfg_ptr->has_ecc;
                common_cfg.mod_type = cfg_ptr->mod_type;
                common_cfg.is_3DS = cfg_ptr->is_3DS;
            }

            ++dimm_slot;
        } while((dimm_slot < DDR_MAX_NUM_SLOTS_PER_CHAN) && (!found_baseline_chan));

        ++ddr_chan;
    } while((ddr_chan < DDR_MAX_NUM_CHANS) && (!found_baseline_chan));



    if(found_baseline_chan)
    {

        // Record the slot dependent pieces of the common config taken from the baseline channel.
        common_cfg.num_slots_populated = 0;
        dimm_slot = 0;

        do
        {
            cfg_ptr = &(mddr_dimm_configs[baseline_chan][dimm_slot]);

            // Ensure the configuration is sane for all slots of a populated channel!!
            if(cfg_ptr->dev_type != SPD_DEVICE_TYPE_UNDEFINED)
            {
                // Record the number of slots populated in the baseline channel
                ++common_cfg.num_slots_populated;

                // Record the common config for the the slot.
                common_cfg.slot_config[dimm_slot].num_ranks = cfg_ptr->num_ranks;
                common_cfg.slot_config[dimm_slot].num_bank_groups = cfg_ptr->num_bank_groups;
                common_cfg.slot_config[dimm_slot].num_rows = cfg_ptr->num_rows;
                common_cfg.slot_config[dimm_slot].num_columns = cfg_ptr->num_columns;
                common_cfg.slot_config[dimm_slot].die_count = cfg_ptr->die_count;
                common_cfg.slot_config[dimm_slot].dimm_size_mb = cfg_ptr->dimm_size_mb;
                common_cfg.slot_config[dimm_slot].ca_mirrored = cfg_ptr->ca_mirrored;

                // Common RDIMM and LRDIMM Values
                common_cfg.slot_config[dimm_slot].out_drv_ck_y13 = cfg_ptr->out_drv_ck_y13;
                common_cfg.slot_config[dimm_slot].out_drv_ck_y02 = cfg_ptr->out_drv_ck_y02;

                // DDR4 RDIMM / LRDIMM Values
                common_cfg.slot_config[dimm_slot].out_drv_ctl_cke = cfg_ptr->out_drv_ctl_cke;
                common_cfg.slot_config[dimm_slot].out_drv_ctl_odt = cfg_ptr->out_drv_ctl_odt;
                common_cfg.slot_config[dimm_slot].out_drv_ctl_ca = cfg_ptr->out_drv_ctl_ca;
                common_cfg.slot_config[dimm_slot].out_drv_ctl_cs = cfg_ptr->out_drv_ctl_cs;

                // DDR4 Only LRDIMM Values
                common_cfg.slot_config[dimm_slot].lrdm_db_drv_ck_bcom_bodt_bcke = cfg_ptr->lrdm_db_drv_ck_bcom_bodt_bcke;
                common_cfg.slot_config[dimm_slot].lrdm_db_drv_ck_bck = cfg_ptr->lrdm_db_drv_ck_bck;

                // The DIMMs in different slots could have different number of columns so pick the max from
                //  populated slots to get the largest page size.
                if (cfg_ptr->num_columns > max_num_columns)
                {
                    max_num_columns = cfg_ptr->num_columns;
                }
            }
            else
            {
                // Mark the configuration for empty slots as invalid.
                common_cfg.slot_config[dimm_slot].num_ranks = SPD_INVALID_RANK_COUNT;
            }

            ++dimm_slot;
        } while(dimm_slot < DDR_MAX_NUM_SLOTS_PER_CHAN);


        //
        // Find page size, 0 means < 1K i.e. 512B
        //
        common_cfg.max_page_size_kb = (max_num_columns * common_cfg.sdram_width / BITS_PER_BYTE) / BYTES_PER_KB;


        //
        // Go through the DIMMs and verify the are supported and conform to the baseline channel.
        //
        status = mddr_verify_dimms(mddr_settings_ptr);
    }

    return status;
}

/*!****************************************************************************
*
* @par Description:
*    This function verifies the SPD configuration collected from all
*    populated DIMMs.
*
*                                  @param
*    mddr_settings_ptr  The pointer to the structure containing the
*                        settings information.
*
* @return
*    The status indicating the if the SPD configurations is good or bad. COM_OK
*    if the configurations are acceptable, COM_ERROR otherwise.
*
*
* @par Notes:
*    DIMMs within a channel can differ but all populated channels are expected
*    to be populated identically.
*
******************************************************************************/
unsigned mddr_verify_dimms(mddr_settings_t *mddr_settings_ptr)
{
    unsigned status = COM_OK;
    uint8_t ddr_chan;
    uint8_t dimm_slot;
    uint8_t num_even_chans = 0;
    uint8_t num_odd_chans = 0;
    spd_dimm_config_t *cfg_ptr;
    bool chan_populated;
    uint8_t num_slots_populated;
    uint32_t channel_size_gb;

    // Reset value to allow for reentrancy for watchdog reset
    num_chans_poped = 0;

    // Go through each DIMM SPD and check the associated SPD information.
    //  The DIMMs (slots) within a single channel can have a level of variation,
    //  but at the channel level all channels should match the common configuration
    //  of the baseline.
    for(ddr_chan = 0; ddr_chan < DDR_MAX_NUM_CHANS; ++ddr_chan)
    {
        // Track populated channels to support counting the number of channels populated in each interleave.
        chan_populated = false;
        num_slots_populated = 0;
        channel_size_gb = 0;

        for (dimm_slot = 0; dimm_slot < DDR_MAX_NUM_SLOTS_PER_CHAN; ++dimm_slot)
        {
            cfg_ptr = &(mddr_dimm_configs[ddr_chan][dimm_slot]);

            if (cfg_ptr->dev_type != SPD_DEVICE_TYPE_UNDEFINED)
            {
                chan_populated = true;

                channel_size_gb += ((mddr_dimm_configs[ddr_chan][dimm_slot].dimm_size_mb) / MB_PER_GB);
                ++num_slots_populated;
            }
        }

        // This is a per channel check to ensure that, in a 2DPC, slot 1 cannot be populated without slot 0.
        // If Slot 0 is empty, and Slot 1 is populated, we report an error.
        if ((chan_populated == true) && (num_slots_populated == 1))
        {
            if (mddr_dimm_configs[ddr_chan][0].dev_type == SPD_DEVICE_TYPE_UNDEFINED)
            {

                mddr_log(CRITICAL, "ERROR:  In Chan %u, Found DIMM in slot 0 but, not in slot 1. Slot 0 must be populated before slot 1\n",
                       hddrss[ddr_chan]);
                status = COM_ERROR;

            }
        }

        // Perform the per slot checks only when the per channel check is OK.
        if (status == COM_OK)
        {
            for(dimm_slot = 0; dimm_slot < DDR_MAX_NUM_SLOTS_PER_CHAN; ++dimm_slot)
            {

                cfg_ptr = &(mddr_dimm_configs[ddr_chan][dimm_slot]);

                if(cfg_ptr->dev_type != SPD_DEVICE_TYPE_UNDEFINED)
                {
                    // Start by determining if the individual DIMM is supportable.
                    if (cfg_ptr->dev_type != SPD_DEVICE_TYPE_DDR4)
                    {
                        mddr_log(CRITICAL, "ERROR:  Unrecognized Device Type 0x%2.2x in Chan %u  Slot %u\n", cfg_ptr->dev_type, hddrss[ddr_chan], dimm_slot);
                        status = COM_ERROR;
                    }
                    else if ((cfg_ptr->mod_type != SPD_MODULE_TYPE_UDIMM) && (cfg_ptr->mod_type != SPD_MODULE_TYPE_RDIMM) &&
                        (cfg_ptr->mod_type != SPD_MODULE_SODIMM) && (cfg_ptr->mod_type != SPD_MODULE_SOUDIMM) && (cfg_ptr->mod_type != SPD_MODULE_TYPE_DDR4_LRDIMM))
                    {
                        mddr_log(CRITICAL, "ERROR:  Unrecognized Module Type 0x%2.2x in Chan %u  Slot %u\n", cfg_ptr->mod_type, hddrss[ddr_chan], dimm_slot);
                        status = COM_ERROR;
                    }
                    else if ((num_slots_populated == DDR_MAX_NUM_SLOTS_PER_CHAN) && (cfg_ptr->mod_type == SPD_MODULE_TYPE_DDR4_LRDIMM))
                    {
                        mddr_log(CRITICAL, "ERROR:  In 2DPC, for module type LRDIMM, more than 1 DIMM per channel populated is not supported\n");
                        status = COM_ERROR;
                    }
                    else if ((cfg_ptr->num_ranks > SPD_MAX_RANKS_SUPPORTED) || ((cfg_ptr->num_ranks == SPD_UNSUPPORTED_RANK_VAL)))
                    {
                        mddr_log(CRITICAL, "ERROR:  Invalid number of chip selects(CS)= %u in Chan %u  Slot %u\n", cfg_ptr->num_ranks, hddrss[ddr_chan], dimm_slot);
                        status = COM_ERROR;
                    }
                    else if ((num_slots_populated == DDR_MAX_NUM_SLOTS_PER_CHAN) && (cfg_ptr->num_ranks > SPD_MAX_RANKS_SUPPORTED_2DPC))
                    {
                        mddr_log(CRITICAL, "ERROR:  In 2DPC, Invalid number of chip selects (CS) = %u in Chan %u  Slot %u\n", cfg_ptr->num_ranks, hddrss[ddr_chan], dimm_slot);
                        status = COM_ERROR;
                    }
                    else if ((cfg_ptr->sdram_width != SPD_SDRAM_WIDTH_X4) && (cfg_ptr->sdram_width != SPD_SDRAM_WIDTH_X8))
                    {
                        mddr_log(CRITICAL, "ERROR:  Unsupported device width 0x%2.2x in Chan %u  Slot %u\n", cfg_ptr->sdram_width, hddrss[ddr_chan], dimm_slot);
                        status = COM_ERROR;
                    }
                    else if (cfg_ptr->dimm_module_width != SPD_MODULE_WIDTH)
                    {
                        mddr_log(CRITICAL, "ERROR:  Invalid DIMM module width 0x%2.2x in Chan %u  Slot %u\n", cfg_ptr->dimm_module_width, hddrss[ddr_chan], dimm_slot);
                        status = COM_ERROR;
                    }
                    else if ((cfg_ptr->num_rows < SPD_MIN_ROWS) || (cfg_ptr->num_rows > SPD_MAX_ROWS))
                    {
                        mddr_log(CRITICAL, "ERROR:  Invalid number of row addresses= %u in Chan %u  Slot %u\n", cfg_ptr->num_rows, hddrss[ddr_chan], dimm_slot);
                        status = COM_ERROR;
                    }
                    else if ((cfg_ptr->num_columns < SPD_MIN_COLUMNS) || (cfg_ptr->num_columns > SPD_MAX_COLUMNS))
                    {
                        mddr_log(CRITICAL, "ERROR:  Invalid number of column addresses= %u in Chan %u  Slot %u\n", cfg_ptr->num_columns, hddrss[ddr_chan], dimm_slot);
                        status = COM_ERROR;
                    }
                    else if ((cfg_ptr->die_count != 1) && (cfg_ptr->die_count != 2) &&
                              (cfg_ptr->die_count != 4) && (cfg_ptr->die_count != 8))
                    {
                        // MC supports only specific die counts for stacked devices.
                        mddr_log(CRITICAL, "ERROR:  Invalid number of dies 0x%2.2x in Chan %u  Slot %u\n", cfg_ptr->die_count, hddrss[ddr_chan], dimm_slot);
                        status = COM_ERROR;
                    }
                    else if (cfg_ptr->is_voltage_ok == false)
                    {
                        mddr_log(CRITICAL, "ERROR:  DIMM does not support required voltage in Chan %u  Slot %u\n", hddrss[ddr_chan], dimm_slot);
                        status = COM_ERROR;
                    }
                    else if (cfg_ptr->mtb_ps == 0)
                    {
                        mddr_log(CRITICAL, "ERROR:  Time base is 0 in Chan %u  Slot %u\n", hddrss[ddr_chan], dimm_slot);
                        status = COM_ERROR;
                    }
                    else if (cfg_ptr->dimm_size_mb < SPD_MIN_DIMM_SIZE)
                    {
                        mddr_log(CRITICAL, "ERROR: DIMM size is less than %d GB, size = %d GB in Chan %u  Slot %u\n",
                               SPD_MIN_DIMM_SIZE / MB_PER_GB, cfg_ptr->dimm_size_mb / MB_PER_GB, hddrss[ddr_chan], dimm_slot);
                        status = COM_ERROR;
                    }
                    else if (cfg_ptr->dimm_size_mb > SPD_MAX_DIMM_SIZE)
                    {
                        mddr_log(CRITICAL, "ERROR: DIMM size is greater than SPD_MAX_DIMM_SIZE GB, size = %d GB in Chan %u  Slot %u\n",
                               SPD_MAX_DIMM_SIZE / MB_PER_GB, cfg_ptr->dimm_size_mb / MB_PER_GB, hddrss[ddr_chan], dimm_slot);
                        status = COM_ERROR;
                    }
                    else if(cfg_ptr->is_rank_mix)    // asymmetrical ranks
                    {
                        mddr_log(CRITICAL, "ERROR: Asymmetrical ranks detected, in Chan %u\n", hddrss[ddr_chan]);
                        status = COM_ERROR;
                    }
                    else if ((num_slots_populated == DDR_MAX_NUM_SLOTS_PER_CHAN) && (cfg_ptr->dimm_size_mb > SPD_MAX_DIMM_SIZE_2DPC))
                    {
                        mddr_log(CRITICAL, "ERROR: DIMM size is greater than %d GB in 2DPC, size = %d GB in Chan %u  Slot %u\n",
                               SPD_MAX_DIMM_SIZE_2DPC / MB_PER_GB, cfg_ptr->dimm_size_mb / MB_PER_GB , hddrss[ddr_chan], dimm_slot);
                        status = COM_ERROR;
                    }
                    else if(channel_size_gb > SPD_MAX_CHANNEL_SIZE)
                    {
                        mddr_log(CRITICAL, "ERROR: Channel size is greater than %d GB, size = %d GB in Chan %u\n",
                               SPD_MAX_CHANNEL_SIZE / MB_PER_GB, channel_size_gb, hddrss[ddr_chan]);
                        status = COM_ERROR;
                    }
                    else if ((cfg_ptr->num_ranks == DDR_RANKS_QUAD_CS_DIMM) && (cfg_ptr->is_3DS))
                    {
                        mddr_log(CRITICAL, "ERROR: Cannot support Quad rank(CS) TSV DIMM, in Chan %u  Slot %u\n", hddrss[ddr_chan], dimm_slot);
                        status = COM_ERROR;
                    }
                    else
                    {
                        // The individual DIMM is supportable. Now check that it is
                        //  consistant with the population of the baseline channel.

                        if(num_slots_populated != common_cfg.num_slots_populated)
                        {
                            mddr_log(CRITICAL, "ERROR:  Number of DIMMs in each populated channel not the same, \
                                                  expected 0x%2.2x got 0x%2.2x in Chan %u\n",
                                   common_cfg.num_slots_populated, num_slots_populated, hddrss[ddr_chan]);
                            status = COM_ERROR;
                        }
                        else if (cfg_ptr->dev_type != common_cfg.dev_type)
                        {
                            mddr_log(CRITICAL, "ERROR:  Device type mismatch, expected 0x%2.2x got 0x%2.2x in Chan %u  Slot %u\n",
                                   common_cfg.dev_type, cfg_ptr->dev_type, hddrss[ddr_chan], dimm_slot);
                            status = COM_ERROR;
                        }
                        else if (cfg_ptr->sdram_width != common_cfg.sdram_width)
                        {
                            mddr_log(CRITICAL, "ERROR:  Device width mismatch, DIMMs with different width populated, \
                                                  expected 0x%2.2x got 0x%2.2x in Chan %u  Slot %u\n",
                                   common_cfg.sdram_width, cfg_ptr->sdram_width, hddrss[ddr_chan], dimm_slot);
                            status = COM_ERROR;
                        }
                        else if ((cfg_ptr->has_ecc != common_cfg.has_ecc) && (mddr_settings_ptr->disable_ecc == false))
                        {
                            mddr_log(CRITICAL, "ERROR:  ECC mismatch, expected 0x%2.2x got 0x%2.2x in Chan %u  Slot %u\n",
                                   common_cfg.has_ecc, cfg_ptr->has_ecc, hddrss[ddr_chan], dimm_slot);
                            status = COM_ERROR;
                        }
                        else if (cfg_ptr->mod_type != common_cfg.mod_type)
                        {
                            mddr_log(CRITICAL, "ERROR:  Module type mismatch, expected 0x%2.2x got 0x%2.2x in Chan %u  Slot %u\n",
                                   common_cfg.mod_type, cfg_ptr->mod_type, hddrss[ddr_chan], dimm_slot);
                            status = COM_ERROR;
                        }
                        else if (cfg_ptr->is_3DS != common_cfg.is_3DS)
                        {
                            mddr_log(CRITICAL, "ERROR:  Device 3DS mismatch, expected 0x%2.2x got 0x%2.2x in Chan %u  Slot %u\n",
                                   common_cfg.is_3DS, cfg_ptr->is_3DS, hddrss[ddr_chan], dimm_slot);
                            status = COM_ERROR;
                        }
                        else if (cfg_ptr->ca_mirrored != common_cfg.slot_config[dimm_slot].ca_mirrored)
                        {
                            mddr_log(CRITICAL, "ERROR:  Device CA mirroring mismatch, expected 0x%2.2x got 0x%2.2x in Chan %u  Slot %u\n",
                                   common_cfg.slot_config[dimm_slot].ca_mirrored, cfg_ptr->ca_mirrored, hddrss[ddr_chan], dimm_slot);
                            status = COM_ERROR;
                        }
                        else if (cfg_ptr->num_ranks != common_cfg.slot_config[dimm_slot].num_ranks)
                        {
                            mddr_log(CRITICAL, "ERROR:  Number of ranks mismatch, expected 0x%2.2x got 0x%2.2x in Chan %u  Slot %u\n",
                                   common_cfg.slot_config[dimm_slot].num_ranks, cfg_ptr->num_ranks, hddrss[ddr_chan], dimm_slot);
                            status = COM_ERROR;
                        }
                        else if (cfg_ptr->num_bank_groups != common_cfg.slot_config[dimm_slot].num_bank_groups)
                        {
                            mddr_log(CRITICAL, "ERROR:  Number of bank groups mismatch, expected 0x%2.2x got 0x%2.2x in Chan %u  Slot %u\n",
                                   common_cfg.slot_config[dimm_slot].num_bank_groups, cfg_ptr->num_bank_groups, hddrss[ddr_chan], dimm_slot);
                            status = COM_ERROR;
                        }
                        else if (cfg_ptr->num_rows != common_cfg.slot_config[dimm_slot].num_rows)
                        {
                            mddr_log(CRITICAL, "ERROR:  Number of row addresses mismatch, expected 0x%2.2x got 0x%2.2x in Chan %u  Slot %u\n",
                                   common_cfg.slot_config[dimm_slot].num_rows, cfg_ptr->num_rows, hddrss[ddr_chan], dimm_slot);
                            status = COM_ERROR;
                        }
                        else if (cfg_ptr->num_columns != common_cfg.slot_config[dimm_slot].num_columns)
                        {
                            mddr_log(CRITICAL, "ERROR:  Number of column address mismatch, expected 0x%2.2x got 0x%2.2x in Chan %u  Slot %u\n",
                                   common_cfg.slot_config[dimm_slot].num_columns, cfg_ptr->num_columns, hddrss[ddr_chan], dimm_slot);
                            status = COM_ERROR;
                        }
                        else if (cfg_ptr->die_count != common_cfg.slot_config[dimm_slot].die_count)
                        {
                            mddr_log(CRITICAL, "ERROR:  Device die count mismatch, expected 0x%2.2x got 0x%2.2x in Chan %u  Slot %u\n",
                                   common_cfg.slot_config[dimm_slot].die_count, cfg_ptr->die_count, hddrss[ddr_chan], dimm_slot);
                            status = COM_ERROR;
                        }
                        else if (cfg_ptr->dimm_size_mb != common_cfg.slot_config[dimm_slot].dimm_size_mb)
                        {
                            mddr_log(CRITICAL, "ERROR:  DIMM size mismatch, capacity of all populated DIMMs not the same, \
                                                  expected %uMB got %uMB in Chan %u  Slot %u\n",
                                   common_cfg.slot_config[dimm_slot].dimm_size_mb, cfg_ptr->dimm_size_mb, hddrss[ddr_chan], dimm_slot);
                            status = COM_ERROR;
                        }
                        else if (common_cfg.slot_config[dimm_slot].dimm_size_mb > common_cfg.slot_config[0].dimm_size_mb)
                        {
                            // DDR Sub-System (DDRSS) Hardware Programming Guide - January 20, 2017 - Section 4.17.1.4
                            //  When a memory channel contains two DIMMs, and the two DIMMs are of different size,
                            //  capacity, the larger DIMM must be populated in slot 0.

                            mddr_log(CRITICAL, "ERROR:  The DIMM in slot %d (%d GB) is larger than the DIMM in slot 0 (%d GB) in Chan %u\n",
                                   dimm_slot, cfg_ptr->dimm_size_mb / MB_PER_GB, common_cfg.slot_config[0].dimm_size_mb / MB_PER_GB, hddrss[ddr_chan]);
                            status = COM_ERROR;
                        }
                        else
                        {
                            if (common_cfg.mod_type == SPD_MODULE_TYPE_RDIMM)
                            {
                                if (cfg_ptr->out_drv_ck_y13 != common_cfg.slot_config[dimm_slot].out_drv_ck_y13)
                                {
                                    // Common accross DDR3 & DDR4 RDIMMs
                                    mddr_log(CRITICAL, "ERROR:  RDIMM output drive CK 13 Mismatch, expected 0x%2.2x got 0x%2.2x in Chan %u  Slot %u\n",
                                           common_cfg.slot_config[dimm_slot].out_drv_ck_y13, cfg_ptr->out_drv_ck_y13, hddrss[ddr_chan], dimm_slot);
                                    status = COM_ERROR;
                                }
                                else if (cfg_ptr->out_drv_ck_y02 != common_cfg.slot_config[dimm_slot].out_drv_ck_y02)
                                {
                                    // Common accross DDR3 & DDR4 RDIMMs
                                    mddr_log(CRITICAL, "ERROR:  RDIMM output drive CK 02 Mismatch, expected 0x%2.2x got 0x%2.2x in Chan %u  Slot %u\n",
                                           common_cfg.slot_config[dimm_slot].out_drv_ck_y02, cfg_ptr->out_drv_ck_y02, hddrss[ddr_chan], dimm_slot);
                                    status = COM_ERROR;
                                }

                                // Verify DDR4 Specifics
                                if(common_cfg.dev_type == SPD_DEVICE_TYPE_DDR4)
                                {
                                    if (cfg_ptr->out_drv_ctl_cke != common_cfg.slot_config[dimm_slot].out_drv_ctl_cke)
                                    {
                                        // DDR4 Only RDIMM Value
                                        mddr_log(CRITICAL, "ERROR:  DDR4 RDIMM output drive control CKE Mismatch, expected 0x%2.2x got 0x%2.2x in Chan %u  Slot %u\n",
                                               common_cfg.slot_config[dimm_slot].out_drv_ctl_cke, cfg_ptr->out_drv_ctl_cke, hddrss[ddr_chan], dimm_slot);
                                        status = COM_ERROR;
                                    }
                                    else if (cfg_ptr->out_drv_ctl_odt != common_cfg.slot_config[dimm_slot].out_drv_ctl_odt)
                                    {
                                        // DDR4 Only RDIMM Value
                                        mddr_log(CRITICAL, "ERROR:  DDR4 RDIMM output drive control ODT Mismatch, expected 0x%2.2x got 0x%2.2x in Chan %u  Slot %u\n",
                                               common_cfg.slot_config[dimm_slot].out_drv_ctl_odt, cfg_ptr->out_drv_ctl_odt, hddrss[ddr_chan], dimm_slot);
                                        status = COM_ERROR;
                                    }
                                    else if (cfg_ptr->out_drv_ctl_ca != common_cfg.slot_config[dimm_slot].out_drv_ctl_ca)
                                    {
                                        // DDR4 Only RDIMM Value
                                        mddr_log(CRITICAL, "ERROR:  DDR4 RDIMM output drive control command/address Mismatch, expected 0x%2.2x got 0x%2.2x in Chan %u  Slot %u\n",
                                               common_cfg.slot_config[dimm_slot].out_drv_ctl_ca, cfg_ptr->out_drv_ctl_ca, hddrss[ddr_chan], dimm_slot);
                                        status = COM_ERROR;
                                    }
                                    else if (cfg_ptr->out_drv_ctl_cs != common_cfg.slot_config[dimm_slot].out_drv_ctl_cs)
                                    {
                                        // DDR4 Only RDIMM Value
                                        mddr_log(CRITICAL, "ERROR:  DDR4 RDIMM output drive control chip select Mismatch, expected 0x%2.2x got 0x%2.2x in Chan %u  Slot %u\n",
                                               common_cfg.slot_config[dimm_slot].out_drv_ctl_cs, cfg_ptr->out_drv_ctl_cs, hddrss[ddr_chan], dimm_slot);
                                        status = COM_ERROR;
                                    }
                                }
                            }

                            if ((common_cfg.dev_type == SPD_DEVICE_TYPE_DDR4) && (common_cfg.mod_type == SPD_MODULE_TYPE_DDR4_LRDIMM))
                            {
                                if (cfg_ptr->lrdm_db_drv_ck_bcom_bodt_bcke != common_cfg.slot_config[dimm_slot].lrdm_db_drv_ck_bcom_bodt_bcke)
                                {
                                    // DDR4 Only LRDIMM Value
                                    mddr_log(CRITICAL, "ERROR:  LRDIMM SPD Output Drive Strength for Clock and Data Buffer, bcom, bodt, bcke Mismatch, \
                                                          expected 0x%2.2x got 0x%2.2x in Chan %u  Slot %u\n",
                                           common_cfg.slot_config[dimm_slot].lrdm_db_drv_ck_bcom_bodt_bcke, cfg_ptr->lrdm_db_drv_ck_bcom_bodt_bcke, hddrss[ddr_chan], dimm_slot);
                                    status = COM_ERROR;
                                }
                                else if (cfg_ptr->lrdm_db_drv_ck_bck != common_cfg.slot_config[dimm_slot].lrdm_db_drv_ck_bck)
                                {
                                    // DDR4 Only LRDIMM Value
                                    mddr_log(CRITICAL, "ERROR:  LRDIMM SPD Output Drive Strength for Clock and Data Buffer, bck Mismatch, \
                                                          expected 0x%2.2x got 0x%2.2x in Chan %u  Slot %u\n",
                                           common_cfg.slot_config[dimm_slot].lrdm_db_drv_ck_bck, cfg_ptr->lrdm_db_drv_ck_bck, hddrss[ddr_chan], dimm_slot);
                                    status = COM_ERROR;
                                }
                            }
                        }
                    }
                }
            }
        }


        if(chan_populated)
        {
            ++num_chans_poped;

            // Note: HDDRSS4 is considered odd, and HDDRSS5 is considered even
            if((ddr_chan % 2) == 0)
            {
                //Even channel
                ++num_even_chans;
            }
            else
            {
                //Odd channel
                ++num_odd_chans;
            }
        }
    }


    if(num_chans_poped == 0)
    {
        mddr_log(CRITICAL, "ERROR:  No DIMMS found!\n");
        status = COM_ERROR;
    }
    else if((num_even_chans != num_odd_chans))
    {
        mddr_log(CRITICAL, "ERROR:  Number of populated channels is not balanced, even=%d, odd=%d\n",
                num_even_chans, num_odd_chans);
        status = COM_ERROR;
    }


    return status;
}



/*!****************************************************************************
*
* @par Description:
*    This function gets the various timing values and combines / collects them
*    into a package.
*
*                                  @param
*    mem_freq_ps    The memory frequency expressed in picoseconds.
*
*
* @return
*    None
*
*
******************************************************************************/
void mddr_get_combined_timing_vals(uint32_t mem_freq_ps)
{
    uint8_t ddr_chan;
    uint8_t dimm_slot;
    spd_dimm_config_t *cfg_ptr;

    uint8_t first_iter = 0x1;
    uint8_t new_cl_high_bit;
    uint8_t old_cl_high_bit;

    uint32_t dimm_trcd_min_clk;
    uint32_t dimm_trp_min_clk;
    uint32_t dimm_tras_min_clk;
    uint32_t dimm_trc_min_clk;
    uint32_t dimm_tfaw_min_clk;
    uint32_t dimm_twr_min_clk;
    uint32_t dimm_twtr_min_clk;

    // DDR4 Only
    uint32_t dimm_trrdl_min_clk;
    uint32_t dimm_trrds_min_clk;
    uint32_t dimm_tccdl_min_clk;
    uint32_t dimm_twtrs_min_clk;


    mddr_timing_vals.cas_lat_sup  = 0x0;

    mddr_timing_vals.taa_min_ps   = 0;
    mddr_timing_vals.tck_min_ps   = 0;
    mddr_timing_vals.tck_max_ps   = 0;

    mddr_timing_vals.trcd_min_clk = 0;
    mddr_timing_vals.trp_min_clk  = 0;
    mddr_timing_vals.tras_min_clk = 0;
    mddr_timing_vals.trc_min_clk  = 0;
    mddr_timing_vals.tfaw_min_clk = 0;

    mddr_timing_vals.trrd_min_clk = 0;
    mddr_timing_vals.twr_min_clk  = 0;
    mddr_timing_vals.twtr_min_clk = 0;
    mddr_timing_vals.trtp_min_clk = 0;
    mddr_timing_vals.trfc_min_ps = 0;

    mddr_timing_vals.trrdl_min_clk = 0;
    mddr_timing_vals.trrds_min_clk = 0;
    mddr_timing_vals.tccdl_min_clk = 0;
    mddr_timing_vals.trfc1_min_ps = 0;
    mddr_timing_vals.trfc2_min_ps = 0;
    mddr_timing_vals.trfc4_min_ps = 0;
    mddr_timing_vals.twtrs_min_clk = 0;

    mddr_timing_vals.tstab_clk = 0;
    mddr_timing_vals.tckev_min_clk = 0;


    // Go through each DIMM configuration and incorporate its timing values.
    for(ddr_chan = 0; ddr_chan < DDR_MAX_NUM_CHANS; ++ddr_chan)
    {
        for(dimm_slot = 0; dimm_slot < DDR_MAX_NUM_SLOTS_PER_CHAN; ++dimm_slot)
        {
            cfg_ptr = &(mddr_dimm_configs[ddr_chan][dimm_slot]);

            if(cfg_ptr->dev_type != SPD_DEVICE_TYPE_UNDEFINED)
            {
                // Get the timing values for the DIMM in clock cycles.
                dimm_trcd_min_clk = mddr_get_num_clocks(cfg_ptr->trcd_min_ps, mem_freq_ps);
                dimm_trp_min_clk  = mddr_get_num_clocks(cfg_ptr->trp_min_ps, mem_freq_ps);
                dimm_tras_min_clk = mddr_get_num_clocks(cfg_ptr->tras_min_ps, mem_freq_ps);
                dimm_trc_min_clk  = mddr_get_num_clocks(cfg_ptr->trc_min_ps, mem_freq_ps);
                dimm_tfaw_min_clk = mddr_get_num_clocks(cfg_ptr->tfaw_min_ps, mem_freq_ps);
                dimm_twr_min_clk  = mddr_get_num_clocks(cfg_ptr->twr_min_ps, mem_freq_ps);
                dimm_twtr_min_clk = mddr_get_num_clocks(cfg_ptr->twtr_min_ps, mem_freq_ps);

                if((cfg_ptr->dev_type == SPD_DEVICE_TYPE_DDR4) && (cfg_ptr->revision <= SPD_REVISION_1_0))
                {
                    // twtr_l=max(4nCK, 7.5ns) up to 2400, rest is TBD
                    dimm_twtr_min_clk = COM_MAX(dimm_twtr_min_clk, 4);
                }

                dimm_trrdl_min_clk = mddr_get_num_clocks(cfg_ptr->trrdl_min_ps, mem_freq_ps);
                dimm_trrds_min_clk = mddr_get_num_clocks(cfg_ptr->trrds_min_ps, mem_freq_ps);
                dimm_tccdl_min_clk = mddr_get_num_clocks(cfg_ptr->tccdl_min_ps, mem_freq_ps);
                dimm_twtrs_min_clk = mddr_get_num_clocks(cfg_ptr->twtrs_min_ps, mem_freq_ps);

                if((cfg_ptr->dev_type == SPD_DEVICE_TYPE_DDR4) && (cfg_ptr->revision <= SPD_REVISION_1_0))
                {
                    // twtr_s=max(2nCK, 2.5ns) up to 2400, rest is TBD
                    dimm_twtrs_min_clk = COM_MAX(dimm_twtrs_min_clk, 2);
                }


                // First iteration, assign the CAS latency value as found
                if (first_iter == 0x1)
                {
                    mddr_timing_vals.cas_lat_sup  = cfg_ptr->cas_lat_sup;
                    first_iter = 0x0;
                }
                // For subsequent iterations, high & low latencies need to be combined, choosing lower side
                else
                {
                    new_cl_high_bit = (cfg_ptr->cas_lat_sup >> SPD_CAL_RANGE_BIT___S) & 0x1;
                    old_cl_high_bit = (mddr_timing_vals.cas_lat_sup >> SPD_CAL_RANGE_BIT___S) & 0x1;

                    if (new_cl_high_bit != old_cl_high_bit)
                    {
                        // CL: 23 to 36 (14 bits) overlap between low range and high range CAS latency bytes.
                        // Hence, to determine the common usable range, when we have SPDs with low and high ranges,
                        // we pick the bits corresponding to Cl=23 thru CL=36.
                        if (new_cl_high_bit > 0x0)    // New SPD has high CAS latency range
                        {
                            // remove bits 14 and higher, shift up to 16 bits
                            cfg_ptr->cas_lat_sup = cfg_ptr->cas_lat_sup & 0x3FFF;
                            cfg_ptr->cas_lat_sup = cfg_ptr->cas_lat_sup << 16;
                        }
                        else                          // old SPD has high CAS latency range
                        {
                            // remove bits 14 and higher, shift up to 16 bits
                            mddr_timing_vals.cas_lat_sup = mddr_timing_vals.cas_lat_sup & 0x3FFF;
                            mddr_timing_vals.cas_lat_sup = mddr_timing_vals.cas_lat_sup << 16;
                        }
                    }

                    // AND in the CAS latency of each dimm to get the common CAS latency across all DIMMs
                    mddr_timing_vals.cas_lat_sup = mddr_timing_vals.cas_lat_sup & cfg_ptr->cas_lat_sup;
                }


                // Keep the highest values.
                mddr_timing_vals.taa_min_ps   = COM_MAX(cfg_ptr->taa_min_ps,  mddr_timing_vals.taa_min_ps);
                mddr_timing_vals.tck_min_ps   = COM_MAX(cfg_ptr->tck_min_ps,  mddr_timing_vals.tck_min_ps);
                mddr_timing_vals.tck_max_ps   = COM_MAX(cfg_ptr->tck_max_ps,  mddr_timing_vals.tck_max_ps);

                mddr_timing_vals.trcd_min_clk = COM_MAX(dimm_trcd_min_clk, mddr_timing_vals.trcd_min_clk);
                mddr_timing_vals.trp_min_clk  = COM_MAX(dimm_trp_min_clk,  mddr_timing_vals.trp_min_clk);
                mddr_timing_vals.tras_min_clk = COM_MAX(dimm_tras_min_clk, mddr_timing_vals.tras_min_clk);
                mddr_timing_vals.trc_min_clk  = COM_MAX(dimm_trc_min_clk,  mddr_timing_vals.trc_min_clk);
                mddr_timing_vals.tfaw_min_clk = COM_MAX(dimm_tfaw_min_clk, mddr_timing_vals.tfaw_min_clk);
                mddr_timing_vals.twr_min_clk  = COM_MAX(dimm_twr_min_clk,  mddr_timing_vals.twr_min_clk);
                mddr_timing_vals.twtr_min_clk = COM_MAX(dimm_twtr_min_clk, mddr_timing_vals.twtr_min_clk);

                mddr_timing_vals.trfc_min_ps = COM_MAX(cfg_ptr->trfc_min_ps, mddr_timing_vals.trfc_min_ps);

                mddr_timing_vals.trrdl_min_clk = COM_MAX(dimm_trrdl_min_clk, mddr_timing_vals.trrdl_min_clk);
                mddr_timing_vals.trrds_min_clk = COM_MAX(dimm_trrds_min_clk, mddr_timing_vals.trrds_min_clk);
                mddr_timing_vals.tccdl_min_clk = COM_MAX(dimm_tccdl_min_clk, mddr_timing_vals.tccdl_min_clk);
                mddr_timing_vals.trfc1_min_ps = COM_MAX(cfg_ptr->trfc1_min_ps, mddr_timing_vals.trfc1_min_ps);
                mddr_timing_vals.trfc2_min_ps = COM_MAX(cfg_ptr->trfc2_min_ps, mddr_timing_vals.trfc2_min_ps);
                mddr_timing_vals.trfc4_min_ps = COM_MAX(cfg_ptr->trfc4_min_ps, mddr_timing_vals.trfc4_min_ps);
                mddr_timing_vals.twtrs_min_clk = COM_MAX(dimm_twtrs_min_clk, mddr_timing_vals.twtrs_min_clk);

                if (mddr_dimm_configs[ddr_chan][dimm_slot].dev_type == SPD_DEVICE_TYPE_DDR4)
                {
                    mddr_timing_vals.trfc_min_ps = mddr_timing_vals.trfc1_min_ps;
                }
            }
        }
    }


    if(platform_mode != SILICON)
    {
        // Bump up trfc for emulation
        mddr_timing_vals.trfc_min_ps = COM_MAX(mddr_timing_vals.trfc_min_ps, 0x9EB100);

        // Bump up trfc2 for emulation
        mddr_timing_vals.trfc2_min_ps *= 3;

        // Bump up trfc4 for emulation
        mddr_timing_vals.trfc4_min_ps *= 3;
    }

    //
    // Get derived timing values. These are derived based on other configuration information and some are
    //  based in part on the the above SPD based timing values.
    //

    mddr_timing_vals.tck_ps = mddr_get_clock_period_timing_val();
    mddr_timing_vals.txs_clk = mddr_get_derived_txs_timing_val(mddr_timing_vals.trfc_min_ps);
    mddr_timing_vals.txsdll_min_clk = mddr_get_derived_txsdll_timing_val();
    mddr_timing_vals.tdllk_min_clk = mddr_get_derived_tdllkmin_timing_val();
    mddr_timing_vals.tstab_clk = mddr_get_derived_tstab_timing_val();
    mddr_timing_vals.tckev_min_clk = mddr_get_derived_tckevmin_timing_val();
}



/*!****************************************************************************
*
* @par Description:
*    This function determines the DDR latencies using the provided configuration
*    and settings.
*
*                                  @param
*    common_cfg_ptr         The pointer to the structure containing the
*                            configuration information.
*                                  @param
*    mddr_settings_ptr       The pointer to the structure containing the
*                            settings information.
*                                  @param
*    latencies_ptr          The pointer to the structure containing the latencies
*                             as calculated / determined by this function.
*
*
* @return
*    None
*
*
* @par Notes:
*    CAL and Control Gear Down modes are not supported in Rev 1. When supported
*    in Rev 2, they will be incorporated into the read latency (RL) and MR4 values
*    per Table 10, page 20 of JEDEC DDR4 Specification(JESD79-4) for
*    DDR4 configurations.
*
******************************************************************************/
void mddr_get_latency_vals
(
    mddr_common_cfg_t *common_cfg_ptr,
    mddr_settings_t *mddr_settings_ptr,
    mddr_latencies_t *latencies_ptr
)
{

    uint32_t cl;
    uint32_t orl;
    uint32_t rl;
    uint32_t wl;
    uint32_t cwl=0;
    uint32_t pl = 0;
    uint32_t tdbi = 0;
    uint32_t tcal = 0;
    uint8_t tmod=0;
    uint8_t tmrd;
    uint8_t mc_mr2_cwl=0;
    uint8_t mc_mr4_cal = 0;
    uint8_t mc_mr5_pl=0;
    uint8_t rcd_nladd = 0;


    // Get the CAS Latency.
    cl = mddr_get_cas_latency(common_cfg_ptr);

      rl = cl;


    if (common_cfg_ptr->dev_type == SPD_DEVICE_TYPE_DDR4)
    {
        // Invalid value used when no DRAM parity
        mc_mr5_pl = 0;

        tmod = max_of_clk_ps(24, 15000, mem_freq_ps); // max of 24nCK, 15ns
        tmrd = 8;

        //
        // From JESD79-4 - JEDEC DDR4 Specification
        //
        // DRAM parity
        if ((mddr_settings_ptr->ca_parity == CA_PARITY_DRAM) ||
            (mddr_settings_ptr->ca_parity == CA_PARITY_DRAM_DIMM))
        {
            // DRAM parity
            // PL per Table 42, page 69 of JEDEC DDR4 Specification(JESD79-4)
            if (ddr_rate <= 2133)
            {
                pl = 4;
                mc_mr5_pl = 1;
            }
            else if (ddr_rate <= 2666)
            {
                pl = 5;
                mc_mr5_pl = 2;
            }
            else if (ddr_rate <= 3200)
            {
                pl = 6;
                mc_mr5_pl = 3;
            }
            else        // RFU in Jedec Spec
            {
                pl = 8;
                mc_mr5_pl = 4;
            }
        }


        if (common_cfg_ptr->mod_type != SPD_MODULE_TYPE_UDIMM)
        {
            if (ddr_rate <= 2400)
            {
                rcd_nladd = 1;       // 1nCK latency addr
            }
            else
            {
                rcd_nladd = 2;       // 2nCK latency addr
            }
        }


        // tAA_DBI per Table 80, page 163 of JEDEC DDR4 Specification(JESD79-4)
        // DBI Read enabled
        if ((mddr_settings_ptr->dbi_mode == DDR_DBI_READ) ||
            (mddr_settings_ptr->dbi_mode == DDR_DBI_READ_WRITE))
        {
            if (ddr_rate < 2132) // handle half freq at 2133
            {
                tdbi = 2;   // tAA_DBI in clock cycles instead of ps. From tAA_DBI table 107, pg203 JESD79-4A
            }
            else
            {
                tdbi = 3;
            }

            // Adding tdbi to cl
            // if (common_cfg_ptr->sdram_width == SPD_SDRAM_WIDTH_X8)
            if ((common_cfg_ptr->sdram_width == SPD_SDRAM_WIDTH_X8) || (common_cfg_ptr->sdram_width == SPD_SDRAM_WIDTH_X16))
            {
                cl = cl + tdbi;
            }
        }

        // CWL per Table 6, page 16 of JEDEC DDR4 Specification(JESD79-4)
        if (ddr_rate <= 1600)
        {
            if((mddr_settings_ptr->ddr4_2t_preamble_mode == DDR4_2T_WRITE) ||
               (mddr_settings_ptr->ddr4_2t_preamble_mode == DDR4_2T_WRITE_READ))
            {
                cwl = 0xA;
                mc_mr2_cwl = 1;
            }
            else
            {
                cwl = 9;
                mc_mr2_cwl = 0;
            }
        }
        else if (ddr_rate <= 1866)
        {
            if((mddr_settings_ptr->ddr4_2t_preamble_mode == DDR4_2T_WRITE) ||
               (mddr_settings_ptr->ddr4_2t_preamble_mode == DDR4_2T_WRITE_READ))
            {
                cwl = 0xB;
                mc_mr2_cwl = 2;
            }
            else
            {
                cwl = 0xA;
                mc_mr2_cwl = 1;
            }
        }
        else if (ddr_rate <= 2133)
        {
            if((mddr_settings_ptr->ddr4_2t_preamble_mode == DDR4_2T_WRITE) ||
               (mddr_settings_ptr->ddr4_2t_preamble_mode == DDR4_2T_WRITE_READ))
            {
                cwl = 0xC;
                mc_mr2_cwl = 3;
            }
            else
            {
                cwl = 0xB;
                mc_mr2_cwl = 2;
            }
        }
        else if (ddr_rate <= 2400)
        {
            if((mddr_settings_ptr->ddr4_2t_preamble_mode == DDR4_2T_WRITE) ||
               (mddr_settings_ptr->ddr4_2t_preamble_mode == DDR4_2T_WRITE_READ))
            {
                cwl = 0xE;
                mc_mr2_cwl = 4;
            }
            else
            {
                cwl = 0xC;
                mc_mr2_cwl = 3;
            }
        }
        else if (ddr_rate <= 2666)
        {
            if((mddr_settings_ptr->ddr4_2t_preamble_mode == DDR4_2T_WRITE) ||
               (mddr_settings_ptr->ddr4_2t_preamble_mode == DDR4_2T_WRITE_READ))
            {
                cwl = 0x10;
                mc_mr2_cwl = 5;
            }
            else
            {
                cwl = 0xE;
                mc_mr2_cwl = 4;
            }
        }
        else // if (ddr_rate <= 3200)   // TBD in Jedec Spec
        {
            if((mddr_settings_ptr->ddr4_2t_preamble_mode == DDR4_2T_WRITE) ||
               (mddr_settings_ptr->ddr4_2t_preamble_mode == DDR4_2T_WRITE_READ))
            {
                cwl = 0x12;
                mc_mr2_cwl = 6;
            }
            else
            {
                cwl = 0x10;
                mc_mr2_cwl = 5;
            }
        }


        // DDR4 CAL mode related settings.

        // !! NOT supported in the controller
        if (mddr_settings_ptr->ddr4_cal_mode_en)
        {
            mddr_log(HIGH, "Command address latency not supported, not enabling!\n");

            if (ddr_rate <= 1600)
            {
                tcal = 3;
                mc_mr4_cal = 1;
            }
            else if (ddr_rate <= 2133)
            {
                tcal = 4;
                mc_mr4_cal = 2;
            }
            else if (ddr_rate <= 2400)
            {
                tcal = 5;
                mc_mr4_cal = 3;
            }
            else if (ddr_rate <= 2666)   // TBD in Jedec Spec
            {
                tcal = 6;
                mc_mr4_cal = 4;
            }
            else // if (ddr_rate <= 3200)   // TBD in Jedec Spec
            {
                tcal = 8;
                mc_mr4_cal = 5;
            }

            // For Rev 2 when CAL and Geardown are supported add optional tcal to tmod.
            tmod += tcal;

        }


    // Geardown mode needs even CL, CWL, tCAL, PL, WR, rd2Pch
    if (mddr_settings_ptr->ddr4_geardown_mode_en)
        {

            if ((cl % 2) == 1)
            {
               ++cl;
            }

            if ((rl % 2) == 1)
            {
               ++rl;
            }

            // WR and rd2Pch needs to be even
            if ((cwl % 2) == 1)
            {                           // cwl={9 or 11}
               ++cwl;
               ++mc_mr2_cwl;
            }

            if ((tcal % 2) == 1)           // odd tcal
            {
                ++tcal;
                ++mc_mr4_cal;
            }

            if ((pl % 2) == 1)
            {
                ++pl;
                ++mc_mr5_pl;
            }

            // tRTP, tWR handled in other FNs
        }
    }



    orl = rl + rcd_nladd + tcal + tdbi + pl;


    //! @todo -GPK- AL to be added to both RL and WL if/when AL is supported.
    //    Write Latency = AL+CWL+PL. Read Latency = AL+CL+PL. Per
    //     Table 42, page 69 of JEDEC DDR4 Specification(JESD79-4)
    wl = cwl + pl;


    // Record all of the latencies.
    latencies_ptr->cl = cl;
    latencies_ptr->cwl = cwl;
    latencies_ptr->pl = pl;
    latencies_ptr->tcal = tcal;
    latencies_ptr->tmod = tmod;
    latencies_ptr->tmrd = tmrd;
    latencies_ptr->orl = orl;
    latencies_ptr->rl = rl;
    latencies_ptr->wl = wl;
    latencies_ptr->tdbi = tdbi;
    latencies_ptr->mc_mr2_cwl = mc_mr2_cwl;
    latencies_ptr->mc_mr5_pl = mc_mr5_pl;
    latencies_ptr->rcd_nladd = rcd_nladd; // Currently unused / unnecessary due to PHY & MC interactions

    mddr_log(INFORMATIONAL, "       RL = 0x%2.2x, WL = 0x%2.2x, PL = 0x%2.2x, tDBI = 0x%2.2x, tMOD = 0x%2.2x, tMRD = 0x%2.2x\n", rl, wl, pl, tdbi, tmod, tmrd);
    mddr_log(INFORMATIONAL, "         CWL = 0x%2.2x, MR2_CWL = 0x%2.2x, MR5_PL = 0x%2.2x\n\n", cwl, mc_mr2_cwl, mc_mr5_pl);
}




/*!****************************************************************************
*
* @par Description:
*    This function determines the vaue for the DDR clock period timing value.
*
* @return
*    The DDR clock period timing value in ps.
*
*
* @par Notes:
*    per Table 86 of JEDEC DDR4 Specification(JESD79-4A)
*
******************************************************************************/
uint32_t mddr_get_clock_period_timing_val(void)
{
    uint32_t tck_ps;


    if (ddr_rate >= 2400)
    {
        tck_ps = 833;                  // 0.833ns
    }
    else if (ddr_rate >= 2133)
    {
        tck_ps = 938;                  // 0.938ns
    }
    else if (ddr_rate >= 1866)
    {
        tck_ps = 1071;                 // 1.071ns
    }
    else if (ddr_rate >= 1600)
    {
        tck_ps = 1250;                 // 1.250ns
    }
    else if (ddr_rate >= 1333)
    {
        tck_ps = 1500;                 // 1.500ns
    }
    else if (ddr_rate >= 1066)
    {
        tck_ps = 1875;                 // 1.875ns
    }
    else
    {
        tck_ps = 2500;                 // 2.500ns
    }

    return tck_ps;
}



/*!****************************************************************************
*
* @par Description:
*    This function determines the vaue for the tXS timing value. The time for
*     commands to exit self refresh without a locked DLL
*
*                                  @param
*    tRFC_min_ps            MAX across channels minimum tRFC in ps
*
* @return
*    The txs timing value, in clocks.
*
*
******************************************************************************/
uint32_t mddr_get_derived_txs_timing_val(uint32_t trfc_min_ps)
{
    uint32_t txs_clk=0;

    if (common_cfg.dev_type == SPD_DEVICE_TYPE_DDR4)
    {
        txs_clk = mddr_get_num_clocks((trfc_min_ps + 10000), mem_freq_ps);
        txs_clk = COM_MAX(txs_clk, 5);  // tXS=max 5ck, trfc_min_clk + 10ns
    }

    return txs_clk;
}



/*!****************************************************************************
*
* @par Description:
*    This function determines the value for the tXSDLL timing value. The
*     minimum time for commands to exit self refresh with a locked DLL.
*
* @return
*    The tsxdll timing value.
*
*
******************************************************************************/
uint32_t mddr_get_derived_txsdll_timing_val(void)
{
    uint32_t txsdll_min_clk;


    if (common_cfg.dev_type == SPD_DEVICE_TYPE_DDR4)
    {
        // txsdll = tDLLKmin
        // JEDEC Standard No. 79-4: Table 13 P. 22

        if (ddr_rate <= 1866)
        {
            txsdll_min_clk = 597;
        }
        else if (ddr_rate <= 2400)
        {
            txsdll_min_clk = 768;
        }
        else
        {
            txsdll_min_clk = 1024;
        }
    }
    else
    {
        // txsdll = tDLLKmin : 512 Clocks for DDR3
        txsdll_min_clk = 512;
    }


    return txsdll_min_clk;
}



/*!****************************************************************************
*
* @par Description:
*    This function determines the vaue for the tDLLKmin timing value. The
*     minimum DLL lock time.
*
* @return
*    The tsxdll timing value.
*
******************************************************************************/
uint32_t mddr_get_derived_tdllkmin_timing_val(void)
{
    uint32_t tdllkmin_clk;

    //! @todo -GPK- Identify JEDEC tables for these.
    if (common_cfg.dev_type == SPD_DEVICE_TYPE_DDR4)
    {
        if (ddr_rate <= 1333)
        {
            tdllkmin_clk = 597;
        }
        else if (ddr_rate <= 1866)
        {
            tdllkmin_clk = 597;
        }
        else if (ddr_rate <= 2400)
        {
            tdllkmin_clk = 768;
        }
        else if (ddr_rate <= 2666)      // TBD freq in DDR4 spec. Need to check this
        {
            tdllkmin_clk = 1024;
        }
        else
        {
            // ddr_rate > 2666 (i.e. 3200)
            tdllkmin_clk = 1024;
        }
    }
    else
    {
        tdllkmin_clk = 512;
    }


    return tdllkmin_clk;
}


/*!****************************************************************************
*
* @par Description:
*    This function determines the vaue for the tstab timing value.
*     Stabilization time - command buffer chip.
*
* @return
*    The tstab timing value.
*
******************************************************************************/
uint32_t mddr_get_derived_tstab_timing_val(void)
{
    uint32_t tstab_clk = 0x0;

    // 5us
    tstab_clk = max_of_clk_ps(0, 0x4C4B40, mem_freq_ps);
    return tstab_clk;
}


/*!****************************************************************************
*
* @par Description:
*    This function determines the vaue for the tckevmin timing value.
*     Minimum number of DDR cycles to stay low when the CK_t/CK_c are pulled low.
*
* @return
*    The tckevmin timing value.
*
******************************************************************************/
uint32_t mddr_get_derived_tckevmin_timing_val(void)
{
    uint32_t tckev_min_clk = 0x0;

    if (ddr_rate < 2400)
    {
        tckev_min_clk = 0x4;
    }
    else if (ddr_rate < 2933)
    {
        tckev_min_clk = 0x6;
    }
    else
    {
        tckev_min_clk = 0x8;
    }

    return tckev_min_clk;
}

/*!****************************************************************************
*
* @par Description:
*    This function finds max between a clock cycle value and a ps time value
*    based on frequency.
*
*                                  @param
*    i_clks         The number of clocks.
*                                  @param
*    i_ps           The number of picoseconds.
*                                  @param
*    freq_ps        The number of picoseconds per clock for a given frequency
*                   in MHz.
*
*
* @return
*    The number of clock cycles.
*
******************************************************************************/
uint32_t max_of_clk_ps(uint32_t i_clks, uint32_t i_ps, uint32_t freq_ps)
{
    uint32_t num_clks;


    num_clks = mddr_get_num_clocks(i_ps, freq_ps);
    num_clks = COM_MAX(num_clks, i_clks);


    return num_clks;
}



/*!****************************************************************************
*
* @par Description:
*    This function calculates the number of clock cycles at a given frequency
*    that would occur for a given time in picoseconds.
*
*                                  @param
*    txx_ps         The number of picoseconds to translate into clock cycles.
*                                  @param
*    freq_ps        The number of picoseconds per clock for a given frequency
*                   in MHz.
*
*
* @return
*    The number of clock cycles.
*
*
******************************************************************************/
uint32_t mddr_get_num_clocks(uint32_t txx_ps, uint32_t freq_ps)
{
    uint32_t num_clks;
    uint32_t remainder;
    uint64_t txx_ps_scaled;
    uint64_t freq_ps_scaled;


    // do guardband (-0.01 clk before ceiling FN)

    // Scale up so the frequency in ps can be used as the .01 guardband
    txx_ps_scaled = 100 * txx_ps;
    freq_ps_scaled = 100 * freq_ps;

    //  Make the adjustment for the guardband
    if (txx_ps_scaled >= freq_ps)
    {
        txx_ps_scaled -= freq_ps;
    }

    num_clks = txx_ps_scaled / freq_ps_scaled;
    remainder = txx_ps_scaled % freq_ps_scaled;

    if (remainder != 0)    //ceiling (round up if remainder)
    {
        num_clks = num_clks + 1;
    }

    return num_clks;
}



/*!****************************************************************************
*
* @par Description:
*    This function sets a value in a configuration / control register.
*
*                                  @param
*    mddr_reg_addr          The address of the configuration register to set.
*                                  @param
*    reg_name_str           String name of the register to use in displaying
*                           the register value in verbose mode.
*                                  @param
*    reg_val                The value to store into the register.
*
*
* @return
*    None
*
******************************************************************************/
void mddr_set_reg
(
    uint64_t mddr_reg_addr,
    char *reg_name_str,
    uint32_t reg_val
)
{
    uint32_t mddr_reg_addr_low;

    mddr_reg_addr_low = (uint32_t)(mddr_reg_addr & 0xFFFFFFFF);

     if (reg_report)
    {
        mddr_log(INFORMATIONAL, " Reading MDDR_%-25s (0x%08X) = 0x%08X\n",
                 reg_name_str, mddr_reg_addr_low, reg_val);
    }
    else
    {
           mddr_log(INFORMATIONAL, "     Setting %-25s = 0x%x\n", reg_name_str, reg_val);
    }

    write32(mddr_reg_addr, reg_val);
}



/*!****************************************************************************
*
* @par Description:
*    This function gets a value from a configuration / control register.
*
*                                  @param
*    mddr_reg_addr          The address of the memory controller configuration
*                           register to set.
*                                  @param
*    reg_name_str           String name of the register to use in displaying
*                           the register value in verbose mode.
*
* @return
*    The value stored in the register.
*
******************************************************************************/
uint32_t mddr_get_reg
(
    uint64_t mddr_reg_addr,
    char *reg_name_str
)
{
    uint32_t reg_val;
    uint32_t mddr_reg_addr_low;


    mddr_reg_addr_low = (uint32_t)(mddr_reg_addr & 0xFFFFFFFF);

    reg_val = read32(mddr_reg_addr);

    if (reg_report)
    {
        mddr_log(INFORMATIONAL, " Reading MDDR_%-25s (0x%08X) = 0x%08X\n",
                 reg_name_str, mddr_reg_addr_low, reg_val);
    }
    else
    {
        mddr_log(INFORMATIONAL, "     Reading %-28s = 0x%08x\n", reg_name_str, reg_val);
    }

    return reg_val;
}


/*!****************************************************************************
*
* @par Description:
*    This function manipulates specific fields of a configuration /
*     control register.
*
*                                  @param
*    mddr_reg_addr          The address of the memory controller configuration
*                           register to manipulate.
*                                  @param
*    reg_name_str           String name of the register to use in displaying
*                           the register value in verbose mode.
*                                  @param
*    mask
*                                  @param
*    set_bits
*
* @return
*    None.
*
*
******************************************************************************/
void mddr_rmw_reg
(
    uint64_t mddr_reg_addr,
    char *reg_name_str,
    uint32_t mask,
    uint32_t set_bits
)
{
    uint32_t reg_val;


    reg_val = mddr_get_reg(mddr_reg_addr, reg_name_str);

    // Clear out previous bits.
    reg_val &= ~(mask);
    reg_val |= set_bits;

    mddr_set_reg(mddr_reg_addr, reg_name_str, reg_val);
}



/*!****************************************************************************
*
* @par Description:
*    This function samples a register, waiting for a specific value for a set
*     of bits read from the register.
*
*                                  @param
*    mddr_base_addr         The base address of the configuration registers for
*                            the memory controller.
*                                  @param
*    s_addr_offset          Address offset of the register to sample.
*                                  @param
*    mask                   Mask to apply to the data to isolate the bits of
*                            interest.
*                                  @param
*    value                  The value that should eventually be reflected in
*                            the bits being inspected.
*                                  @param
*    to_ms                  Timeout in ms.
*
*
* @return
*    None
*
*
******************************************************************************/
void mddr_rd_chk_reg
(
    uint64_t mddr_base_addr,
    uint64_t s_addr_offset,
    uint64_t mask,
    uint32_t value,
    uint32_t to_ms
)
{
    uint32_t reg_value;
    uint32_t wait_time_us = 0;
    uint32_t wait_time_ms;
    uint32_t tick_marks = 0;


    do
    {
        // Delay for 1us before each read to avoid beating on the target register and
        //  to provide an easy way to track the time to allow the timeout.
//        mddr_mc_wait_us(mddr_base_addr, 1);

        ++wait_time_us;
        wait_time_ms = wait_time_us / MICRO_SEC_PER_MILLI_SEC;

        // Provide feedback for long delays
        if((wait_time_us % 10000) == 0x0)
        {
            if(tick_marks == 20)
            {
                tick_marks = 0;
                mddr_log(LOW, ". \n");
            }
            else
            {
                ++tick_marks;
                mddr_log(LOW, ". ");
            }
        }

        // Extract only the bits of interest from the register.
        reg_value = read32(mddr_base_addr + s_addr_offset) & mask;
    } while((reg_value != value) && ( wait_time_ms < to_ms));

    mddr_log(LOW, "\n\n");

    if(reg_value != value)
    {
        mddr_log(HIGH, "    Timeout waiting on read val : Expected 0x%8.8x  Got 0x%8.8x\n", value, reg_value);
    }
}




/*!****************************************************************************
*
* @par Description:
*    This function polls a field of a register until the field is cleared.
*
*                                  @param
*    mddr_base_addr   The base address of the configuration registers for
*                     the memory controller.
*                                  @param
*    mddr_reg_addr   The address of the memory controller configuration
*                     register to poll.
*                                  @param
*    reg_mask        The mask to apply to the register to isolate the target
*                     field.
*
*
* @return
*    None
******************************************************************************/
void mddr_wait_for_clear(uint64_t mddr_base_addr, uint64_t mddr_reg_addr, uint32_t reg_mask, uint8_t max_loops)
{
    uint32_t reg_val = 0x0;
    uint8_t i = 0;

    do
    {
        reg_val = read32(mddr_reg_addr);
        reg_val &= reg_mask;

       // mddr_mc_wait_us(mddr_base_addr, 1);
        i++;
    } while((i < max_loops) && (reg_val == 0x0));
}



/*!****************************************************************************
*
* @par Description:
*    This function polls a field of a register until the field is set.
*
*                                  @param
*    mddr_base_addr   The base address of the configuration registers for
*                     the memory controller.
*                                  @param
*    mddr_reg_addr   The address of the memory controller configuration
*                     register to poll.
*                                  @param
*    reg_mask        The mask to apply to the register to isolate the target
*                     field.
*
* @return
*    None
******************************************************************************/
void mddr_wait_for_set(uint64_t mddr_base_addr, uint64_t mddr_reg_addr, uint32_t reg_mask, uint8_t max_loops)
{
    uint32_t reg_val = 0x0;
    uint8_t i = 0;

    do
    {
        reg_val = read32(mddr_reg_addr);
        reg_val &= reg_mask;

    //    mddr_mc_wait_us(mddr_base_addr, 1);
        i++;
    } while((i < max_loops) && (reg_val == 0x0));
}

/*!****************************************************************************
*
* @par Description:
*    This function checks the current configuration to determine if the channel
*     is populated with DIMMs.
*
*                                  @param
*    ddr_chan       The channel to check.
*
*
* @return
*    Boolean indication of if the channel has any DIMMs.
*
******************************************************************************/
bool mddr_check_chan_populated(uint8_t ddr_chan)
{
    uint8_t dimm_slot;
    bool chan_populated;

    chan_populated = false;

    for(dimm_slot = 0; dimm_slot < DDR_MAX_NUM_SLOTS_PER_CHAN; ++dimm_slot)
    {
        if(mddr_dimm_configs[ddr_chan][dimm_slot].dev_type != SPD_DEVICE_TYPE_UNDEFINED)
        {
            chan_populated = true;
        }
    }

    return chan_populated;
}

/*!****************************************************************************
*
* @par Description:
*    This function determines the number of DIMMs in a populated channel.
*    The count is expected to be the same for all populated channels.
*
* @return
*    Count of the number of DIMMs populated in first populated channel.
*
******************************************************************************/
uint8_t mddr_get_dimm_per_chan_cnt(void)
{
    uint8_t ddr_chan;
    uint8_t dimm_slot;
    uint8_t num_dimm_slots_pop = 0;

    ddr_chan = 0;

    // Count the DIMMs in the first channel that is populated.
    while((num_dimm_slots_pop == 0) && (ddr_chan < DDR_MAX_NUM_CHANS))
    {
        // Check the channel's slots for DIMMs
        for (dimm_slot = 0; dimm_slot < DDR_MAX_NUM_SLOTS_PER_CHAN; ++dimm_slot)
        {
            // Slot populated, count it.
            if(mddr_dimm_configs[ddr_chan][dimm_slot].dev_type != SPD_DEVICE_TYPE_UNDEFINED)
            {
                ++num_dimm_slots_pop;
            }
        }

        ++ddr_chan;
    }

    return num_dimm_slots_pop;
}

/*!****************************************************************************
*
* @par Description:
*    This function determines the CAS latency using the provided configuration
*    and settings.
*
*                                  @param
*    common_cfg_ptr         The pointer to the structure containing the
*                            configuration information.
*
*
* @return
*    The CAS latency in clock cycles.
* @par Notes:
*    The configuration reflects what DIMMs are present in the system and the
*    settings reflect the desired configurable options.
*
* @verbatim
*     400   533   666   800    933   1066   1200   1333   1600 MHz clock
*     800  1066  1333  1600   1866   2133   2400   2666   3200 MT/s
*    (2.5, 1.876, 1.5, 1.25, 1.071, 0.938, 0.833, 0.750, 0.625 ns)
* @endverbatim
*
******************************************************************************/
uint32_t mddr_get_cas_latency
(
    mddr_common_cfg_t *common_cfg_ptr
)
{
    uint32_t default_cas_latency;
    uint32_t max_cas_latency = 0;
    uint32_t cl;
    unsigned status = COM_OK;
    uint32_t sup_cl_start=0;
    uint32_t cl_in_sup;
    uint32_t taamax_ps;
    bool found = false;


    //==============================================================================
    //  1. AND in the CAS_LAT_SUPPORTED of each dimm (get common CL)
    //  2. find largest tAAmin
    //  3. find largest tCKmin (common highest supported freq)
    //  4. if desired freq(tCKproposed) not standard JEDEC value
    //
    //   400   533   666   800    933   1066   1200   1333   1466   1600  MHz clock
    //   800  1066  1333  1600   1866   2133   2400   2666   2933   3200  MT/s
    //  (2.5, 1.876, 1.5, 1.25, 1.071, 0.938, 0.833, 0.750, 0.682, 0.625  ns) choose lower of the 2
    //  DDR3------------------------------------------------------------------------
    //    6     7     9     11     13     14  15-16?     ?                CAS latency
    //    5     6     7      8      9     10     11     12                CAS write latency
    //  DDR4------------------------------------------------------------------------
    //                      11     13     15  16-18  18-20  20-22  22-24  CAS latency
    //                       9     10     11    12      14     16     16  CAS write latency
    //
    //==============================================================================

    if (common_cfg_ptr->dev_type == SPD_DEVICE_TYPE_DDR4)
    {
          // DDR4
        default_cas_latency = DDR4_DEFAULT_CAS_LATENCY;

        if ((mddr_timing_vals.cas_lat_sup >> 31) == 0x0)
        {
            // CAS Latency low range
            max_cas_latency = DDR4_CL_MAX_LR;
            sup_cl_start = DDR4_CL_START_LR;   // CL starts at 7
        }
        else
        {
            // CAS Latency high range
            max_cas_latency = DDR4_CL_MAX_HR;
            sup_cl_start = DDR4_CL_START_HR;   // CL starts at 17
        }

        taamax_ps = DDR4_TAAMAX;
    }


    if(platform_mode == SILICON)
    {
        cl = default_cas_latency;

        if (mem_freq_ps < 625)
        {
            mddr_log(HIGH, "Error too fast DIMM speed desired\n");
            status = COM_ERROR;
        }
        else if (mddr_timing_vals.tck_min_ps > mem_freq_ps)
        {
            mddr_log(HIGH, "ERROR : Desired frequency is faster than DIMMs support\n");
            status = COM_ERROR;
        }
        else if (mem_freq_ps > mddr_timing_vals.tck_max_ps)
        {
            mddr_log(HIGH, "Warning : Desired frequency is slower than DIMMs support\n");
            status = COM_ERROR;
        }
        else
        {
            cl = mddr_get_num_clocks(mddr_timing_vals.taa_min_ps, mem_freq_ps);
        }

        // Find the closest supported CAS latency.
        do
        {
            // Convert the CAS latency to match format in supported CAS Latency flags bitmap.
            cl_in_sup = (1 << (cl - sup_cl_start));

            if((cl_in_sup & (mddr_timing_vals.cas_lat_sup)) == 0)
            {
                // CAS latency is not in the common supported list so move to the next value
                ++cl;
            }
            else
            {
                found = true;
            }
        } while((cl < max_cas_latency) && (!found));


        // Check for CAS latency in CL supported bitmap(cas_lat_sup) to determine if it is a CL supported by the DIMMs.
        if (!found)
        {
            mddr_log(HIGH, "ERROR : Supported CAS Latency not found, supported bitmap=0x%8.8x\n",
                   mddr_timing_vals.cas_lat_sup);
            mddr_log(HIGH, "    ERROR :  Need to choose higher tCK (slower speed)\n");
            status = COM_ERROR;
        }
        else if (taamax_ps < (cl * mem_freq_ps))
        {
            mddr_log(HIGH, "ERROR TAAMAX exceeded for this CL & frequency combination\n");
            status = COM_ERROR;
        }

        if (status == COM_ERROR)
        {
            cl = default_cas_latency;   // default if in error.
            mddr_log(HIGH, "ERROR : Finding valid CAS Latency for freq, defaulting to %d\n", cl);
        }
    }
    else
    {
        cl = default_cas_latency;
    }

    mddr_log(INFORMATIONAL, "CAS Latency of %d selected for freq = %d MHz\n\n", cl, mem_freq_mhz);

    return cl;
}


/*!****************************************************************************
*
* @par Description:
*    This function determines the maximum clock rate supported by all DIMMs
*     populated.
*
* @return
*    Highest common clock rate in MHz.
*
******************************************************************************/
uint16_t mddr_get_dimms_max_clk(void)
{
    uint16_t max_clk = 1600;
    uint16_t dimm_clk;
    uint8_t ddr_chan;
    uint8_t dimm_slot;


    // Go through all channels and slots searching for the greatest common DIMM clock rate supported
    for(ddr_chan = 0; ddr_chan < DDR_MAX_NUM_CHANS; ++ddr_chan)
    {
        for(dimm_slot = 0; dimm_slot < DDR_MAX_NUM_SLOTS_PER_CHAN; ++dimm_slot)
        {
            // Check to see if there is a DIMM in the slot
            if(mddr_dimm_configs[ddr_chan][dimm_slot].dev_type != SPD_DEVICE_TYPE_UNDEFINED)
            {
                // Get the max clock rate of the DIMM
                dimm_clk = MDDR_PS_TO_MHZ(mddr_dimm_configs[ddr_chan][dimm_slot].tck_min_ps);

                // Get the common clock rate.
                max_clk = COM_MIN(dimm_clk, max_clk);
            }
        }
    }


    // Align the rate to expected clock bins.
    if (max_clk >= 1333)
    {
        max_clk = 1333;
    }
    else if (max_clk >= 1200)
    {
        max_clk = 1200;
    }
    else if (max_clk >= 1066)
    {
        max_clk = 1066;
    }
    else if (max_clk >= 933)
    {
        max_clk = 933;
    }
    else if (max_clk >= 800)
    {
        max_clk = 800;
    }
    else
    {
        // The rate is less that the min bin expected so just take it as is.
    }

    return max_clk;
}



/*!****************************************************************************
*
* @par Description:
*    This function records the contents of the provided MDDR settings structure
*     into the internal MDDR settings.
*
*                                  @param
*    settings_ptr         Pointer to MDDR settings structure that is
*                          the destination of the copy.
*
* @return
*    None
*
******************************************************************************/
unsigned com_blk_copy(uint8_t *src_ptr, uint8_t *dst_ptr, uint16_t size_bytes);
void mddr_record_init_parms(mddr_settings_t *settings_ptr)
{
    uint16_t size_bytes;
    uint8_t *src_ptr;
    uint8_t *dst_ptr;

    size_bytes = sizeof(mddr_settings_t);
    src_ptr = (uint8_t *)settings_ptr;
    dst_ptr = (uint8_t *)&mddr_settings;

    com_blk_copy(src_ptr, dst_ptr, size_bytes);
}

/*!****************************************************************************
*
* @par Description:
*    This locks down the hardware for unused channels.
*
*                                  @param
*    ddr_chan           DDR channel to perform function on.
*
*
* @return
*    None
*
*
******************************************************************************/
void mddr_lockdown_unused_chan(uint8_t ddr_chan)
{
    if (platform_mode == SILICON)
    {
    }
}

/*!****************************************************************************
*
* @par Description:
*    This function determines the DDR type which is used to distinguish
*     between DDR3 and DDR4.
*
******************************************************************************/
uint32_t mddr_determine_ddr_type(target_mode_e mode, uint8_t ddr_chan)
{
    uint32_t ddr_type = 0x4;
    return ddr_type;
}
