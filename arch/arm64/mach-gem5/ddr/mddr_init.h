/*!*****************************************************************************
* @file mddr_init.h
*
* @par Description:
*
*    This header contains the driver internal prototypes, derived data types,
*    etc. to allow the cooperation of the MDDR sub-system modules / components .
*
******************************************************************************/
#ifndef _MODULE_MDDR_INIT_H_
#define _MODULE_MDDR_INIT_H_
/******************************************************************************
* Included Headers
******************************************************************************/
#include  "spd.h"

/******************************************************************************
* Defined Constants
******************************************************************************/
#define DDR_MAX_SPEED_1DPC 1333
#define DDR_MAX_SPEED_2DPC 1200
#define DDR_MAX_SPEED_RECOVERY 800
#define DDR_MAX_SPEED_EMU 800

#define DDR4_CL_START_LR                   7          // min CAS Latency for DDR4 low range
#define DDR4_CL_START_HR                  17          // min CAS Latency for DDR4 high range
#define DDR4_CL_MAX_LR                    24          // max CAS Latency for DDR4 low range
#define DDR4_CL_MAX_HR                    34          // max CAS Latency for DDR4 high range
#define DDR4_TAAMAX                       (18 * PICO_SEC_PER_NANO_SEC)    // 18ns in ps
#define DDR4_DEFAULT_CAS_LATENCY          10

#define LRDIMM_SI_DDR_FREQ_VARNTS         3

// Maximum value of cycle adjusts for each field of the TIMING_ADJ_RDWR reg
#define DDR_CYCLES_ADJ_RDWR_MAX           0xF

#define COM_MHZ_TO_PS(freq_mhz)(1000000 / freq_mhz)
#define COM_KHZ_TO_PS(freq_khz)(1000000000 / freq_khz)
#define COM_PS_TO_MHZ(ps)(1000000 / ps)
#define COM_PS_TO_KHZ(ps)(1000000000 / ps)

#define MDDR_MHZ_TO_PS(freq_mhz)(COM_MHZ_TO_PS(freq_mhz))
#define MDDR_KHZ_TO_PS(freq_khz)(COM_KHZ_TO_PS(freq_khz))
#define MDDR_PS_TO_MHZ(ps)(COM_PS_TO_MHZ(ps))
#define MDDR_PS_TO_KHZ(ps)(COM_PS_TO_KHZ(ps))

/******************************************************************************
* Derived data types
******************************************************************************/

/*!****************************************************************************
*
* @par Description
*    This type defines the types of impedance(drive strength) the PHY supports:
*    per channel, impedance for CA[0] and DQ[1]
*    DQ 28.2, 30, 32, 34.3, 36.9, 40, 43.6, 48, 53.3 60, 68.6 80, 96, 120, 160, 240 480 ohms
*    CA 20, 24, 30, 40, 60, 120 ohms
*
******************************************************************************/
typedef enum __attribute__((packed))
{
    IMP_OHMS20 = 20,
    IMP_OHMS24 = 24,
    IMP_OHMS28 = 28,
    IMP_OHMS30 = 30,
    IMP_OHMS32 = 32,
    IMP_OHMS34 = 34,
    IMP_OHMS37 = 37,
    IMP_OHMS40 = 40,
    IMP_OHMS44 = 44,
    IMP_OHMS48 = 48,
    IMP_OHMS53 = 53,
    IMP_OHMS60 = 60,
    IMP_OHMS69 = 69,
    IMP_OHMS80 = 80,
    IMP_OHMS96 = 96,
    IMP_OHMS120 = 120,
    IMP_OHMS160 = 160,
    IMP_OHMS240 = 240,
    IMP_OHMS480 = 480
} mddr_si_imp_t;

/*!****************************************************************************
*
* @par Description
*    This type defines the types of termination (ODT) the PHY supports:
*    per channel, termination DQ
*    28.2, 30, 32, 34.3, 36.9, 40, 43.6, 48, 53.3 60, 68.6 80, 96, 120, 160, 240 480 ohms
*
******************************************************************************/
typedef enum __attribute__((packed))
{
    OHMS28 = 28,
    OHMS30 = 30,
    OHMS32 = 32,
    OHMS34 = 34,
    OHMS37 = 37,
    OHMS40 = 40,
    OHMS44 = 44,
    OHMS48 = 48,
    OHMS53 = 53,
    OHMS60 = 60,
    OHMS69 = 69,
    OHMS80 = 80,
    OHMS96 = 96,
    OHMS120 = 120,
    OHMS160 = 160,
    OHMS240 = 240,
    OHMS480 = 480
} mddr_si_term_t;


/*!****************************************************************************
*
* @par Description
*    This type provides a container to group the platform specific cycle
*     adjustments for things like trace length.
*
* @par Members
*
*                                  <PRE>
*    dimm_wtr            Additional cycles from WR to RD different DIMMs
*    dimm_wtw            Additional cycles from WR to WR different DIMMs
*    dimm_rtw            Additional cycles from RD to WR different DIMMs
*    dimm_rtr            Additional cycles from RD to RD different DIMMs
*    cs_wtr              Additional cycles from WR to RD different chip selects
*    cs_wtw              Additional cycles from WR to WR different chip selects
*    cs_rtw              Additional cycles from RD to WR different chip selects
*    cs_rtr              Additional cycles from RD to RD different chip selects
*    rank_wtr            Additional cycles from WR to RD different ranks in a chip select
*    rank_wtw            Additional cycles from WR to WR different ranks in a chip select
*    rank_rtw            Additional cycles from RD to WR different ranks in a chip select
*    rank_rtr            Additional cycles from RD to RD different ranks in a chip select
*    srank_wtr           Additional cycles from WR to RD same rank
*    srank_wtw           Additional cycles from WR to WR same rank
*    srank_rtw           Additional cycles from RD to WR same rank
*    srank_rtr           Additional cycles from RD to RD same rank
*    ref_to_ref          Additional cycles from auto-refresh to any auto-refresh command
*    zq_to_zq            Minimum number of DDR cycles from the end of last ZQ
*                         calibration to the start of the next ZQ calibration.
*                                  </PRE>
*
******************************************************************************/
typedef struct
{
    uint8_t dimm_wtr;
    uint8_t dimm_wtw;
    uint8_t dimm_rtw;
    uint8_t dimm_rtr;
    uint8_t cs_wtr;
    uint8_t cs_wtw;
    uint8_t cs_rtw;
    uint8_t cs_rtr;
    uint8_t rank_wtr;
    uint8_t rank_wtw;
    uint8_t rank_rtw;
    uint8_t rank_rtr;
    uint8_t srank_wtr;
    uint8_t srank_wtw;
    uint8_t srank_rtw;
    uint8_t srank_rtr;
    uint16_t ref_to_ref;
    uint16_t zq_to_zq;
} mddr_plat_cycle_adj_t;


/*!****************************************************************************
*
* @par Description
*    This type provides a container to group the per rank SI settings.
*    These pertain to SI settings on DRAM side
*
* @par Members
*
*                                  <PRE>
*    rtt_park                For DDR4: 0=disabled, 60, 120, 40, 240, 48, 80, 34 ohms
*                             ODT low or rtt_nom disabled
*    rtt_nom                 For DDR4: 0=disabled, 60, 120, 40, 240, 48, 80, 34 ohms
*                             ODT high
*    rtt_wr                  For DDR4: 0=disabled, 120, 240, 1 (Hi-Z), 80 ohms
*                             Any write command
*    ron                     Ron = 34 ohms, 48 ohms
*    odt_rd, odt_wr          per channel settings;  only CS0:3 are valid
*                             per rank, 4 bits each representing the 4 ODT signals
*                             (DIMM1=[3:2], DIMM0=[1:0])
*    vref                    vrefdq seetings: per channel, slot, rank, dram in hundredth
*                             percentage of VDDQ (eg 77.55%=7755)
*                                  </PRE>
*
******************************************************************************/
typedef struct
{
    uint8_t rtt_park;
    uint8_t rtt_nom;
    uint8_t rtt_wr;
    uint8_t ron;    // drive strength
    uint8_t odt_rd;
    uint8_t odt_wr;
    uint16_t vref[18];      // dq vref in integer to thousandths place, eg 50.00% = 5000
}mddr_si_rank_t;


/*!****************************************************************************
*
* @par Description
*    This type provides a container to group the per slot SI settings.
*    These pertain to SI settings on DRAM side
*
* @par Members
*
*                                  <PRE>
*    ibt                     all channels, all slots, RDIMM IBT settings (ca[0], cs[1], cke[2], odt[3])
*    rank_si                 per rank SI settings
*                                  </PRE>
*
******************************************************************************/
typedef struct
{
    uint16_t ibt[4];      // input buffer termination (L/RDIMM)
    mddr_si_rank_t rank_si[DDR_MAX_RANKS_PER_SLOT];
}mddr_si_dimm_t;

/*!****************************************************************************
*
* @par Description
*    This type provides a container to group the per channel SI settings.
*    These pertain to SI settings on Controller PHY side (host)
*
* @par Members
*
*                                  <PRE>
*    mc_slew                 per channel, slew for CA[0] and DQ[1], bits n[7:4], p[3:0]
*                             0=slowest, 0xF=fastest
*    mc_imp                  per channel, impedance for CA[0] and DQ[1]
*    mc_term                 per channel, termination DQ
*    mc_vref                 per channel, per slot, mc vrefdq settings
*    cycle_adjusts           per channel, platform specific cycle adjustments.
*    dimm_si                 per channel, per slot SI settings
*                                  </PRE>
*
******************************************************************************/
typedef struct
{
    uint8_t mc_slew[2];      // 0=CA, 1=Data
    mddr_si_imp_t mc_imp[2];        //  0=CA, 1=Data
    mddr_si_term_t mc_term;
    uint16_t mc_vref;
    mddr_plat_cycle_adj_t cycle_adjusts;
    mddr_si_dimm_t dimm_si[DDR_MAX_NUM_SLOTS_PER_CHAN];
}mddr_si_chan_t;


/*!****************************************************************************
*
* @par Description
*    This type provides key information necessary to select the specific set of
*     SI values to use when configuring a single DDR channel.
*
* @par Members
*
*                                  <PRE>
*    ddr_chan           DDR channel to perform function on.
*    mem_freq_mhz       Target operation frequency of the memory.
*    num_dimm_slots     Number of DIMM slots physically present on a DDR
*                        channel. This is independent of if the slots are
*                        populated or not. Must be 1 or 2.
*    num_dimm_slots_pop Number of DIMMs populated in a DDR channel.
*    num_ranks_slot     Array of rank counts for the the DIMMs by slot.
*                        SPD_INVALID_RANK_COUNT indicates no DIMM.
*                                  </PRE>
*
******************************************************************************/
typedef struct
{
    uint8_t ddr_chan;
    uint32_t mem_freq_mhz;
    uint8_t num_dimm_slots;
    uint8_t num_dimm_slots_pop;
    uint8_t num_ranks_slot[DDR_MAX_NUM_SLOTS_PER_CHAN];
}mddr_si_selector_t;


/*!****************************************************************************
*
* @par Description
*    This type provides a container to group the parameters associated with
*     DDR SI Binary file. This provides the start location and size of the
*     consolidated DDR SI Binary file.
*
* @par Members
*
*                                  <PRE>
*    bin_start_addr      The start address of the DDR SI binary file
*    bin_size            The size of the DDR SI binary file
*                                  </PRE>
*
******************************************************************************/
typedef struct
{
    uint64_t bin_start_addr;
    uint32_t bin_size;
}mddr_si_binary_hdr_t;


/*!****************************************************************************
*
* @par Description
*    This type defines all the parameters for LRDIMM DB (databuffer) Host side
*    interface SI values.
*
* @par Members
*
*                                  <PRE>
*    lr_host_db_rtt_park   LRDIMM DB Host interface rtt_park.
*    lr_host_db_rtt_wr     LRDIMM DB Host interface rtt_wr.
*    lr_host_db_rtt_nom    LRDIMM DB Host interface rtt_nom.
*    lr_host_db_ron        LRDIMM DB Host interface ron.
*    lr_host_db_vrefdq     LRDIMM DB Host interface VrefDQ.
*                                  </PRE>
*
******************************************************************************/
typedef struct
{
    uint8_t lr_host_db_rtt_park;
    uint8_t lr_host_db_rtt_wr;
    uint8_t lr_host_db_rtt_nom;
    uint8_t lr_host_db_ron;
    uint16_t lr_host_db_vrefdq;
}mddr_lr_host_db_si_dimm_t;


/*!****************************************************************************
*
* @par Description
*    This type defines all the parameters for LRDIMM DB (databuffer) DRAM side
*    interface SI values and LRDIMM SPD settings for DRAM-DB.
*
* @par Members
*
*                                  <PRE>
*    --------------- LRDIMM databuffer DRAM interface side settings ---------------
*    lr_spd_db_dq_dfe             LRDIMM DB DQ DFE capabilities - feedback equalization support.
*    lr_spd_db_dq_gain_adj        LRDIMM DB DQ DFE capabilities - gain adjustment support.
*    lr_spd_db_mdq_ron            LRDIMM DB drive strength for MDQ/MDQS outputs.
*    lr_spd_db_mdq_term           LRDIMM DB Read RTT termination strength.
*    lr_spd_db_mdq_vrefdq         LRDIMM DB VrefDQ.
*    lr_spd_db_mdq_vrefdq_range   LRDIMM DB VrefDQ range.
*
*    --------------- LRDIMM SPD settings for DRAM-DB ---------------
*    lr_spd_db_rtt_park01         LRDIMM DRAM ODT termination strength (rtt_park) for ranks 0,1.
*    lr_spd_db_rtt_park23         LRDIMM DRAM ODT termination strength (rtt_park) for ranks 2,3.
*    lr_spd_db_rtt_wr             LRDIMM DRAM ODT termination strength (rtt_wr).
*    lr_spd_db_rtt_nom            LRDIMM DRAM ODT termination strength (rtt_nom).
*    lr_spd_db_ron                LRDIMM DRAM output buffer drive strength.
*    lr_spd_db_vrefdq             LRDIMM DRAM VrefDQ for package rank 0-3.
*    lr_spd_db_vrefdq_range       LRDIMM DRAM VrefDQ range.
*                                  </PRE>
*
******************************************************************************/
typedef struct
{
    uint8_t lr_spd_db_dq_dfe;
    uint8_t lr_spd_db_dq_gain_adj;
    uint8_t lr_spd_db_mdq_ron[LRDIMM_SI_DDR_FREQ_VARNTS];
    uint8_t lr_spd_db_mdq_term[LRDIMM_SI_DDR_FREQ_VARNTS];
    uint8_t lr_spd_db_mdq_vrefdq;
    uint8_t lr_spd_db_mdq_vrefdq_range;

    uint8_t lr_spd_db_rtt_park01[LRDIMM_SI_DDR_FREQ_VARNTS];
    uint8_t lr_spd_db_rtt_park23[LRDIMM_SI_DDR_FREQ_VARNTS];
    uint8_t lr_spd_db_rtt_wr[LRDIMM_SI_DDR_FREQ_VARNTS];
    uint8_t lr_spd_db_rtt_nom[LRDIMM_SI_DDR_FREQ_VARNTS];
    uint8_t lr_spd_db_ron[LRDIMM_SI_DDR_FREQ_VARNTS];
    uint8_t lr_spd_db_vrefdq[DDR_MAX_RANKS_PER_SLOT];
    uint8_t lr_spd_db_vrefdq_range[DDR_MAX_RANKS_PER_SLOT];
}mddr_lr_spd_db_si_dimm_t;


/*!****************************************************************************
*
* @par Description
*    This type provides the top-level container for LRDIMM Host, databuffer
*    and DRAM interface SI values.
*
* @par Members
*
*                                  <PRE>
*    lr_host_db_si    LRDIMM DB (databuffer) Host side interface values.
*    lr_spd_db_si     LRDIMM DB DRAM side interface values & SPD settings for DRAM-DB.
*                                  </PRE>
*
******************************************************************************/
typedef struct
{
    mddr_lr_host_db_si_dimm_t lr_host_db_si[DDR_MAX_NUM_SLOTS_PER_CHAN];
    mddr_lr_spd_db_si_dimm_t lr_spd_db_si[DDR_MAX_NUM_SLOTS_PER_CHAN];
}mddr_lr_db_si_chan_t;


/*!****************************************************************************
*
* @par Description
*    This type is used to collect the translated DIMM SPD information that is
*     expected to be common accross all populated DDR channels but can differ
*     between the slots of a channel. I.e. The same slot in each channel is
*     populated with like DIMMs but the slots in a channel can differ is some
*     ways.
*
* @par Members
*
*                                  <PRE>
*    num_ranks            Number of ranks on the the DIMM.
*    num_bank_groups      Bank group indication value. DDR4 only.
*    num_rows             Number of rows.
*    num_columns          Number of columns.
*    die_count            Number of dies on the DIMM.
*    dimm_size_mb         Size of the DIMM in MB.
*    ca_mirrored          Boolean indication of if the DIMMs are CA mirrored.
*
*    rcw0f_00             RDIMM configuration words.
*    out_drv_ck_y13       RDIMM / LRDIMM clock driver Y13 signal output strength.
*    out_drv_ck_y02       RDIMM / LRDIMM clock driver Y02 signal output strength.
*
*    rdm_out_drv_ctl_a    RDIMM driver output strength for control signal A. DDR3 only.
*    rdm_out_drv_ctl_b    RDIMM driver output strength for control signal B. DDR3 only.
*    rdm_out_drv_ca_a     RDIMM drive output strength for commands/addresses signal A. DDR3 only.
*    rdm_out_drv_ca_b     RDIMM drive output strength for commands/addresses signal B. DDR3 only.
*
*    out_drv_ctl_cke      RDIMM / LRDIMM drive output strength for the CKE signal. DDR4 only.
*    out_drv_ctl_odt      RDIMM / LRDIMM drive output strength for the ODT signal. DDR4 only.
*    out_drv_ctl_ca       RDIMM / LRDIMM drive output strength for the commands/addresses signal. DDR4 only.
*    out_drv_ctl_cs       RDIMM / LRDIMM drive output strength for the chip select signal. DDR4 only.
*
*    lrdm_db_drv_ck_bcom_bodt_bcke    LRDIMM output drive strength for clock and data buffer ctrl. DDR4 only.
*    lrdm_db_drv_ck_bck               LRDIMM output drive strength for clock and data buffer ctrl. DDR4 only.
*                                  </PRE>
*
******************************************************************************/
typedef struct
{
    uint8_t num_ranks;
    uint8_t num_bank_groups;
    uint8_t num_rows;
    uint8_t num_columns;
    uint8_t die_count;
    uint32_t dimm_size_mb;
    bool ca_mirrored;

    // Common RDIMM / LRDIMM Values
    uint8_t out_drv_ck_y13;
    uint8_t out_drv_ck_y02;
    uint64_t rcw0f_00;

    // DDR4 RDIMM / LRDIMM Values
    uint8_t out_drv_ctl_cke;
    uint8_t out_drv_ctl_odt;
    uint8_t out_drv_ctl_ca;
    uint8_t out_drv_ctl_cs;

    // DDR4 Only LRDIMM Values
    uint8_t lrdm_db_drv_ck_bcom_bodt_bcke;
    uint8_t lrdm_db_drv_ck_bck;
}mddr_slot_cfg_t;


/*!****************************************************************************
*
* @par Description
*    This type is used to collect the translated DIMM SPD information that is
*     expected to be common accross all populated DDR channels. Each populated
*     channel will be be populated alike.
*
* @par Members
*
*                                  <PRE>
*    dev_type             Device type. DDR3 or DDR4.
*    mod_type             DIMM module type. UDIMM, RDIMM, SODIMM or SOUDIMM.
*    sdram_width          SDRAM device width indicator.
*    max_page_size_kb     Largest page size from all DIMMs
*    has_ecc              Boolean indication of if the DIMMs have ECC.
*    is_3DS               Boolean indication of if the DIMMs are 3DS stacked.
*    slots_ck_sel         Combined manual CK (CKE groups clock) selects for
*                          the slots of a channel. The slots can have
*                          different values.
*    slots_cke_sel        Combined manual CKE groups selects for the DIMM
*                          slots of a channel. The slots can have
*                          different values.
*    slot_config          Array containing the configurations that can differ
*                           between the slots of a channel.
*                                  </PRE>
*
******************************************************************************/
typedef struct
{
    uint8_t         dev_type;
    uint8_t         mod_type;
    uint8_t         sdram_width;
    uint32_t        max_page_size_kb;
    bool            has_ecc;
    bool            is_3DS;
    uint8_t         slots_ck_sel;
    uint8_t         slots_cke_sel;
    uint8_t         num_slots_populated;
    mddr_slot_cfg_t slot_config[DDR_MAX_NUM_SLOTS_PER_CHAN];
}mddr_common_cfg_t;

/*!****************************************************************************
*
* @par Description
*    This type is used to collect the timing values based on the timing
*    parameters extracted from the SPD of the populated DIMMs and platform
*    specific settings.
*
* @par Members
*
*                                  <PRE>
*    cas_lat_sup         CAS latencies supported flags.
*    taa_min_ps          Minimum CAS Latency Time in ps.
*    tck_min_ps          SDRAM Minimum Cycle Time in ps.
*    tck_max_ps          SDRAM Maximum Cycle Time in ps.
*    tck_ps              DDR Clock period in ps.
*    tcke_clk            Minimum CKE pulse width in clock ticks.
*
*    trcd_min_clk        Minimum RAS to CAS Delay Time in clock ticks.
*    trp_min_clk         Minimum Row Precharge Delay Time in clock ticks.
*    trtp_min_clk        Minimum Internal Read to Precharge Command Delay Time in clock ticks. SPD is DDR3 only derived for DDR4.
*    tras_min_clk        Minimum Active to Precharge Delay Time in clock ticks.
*    trc_min_clk         Minimum Active to Active/Refresh Delay Time in clock ticks.
*    tfaw_min_clk        Minimum Four Activate Window Time in clock ticks.
*    txs_clk             Time for commands to exit self refresh without a locked DLL.
*                         (Refresh Recovery Delay Time in clock ticks plus 10ns).
*    txp_clk             Exit power down with DLL on to any valid command.
*    txsdll_min_clk      Minimum time for commands to exit self refresh with a locked DLL.
*    tdllk_min_clk       Minimum DLL lock time.
*    twr_min_clk         Minimum Write Recovery Time in clock ticks.
*    twtr_min_clk        Minimum Internal Write to Read Command Delay Time in clock ticks.
*    mr_wr_clk           MR bounded version of twr_min_clk, Write Recovery Time.
*    tstab_clk           Stabilization time - command buffer chip.
*    tckev_min_clk       Minimum number of DDR cycles to stay low when the CK_t/CK_c are pulled low.
*
*    trrd_min_clk        Minimum Row Active to Row Active Delay Time in clock ticks.  DDR3 only.
*    trfc_min_ps         Minimum Refresh Recovery Delay Time in clock ticks.  DDR3 only.
*
*    trrdl_min_clk       Minimum Activate to Activate Delay Time, same bank group, in clock ticks.  DDR4 only.
*    trrds_min_clk       Minimum Activate to Activate Delay Time, different bank group in clock ticks.  DDR4 only.
*    tccdl_min_clk       Minimum CAS to CAS Delay Time (Long), same bank group.  DDR4 only.
*    trfc1_min_ps        Minimum Refresh Recovery Delay Time - RFC1 in clock ticks.  DDR4 only.
*    trfc2_min_ps        Minimum Refresh Recovery Delay Time - RFC2 in clock ticks.  DDR4 only.
*    trfc4_min_ps        Minimum Refresh Recovery Delay Time - RFC4 in clock period.  DDR4 only.
*    twtrs_min_clk       Delay from start of internal write trans-action to internal read command
*                         for dif-ferent bank group. DDR4 only.
*                                  </PRE>
*
******************************************************************************/
typedef struct
{
    // Common
    uint32_t cas_lat_sup;
    uint32_t taa_min_ps;
    uint32_t tck_min_ps;
    uint32_t tck_max_ps;
    uint32_t tck_ps;
    uint32_t tcke_clk;

    uint32_t trcd_min_clk;
    uint32_t trp_min_clk;
    uint32_t trtp_min_clk;
    uint32_t tras_min_clk;
    uint32_t trc_min_clk;
    uint32_t tfaw_min_clk;
    uint32_t txs_clk;
    uint32_t txp_clk;
    uint32_t txsdll_min_clk;
    uint32_t tdllk_min_clk;
    uint32_t twr_min_clk;
    uint32_t twtr_min_clk;
    uint32_t mr_wr_clk; // AKA &mc_wr in scripts
    uint32_t tstab_clk;
    uint32_t tckev_min_clk;

    // DDR3 Only
    uint32_t trrd_min_clk;
    uint32_t trfc_min_ps;

    // DDR4 Only
    uint32_t trrdl_min_clk;
    uint32_t trrds_min_clk;
    uint32_t tccdl_min_clk;
    uint32_t trfc1_min_ps;
    uint32_t trfc2_min_ps;
    uint32_t trfc4_min_ps;
    uint32_t twtrs_min_clk;
} mddr_timing_values_t;

/*!****************************************************************************
*
* @par Description
*    This type is used to collect the calculated latency values.
*
* @par Members
*
*                                  <PRE>
*    cl                  CAS latency.
*    cwl                 CAS write latency.
*    rl                  Read latency.
*    orl                 Overall read latency.
*    wl                  Write latency.
*    pl                  Parity latency.
*    tcal                CS to Command/Address Latency.
*    tdbi                DBI induced read latency. tAA_DBI in clock cycles
*                         instead of ps.
*    tpdm                RDIMM induced read latency.
*    tmod                mode register set command update delay
*    tmrd                mode register set command cycle time
*    mc_mr2_cwl          CAS Write Latency MR2 format / value.
*    mc_mr5_pl           Parity Latency MR2 format / value.
*    rcd_nladd           RDIMM/LRDIMM command buffer chip timing parameter.
*                                  </PRE>
*
******************************************************************************/
typedef struct
{
    uint32_t cl;
    uint32_t cwl;
    uint32_t rl;
    uint32_t orl;
    uint32_t wl;
    uint32_t pl;
    uint32_t tcal;
    uint32_t tdbi;
    uint32_t tpdm;
    uint8_t tmod;
    uint8_t tmrd;
    uint8_t mc_mr2_cwl;
    uint8_t mc_mr5_pl;
    uint8_t rcd_nladd;
} mddr_latencies_t;


/******************************************************************************
* Global Variables
******************************************************************************/

/*!****************************************************************************
*
* @defgroup mddr_internal_globals MDDR Internal Globals
*
* @par Description
*    Get access to the internal global variables.
*
* @{
******************************************************************************/
extern spd_dimm_config_t mddr_dimm_configs[DDR_MAX_NUM_CHANS][DDR_MAX_NUM_SLOTS_PER_CHAN];
extern mddr_common_cfg_t common_cfg;
extern mddr_latencies_t latencies;

extern uint64_t mddr_base_addrs[DDR_MAX_NUM_CHANS];
extern uint8_t hddrss[DDR_MAX_NUM_CHANS];
extern uint8_t num_chans_poped;
extern uint8_t num_dimms_per_chan_poped;
extern uint32_t mddr_valid_ranks;
extern uint32_t mddr_combined_valid_ranks;
extern uint32_t mem_freq_mhz;
extern uint32_t ddr_rate;
extern uint32_t mem_freq_ps;
extern uint32_t mddr_xo_freq_khz;
extern target_mode_e platform_mode;

/* @} */


/******************************************************************************
* Macros
******************************************************************************/



/*****************************************************************************/
#endif
