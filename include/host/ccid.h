#ifndef __CCID_H_INCLUD__
#define __CCID_H_INCLUD__

#include <host/list.h>

typedef int scs_size_t;

#define SCD_DT_SCD			(USB_TYPE_CLASS | 0x01)

/* Smart Card Device Class */
typedef struct scd_desc {
	uint8_t	 bLength;
#define SCD_DT_SCD_SIZE	0x36

	uint8_t	 bDescriptorType;
	uint16_t bcdCCID;

	uint8_t	 bMaxSlotIndex;
	uint8_t	 bVoltageSupport;
#define SCD_VOLTAGE_5V		0x01
#define SCD_VOLTAGE_3V		0x02
#define SCD_VOLTAGE_1_8V	0x04
#define SCD_VOLTAGE_ALL		(SCD_VOLTAGE_5V | \
				 SCD_VOLTAGE_3V | \
				 SCD_VOLTAGE_1_8V)
#define SCD_VOLTAGE_AUTO	0x00

	uint32_t dwProtocols;
#define SCD_PROTOCOL_NONE	0x0000

	uint32_t dwDefaultClock;
	uint32_t dwMaximumClock;
	uint8_t	 bNumClockSupported;
#define ICCD_FIXED_CLOCK	0x0DFC

	uint32_t dwDataRate;
	uint32_t dwMaxDataRate;
	uint8_t	 bNumDataRatesSupported;
#define ICCD_FIXED_DATARATE	0x2580

	uint32_t dwMaxIFSD;
	uint32_t dwSynchProtocols;
#define SCD_SYNCH_PROTO_NONE		0x0000
#define SCD_SYNCH_PROTO_2W		0x0001
#define SCD_SYNCH_PROTO_3W		0x0002
#define SCD_SYNCH_PROTO_I2C		0x0004

	uint32_t dwMechanical;
#define SCD_MECHA_NONE			0x00
#define SCD_MECHA_ACCEPT		0x01
#define SCD_MECHA_EJECTION		0x02
#define SCD_MECHA_CAPTURE		0x04
#define SCD_MECHA_LOCK			0x08
#define SCD_MECHA_NONE			0x00

	uint32_t dwFeatures;
#define SCD_FEATURE_NONE		0x00
#define SCD_FEATURE_AUTO_CONFIG		0x00000002
#define SCD_FEATURE_AUTO_ACTIVATE	0x00000004
#define SCD_FEATURE_AUTO_CLASS		0x00000008
#define SCD_FEATURE_AUTO_FREQ		0x00000010
#define SCD_FEATURE_AUTO_DATA		0x00000020

#define SCD_FEATURE_SLOT_MASK		0x0000000C

#define SCD_FEATURE_AUTO_PPS_PROP	0x00000040
#define SCD_FEATURE_AUTO_PPS_BASE	0x00000080

#define SCD_FEATURE_CAP_CLOCK_STOP	0x00000100
#define SCD_FEATURE_CAP_ACCEPT_NAD	0x00000200
#define SCD_FEATURE_AUTO_IFSD		0x00000400

#define SCD_FEATURE_XCHG_CHAR		0x00000000
#define SCD_FEATURE_XCHG_TPDU		0x00010000
#define SCD_FEATURE_XCHG_APDU		0x00020000
#define SCD_FEATURE_XCHG_APDU_EXT	0x00040000
#define SCD_FEATURE_XCHG_MASK		0x00070000

#define SCD_FEATURE_WAKEUP_ICC		0x00100000

#define ICCD_FEATURE_SPECIFIC		0x00000800
#define ICCD_FEATURE_DEFAULT		(ICCD_FEATURE_SPECIFIC | SCD_FEATURE_AUTO_PPS_PROP)

	uint32_t dwMaxCCIDMessageLength;

	uint8_t	 bClassGetResponse;
	uint8_t	 bClassEnvelope;
#define SCD_MUTE_APDU_CLASS		0x00
#define SCD_ECHO_APDU_CLASS		0xFF

	uint16_t wLcdLayout;
#define SCD_LCD_LAYOUT_NONE		0x0000

	uint8_t	 bPINSupport;
#define SCD_SPE_SUPPORT_NONE		0x00
#define SCD_SPE_SUPPORT_VERIFY		0x01
#define SCD_SPE_SUPPORT_MODIFY		0x02
#define SCD_SPE_SUPPORT_ALL		(SCD_SPE_SUPPORT_VERIFY | \
					 SCD_SPE_SUPPORT_MODIFY)

	uint8_t	 bMaxCCIDBusySlots;
} scd_desc_t;

#define CCID_REQ_ABORT			0x01
#define CCID_REQ_GET_CLOCK_FREQS	0x02
#define CCID_REQ_GET_DATA_RATES		0x03

#define SCD_DT_SCD			(USB_TYPE_CLASS | 0x01)

typedef void (*ccid_cmpl_cb)(void *data);

struct ccid_cmd {
	uint8_t bMessageType;
	uint8_t bSlot;
	uint8_t bSeq;
	ccid_cmpl_cb cb;
	void *cb_data;
	struct list_head link;
};

struct ccid_t0_param {
	uint8_t bmFindexDindex;
	uint8_t bmTCCKST0;
	uint8_t bGuardTimeT0;
	uint8_t bWaitingIntegerT0;
	uint8_t bClockStop;
};

struct ccid_t1_param {
	uint8_t bmFindexDindex;
	uint8_t bmTCCKST1;
	uint8_t bGuardTimeT1;
	uint8_t bWaitingIntegerT1;
	uint8_t bClockStop;
	uint8_t bIFSC;
	uint8_t bNadValue;
};

#define SCD_HEADER_SIZE			10
#define CCID_MESSAGE_SIZE		(IFD_BUF_SIZE + SCD_HEADER_SIZE)

#define SCD_ENDP_BULK_IN		0x00
#define SCD_ENDP_BULK_OUT		0x01
#define SCD_ENDP_INTR_IN		0x02

#define SCD_SLOT_STATUS_ACTIVE		0x00
#define SCD_SLOT_STATUS_INACTIVE	0x01
#define SCD_SLOT_STATUS_NOTPRESENT	0x02
#define SCD_SLOT_STATUS_MASK		0x03

#define CCID_ERROR_CMD_ABORTED			0xFF
#define CCID_ERROR_XFR_PARITY_ERROR		0xFD
#define CCID_ERROR_BAD_ATR_TS			0xF8
#define CCID_ERROR_BAD_ATR_TCK			0xF7
#define CCID_ERROR_PROTO_UNSUPPORT		0xF6
#define	CCID_ERROR_ICC_CLASS_NOSUP		0xF5
#define CCID_ERROR_BAD_T0_PB			0xF4
#define CCID_ERROR_PROTO_DEACTIVATE		0xF3
#define CCID_ERROR_BUSY_AUTO_SEQ		0xF2
#define CCID_ERROR_PIN_TIMEOUT			0xF0
#define CCID_ERROR_PIN_CANCELLED		0xEF
#define CCID_ERROR_CMD_SLOT_BUSY		0xE0

#define SCD_ERROR_ICC_MUTE			0xFE
#define SCD_ERROR_XFR_OVERRUN			0xFC
#define SCD_ERROR_HW_ERROR			0xFB
#define SCD_ERROR_USER_DEFINED			0xC0
#define SCD_ERROR_USER(e)			(SCD_ERROR_USER_DEFINED-e)
#define SCD_ERROR_RESERVED			0x80
#define SCD_ERROR_CMD_UNSUPPORT			0x00

#define SCD_CMD_STATUS_SUCC		(0x00 << 6)
#define SCD_CMD_STATUS_FAIL		(0x01 << 6)
#define SCD_CMD_STATUS_TIME_EXT		(0x02 << 6)
#define SCD_CMD_STATUS_MASK		(0xc0)

/* clock status for RDR2PC_SlotStatus */
#define CCID_CLOCK_RUNNING		0x00
#define CCID_CLOCK_STOPPED_L		0x01
#define CCID_CLOCK_STOPPED_H		0x02
#define CCID_CLOCK_STOPPED_U		0x03

#define SCD_PC2RDR_ICCPOWERON		0x62
#define SCD_PC2RDR_ICCPOWEROFF		0x63
#define SCD_PC2RDR_GETSLOTSTATUS	0x65
#define SCD_PC2RDR_ESCAPE		0x6B
#define SCD_PC2RDR_GETPARAMETERS	0x6C
#define SCD_PC2RDR_XFRBLOCK		0x6F

#define CCID_PC2RDR_SETPARAMETERS	0x61
#define CCID_PC2RDR_SECURE		0x69
#define CCID_PC2RDR_T0APDU		0x6A
#define CCID_PC2RDR_RESETPARAMETERS	0x6D
#define CCID_PC2RDR_ICCCLOCK		0x6E
#define CCID_PC2RDR_MECHANICAL		0x71
#define CCID_PC2RDR_ABORT		0x72
#define CCID_PC2RDR_SETDATAANDFREQ	0x73

#define SCD_RDR2PC_DATABLOCK		0x80
#define SCD_RDR2PC_SLOTSTATUS		0x81
#define CCID_RDR2PC_PARAMETERS		0x82
#define SCD_RDR2PC_ESCAPE		0x83
#define CCID_RDR2PC_DATARATEANDCLOCK	0x84

#define SCD_RDR2PC_NOTIFYSLOTCHANGE	0x50
#define CCID_RDR2PC_HARDWAREERROR	0x51	

/* CCID_PC2RDR_ICCCLOCK */
#define CCID_CLOCK_RESTART		0x00
#define CCID_CLOCK_STOP			0x01

/* CCID_PC2RDR_MECHANICAL */
#define CCID_MECHA_ACCEPT_CARD		0x01
#define CCID_MECHA_EJECT_CARD		0x02
#define CCID_MECHA_CAPTURE_CARD		0x03
#define CCID_MECHA_LOCK_CARD		0x04
#define CCID_MECHA_UNLOCK_CARD		0x05

#define CCID_SPE_DEFAULT_MSGID	0xff

#define CCID_SPE_PIN_VERIFY	0x00
#define CCID_SPE_PIN_MODIFY	0x01
#define CCID_SPE_PIN_CANCEL	0x02
#define CCID_SPE_RESEND_IBLOCK	0x03
#define CCID_SPE_SEND_NEXTAPDU	0x04

#define CCID_SPE_MAX_PIN	16

struct ccid_po_param {
	uint8_t bTimeout;
	uint8_t bmFormatString;
	uint8_t bmPINBlockString;
	uint8_t bmPINLengthFormat;
};

/* PIN verification */
struct ccid_pv_param {
	struct ccid_po_param po;
	uint16_t wPINMaxExtraDigit;
	uint8_t bEntryValidationCondition;
	uint8_t bNumberMessage;
	uint16_t wLangId;
	uint8_t bMsgIndex;
	uint8_t bTeoPrologue[3];
};

#define CCID_SPE_VERIFY_FIXED	9	/* bytes before bNumberMessage */
#define CCID_SPE_VERIFY_SIZE	15

/* PIN modification */
struct ccid_pm_param {
	struct ccid_po_param po;
	uint8_t bInsertionOffsetOld;
	uint8_t bInsertionOffsetNew;
	uint16_t wPINMaxExtraDigit;
	uint8_t bConfirmPIN;
	uint8_t bEntryValidationCondition;
	uint8_t bNumberMessage;
	uint16_t wLangId;
	uint8_t bMsgIndex[3];
	uint8_t bTeoPrologue[3];
};

#define CCID_SPE_MODIFY_FIXED	12	/* bytes before bNumberMessage */
#define CCID_SPE_MODIFY_SIZE	18

#define CCID_MAX_BUFFER		(IFD_BUF_SIZE + SCD_HEADER_SIZE + sizeof (struct ccid_pm_param))

#define CCID_INVALID_SEQ	0xFF

#define CCID_CLASS_AUTO			0x00
#define CCID_CLASS_A			0x01
#define CCID_CLASS_B			0x02
#define CCID_CLASS_C			0x03
#define CCID_CLASS_INVAL		0x04

#define CCID_INFO_XCHG_SPECIFIC		0x00
#define CCID_INFO_XCHG_NEGO_PPS		0x01
#define CCID_INFO_XCHG_NEGO_1ST		0x02

/* Request */
#define CCID_REQ_ABORT			0x01
#define CCID_REQ_GET_CLOCK_FREQS	0x02
#define CCID_REQ_GET_DATA_RATES		0x03

/*=========================================================================
 * msg queueing
 *=======================================================================*/
boolean __ccid_async_out(void);
boolean __ccid_async_in(void);
boolean ccid_async_out(ccid_cmpl_cb cb, void *data);
void ccid_async_in(void);
boolean ccid_sync(void);

/*=========================================================================
 * sync commands
 *=======================================================================*/
boolean ccid_sync_GetSlotStatus(void);
boolean ccid_sync_IccPowerOn(uint8_t bClass);
boolean ccid_sync_IccPowerOff(void);
boolean ccid_sync_SetParameters(uint8_t bProtocolNum,
				void *abProtocolDataStructure);
boolean ccid_sync_GetParameters(uint8_t bProtocolNum);
boolean ccid_sync_ResetParameters(void);
boolean ccid_sync_XfrBlock(uint8_t bBWI, uint16_t wLevelParameter,
			   uint8_t *abData, uint32_t dwLength);
boolean ccid_sync_Secure(uint8_t bBWI, uint16_t wLevelParameter,
			 uint8_t bPINOperation, void *abPINDataStructure,
			 uint8_t *abData, uint32_t dwLength);
boolean ccid_sync_IccClock(uint8_t bClockCommand);
boolean ccid_sync_Mechanical(uint8_t bFunction);
boolean ccid_sync_SetDataAndFreq(uint32_t dwClockFrequency,
				 uint32_t dwDataRate);

/*=========================================================================
 * async commands
 *=======================================================================*/
uint8_t ccid_async_GetSlotStatus(ccid_cmpl_cb cb, void *data);
boolean ccid_async_Abort(uint8_t bSeq, ccid_cmpl_cb cb, void *data);

/*=========================================================================
 * sync/async responses
 *=======================================================================*/
boolean ccid_resp_sanity(void);
boolean ccid_resp_fail(void);
uint8_t ccid_resp_error(void);
uint8_t ccid_resp_status(void);
uint32_t ccid_resp_length(void);
int ccid_resp_block(uint8_t *abData, uint32_t dwLength);
uint8_t ccid_resp_param(void *abProtocolDataStructure);

/*=========================================================================
 * endpoint transfers
 *=======================================================================*/
int ccid_bulk_out(int length);
int ccid_bulk_in(int length);
int ccid_ctrl_dev(uint8_t dir,
		  uint8_t bRequest, uint16_t wValue,
		  uint8_t *data, uint16_t wLength);
int ccid_ctrl_cls(uint8_t dir,
		  uint8_t bRequest, uint16_t wValue,
		  uint8_t *data, uint16_t wLength);
int ccid_intr_in(uint8_t *data, int len);

uint8_t ccid_seq_alloc(void);
void ccid_set_timeout(int timeout);
void ccid_reset_timeout(void);

/*=========================================================================
 * misc
 *=======================================================================*/
void ccid_get_descriptor(scd_desc_t *desc);
void ccid_get_device(struct libusb_device_descriptor *desc);
void ccid_get_config(struct libusb_config_descriptor *desc);
void ccid_get_interface(struct libusb_interface_descriptor *desc);
boolean ccid_probe_interface(void);

#define CCID_SLOT_NO		0

int ccid_init(void);
void ccid_exit(void);

#endif /* __CCID_H_INCLUD__ */
