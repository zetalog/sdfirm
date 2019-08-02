/*!****************************************************************************
* @file spd.h
*
* @par Description:
*    This header provides interfacing and related functionality to make
*    use of the SPD facilities.
*
******************************************************************************/
#ifndef _MODULE_SPD_H_
#define _MODULE_SPD_H_
/******************************************************************************
* Included Headers
******************************************************************************/
/******************************************************************************
* Defined Constants
******************************************************************************/
#define SPD_PAGE_SIZE 256
#define DDR_MAX_NUM_CHANS               2
#define DDR_MAX_NUM_SLOTS_PER_CHAN      2
#define DDR_BYTE_LANES_PER_CHAN         9
#define DDR_MAX_RANKS_PER_SLOT         4
#define DDR_MAX_DRAMS_PER_RANK         18
#define DDR_MAX_RANKS_PER_CHAN         (DDR_MAX_NUM_SLOTS_PER_CHAN * DDR_MAX_RANKS_PER_SLOT)
#define DDR_MAX_NUM_DIMMS               (DDR_MAX_NUM_CHANS * DDR_MAX_NUM_SLOTS_PER_CHAN)

#define SPD_BLOCK_SIZE                          0x100
#define SPD_MAX_SERIAL_EEPROM_SIZE              (2 * SPD_BLOCK_SIZE)

// SPD Device Type Values
#define SPD_DEVICE_TYPE_DDR4                    0xC
#define SPD_DEVICE_TYPE_UNDEFINED               0xFF

// SPD Module Values Common to DDR4
#define SPD_MODULE_TYPE_RDIMM                   0x1
#define SPD_MODULE_TYPE_UDIMM                   0x2
#define SPD_MODULE_SODIMM                       0x3
#define SPD_MODULE_TYPE_DDR4_LRDIMM             0x4
#define SPD_MODULE_SOUDIMM                      0x8

// SPD device width Common to   DDR4
#define SPD_SDRAM_WIDTH_X4                      0x4
#define SPD_SDRAM_WIDTH_X8                      0x8
#define SPD_SDRAM_WIDTH_X16                     0x10
#define SPD_SDRAM_WIDTH_X32                     0x20

// SPD module width Common to   DDR4
#define SPD_MODULE_WIDTH                        64

// SPD maximum supported number of ranks Common to DDR4
#define SPD_MAX_RANKS_SUPPORTED_2DPC            2
#define SPD_MAX_RANKS_SUPPORTED                 4
#define SPD_UNSUPPORTED_RANK_VAL                3    // MC does not support 3 rank DIMMs
#define SPD_INVALID_RANK_COUNT                  0xFF // Marker value for number of ranks indicating an invalid configuration.

// SPD Row Bits range
#define SPD_MIN_ROWS                            12
#define SPD_MAX_ROWS                            18

// SPD Column Bits range
#define SPD_MIN_COLUMNS                         10
#define SPD_MAX_COLUMNS                         12

// Minimum DIMM size supported in MB
#define SPD_MIN_DIMM_SIZE                       0x400 // 1GB

// Max DIMM size supported in MB
#define SPD_MAX_DIMM_SIZE                       0x20000 // 128 GB

// Max DIMM size supported in MB
#define SPD_MAX_DIMM_SIZE_2DPC                  0x10000 // 64 GB

// Max Channel size supported in GB
#define SPD_MAX_CHANNEL_SIZE                    0x80 // 128 GB

// RDIMM Signal Strengths
#define RDIMM_LIGHT_DRIVE                       0
#define RDIMM_MODERATE_DRIVE                    1
#define RDIMM_STRONG_DRIVE                      2
#define RDIMM_INVALID_DRIVE                     3

// RDIMM marker value for combined invalid configuration words.
#define RDIMM_INVALID_CWS                       0xFFFFFFFFFFFFFFFF

// Memory attribute values used to populate the SPD type_detail bit-field.
#define MEM_ATTR_TYPE_DETAIL_RESERVED_0         (0x1 << 0)
#define MEM_ATTR_TYPE_DETAIL_OTHER              (0x1 << 1)
#define MEM_ATTR_TYPE_DETAIL_UNKNOWN            (0x1 << 2)
#define MEM_ATTR_TYPE_DETAIL_FAST_PAGED         (0x1 << 3)
#define MEM_ATTR_TYPE_DETAIL_STATIC_COLUMN      (0x1 << 4)
#define MEM_ATTR_TYPE_DETAIL_PSEUDO_STATIC      (0x1 << 5)
#define MEM_ATTR_TYPE_DETAIL_RAMBUS             (0x1 << 6)
#define MEM_ATTR_TYPE_DETAIL_SYNCHRONOUS        (0x1 << 7)
#define MEM_ATTR_TYPE_DETAIL_CMOS               (0x1 << 8)
#define MEM_ATTR_TYPE_DETAIL_EDO                (0x1 << 9)
#define MEM_ATTR_TYPE_DETAIL_WINDOW_DRAM        (0x1 << 10)
#define MEM_ATTR_TYPE_DETAIL_CACHE_DRAM         (0x1 << 11)
#define MEM_ATTR_TYPE_DETAIL_NON_VOLATILE       (0x1 << 12)
#define MEM_ATTR_TYPE_DETAIL_REGISTERED         (0x1 << 13)
#define MEM_ATTR_TYPE_DETAIL_UNBUFFERED         (0x1 << 14)
#define MEM_ATTR_TYPE_DETAIL_RESERVED_15        (0x1 << 15)


// Module Manufacturer ID - Micron
#define SPD_MODULE_MANUFACTURER_ID_MICRON       0x2C80

// Module part number length in bytes
#define SPD_MODULE_PART_NUM_LEN_DDR4            20

// Module part number length in bytes
#define SPD_MODULE_PART_NUM_LEN                 SPD_MODULE_PART_NUM_LEN_DDR4

// Module part number string length in bytes including termination character.
#define SPD_MODULE_PART_NUM_LEN_STR            (SPD_MODULE_PART_NUM_LEN + 1)

// SPD revision 1.0
#define SPD_REVISION_1_0                        0x10

// CAS Latency range bit
#define SPD_CAL_RANGE_BIT___S                  31

/******************************************************************************
* Derived data types
******************************************************************************/
/*!****************************************************************************
*
* @par Description
*    Mode of the target execution configuration. Allows runtime adaptation
*    based on the desired/actual target configuration.
*
* @par Members
*                                  <PRE>
*    SILICON     for a development platform target.
*    CHIPSIM     for a CHIPSIM environment target.
*    PALLADIUM_Z1  for a PALLADIUM_Z1 environment target.
*                                  </PRE>
******************************************************************************/
typedef enum
{
    SILICON,
    CHIPSIM,
    PALLADIUM_Z1
} target_mode_e;

/*!****************************************************************************
*
* @par Description
*    This type is used to collect the translated SPD information extracted
*    from a single DIMM.
*
* @par Members
*
*                                  <PRE>
*    revision             The revision of the SPD.
*    dev_type             Device type. only DDR4.
*    mod_type             DIMM module type. UDIMM, RDIMM, SODIMM or SOUDIMM.
*    num_ranks            Number of ranks on the the DIMM.
*    sdram_width          SDRAM device width indicator.
*    has_ecc              Boolean indication of if the DIMM has ECC.
*    dimm_module_width    Width in bits of the DIMM module, excluding ECC bits.
*    num_bank_groups      Bank group indication value. DDR4 only.
*    device_capacity      Capacity of a single device on the DIMM.
*    num_rows             Number of rows.
*    num_columns          Number of columns.
*    die_count            Number of dies on the DIMM.
*    is_3DS               Boolean indication of if the DIMM is 3DS stacked.
*    ca_mirrored          Boolean indication of if the DIMM is CA mirrored.
*    dimm_size_mb         Size of the DIMM in MB.
*    has_therm_sensor     Boolean indication of if the DIMM incorporates a thermal sensor.
*    is_rank_mix          Boolean indication of if the DIMM has mixed ranks densities.
*    is_voltage_ok        Boolean indication of if the DIMM is operable at the
*                          expected voltage.
*    ftb_fs               Fine timebase in fs, femtoseconds.
*    mtb_ps               Medium timebase in ps.
*    cas_lat_sup          CAS latencies supported flags.
*    taa_min_ps           Minimum CAS Latency Time in ps.
*    tck_min_ps           SDRAM Minimum Cycle Time in ps.
*    tck_max_ps           SDRAM Maximum Cycle Time in ps.
*    trcd_min_ps          Minimum RAS to CAS Delay Time in ps.
*    trp_min_ps           Minimum Row Precharge Delay Time in ps.
*    tras_min_ps          Minimum Active to Precharge Delay Time in ps.
*    trc_min_ps           Minimum Active to Active/Refresh Delay Time in ps.
*    tfaw_min_ps          Minimum Four Activate Window Time in ps.
*    twr_min_ps           Minimum Write Recovery Time in ps.
*    twtr_min_ps          Minimum Internal Write to Read Command Delay Time in ps.
*    out_drv_ck_y13       RDIMM / LRDIMM clock driver Y13 signal output strength.
*    out_drv_ck_y02       RDIMM / LRDIMM clock driver Y02 signal output strength.
*    trrd_min_ps          Minimum Row Active to Row Active Delay Time in ps. DDR3 only.
*    trtp_min_ps          Minimum Internal Read to Precharge Command Delay Time in ps. DDR3 only.
*    trfc_min_ps          Minimum Refresh Recovery Delay Time in ps.  DDR3 only.
*    rdm_out_drv_ctl_a    RDIMM driver output strength for control signal A. DDR3 only.
*    rdm_out_drv_ctl_b    RDIMM driver output strength for control signal B. DDR3 only.
*    rdm_out_drv_ca_a     RDIMM drive output strength for commands/addresses signal A. DDR3 only.
*    rdm_out_drv_ca_b     RDIMM drive output strength for commands/addresses signal B. DDR3 only.
*    rdm_ddr3_cw15_0      RDIMM configuration words. DDR3 only.
*    trrdl_min_ps         Minimum Activate to Activate Delay Time, same bank group, in ps. DDR4 only.
*    trrds_min_ps         Minimum Activate to Activate Delay Time, different bank group in ps. DDR4 only.
*    tccdl_min_ps         Minimum CAS to CAS Delay Time, same bank group. DDR4 only.
*    trfc1_min_ps         Minimum Refresh Recovery Delay Time - RFC1 in ps. DDR4 only.
*    trfc2_min_ps         Minimum Refresh Recovery Delay Time - RFC2 in ps. DDR4 only.
*    trfc4_min_ps         Minimum Refresh Recovery Delay Time - RFC4 in ps. DDR4 only.
*    twtrs_min_ps         Delay from start of internal write trans-action to internal read command
*                          for dif-ferent bank group. DDR4 only.
*    out_drv_ctl_cke      RDIMM / LRDIMM drive output strength for the CKE signal. DDR4 only.
*    out_drv_ctl_odt      RDIMM / LRDIMM drive output strength for the ODT signal. DDR4 only.
*    out_drv_ctl_ca       RDIMM / LRDIMM drive output strength for the commands/addresses signal. DDR4 only.
*    out_drv_ctl_cs       RDIMM / LRDIMM drive output strength for the chip select signal. DDR4 only.
*    type_detail          Bit field that contains memory attributes.
*    max_clk_speed        Maximum speed of the memory (MHz).
*    manufacturer_id      ID specifying the memory manufacturer.
*    serial_number        The serial number of the memory device.
*    part_number          Byte array specifying the part number.
*                                  </PRE>
*
******************************************************************************/
typedef struct
{
    uint8_t revision;
    uint8_t dev_type;
    uint8_t mod_type;
    uint8_t num_ranks;
    uint8_t sdram_width;
    bool has_ecc;
    uint8_t dimm_module_width;
    uint8_t num_bank_groups;
    uint16_t device_capacity;
    uint8_t num_rows;
    uint8_t num_columns;
    uint8_t die_count;
    bool is_3DS;
    bool ca_mirrored;
    uint32_t dimm_size_mb;
    bool has_therm_sensor;


    bool is_rank_mix;
    bool is_voltage_ok;
    uint32_t ftb_fs;
    uint32_t mtb_ps;

    uint32_t cas_lat_sup;
    uint32_t taa_min_ps;
    uint32_t tck_min_ps;
    uint32_t tck_max_ps;

    // Common Timing Values
    uint32_t trcd_min_ps;
    uint32_t trp_min_ps;
    uint32_t tras_min_ps;
    uint32_t trc_min_ps;
    uint32_t tfaw_min_ps;
    uint32_t twr_min_ps;
    uint32_t twtr_min_ps;
    uint8_t out_drv_ck_y13;
    uint8_t out_drv_ck_y02;

    // DDR3 Only Timing Values
    uint32_t trrd_min_ps;
    uint32_t trtp_min_ps;
    uint32_t trfc_min_ps;

    // DDR3 Only RDIMM Values
    uint8_t rdm_out_drv_ctl_a;
    uint8_t rdm_out_drv_ctl_b;
    uint8_t rdm_out_drv_ca_a;
    uint8_t rdm_out_drv_ca_b;
    uint64_t rdm_ddr3_cw15_0;

    // DDR4 Only Timing Values
    uint32_t trrdl_min_ps;
    uint32_t trrds_min_ps;
    uint32_t tccdl_min_ps;
    uint32_t trfc1_min_ps;
    uint32_t trfc2_min_ps;
    uint32_t trfc4_min_ps;
    uint32_t twtrs_min_ps;

    // DDR4 RDIMM / LRDIMM Values
    uint8_t out_drv_ctl_cke;
    uint8_t out_drv_ctl_odt;
    uint8_t out_drv_ctl_ca;
    uint8_t out_drv_ctl_cs;

    // DDR4 Only LRDIMM Values
    uint8_t lrdm_db_drv_ck_bcom_bodt_bcke;
    uint8_t lrdm_db_drv_ck_bck;

    // DIMM Identification
    uint16_t type_detail;
    uint16_t max_clk_speed;
    uint16_t manufacturer_id;
    uint32_t serial_number;
    uint8_t  part_number[SPD_MODULE_PART_NUM_LEN_STR];
}spd_dimm_config_t;

/*!****************************************************************************
*
* @par Description
*    This type provides SMBus bus information for a DIMM slot.
*
* @par Members
*
*                                  <PRE>
*    smbus_id               SMBus id of the DIMM slot
*    spd_eeprom_bus_addr    SMBus address of the DIMM slot EEPROM
*    spd_ts_bus_addr        SMBus address of the DIMM slot thermal sensor
*    spd_cmd_bus_addr       SMBus address of the DIMM slot comand region
*                                  </PRE>
*
******************************************************************************/
typedef struct
{
    uint8_t smbus_id;
    uint8_t spd_eeprom_bus_addr;
    uint8_t spd_ts_bus_addr;
    uint8_t spd_cmd_bus_addr;
    uint8_t rcd_bus_addr;
} dimm_slot_smbus_info_t;

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
extern uint8_t hddrss[DDR_MAX_NUM_CHANS];

/*!****************************************************************************
*
* @defgroup spd_clk_converters SPD Clock Converters
*
* @par Description
*    Set of macros to convert between clock rates and time.
*
* @{
******************************************************************************/
#define SPD_MHZ_TO_PS(freq_mhz)(COM_MHZ_TO_PS(freq_mhz))
#define SPD_KHZ_TO_PS(freq_khz)(COM_KHZ_TO_PS(freq_khz))
#define SPD_PS_TO_MHZ(ps)(COM_PS_TO_MHZ(ps))
#define SPD_PS_TO_KHZ(ps)(COM_PS_TO_KHZ(ps))


/******************************************************************************
* Function Prototypes
******************************************************************************/

void spd_init(target_mode_e mode, dimm_slot_smbus_info_t smbus_info[DDR_MAX_NUM_CHANS][DDR_MAX_NUM_SLOTS_PER_CHAN]);
unsigned spd_read_dimm_spd_bytes_total(uint8_t chan_num, uint8_t slot_num);
unsigned spd_read_dimm_nbytes(unsigned ddr_chan, unsigned dimm_slot, uint64_t spd_base, unsigned load_size);
unsigned spd_fetch_ddr_spd_info(unsigned ddr_chan, unsigned dimm_slot, uint64_t spd_base, unsigned load_size);
void spd_get_dimm_config(uint8_t ddr_chan, uint8_t dimm_slot, uint64_t spd_base, spd_dimm_config_t *dimm_config_ptr);
int16_t spd_get_dimm_temp(uint8_t chan_num, uint8_t slot_num);
bool spd_get_dimm_eventn_asserted(uint8_t chan_num, uint8_t slot_num);

/*****************************************************************************/
#endif
