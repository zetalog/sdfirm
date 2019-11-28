#ifndef __COS_H_INCLUDE__
#define __COS_H_INCLUDE__

#include <target/generic.h>
#include <target/scs_slot.h>
#include <target/usb.h>

typedef uint16_t cos_sw_t;
typedef uint8_t icc_event_t;
typedef uint16_t fid_t;
typedef uint16_t cos_fid_t;

#include <driver/cos.h>

struct cos_xfr {
	uint8_t rx;
	uint8_t tx;
};

struct cos_apdu_hdr {
	uint8_t cla;
	uint8_t ins;
	uint8_t p1, p2;
	uint8_t p3;
#define COS_APDU_LC	p3
#define COS_APDU_LE	p3
	uint8_t *hdr_data;
};

struct icc_proto {
	uint8_t id;
	uint8_t hdr;
	scs_tvoid_cb xfr_in;
	scs_tvoid_cb xfr_out;
	scs_tvoid_cb xfr_cmpl;
	scs_tvoid_cb ne_expire;
};
__TEXT_TYPE__(struct icc_proto, icc_proto_t);

#ifndef CONFIG_COS_MAX_CHANS
#define NR_MAX_CHANS			1
#else
#define NR_MAX_CHANS			CONFIG_COS_MAX_CHANS
#endif
#define INVALID_ICC_CHAN		NR_MAX_CHANS

#ifdef CONFIG_ICC_T0
#define NR_ICC_PROTOS			1
#endif

/* ICC event */
#define ICC_EVENT_HANDLE_CMD		0x01	/* complete command receiving */
#define ICC_EVENT_HANDLE_DATA_IN	0x02	/* have data to send  */
#define ICC_EVENT_HANDLE_RESP		0x03	/* have data to send  */
#define ICC_EVENT_HANDLE_ATR		0x04

/* CLA byte */
#define ICC_CLA_PROP_MASK		0x80
#define ICC_CLA_INDS_FIRST_MASK		0xE0
#define ICC_CLA_BIT_MASK(x)		(1 << (x-1))	/* bit start from 1 */		

/* INS byte */
#define	COS_INS_DEACTIVE_FILE		0x04
#define	COS_INS_ERASE_RECORDS		0x0C
#define	COS_INS_ERASE_BINARY0		0x0E
#define	COS_INS_ERASE_BINARY1		0x0F
#define	COS_INS_VERIFY1			0x20
#define	COS_INS_VERIFY2			0x21
#define	COS_INS_ACTIVE_FILE		0x44
#define	COS_INS_MANAGE_CHANNEL		0x70
#define	COS_INS_EXTERNAL_AUTH		0x82
#define	COS_INS_GET_CHALLENGE		0x84
#define	COS_INS_INTERNAL_AUTH		0x88
#define	COS_INS_SEARCH_BINARY0		0xA0
#define	COS_INS_SEARCH_BINARY1		0xA1
#define	COS_INS_SEARCH_RECORD		0xA2
#define	COS_INS_SELECT			0xA4
#define	COS_INS_READ_BINARY0		0xB0
#define	COS_INS_READ_BINARY1		0xB1
#define	COS_INS_READ_RECORD0		0xB2
#define	COS_INS_READ_RECORD1		0xB3
#define	COS_INS_GET_RESPONSE		0xC0
#define	COS_INS_ENVELOPE0		0xC2
#define	COS_INS_ENVELOPE1		0xC3	/* may chain */
#define	COS_INS_GET_DATA0		0xCA
#define	COS_INS_GET_DATA1		0xCB
#define	COS_INS_WRITE_BINARY0		0xD0
#define	COS_INS_WRITE_BINARY1		0xD1
#define	COS_INS_WRITE_RECORD		0xD2
#define	COS_INS_UPDATE_BINARY0		0xD6
#define	COS_INS_UPDATE_BINARY1		0xD7
#define	COS_INS_PUT_DATA		0xDA
#define	COS_INS_UPDATE_RECORD0		0xDC
#define	COS_INS_UPDATE_RECORD1		0xDD
#define	COS_INS_CREATE_FILE		0xE0
#define	COS_INS_APPEND_RECORD		0xE2

/* ISO7816-4 command-response (short APDU) */
#define COS_APDU_HDR_SIZE	4	/* CLS.INS.P1.P2 */
#define COS_APDU_SIZE		COS_APDU_HDR_SIZE+1+255+1

#define COS_DEF_CID	0
#define COS_CHAN_STATE_OPEND	0x00
#define COS_CHAN_STATE_CLOSED	0x01

#define COS_DEV_STATE_PRESENT		0x00

#define COS_SW_NORMAL			0x9000		/* Normal processing */

#define COS_SW_ERROR_69			0x6900		/* == Checking Error == */
#define COS_SW_CMD_INCOMP		0x6981		/* Command incompatible with file structure */
#define COS_SW_SECU_NOT_SATISFY		0x6982		/* Security status not satisfied */
#define COS_SW_ERROR_6A			0x6A00		/* == Checking Error == */
#define COS_SW_PARAM_ERR		0x6A80		/* Incorrect parameters in the command data field */
#define COS_SW_FILE_NOT_FOUND		0x6A82		/* File or application not found */
#define COS_SW_UNSUPP_INS		0x6D00		/* INS not valid */

/* Selection Method */
#define ICC_SEL_TYPE_DF_NAME	0x01
#define ICC_SEL_TYPE_FID	0x02
#define ICC_SEL_TYPE_PATH	0x03
#define ICC_SEL_TYPE_SHORT_EF	0x04

#define COS_LCS_CREATION	0x01
#define COS_LCS_INITIAL		0x03
#define COS_LCS_OPER_ACT	0x05	/* FIXME: value */
#define COS_LCS_OPER_DEACT	0x04	/* FIXME: value */
#define COS_LCS_TERMINATION	0x0C	/* FIXME: value */

#define ICC_FILE_TYPE_DF	0x01
#define ICC_FILE_TYPE_EF	0x02
#define ICC_FILE_TYPE_MASK	0x03
#define ICC_FILE_DF_ROOT	(ICC_FILE_TYPE_DF | 0x10)	/* MF */
#define ICC_FILE_DF_APP		(ICC_FILE_TYPE_DF | 0x20)
#define ICC_FILE_EF_INTERNAL	(ICC_FILE_TYPE_EF | 0x40)
#define ICC_FILE_EF_WORKING	(ICC_FILE_TYPE_EF | 0x80)

#define COS_DATA_TYPE_UNIT	0x01
#define COS_DATA_TYPE_RECORD	0x02
#define COS_DATA_TYPE_OBJECT	0x04
#define COS_DATA_TYPE_MASK	0x07

struct cos_btlv_tag {
	uint8_t first;
	uint8_t flen;
	uint16_t tag;
};

struct cos_btlv_length {
	uint8_t flen;
	uint32_t length;
};

struct cos_btlv_value {
	uint8_t *value;
};

struct cos_object {
	uint8_t type;
#define COS_OBJ_TYPE_CMD	0x01
#define COS_OBJ_TYPE_FILE	0x02
#define COS_OBJ_TYPE_OBJ	0x03
#define COS_OBJ_TYPE_TABLE	0x04
};

/* security architecture */
union cos_secu_status {
	uint8_t global;
	uint8_t app_spec;
	uint8_t cmd_spec;
	uint8_t file_spec;
};

/* Logical channel */
struct cos_chan {
#if NR_MAX_CHANS > 1
	boolean opened;
	boolean actived;
	uint8_t id;
#endif	
	cos_fid_t pfid;	/* previous */
	cos_fid_t cfid;	/* current */
	union cos_secu_status status;
};

#if NR_MAX_CHANS > 1
uint8_t cos_cid;
#else
#define cos_cid			COS_DEF_CID
#endif

struct cos_sae {
	uint8_t dummy;
};
struct cos_sac {
	uint8_t dummy;
};

struct cos_file_ctrl {
	/* life cycle status byte */
	uint8_t lcs;
	/* file descriptor byte */
	uint8_t fdb;
	/* serucity attribute compact */
	uint8_t saclen;
	struct cos_sac sac;
	/* serucity attribute expaned */
	uint8_t saelen;
	struct cos_sae sae;
	/* security environment ID */
	uint8_t seid;
};

struct cos_file {
	uint8_t ftype;
	uint8_t dtype;

	fid_t fid;
	fid_t parent;
	/* file ctrl info */
	struct cos_file_ctrl	fci;
};

struct cos_tlv {
	uint8_t tlv_type;
#define COS_TLV_TYPE_SIMPLE	0x01
#define COS_TLV_TYPE_BER	0x02
};

struct cos_df {
	struct cos_file comm;
	struct cos_tlv tlv;
};

struct cos_ef {
	struct cos_file comm;
	uint8_t sfid;
	uint16_t size;
	union {
		struct cos_tlv tlv;
		uint16_t unit_size;
		uint16_t record_num;
	};
};

struct cos_record {
	uint16_t size;
	uint8_t org;
#define COS_RECORD_ORG_LINEAR	0x01
#define COS_RECORD_ORG_CYCLIC	0x02
};

struct cos_secu_mech {
	union {
		uint8_t auth_pass;
		uint8_t auth_key;
		uint8_t data_auth;
		uint8_t data_enc;
	};
};

struct cos_secu_attr {
	uint8_t dummy;
};

struct cos_secu {
	union cos_secu_status status;
	struct cos_secu_attr   attr;
	struct cos_secu_mech   mech;

};

scs_err_t cos_power_on(void);
scs_err_t cos_power_off(void);
boolean cos_activated(void);

scs_err_t cos_xchg_block(scs_size_t nc, scs_size_t ne);
scs_size_t cos_xchg_avail(void);
scs_err_t cos_xchg_write(scs_off_t index, uint8_t byte);
uint8_t cos_xchg_read(scs_off_t index);
void cos_xchg_reset(scs_size_t tx);

void cos_write_resp(uint8_t byte);

uint8_t cos_get_status(void);
scs_err_t cos_get_error(void);
void cos_set_error(scs_err_t err);
void cos_register_completion(scs_cmpl_cb compl);

void cos_cid_restore(uint8_t cid);
#define cos_cid_select(cid)		cos_cid_restore(cid)
uint8_t cos_cid_save(uint8_t cid);

#endif /* __COS_H_INCLUDE__ */
