#ifndef __USB_SCD_H_INCLUDE__
#define __USB_SCD_H_INCLUDE__

#include <target/config.h>
#include <target/generic.h>
#include <target/usb.h>
#include <target/scs.h>

#ifdef CONFIG_SCD_DEBUG
#define scd_debug(tag, val)		dbg_print((tag), (val))
#else
#define scd_debug(tag, val)
#endif

#define USB_INTERFACE_CLASS_SCD		11
#define USB_INTERFACE_PROTOCOL_SCD	0x00

#ifdef CONFIG_SCD_BULK
# ifdef CONFIG_SCD_INTERRUPT
#  define NR_SCD_ENDPS			3
# else
#  define NR_SCD_ENDPS			2
# endif
#else
# ifdef CONFIG_SCD_INTERRUPT
#  define NR_SCD_ENDPS			1
# else
#  define NR_SCD_ENDPS			0
# endif
#endif

/* SCD device ID */
#define SCD_FUNC_BULK	0x00000001
#define SCD_FUNC_IRQ	0x00000002
#define SCD_FUNC_T1	0x00000004
#define SCD_FUNC_APDU	0x00000008
#define SCD_FUNC_SECURE	0x00000010
#define SCD_FUNC_ABORT	0x00000020

/* Smart Card Device Class */
typedef struct scd_desc {
	uint8_t	 bLength;
	uint8_t	 bDescriptorType;
	uint16_t bcdCCID;
	uint8_t	 bMaxSlotIndex;
	uint8_t	 bVoltageSupport;
#define SCD_VOLTAGE_5V			0x01
#define SCD_VOLTAGE_3V			0x02
#define SCD_VOLTAGE_1_8V		0x04
#define SCD_VOLTAGE_ALL			(SCD_VOLTAGE_5V | \
					 SCD_VOLTAGE_3V | \
					 SCD_VOLTAGE_1_8V)
#define SCD_VOLTAGE_AUTO		0x00

	uint32_t dwProtocols;
	uint32_t dwDefaultClock;
	uint32_t dwMaximumClock;
	uint8_t	 bNumClockSupported;
#define ICCD_FIXED_CLOCK		0x0DFC

	uint32_t dwDataRate;
	uint32_t dwMaxDataRate;
	uint8_t	 bNumDataRatesSupported;
#define ICCD_FIXED_DATARATE		0x2580

	uint32_t dwMaxIFSD;
#define ICCD_FIXED_MAX_IFSD		0xFE

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
#define SCD_DT_SCD			(USB_TYPE_CLASS | 0x01)
#define SCD_DT_SCD_SIZE			0x36

typedef uint8_t					scd_qid_t;

#ifdef CONFIG_SCD_CCID
#include <target/scd_ccid.h>
#endif
#ifdef CONFIG_SCD_ICCD
#include <target/scd_iccd.h>
#endif

/* functions should be implemented by SCD generic */
#define scd_slot_success(err)			((err) == SCS_ERR_SUCCESS)
#define scd_slot_progress(err)			((err) == SCS_ERR_PROGRESS)

/* functions should be implemented by SCD protocol */
uint8_t scd_slot_status(void);
uint8_t scd_slot_error(scs_err_t err);
void scd_sid_select(scd_sid_t sid);

#define INVALID_SCD_QID			NR_SCD_QUEUES
#define INVALID_SCD_SID			NR_SCD_SLOTS

#if NR_SCD_QUEUES > 1
extern __near__ scd_qid_t scd_qid;
void scd_qid_restore(scd_qid_t qid);
scd_qid_t scd_qid_save(scd_qid_t qid);
#else
#define scd_qid				0
#define scd_qid_restore(qid)
#define scd_qid_save(qid)		(qid)
#endif
#define scd_qid_select(qid)		scd_qid_restore(qid)

#ifdef CONFIG_SCD_BULK
#define SCD_ENDP_BULK_IN		0x00
#define SCD_ENDP_BULK_OUT		0x01

#define SCD_SLOT_STATUS_ACTIVE			0x00
#define SCD_SLOT_STATUS_INACTIVE		0x01
#define SCD_SLOT_STATUS_NOTPRESENT		0x02

#define SCD_ERROR_ICC_MUTE			0xFE
#define SCD_ERROR_XFR_OVERRUN			0xFC
#define SCD_ERROR_HW_ERROR			0xFB
#define SCD_ERROR_USER_DEFINED			0xC0
#define SCD_ERROR_USER(e)			(SCD_ERROR_USER_DEFINED-e)
#define SCD_ERROR_RESERVED			0x80
#define SCD_ERROR_CMD_UNSUPPORT			0x00

#define SCD_HEADER_SIZE				10

#define SCD_CMD_STATUS_SUCC			(0x00 << 6)
#define SCD_CMD_STATUS_FAIL			(0x01 << 6)
#define SCD_CMD_STATUS_TIME_EXT			(0x02 << 6)
#define SCD_CMD_STATUS_MASK			(0xc0)

struct scd_cmd {
	uint8_t  bMessageType;
	scs_size_t dwLength;
	uint8_t  bSlot;
	uint8_t  bSeq;
	uint8_t  abRFU[3];
};

struct scd_resp {
	uint8_t  bMessageType;
	scs_size_t dwLength;
	/* Slot Status Register */
	uint8_t bStatus;
	/* Slot Error Register */
	uint8_t bError;
	uint8_t abRFU3;
};

#define SCD_PC2RDR_ICCPOWERON			0x62
#define SCD_PC2RDR_ICCPOWEROFF			0x63
#define SCD_PC2RDR_GETSLOTSTATUS		0x65
#define SCD_PC2RDR_ESCAPE			0x6B
#define SCD_PC2RDR_GETPARAMETERS		0x6C
#define SCD_PC2RDR_XFRBLOCK			0x6F

#define SCD_RDR2PC_DATABLOCK			0x80
#define SCD_RDR2PC_SLOTSTATUS			0x81
#define SCD_RDR2PC_ESCAPE			0x83

/* SCD_PC2RDR_XFRBLOCK parameters */
struct scd_xb_param {
	scs_err_t dwIccOutErr;
	urb_size_t dwIccOutCnt;
	urb_size_t dwIccExpCnt;
	uint8_t bIccWaitInt;
};

/* SCD_RDR2PC_DATABLOCK parameters */
struct scd_db_param {
	urb_size_t dwIccOutIter;
	scs_err_t dwIccOutErr;
};

struct scd_t0_param {
	uint8_t bmFindexDindex;
	uint8_t bmTCCKST0;
	uint8_t bGuardTimeT0;
	uint8_t bWaitingIntegerT0;
	uint8_t bClockStop;
};

struct scd_t1_param {
	uint8_t bmFindexDindex;
	uint8_t bmTCCKST1;
	uint8_t bGuardTimeT1;
	uint8_t bWaitingIntegerT1;
	uint8_t bClockStop;
	uint8_t bIFSC;
	uint8_t bNadValue;
};

#define SCD_SLOT_STATE_PC2RDR		0x00
#define SCD_SLOT_STATE_RDR2PC		0x01
#define SCD_SLOT_STATE_SANITY		0x02
#define SCD_SLOT_STATE_RUNNING		0x03
#define SCD_SLOT_STATE_WAITING		0x04

/* XXX: Temporary Storage for SCD Stack
 * This structure holds temporary storages, which should be allocated in
 * heap.  In a system without heap, it is perferred to be united objects.
 */
typedef union scd_data {
	struct scd_db_param db;
	struct scd_xb_param xb;
#define SCD_XB_ERR			scd_cmd_data.xb.dwIccOutErr
#define SCD_XB_NC			scd_cmd_data.xb.dwIccOutCnt
#define SCD_XB_NE			scd_cmd_data.xb.dwIccExpCnt
#define SCD_XB_WI			scd_cmd_data.xb.bIccWaitInt
	struct scd_t0_param t0;
	struct scd_t1_param t1;
#ifdef CONFIG_SCD_CCID
	struct ccid_fd_param fd;
#endif
#ifdef CONFIG_CCID_SECURE
	struct ccid_po_param po;
	struct ccid_pv_param pv;
	struct ccid_pm_param pm;
#endif
} scd_data_t;

extern __near__ uint8_t scd_states[NR_SCD_QUEUES];
extern __near__ struct scd_cmd scd_cmds[NR_SCD_QUEUES];
extern __near__ struct scd_resp scd_resps[NR_SCD_QUEUES];
extern scd_data_t scd_cmd_data;

#if NR_SCD_QUEUES != NR_SCD_SLOTS
boolean scd_abort_requested(void);
boolean scd_abort_completed(void);
boolean scd_abort_responded(void);
#else
#define scd_abort_requested()		(false)
#define scd_abort_completed()		(false)
#define scd_abort_responded()		(false)
#endif

void scd_Escape_init(void);
void scd_Escape_out(void);
void scd_Escape_cmp(void);
void scd_Escape_in(void);

boolean scd_is_cmd_status(uint8_t status);
void __scd_queue_reset(scd_qid_t qid);

#define __scd_submit_response(addr, qid)				\
	do {								\
		if (scd_states[qid] == SCD_SLOT_STATE_RDR2PC) {		\
			usbd_request_submit((addr),			\
					    SCD_HEADER_SIZE +		\
					    scd_resps[qid].dwLength);	\
		}							\
	} while (0)
#define __scd_submit_command(addr, qid)					\
	do {								\
		if (scd_states[qid] == SCD_SLOT_STATE_PC2RDR) {		\
			usbd_request_submit((addr), SCD_HEADER_SIZE);	\
		}							\
	} while (0)
void __scd_handle_command(scd_qid_t qid);
void __scd_complete_command(scd_qid_t qid);
void __scd_complete_response(scd_qid_t qid);
void __scd_handle_response(scd_qid_t qid);

void __scd_XfrBlock_out(scs_size_t hdr_size, scs_size_t blk_size);
void __scd_CmdSuccess_out(void);
void __scd_CmdFailure_out(uint8_t error, uint8_t status);
void scd_CmdHeader_out(void);
void scd_XfrBlock_out(void);
void scd_SlotStatus_out(void);
void scd_DataBlock_out(void);
void scd_IccPowerOn_out(void);
#define scd_XfrBlock_out()		(__scd_XfrBlock_out(SCD_HEADER_SIZE, scd_cmds[scd_qid].dwLength))
#define scd_CmdFailure_out(error)	(__scd_CmdFailure_out(error, scd_slot_status()))
#define scd_CmdResponse_cmp()		(scd_slot_enter(SCD_SLOT_STATE_RDR2PC))
void scd_SlotNotExist_cmp(void);
void scd_CmdOffset_cmp(uint8_t offset);
void scd_SlotStatus_cmp(void);
#define scd_DataBlock_cmp(err)		(scd_ScsSequence_cmp(err, true))
void scd_XfrBlock_cmp(void);
void scd_IccPowerOn_cmp(void);
void scd_IccPowerOff_cmp(void);
void scd_ScsSequence_cmp(scs_err_t err, boolean block);

void scd_RespHeader_in(scs_size_t length);
#define scd_SlotStatus_in()		(scd_RespHeader_in(0))
void scd_DataBlock_in(void);

/* protocol drivers */
uint8_t scd_resp_message(void);
void scd_slot_enter(uint8_t state);
void scd_queue_reset(scd_qid_t qid);

/* bulk protocol entrance */
void scd_submit_command(void);
void scd_handle_command(void);
void scd_complete_command(void);
void scd_submit_response(void);
void scd_complete_response(void);
void scd_handle_response(void);

/* bulk protocol hooks */
void scd_handle_bulk_pc2rdr(void);
void scd_complete_bulk_pc2rdr(void);
void scd_handle_bulk_rdr2pc(void);

/* default handlers called by bulk protocol hooks */
#define scd_handle_pc2rdr_default()
#define scd_complete_pc2rdr_default()	(scd_CmdOffset_cmp(0))
#define scd_handle_rdr2pc_default()	BUG()

extern usbd_endpoint_t scd_endpoint_out;
extern usbd_endpoint_t scd_endpoint_in;

void scd_bulk_init(void);
#define scd_bulk_register(out, in)					\
	do {								\
		(in) = usbd_claim_endpoint(true, &scd_endpoint_in);	\
		(out) = usbd_claim_endpoint(true, &scd_endpoint_out);	\
	} while (0)
#define scd_get_bulk_desc(out, in)					\
	do {								\
		usbd_input_endpoint_desc(out);				\
		usbd_input_endpoint_desc(in);				\
	} while (0)
#else
#define SCD_HEADER_SIZE				0
#define scd_bulk_init()
#define scd_bulk_register(out, in)
#define scd_get_bulk_desc(out, in)
#endif

#ifdef CONFIG_SCD_INTERRUPT
#define SCD_ENDP_INTR_IN		(NR_SCD_ENDPS-1)
#define SCD_ENDP_INTERVAL_INTERRUPT	0xFF

#define SCD_RDR2PC_NOTIFYSLOTCHANGE	0x50

/* Implemented by drivers */
void scd_handle_present(void);
void scd_discard_present(void);
void scd_submit_present(void);
boolean scd_present_changed(void);
void scd_submit_interrupt(void);
void scd_handle_interrupt(void);
void scd_discard_interrupt(void);
void scd_irq_init(void);

/* Called by drivers */
void __scd_handle_present_sid(scd_sid_t sid);
void __scd_discard_present_sid(scd_sid_t sid);
void __scd_submit_present_sid(scd_sid_t sid);
boolean __scd_present_changed_sid(scd_sid_t sid);
boolean __scd_present_changed_all(void);
void __scd_handle_present_all(void);

void __scd_submit_interrupt(uint8_t addr);

extern usbd_endpoint_t scd_endpoint_irq;

#define scd_irq_register(irq)						\
	do {								\
		irq = usbd_claim_endpoint(true, &scd_endpoint_irq);	\
	} while (0)
#define scd_get_irq_desc(irq)						\
	do {								\
		usbd_input_endpoint_desc(irq);				\
	} while (0)
#else
#define scd_irq_register(irq)
#define scd_get_irq_desc(irq)
#define scd_irq_init()
#endif

void scd_ctrl_get_desc(void);
void scd_handle_ctrl_class(void);
uint8_t scd_proto_features(void);
extern usbd_interface_t usb_scd_interface;
void scd_devid_init(void);

#endif /* __USB_SCD_H_INCLUDE__ */
