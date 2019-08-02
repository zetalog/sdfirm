/*!****************************************************************************
* @file mddr.h
*
* @par Description:
*    This header contains the prototypes, derived data types, etc. to allow the
*    initialization of the Monaco DDR Subsystem.
*
******************************************************************************/
#ifndef _MODULE_MDDR_H_
#define _MODULE_MDDR_H_
/******************************************************************************
* Included Headers
******************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include "spd.h"

/******************************************************************************
* Defined Constants
******************************************************************************/
#define UNUSED_PARAM                    __attribute__((unused))

#define MILLI_SEC_PER_SEC        1000
#define MICRO_SEC_PER_MILLI_SEC 1000
#define NANO_SEC_PER_MICRO_SEC  1000
#define PICO_SEC_PER_NANO_SEC   1000
#define PICO_SEC_PER_MICRO_SEC   (PICO_SEC_PER_NANO_SEC * NANO_SEC_PER_MICRO_SEC)
#define FEMTO_SEC_PER_PICO_SEC   1000

#define MB_PER_GB               1024
#define KB_PER_MB            1024
#define BYTES_PER_KB            1024
#define BITS_PER_BYTE           8

/******************************************************************************
* Defined Constants
******************************************************************************/
#define MDDR_HDDRSS2_ADDR                  0xFF04000000  // Chan 0, MCSU0
#define MDDR_HDDRSS3_ADDR                  0xFF06000000  // Chan 1, MCSU1


#define MDDR_DIMM_TEMP_INVALID             0x1000
#define MB_PER_GB               1024
#define KB_PER_MB            1024

#define MAX_1DIMM_FREQ_MHZ_SILICON          1333
#define MAX_2DIMM_FREQ_MHZ_SILICON          1200

#define MAX_1DIMM_FREQ_MHZ_RUMI             3
#define MAX_2DIMM_FREQ_MHZ_RUMI             3

#define MDDR_XO_FREQ_KHZ_SILICON            20000   // 20 MHz
#define MDDR_XO_FREQ_KHZ_RUMI               332     // 2.66MHz/8

#define PROP_DELAY_MIN                    0x0
#define PROP_DELAY_MAX                    0xFF

/*!****************************************************************************
*
* @par Description
*    For memory writes, this field specifies the number of DDR cycles that
*     extra DQS toggles are appended to the end of the write data burst.
*
* @par Members
*                                  <PRE>
*    EXTRA_DQS_TOGGLE_MIN       Minumum value supported by the driver
*    EXTRA_DQS_TOGGLE_MAX       Maximum value supported by the driver
*                                  </PRE>
*
******************************************************************************/
#define EXTRA_DQS_TOGGLE_MIN              0x0
#define EXTRA_DQS_TOGGLE_MAX              0x7


/*!****************************************************************************
*
* @par Description
*    Maximum DDR frequency values (MHz) as per 1DPC/2DPC silicon configurations
*     and DDR frequency for emulation
*
******************************************************************************/
#define MAX_2DPC_CLK                      1200
#define MAX_1DPC_CLK                      1333
#define MAX_EMU_CLK                       3


/*!****************************************************************************
*
* @par Description
*    DDR rank configuration for single, dual and quad chip select DIMMs
*
******************************************************************************/
#define DDR_RANKS_SINGLE_CS_DIMM          1
#define DDR_RANKS_DUAL_CS_DIMM            2
#define DDR_RANKS_QUAD_CS_DIMM            4

/******************************************************************************
* Derived data types
******************************************************************************/
/*!****************************************************************************
*
* @par Description
*    This type provides the recognizable names for target frequencies to use
*     for DDR dynamic frequenct switch.
*
* @par Notes:
*    Generic names are used because the actual operating frequencies are
*     determined at boot time.
*
* @par Members
*
*                                  <PRE>
*    DDR_FREQ_1         First of the target DDR operating frequencies.
*    DDR_FREQ_2         Second of the target DDR operating frequencies.
*    DDR_FREQ_3         Thrid of the target DDR operating frequencies.
*    DDR_FREQ_4         Fourth of the target DDR operating frequencies.
*    DDR_FREQ_5         Fifth of the target DDR operating frequencies.
*    DDR_FREQ_6         Sixth of the target DDR operating frequencies.
*    DDR_FREQ_7         Seventh of the target DDR operating frequencies.
*    DDR_FREQ_8         Eighth of the target DDR operating frequencies.
*    DDR_FREQ_9         Ninth of the target DDR operating frequencies.
*    DDR_FREQ_ID_MAX    Max number of identified/named target frequencies.
*                                  </PRE>
*
******************************************************************************/
typedef enum
{
    DDR_FREQ_1,
    DDR_FREQ_2,
    DDR_FREQ_3,
    DDR_FREQ_4,
    DDR_FREQ_5,
    DDR_FREQ_6,
    DDR_FREQ_7,
    DDR_FREQ_8,
    DDR_FREQ_9,
    DDR_FREQ_ID_MAX
}mddr_freq_ind_t;

typedef enum
{
    DDR_DIST_GRANULE_256B,      //!< Selects a granule size of 256 Bytes.
    DDR_DIST_GRANULE_4KB,       //!< Selects a granule size of 4 KBytes.
    DDR_DIST_GRANULE_8KB,       //!< Selects a granule size of 8 KBytes.
    DDR_DIST_GRANULE_NUM_TYPES  //!< Number of granule size configuration options.
} mddr_ddr_intlv_t;

typedef enum
{
    DDR_DBI_OFF,        //!< Selects DBI off.
    DDR_DBI_READ,       //!< Selects DBI for reads only.
    DDR_DBI_WRITE,      //!< Selects DBI for writes only.
    DDR_DBI_READ_WRITE, //!< Selects DBI for both reads and writes.
    DDR_DBI_NUM_TYPES   //!< Number of data bus inversion configuration options.
} mddr_ddr_dbi_t;

typedef enum
{
    CA_PARITY_DISABLE,      //!< Selects CA parity disabled.
    CA_PARITY_DRAM,         //!< Selects DRAM CA parity (DDR4) enabled.
    CA_PARITY_DIMM,         //!< Selects DIMM CA parity (L/RDIMM) enabled.
    CA_PARITY_DRAM_DIMM,    //!< Selects DIMM and DRAM CA parity enabled.
    CA_PARITY_NUM_TYPES     //!< Number of CA parity configuration modes.
} mddr_ca_parity_t;

typedef enum
{
    DB_BURST_LEN_OTF,       //!< OTF - On The Fly : BL8 & BC4
    DB_BURST_LEN_BL8,       //!< BL8 - Burst Length 8
    DB_BURST_LEN_NUM_TYPES  //!< Number of burst length coniguration modes.
} mddr_burst_len_t;


typedef enum
{
    DDR_PAGE_POLICY_CLOSED,     //!< Close the page as soon as there are no outstanding commands pending for the page.
    DDR_PAGE_POLICY_OPEN,       //!< Hold the page open until there are no outstanding commands pending for the page and either there is a command for another page in the same bank or the page is due a refresh.
    DDR_PAGE_POLICY_TIMER,      //!< Same as open page policy except the page will also be closed when the pages idle timer expires.
    DDR_PAGE_POLICY_NUM_TYPES   //!< Number of page policy modes.
} mddr_page_policy_t;


typedef struct
{
    bool enable_auto_pchg;
    mddr_burst_len_t burst_len_mode;
    mddr_page_policy_t page_policy;
    uint16_t page_idle_timer;
} mddr_page_settings_t;



typedef enum
{
    DDR_REFRESH_1X = 0,     //!< 1X DDR refresh command mode.
    DDR_REFRESH_2X = 1,     //!< 2X DDR refresh command mode.
    DDR_REFRESH_4X = 2      //!< 4X DDR refresh command mode.
} mddr_rfc_cmd_mode_t;


typedef enum
{
    DDR4_2T_DISABLE,    //!< Selects DDR4 2Tck preamble disabled.
    DDR4_2T_READ,       //!< Selects DDR4 2Tck preamble for reads enabled.
    DDR4_2T_WRITE,      //!< Selects DDR4 2Tck preamble for writes enabled.
    DDR4_2T_WRITE_READ, //!< Selects DDR4 2Tck preamble for writes and reads enabled.
    DDR4_2T_NUM_TYPES   //!< Number of DDR4 2Tck preamble configuration modes.
} mddr_ddr4_2t_preamble_t;


/*!****************************************************************************
*
* @par Description
*    This type defines the number of stretch cycles for the CA bus before
*     asserting the CS_n.
*
******************************************************************************/
typedef enum
{
    CA_STRETCH_1T = 0,  //!< Selects 1 stretch cycles for the CA bus.
    CA_STRETCH_2T = 1,  //!< Selects 2 stretch cycles for the CA bus.
    CA_STRETCH_3T = 2,  //!< Selects 3 stretch cycles for the CA bus.
    CA_STRETCH_4T = 3   //!< Selects 4 stretch cycles for the CA bus.
} mddr_ca_stretch_t;


/*!****************************************************************************
*
* @par Description
*    This type defines the ECC WR/RD configuration values.
*
******************************************************************************/
typedef enum
{
    ECC_WR     = 0x01,
    ECC_RD     = 0x02,
    RS_WR      = 0x04,
    RS_RD      = 0x08
} mddr_ecc_wrrd_t;


/*!****************************************************************************
*
* @par Description
*    This type defines the possible array of DDR frequencies.
*    Note: As per the plan of record, only a subset of these DDR frequencies
*          are supported.
*
******************************************************************************/
typedef enum
{
    DDR_FREQ_IDX_625_MHZ    = 0,
    DDR_FREQ_IDX_660_MHZ    = 1,
    DDR_FREQ_IDX_800_MHZ    = 2,
    DDR_FREQ_IDX_933_MHZ    = 3,
    DDR_FREQ_IDX_1066_MHZ   = 4,
    DDR_FREQ_IDX_1200_MHZ   = 5,
    DDR_FREQ_IDX_1333_MHZ   = 6,
    DDR_FREQ_IDX_1467_MHZ   = 7,
    DDR_FREQ_IDX_1600_MHZ   = 8,
    DDR_FREQ_IDX_MAX        = 9
} mddr_mem_freq_mhz_t;


/*!****************************************************************************
*
* @par Description
*    This type defines the bank group address decode types for the
*    Bank group address bits[1:0] of the address decode control register
*
******************************************************************************/
typedef enum
{
    ADDR_DECODE_BG_MCA_16_15   = 0x0, // BG[1:0] are at MCA[16:15]
    ADDR_DECODE_BG_MCA_8_7     = 0x1, // BG[1:0] move to MCA[8:7]
    ADDR_DECODE_BG_MCA_16_7    = 0x2, // BG[1:0] move to MCA[16],MCA[7]
    ADDR_DECODE_BG_R_C_RK_B_BG = 0x3,  // Row, Column[9:4], Rank, Bank, Bank Group, Column[3:0] order
    ADDR_DECODE_BG_NUM_TYPES           //!< Number of address decode bank group types.
} mddr_bg_decode_type_t;

/*!****************************************************************************
*
* @par Description
*    This type provides a container to configure DDR PHY for addressing New DDR
*    training flow and right eye margin fix introduced by Hynix DDR DIMM issue.
*    In conjunction with PHY 0.94version, below settings are tunable.
*
* @par Members
*
*                                  <PRE>
*    ddrphy_mem40             Tuning value for PHY dmem40 register.
*    ddrphy_mem41             Tuning value for PHY dmem41 register.
*    ddrphy_mem42             Tuning value for PHY dmem42 register.
*    ddrphy_mem43             Tuning value for PHY dmem43 register.
*    ddrphy_xcnt              Xcnt value for Rd2D Read Group Sizes. This value
*                              combined to bit[1:0] of dmem41 register
*                                  </PRE>
*
******************************************************************************/

typedef struct
{
    uint16_t ddrphy_mem40;
    uint16_t ddrphy_mem41;
    uint16_t ddrphy_mem42;
    uint16_t ddrphy_mem43;
    uint16_t ddrphy_xcnt;
}mddr_phy_config_t;

/*!****************************************************************************
*
* @par Description
*    This type provides a container to group all for the high-level
*    configuration controls available to adjust the memory controller
*    initialization. Include both the hardware dependent settings and
*    the selectable / configurable options.
*
* @par Members
*
*                                  <PRE>
*    mem_freq_mhz               Array of target operation frequencies of the memory.
*                                Must be filled in sequentially. I.e. Skipping entries
*                                is not permitted and all unused entries must be filled
*                                in with the marker value DDR_FREQ_INVALID.
*                                This is the desired frequency
*    max_1dimm_freq_mhz         Max DDR clock set by software for 1 DIMM poped.
*                                This applies to 1DPC and 2DPC 1DIMM poped
*    max_2dimm_freq_mhz         Max DDR clock set by software for 2 DIMMs poped.
*                                This applies to 2DPC.
*    mddr_xo_freq_khz           Reference clock frequency.
*    allow_overclocking         Flag indicating if the DIMM speed grade should be ignored
*                                and the DIMMs forced to run at the requested frequency.
*    disable_ecc                Flag indicating if any DIMM ecc should be disabled.
*    ecc_wrrd_mode              Indicates the ECC read/write configuration,
*                                valid only if DIMM ecc is enabled
*    rs_quad_det_en             Indicates the Reed-Solomon quad detection configuration
*                                Valid only if ecc_wrrd_mode has RS_RD
*                                RS_QUAD_DET_DISABLE = single/double symbol correct, triple symbot detect
*                                RS_QUAD_DET_ENABLE = single symbol correct, double/triple/quad symbol detect
*    disable_patrol_scrub       Flag indicating if the ECC patrol scrub should be
*                                disabled when ECC is enabled. Only the initial
*                                scrub for ECC coherency will be performed.
*    comp_settings              Configuration structure for compression/decompression;
*                                valid only if 72bit DIMM
*    mddr_scrambling_en         Flag indicating if scrambling is to be enabled.
*    dbi_mode                   Indicates the DBI mode to use.
*    ca_parity                  Flag indicating if CA parity is to be enabled.
*    page_settings              Indicates the page related settings to use.
*    ddr_granule                Indicates the interleave mode / granule to use.
*    dimm_intlv_en              Flag indicating if DIMM interleaving is to be enabled.
*                                This field is for future expansion and the feature is
*                                currently unsupported.
*    rfc_mode                   Indicates the DDR refresh command rate.
*    auto_self_refresh_en       Flag indicating if memory controller based auto self
*                                refresh mode is to be enabled.
*    sr_entry_idle_delay        Self-refresh idle time. This is the delay, in number of DDR cycles,
*                                from the time that the controller is idle to the time that the
*                                controller puts the DRAM into self-refresh mode.
*    auto_power_down_en         Flag indicating if memory controller based auto power
*                                down mode is to be enabled.
*    pd_entry_idle_delay        Power-down idle time. This is the delay in number of DDR cycles
*                                from the time that the controller is idle to the time that the
*                                controller puts the DRAM into power down mode.
*    adj_ref_dimm0              Adjustment of DIMM0's refresh rate.
*    adj_ref_dimm1              Adjustment of DIMM1's refresh rate.
*    dfi_lp_mode                Indicates the PHY DFI Low Power Mode configuration.
*    dfi_lp2_entry_idle_delay   This is the delay, in number of DDR cycles, from the time
*                                the system enters-self-refresh until the controller puts the
*                                DDR PHY into DFI-LP2 state.  If the delay is non-zero, and if
*                                DFI-LP is enabled for self-refresh, then the PHY will enter
*                                DFI-LP state first.
*    ddr4_2t_preamble_mode      Defines the DDR4 2T preamble mode configuration to use.
*    num_dimm_slots             Number of DIMM slots physically present on a DDR
*                                channel. This is independent of if the slots are
*                                populated or not.
*    periodic_zq_cal_en         Flag indicating if periodic ZQ cals should be done
*                                by the MC.
*    shared_dimm_clk            Flag indicating if the clock is shared between the
*                                channels DIMMs. Only meaningful if the platform has
*                                2 slots per channel.
*    prop_delay                 Timing for propagation delay on DRAM DQ bus
*    extra_dqs_toggle           Number of extra DQS toggles to be appended to the end
*                                of the write data burst.
*    ca_stretch                 Indicates the number of stretch cycles for the CA bus
*                                before asserting the CS_n
*    ddr4_cal_mode_en           Flag indicating if DDR4 CAL mode is to be enabled.
*    ddr4_geardown_mode_en      Flag indicating if DDR4 geardown mode is to be enabled.
*    ddr4_wr_crc_en             Flag indicating if DDR4 write CRC mode is to be enabled.
*    therm_throttle_en          Flag indicating if thermal throttling is to be enabled.
*    clk_type                   Indicates the clock source to be used for MDDR.
*                                0x0 = Clkgen (Internal); 0x1 = Agera (External)
*    clkgen_ssc_en              Flag indicating if ClkGen Spread Spectrum Clocking
*                                is to be enabled.
*    bank_group_decode          Indicates the bank group address decode type
*    bum_settings               Bus Utilization Monitor event settings.
*    tcrit_event_en             Indicates SPD Temperature Sensor (TS) Critical
*                                Temperature (TCRIT) event is to be enabled.
*    tcrit_threshhold           Indicates SPD Temperature Sensor (TS) Critical
*                                Temperature (TCRIT) Limit in degrees C.
*    ecc_ce_threshold           Indicates ECC CE counter threshold when accumulated number of CE
*                                reaches threshold, ERROR_ECC_EC_TOTAL_CNTR is tirggered.
*                                  </PRE>
*
******************************************************************************/
typedef struct
{
    uint16_t mem_freq_mhz[DDR_FREQ_ID_MAX];
    uint16_t max_1dimm_freq_mhz;
    uint16_t max_2dimm_freq_mhz;
    uint16_t mddr_xo_freq_khz;
    bool allow_overclocking;
    bool disable_ecc;
    mddr_ecc_wrrd_t ecc_wrrd_mode;
    uint8_t rs_quad_det_en;
    bool disable_patrol_scrub;
    bool mddr_scrambling_en;
    mddr_ddr_dbi_t dbi_mode;
    mddr_ca_parity_t ca_parity;
    mddr_page_settings_t page_settings;
    mddr_ddr_intlv_t ddr_granule;
    bool dimm_intlv_en;
    mddr_rfc_cmd_mode_t rfc_mode;
    bool auto_self_refresh_en;
    uint16_t sr_entry_idle_delay;
    bool auto_power_down_en;
    uint16_t pd_entry_idle_delay;
    uint16_t dfi_lp2_entry_idle_delay;
    mddr_ddr4_2t_preamble_t ddr4_2t_preamble_mode;
    uint8_t num_dimm_slots;
    bool periodic_zq_cal_en;
    bool shared_dimm_clk;
    uint8_t prop_delay;
    uint8_t extra_dqs_toggle;
    mddr_ca_stretch_t  ca_stretch;
    bool ddr4_cal_mode_en;
    bool ddr4_geardown_mode_en;
    bool ddr4_wr_crc_en;
    bool therm_throttle_en;

    bool clkgen_ssc_en;
    mddr_bg_decode_type_t bank_group_decode;
    bool tcrit_event_en;
    uint8_t tcrit_threshhold;
    uint16_t ecc_ce_threshold;
    bool CA_margin_enabled;
    mddr_phy_config_t phy_config;
} mddr_settings_t;


/*!****************************************************************************
*
* @par Description
*    This type provides a container to collect the channel specific
*    configuration information so that it can be made available to higher level
*    software.
*
* @par Members
*
*                                  <PRE>
*    num_slots_supported    Number of slots in the channel.
*    num_slots_populated    Number of slots in the channel populated with DIMMs.
*    slot_map               Map of populated DIMM slots.
*    mem_size_gb            Amount of memory in GB populated in the channel.
*                                  </PRE>
*
******************************************************************************/
typedef struct
{
    uint8_t  num_slots_supported;
    uint8_t  num_slots_populated;
    uint8_t  slot_map;
    uint32_t mem_size_gb;
} mddr_chan_cfg_t;


/*!****************************************************************************
*
* @par Description
*    This type provides a container to collect the overall MDDR configuration
*    so that it can be made available to higher level software.
*
* @par Members
*
*                                  <PRE>
*    num_chans_supported    Number of channels supported by the architecture.
*    num_chans_active       Number of channels poplated with DIMMs.
*    chan_map               Map of all active / populated channels.
*    chan_cfg               Array containing the channel specific configuration
*                            information for all channels.
*    mddr_settings          Common settings as currently configured. Primarily
*                            driven by settings setup during driver
*                            initialization.
*                                  </PRE>
*
******************************************************************************/
typedef struct
{
    uint8_t  num_chans_supported;
    uint8_t  num_chans_active;
    uint32_t chan_map;
    mddr_chan_cfg_t chan_cfg[DDR_MAX_NUM_CHANS];
    mddr_settings_t mddr_settings;
} mddr_cfg_t;


/*!****************************************************************************
*
* @par Description
*    This type provides a select subset of the SPD information extracted from
*     a DIMM. The fields contain normalized versions of the information rather
*     than the raw data extracted from the DIMM.
*
* @par Members
*
*                                  <PRE>
*    dimm_spd    Array containing key extracted SPD information from all
*                 populated DIMM slots.
*                                  </PRE>
*
******************************************************************************/
typedef struct
{
    spd_dimm_config_t dimm_spd[DDR_MAX_NUM_CHANS][DDR_MAX_NUM_SLOTS_PER_CHAN];
} mddr_spd_info_t;

/*!****************************************************************************
*
* @par Description
*    This type serves as a package to collect all all DIMM temperatures.
*
* @par Members
*
*                                  <PRE>
*    dimm_temps             Array containing ambient temperature information from
*                            each populated DIMM that contains a thermal sensor.
*    dimm_event_asserted    Array indicating whether the EVENT_n pin is asserted
*                            from each populated DIMM that contains a thermal sensor.
*                                  </PRE>
*
******************************************************************************/
typedef struct
{
    int16_t dimm_temps[DDR_MAX_NUM_CHANS][DDR_MAX_NUM_SLOTS_PER_CHAN];
    bool dimm_eventn_asserted[DDR_MAX_NUM_CHANS][DDR_MAX_NUM_SLOTS_PER_CHAN];
} mddr_dimm_therm_t;

/*!****************************************************************************
*
* @par Description
*    This type serves as a package to collect DDR sub-system state
*     information, including Power-Down, Self-Refresh, and Auto-Refresh.
*
* @par Members
*
*                                  <PRE>
*    dimm_in_pd         Array containing Power-Down state for all populated DIMMs.
*                        This will be set to true if any rank on the DIMM indicates
*                        it is in the Power-Down state.
*    dimm_in_sr         Array containing Self-Refresh state for all populated DIMMs.
*                        This will be set to true if any rank on the DIMM indicates
*                        it is in the Self-Refresh state.
*    dimm_ref_rate_adj  Array containing the Periodic Auto-Refresh DIMM Refresh Rate
*                        for all populated DIMMs.
*                                  </PRE>
*
******************************************************************************/
typedef struct
{
    bool    dimm_in_pd[DDR_MAX_NUM_CHANS][DDR_MAX_NUM_SLOTS_PER_CHAN];
    bool    dimm_in_sr[DDR_MAX_NUM_CHANS][DDR_MAX_NUM_SLOTS_PER_CHAN];
    uint8_t dimm_ref_rate_adj[DDR_MAX_NUM_CHANS][DDR_MAX_NUM_SLOTS_PER_CHAN];
} mddr_state_info_t;

/******************************************************************************
* Macros
******************************************************************************/
#define MDDR_DIMM_TEMP_WHOLE(_temp_) (_temp_ / 16)
#define MDDR_DIMM_TEMP_FRAC(_temp_)  ((((_temp_ > 0) ? _temp_ : -_temp_) % 16) * 625)
#define MDDR_GET_VAR_NAME(var) #var

#define COM_MAX(val1,val2) ((val1 > val2) ? val1 : val2)
#define COM_MIN(val1,val2) ((val1 < val2) ? val1 : val2)

/******************************************************************************
* Function Prototypes
******************************************************************************/
uint32_t mddr_determine_ddr_type(target_mode_e mode, uint8_t ddr_chan);
/*****************************************************************************/
#endif
