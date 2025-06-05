#ifndef __UEFI_CPER_H_INCLUDE__
#define __UEFI_CPER_H_INCLUDE__

#include <target/generic.h>

/* Hest buffer size */
#define RAS_HEST_BUFFER_SIZE 0x1000

/* UEFI Appendix N - Common Platform Error Record (CPER) */

/* N.2.1 Record Header */
#define CPER_REVISION			0x300

/* Error Serverity */
#define CPER_ERROR_RECOVERABLE		0
#define CPER_ERROR_FATAL		1
#define CPER_ERROR_CORRECTED		2
#define CPER_ERROR_INFORMATIONAL	3

/* Validation Bits */
#define CPER_VALIDATION_PlatformID	_BV(0)
#define CPER_VALIDATION_TimeStamp	_BV(1)
#define CPER_VALIDATION_PartitionID	_BV(2)

/* Notification Type */
static const uuid_t CPER_NOTIFICATION_CMC_GUID={0x2DCE8BB1,0xBDD7,0x450e,{0xB9,0xAD,0x9C,0xF4,0xEB,0xD4,0xF8,0x90}};
static const uuid_t CPER_NOTIFICATION_CPE_GUID={0x4E292F96,0xD843,0x4a55,{0xA8,0xC2,0xD4,0x81,0xF2,0x7E,0xBE,0xEE}};
static const uuid_t CPER_NOTIFICATION_MCE_GUID={0xE8F56FFE,0x919C,0x4cc5,{0xBA,0x88,0x65,0xAB,0xE1,0x49,0x13,0xBB}};
static const uuid_t CPER_NOTIFICATION_PCIe_GUID={0xCF93C01F,0x1A16,0x4dfc,{0xB8,0xBC,0x9C,0x4D,0xAF,0x67,0xC1,0x04}};
static const uuid_t CPER_NOTIFICATION_INIT_GUID={0xCC5263E8,0x9308,0x454a,{0x89,0xD0,0x34,0x0B,0xD3,0x9B,0xC9,0x8E}};
static const uuid_t CPER_NOTIFICATION_NMI_GUID={0x5BAD89FF,0xB7E6,0x42c9,{0x81,0x4A,0xCF,0x24,0x85,0xD6,0xE9,0x8A}};
static const uuid_t CPER_NOTIFICATION_BOOT_GUID={0x3D61A466,0xAB40,0x409a,{0xA6,0x98,0xF3,0x62,0xD4,0x64,0xB3,0x8F}};
static const uuid_t CPER_NOTIFICATION_DMAr_GUID={0x667DD791,0xC6B3,0x4c27,{0x8A,0x6B,0x0F,0x8E,0x72,0x2D,0xEB,0x41}};
static const uuid_t CPER_NOTIFICATION_SEA_GUID={0x9A78788A,0xBBE8,0x11E4,{0x80,0x9E,0x67,0x61,0x1E,0x5D,0x46,0xB0}};
static const uuid_t CPER_NOTIFICATION_SEI_GUID={0x5C284C81,0xB0AE,0x4E87,{0xA3,0x22,0xB0,0x4C,0x85,0x62,0x43,0x23}};
static const uuid_t CPER_NOTIFICATION_PEI_GUID={0x09A9D5AC,0x5204,0x4214,{0x96,0xE5,0x94,0x99,0x2E,0x75,0x2B,0xCD}};
static const uuid_t CPER_NOTIFICATION_CXL_GUID={0x69293BC9,0x41DF,0x49A3{0xB4,0xBD,0x4F,0xB0,0xDB,0x30,0x41,0xF6}};

/* Error Record Header Flags */
#define CPER_HW_ERROR_FLAGS_RECOVERED	1
#define CPER_HW_ERROR_FLAGS_PREVERR	2
#define CPER_HW_ERROR_FLAGS_SIMULATED	4

/* block status value
 * Bit 0 - Uncorrectable Error Valid: If set to one, indicates that an
 * uncorrectable error condition exists.
 * Bit 1 - Correctable Error Valid: If set to one, indicates that a
 * correctable error condition exists.
 * Bit 2 - Multiple Uncorrectable Errors: If set to one, indicates that
 * more than one uncorrectable errors have been detected.
 * Bit 3 - Multiple Correctable Errors: If set to one, indicates that
 * more than one correctable errors have been detected.
 * Bit 4-13 - Error Data Entry Count: This value indicates the number
 * of Error Data Entries found in the Data section.
 * Bit 14-31 - Reserved
 */

/* CPER contains three main sections */
#define BLOCK_STATUS_HEADER_UNCORRECTABLE_ERROR_VALID		0x1
#define BLOCK_STATUS_HEADER_CORRECTABLE_ERROR_VALID		0x2
#define BLOCK_STATUS_HEADER_MULTIPLE_UNCORRECTABLE_ERROR	0x4
#define BLOCK_STATUS_HEADER_MULTIPLE_CORRECTABLE_ERROR		0x8
#define BLOCK_STATUS_HEADER_SINGLE_ERROR_DATA_ENTRY		0x10

/* record header*/
typedef struct _ras_cper_error_record_header_type {
   uint32_t block_status; /* block status value defined above*/
   uint32_t raw_data_offset;/* offset to end of all error data records*/
   uint32_t raw_data_offset_length; /* 4k-raw_data_offset*/
   uint32_t data_length; /*data length not inculding the record header*/
   uint32_t error_severity;
} ras_cper_error_record_header_type;

/* Section descriptor*/
#define SECTION_TYPE_LENGTH 16
#define FRU_ID_LENGTH 16
#define FRU_TEXT_LENGTH 20


#define   FLAGS_PRIMARY 0x1
#define   FLAGS_CONTAINMENT 0x2
#define   FLAGS_RESET 0x4
#define   FLAGS_THRESHOLD 0x8
#define   FLAGS_NOACCESS 0x10
#define   FLAGS_LATENT 0x20
#define   FLAGS_PROPAGATED 0x40
#define   FLAGS_OVERFLOW 0x80

/* N.2.2 Section Descriptor */
#define CPER_SECTION_FLAGS_PRIMARY			_BV(0)
#define CPER_SECTION_FLAGS_CONTAINMENT_WARNING		_BV(1)
#define CPER_SECTION_FLAGS_RESET			_BV(2)
#define CPER_SECTION_FLAGS_ERROR_THRES_EXCEED		_BV(3)
#define CPER_SECTION_FLAGS_RESOURCE_NOT_ACCESSIBLE	_BV(4)
#define CPER_SECTION_FLAGS_LATENT_ERROR			_BV(5)
#define CPER_SECTION_FLAGS_PROPAGETED			_BV(6)
#define CPER_SECTION_FLAGS_ZZ

typedef struct _ras_cper_section_descriptor_type
{
   uint8_t section_type[SECTION_TYPE_LENGTH]; /* unique guid for each generic error record*/
   uint32_t error_severity;
   uint16_t revision; /*0x300*/
   uint8_t validation_bits;/*fru id contains valid information or not*/
   uint8_t flags;
   uint32_t error_data_length; /*generic error data length*/
   uint8_t fru_id[FRU_ID_LENGTH];
   uint8_t fru_text[FRU_TEXT_LENGTH];
   uint64_t timestamp;
}ras_cper_section_descriptor_type;


/*Generic error data, this structure will different for each section type
  Each section type is indentified with a unique GUID*/


#define PLATFORM_MEMORY_VALIDATION_BIT_ERROR_STATUS_VALID 0x1
#define PLATFORM_MEMORY_VALIDATION_BIT_PHYSICAL_ADDRESS_VALID 0x2
#define PLATFORM_MEMORY_VALIDATION_BIT_PHYSICAL_ADDRESS_MASK_VALID 0x4
#define PLATFORM_MEMORY_VALIDATION_BIT_NODE_VALID 0x8
#define PLATFORM_MEMORY_VALIDATION_BIT_CARD_VALID 0x10
#define PLATFORM_MEMORY_VALIDATION_BIT_MODULE_VALID 0x20
#define PLATFORM_MEMORY_VALIDATION_BIT_BANK_VALID 0x40
#define PLATFORM_MEMORY_VALIDATION_BIT_DEVICE_VALID 0x80
#define PLATFORM_MEMORY_VALIDATION_BIT_ROW_VALID 0x100
#define PLATFORM_MEMORY_VALIDATION_BIT_COLUMN_VALID 0x200
#define PLATFORM_MEMORY_VALIDATION_BIT_BITPOSITION_VALID 0x400
#define PLATFORM_MEMORY_VALIDATION_BIT_PLATFORM_REQUESTER_ID_VALID 0x800
#define PLATFORM_MEMORY_VALIDATION_BIT_PLATFORM_RESPONDER_ID_VALID 0x1000
#define PLATFORM_MEMORY_VALIDATION_BIT_MEMORY_PLATFORM_TARGET_VALID 0x2000
#define PLATFORM_MEMORY_VALIDATION_BIT_MEMORY_ERROR_TYPE_VALID 0x4000
#define PLATFORM_MEMORY_VALIDATION_BIT_RANK_NUMBER_VALID 0x8000
#define PLATFORM_MEMORY_VALIDATION_BIT_CARD_HANDLE_VALID 0x10000
#define PLATFORM_MEMORY_VALIDATION_BIT_MODULE_HANDLE_VALID 0x20000
#define PLATFORM_MEMORY_VALIDATION_BIT_EXTENDED_ROW_BIT_VALID 0x40000
#define PLATFORM_MEMORY_VALIDATION_BIT_BANK_GROUP_VALID 0x80000
#define PLATFORM_MEMORY_VALIDATION_BIT_BANK_ADDRESS_VALID 0x100000
#define PLATFORM_MEMORY_VALIDATION_BIT_CHIP_INDENTIFICATION_VALID 0x200000

#define PLATFORM_MEMORY_ERROR_STATUS_ERR_INTERNAL 0x100
#define PLATFORM_MEMORY_ERROR_STATUS_ERR_INTERNAL_MEM 0x400
#define PLATFORM_MEMORY_ERROR_STATUS_ERR_INTERNAL_TLB 0x500
#define PLATFORM_MEMORY_ERROR_STATUS_ERR_INTERNAL_CACHE 0x600
#define PLATFORM_MEMORY_ERROR_STATUS_ERR_INTERNAL_FUNCTION 0x700
#define PLATFORM_MEMORY_ERROR_STATUS_ERR_INTERNAL_SELFTEST 0x800
#define PLATFORM_MEMORY_ERROR_STATUS_ERR_INTERNAL_FLOW 0x900

#define PLATFORM_MEMORY_ERROR_STATUS_ERR_BUS 0x1000
#define PLATFORM_MEMORY_ERROR_STATUS_ERR_BUS_MAP 0x1100
#define PLATFORM_MEMORY_ERROR_STATUS_ERR_BUS_IMPROPER 0x1200
#define PLATFORM_MEMORY_ERROR_STATUS_ERR_BUS_UNIMPL 0x1300
#define PLATFORM_MEMORY_ERROR_STATUS_ERR_BUS_LOL 0x1400
#define PLATFORM_MEMORY_ERROR_STATUS_ERR_BUS_RESPONSE 0x1500
#define PLATFORM_MEMORY_ERROR_STATUS_ERR_BUS_PARITY 0x1600
#define PLATFORM_MEMORY_ERROR_STATUS_ERR_BUS_PROTOCOL 0x1700
#define PLATFORM_MEMORY_ERROR_STATUS_ERR_BUS_PATH_ERROR 0x1800
#define PLATFORM_MEMORY_ERROR_STATUS_ERR_BUS_TIMEOUT 0x1900
#define PLATFORM_MEMORY_ERROR_STATUS_ERR_BUS_POISIONED 0x1a00

#define PLATFORM_MEMORY_ERROR_STATUS_ADDRESS 0x10000
#define PLATFORM_MEMORY_ERROR_STATUS_CONTROL 0x20000
#define PLATFORM_MEMORY_ERROR_STATUS_DATA 0x40000
#define PLATFORM_MEMORY_ERROR_STATUS_RESPONDER 0x80000
#define PLATFORM_MEMORY_ERROR_STATUS_REQUESTER 0x100000
#define PLATFORM_MEMORY_ERROR_STATUS_FIRST_ERROR 0x200000
#define PLATFORM_MEMORY_ERROR_STATUS_OVERFLOW 0x400000

#define PLATFORM_MEMORY_MEMORY_ERROR_TYPE_UNKNOWN 0x0
#define PLATFORM_MEMORY_MEMORY_ERROR_TYPE_NOERROR 0x1
#define PLATFORM_MEMORY_MEMORY_ERROR_TYPE_SINGLEBIT_ECC 0x2
#define PLATFORM_MEMORY_MEMORY_ERROR_TYPE_MULTIPLEBIT_ECC 0x3
#define PLATFORM_MEMORY_MEMORY_ERROR_TYPE_SINGLESYMBOL_CHIPKILL_ECC 0x4
#define PLATFORM_MEMORY_MEMORY_ERROR_TYPE_MULTIPLESYMBOL_CHIPKILL_ECC 0x5
#define PLATFORM_MEMORY_MEMORY_ERROR_TYPE_MASTER_ABORT 0x6
#define PLATFORM_MEMORY_MEMORY_ERROR_TYPE_TARGET_ABORT 0x7
#define PLATFORM_MEMORY_MEMORY_ERROR_TYPE_PARITY_ERROR 0x8
#define PLATFORM_MEMORY_MEMORY_ERROR_TYPE_WATCHDOG_TIMEOUT 0x9
#define PLATFORM_MEMORY_MEMORY_ERROR_TYPE_INVALID_ADDRESS 0xa
#define PLATFORM_MEMORY_MEMORY_ERROR_TYPE_MIRROR_BROKEN 0xb
#define PLATFORM_MEMORY_MEMORY_ERROR_TYPE_MEMORY_SPARING 0xc
#define PLATFORM_MEMORY_MEMORY_ERROR_TYPE_SCRUB_CORRECTED_ERROR 0xd
#define PLATFORM_MEMORY_MEMORY_ERROR_TYPE_SCURB_UNCORRECTED_ERROR 0xe
#define PLATFORM_MEMORY_MEMORY_ERROR_TYPE_PHYSICAL_MEMORY_MAP_OUT_EVENT 0xf



typedef PACKED struct PACKED_POST
{
   uint64_t validation_bits;/*Indicates which fields in the memory error record are valid.
                              Bit 0 – Error Status Valid
                              Bit 1 – Physical Address Valid
                              Bit 2 – Physical Address Mask Valid
                              Bit 3 – Node Valid
                              Bit 4 – Card Valid
                              Bit 5 – Module Valid
                              Bit 6 – Bank Valid (When Bank is addressed via group/address,
                              refer to Bit 19 and 20)
                              Bit 7 – Device Valid
                              Bit 8 – Row Valid
                              1 - the Row field at Offset 42 contains row number (15:0)
                              and row number (17:16) are 00b
                              0 - the Row field at Offset 42 is not used, or is defined by
                              Bit 18 (Extended Row Bit 16 and 17
                              Valid).
                              Bit 9 – Column Valid
                              Bit 10 – Bit Position Valid
                              Bit 11 – Platform Requestor Id Valid
                              Bit 12 – Platform Responder Id Valid
                              Bit 13 – Memory Platform Target Valid
                              Bit 14 – Memory Error Type Valid
                              Bit 15 - Rank Number Valid
                              Bit 16 - Card Handle Valid
                              Bit 17 - Module Handle Valid
                              Bit 18 - Extended Row Bit 16 and 17 Valid (refer to Byte Offset 42
                              and 73 below)
                              1 - the Row field at Offset 42 contains row number (15:0)
                              and the Extended field at Offset 73 contains row number
                              (17:16)
                              0 - the Extended field at Offset 73 and the Row field at
                              Offset 42 are not used, or the Rowfield at Offset 42 is
                              defined by Bit 8 (Row Valid).
                              When this bit is set to 1, Bit 8 (Row Valid) must be set
                              to 0.
                              Bit 19 - Bank Group Valid
                              Bit 20 - Bank Address Valid
                              Bit 21 - Chip Identification Valid
                              Bit 22-63 Reserved*/
   uint64_t error_status;
   uint64_t physical_address;
   uint64_t physical_address_mask;
   uint16_t node;
   uint16_t card;
   uint16_t module;
   uint16_t bank;
   uint16_t device;
   uint16_t row;
   uint16_t column;
   uint16_t bit_position;
   uint64_t requester_id;
   uint64_t responder_id;
   uint64_t target_id;
   uint8_t  memory_error_type;/*Identifies the type of error that occurred:
                                 0 – Unknown
                                 1 – No error
                                 2 – Single-bit ECC
                                 3 – Multi-bit ECC
                                 4 – Single-symbol ChipKill ECC
                                 5 – Multi-symbol ChipKill ECC
                                 6 – Master abort
                                 7 – Target abort
                                 8 – Parity Error
                                 9 – Watchdog timeout
                                 10 – Invalid address
                                 11 – Mirror Broken
                                 12 – Memory Sparing
                                 13 - Scrub corrected error
                                 14 - Scrub uncorrected error
                                 15 - Physical Memory Map-out event
                                 All other values reserved.*/

   uint8_t extended;
   uint16_t rank_number;
   uint16_t card_handle;
   uint16_t module_handle;
}ras_generic_error_platform_memory;

typedef struct _ras_generic_error_processor
{
   uint64_t validation_bits;/*Indicates which fields in the memory error record are valid.
                              Bit 0 – Processor Type Valid
                              Bit 1 – Processor ISA Valid
                              Bit 2 – Processor Error Type Valid
                              Bit 3 – Operation Valid
                              Bit 4 – Flags Valid
                              Bit 5 – Level Valid
                              Bit 6 – CPU Version Valid
                              Bit 7 – CPU Brand Info Valid
                              Bit 8 – CPU Id Valid
                              Bit 9 – Target Address Valid
                              Bit 10 – Requester Identifier Valid
                              Bit 11 – Responder Identifier Valid
                              Bit 12 – Instruction IP Valid
                              All other bits are reserved and must be zero.*/
   uint8_t  processor_type;  /*0:1A32/x64 1:IA64 */
   uint8_t  processor_isa;  /*0:1A32 1:I64 2:xA64 */
   uint8_t  processor_error_type; /*0x00: Unknown
                                 0x01: Cache Error
                                 0x02: TLB Error
                                 0x04: Bus Error
                                 0x08: Micro-Architectural Error*/
   uint8_t operation;    /*0: Unknown or generic
                        1: Data Read
                        2: Data Write
                        3: Instruction Execution
                        All other values reserved.*/
   uint8_t flags; /*Bit 0: Restartable – If 1, program execution can be restarted
                        reliably after the error.
                        Bit 1: Precise IP – If 1, the instruction IP captured is directly
                        associated with the error.
                        Bit 2: Overflow – If 1, a machine check overflow occurred (a
                        second error occurred while the results of a previous error were
                        still in the error reporting resources).
                        Bit 3: Corrected – If 1, the error was corrected by hardware and/
                        or firmware.*/

   uint8_t level; /*Level of the structure where the error occurred, with 0 being the
                 lowest level of cache.*/

   uint16_t res;
   uint64_t cpu_version;
   uint8_t cpu_brand[128];
   uint64_t processor_id;
   uint64_t target_address;
   uint64_t requestor_identifier;
   uint64_t responder_identifier;
   uint64_t instruction_ip;
}ras_generic_error_processor;

#define MIDR_HAWKER 0x510F2811
#define MIDR_AW 0x510F8000

#define ARMV8_PROCESSOR_VALIDATION_BIT_MPIDR_VALID 0x1
#define ARMV8_PROCESSOR_VALIDATION_BIT_ERROR_AFFINITY_LEVEL_VALID 0x2
#define ARMV8_PROCESSOR_VALIDATION_BIT_ERROR_RUNNING_STATE_VALID 0x4
#define ARMV8_PROCESSOR_VALIDATION_BIT_VENDOR_SPECIFIC_INFO_VALID 0x8

#define RAS_AFFINITY_LEVEL_CPU 0
#define RAS_AFFINITY_LEVEL_CLUSTER 1
#define RAS_AFFINITY_LEVEL_OUTSIDE_CLUSTER 2

/* ARM Processor record consist of three different structures
   ras_armv8_error_processor,
   ras_armv8_error_processor_information,
   either of ras_armv8_error_processor_context_64 or ras_armv8_error_processor_context_32*/
typedef PACKED struct PACKED_POST
{
   uint32_t validation_bits;/*Indicates which fields in the memory error record are valid.
                              Bit 0 – MPIDR valid
                              Bit 1 – Error Affinity level Valid
                              Bit 2 – Vendor specific info Valid
                              All other bits are reserved and must be zero.*/

   uint16_t err_info_num;  /* Number of processor error information structs, value ranges from 1-255*/
   uint16_t context_info_num;  /* Number of context information structs  value ranges from  0-65536*/
   uint32_t section_length; /* Length of entire section*/
   uint8_t error_affinity_level; /* For errors that can be attributed to a specific affinity level, this field defines the affinity
                                 level at which the error was produced, detected, and/or consumed. This is a value between 0 and 3.  All other values (4-255) are reserved

                               */
   uint8_t reserved[3];/* <Must be zero's*/
   uint64_t mpidr; /*This field is valid for “attributable errors” that can be attributed to a specific CPU, cache, or cluster.  This is the processor’s unique ID in the system*/
   uint64_t midr; /* This field provides identification information of the chip, including an implementer code for the device and a device ID number*/
   uint32_t running_state;/* bit 0 set indicates the processor is running - if bit is set than psci_state must be 0 */
   uint32_t psci_state; /* if processor is not running, indicates PSCI state*/
 }ras_armv8_error_processor;

#define ARMV8_PROCESSOR_INFO_VALIDATION_BIT_MULTIPLE_ERROR_VALID 0x1
#define ARMV8_PROCESSOR_INFO_VALIDATION_BIT_FLAGS_VALID 0x2
#define ARMV8_PROCESSOR_INFO_VALIDATION_BIT_ERROR_INFORMATION_VALID 0x4
#define ARMV8_PROCESSOR_INFO_VALIDATION_BIT_VIRTUAL_FAULT_ADDRESS_VALID 0x8
#define ARMV8_PROCESSOR_INFO_VALIDATION_BIT_PHYSICAL_FAULT_ADDRESS_VALID 0x10


#define ARMV8_PROCESSOR_INFO_PROCESSOR_ERROR_TYPE_CACHE 0x0
#define ARMV8_PROCESSOR_INFO_PROCESSOR_ERROR_TYPE_TLB 0x1
#define ARMV8_PROCESSOR_INFO_PROCESSOR_ERROR_TYPE_BUS 0x2
#define ARMV8_PROCESSOR_INFO_PROCESSOR_ERROR_TYPE_MICRO_ARCH 0x3

#define ARMV8_PROCESSOR_INFO_MULTIPLE_ERROR_SINGLE 0x0
#define ARMV8_PROCESSOR_INFO_MULTIPLE_ERROR_MULTIPLE 0x1

#define ARMV8_PROCESSOR_INFO_FLAGS_FIRST_ERROR_CAP 0x1
#define ARMV8_PROCESSOR_INFO_FLAGS_LAST_ERROR_CAP 0x2
#define ARMV8_PROCESSOR_INFO_FLAGS_PROPAGATED 0x4
#define ARMV8_PROCESSOR_INFO_FLAGS_OVERFLOW 0x8

typedef PACKED struct PACKED_POST
{
   uint8_t version;
   uint8_t length;
   uint16_t validation_bit; /*
                        Bit 0 – Multiple Error Valid
                        Bit 1 – Flags Valid
                        Bit 2 – Error Information Valid
                        Bit 3 – Virtual Fault Address
                        Bit 4 – Physical Fault Address
                        All other bits are reserved and must be zero.
                        */

   uint8_t  processor_error_type; /*0x00: Unknown
                                 0x01: Cache Error
                                 0x02: TLB Error
                                 0x04: Bus Error
                                 0x08: Micro-Architectural Error*/

   uint16_t multiple_error; /*
                           This field indicates whether multiple errors have occurred.
                           In the case of multiple error with a valid count, this field will specify the error count.
                           The value of this field is defined as follows:
                           0: Single Error
                           1: Multiple Errors
                           2-65535: Error Count (if known)

                           */
   uint8_t flags;
                /*
                  This field indicates flags that describe the error attributes.
                  The value of this field is defined as follows:
                  Bit 0 – First error captured
                  Bit 1 – Last error captured
                  Bit 2 – Propagated
                  Bit 3 - Overflow
                  All other bits are reserved and must be zero

                  Note: Overflow bit indicates that firmware/hardware error buffers had experience an overflow, and
                  it is possible that some error information has been lost.

               */
   uint64_t error_information; /* described below , contains one of following
                                 ras_armv8_error_processor_cache_information
                                 ras_armv8_error_processor_tlb_information
                                 ras_armv8_error_processor_bus_information*/
   uint64_t virtual_fault_address;
   uint64_t physical_fault_address;

}ras_armv8_error_processor_information;

#define ARMV8_PROCESSOR_CACHE_INFO_VALIDATION_BIT_TRANSACTION_TYPE_VALID 0x1
#define ARMV8_PROCESSOR_CACHE_INFO_VALIDATION_BIT_OPERATION_VALID 0x2
#define ARMV8_PROCESSOR_CACHE_INFO_VALIDATION_BIT_LEVEL_VALID 0x4
#define ARMV8_PROCESSOR_CACHE_INFO_VALIDATION_BIT_PROCESSOR_CONTEXT_CORRUPT_VALID 0x8
#define ARMV8_PROCESSOR_CACHE_INFO_VALIDATION_BIT_CORRECTED_VALID 0x10
#define ARMV8_PROCESSOR_CACHE_INFO_VALIDATION_BIT_PRECISE_PC_VALID 0x20
#define ARMV8_PROCESSOR_CACHE_INFO_VALIDATION_BIT_RESTARTABLE_PC_VALID 0x40

#define ARMV8_PROCESSOR_CACHE_INFO_TRANSACTION_TYPE_INSTRUCTION 0x0
#define ARMV8_PROCESSOR_CACHE_INFO_TRANSACTION_TYPE_DATA 0x1
#define ARMV8_PROCESSOR_CACHE_INFO_TRANSACTION_TYPE_GENERIC 0x2

#define ARMV8_PROCESSOR_CACHE_INFO_OPERATION_GENERIC_ERROR 0x0
#define ARMV8_PROCESSOR_CACHE_INFO_OPERATION_GENERIC_READ 0x1
#define ARMV8_PROCESSOR_CACHE_INFO_OPERATION_GENERIC_WRITE 0x2
#define ARMV8_PROCESSOR_CACHE_INFO_OPERATION_DATA_READ 0x3
#define ARMV8_PROCESSOR_CACHE_INFO_OPERATION_DATA_WRITE 0x4
#define ARMV8_PROCESSOR_CACHE_INFO_OPERATION_INSTRUCTION_FETCH 0x5
#define ARMV8_PROCESSOR_CACHE_INFO_OPERATION_PREFETCH 0x6
#define ARMV8_PROCESSOR_CACHE_INFO_OPERATION_EVICTION 0x7
#define ARMV8_PROCESSOR_CACHE_INFO_OPERATION_SNOOPING 0x8
#define ARMV8_PROCESSOR_CACHE_INFO_OPERATION_SNOOPED 0x9
#define ARMV8_PROCESSOR_CACHE_INFO_OPERATION_MANAGEMENT 0xa

typedef PACKED struct PACKED_POST
{
   uint16_t validation_bit;
                        /*
                        Indicates which fields in the Cache Check structure are valid:
                        Bit 0 – Transaction Type Valid
                        Bit 1 – Operation Valid
                        Bit 2 – Level Valid
                        Bit 3 – Processor Context Corrupt Valid
                        Bit 4 – Corrected Valid
                        Bit 5 – Precise PC Valid
                        Bit 6 – Restartable PC Valid
                        All other bits are reserved and must be zero.

                        */

   uint8_t transaction_type:2;
                     /*
                       Type of cache error:
                        0 – Instruction
                        1 – Data Access
                        2 – Generic
                        All other values are reserved
                     */
   uint8_t operation:4;
                   /*
                     Type of cache operation that caused the error:
                        0 – generic error (type of error cannot be determined)
                        1 – generic read (type of instruction or data request cannot be determined)
                        2 – generic write (type of instruction or data request cannot be determined)
                        3 – data read
                        4 – data write
                        5 – instruction fetch
                        6 – prefetch
                        7 – eviction
                        8 – snooping
                        9- snooped
                        10 –management
                        All other values are reserved

                   */

      uint8_t level:3; /* Cache level*/
      uint8_t  processor_context_corrupted:1; /* 0=not corrupted*/
      uint8_t corrected:1; /* 1 corrected 0 not corrected*/
      uint8_t precise_pc:1; /*This field indicates that the program counter that is directly associated with the error*/
      uint8_t restartable_pc:1; /*This field indicates that program execution can be restarted reliably at the PC associated with the error */
      uint64_t  reserved:35; /* Must be zeros*/

}ras_armv8_error_processor_cache_information;

#define ARMV8_PROCESSOR_TLB_INFO_VALIDATION_BIT_TRANSACTION_TYPE_VALID 0x1
#define ARMV8_PROCESSOR_TLB_INFO_VALIDATION_BIT_OPERATION_VALID 0x2
#define ARMV8_PROCESSOR_TLB_INFO_VALIDATION_BIT_LEVEL_VALID 0x4
#define ARMV8_PROCESSOR_TLB_INFO_VALIDATION_BIT_PROCESSOR_CONTEXT_CORRUPT_VALID 0x8
#define ARMV8_PROCESSOR_TLB_INFO_VALIDATION_BIT_CORRECTED_VALID 0x10
#define ARMV8_PROCESSOR_TLB_INFO_VALIDATION_BIT_PRECISE_PC_VALID 0x20
#define ARMV8_PROCESSOR_TLB_INFO_VALIDATION_BIT_RESTARTABLE_PC_VALID 0x40

#define ARMV8_PROCESSOR_TLB_INFO_TRANSACTION_TYPE_INSTRUCTION 0x0
#define ARMV8_PROCESSOR_TLB_INFO_TRANSACTION_TYPE_DATA 0x1
#define ARMV8_PROCESSOR_TLB_INFO_TRANSACTION_TYPE_GENERIC 0x2

#define ARMV8_PROCESSOR_TLB_INFO_OPERATION_GENERIC_ERROR 0x0
#define ARMV8_PROCESSOR_TLB_INFO_OPERATION_GENERIC_READ 0x1
#define ARMV8_PROCESSOR_TLB_INFO_OPERATION_GENERIC_WRITE 0x2
#define ARMV8_PROCESSOR_TLB_INFO_OPERATION_DATA_READ 0x3
#define ARMV8_PROCESSOR_TLB_INFO_OPERATION_DATA_WRITE 0x4
#define ARMV8_PROCESSOR_TLB_INFO_OPERATION_INSTRUCTION_FETCH 0x5
#define ARMV8_PROCESSOR_TLB_INFO_OPERATION_PREFETCH 0x6
#define ARMV8_PROCESSOR_TLB_INFO_OPERATION_LOCAL_MANAGEMENT 0x7
#define ARMV8_PROCESSOR_TLB_INFO_OPERATION_EXTERNAL_MANAGEMENT 0x8

typedef PACKED struct PACKED_POST
{
   uint16_t validation_bit;
                        /*
                        Indicates which fields in the TLB error structure are valid:
                        Bit 0 – Transaction Type Valid
                        Bit 1 – Operation Valid
                        Bit 2 – Level Valid
                        Bit 3 – Processor Context Corrupt Valid
                        Bit 4 – Corrected Valid
                        Bit 5 – Precise PC Valid
                        Bit 6 – Restartable PC Valid
                        All other bits are reserved and must be zero.

                        */

   uint8_t transaction_type:2;
                     /*
                       Type of TLB error:
                        0 – Instruction
                        1 – Data Access
                        2 – Generic
                        All other values are reserved
                     */
   uint8_t operation:4;
                   /*
                     Type of TLB operation that caused the error:
                        0 – generic error (type of error cannot be determined)
                        1 – generic read (type of instruction or data request cannot be determined)
                        2 – generic write (type of instruction or data request cannot be determined)
                        3 – data read
                        4 – data write
                        5 – instruction fetch
                        6 – prefetch
                        7 –local management
                        8- external management
                        All other values are reserved

                   */

      uint8_t level:3; /* tlb level*/
      uint8_t  processor_context_corrupted:1; /* 0=not corrupted*/
      uint8_t corrected:1; /* 1 corrected 0 not corrected*/
      uint8_t precise_pc:1; /*This field indicates that the program counter that is directly associated with the error*/
      uint8_t restartable_pc:1; /*This field indicates that program execution can be restarted reliably at the PC associated with the error */
      uint64_t  reserved:35; /* Must be zeros*/

}ras_armv8_error_processor_tlb_information;

#define ARMV8_PROCESSOR_BUS_INFO_VALIDATION_BIT_TRANSACTION_TYPE_VALID 0x1
#define ARMV8_PROCESSOR_BUS_INFO_VALIDATION_BIT_OPERATION_VALID 0x2
#define ARMV8_PROCESSOR_BUS_INFO_VALIDATION_BIT_LEVEL_VALID 0x4
#define ARMV8_PROCESSOR_BUS_INFO_VALIDATION_BIT_PROCESSOR_CONTEXT_CORRUPT_VALID 0x8
#define ARMV8_PROCESSOR_BUS_INFO_VALIDATION_BIT_CORRECTED_VALID 0x10
#define ARMV8_PROCESSOR_BUS_INFO_VALIDATION_BIT_PRECISE_PC_VALID 0x20
#define ARMV8_PROCESSOR_BUS_INFO_VALIDATION_BIT_RESTARTABLE_PC_VALID 0x40
#define ARMV8_PROCESSOR_BUS_INFO_VALIDATION_BIT_PARTICIPATION_TYPE_VALID 0x80
#define ARMV8_PROCESSOR_BUS_INFO_VALIDATION_BIT_TIME_OUT_VALID 0x100
#define ARMV8_PROCESSOR_BUS_INFO_VALIDATION_BIT_ADDRESS_SPACE_VALID 0x200
#define ARMV8_PROCESSOR_BUS_INFO_VALIDATION_BIT_MEMORY_ATTRIBUTES_VALID 0x400
#define ARMV8_PROCESSOR_BUS_INFO_VALIDATION_BIT_ACCESS_MODE_VALID 0x800

#define ARMV8_PROCESSOR_BUS_INFO_TRANSACTION_TYPE_INSTRUCTION 0x0
#define ARMV8_PROCESSOR_BUS_INFO_TRANSACTION_TYPE_DATA 0x1
#define ARMV8_PROCESSOR_BUS_INFO_TRANSACTION_TYPE_GENERIC 0x2

#define ARMV8_PROCESSOR_BUS_INFO_OPERATION_GENERIC_ERROR 0x0
#define ARMV8_PROCESSOR_BUS_INFO_OPERATION_GENERIC_READ 0x1
#define ARMV8_PROCESSOR_BUS_INFO_OPERATION_GENERIC_WRITE 0x2
#define ARMV8_PROCESSOR_BUS_INFO_OPERATION_DATA_READ 0x3
#define ARMV8_PROCESSOR_BUS_INFO_OPERATION_DATA_WRITE 0x4
#define ARMV8_PROCESSOR_BUS_INFO_OPERATION_INSTRUCTION_FETCH 0x5
#define ARMV8_PROCESSOR_BUS_INFO_OPERATION_PREFETCH 0x6

#define ARMV8_PROCESSOR_BUS_INFO_PARTICIPATION_TYPE_LOCAL_PROC_ORIG_REQUEST 0x0
#define ARMV8_PROCESSOR_BUS_INFO_PARTICIPATION_TYPE_LOCAL_PROC_RESP_REQUEST 0x1
#define ARMV8_PROCESSOR_BUS_INFO_PARTICIPATION_TYPE_LOCAL_PROC_OBS_REQUEST 0x2
#define ARMV8_PROCESSOR_BUS_INFO_PARTICIPATION_TYPE_LOCAL_PROC_GENERIC_REQUEST 0x3

#define ARMV8_PROCESSOR_BUS_INFO_ADDRESS_SPACE_EXTERNAL_MEMORY_ACCESS 0x0
#define ARMV8_PROCESSOR_BUS_INFO_ADDRESS_SPACE_INTERNAL_MEMORY_ACCESS 0x1
#define ARMV8_PROCESSOR_BUS_INFO_ADDRESS_SPACE_DEVICE_MEMORY_ACCESS 0x2

#define ARMV8_PROCESSOR_BUS_INFO_ACCESS_MODE_SECURE 0x0
#define ARMV8_PROCESSOR_BUS_INFO_ACCESS_MODE_NORMAL 0x0

typedef PACKED struct PACKED_POST
{
   uint16_t validation_bit;
                        /*
                        Indicates which fields in the BUS error structure are valid:
                        Bit 0 – Transaction Type Valid
                        Bit 1 – Operation Valid
                        Bit 2 – Level Valid
                        Bit 3 – Processor Context Corrupt Valid
                        Bit 4 – Corrected Valid
                        Bit 5 – Precise PC Valid
                        Bit 6 – Restartable PC Valid
                        Bit 7 – Participation Type Valid
                        Bit 8 – Time Out Valid
                        Bit 9 – Address Space Valid
                        Bit 10 – Memory Attributes Valid
                        Bit 11 – Access Mode valid
                        All other bits are reserved and must be zero.

                        */

   uint8_t transaction_type:2;
                     /*
                       Type of BUS error:
                        0 – Instruction
                        1 – Data Access
                        2 – Generic
                        All other values are reserved
                     */
   uint8_t operation:4;
                   /*
                     Type of BUS operation that caused the error:
                        0 – generic error (type of error cannot be determined)
                        1 – generic read (type of instruction or data request cannot be determined)
                        2 – generic write (type of instruction or data request cannot be determined)
                        3 – data read
                        4 – data write
                        5 – instruction fetch
                        6 – prefetch
                        All other values are reserved

                   */

      uint8_t level:3; /* affinity level*/
      uint8_t  processor_context_corrupted:1; /* 0=not corrupted*/
      uint8_t corrected:1; /* 1 corrected 0 not corrected*/
      uint8_t precise_pc:1; /*This field indicates that the program counter that is directly associated with the error*/
      uint8_t restartable_pc:1; /*This field indicates that program execution can be restarted reliably at the PC associated with the error */
      uint8_t participation_type:2;
                             /*
                              Type of Participation
                              0 – Local Processor originated request
                              1 – Local processor Responded to request
                              2 – Local processor Observed
                              3 – Generic

                              The usage of this field depends on the vendor, but the examples below provide some guidance on how this field is to be used:
                              If bus error occurs on an LDR instruction, the local processor originated the request.
                              If the bus error occurs due to a snoop operation, local processor responded to the request
                              If a bus error occurs due to cache prefetching and an SEI was sent to a particular CPU to notify this bus error has occurred, then the local processor only observed the error.

                             */
      uint8_t time_out:1;/*Indicates the request time out*/
      uint8_t address_space:2; /*
                              0 – External Memory Access (e.g. DDR)
                              1 – Internal Memory Access (e.g. internal chip ROM)
                              3 – Device Memory Access
                              */

      uint16_t memory_access_attributes:9; /* Memroy access attributes as defined in armv8 specification*/
      uint8_t  access_mode:1; /* 0 secure 1 normal*/
      uint32_t  reserved:20; /* Must be zeros*/

}ras_armv8_error_processor_bus_information;

#define ARMV8_PROCESSOR_ERROR_CONTEXT_EXCEPTION_LEVEL_0 0x0
#define ARMV8_PROCESSOR_ERROR_CONTEXT_EXCEPTION_LEVEL_1 0x1
#define ARMV8_PROCESSOR_ERROR_CONTEXT_EXCEPTION_LEVEL_2 0x2
#define ARMV8_PROCESSOR_ERROR_CONTEXT_EXCEPTION_LEVEL_3 0x3

typedef PACKED struct PACKED_POST
{
   uint8_t type:4; /* value is 1 for AArch64*/
   uint8_t exception_level:3;
                 /*
                 0: EL0
                  1: EL1
                  2: EL2
                  3: EL3
                 */
   uint8_t ns:1; /* 0=secure 1=normal*/
   uint8_t reserved[7]; /* Must be zeros*/
   uint8_t general_purpose[288]; /* x0-x31,sp,elr,cpsr,spsr*/
   uint8_t spr_registers[68];/*
                              Contains the following system register state captured
                              during the exception for the particular execution state:
                              SCTLR, AFSR0, AFSR1, ESR, FAR, ISR, CONTEXTIDR, TPIDR, TCR, HCR, VTCR, TTBR0, TTBR1
                              NOTE: In the case of hardware exceptions occurring in EL0, this will dump EL1 system registers except for TPIDR_EL0
                           */
   uint32_t reserved_1; /* Must be zeros*/

}ras_armv8_error_processor_context_64;

typedef PACKED struct PACKED_POST
{
   uint8_t type:4; /* value is 2 for AArch32*/
   uint8_t exception_level:3;
                 /*
                  0: EL0
                  1: EL1
                  2: EL2
                  3: EL3
                 */
   uint8_t ns:1; /* 0=secure 1=normal*/
   uint8_t reserved[7]; /* Must be zeros*/
   uint8_t general_purpose[168]; /* R0_usr-R12_usr, SP_usr, LR_usr, PC, CPSR, SP_hyp, SPSR_hyp, ELR_hyp, SP_svc, LR_svc, SPSR_svc,
                                 SP_abt, LR_abt, SPSR_abt, SP_und, LR_und, SPSR_und, SP_mon, LR_mon,
                                 SPSR_mon, SP_irq, LR_irq, SPSR_irq, R8_fiq-R12_fiq, SP_fiq, LR_fiq, SPSR_fiq*/
   uint8_t spr_registers[68];/*
                              Contains the following system register state captured
                              during the exception for the particular execution state:
                              SCTLR, AFSR0, AFSR1, ESR, FAR, ISR, CONTEXTIDR, TPIDR, TCR, HCR, VTCR, TTBR0, TTBR1
                              NOTE: In the case of hardware exceptions occurring in EL0, this will dump EL1 system registers except for TPIDR_EL0
                           */
   uint32_t reserved_1[3]; /* Must be zeros*/

}ras_armv8_error_processor_context_32;



/* PCIE Cper error block*/
#define PCIE_EXPRESS_ERROR_VALIDATION_BIT_PORT_TYPE_VALID 1<<0
#define PCIE_EXPRESS_ERROR_VALIDATION_BIT_VERSION_VALID 1<<1
#define PCIE_EXPRESS_ERROR_VALIDATION_BIT_COMMAND_STATUS_VALID 1<<2
#define PCIE_EXPRESS_ERROR_VALIDATION_BIT_DEVICE_ID_VALID 1<<3
#define PCIE_EXPRESS_ERROR_VALIDATION_BIT_DEVICE_SERIAL_NUMBER_VALID 1<<4
#define PCIE_EXPRESS_ERROR_VALIDATION_BIT_BRIDGE_CONTROL_STATUS_VALID 1<<5
#define PCIE_EXPRESS_ERROR_VALIDATION_BIT_CAPABILITY_STRUCTURE_STATUS_VALID 1<<6
#define PCIE_EXPRESS_ERROR_VALIDATION_BIT_AER_INFO_VALID 1<<7


#define PCIE_EXPRESS_ERROR_PORT_TYPE_PCIE_EXPRESS_END_POINT 0
#define PCIE_EXPRESS_ERROR_PORT_TYPE_LEGACY_PCIE_END_POINT_DEVICE 1
#define PCIE_EXPRESS_ERROR_PORT_TYPE_ROOT_PORT 4
#define PCIE_EXPRESS_ERROR_PORT_TYPE_UPSTREAM_SWITCH_PORT 5
#define PCIE_EXPRESS_ERROR_PORT_TYPE_DOWNSTREAM_SWITCH_PORT 6
#define PCIE_EXPRESS_ERROR_PORT_TYPE_PCI_EXPRESS_TO_PCI_BRIDGE 7
#define PCIE_EXPRESS_ERROR_PORT_TYPE_PCI_TO_PCI_EXPRESS_BRDIGE 8
#define PCIE_EXPRESS_ERROR_PORT_TYPE_ROOT_COMPLEX_INTEGRATED_ENDPOINT 9
#define PCIE_EXPRESS_ERROR_PORT_TYPE_ROOT_COMPLEX_EVENT_COLLECTOR 10

#define PCIE_EXPRESS_ERROR_CAP_SIZE 60
#define PCIE_EXPRESS_ERROR_AER_INFO_SIZE 96

typedef PACKED struct PACKED_POST
{
   uint64_t validation_bit;/*Bit 0 –Port Type Valid
                           Bit 1 – Version Valid
                           Bit 2 – Command Status Valid
                           Bit 3 – Device ID Valid
                           Bit 4 – Device Serial Number Valid
                           Bit 5 – Bridge Control Status Valid
                           Bit 6 – Capability Structure Status Valid
                           Bit 7 – AER Info Valid
                           Bit 8-63 – Reserved */
   uint32_t port_type; /*0: PCI Express End Point
                       1: Legacy PCI End Point Device
                       4: Root Port
                       5: Upstream Switch Port
                       6: Downstream Switch Port
                       7: PCI Express to PCI/PCI-X Bridge
                       8: PCI/PCI-X to PCI Express Bridge
                       9: Root Complex Integrated Endpoint Device
                       10: Root Complex Event Collector*/
   struct
   {
      uint8_t minor; /* in bcd*/
      uint8_t major;
      uint16_t res;
   }version;

   struct
   {
      uint16_t pci_command;
      uint16_t pci_status;
   }command_status;

   uint32_t reserved;

   PACKED struct PACKED_POST
   {
      uint16_t vendor_id;
      uint16_t device_id;
      uint8_t class_code[3];
      uint8_t func_num;
      uint8_t device_num;
      uint16_t seg_num;
      uint8_t bus_num;
      uint8_t sec_bus_num;
      uint16_t res_1:3;
      uint16_t slot_num:13;
      uint8_t res_2;
   }device_id;

   struct
   {
      uint32_t lower_dw;
      uint32_t upper_dw;
   }serial_num;

    struct
   {
      uint16_t bridge_sec_status;
      uint16_t bridge_control;
   }bridge_control_status;

   uint8_t capability_structure[PCIE_EXPRESS_ERROR_CAP_SIZE];
   uint8_t aer_info[PCIE_EXPRESS_ERROR_AER_INFO_SIZE];
}ras_pcie_express_error;



/* Error string length should be such that the header is 4 byte aligned*/
#define QC_ERROR_STRING_LENGTH 55
#define QC_CAP_REGISTERS_MAX 10

typedef enum
{
   RAS_QCOM_ERROR_INVALID = 0,
   RAS_QCOM_ERROR_IMEM_TYPE,
   RAS_QCOM_ERROR_BERT_TYPE,
   RAS_QCOM_ERROR_IMC_ERROR_FATAL_TYPE,
   RAS_QCOM_ERROR_PMIC_OVER_TEMP_TYPE,
   RAS_QCOM_ERROR_XPU_TYPE,
   RAS_QCOM_ERROR_EL3_FIRMWARE,
   RAS_QCOM_ERROR_CVR_ERROR_TYPE,
   RAS_QCOM_ERROR_OVERTEMP_TYPE,
   RAS_QCOM_ERROR_PCI_ATS,
   /*
    * New entries here and update all the below:
    *    http://qwiki.qualcomm.com/qct-enablement/Server/ACPI/RAS/VendorErrors
    *    boot_ras.h in XBL
    *    ras_cper.h in IMC
    */
   RAS_QCOM_ERROR_MAXIMUM,
} ras_qcom_error_type;

typedef enum {
    SECURE_DOMAIN = 0,
    NON_SECURE_DOMAIN,
    QAD0_DOMAIN
} domain_type_t;

typedef enum
{
   ERROR_PCC_RECOVERABLE =0,
   ERROR_PCC_FATAL =1,
   ERROR_PCC_CORRECTED =2,
   ERROR_PCC_INFORMATIONAL=3,
   ERROR_PCC_RESET=4,
   ERROR_PCC_MAX=4,
   ERROR_PCC_INVALID=0xFFFF
}ras_pcc_error_severity;

#define  RAS_XPU_ERROR_PRINT_STRING_SIZE 95
typedef struct _ras_xpu {
   char print_string[RAS_XPU_ERROR_PRINT_STRING_SIZE];
   uint64_t xpu_instance;
   uint64_t phys_addr;
   uint32_t error_status;
   uint32_t raw_synd0;
   uint32_t raw_synd1;
   uint32_t raw_synd2;
   ras_pcc_error_severity severity_level;
   domain_type_t xpu_err_domain;
} ras_xpu_error;

typedef enum {
	FIRMWARE_ERROR_INVALID = 0,
	FIRMWARE_ERROR_UNKNOWN_SYNC_EXCEPTION,
	FIRMWARE_ERROR_UNKNOWN_EXCEPTION_TYPE,
	FIRMWARE_ERROR_UNKNOWN_XPU_INTERRUPT,
	FIRMWARE_ERROR_IMC_SYNC_POINT_PCC_INIT_FAILED,
	FIRMWARE_ERROR_IMC_SYNC_POINT_PCC_SEND_FAILED,
	FIRMWARE_ERROR_FAILED_ROLLBACK_UPDATE,
	FIRMWARE_ERROR_AFSR_CHECK_FAILED,
	FIRMWARE_ERROR_ERROR_TYPE_CHECK_FAILED,
	FIRMWARE_ERROR_SYNDROME_CHECK_FAILED,
	FIRMWARE_ERROR_SERROR_NON_RECOVERABLE_PACKET_FAILED,
	FIRMWARE_ERROR_SERROR_RECOVERABLE_PACKET_FAILED,
	FIRMWARE_ERROR_SYNC_RECOVERABLE_PACKET_FAILED,
	FIRMWARE_ERROR_FAILED_RAS_GENERIC_HANDLER,
	FIRMWARE_ERROR_NON_FATAL_QSP_EXCEPTION,
	FIRMWARE_ERROR_SFS_INVALID_MM_SMC,
	FIRMWARE_ERROR_SFS_INVALID_TPM_SMC,
	FIRMWARE_ERROR_FATAL_XPU_VIOLATION,
	FIRMWARE_ERROR_FATAL_NS_INTERRUPT_IN_EL3,
	FIRMWARE_ERROR_FATAL_EL3_RAS_ERROR,
	FIRMWARE_ERROR_FAILED_SYNC_EXIT_SMC,
	FIRMWARE_ERROR_FAILED_QSP_SYNC_EXIT_EXCEPTION,
	FIRMWARE_ERROR_FAILED_SYNC_EXIT_MM_EXCEPTION,
	FIRMWARE_ERROR_FAILED_SYNC_EXIT_MM_INIT,
	FIRMWARE_ERROR_FAILED_EL3_HEAP_SETUP,
	FIRMWARE_ERROR_FAILED_EL3_MMU_SETUP,
	FIRMWARE_ERROR_FATAL_RAS_REPLAY_UNKNOWN_VBAR,
	FIRMWARE_ERROR_FATAL_RAS_REPLAY_UNKNOWN_EL,
	FIRMWARE_ERROR_FATAL_VOLTAGE_CONTROL_LIB_PCC_SEND_FAILED,
	FIRMWARE_ERROR_FATAL_RAS_PCC_SEND_FAIL,
	FIRMWARE_ERROR_FATAL_RAS_XPU_PCC_SEND_FAIL,
	FIRMWARE_ERROR_FATAL_RAS_XPU_SECURE_VIOLATION,
	FIRMWARE_ERROR_FATAL_FAILED_QSPD_SETUP,
	FIRMWARE_ERROR_FATAL_FAILED_EXIT_TO_NS_WORLD,
	FIRMWARE_ERROR_END = 0xFFFF,
} firmware_error_ids_t;

#define  RAS_EL3_ERROR_PRINT_STRING_SIZE 55
typedef struct _ras_el3 {
   char print_string[RAS_EL3_ERROR_PRINT_STRING_SIZE];
   uint32_t el3_firmware_packet_version;
   uint32_t esr_reg;
   uint64_t elr_reg;
   uint32_t spsr_reg;
   uint32_t afsr_reg;
   uint32_t scr_reg;
   uint32_t sctlr_reg;
   ras_pcc_error_severity severity_level;
   firmware_error_ids_t error_id;
   uint64_t mpidr;
} ras_el3_error;


#define QCOM_BERT_COREDUMP_MAX_SIZE 512
#define RAS_BERT_BUFFER_SIZE 32768

/* Vendor specific error structure used for IMEM*/
typedef struct _ras_qcom_specific_error_header
{
   uint8_t   error_type;
   uint8_t   error_string[QC_ERROR_STRING_LENGTH];
}ras_qcom_specific_error_header;

typedef struct _ras_imem_error
{
   uint32_t  hw_id;
   uint32_t  hw_ver;
   uint64_t  far_address;
   uint32_t  num_cap_registers;
   uint32_t  cap_register[QC_CAP_REGISTERS_MAX];
} ras_imem_error;

typedef struct _ras_pmic_overtemp_error
{
    pm_device_info_type pmic_device_info;
    uint8_t               slave_id;
    uint8_t               temp_alarm_fsm_state;
    uint8_t               temp_alarm_stage;
} ras_pmic_overtemp_error;

#define RAS_OVERTEMP_MAX_STRLEN 28
typedef struct _ras_overtemp_error
{
    uint16_t tz_uid;              // Unique ID of ThermalZone
    int16_t tz_critical_temp_dC;  // Critical temperature of ThermalZone
    char tz_desc[RAS_OVERTEMP_MAX_STRLEN]; // Description of ThermalZone
} ras_overtemp_error;

typedef struct _ras_cvr_rail_error
{
    avsbus_dev_rail_t       rail_id;
    uint8_t                 status_vout;
    uint8_t                 status_iout;
} ras_cvr_rail_error;

typedef struct _ras_cvr_error
{
    uint16_t            status_full;
    uint8_t             status_input;
    uint8_t             status_temperature;
    uint8_t             status_cml;
    uint8_t             rail_error_count;
    ras_cvr_rail_error  rail_error[AVSBUS_DEV_RAIL_TOTAL];
} ras_cvr_error;

typedef struct _ras_ats_error
{
    uint64_t            fault_address;
    uint32_t            error_status;
    uint16_t            segment;
    uint16_t            bdf;
    uint32_t            raw_syndrome[5];
} ras_ats_error;


typedef struct _ras_bert_error
{
   uint8_t coredump[QCOM_BERT_COREDUMP_MAX_SIZE];
} ras_bert_error;

typedef PACKED struct PACKED_POST
{
   ras_qcom_specific_error_header header;
   union {
      ras_imem_error     imem_error;
      ras_bert_error     bert_error;
      ras_pmic_overtemp_error    pmic_overtemp_error;
      ras_overtemp_error overtemp_error;
      ras_xpu_error      xpu_error;
      ras_el3_error      el3_error;
      ras_cvr_error      cvr_error;
      ras_ats_error      ats_error;
   }ras_error;


}ras_qcom_specific_error;

/* EINJ Error types*/
#define RAS_EINJ_PROCESSOR_CORRECTABLE_ERROR (1<<0)
#define RAS_EINJ_PROCESSOR_UNCORRECTABLE_NONFATAL_ERROR (1<<1)
#define RAS_EINJ_PROCESSOR_UNCORRECTABLE_FATAL_ERROR (1<<2)
#define RAS_EINJ_MEMORY_CORRECTABLE_ERROR (1<<3)
#define RAS_EINJ_MEMORY_UNCORRECTABLE_NONFATAL_ERROR (1<<4)
#define RAS_EINJ_MEMORY_UNCORRECTABLE_FATAL_ERROR (1<<5)
#define RAS_EINJ_PCIE_CORRECTABLE_ERROR (1<<6)
#define RAS_EINJ_PCIE_UNCORRECTABLE_NONFATAL_ERROR (1<<7)
#define RAS_EINJ_PCIE_UNCORRECTABLE_FATAL_ERROR (1<<8)

typedef struct ras_einj_set_error_type_addr_struct
{
   uint32_t error_type;
   uint32_t vendor_offset;
   uint32_t flags;
   uint32_t apcid;
   uint64_t memory_address;
   uint64_t memory_address_range;
   uint32_t pcie_sbdf;
}ras_einj_set_error_type_addr;

static const uuid_t PLATFORM_MEMORY_SECTION_GUID={0xA5BC1114, 0x6F64, 0x4EDE, {0xB8, 0x63, 0x3E, 0x83, 0xED, 0x7C, 0x83, 0xB1}};
static const uuid_t GENERIC_PROCESSOR_SECTION_GUID={0x9876CCAD, 0x47B4, 0x4BDB, {0xB6, 0x5E, 0x16, 0xF1, 0x93, 0xC4, 0xF3, 0xD8}};
static const uuid_t ARMV8_PROCESSOR_SECTION_GUID={0xe19e3d16, 0xbc11, 0x11e4, {0x9c, 0xaa, 0xc2, 0x05, 0x1d, 0x5d, 0x46, 0xb0}};
static const uuid_t PCIE_EXPRESS_ERROR_SECTION_GUID= {0xd995e954, 0xbbc1, 0x430f, {0xad, 0x91, 0xb4, 0x4d, 0xcb, 0x3c, 0x6f, 0x35}};

#endif /* __UEFI_CPER_H_INCLUDE__ */
