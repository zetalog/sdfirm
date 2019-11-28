#ifndef __SCSI_H_INCLUDE__
#define __SCSI_H_INCLUDE__

#include <target/generic.h>
#include <target/bulk.h>

typedef uint8_t scsi_lun_t;	/* logical unit number */
typedef uint8_t scsi_tag_t;	/* task identifier */
typedef uint8_t scsi_crn_t;	/* command reference number */
typedef uint8_t scsi_pid_t;	/* port identifier */
typedef uint8_t scsi_status_t;
#ifdef CONFIG_SCSI_LBA_64BIT
typedef uint64_t scsi_lba_t;
typedef uint64_t scsi_lbs_t;
#else
typedef uint32_t scsi_lba_t;
typedef uint32_t scsi_lbs_t;
#endif

#define SCSI_VERSION_NONE	0x00
#define SCSI_VERSION_SPC	0x03
#define SCSI_VERSION_SPC2	0x04
#define SCSI_VERSION_SPC3	0x05
#define SCSI_VERSION		SCSI_VERSION_SPC3

#ifdef CONFIG_SCSI_INITIATOR
typedef uint32_t scsi_nexus_t;
#define SCSI_NEXUS_I(nexus)			HIBYTE(HIWORD(nexus))
#define SCSI_NEXUS_T(nexus)			LOBYTE(HIWORD(nexus))
#define SCSI_NEXUS_L(nexus)			HIBYTE(LOWORD(nexus))
#define SCSI_NEXUS_Q(nexus)			LOBYTE(LOWORD(nexus))
#define SCSI_NEXUS_C_ITLQ(I, T, L, Q)		MAKELONG(MAKEWORD(Q, L), MAKEWORD(T, I))
#define INVALID_SCSI_NEXUS			0xFFFFFFFF
#else
typedef uint16_t scsi_nexus_t;
#define SCSI_NEXUS_I(nexus)			0
#define SCSI_NEXUS_T(nexus)			0
#define SCSI_NEXUS_L(nexus)			HIBYTE(nexus)
#define SCSI_NEXUS_Q(nexus)			LOBYTE(nexus)
/* constructors */
#define SCSI_NEXUS_C_LQ(L, Q)			MAKEWORD(Q, L)
#define SCSI_NEXUS_C_ITLQ(I, T, L, Q)		SCSI_NEXUS_C_LQ(Q, L)
#define INVALID_SCSI_NEXUS			0xFFFF
#endif

/* ADDRESS METHOD Field Values
 *
 * ADDRESS METHOD field is in the addressing fields of the logical unit
 * number.  See Table 7 - ADDRESS METHOD field values in the chapter 4.9.5
 * Eight byte logical unit number structure of the SAM-3 for more details.
 */
#define SCSI_ADDR_PERIPH_DEV			0x00
#define SCSI_ADDR_FLAT_SPACE			0x01
#define SCSI_ADDR_LOGIC_UNIT			0x02
#define SCSI_ADDR_EX_LOGIC_UNIT			0x03

/* Service Response */
#define SCSI_SERV_RESP_TASK_CMPL		0x01	/* task complete */
#define SCSI_SERV_RESP_LINKED_CMND_CMPL		0x02	/* linked command complete */
#define SCSI_SERV_RESP_DELIVERY_OR_TRGT_FAIL	0x03	/* service delivery or target failure */

/* Status Codes
 *
 * See Table 22 - Status codes in the chapter 5.3.1 Status codes of the
 * SAM-3 for more details.
 */
#define SCSI_STATUS_GOOD			0x00
#define SCSI_STATUS_CHECK_CONDITION		0x02
#define SCSI_STATUS_CONDITION_MET		0x04
#define SCSI_STATUS_BUSY			0x08
#define SCSI_STATUS_INTERMEDIATE		0x10
#define SCSI_STATUS_INTERMEDIATE_CONDITION_MET	0x14
#define SCSI_STATUS_RESERVATION_CONFLICT	0x18
#define SCSI_STATUS_TASK_SET_FULL		0x28
#define SCSI_STATUS_ACA_ACTIVE			0x30
#define SCSI_STATUS_TASK_ABORTED		0x40

/* CONTROL byte in CDB */
#define SCSI_CONTROL_MASK_VENDOR_SPEC		0xC0
#define SCSI_CONTROL_FLAG_NACA			0x04
#define SCSI_CONTROL_FLAG_LINK			0x01
#define SCSI_CONTROL_LINK(ctrl)			(ctrl & SCSI_CONTROL_FLAG_LINK)
#define SCSI_CONTROL_NACA(ctrl)			(ctrl & SCSI_CONTROL_FLAG_NACA)

#define SCSI_EVENT_POWER_ON			0x01
#define SCSI_EVENT_HARD_RESET			0x02
#define SCSI_EVENT_LOGICAL_UNIT_RESET		0x04
#define SCSI_EVENT_I_T_NEXUS_LOSS		0x08

#define SCSI_CDB_GROUP_MASK			0xE0
#define SCSI_CDB_GROUP_OFFSET			5
#define SCSI_CDB_GROUP(opcode)			\
	((opcode & SCSI_CDB_GROUP_MASK) >> SCSI_CDB_GROUP_OFFSET)
#define SCSI_CDB_GROUP_6			0x00
#define SCSI_CDB_GROUP_10_1			0x01
#define SCSI_CDB_GROUP_10_2			0x02
#define SCSI_CDB_GROUP_16			0x04
#define SCSI_CDB_GROUP_12			0x05
#define SCSI_CDB_GROUP_VAR			0x03

#define SCSI_PERIPH_QUAL_SPEC_CONN		0x00
#define SCSI_PERIPH_QUAL_SPEC_NOTCONN		0x01
#define SCSI_PERIPH_QUAL_NOTSPEC		0x03

#define SCSI_DEVICE_DIRECT_ACCESS_BLOCK		0x00	/* SBC-2 */
#define SCSI_DEVICE_SEQUENTIAL_ACCESS		0x01	/* SSC-2 */
#define SCSI_DEVICE_PRINTER			0x02	/* SSC */
#define SCSI_DEVICE_PROCESSOR			0x03	/* SPC-2 */
#define SCSI_DEVICE_WRITE_ONCE			0x04	/* SBC */
#define SCSI_DEVICE_CD_DVD			0x05	/* MMC-4 */
#define SCSI_DEVICE_OPTICAL_MEMORY		0x07	/* SBC */
#define SCSI_DEVICE_MEDIUM_CHARGER		0x08	/* SMC-2 */
#define SCSI_DEVICE_STORAGE_ARRAY_CONTROLLER	0x0C	/* SCC-2 */
#define SCSI_DEVICE_ENCLOSURE_SERVICES		0x0D	/* SES */
#define SCSI_DEVICE_SIMPLIFIED_DIRECT_ACCESS	0x0E	/* RBC */
#define SCSI_DEVICE_OPTICAL_CARD_READER_WRITER	0x0F	/* OCRW */
#define SCSI_DEVICE_BRIDGE_CONTROLLER_COMMANDS	0x10	/* BCC */
#define SCSI_DEVICE_OBJECT_BASED_STORAGE	0x11	/* OSD */

struct scsi_sense_desc {
	uint8_t type;
#define SCSI_SENSE_DESC_INFO			0x00
#define SCSI_SENSE_DESC_CMND_SPEC_INFO		0x01
#define SCSI_SENSE_DESC_KEY_SPEC		0x02
#define SCSI_SENSE_DESC_FIELD_REP_UNIT		0x03
#define SCSI_SENSE_DESC_STREAM_CMNDS		0x04
#define SCSI_SENSE_DESC_BLOCK_CMNDS		0x05
#define SCSI_SENSE_DESC_OSD_OBJ_IDENT		0x06
#define SCSI_SENSE_DESC_OSD_RESP_CHECK		0x07
#define SCSI_SENSE_DESC_OSD_ATTR_IDENT		0x08
#define SCSI_SENSE_DESC_ATA_RETURN		0x09
	uint8_t additional_legth;
};

struct scsi_sense {
#define SCSI_RESPONSE_CODE_CURRENT		0x70
#define SCSI_RESPONSE_CODE_DEFERRED		0x71
#define SCSI_RESPONSE_CODE_DESC			0x02

#define SCSI_SENSE_FLAG_VALID			0x01
#define SCSI_SENSE_FLAG_FILEMARK		0x02
#define SCSI_SENSE_FLAG_EOM			0x04
#define SCSI_SENSE_FLAG_ILI			0x08
#define SCSI_SENSE_FLAG_SKSV			0x10

#define SCSI_SENSE_FIXED_VALID			0x80
#define SCSI_SENSE_FIXED_FILEMARK		0x80
#define SCSI_SENSE_FIXED_EOM			0x40
#define SCSI_SENSE_FIXED_ILI			0x20
#define SCSI_SENSE_FIXED_SKSV			0x80
	uint8_t flags;

	uint8_t key;
#define SCSI_SENSE_NO_SENSE				0x00
#define SCSI_SENSE_RECOVERED_ERROR			0x01
#define SCSI_SENSE_NOT_READY				0x02
#define SCSI_SENSE_MEDIUM_ERROR				0x03
#define SCSI_SENSE_HARDWARE_ERROR			0x04
#define SCSI_SENSE_ILLEGAL_REQUEST			0x05
#define SCSI_SENSE_UNIT_ATTENTION			0x06
#define SCSI_SENSE_DATA_PROTECT				0x07
#define SCSI_SENSE_BLANK_CHECK				0x08
#define SCSI_SENSE_VENDOR_SPECIFIC			0x09
#define SCSI_SENSE_COPY_ABORTED				0x0A
#define SCSI_SENSE_ABORTED_COMMAND			0x0B
#define SCSI_SENSE_VOLUME_OVERFLOW			0x0D
#define SCSI_SENSE_MISCOMPARE				0x0E
	/* additional sense code */
	uint8_t asc;
#define SCSI_ASC_NO_ADDITIONAL_SENSE_INFO		0x00
#define SCSI_ASC_LOGICAL_UNIT_NOT_READY			0x04
#define SCSI_ASC_INVALID_COMMAND_OPERATION_CODE		0x20
#define SCSI_ASC_LOGICAL_BLOCK_ADDRESS_OUT_OF_RANGE	0x21
#define SCSI_ASC_INVALID_FIELD_IN_CDB			0x24
#define SCSI_ASC_LOGICAL_UNIT_NOT_SUPPORTED		0x25
#define SCSI_ASC_NOT_READY_TO_READY_CHANGE		0x28
#define SCSI_ASC_ROUNDED_PARAMETER			0x37
#define SCSI_ASC_SAVING_PARAMETERS_NOT_SUPPORTED	0x39
#define SCSI_ASC_LOGICAL_UNIT_FAILED_SELF_TEST		0x3E
#define SCSI_ASC_INVALID_MESSAGE_ERROR			0x49
	/* additional sense code qualifier */
	uint8_t ascq;
#define SCSI_ASCQ_LOGICAL_UNIT_FAILED_SELF_TEST		0x03
#define SCSI_ASCQ_SELF_TEST_IN_PROGRESS			0x09
	/* field replaceable unit code */
	uint8_t fruc;
	/* sense key specific */
	uint32_t spec;
};

#include <target/scsi_sbc.h>

#define SCSI_CDB_BITS(cdb, name)				\
	((cdb[name##_IDX] & name##_MSK) >> name##_OFF)
#define SCSI_CDB_BYTE(cdb, name)				\
	(cdb[name##_IDX])
#define scsi_cdb_byte(cdb, index)				\
	((uint8_t)(cdb[index]))
#define scsi_cdb_word(cdb, index)				\
	MAKEWORD(scsi_cdb_byte(cdb, (uint8_t)(index+1)),	\
		 scsi_cdb_byte(cdb, index))
#define scsi_cdb_long(cdb, index)				\
	MAKELONG(scsi_cdb_word(cdb, (uint8_t)(index+2)),	\
		 scsi_cdb_word(cdb, index))
#define scsi_getll(cdb, index)					\
	MAKELLONG(scsi_cdb_long(cdb, (uint8_t)(index+4)),	\
		  scsi_cdb_long(cdb, index))

struct scsi_cmnd_request_sense {
	boolean desc;
	uint8_t length;
#define SCSI_REQUEST_SENSE_DESC_IDX		1
#define SCSI_REQUEST_SENSE_DESC_MSK		0x01
#define SCSI_REQUEST_SENSE_DESC_OFF		0
#define SCSI_SENSE_FIXED_MIN_LENGTH		0x11
};

struct scsi_cmnd_inquiry {
	boolean evpd;
	uint8_t page_code;
	uint8_t length;
#define SCSI_INQUIRY_EVPD_IDX			1
#define SCSI_INQUIRY_EVPD_MSK			0x01
#define SCSI_INQUIRY_EVPD_OFF			0
#define SCSI_INQUIRY_PAGE_CODE_IDX		2

#define SCSI_STANDARD_INQUIRY_DATA		0x00	/* for inquiry interface */
#define SCSI_VPD_SUPPORTED_VPD_PAGES		0x00
#define SCSI_VPD_ASCII_INFO_BEGIN		0x01
#define SCSI_VPD_ASCII_INFO_END			0x7F
#define SCSI_VPD_UNIT_SERIAL_NUMBER		0x80
#define SCSI_VPD_DEVICE_IDENTIFICATION		0x83
#define SCSI_VPD_SOFT_INTF_IDENTIFICATION	0x84
#define SCSI_VPD_MGR_NETWORK_ADDRESSES		0x85
#define SCSI_VPD_EXTENDED_INQUIRY_DATA		0x86
#define SCSI_VPD_MODE_PAGE_POLICY		0x87
#define SCSI_VPD_SCSI_PORTS			0x88

#define SCSI_INQUIRY_RMB			0x80
#define SCSI_INQUIRY_NORMACA			0x20
#define SCSI_INQUIRY_HISUP			0x10
#define SCSI_INQUIRY_RESP_DATA_FORMAT		0x02
#define SCSI_INQUIRY_SCCS			0x80
#define SCSI_INQUIRY_ACC			0x40
#define SCSI_INQUIRY_TPGS			0x00
#define SCSI_INQUIRY_3PC			0x08
#define SCSI_INQUIRY_PROTECT			0x01
#define SCSI_INQUIRY_BQUE			0x80
#define SCSI_INQUIRY_ENCSERV			0x40
#define SCSI_INQUIRY_MULTIP			0x10
#define SCSI_INQUIRY_MCHNGR			0x08
#define SCSI_INQUIRY_ADDR16			0x01
#define SCSI_INQUIRY_WBUS16			0x20
#define SCSI_INQUIRY_SYNC			0x10
#define SCSI_INQUIRY_LINKED			0x08
#define SCSI_INQUIRY_CMDQUE			0x02
#define SCSI_INQUIRY_CLOCKING			0x0C
#define SCSI_INQUIRY_QAS			0x02
#define SCSI_INQUIRY_IUS			0x01
#define SCSI_VER_DESC_SAM3			0x60
#define SCSI_VER_DESC_SPC3			0x300

#define SCSI_INQUIRY_MIN_LENGTH			96
/* for filling ASCII spaces */
#define SCSI_INQUIRY_SPACE			0x20
/* for filling binary spaces */
#define SCSI_INQUIRY_RESERVED			0x00

#define SCSI_INQUIRY_EXT_LENGTH			0x3C
#define SCSI_INQUIRY_EXT_RTO			0x08
#define SCSI_INQUIRY_EXT_GRD_CHK		0x04
#define SCSI_INQUIRY_EXT_APP_CHK		0x02
#define SCSI_INQUIRY_EXT_REF_CHK		0x01
#define SCSI_INQUIRY_EXT_GROUP_SUP		0x10
#define SCSI_INQUIRY_EXT_PRIOR_SUP		0x08
#define SCSI_INQUIRY_EXT_HEADSUP		0x04
#define SCSI_INQUIRY_EXT_ORDSUP			0x02
#define SCSI_INQUIRY_EXT_SIMPSUP		0x01
#define SCSI_INQUIRY_EXT_NV_SUP			0x02
#define SCSI_INQUIRY_EXT_V_SUP			0x01
};

struct scsi_cmnd_report_luns {
	uint8_t select_report;
#define SCSI_REPORT_LUNS_SELECT_REPORT_IDX	2
#define SCSI_REPORT_LUNS_ADDRESSING		0x00
#define SCSI_REPORT_LUNS_WELL_KNOWN		0x01
#define SCSI_REPORT_LUNS_ALL			0x02
};

#define SCSI_MODE_PAGE_POLICY_SHARED		0x00
#define SCSI_MODE_PAGE_POLICY_TARGET		0x01
#define SCSI_MODE_PAGE_POLICY_INITIATOR		0x02
#define SCSI_MODE_PAGE_POLICY_I_T_NEXUS		0x03

struct scsi_cmnd_mode_sense {
#define SCSI_MODE_SENSE_DBD_IDX			1
#define SCSI_MODE_SENSE_DBD_MSK			0x08
#define SCSI_MODE_SENSE_DBD_OFF			3
#define SCSI_MODE_SENSE_PC_IDX			2
#define SCSI_MODE_SENSE_PC_MSK			0xC0
#define SCSI_MODE_SENSE_PC_OFF			6
#define SCSI_MODE_SENSE_PAGE_CODE_IDX		2
#define SCSI_MODE_SENSE_PAGE_CODE_MSK		0x6F
#define SCSI_MODE_SENSE_PAGE_CODE_OFF		0
#define SCSI_MODE_SENSE_SUBPAGE_CODE_IDX	3
	uint8_t dbd;
	uint8_t pc;
#define SCSI_MODE_SENSE_PC_CURRENT		0x00
#define SCSI_MODE_SENSE_PC_CHANGEABLE		0x01
#define SCSI_MODE_SENSE_PC_DEFAULT		0x02
#define SCSI_MODE_SENSE_PC_SAVED		0x03
	uint8_t page_code;
	uint8_t subpage_code;
#define SCSI_MODE_SENSE_SUBPAGE_PAGE0		0x00
#define SCSI_MODE_SENSE_SUBPAGE_ALL		0xFF
};

struct scsi_cmnd_mode_select {
#define SCSI_MODE_SELECT_PF_IDX			1
#define SCSI_MODE_SELECT_PF_MSK			0x10
#define SCSI_MODE_SELECT_PF_OFF			4
#define SCSI_MODE_SELECT_SP_IDX			1
#define SCSI_MODE_SELECT_SP_MSK			0x01
#define SCSI_MODE_SELECT_SP_OFF			0
#define SCSI_MODE_SELECT_PARAMETER_LIST_LENGTH_IDX	4
	uint8_t pf;
	uint8_t sp;
};

struct scsi_cmnd_prevent_allow_medium_removal {
#define SCSI_PREVENT_ALLOW_MEDIUM_REMOVAL_PREVENT_IDX	4
#define SCSI_PREVENT_ALLOW_MEDIUM_REMOVAL_PREVENT_MSK	0x03
#define SCSI_PREVENT_ALLOW_MEDIUM_REMOVAL_PREVENT_OFF	0
	uint8_t prevent;
#define SCSI_RMB_PREVENT_NONE	0x00
#define SCSI_RMB_PREVENT_XPRT	0x01
#define SCSI_RMB_PREVENT_MCHNGR	0x02
#define SCSI_RMB_PREVENT_BOTH	0x03
};

union scsi_cmnd_fields {
	struct scsi_cmnd_inquiry inquiry;
	struct scsi_cmnd_request_sense request_sense;
	struct scsi_cmnd_report_luns report_luns;
	/* SPC configurable commands */
#ifdef CONFIG_SCSI_MODE
	struct scsi_cmnd_mode_sense mode_sense;
	struct scsi_cmnd_mode_select mode_select;
#endif
#ifdef CONFIG_SCSI_RMB
	struct scsi_cmnd_prevent_allow_medium_removal prevent_allow_medium_removal;
#endif

	/* device type specific */
#ifdef CONFIG_SCSI_SBC
	/* SBC commands */
	struct sbc_cmnd_read_capacity read_capacity;
	struct sbc_cmnd_read read;
	struct sbc_cmnd_write write;
#endif
};

/* mandatory commands defined in SPC-3 */
/* group 6 */
#define SCSI_CMND_TEST_UNIT_READY		0x00
#define SCSI_CMND_REQUEST_SENSE			0x03
#define SCSI_CMND_INQUIRY			0x12
#define SCSI_CMND_MODE_SELECT_6			0x15
#define SCSI_CMND_MODE_SENSE_6			0x1A
#define SCSI_CMND_PREVENT_ALLOW_MEDIUM_REMOVAL	0x1E
/* group 10 */
#define SCSI_CMND_MODE_SELECT_10		0x55
#define SCSI_CMND_MODE_SENSE_10			0x5A
/* group 12 */
#define SCSI_CMND_REPORT_LUNS			0xA0

#define SCSI_TASK_ENDED				0x00
#define SCSI_TASK_DORMANT			0x01
#define SCSI_TASK_BLOCKED			0x02
#define SCSI_TASK_ENABLED			0x03

#define SCSI_TASK_SIMPLE			0x01
#define SCSI_TASK_ORDERED			0x02
#define SCSI_TASK_HEAD_OF_QUEUE			0x04
#define SCSI_TASK_ACA				0x08

/* mode page page and subpage codes */
#define SCSI_MODE_PAGE_CONTROL			0x0A
#define SCSI_MODE_PAGE_DISCONNECT_RECONNECT	0x02
#define SCSI_MODE_PAGE_EXTENDED			0x15
#define SCSI_MODE_PAGE_EXTENDED_DEV_SPECIFIC	0x16
#define SCSI_MODE_PAGE_INFO_EXCEPT_CONTROL	0x1C
#define SCSI_MODE_PAGE_POWER_CONDITION		0x1A
#define SCSI_MODE_PAGE_PROTOCOL_SPECIFIC_LUN	0x18
#define SCSI_MODE_PAGE_PROTOCOL_SPECIFIC_PORT	0x19
#define SCSI_MODE_PAGE_VENDOR_SPECIFIC		0x00
#define SCSI_MODE_PAGE_RETURN_ALL		0x3F

#define SCSI_MODE_SUBPAGE_CONTROL_EXTENSION	0x01
#define SCSI_MODE_SUBPAGE_RETURN_ALL		0xFF

#include <target/scsi_target.h>

#endif /* __SCSI_H_INCLUDE__ */
