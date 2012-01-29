/*
 * ZETALOG's Personal COPYRIGHT
 *
 * Copyright (c) 2009
 *    ZETALOG - "Lv ZHENG".  All rights reserved.
 *    Author: Lv "Zetalog" Zheng
 *    Internet: zetalog@gmail.com
 *
 * This COPYRIGHT used to protect Personal Intelligence Rights.
 * Redistribution and use in source and binary forms with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *    This product includes software developed by the Lv "Zetalog" ZHENG.
 * 3. Neither the name of this software nor the names of its developers may
 *    be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 * 4. Permission of redistribution and/or reuse of souce code partially only
 *    granted to the developer(s) in the companies ZETALOG worked.
 * 5. Any modification of this software should be published to ZETALOG unless
 *    the above copyright notice is no longer declaimed.
 *
 * THIS SOFTWARE IS PROVIDED BY THE ZETALOG AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE ZETALOG OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * @(#)usb_ccid.c: circuit chip interface device functions
 * $Id: usb_ccid.c,v 1.135 2011-10-26 10:09:44 zhenglv Exp $
 */

/* TODO:
 *   1. PC2RDR_Escape support
 *   2. SPE timeout validation support
 *   3. multiple clock frequencies support
 */
#include <target/usb_scd.h>
#include <target/lcd.h>
#include <target/kbd.h>

#define CCID_XB_ERR	ccid_cmd_data.xb.dwIccOutErr
#define CCID_XB_NC	ccid_cmd_data.xb.dwIccOutCnt
#define CCID_XB_NE	ccid_cmd_data.xb.dwIccExpCnt
#define CCID_XB_WI	ccid_cmd_data.xb.bIccWaitInt

/* DataRate and ClockFreq */
struct ccid_fd_param {
	uint32_t dwClockFrequency;
	uint32_t dwDataRate;
};

#include "ccid_spe.h"

typedef union ccid_data {
	struct scd_t0_param t0;
	struct scd_t1_param t1;
	struct scd_db_param db;
	struct scd_xb_param xb;
#ifdef CONFIG_USB_CCID
	struct ccid_fd_param fd;
#endif
#ifdef CONFIG_CCID_SECURE
	struct ccid_po_param po;
	struct ccid_pv_param pv;
	struct ccid_pm_param pm;
#endif
} ccid_data_t;

#define CCID_ENDP_INTERVAL_INTR		0x7F
#define CCID_ENDP_INTERVAL_IN		0x01
#define CCID_ENDP_INTERVAL_OUT		0x01
#define CCID_XCHG_IS_LEVEL(xchg)	\
	((ccid_device_features() & SCD_FEATURE_XCHG_MASK) == xchg)

#define CCID_ADDR_IN			ccid_addr[SCD_ENDP_BULK_IN]
#define CCID_ADDR_OUT			ccid_addr[SCD_ENDP_BULK_OUT]
#define CCID_ADDR_IRQ			ccid_addr[SCD_ENDP_INTR_IN]

#define CCID_QID_OUT			(NR_SCD_QUEUES-1)
#define CCID_QID_IN			ccid_seq_queue[0]

static void ccid_slot_reset(ccid_qid_t qid);
static void ccid_slot_abort(uint8_t type, ifd_sid_t sid, uint8_t seq);
static void ccid_submit_response(void);
static void ccid_submit_command(void);
static void ccid_handle_iso7816_cmpl(void);

/* Multiple CCID Slot support
 *
 *                      +----------+        +----------+   |  slot
 *                      | Bulk OUT |        | Bulk IN  |   | states
 *                      +----------+        +----+-----+   |
 *                           \|/                /|\        +=========+
 *  +--------------+/ +-------+--------+         |         |         |
 *  |   reqs[sid]  +--|  reqs[OUT_QID] |         |         |         |
 *  +--------------+\ +----------------+         |         |         |
 *        \|/                \|/                 |         | PC2RDR  |
 *  +------+-------+  +-------+--------+         |         |         |
 *  | iso7816 call |  | resps[OUT_QID] |         |         |         |
 *  +--------------+  +----------------+         |         |         |
 *        \|/                 |                  |         +=========+
 *  +------+-------+          |                  |         |         |
 *  | iso7816 cmpl |          |                  |         |         |
 *  +--------------+          |                  |         |         |
 *        \|/                 |                  |         | ISO7816 |
 *  +------+-------+          |                  |         |         |
 *  |  resps[sid]  |          |                  |         |         |
 *  +--------------+          |                  |         |         |
 *         |                  |                  |         +=========+
 *         |                  |          +---------------+ |         |
 *         |                  |          | resps[IN_QID] | | RDR2PC  |
 *         |                  |          +-------+-------+ |         |
 *         |                  |                 /|\        +=========+
 *      enqueue            enqueue            dequeue      |
 *        \|/                \|/                 |         |
 *  +------+------------------+--------------------------+ |
 *  |                      seq_queue                     | |
 *  +----------------------------------------------------+ |
 *
 * ccid_qid_t 0 ~ NR_SCD_QUEUES (0 ~ NR_SCD_SLOTS+1)
 * ifd_sid_t  0 ~ NR_SCD_SLOTS
 * ccid_seq_t 0 ~ NR_SCD_QUEUES (seq_queue index)
 * seq_queue: USB request queue, only queue[0] is sensitive for BULK IN
 */
/*
 * CCID state machine is implemented by 4 hooks:
 *
 * SETUP
 * poll
 * 	(OUT) ccid_submit_command
 * 	  usbd_request_submit(OUT) (OUT_QID is PC2RDR)
 * 	    -> DATA
 * 
 * 	(IN) ccid_submit_response
 * 	  usbd_request_submit(IN) (nr_seqs > 0 && IN_QID is RDR2PC)
 * 	    -> DATA
 * 
 * DATA
 * iocb
 * 	(OUT) usbd_request_commit
 * 
 * STATUS
 * done
 * 	(OUT) usbd_request_handled
 * 	(OUT) scd_slot_enter(ISO7816)
 * 	  icc/scs_xxx
 * 	(OUT) scd_slot_enter(RDR2PC)
 * 	  ccid_enqueue
 * 	    ccid_submit_response
 * 	      usbd_request_submit(IN) (is IN_QID)
 * 	        -> DATA
 * 
 * 	(IN) scd_slot_enter(PC2RDR)
 * 	  ccid_dequeue
 * 	    ccid_submit_command
 * 	      usbd_request_submit(OUT) (is OUT_QID)
 * 	        -> DATA
 * 	    ccid_submit_response
 * 	      usbd_request_submit(IN) (is IN_QID)
 * 	        -> DATA
 * 
 * 7816_cmpl
 * 	scd_slot_enter(RDR2PC)
 * 	  ccid_enqueue
 * 	    ccid_submit_response
 * 	      usbd_request_submit(IN) (is IN_QID)
 * 	        -> DATA
 */
__near__ struct ccid_slot ccid_slots[NR_SCD_QUEUES];
__near__ ifd_sid_t ccid_seq_queue[NR_SCD_QUEUES];
__near__ ccid_seq_t ccid_nr_seqs = 0;

__near__ uint8_t ccid_addr[NR_SCD_ENDPS];

ccid_data_t ccid_cmd_data;
__near__ struct ccid_hwerr ccid_intr_hwerrs[NR_SCD_SLOTS];

DECLARE_BITMAP(ccid_running_intrs, NR_SCD_SLOTS+NR_SCD_SLOTS);
DECLARE_BITMAP(ccid_pending_intrs, NR_SCD_SLOTS+NR_SCD_SLOTS);

__near__ ccid_qid_t scd_qid = INVALID_SCD_QID;

#define CCID_STRING_FIRST	0x10
#define CCID_STRING_INTERFACE	CCID_STRING_FIRST+0
#define CCID_STRING_LAST	CCID_STRING_INTERFACE

/*=========================================================================
 * ISO7816 translator
 *=======================================================================*/
static uint32_t ccid_device_features(void)
{
	uint32_t features = SCD_FEATURE_NONE;

#ifdef CONFIG_IFD_AUTO_CLASS_SELECT
	features |= SCD_FEATURE_AUTO_CLASS;
#endif
#ifdef CONFIG_IFD_AUTO_RESET
	features |= SCD_FEATURE_AUTO_ACTIVATE;
#endif
#ifdef CONFIG_IFD_AUTO_INFO_XCHG
	features |= SCD_FEATURE_AUTO_CONFIG;

#ifdef CONFIG_IFD_AUTO_PPS_PROP
	features |= SCD_FEATURE_AUTO_FREQ;
	features |= SCD_FEATURE_AUTO_DATA;
	features |= SCD_FEATURE_AUTO_PPS_PROP;
#else
	/* PPS algorithm choice */
	features |= SCD_FEATURE_AUTO_PPS_BASE;
#endif
	/* XXX: No Prop support
	 * SCD_FEATURE_AUTO_PPS_PROP is hard to implement in resource
	 * limited devices.
	 * We won't support this feature currently.
	 */
#endif
#ifdef CONFIG_IFD_CLOCK_CONTROL
	features |= SCD_FEATURE_CAP_CLOCK_STOP;
#endif
#ifdef CONFIG_IFD_T1
#ifdef CONFIG_IFD_T1_NAD
	features |= SCD_FEATURE_CAP_ACCEPT_NAD;
#endif
#ifndef CONFIG_IFD_XCHG_TPDU
	/* TPDU exchange requires ifsd negotiation from host */
	features |= SCD_FEATURE_AUTO_IFSD;
#endif
#endif
#ifdef CONFIG_IFD_XCHG_TPDU
	features |= SCD_FEATURE_XCHG_TPDU;
#endif
#ifdef CONFIG_IFD_XCHG_APDU
	features |= SCD_FEATURE_XCHG_APDU;
#endif
	/* SCD_FEATURE_WAKEUP_ICC */
	return features;
}

uint8_t scd_resp_message(void)
{
	switch (scd_cmds[scd_qid].bMessageType) {
	case SCD_PC2RDR_ICCPOWERON:
	case SCD_PC2RDR_XFRBLOCK:
	case CCID_PC2RDR_SECURE:
		return SCD_RDR2PC_DATABLOCK;
	case SCD_PC2RDR_ICCPOWEROFF:
	case CCID_PC2RDR_GETSLOTSTATUS:
	case CCID_PC2RDR_T0APDU:
	case CCID_PC2RDR_ICCCLOCK:
	case CCID_PC2RDR_MECHANICAL:
	case CCID_PC2RDR_ABORT:
		return SCD_RDR2PC_SLOTSTATUS;
	case SCD_PC2RDR_ESCAPE:
		return SCD_RDR2PC_ESCAPE;
#ifdef CONFIG_IFD_AUTO_PPS_PROP
	case CCID_PC2RDR_SETPARAMETERS:
	case CCID_PC2RDR_GETPARAMETERS:
	case CCID_PC2RDR_RESETPARAMETERS:
	case CCID_PC2RDR_SETDATAANDFREQ:
		return SCD_RDR2PC_SLOTSTATUS;
#else
	case CCID_PC2RDR_SETPARAMETERS:
	case CCID_PC2RDR_GETPARAMETERS:
	case CCID_PC2RDR_RESETPARAMETERS:
		return CCID_RDR2PC_PARAMETERS;
	case CCID_PC2RDR_SETDATAANDFREQ:
		return CCID_RDR2PC_DATARATEANDCLOCK;
#endif
	}
	return SCD_RDR2PC_SLOTSTATUS;
}

static uint8_t ccid_proto_features(void)
{
	uint8_t protocols;
#ifdef CONFIG_SCD_ESC_PN53X
#else
	protocols = (1 << SCD_PROTOCOL_T0);
#ifdef CONFIG_IFD_T1
	protocols |= (1 << SCD_PROTOCOL_T1);
#endif
#endif
	return protocols;
}

static void ccid_qid_restore(ccid_qid_t qid)
{
	scd_qid = qid;
}

static ccid_qid_t ccid_qid_save(ccid_qid_t qid)
{
	ccid_qid_t sqid;

	sqid = scd_qid;
	ccid_qid_restore(qid);
	return sqid;
}

#define ccid_qid_select(qid)	ccid_qid_restore(qid)

static void ccid_sid_select(ifd_sid_t sid)
{
	ccid_qid_select(sid);
	BUG_ON(sid >= NR_SCD_SLOTS);
	ifd_sid_select(sid);
}

static uint8_t ccid_slot_error(scs_err_t err)
{
	switch (err) {
	case SCS_ERR_UNSUPPORT:
		return CCID_ERROR_CMD_UNSUPPORT;
	case SCS_ERR_OVERRUN:
		return CCID_ERROR_XFR_OVERRUN;
	case SCS_ERR_BUSY_SLOT:
		return CCID_ERROR_CMD_SLOT_BUSY;
	case SCS_ERR_BUSY_AUTO:
		return CCID_ERROR_BUSY_AUTO_SEQ;
	case SCS_ERR_TIMEOUT:
	case SCS_ERR_NOTPRESENT:
		return CCID_ERROR_ICC_MUTE;
	case IFD_ERR_BAD_TCK:
		return CCID_ERROR_BAD_ATR_TCK;
	case IFD_ERR_BAD_TS:
		return CCID_ERROR_BAD_ATR_TS;
	case IFD_ERR_BAD_PPSS:
	case IFD_ERR_BAD_PCK:
	case SCS_ERR_PARITY_ERR:
		return CCID_ERROR_XFR_PARITY_ERROR;
	case IFD_ERR_BAD_PROTO:
		return CCID_ERROR_PROTO_DEACTIVATE;
	case SCS_ERR_ABORTED:
		return CCID_ERROR_CMD_ABORTED;
	default:
		return CCID_ERROR_HW_ERROR;
	}
}

/*=========================================================================
 * LCD display
 *=======================================================================*/
#ifdef CONFIG_LCD
#ifdef CONFIG_CCID_MSG
#define CCID_DEFAULT_MSG	CONFIG_CCID_MSG
#else
#define CCID_DEFAULT_MSG	"CCID Ready"
#endif

__near__ boolean ccid_msg_custom = false;

void ccid_display_slot(void)
{
	ccid_qid_t qid = scd_qid;

	if (qid >= NR_SCD_SLOTS)
		qid = 0;
	lcd_draw_point((unsigned char)('0'+qid), MAX_LCD_COLS-1, 0);
}

void ccid_display_custom(text_char_t *msg)
{
	lcd_draw_row(msg, 0);
	ccid_display_slot();
	ccid_msg_custom = true;
}

void ccid_display_default(void)
{
	lcd_draw_row(CCID_DEFAULT_MSG, 0);
	lcd_draw_row("", 1);
	ccid_display_slot();
	ccid_msg_custom = false;
}
#else
void ccid_display_slot(void)
{
}

void ccid_display_default(void)
{
}

void ccid_display_custom(text_char_t *msg)
{
}
#endif

/*=========================================================================
 * CCID slots
 *=======================================================================*/
uint8_t scd_slot_status(void)
{
	switch (ifd_slot_get_state()) {
	case IFD_SLOT_STATE_NOTPRESENT:
		return SCD_SLOT_STATUS_NOTPRESENT;
	case IFD_SLOT_STATE_PRESENT:
	case IFD_SLOT_STATE_SELECTED:
	case IFD_SLOT_STATE_ACTIVATED:
	case IFD_SLOT_STATE_HWERROR:
		return SCD_SLOT_STATUS_INACTIVE;
	case IFD_SLOT_STATE_ATR_READY:
	case IFD_SLOT_STATE_PPS_READY:
		return SCD_SLOT_STATUS_ACTIVE;
	}
	return SCD_SLOT_STATUS_NOTPRESENT;
}

static void ccid_slot_reset(ccid_qid_t qid)
{
	ccid_slots[qid].aborting = CCID_ABORT_NONE;
	ccid_slots[qid].state = SCD_SLOT_STATE_PC2RDR;
	scd_resps[qid].bStatus &= ~SCD_CMD_STATUS_MASK;
}

static boolean ccid_is_cmd_status(uint8_t status)
{
	return ((scd_resps[scd_qid].bStatus & SCD_CMD_STATUS_MASK) == status);
}

static void ccid_slot_abort(uint8_t type, ifd_sid_t sid, uint8_t seq)
{
	ccid_qid_t sqid;
	boolean aborting = false;
	scs_err_t err = SCS_ERR_SUCCESS;

	sqid = ccid_qid_save(sid);

	switch (type) {
	case CCID_ABORT_CTRL:
		ccid_slots[sid].ctrl_seq = seq;
		scd_debug(SCD_DEBUG_ABORT, 0);
		break;
	case CCID_ABORT_BULK:
		ccid_slots[sid].bulk_seq = seq;
		scd_debug(SCD_DEBUG_ABORT, 1);
		break;
	}
	ccid_slots[sid].aborting |= type;

	if (ccid_slots[sid].aborting == (CCID_ABORT_CTRL | CCID_ABORT_BULK) &&
	    ccid_slots[sid].bulk_seq == ccid_slots[sid].ctrl_seq) {
		scd_debug(SCD_DEBUG_ABORT, 4);
		ccid_kbd_abort();
		scd_debug(SCD_DEBUG_ABORT, 5);
		ifd_xchg_abort(sid);
		scd_debug(SCD_DEBUG_ABORT, 2);
		aborting = true;
		ccid_slots[sid].aborting = CCID_ABORT_NONE;
	}
	ccid_qid_restore(sqid);

	if (type == CCID_ABORT_BULK) {
		scd_debug(SCD_DEBUG_ABORT, 3);
		if (!aborting)
			err = SCS_ERR_ABORTED;
		ccid_ScsSequence_cmp(err);
	}
}

static void ccid_enqueue(ccid_qid_t qid)
{
	ccid_seq_queue[ccid_nr_seqs] = qid;
	ccid_nr_seqs++;
	BUG_ON(ccid_nr_seqs > NR_SCD_QUEUES);

	if (qid == CCID_QID_IN) {
		ccid_submit_response();
	}
}

static void ccid_dequeue(ccid_qid_t qid)
{
	ccid_seq_t i;

	BUG_ON(qid >= INVALID_SCD_QID);

	if (ccid_nr_seqs > 0 && CCID_QID_IN == qid) {
		/* remove top one */
		ccid_slot_reset(CCID_QID_IN);
		/* old seq should be INVALID */
		ccid_nr_seqs--;

		/* reorder reset ones */
		for (i = 0; i < ccid_nr_seqs; i++) {
			/* new seq should be VALID */
			qid = ccid_seq_queue[i+1];
			BUG_ON(qid > NR_SCD_QUEUES-1);
			ccid_seq_queue[i] = qid;
		}
		BUG_ON(ccid_nr_seqs > NR_SCD_QUEUES);
		ccid_submit_response();
	}
	if (CCID_QID_OUT == qid) {
		ccid_submit_command();
	}
}

void scd_slot_enter(uint8_t state)
{
	if (ccid_slots[scd_qid].state != state) {
		scd_debug(SCD_DEBUG_STATE, state);
		ccid_slots[scd_qid].state = state;
	}
	if (state == SCD_SLOT_STATE_RDR2PC) {
		ccid_enqueue(scd_qid);
	}
	if (state == SCD_SLOT_STATE_PC2RDR) {
		ccid_dequeue(scd_qid);
		scd_resps[scd_qid].abRFU3 = 0;
	}
}

static void ccid_submit_response(void)
{
	if (ccid_nr_seqs > 0 &&
	    ccid_slots[CCID_QID_IN].state == SCD_SLOT_STATE_RDR2PC) {
		usbd_request_submit(CCID_ADDR_IN,
				    SCD_HEADER_SIZE +
				    scd_resps[CCID_QID_IN].dwLength);
	}
}

static void ccid_submit_command(void)
{
	if (ccid_slots[CCID_QID_OUT].state == SCD_SLOT_STATE_PC2RDR) {
		usbd_request_submit(CCID_ADDR_OUT, SCD_HEADER_SIZE);
	}
}

/*=========================================================================
 * bulk-out data
 *=======================================================================*/
static void ccid_DataBlock_out(void)
{
	scs_size_t nr = ifd_xchg_avail();
	scs_size_t ne = CCID_XB_NE;

	__scd_CmdSuccess_out();
	scd_resps[scd_qid].abRFU3 = 0;
	scd_resps[scd_qid].dwLength = ne ? (nr < ne ? nr : ne) : nr;
}

#ifndef CONFIG_IFD_AUTO_PPS_PROP
static void ccid_DataAndFreq_out(void)
{
	__scd_CmdSuccess_out();
	/* TODO: chain parameter */
	scd_resps[scd_qid].abRFU3 = 0;
	scd_resps[scd_qid].dwLength = 8;
}

static void ccid_Parameters_out(void)
{
	__scd_CmdSuccess_out();
	switch (scd_resps[scd_qid].abRFU3) {
	case SCD_PROTOCOL_T0:
		scd_resps[scd_qid].dwLength = sizeof (struct scd_t0_param);
		break;
#ifdef CONFIG_IFD_T1
	case SCD_PROTOCOL_T1:
		scd_resps[scd_qid].dwLength = sizeof (struct scd_t1_param);
		break;
#endif
	}
}

static void ccid_GetParameters_out(void)
{
#ifdef CONFIG_IFD_AUTO_PPS_PROP
	/* FIXME: Windows Host Driver BUG?
	 *
	 * If this is enabled, host applications will not find card
	 * inserted.  We wonder whether windows does not care about
	 * parameters on APDU level exchange.
	 */
	scd_resps[scd_qid].abRFU3 = SCD_PROTOCOL_T0;
#else
	scd_resps[scd_qid].abRFU3 = ifd_get_proto();
#endif
	ccid_Parameters_out();
}

static void ccid_SetParametersT0_out(void)
{
	USBD_OUTB(ccid_cmd_data.t0.bmFindexDindex);
	USBD_OUTB(ccid_cmd_data.t0.bmTCCKST0);
	USBD_OUTB(ccid_cmd_data.t0.bGuardTimeT0);
	USBD_OUTB(ccid_cmd_data.t0.bWaitingIntegerT0);
	USBD_OUTB(ccid_cmd_data.t0.bClockStop);
	scd_resps[scd_qid].abRFU3 = SCD_PROTOCOL_T0;
	ccid_Parameters_out();
}

#ifdef CONFIG_IFD_T1
static void ccid_SetParametersT1_out(void)
{
	USBD_OUTB(ccid_cmd_data.t1.bmFindexDindex);
	USBD_OUTB(ccid_cmd_data.t1.bmTCCKST1);
	USBD_OUTB(ccid_cmd_data.t1.bGuardTimeT1);
	USBD_OUTB(ccid_cmd_data.t1.bWaitingIntegerT1);
	USBD_OUTB(ccid_cmd_data.t1.bClockStop);
	USBD_OUTB(ccid_cmd_data.t1.bIFSC);
	USBD_OUTB(ccid_cmd_data.t1.bNadValue);
	scd_resps[scd_qid].abRFU3 = SCD_PROTOCOL_T1;
	ccid_Parameters_out();
}
#endif

static void ccid_SetParameters_out(void)
{
	uint8_t bProtocolNum = scd_cmds[scd_qid].abRFU[0];

	switch (bProtocolNum) {
	case SCD_PROTOCOL_T0:
		ccid_SetParametersT0_out();
		break;
#ifdef CONFIG_IFD_T1
	case SCD_PROTOCOL_T1:
		ccid_SetParametersT1_out();
		break;
#endif
	}
}

void ccid_GetParameters_cmp(void)
{
 	ccid_GetParameters_out();
	scd_CmdResponse_cmp();
}

void ccid_Parameters_cmp(scs_err_t err)
{
	if (scd_slot_success(err)) {
		ccid_GetParameters_cmp();
	} else {
		if (err == IFD_ERR_BAD_FD)
			scd_CmdOffset_cmp(10);
		else
			ccid_ScsSequence_cmp(err);
	}
}

static void ccid_ResetParameters_cmp(void)
{
	scs_err_t err;

	err = ifd_reset_param();
	ccid_Parameters_cmp(err);
}

static void ccid_SetParameters_cmp(void)
{
	uint8_t bProtocolNum = scd_cmds[scd_qid].abRFU[0];
	scs_err_t err;

	switch (bProtocolNum) {
	case SCD_PROTOCOL_T0:
		if (usbd_request_handled() >=
		    SCD_HEADER_SIZE + sizeof (struct scd_t0_param)) {
			scd_slot_enter(SCD_SLOT_STATE_ISO7816);
			err = ifd_set_t0_param(ccid_cmd_data.t0.bmFindexDindex,
					       ccid_cmd_data.t0.bGuardTimeT0,
					       ccid_cmd_data.t0.bWaitingIntegerT0,
					       ccid_cmd_data.t0.bClockStop);
			ccid_Parameters_cmp(err);
		} else {
			scd_CmdOffset_cmp(1);
		}
		break;
#ifdef CONFIG_IFD_T1
	case SCD_PROTOCOL_T1:
		if (usbd_request_handled() >=
		    SCD_HEADER_SIZE + sizeof (struct scd_t1_param)) {
			scd_slot_enter(SCD_SLOT_STATE_ISO7816);
			err = ifd_set_t1_param(ccid_cmd_data.t1.bmFindexDindex,
					       ccid_cmd_data.t1.bmTCCKST1,
					       ccid_cmd_data.t1.bGuardTimeT1,
					       ccid_cmd_data.t1.bWaitingIntegerT1,
					       ccid_cmd_data.t1.bClockStop,
					       ccid_cmd_data.t1.bIFSC,
					       ccid_cmd_data.t1.bNadValue);
			ccid_Parameters_cmp(err);
		} else {
			scd_CmdOffset_cmp(1);
		}
		break;
#endif
	default:
		scd_CmdOffset_cmp(7);
		break;
	}
}

static void ccid_Parameters_in(void)
{
	uint32_t param_length;

	switch (scd_resps[scd_qid].abRFU3) {
	case SCD_PROTOCOL_T0:
		param_length = sizeof (struct scd_t0_param);
		break;
#ifdef CONFIG_IFD_T1
	case SCD_PROTOCOL_T1:
		param_length = sizeof (struct scd_t1_param);
		break;
#endif
	default:
		param_length = 0;
		break;
	}

	scd_RespHeader_in(param_length);

	switch (scd_resps[scd_qid].abRFU3) {
	case SCD_PROTOCOL_T0:
		USBD_INB(ifd_get_t0_param(IFD_T0_PARAM_FD));
		USBD_INB(ifd_get_t0_param(IFD_T0_PARAM_TCCKS));
		USBD_INB(ifd_get_t0_param(IFD_T0_PARAM_GT));
		USBD_INB(ifd_get_t0_param(IFD_T0_PARAM_WI));
		USBD_INB(ifd_get_t0_param(IFD_T0_PARAM_CLOCKSTOP));
		break;
#ifdef CONFIG_IFD_T1
	case SCD_PROTOCOL_T1:
		USBD_INB(ifd_get_t1_param(IFD_T1_PARAM_FD));
		USBD_INB(ifd_get_t1_param(IFD_T1_PARAM_TCCKS));
		USBD_INB(ifd_get_t1_param(IFD_T1_PARAM_GT));
		USBD_INB(ifd_get_t1_param(IFD_T1_PARAM_WI));
		USBD_INB(ifd_get_t1_param(IFD_T1_PARAM_CLOCKSTOP));
		USBD_INB(ifd_get_t1_param(IFD_T1_PARAM_IFSC));
		USBD_INB(ifd_get_t1_param(IFD_T1_PARAM_NAD));
		break;
#endif
	}
}

/* XXX: GetDataAndFreq support
 * When automatic features are activated and multiple clock are supported
 * by the reader, this command is used to achieve active DataRate and
 * ClockFrequency.  The forced values being discarded.
 */
static void ccid_SetDataAndFreq_out(void)
{
	USBD_OUTL(ccid_cmd_data.fd.dwClockFrequency);
	USBD_OUTL(ccid_cmd_data.fd.dwDataRate);
}

static void ccid_GetDataAndFreq_cmp(void)
{
	ccid_DataAndFreq_out();
	scd_CmdResponse_cmp();
}

static void ccid_DataAndFreq_cmp(scs_err_t err)
{
	if (scd_slot_success(err)) {
		ccid_GetDataAndFreq_cmp();
	} else {
		ccid_ScsSequence_cmp(err);
	}
}

static void ccid_SetDataAndFreq_cmp(void)
{
	scs_err_t err;

	err = ifd_set_freq_data(ccid_cmd_data.fd.dwClockFrequency,
				ccid_cmd_data.fd.dwDataRate);
	ccid_DataAndFreq_cmp(err);
}

static void ccid_DataAndFreq_in(void)
{
	scd_RespHeader_in(8);
	USBD_INL(ifd_get_freq());
	USBD_INL(ifd_get_data());
}
#else
/* No ISO7816 */
#define ccid_Parameters_cmp(err)	BUG()
#define ccid_DataAndFreq_cmp(err)	BUG()

/* IN endpoint */
#define ccid_Parameters_in()		scd_SlotStatus_in()
#define ccid_DataAndFreq_in()		scd_SlotStatus_in()

/* IN endpoint completion */
#define ccid_GetParameters_out()	scd_CmdFailure_out(CCID_ERROR_CMD_UNSUPPORT)
void ccid_GetParameters_cmp(void)
{
 	ccid_GetParameters_out();
	scd_CmdResponse_cmp();
}
static void ccid_ResetParameters_cmp(void)
{
 	ccid_GetParameters_out();
	scd_CmdResponse_cmp();
}

#define ccid_SetParameters_cmp()	scd_SlotStatus_cmp()
#define ccid_GetDataAndFreq_cmp()	scd_SlotStatus_cmp()
#define ccid_SetDataAndFreq_cmp()	scd_SlotStatus_cmp()

/* OUT endpoint */
#define ccid_SetParameters_out()	scd_CmdFailure_out(CCID_ERROR_CMD_UNSUPPORT)
#define ccid_SetDataAndFreq_out()	scd_CmdFailure_out(CCID_ERROR_CMD_UNSUPPORT)
#endif

static void ccid_IccPowerOn_out(void)
{
	if (usbd_request_handled() == SCD_HEADER_SIZE) {
		/* reset SCS error */
		CCID_XB_ERR = SCS_ERR_SUCCESS;
		/* reset Nc */
		CCID_XB_NC = 0;
		/* Ne should be 32 (ATR) + 1 (TS) */
		CCID_XB_NE = IFD_ATR_MAX;
		/* reset WI */
		CCID_XB_WI = 0;
	}
}

void __ccid_XfrBlock_out(scs_size_t hdr_size, scs_size_t blk_size)
{
	scs_off_t i;
	uint8_t byte = 0;

	if (usbd_request_handled() == hdr_size) {
		/* reset SCS error */
		CCID_XB_ERR = SCS_ERR_SUCCESS;
		/* reset Nc */
		CCID_XB_NC = 0;
		/* Ne is from wLevelParameter
		 * TODO: extended APDU level
		 * more efforts
		 */
		CCID_XB_NE = (scd_cmds[scd_qid].abRFU[1] << 8) |
			      scd_cmds[scd_qid].abRFU[2];
		/* reset WI */
		CCID_XB_WI = scd_cmds[scd_qid].abRFU[0];
	}
	/* force USB reap on error */
	if (!scd_slot_success(CCID_XB_ERR)) {
		return;
	}

	usbd_iter_accel();
	for (i = CCID_XB_NC; i < blk_size; i++) {
		/* XXX: USBD_OUTB May Fake Reads 'byte'
		 * Following codes are enabled only when USBD_OUTB actually
		 * gets something into the 'byte', which happens when:
		 * handled-1 == NC+hdr_size.
		 */
		USBD_OUT_BEGIN(byte) {
			/* Now byte contains non-fake value. */
			CCID_XB_ERR = scd_write_byte(CCID_XB_NC, byte);
			if (!scd_slot_success(CCID_XB_ERR)) {
				return;
			}
			CCID_XB_NC++;
		} USBD_OUT_END
	}
}

static void ccid_XfrBlock_out(void)
{
	__ccid_XfrBlock_out(SCD_HEADER_SIZE, scd_cmds[scd_qid].dwLength);
}

static void ccid_handle_slot_pc2rdr(void)
{
	if (scd_cmds[scd_qid].bMessageType == CCID_PC2RDR_GETSLOTSTATUS) {
		return;
	}
	if (scd_slot_status() == SCD_SLOT_STATUS_NOTPRESENT) {
		scd_CmdFailure_out(CCID_ERROR_ICC_MUTE);
		return;
	}
#if 0
	/* FIXME: check auto sequence */
	if (scd_slot_status() != SCD_SLOT_STATUS_ACTIVE) {
		scd_CmdFailure_out(CCID_ERROR_BUSY_AUTO_SEQ);
		return;
	}
#endif

	switch (scd_cmds[scd_qid].bMessageType) {
	case SCD_PC2RDR_ICCPOWERON:
		ccid_IccPowerOn_out();
		break;
	case SCD_PC2RDR_ICCPOWEROFF:
	case CCID_PC2RDR_RESETPARAMETERS:
	case CCID_PC2RDR_GETPARAMETERS:
	case CCID_PC2RDR_T0APDU:
#ifdef CONFIG_IFD_CLOCK_CONTROL
	case CCID_PC2RDR_ICCCLOCK:
#endif
#ifdef CONFIG_IFD_MECHA_CONTROL
	case CCID_PC2RDR_MECHANICAL:
#endif
		/* nothing to do */
		break;
	case SCD_PC2RDR_XFRBLOCK:
		ccid_XfrBlock_out();
		break;
	case CCID_PC2RDR_SECURE:
		ccid_Secure_out();
		break;
	case SCD_PC2RDR_ESCAPE:
		scd_Escape_out();
		break;
	case CCID_PC2RDR_SETPARAMETERS:
		ccid_SetParameters_out();
		break;
	case CCID_PC2RDR_SETDATAANDFREQ:
		ccid_SetDataAndFreq_out();
		break;
	}
}

static void ccid_handle_command(void)
{
	ifd_sid_t sid;

	ccid_qid_select(CCID_QID_OUT);
	scd_debug(SCD_DEBUG_SLOT, scd_qid);

	BUG_ON(ccid_slots[scd_qid].state != SCD_SLOT_STATE_PC2RDR &&
	       ccid_slots[scd_qid].state != SCD_SLOT_STATE_SANITY);

	scd_slot_enter(SCD_SLOT_STATE_SANITY);

	/* CCID message header */
	scd_CmdHeader_out();

	if (usbd_request_handled() < SCD_HEADER_SIZE)
		return;

	if (usbd_request_handled() == SCD_HEADER_SIZE) {
		usbd_request_commit(SCD_HEADER_SIZE +
				    scd_cmds[scd_qid].dwLength);
		scd_debug(SCD_DEBUG_PC2RDR, scd_cmds[scd_qid].bMessageType);
	}

	sid = scd_cmds[scd_qid].bSlot;
	if (sid >= NR_SCD_SLOTS) {
		return;
	}
	if (scd_cmds[scd_qid].bMessageType == CCID_PC2RDR_ABORT) {
		return;
	}
	if (ccid_slots[sid].aborting != CCID_ABORT_NONE) {
		scd_CmdFailure_out(CCID_ERROR_CMD_ABORTED);
		return;
	}
	if (ccid_slots[sid].state != SCD_SLOT_STATE_PC2RDR) {
		scd_CmdFailure_out(CCID_ERROR_CMD_SLOT_BUSY);
		return;
	}

	if (usbd_request_handled() == SCD_HEADER_SIZE) {
		scd_cmds[sid].bMessageType = scd_cmds[scd_qid].bMessageType;
		scd_cmds[sid].dwLength = scd_cmds[scd_qid].dwLength;
		scd_cmds[sid].bSlot = scd_cmds[scd_qid].bSlot;
		scd_cmds[sid].bSeq = scd_cmds[scd_qid].bSeq;
		scd_cmds[sid].abRFU[0] = scd_cmds[scd_qid].abRFU[0];
		scd_cmds[sid].abRFU[1] = scd_cmds[scd_qid].abRFU[1];
		scd_cmds[sid].abRFU[2] = scd_cmds[scd_qid].abRFU[2];
	}

	/* slot ID determined */
	ccid_sid_select(sid);
	scd_debug(SCD_DEBUG_SLOT, scd_qid);

	ccid_handle_slot_pc2rdr();
}

/*=========================================================================
 * bulk-in data
 *=======================================================================*/
void ccid_DataBlock_in(void)
{
	scs_off_t i;

	scd_RespHeader_in(scd_resps[scd_qid].dwLength);

	usbd_iter_accel();

	for (i = usbd_request_handled()-SCD_HEADER_SIZE;
	     i < scd_resps[scd_qid].dwLength; i++) {
		USBD_INB(scd_read_byte(i));
	}

#if 0
	if (usbd_request_handled() ==
	    scd_resps[scd_qid].dwLength + SCD_HEADER_SIZE) {
		/* allow next XfrBlock fetching next bytes */
		scd_resps[scd_qid].dwLength;
	}
#endif

	BUG_ON(usbd_request_handled() >
	       scd_resps[scd_qid].dwLength + SCD_HEADER_SIZE);
}

static void ccid_handle_response(void)
{
	ccid_qid_select(CCID_QID_IN);
	scd_debug(SCD_DEBUG_SLOT, scd_qid);

	BUG_ON(ccid_nr_seqs == 0);
	BUG_ON(ccid_slots[scd_qid].state != SCD_SLOT_STATE_RDR2PC);
	BUG_ON(scd_qid >= NR_SCD_QUEUES);

	if (scd_cmds[scd_qid].bMessageType == CCID_PC2RDR_ABORT) {
		scd_SlotStatus_in();
		return;
	}
	if (ccid_is_cmd_status(SCD_CMD_STATUS_FAIL)) {
		scd_SlotStatus_in();
		return;
	}

	BUG_ON(scd_qid >= NR_SCD_SLOTS);
	BUG_ON(scd_cmds[scd_qid].bSlot != scd_qid);

	ccid_sid_select(scd_cmds[scd_qid].bSlot);
	scd_debug(SCD_DEBUG_SLOT, scd_qid);

	switch (scd_cmds[scd_qid].bMessageType) {
	case SCD_PC2RDR_ICCPOWERON:
	case SCD_PC2RDR_XFRBLOCK:
	case CCID_PC2RDR_SECURE:
		ccid_DataBlock_in();
		break;
	case SCD_PC2RDR_ICCPOWEROFF:
	case CCID_PC2RDR_GETSLOTSTATUS:
#ifdef CONFIG_IFD_CLOCK_CONTROL
	case CCID_PC2RDR_ICCCLOCK:
#endif
#ifdef CONFIG_IFD_MECHA_CONTROL
	case CCID_PC2RDR_MECHANICAL:
#endif
	case CCID_PC2RDR_T0APDU:
		scd_SlotStatus_in();
		break;
	case CCID_PC2RDR_GETPARAMETERS:
	case CCID_PC2RDR_RESETPARAMETERS:
	case CCID_PC2RDR_SETPARAMETERS:
		ccid_Parameters_in();
		break;
	case SCD_PC2RDR_ESCAPE:
		scd_Escape_in();
		break;
	case CCID_PC2RDR_SETDATAANDFREQ:
		ccid_DataAndFreq_in();
		break;
	case CCID_PC2RDR_ABORT:
	default:
		BUG();
		break;
	}
}

static void ccid_complete_response(void)
{
	ccid_qid_select(CCID_QID_IN);
	scd_debug(SCD_DEBUG_SLOT, scd_qid);
	BUG_ON(ccid_nr_seqs > NR_SCD_QUEUES);
	if (ccid_nr_seqs > 0) {
		scd_slot_enter(SCD_SLOT_STATE_PC2RDR);
	}
}

/*=========================================================================
 * bulk-out cmpl
 *=======================================================================*/
void ccid_GetDataBlock_cmp(void)
{
	ccid_DataBlock_out();
	scd_CmdResponse_cmp();
}

void ccid_DataBlock_cmp(scs_err_t err)
{
	if (scd_slot_success(err)) {
		ccid_GetDataBlock_cmp();
	} else {
		ccid_ScsSequence_cmp(err);
	}
}

#ifdef CONFIG_IFD_CLOCK_CONTROL
static void ccid_IccClock_cmp(void)
{
	uint8_t bClockCommand = scd_cmds[scd_qid].abRFU[0];
	scs_err_t err;

	switch (bClockCommand) {
	case CCID_CLOCK_RESTART:
		scd_slot_enter(SCD_SLOT_STATE_ISO7816);
		err = ifd_restart_clock();
		ccid_ScsSequence_cmp(err);
		break;
	case CCID_CLOCK_STOP:
		scd_slot_enter(SCD_SLOT_STATE_ISO7816);
		err = ifd_stop_clock();
		ccid_ScsSequence_cmp(err);
		break;
	default:
		scd_CmdOffset_cmp(7);
		break;
	}
}
#endif

#ifdef CONFIG_IFD_MECHA_CONTROL
static void ccid_Mechanical_cmp(void)
{
	scs_err_t err;
	uint8_t bMechaCommand = scd_cmds[scd_qid].abRFU[0];

	switch (bMechaCommand) {
	case CCID_MECHA_LOCK_CARD:
		scd_slot_enter(SCD_SLOT_STATE_ISO7816);
		err = ifd_lock_card();
		ccid_ScsSequence_cmp(err);
		break;
	case CCID_MECHA_UNLOCK_CARD:
		scd_slot_enter(SCD_SLOT_STATE_ISO7816);
		err = ifd_unlock_card();
		ccid_ScsSequence_cmp(err);
		break;
	default:
		scd_CmdOffset_cmp(7);
		break;
	}
}
#endif

static void ccid_Abort_cmp(void)
{
	ccid_slot_abort(CCID_ABORT_BULK,
			scd_cmds[scd_qid].bSlot,
			scd_cmds[scd_qid].bSeq);
}

void ccid_ScsSequence_cmp(scs_err_t err)
{
	if (!scd_slot_progress(err)) {
		BUG_ON(scd_slot_success(err));
		if (!scd_slot_success(err))
			scd_CmdFailure_out(ccid_slot_error(err));
		else
			scd_SlotStatus_out();
		scd_CmdResponse_cmp();
	} else {
		scd_slot_enter(SCD_SLOT_STATE_ISO7816);
	}
}

static void ccid_IccPowerOn_cmp(void)
{
	scs_err_t err;
	uint8_t bPowerSelect = scd_cmds[scd_qid].abRFU[0];

	err = ifd_power_on(bPowerSelect);
	ccid_ScsSequence_cmp(err);
}

static void ccid_IccPowerOff_cmp(void)
{
	scs_err_t err;

	err = ifd_power_off();
	ccid_ScsSequence_cmp(err);
}

#ifdef CONFIG_IFD_T0_APDU
static void ccid_T0APDU_cmp(void)
{
	scs_err_t err;

	err = ifd_set_t0_apdu(scd_cmds[scd_qid].abRFU[0],
			      scd_cmds[scd_qid].abRFU[1],
			      scd_cmds[scd_qid].abRFU[2]);
	ccid_ScsSequence_cmp(err);
}
#endif

void ccid_XfrBlock_cmp(void)
{
	scs_err_t err = CCID_XB_ERR;

	/* TODO: wLevelParameter check when XCHG_CHAR or XCHG_APDU_EXT */
	if (scd_slot_success(err) && CCID_XB_NC != 0) {
		err = ifd_xchg_block(CCID_XB_NC, CCID_XB_NE);
	}
	ccid_ScsSequence_cmp(err);
}

static void ccid_complete_slot_pc2rdr(void)
{
	if (usbd_request_handled() !=
	    (scd_cmds[scd_qid].dwLength + SCD_HEADER_SIZE)) {
		scd_CmdOffset_cmp(1);
		return;
	}
	if (scd_cmds[scd_qid].bMessageType == CCID_PC2RDR_GETSLOTSTATUS) {
		scd_SlotStatus_cmp();
		return;
	}
	if (ccid_is_cmd_status(SCD_CMD_STATUS_FAIL)) {
		scd_CmdResponse_cmp();
		return;
	}

	switch (scd_cmds[scd_qid].bMessageType) {
	case SCD_PC2RDR_ICCPOWERON:
		ccid_IccPowerOn_cmp();
		break;
	case SCD_PC2RDR_ICCPOWEROFF:
		ccid_IccPowerOff_cmp();
		break;
	case SCD_PC2RDR_XFRBLOCK:
		ccid_XfrBlock_cmp();
		break;
	case CCID_PC2RDR_SECURE:
		ccid_Secure_cmp();
		break;
	case SCD_PC2RDR_ESCAPE:
		scd_Escape_cmp();
		break;
	case CCID_PC2RDR_SETPARAMETERS:
		ccid_SetParameters_cmp();
		break;
	case CCID_PC2RDR_GETPARAMETERS:
		ccid_GetParameters_cmp();
		break;
	case CCID_PC2RDR_RESETPARAMETERS:
		ccid_ResetParameters_cmp();
		break;
#ifdef CONFIG_IFD_T0_APDU
	case CCID_PC2RDR_T0APDU:
		ccid_T0APDU_cmp();
		break;
#endif
#ifdef CONFIG_IFD_CLOCK_CONTROL
	case CCID_PC2RDR_ICCCLOCK:
		ccid_IccClock_cmp();
		break;
#endif
#ifdef CONFIG_IFD_MECHA_CONTROL
	case CCID_PC2RDR_MECHANICAL:
		ccid_Mechanical_cmp();
		break;
#endif
	case CCID_PC2RDR_SETDATAANDFREQ:
		ccid_SetDataAndFreq_cmp();
		break;
	case CCID_PC2RDR_GETSLOTSTATUS:
	case CCID_PC2RDR_ABORT:
		/* handled before */
		BUG();
		break;
	default:
		scd_CmdOffset_cmp(0);
		break;
	}
}

static void ccid_complete_command(void)
{
	ifd_sid_t sid;

	ccid_qid_select(CCID_QID_OUT);
	scd_debug(SCD_DEBUG_SLOT, scd_qid);

	BUG_ON(ccid_slots[scd_qid].state != SCD_SLOT_STATE_PC2RDR &&
	       ccid_slots[scd_qid].state != SCD_SLOT_STATE_SANITY);

	if (usbd_request_handled() < SCD_HEADER_SIZE ||
	    scd_cmds[scd_qid].bSlot >= NR_SCD_SLOTS) {
		scd_SlotNotExist_cmp();
		return;
	}

	sid = scd_cmds[scd_qid].bSlot;
	/* XXX: care should be taken on BULK ABORT */
	if (scd_cmds[scd_qid].bMessageType == CCID_PC2RDR_ABORT) {
		ccid_Abort_cmp();
		return;
	}
	if (ccid_is_cmd_status(SCD_CMD_STATUS_FAIL)) {
		/* CMD_ABORTED and SLOT_BUSY error */
		scd_CmdResponse_cmp();
		return;
	}

	/* SANITY state completed */
	/* CCID_QID_OUT should be idle */
	scd_slot_enter(SCD_SLOT_STATE_PC2RDR);

	/* now we are able to handle slot specific request */
	ccid_sid_select(sid);
	scd_debug(SCD_DEBUG_SLOT, scd_qid);
	ccid_complete_slot_pc2rdr();
}

/*============================================================
 * CCID spe
 *============================================================*/
#ifdef CONFIG_CCID_SECURE
#include "ccid_spe.c"

/*============================================================
 * !!don't update following codes!!
 *============================================================*/
static void ccid_PinBlock_cmp(void)
{
	ccid_display_default();
	ccid_XfrBlock_cmp();
}

static void ccid_BadInsByte_out(void)
{
	CCID_XB_ERR = SCS_ERR_PARITY_ERR;
}

static void ccid_PinInsByte_out(void)
{
	uint8_t ins = scd_read_byte(1);
	scd_debug(SCD_DEBUG_INS, ins);
	switch (ccid_cmd_data.po.bPINOpeartion) {
	case CCID_SPE_PIN_VERIFY:
		if (ins != 0x20)
			ccid_BadInsByte_out();
		break;
	case CCID_SPE_PIN_MODIFY:
		if (ins != 0x24)
			ccid_BadInsByte_out();
		break;
	}
}

static void ccid_PinBlock_out(uint8_t base_size)
{
	if (ccid_spe_msg_number() > 1)
		base_size += ccid_spe_msg_number()-1;
	if (usbd_request_handled() >= (SCD_HEADER_SIZE + base_size)) {
		__ccid_XfrBlock_out(SCD_HEADER_SIZE + base_size,
				    scd_cmds[scd_qid].dwLength - base_size);
		if (usbd_request_handled() >= (SCD_HEADER_SIZE + base_size + 2)) {
			ccid_PinInsByte_out();
		}
	}
}

static void ccid_PinMessage_out(void)
{
	USBD_OUTW(ccid_cmd_data.po.wLangId);
	USBD_OUTB(ccid_cmd_data.po.bMsgIndex[0]);
	if (ccid_spe_msg_number() == 0)
		ccid_cmd_data.po.bMsgIndex[0] = 0xFF;
	if (ccid_spe_msg_number() > 1)
		USBD_OUTB(ccid_cmd_data.po.bMsgIndex[1]);
	else
		ccid_cmd_data.po.bMsgIndex[1] = 0xFF;
	if (ccid_spe_msg_number() > 2)
		USBD_OUTB(ccid_cmd_data.po.bMsgIndex[2]);
	else
		ccid_cmd_data.po.bMsgIndex[2] = 0xFF;
}

static void ccid_PinFormat_out(void)
{
	USBD_OUTB(ccid_cmd_data.po.bTimeout);
	USBD_OUTB(ccid_cmd_data.po.bmFormatString);
	USBD_OUTB(ccid_cmd_data.po.bmPINBlockString);
	USBD_OUTB(ccid_cmd_data.po.bmPINLengthFormat);
}

static void ccid_PinTeoPrologue_out(void)
{
	USBD_OUTB(ccid_cmd_data.po.bTeoPrologue[0]);
	USBD_OUTB(ccid_cmd_data.po.bTeoPrologue[1]);
	USBD_OUTB(ccid_cmd_data.po.bTeoPrologue[2]);
}

static boolean ccid_EntryValidaton_ok(void)
{
	if (!ccid_cmd_data.po.bEntryValidationCondition) {
		CCID_XB_ERR = SCS_ERR_TIMEOUT;
		return false;
	}
	return true;
}

static void ccid_PinVerify_out(void)
{
	ccid_PinFormat_out();
	USBD_OUTW(ccid_cmd_data.po.wPINMaxExtraDigit);
	USBD_OUTB(ccid_cmd_data.po.bEntryValidationCondition);
	USBD_OUTB(ccid_cmd_data.po.bNumberMessage);
	if (usbd_request_handled() >=
	    (SCD_HEADER_SIZE + CCID_SPE_VERIFY_FIXED)) {
		if (ccid_EntryValidaton_ok()) {
			ccid_PinMessage_out();
			ccid_PinTeoPrologue_out();
			ccid_PinBlock_out(CCID_SPE_VERIFY_SIZE);
		}
	}
}

static void ccid_PinModify_out(void)
{
	ccid_PinFormat_out();
	USBD_OUTB(ccid_cmd_data.pm.bInsertionOffsetOld);
	USBD_OUTB(ccid_cmd_data.pm.bInsertionOffsetNew);
	USBD_OUTW(ccid_cmd_data.po.wPINMaxExtraDigit);
	USBD_OUTB(ccid_cmd_data.pm.bConfirmPIN);
	USBD_OUTB(ccid_cmd_data.po.bEntryValidationCondition);
	USBD_OUTB(ccid_cmd_data.po.bNumberMessage);
	if (usbd_request_handled() >=
	    (SCD_HEADER_SIZE + CCID_SPE_MODIFY_FIXED)) {
		if (ccid_EntryValidaton_ok()) {
			ccid_PinMessage_out();
			ccid_PinTeoPrologue_out();
			ccid_PinBlock_out(CCID_SPE_MODIFY_SIZE);
		}
	}
}

static void ccid_PinVerify_cmp(void)
{
	/* TODO: make sure CCID_XB_NC is sufficient to contain PINs */
	if (scd_slot_success(CCID_XB_ERR)) {
		/* TODO: call scd_CmdOffset_cmp(); */
		ccid_spe_operate_init();
	}
}

static void ccid_PinModify_cmp(void)
{
	/* TODO: make sure CCID_XB_NC is sufficient to contain PINs */
	if (scd_slot_success(CCID_XB_ERR)) {
		/* TODO: call scd_CmdOffset_cmp(); */
		ccid_spe_operate_init();
	}
}

void ccid_Secure_out(void)
{
	USBD_OUTB(ccid_cmd_data.po.bPINOpeartion);
	if (usbd_request_handled() >= SCD_HEADER_SIZE+1) {
		switch (ccid_cmd_data.po.bPINOpeartion) {
		case CCID_SPE_PIN_VERIFY:
			ccid_PinVerify_out();
			break;
		case CCID_SPE_PIN_MODIFY:
			ccid_PinModify_out();
			break;
		}
	}
}

void ccid_Secure_cmp(void)
{
	switch (ccid_cmd_data.po.bPINOpeartion) {
	case CCID_SPE_PIN_VERIFY:
		ccid_PinVerify_cmp();
		break;
	case CCID_SPE_PIN_MODIFY:
		ccid_PinModify_cmp();
		break;
	default:
		scd_CmdOffset_cmp(10);
		break;
	}
}

void ccid_spe_init(void)
{
	ccid_spe_slot_init();
}
#endif

/*=========================================================================
 * interrupt data
 *=======================================================================*/
#ifdef CONFIG_SCD_INTERRUPT
static void ccid_change_init(void);

/*=========================================================================
 * slot changes
 *=======================================================================*/
#define CCID_INTR_CHANGE(sid)		((sid<<1)+1)
#define CCID_INTR_STATUS(sid)		((sid<<1))
static boolean __ccid_change_running(ifd_sid_t sid)
{
	return test_bit(CCID_INTR_CHANGE(sid), ccid_running_intrs);
}

#if 0
static boolean ccid_change_running(void)
{
	ifd_sid_t sid;

	for (sid = 0; sid < NR_SCD_SLOTS; sid++) {
		if (__ccid_change_running(sid))
			return true;
	}
	return false;
}
#endif

static boolean __ccid_change_pending(ifd_sid_t sid)
{
	return test_bit(CCID_INTR_CHANGE(sid), ccid_pending_intrs);
}

static boolean ccid_change_pending(void)
{
	ifd_sid_t sid;

	for (sid = 0; sid < NR_SCD_SLOTS; sid++) {
		if (__ccid_change_pending(sid))
			return true;
	}
	return false;
}

static void ccid_change_data(void)
{
	uint8_t i;
	
	USBD_INB(CCID_RDR2PC_NOTIFYSLOTCHANGE);
	for (i = 0; i < sizeof (ccid_running_intrs); i++) {
		USBD_INB(ccid_running_intrs[i]);
	}
}

static void ccid_change_discard(void)
{
	ifd_sid_t sid;

	for (sid = 0; sid < NR_SCD_SLOTS; sid++) {
		if (__ccid_change_running(sid)) {
			clear_bit(CCID_INTR_CHANGE(sid), ccid_running_intrs);
			scd_debug(SCD_DEBUG_INTR, CCID_INTR_RUNNING_UNSET);
		}
	}
}

static void ccid_change_submit(void)
{
	ifd_sid_t sid;

	for (sid = 0; sid < NR_SCD_SLOTS; sid++) {
		/* copy changed bits */
		clear_bit(CCID_INTR_CHANGE(sid), ccid_pending_intrs);
		scd_debug(SCD_DEBUG_INTR, CCID_INTR_PENDING_UNSET);
		set_bit(CCID_INTR_CHANGE(sid), ccid_running_intrs);
		scd_debug(SCD_DEBUG_INTR, CCID_INTR_RUNNING_SET);

		/* copy status bits */
		if (test_bit(CCID_INTR_STATUS(sid), ccid_pending_intrs)) {
			set_bit(CCID_INTR_STATUS(sid), ccid_running_intrs);
			scd_debug(SCD_DEBUG_INTR, CCID_INTR_ICC_PRESENT);
		} else {
			clear_bit(CCID_INTR_STATUS(sid), ccid_running_intrs);
			scd_debug(SCD_DEBUG_INTR, CCID_INTR_ICC_NOTPRESENT);
		}
	}
}

#ifdef CONFIG_CCID_DISCARD
static void ccid_discard(void)
{
	ccid_seq_t i, j;
	ccid_qid_t qid;

	/* discard running response with matched SID */
	/* discard pending responses with matched SID */
	for (i = 1, j = 1; i < ccid_nr_seqs; i++) {
		qid = ccid_seq_queue[i];
		/* new seq should be VALID */
		BUG_ON(qid > NR_SCD_QUEUES-1);
		if (scd_cmds[scd_qid].bSlot != scd_qid) {
			ccid_seq_queue[j++] = qid;
		} else {
			ccid_qid_t sqid;
			ccid_nr_seqs--;
			sqid = ccid_qid_save(CCID_QID_OUT);
			scd_slot_enter(SCD_SLOT_STATE_PC2RDR);
			ccid_qid_restore(sqid);
		}
	}

	BUG_ON(ccid_nr_seqs > NR_SCD_QUEUES);

	/* allow host sending commands, this should be called before
	 * ccid_submit_response
	 */
	if (CCID_QID_IN == scd_qid) {
		/* usbd_request_discard_addr(CCID_ADDR_OUT); */
		usbd_request_discard_addr(CCID_ADDR_IN);
	}
}
#else
#define ccid_discard()
#endif

static void ccid_change_raise(void)
{
	if (scd_qid < NR_SCD_SLOTS) {
		boolean changed = false;
		if (scd_slot_status() == SCD_SLOT_STATUS_NOTPRESENT) {
			ccid_discard();
			if (test_bit(CCID_INTR_STATUS(scd_qid), ccid_pending_intrs)) {
				clear_bit(CCID_INTR_STATUS(scd_qid), ccid_pending_intrs);
				changed = true;
			}
		} else {
			if (!test_bit(CCID_INTR_STATUS(scd_qid), ccid_pending_intrs)) {
				set_bit(CCID_INTR_STATUS(scd_qid), ccid_pending_intrs);
				changed = true;
			}
		}
		if (changed) {
			set_bit(CCID_INTR_CHANGE(scd_qid), ccid_pending_intrs);
			scd_debug(SCD_DEBUG_INTR, CCID_INTR_PENDING_SET);
		}
	}
}

static uint16_t ccid_change_length(void)
{
	return 1 + div16u(ALIGN(NR_SCD_SLOTS, 4), 4);
}

/*=========================================================================
 * hardware errors
 *=======================================================================*/
#ifdef CONFIG_CCID_INTERRUPT_HWERR
static boolean __ccid_hwerr_running(ifd_sid_t sid)
{
	return ccid_intr_hwerrs[sid].bState & CCID_HWERR_RUNNING;
}

static boolean ccid_hwerr_running(void)
{
	ifd_sid_t sid;
	for (sid = 0; sid < NR_SCD_SLOTS; sid++) {
		if (__ccid_hwerr_running(sid))
			return true;
	}
	return false;
}

static boolean __ccid_hwerr_pending(ifd_sid_t sid)
{
	return ccid_intr_hwerrs[sid].bState & CCID_HWERR_PENDING;
}

static boolean ccid_hwerr_pending(void)
{
	ifd_sid_t sid;
	for (sid = 0; sid < NR_SCD_SLOTS; sid++) {
		if (__ccid_hwerr_pending(sid))
			return true;
	}
	return false;
}

static void ccid_hwerr_raise(uint8_t err)
{
	if (scd_qid < NR_SCD_SLOTS) {
		ccid_intr_hwerrs[scd_qid].bState |= CCID_HWERR_PENDING;
		ccid_intr_hwerrs[scd_qid].bPendingSeq = scd_cmds[scd_qid].bSeq;
		ccid_intr_hwerrs[scd_qid].bPendingCode |= err;
	}
}

static boolean ccid_hwerr_data(void)
{
	ifd_sid_t sid;
	for (sid = 0; sid < NR_SCD_SLOTS; sid++) {
		if (__ccid_hwerr_running(sid)) {
			USBD_INB(CCID_RDR2PC_HARDWAREERROR);
			USBD_INB(sid);
			USBD_INB(ccid_intr_hwerrs[sid].bSeq);
			USBD_INB(ccid_intr_hwerrs[sid].bHardwareErrorCode);
			return true;
		}
	}
	return false;
}

static void ccid_hwerr_discard(void)
{
	ifd_sid_t sid;
	for (sid = 0; sid < NR_SCD_SLOTS; sid++) {
		if (__ccid_hwerr_running(sid)) {
			ccid_intr_hwerrs[sid].bState &= ~CCID_HWERR_RUNNING;
		}
	}
}

static boolean ccid_hwerr_submit(void)
{
	ifd_sid_t sid;
	for (sid = 0; sid < NR_SCD_SLOTS; sid++) {
		if (ccid_hwerr_pending(sid)) {
			ccid_intr_hwerrs[sid].bState &= ~CCID_HWERR_PENDING;
			ccid_intr_hwerrs[sid].bState |= CCID_HWERR_RUNNING;
			return true;
		}
	}
	return false;
}

static uint16_t ccid_hwerr_pending_length(void)
{
	ifd_sid_t sid;
	for (sid = 0; sid < NR_SCD_SLOTS; sid++) {
		if (ccid_hwerr_pending(sid))
			return CCID_IRQ_HWERR_SIZE;
	}
	return 0;
}

static uint16_t ccid_hwerr_running_length(void)
{
	ifd_sid_t sid;
	for (sid = 0; sid < NR_SCD_SLOTS; sid++) {
		if (ccid_hwerr_running(sid))
			return CCID_IRQ_HWERR_SIZE;
	}
	return 0;
}

static void ccid_hwerr_init(void)
{
	ifd_sid_t sid;
	for (sid = 0; sid < NR_SCD_SLOTS; sid++) {
		ccid_intr_hwerrs[sid].bState = 0;
		ccid_intr_hwerrs[sid].bSeq = 0;
	}
}

static uint16_t ccid_pending_length(void)
{
	return ccid_hwerr_pending_length() || ccid_change_length();
}

static uint16_t ccid_running_length(void)
{
	return ccid_hwerr_running_length() || ccid_change_length();
}

static boolean ccid_interrupt_running(void)
{
	return ccid_hwerr_running() || ccid_change_running();
}

static boolean ccid_interrupt_pending(void)
{
	return ccid_hwerr_pending() || ccid_change_pending();
}

static void ccid_discard_interrupt(void)
{
	ccid_hwerr_discard();
	ccid_change_discard();
}

static void __ccid_submit_interrupt(void)
{
	if (ccid_hwerr_submit())
		return;
	ccid_change_submit();
}

static void ccid_handle_interrupt(void)
{
	if (ccid_hwerr_data())
		return;
	ccid_change_data();
}

static void ccid_intr_init(void)
{
	ccid_hwerr_init();
	ccid_change_init();
}
#else
#define ccid_interrupt_pending()	ccid_change_pending()
#define ccid_interrupt_running()	ccid_change_running()
#define ccid_running_length()		ccid_change_length()
#define ccid_pending_length()		ccid_change_length()
#define __ccid_submit_interrupt()	ccid_change_submit()
#define ccid_intr_init()		ccid_change_init()

void ccid_discard_interrupt(void)
{
	ccid_change_discard();
}

void ccid_handle_interrupt(void)
{
	ccid_change_data();
}
#endif

/*=========================================================================
 * generic interupts
 *=======================================================================*/
static void ccid_submit_interrupt(void)
{
	if (ccid_interrupt_pending()) {
		if (usbd_request_submit(CCID_ADDR_IRQ,
					ccid_pending_length())) {
			__ccid_submit_interrupt();
		}
	}
}

static void ccid_handle_iso7816_intr(void)
{
	ccid_qid_select(scd_sid);
	BUG_ON(scd_qid >= NR_SCD_SLOTS);
	ccid_change_raise();
}

static void ccid_intr_start(void)
{
	ifd_sid_t sid, ssid;
	for (sid = 0; sid < NR_SCD_SLOTS; sid++) {
		ssid = ccid_qid_save(sid);
		ccid_change_raise();
		ccid_qid_restore(ssid);
	}
}

usbd_endpoint_t ccid_endpoint_irq = {
	USBD_ENDP_INTR_IN,
	CCID_ENDP_INTERVAL_INTR,
	ccid_submit_interrupt,
	ccid_handle_interrupt,
	ccid_discard_interrupt,
};

static void ccid_intr_desc(void)
{
	usbd_input_endpoint_desc(CCID_ADDR_IRQ);
}

static void ccid_change_init(void)
{
	CCID_ADDR_IRQ = usbd_claim_endpoint(true, &ccid_endpoint_irq);
}
#else
static void ccid_handle_iso7816_intr(void)
{
}

#define ccid_intr_desc()
#define ccid_intr_start()
#define ccid_intr_init()
#endif

/*=========================================================================
 * CCID entrances
 *=======================================================================*/
static void ccid_handle_iso7816_cmpl(void)
{
	scs_err_t err;

	/* slot ID is determined before cmpl is called */
	ccid_qid_select(scd_sid);

	BUG_ON(scd_qid >= NR_SCD_SLOTS);
	BUG_ON(ccid_slots[scd_qid].state != SCD_SLOT_STATE_ISO7816);

	err = ifd_xchg_get_error();

	switch (scd_cmds[scd_qid].bMessageType) {
	case SCD_PC2RDR_ICCPOWERON:
	case SCD_PC2RDR_XFRBLOCK:
	case CCID_PC2RDR_SECURE:
		ccid_DataBlock_cmp(err);
		break;
	case SCD_PC2RDR_ICCPOWEROFF:
#ifdef CONFIG_IFD_CLOCK_CONTROL
	case CCID_PC2RDR_ICCCLOCK:
#endif
#ifdef CONFIG_IFD_MECHA_CONTROL
	case CCID_PC2RDR_MECHANICAL:
#endif
		scd_SlotStatus_cmp();
		break;
	case CCID_PC2RDR_SETPARAMETERS:
	case CCID_PC2RDR_RESETPARAMETERS:
		ccid_Parameters_cmp(err);
		break;
	case CCID_PC2RDR_SETDATAANDFREQ:
		ccid_DataAndFreq_cmp(err);
		break;
	default:
		BUG();
		break;
	}
}

/*=========================================================================
 * control data
 *=======================================================================*/
static uint16_t ccid_config_length(void)
{
	return SCD_DT_SCD_SIZE;
}

static void ccid_get_ccid_desc(void)
{
	USBD_INB(SCD_DT_SCD_SIZE);
	USBD_INB(SCD_DT_SCD);
	USBD_INW(CCID_VERSION_DEFAULT);
	USBD_INB(NR_SCD_SLOTS-1);
	USBD_INB(SCD_VOLTAGE_ALL);
	USBD_INL(ccid_proto_features());
	USBD_INL((uint32_t)(IFD_HW_FREQ_DEF));
	USBD_INL((uint32_t)(IFD_HW_FREQ_MAX));
	USBD_INB(ifd_cf_nr_freq());
	USBD_INL((uint32_t)(IFD_HW_DATA_DEF));
	USBD_INL((uint32_t)(IFD_HW_DATA_MAX));
	USBD_INB(ifd_cf_nr_data());
	USBD_INL(IFD_T1_MAX_IFSD);
	USBD_INL(SCD_SYNCH_PROTO_NONE);
	USBD_INL(SCD_MECHA_NONE);
	USBD_INL(ccid_device_features());
	USBD_INL(CCID_MESSAGE_SIZE);
	USBD_INB(SCD_MUTE_APDU_CLASS);
	USBD_INB(SCD_MUTE_APDU_CLASS);
	USBD_INW(CCID_SPE_LCD_LAYOUT);
	USBD_INB(CCID_SPE_SUPPORT_FUNC);
	USBD_INB(NR_SCD_QUEUES);
}

static void ccid_get_config_desc(void)
{
	usbd_input_interface_desc(USB_INTERFACE_CLASS_SCD,
				  USB_DEVICE_SUBCLASS_NONE,
				  USB_INTERFACE_PROTOCOL_SCD,
				  CCID_STRING_INTERFACE);
	ccid_get_ccid_desc();
	usbd_input_endpoint_desc(CCID_ADDR_IN);
	usbd_input_endpoint_desc(CCID_ADDR_OUT);
	ccid_intr_desc();
}

static void ccid_get_string_desc(void)
{
	uint8_t id = LOBYTE(usbd_control_request_value());

	switch (id) {
	case CCID_STRING_INTERFACE:
		usbd_input_device_name();
		break;
	default:
		USBD_INB(0x00);
		break;
	}
}

static void ccid_get_descriptor(void)
{
	uint8_t desc;
	
	desc = HIBYTE(usbd_control_request_value());

	switch (desc) {
	case USB_DT_CONFIG:
		ccid_get_config_desc();
		break;
	case USB_DT_STRING:
		ccid_get_string_desc();
		break;
	default:
		usbd_endpoint_halt();
		break;
	}
}

#define ccid_set_descriptor()		usbd_endpoint_halt()

static void ccid_abort(void)
{
	uint8_t bSlot = LOBYTE(usbd_control_request_value());
	uint8_t bSeq = HIBYTE(usbd_control_request_value());

	if (bSlot < NR_SCD_SLOTS) {
		ccid_slot_abort(CCID_ABORT_CTRL, bSlot, bSeq);
	} else {
		usbd_endpoint_halt();
	}
}

static void ccid_get_clock_freqs(void)
{
	uint8_t c;
	for (c = 0; c < ifd_cf_nr_freq(); c++) {
		USBD_INL(ifd_cf_get_freq(c));
	}
}

static void ccid_get_data_rates(void)
{
	uint8_t c, d, f;
	uint32_t rate;

	for (c = 0; c < ifd_cf_nr_freq(); c++) {
		for (d = 0; d < NR_IFD_DIS; d++) {
			for (f = 0; f < NR_IFD_FIS; f++) {
				rate = ifd_cf_get_data(ifd_cf_get_freq(c), d, f);
				if (rate)
					USBD_INL(rate);
			}
		}
	}
}

static void ccid_handle_standard_request(void)
{
	uint8_t req = usbd_control_request_type();

	switch (req) {
	case USB_REQ_GET_DESCRIPTOR:
		ccid_get_descriptor();
		break;
	case USB_REQ_SET_DESCRIPTOR:
		ccid_set_descriptor();
		break;
	default:
		usbd_endpoint_halt();
	}
}

static void ccid_handle_class_request(void)
{
	uint8_t req = usbd_control_request_type();

	scd_debug(SCD_DEBUG_CS_REQ, req);

	switch (req) {
	case CCID_REQ_ABORT:
		ccid_abort();
		break;
	case CCID_REQ_GET_CLOCK_FREQS:
		ccid_get_clock_freqs();
		break;
	case CCID_REQ_GET_DATA_RATES:
		ccid_get_data_rates();
		break;
	default:
		usbd_endpoint_halt();
	}
}

static void ccid_handle_ctrl_data(void)
{
	uint8_t type = usbd_control_setup_type();
	uint8_t recp = usbd_control_setup_recp();

	switch (recp) {
	case USB_RECP_DEVICE:
		switch (type) {
		case USB_TYPE_STANDARD:
			ccid_handle_standard_request();
			return;
		}
		break;
	case USB_RECP_INTERFACE:
		switch (type) {
		case USB_TYPE_STANDARD:
			ccid_handle_standard_request();
			return;
		case USB_TYPE_CLASS:
			ccid_handle_class_request();
			return;
		}
		break;
	}
	usbd_endpoint_halt();
}

void ccid_start(void)
{
	ccid_intr_start();
	ccid_display_default();
}

#define CCID_FUNC_T1	0x00000001
#define CCID_FUNC_SPE	0x00000002
#define CCID_FUNC_TPDU	0x00000004
#define CCID_FUNC_APDU	0x00000008
#define CCID_FUNC_CLS	0x00000010
#define CCID_FUNC_ATR	0x00000020
#define CCID_FUNC_PPS	0x00000040

void ccid_devid_init(void)
{
#ifdef CONFIG_IFD_T1
	DEVICE_FUNC(CCID_FUNC_T1);
#endif
#ifdef CONFIG_CCID_SECURE
	DEVICE_FUNC(CCID_FUNC_SPE);
#endif
#ifdef CONFIG_IFD_XCHG_APDU
	DEVICE_FUNC(CCID_FUNC_APDU);
#endif
#ifdef CONFIG_IFD_XCHG_TPDU
	DEVICE_FUNC(CCID_FUNC_TPDU);
#endif
#ifdef CONFIG_IFD_AUTO_CLASS_SELECT
	DEVICE_FUNC(CCID_FUNC_CLS);
#endif
#ifdef CONFIG_IFD_AUTO_RESET
	DEVICE_FUNC(CCID_FUNC_ATR);
#endif
#ifdef CONFIG_IFD_AUTO_INFO_XCHG
	DEVICE_FUNC(CCID_FUNC_PPS);
#endif
}

usbd_endpoint_t ccid_endpoint_in = {
	USBD_ENDP_BULK_IN,
	CCID_ENDP_INTERVAL_IN,
	ccid_submit_response,
	ccid_handle_response,
	ccid_complete_response,
};

usbd_endpoint_t ccid_endpoint_out = {
	USBD_ENDP_BULK_OUT,
	CCID_ENDP_INTERVAL_OUT,
	ccid_submit_command,
	ccid_handle_command,
	ccid_complete_command,
};

usbd_interface_t usb_ccid_interface = {
	CCID_STRING_FIRST,
	CCID_STRING_LAST,
	NR_SCD_ENDPS,
	ccid_config_length,
	ccid_handle_ctrl_data,
};

#ifdef CONFIG_CCID_SECURE
#define CCID_INTERFACE_POWER	100
#else
#define CCID_INTERFACE_POWER	50
#endif

void ccid_init(void)
{
	ccid_qid_t qid;

	ccid_devid_init();

	usbd_declare_interface(CCID_INTERFACE_POWER,
			       &usb_ccid_interface);

	ifd_register_handlers(ccid_handle_iso7816_intr,
			      ccid_handle_iso7816_cmpl);

	/* XXX: CCID Endpoints Ordering
	 *
	 * IN endpoint should always be handled prior to OUT endpoint.
	 *
	 * Consider an exchange flow as follows:
	 * OUT -> PC2RDR -> ISO7816 -> RDR2PC -> IN
	 * Finally, txcmpl interrupt on IN may raise.
	 * Unfortunately, rxaval on OUT may also raise meanwhile, be
	 * aware of that host is running faster than the device.
	 * If CCID_ADDR_OUT < CCID_ADDR_IN, the rxaval will be handled
	 * first as USB layer will traverse endpoint event queue from
	 * lower EID to higher EID.
	 * Result is that we will return SLOT_BUSY to the second request!
	 * If host hasn't any retry mechanisms, it fails on this command.
	 *
	 * A reasonable solution to solve this is to add priority support
	 * for endpoints in USB layer, as this will add size overhead to
	 * the firmware, we do not implement in this way.  Just keep it in
	 * mind that endpoints have natural priority that ordered by their
	 * EID.  So we make IN endpoint registered before OUT endpoint's
	 * registration like follows.
	 */
	CCID_ADDR_IN = usbd_claim_endpoint(true, &ccid_endpoint_in);
	CCID_ADDR_OUT = usbd_claim_endpoint(true, &ccid_endpoint_out);
	for (qid = 0; qid < NR_SCD_QUEUES; qid++) {
		ccid_slot_reset(qid);
	}
	ccid_intr_init();
	ccid_spe_init();
	ccid_start();
	scd_Escape_init();
}
