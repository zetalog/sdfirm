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
 * @(#)scd_ccid.c: circuit chip interface device functions
 * $Id: scd_ccid.c,v 1.135 2011-10-26 10:09:44 zhenglv Exp $
 */

/* TODO:
 *   1. PC2RDR_Escape support
 *   2. SPE timeout validation support
 *   3. multiple clock frequencies support
 */
#include <target/usb_scd.h>
#include <target/lcd.h>
#include <target/kbd.h>

#define CCID_XCHG_IS_LEVEL(xchg)	\
	((ccid_device_features() & SCD_FEATURE_XCHG_MASK) == xchg)

#define CCID_ADDR_IN			ccid_addr[SCD_ENDP_BULK_IN]
#define CCID_ADDR_OUT			ccid_addr[SCD_ENDP_BULK_OUT]
#define CCID_ADDR_IRQ			ccid_addr[SCD_ENDP_INTR_IN]

#define CCID_QID_OUT			(NR_SCD_QUEUES-1)
#define CCID_QID_IN			ccid_seq_queue[0]

static void ccid_slot_abort(uint8_t type, scd_sid_t sid, uint8_t seq);
static void ccid_handle_iso7816_cmpl(void);

/* Multiple CCID Slot support
 *
 *                      +----------+        +----------+   |  slot
 *                      | Bulk OUT |        | Bulk IN  |   | states
 *                      +----------+        +----+-----+   |
 *                           \|/                /|\        +=========+
 *  +--------------+/ +-------+--------+         |         |         |
 *  |   cmds[sid]  +--|  cmds[OUT_QID] |         |         |         |
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
 * scd_qid    0 ~ NR_SCD_QUEUES (0 ~ NR_SCD_SLOTS+1)
 * scd_sid    0 ~ NR_SCD_SLOTS
 * ccid_seq_t 0 ~ NR_SCD_QUEUES (seq_queue index)
 * seq_queue: USB request queue, only queue[0] is sensitive for BULK IN
 */
/*
 * CCID state machine is implemented by 4 hooks:
 *
 * poll
 * 	(OUT) scd_submit_command
 * 	  usbd_request_submit(OUT) (OUT_QID is PC2RDR)
 * 	    -> DATA
 * 
 * 	(IN) scd_submit_response
 * 	  usbd_request_submit(IN) (nr_seqs > 0 && IN_QID is RDR2PC)
 * 	    -> DATA
 * 
 * iocb
 * 	(OUT) usbd_request_commit
 * 
 * done
 * 	(OUT) usbd_request_handled
 * 	(OUT) scd_slot_enter(ISO7816)
 * 	  icc/scs_xxx
 * 	(OUT) scd_slot_enter(RDR2PC)
 * 	  ccid_enqueue
 * 	    scd_submit_response
 * 	      usbd_request_submit(IN) (is IN_QID)
 * 	        -> DATA
 * 
 * 	(IN) scd_slot_enter(PC2RDR)
 * 	  ccid_dequeue
 * 	    scd_submit_command
 * 	      usbd_request_submit(OUT) (is OUT_QID)
 * 	        -> DATA
 * 	    scd_submit_response
 * 	      usbd_request_submit(IN) (is IN_QID)
 * 	        -> DATA
 * 
 * 7816_cmpl
 * 	scd_slot_enter(RDR2PC)
 * 	  ccid_enqueue
 * 	    scd_submit_response
 * 	      usbd_request_submit(IN) (is IN_QID)
 * 	        -> DATA
 */
__near__ struct ccid_abort ccid_aborts[NR_SCD_QUEUES];
__near__ scd_sid_t ccid_seq_queue[NR_SCD_QUEUES];
__near__ ccid_seq_t ccid_nr_seqs = 0;

__near__ uint8_t ccid_addr[NR_SCD_ENDPS];

/*=========================================================================
 * ISO7816 translator
 *=======================================================================*/
void scd_sid_select(scd_sid_t sid)
{
	scd_qid_select(sid);
	BUG_ON(sid >= NR_SCD_SLOTS);
	ifd_sid_select(sid);
}

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

uint8_t scd_slot_error(scs_err_t err)
{
	switch (err) {
	case SCS_ERR_UNSUPPORT:
		return SCD_ERROR_CMD_UNSUPPORT;
	case SCS_ERR_OVERRUN:
		return SCD_ERROR_XFR_OVERRUN;
	case SCS_ERR_TIMEOUT:
	case SCS_ERR_NOTPRESENT:
		return SCD_ERROR_ICC_MUTE;
	case SCS_ERR_BUSY_SLOT:
		return CCID_ERROR_CMD_SLOT_BUSY;
	case SCS_ERR_BUSY_AUTO:
		return CCID_ERROR_BUSY_AUTO_SEQ;
	case SCS_ERR_ABORTED:
		return CCID_ERROR_CMD_ABORTED;
	case SCS_ERR_PARITY_ERR:
		return CCID_ERROR_XFR_PARITY_ERROR;
	case IFD_ERR_BAD_PPSS:
	case IFD_ERR_BAD_PCK:
		return CCID_ERROR_XFR_PARITY_ERROR;
	case IFD_ERR_BAD_TCK:
		return CCID_ERROR_BAD_ATR_TCK;
	case IFD_ERR_BAD_TS:
		return CCID_ERROR_BAD_ATR_TS;
	case IFD_ERR_BAD_PROTO:
		return CCID_ERROR_PROTO_DEACTIVATE;
	default:
		return SCD_ERROR_HW_ERROR;
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
	scd_qid_t qid = scd_qid;

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
 * bulk endpoints
 *=======================================================================*/
uint8_t scd_resp_message(void)
{
	switch (scd_cmds[scd_qid].bMessageType) {
	case SCD_PC2RDR_ICCPOWERON:
	case SCD_PC2RDR_XFRBLOCK:
	case CCID_PC2RDR_SECURE:
		return SCD_RDR2PC_DATABLOCK;
	case SCD_PC2RDR_ICCPOWEROFF:
	case SCD_PC2RDR_GETSLOTSTATUS:
	case CCID_PC2RDR_T0APDU:
	case CCID_PC2RDR_ICCCLOCK:
	case CCID_PC2RDR_MECHANICAL:
	case CCID_PC2RDR_ABORT:
		return SCD_RDR2PC_SLOTSTATUS;
	case SCD_PC2RDR_ESCAPE:
		return SCD_RDR2PC_ESCAPE;
#ifdef CONFIG_IFD_AUTO_PPS_PROP
	case CCID_PC2RDR_SETPARAMETERS:
	case SCD_PC2RDR_GETPARAMETERS:
	case CCID_PC2RDR_RESETPARAMETERS:
	case CCID_PC2RDR_SETDATAANDFREQ:
		return SCD_RDR2PC_SLOTSTATUS;
#else
	case CCID_PC2RDR_SETPARAMETERS:
	case SCD_PC2RDR_GETPARAMETERS:
	case CCID_PC2RDR_RESETPARAMETERS:
		return CCID_RDR2PC_PARAMETERS;
	case CCID_PC2RDR_SETDATAANDFREQ:
		return CCID_RDR2PC_DATARATEANDCLOCK;
#endif
	}
	return SCD_RDR2PC_SLOTSTATUS;
}

/*=========================================================================
 * CCID slots
 *=======================================================================*/
void scd_queue_reset(scd_qid_t qid)
{
	ccid_aborts[qid].aborting = CCID_ABORT_NONE;
	__scd_queue_reset(qid);
}

static void ccid_slot_abort(uint8_t type, scd_sid_t sid, uint8_t seq)
{
	scd_qid_t sqid;
	boolean aborting = false;
	scs_err_t err = SCS_ERR_SUCCESS;

	sqid = scd_qid_save(sid);

	switch (type) {
	case CCID_ABORT_CTRL:
		ccid_aborts[sid].ctrl_seq = seq;
		scd_debug(SCD_DEBUG_ABORT, 0);
		break;
	case CCID_ABORT_BULK:
		ccid_aborts[sid].bulk_seq = seq;
		scd_debug(SCD_DEBUG_ABORT, 1);
		break;
	}
	ccid_aborts[sid].aborting |= type;

	if (ccid_aborts[sid].aborting == (CCID_ABORT_CTRL | CCID_ABORT_BULK) &&
	    ccid_aborts[sid].bulk_seq == ccid_aborts[sid].ctrl_seq) {
		scd_debug(SCD_DEBUG_ABORT, 4);
		ccid_kbd_abort();
		scd_debug(SCD_DEBUG_ABORT, 5);
		ifd_xchg_abort(sid);
		scd_debug(SCD_DEBUG_ABORT, 2);
		aborting = true;
		ccid_aborts[sid].aborting = CCID_ABORT_NONE;
	}
	scd_qid_restore(sqid);

	if (type == CCID_ABORT_BULK) {
		scd_debug(SCD_DEBUG_ABORT, 3);
		if (!aborting)
			err = SCS_ERR_ABORTED;
		scd_ScsSequence_cmp(err, false);
	}
}

static void ccid_enqueue(scd_qid_t qid)
{
	ccid_seq_queue[ccid_nr_seqs] = qid;
	ccid_nr_seqs++;
	BUG_ON(ccid_nr_seqs > NR_SCD_QUEUES);

	if (qid == CCID_QID_IN) {
		scd_submit_response();
	}
}

static void ccid_dequeue(scd_qid_t qid)
{
	ccid_seq_t i;

	BUG_ON(qid >= INVALID_SCD_QID);

	if (ccid_nr_seqs > 0 && CCID_QID_IN == qid) {
		/* remove top one */
		scd_queue_reset(CCID_QID_IN);
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
		scd_submit_response();
	}
	if (CCID_QID_OUT == qid) {
		scd_submit_command();
	}
}

void scd_slot_enter(uint8_t state)
{
	if (scd_states[scd_qid] != state) {
		scd_debug(SCD_DEBUG_STATE, state);
		scd_states[scd_qid] = state;
	}
	if (state == SCD_SLOT_STATE_RDR2PC) {
		ccid_enqueue(scd_qid);
	}
	if (state == SCD_SLOT_STATE_PC2RDR) {
		ccid_dequeue(scd_qid);
		scd_resps[scd_qid].abRFU3 = 0;
	}
}

void scd_submit_response(void)
{
	if (ccid_nr_seqs > 0) {
		__scd_submit_response(CCID_ADDR_IN, CCID_QID_IN);
	}
}

void scd_submit_command(void)
{
	__scd_submit_command(CCID_ADDR_OUT, CCID_QID_OUT);
}

/*=========================================================================
 * bulk-out data
 *=======================================================================*/
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
	USBD_OUTB(scd_cmd_data.t0.bmFindexDindex);
	USBD_OUTB(scd_cmd_data.t0.bmTCCKST0);
	USBD_OUTB(scd_cmd_data.t0.bGuardTimeT0);
	USBD_OUTB(scd_cmd_data.t0.bWaitingIntegerT0);
	USBD_OUTB(scd_cmd_data.t0.bClockStop);
	scd_resps[scd_qid].abRFU3 = SCD_PROTOCOL_T0;
	ccid_Parameters_out();
}

#ifdef CONFIG_IFD_T1
static void ccid_SetParametersT1_out(void)
{
	USBD_OUTB(scd_cmd_data.t1.bmFindexDindex);
	USBD_OUTB(scd_cmd_data.t1.bmTCCKST1);
	USBD_OUTB(scd_cmd_data.t1.bGuardTimeT1);
	USBD_OUTB(scd_cmd_data.t1.bWaitingIntegerT1);
	USBD_OUTB(scd_cmd_data.t1.bClockStop);
	USBD_OUTB(scd_cmd_data.t1.bIFSC);
	USBD_OUTB(scd_cmd_data.t1.bNadValue);
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
			scd_ScsSequence_cmp(err, false);
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
			err = ifd_set_t0_param(scd_cmd_data.t0.bmFindexDindex,
					       scd_cmd_data.t0.bGuardTimeT0,
					       scd_cmd_data.t0.bWaitingIntegerT0,
					       scd_cmd_data.t0.bClockStop);
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
			err = ifd_set_t1_param(scd_cmd_data.t1.bmFindexDindex,
					       scd_cmd_data.t1.bmTCCKST1,
					       scd_cmd_data.t1.bGuardTimeT1,
					       scd_cmd_data.t1.bWaitingIntegerT1,
					       scd_cmd_data.t1.bClockStop,
					       scd_cmd_data.t1.bIFSC,
					       scd_cmd_data.t1.bNadValue);
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
	USBD_OUTL(scd_cmd_data.fd.dwClockFrequency);
	USBD_OUTL(scd_cmd_data.fd.dwDataRate);
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
		scd_ScsSequence_cmp(err, false);
	}
}

static void ccid_SetDataAndFreq_cmp(void)
{
	scs_err_t err;

	err = ifd_set_freq_data(scd_cmd_data.fd.dwClockFrequency,
				scd_cmd_data.fd.dwDataRate);
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
#define ccid_GetParameters_out()	scd_CmdFailure_out(SCD_ERROR_CMD_UNSUPPORT)

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
#define ccid_SetParameters_out()	scd_CmdFailure_out(SCD_ERROR_CMD_UNSUPPORT)
#define ccid_SetDataAndFreq_out()	scd_CmdFailure_out(SCD_ERROR_CMD_UNSUPPORT)
#endif

void scd_handle_bulk_pc2rdr(void)
{
	switch (scd_cmds[scd_qid].bMessageType) {
	case CCID_PC2RDR_SECURE:
		ccid_Secure_out();
		break;
	case CCID_PC2RDR_SETPARAMETERS:
		ccid_SetParameters_out();
		break;
	case CCID_PC2RDR_SETDATAANDFREQ:
		ccid_SetDataAndFreq_out();
		break;
	case CCID_PC2RDR_RESETPARAMETERS:
	case CCID_PC2RDR_T0APDU:
#ifdef CONFIG_IFD_CLOCK_CONTROL
	case CCID_PC2RDR_ICCCLOCK:
#endif
#ifdef CONFIG_IFD_MECHA_CONTROL
	case CCID_PC2RDR_MECHANICAL:
#endif
		/* nothing to do */
		break;
	default:
		scd_handle_pc2rdr_default();
		break;
	}
}

void scd_handle_command(void)
{
	__scd_handle_command(CCID_QID_OUT);
}

/*=========================================================================
 * bulk-in data
 *=======================================================================*/
void scd_handle_bulk_rdr2pc(void)
{
	switch (scd_cmds[scd_qid].bMessageType) {
#ifdef CONFIG_IFD_CLOCK_CONTROL
	case CCID_PC2RDR_ICCCLOCK:
#endif
#ifdef CONFIG_IFD_MECHA_CONTROL
	case CCID_PC2RDR_MECHANICAL:
#endif
	case CCID_PC2RDR_T0APDU:
		scd_SlotStatus_in();
		break;
	case SCD_PC2RDR_GETPARAMETERS:
	case CCID_PC2RDR_RESETPARAMETERS:
	case CCID_PC2RDR_SETPARAMETERS:
		ccid_Parameters_in();
		break;
	case CCID_PC2RDR_SETDATAANDFREQ:
		ccid_DataAndFreq_in();
		break;
	case CCID_PC2RDR_SECURE:
		scd_DataBlock_in();
		break;
	case CCID_PC2RDR_ABORT:
		BUG();
		break;
	default:
		scd_handle_rdr2pc_default();
		break;
	}
}

void scd_handle_response(void)
{
	BUG_ON(ccid_nr_seqs == 0);
	__scd_handle_response(CCID_QID_IN);
}

void scd_complete_response(void)
{
	BUG_ON(ccid_nr_seqs > NR_SCD_QUEUES);
	if (ccid_nr_seqs > 0) {
		__scd_complete_response(CCID_QID_IN);
	}
}

/*=========================================================================
 * bulk-out cmpl
 *=======================================================================*/
#ifdef CONFIG_IFD_CLOCK_CONTROL
static void ccid_IccClock_cmp(void)
{
	uint8_t bClockCommand = scd_cmds[scd_qid].abRFU[0];
	scs_err_t err;

	switch (bClockCommand) {
	case CCID_CLOCK_RESTART:
		scd_slot_enter(SCD_SLOT_STATE_ISO7816);
		err = ifd_restart_clock();
		scd_ScsSequence_cmp(err, false);
		break;
	case CCID_CLOCK_STOP:
		scd_slot_enter(SCD_SLOT_STATE_ISO7816);
		err = ifd_stop_clock();
		scd_ScsSequence_cmp(err, false);
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
		scd_ScsSequence_cmp(err, false);
		break;
	case CCID_MECHA_UNLOCK_CARD:
		scd_slot_enter(SCD_SLOT_STATE_ISO7816);
		err = ifd_unlock_card();
		scd_ScsSequence_cmp(err, false);
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

#ifdef CONFIG_IFD_T0_APDU
static void ccid_T0APDU_cmp(void)
{
	scs_err_t err;

	err = ifd_set_t0_apdu(scd_cmds[scd_qid].abRFU[0],
			      scd_cmds[scd_qid].abRFU[1],
			      scd_cmds[scd_qid].abRFU[2]);
	scd_ScsSequence_cmp(err, false);
}
#endif

#ifdef CONFIG_CCID_DISCARD
static void ccid_discard(void)
{
	ccid_seq_t i, j;
	scd_qid_t qid;

	/* discard running response with matched SID */
	/* discard pending responses with matched SID */
	for (i = 1, j = 1; i < ccid_nr_seqs; i++) {
		qid = ccid_seq_queue[i];
		/* new seq should be VALID */
		BUG_ON(qid > NR_SCD_QUEUES-1);
		if (scd_cmds[scd_qid].bSlot != scd_qid) {
			ccid_seq_queue[j++] = qid;
		} else {
			scd_qid_t sqid;
			ccid_nr_seqs--;
			sqid = scd_qid_save(CCID_QID_OUT);
			scd_slot_enter(SCD_SLOT_STATE_PC2RDR);
			scd_qid_restore(sqid);
		}
	}

	BUG_ON(ccid_nr_seqs > NR_SCD_QUEUES);

	/* allow host sending commands, this should be called before
	 * scd_submit_response
	 */
	if (CCID_QID_IN == scd_qid) {
		/* usbd_request_discard_addr(CCID_ADDR_OUT); */
		usbd_request_discard_addr(CCID_ADDR_IN);
	}
}
#else
#define ccid_discard()
#endif

void scd_complete_bulk_pc2rdr(void)
{
	switch (scd_cmds[scd_qid].bMessageType) {
	case CCID_PC2RDR_SECURE:
		ccid_Secure_cmp();
		break;
	case CCID_PC2RDR_SETPARAMETERS:
		ccid_SetParameters_cmp();
		break;
	case SCD_PC2RDR_GETPARAMETERS:
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
	case SCD_PC2RDR_GETSLOTSTATUS:
	case CCID_PC2RDR_ABORT:
		/* handled before */
		BUG();
		break;
	default:
		scd_complete_pc2rdr_default();
		break;
	}
}

void scd_complete_command(void)
{
	__scd_complete_command(CCID_QID_OUT);
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
	scd_XfrBlock_cmp();
}

static void ccid_BadInsByte_out(void)
{
	SCD_XB_ERR = SCS_ERR_PARITY_ERR;
}

static void ccid_PinInsByte_out(void)
{
	uint8_t ins = scd_read_byte(1);
	scd_debug(SCD_DEBUG_INS, ins);
	switch (scd_cmd_data.po.bPINOpeartion) {
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
		__scd_XfrBlock_out(SCD_HEADER_SIZE + base_size,
				   scd_cmds[scd_qid].dwLength - base_size);
		if (usbd_request_handled() >= (SCD_HEADER_SIZE + base_size + 2)) {
			ccid_PinInsByte_out();
		}
	}
}

static void ccid_PinMessage_out(void)
{
	USBD_OUTW(scd_cmd_data.po.wLangId);
	USBD_OUTB(scd_cmd_data.po.bMsgIndex[0]);
	if (ccid_spe_msg_number() == 0)
		scd_cmd_data.po.bMsgIndex[0] = 0xFF;
	if (ccid_spe_msg_number() > 1)
		USBD_OUTB(scd_cmd_data.po.bMsgIndex[1]);
	else
		scd_cmd_data.po.bMsgIndex[1] = 0xFF;
	if (ccid_spe_msg_number() > 2)
		USBD_OUTB(scd_cmd_data.po.bMsgIndex[2]);
	else
		scd_cmd_data.po.bMsgIndex[2] = 0xFF;
}

static void ccid_PinFormat_out(void)
{
	USBD_OUTB(scd_cmd_data.po.bTimeout);
	USBD_OUTB(scd_cmd_data.po.bmFormatString);
	USBD_OUTB(scd_cmd_data.po.bmPINBlockString);
	USBD_OUTB(scd_cmd_data.po.bmPINLengthFormat);
}

static void ccid_PinTeoPrologue_out(void)
{
	USBD_OUTB(scd_cmd_data.po.bTeoPrologue[0]);
	USBD_OUTB(scd_cmd_data.po.bTeoPrologue[1]);
	USBD_OUTB(scd_cmd_data.po.bTeoPrologue[2]);
}

static boolean ccid_EntryValidaton_ok(void)
{
	if (!scd_cmd_data.po.bEntryValidationCondition) {
		SCD_XB_ERR = SCS_ERR_TIMEOUT;
		return false;
	}
	return true;
}

static void ccid_PinVerify_out(void)
{
	ccid_PinFormat_out();
	USBD_OUTW(scd_cmd_data.po.wPINMaxExtraDigit);
	USBD_OUTB(scd_cmd_data.po.bEntryValidationCondition);
	USBD_OUTB(scd_cmd_data.po.bNumberMessage);
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
	USBD_OUTB(scd_cmd_data.pm.bInsertionOffsetOld);
	USBD_OUTB(scd_cmd_data.pm.bInsertionOffsetNew);
	USBD_OUTW(scd_cmd_data.po.wPINMaxExtraDigit);
	USBD_OUTB(scd_cmd_data.pm.bConfirmPIN);
	USBD_OUTB(scd_cmd_data.po.bEntryValidationCondition);
	USBD_OUTB(scd_cmd_data.po.bNumberMessage);
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
	/* TODO: make sure SCD_XB_NC is sufficient to contain PINs */
	if (scd_slot_success(SCD_XB_ERR)) {
		/* TODO: call scd_CmdOffset_cmp(); */
		ccid_spe_operate_init();
	}
}

static void ccid_PinModify_cmp(void)
{
	/* TODO: make sure SCD_XB_NC is sufficient to contain PINs */
	if (scd_slot_success(SCD_XB_ERR)) {
		/* TODO: call scd_CmdOffset_cmp(); */
		ccid_spe_operate_init();
	}
}

void ccid_Secure_out(void)
{
	USBD_OUTB(scd_cmd_data.po.bPINOpeartion);
	if (usbd_request_handled() >= SCD_HEADER_SIZE+1) {
		switch (scd_cmd_data.po.bPINOpeartion) {
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
	switch (scd_cmd_data.po.bPINOpeartion) {
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
	DEVICE_FUNC(SCD_FUNC_SECURE);
}
#endif

/*=========================================================================
 * interrupt endpoint
 *=======================================================================*/
#ifdef CONFIG_SCD_INTERRUPT
/*=========================================================================
 * slot changes
 *=======================================================================*/
boolean scd_present_changed(void)
{
	return __scd_present_changed_all();
}

void scd_handle_present(void)
{
	__scd_handle_present_all();
}

void scd_discard_present(void)
{
	scd_sid_t sid;
	for (sid = 0; sid < NR_SCD_SLOTS; sid++) {
		__scd_discard_present_sid(sid);
	}
}

void scd_submit_present(void)
{
	scd_sid_t sid;
	for (sid = 0; sid < NR_SCD_SLOTS; sid++) {
		__scd_submit_present_sid(sid);
	}
}

/*=========================================================================
 * hardware errors
 *=======================================================================*/
#ifdef CONFIG_CCID_INTERRUPT_HWERR
__near__ struct ccid_hwerr ccid_intr_hwerrs[NR_SCD_SLOTS];
__near__ scd_sid_t ccid_hwerr_sid;

static boolean __ccid_hwerr_running(scd_sid_t sid)
{
	return ccid_intr_hwerrs[sid].bState & CCID_HWERR_RUNNING;
}

static boolean __ccid_hwerr_pending(scd_sid_t sid)
{
	return ccid_intr_hwerrs[sid].bState & CCID_HWERR_PENDING;
}

static scd_sid_t ccid_hwerr_pending_sid(void)
{
	scd_sid_t sid;
	for (sid = 0; sid < NR_SCD_SLOTS; sid++) {
		if (__ccid_hwerr_pending(sid))
			return scd_sid_t;
	}
	return INVALID_SCD_SID;
}

static void ccid_hwerr_raise_irq(uint8_t err)
{
	if (scd_qid < NR_SCD_SLOTS) {
		ccid_intr_hwerrs[scd_qid].bState |= CCID_HWERR_PENDING;
		ccid_intr_hwerrs[scd_qid].bPendingSeq = scd_cmds[scd_qid].bSeq;
		ccid_intr_hwerrs[scd_qid].bPendingCode |= err;
	}
}

static void ccid_hwerr_handle_irq(void)
{
	USBD_INB(CCID_RDR2PC_HARDWAREERROR);
	USBD_INB(ccid_hwerr_sid);
	USBD_INB(ccid_intr_hwerrs[ccid_hwerr_sid].bSeq);
	USBD_INB(ccid_intr_hwerrs[ccid_hwerr_sid].bHardwareErrorCode);
}

static void ccid_hwerr_discard_irq(void)
{
	ccid_intr_hwerrs[ccid_hwerr_sid].bState &= ~CCID_HWERR_RUNNING;
	ccid_hwerr_sid = INVALID_SCD_SID;
}

static void ccid_hwerr_submit_irq(scd_sid_t sid)
{
	ccid_intr_hwerrs[sid].bState &= ~CCID_HWERR_PENDING;
	ccid_intr_hwerrs[sid].bState |= CCID_HWERR_RUNNING;
	ccid_hwerr_sid = sid;
}

static void ccid_hwerr_init_irq(void)
{
	scd_sid_t sid;
	for (sid = 0; sid < NR_SCD_SLOTS; sid++) {
		ccid_intr_hwerrs[sid].bState = 0;
		ccid_intr_hwerrs[sid].bSeq = 0;
	}
	ccid_hwerr_sid = INVALID_SCD_SID;
}
#else
#define ccid_hwerr_pending_sid()		INVALID_SCD_SID
#define ccid_hwerr_submit_irq(sid)
#define ccid_hwerr_discard_irq()
#define ccid_hwerr_handle_irq()
#define ccid_hwerr_raise_irq(err)
#define ccid_hwerr_sid				INVALID_SCD_SID
#define ccid_hwerr_init_irq()
#endif

/*=========================================================================
 * generic interupts
 *=======================================================================*/
void scd_discard_interrupt(void)
{
	if (ccid_hwerr_sid < NR_SCD_SLOTS)
		ccid_hwerr_discard_irq();
	else
		scd_discard_present();
}

void scd_handle_interrupt(void)
{
	if (ccid_hwerr_sid < NR_SCD_SLOTS)
		ccid_hwerr_handle_irq();
	else
		scd_handle_present();
}

void scd_submit_interrupt(void)
{
	scd_sid_t sid = ccid_hwerr_pending_sid();
	if (sid < NR_SCD_SLOTS) {
		if (usbd_request_submit(CCID_ADDR_IRQ,
					CCID_IRQ_HWERR_SIZE)) {
			ccid_hwerr_submit_irq(sid);
		}
	} else {
		__scd_submit_interrupt(CCID_ADDR_IRQ);
	}
}

void scd_irq_init(void)
{
	ccid_hwerr_init_irq();
}
#endif

#if NR_SCD_QUEUES != NR_SCD_SLOTS
boolean scd_abort_responded(void)
{
	BUG_ON(scd_qid >= NR_SCD_QUEUES);
	if (scd_cmds[scd_qid].bMessageType == CCID_PC2RDR_ABORT) {
		scd_SlotStatus_in();
		return true;
	}
	return false;
}

boolean scd_abort_completed(void)
{
	if (scd_cmds[scd_qid].bMessageType == CCID_PC2RDR_ABORT) {
		ccid_Abort_cmp();
		return true;
	}
	return false;
}

boolean scd_abort_requested(void)
{
	scd_sid_t sid;

	sid = scd_cmds[scd_qid].bSlot;
	if (scd_cmds[scd_qid].bMessageType == CCID_PC2RDR_ABORT) {
		return true;
	}
	if (ccid_aborts[sid].aborting != CCID_ABORT_NONE) {
		scd_CmdFailure_out(CCID_ERROR_CMD_ABORTED);
		return true;
	}
	if (scd_states[sid] != SCD_SLOT_STATE_PC2RDR) {
		scd_CmdFailure_out(CCID_ERROR_CMD_SLOT_BUSY);
		return true;
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
	return false;
}
#endif

/*=========================================================================
 * control endpoint
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

static uint8_t ccid_proto_features(void)
{
	uint8_t protocols;
#ifdef CONFIG_SCD_ESC_ACR122
#else
	protocols = (1 << SCD_PROTOCOL_T0);
#ifdef CONFIG_IFD_T1
	protocols |= (1 << SCD_PROTOCOL_T1);
#endif
#endif
	return protocols;
}

void scd_ctrl_get_desc(void)
{
	USBD_INB(SCD_DT_SCD_SIZE);
	USBD_INB(SCD_DT_SCD);
	USBD_INW(SCD_VERSION);
	USBD_INB(NR_SCD_USB_SLOTS-1);
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
	USBD_INB(NR_SCD_USB_QUEUES);
	scd_get_bulk_desc(CCID_ADDR_OUT, CCID_ADDR_IN);
	scd_get_irq_desc(CCID_ADDR_IRQ);
}

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

void scd_handle_ctrl_class(void)
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

/*=========================================================================
 * CCID entrances
 *=======================================================================*/
static void ccid_handle_iso7816_cmpl(void)
{
	scs_err_t err;

	/* slot ID is determined before cmpl is called */
	scd_qid_select(scd_sid);

	BUG_ON(scd_qid >= NR_SCD_SLOTS);
	BUG_ON(scd_states[scd_qid] != SCD_SLOT_STATE_ISO7816);

	err = ifd_xchg_get_error();

	switch (scd_cmds[scd_qid].bMessageType) {
	case SCD_PC2RDR_ICCPOWERON:
	case SCD_PC2RDR_XFRBLOCK:
	case CCID_PC2RDR_SECURE:
		scd_DataBlock_cmp(err);
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

#ifdef CONFIG_CCID_SECURE
#define CCID_INTERFACE_POWER	100
#else
#define CCID_INTERFACE_POWER	50
#endif

static void ccid_devid_init(void)
{
	scd_devid_init();
#ifdef CONFIG_IFD_XCHG_APDU
	DEVICE_FUNC(SCD_FUNC_APDU);
#endif
}

void scd_init(void)
{
	ccid_devid_init();

	ifd_register_completion(ccid_handle_iso7816_cmpl);
	usbd_declare_interface(CCID_INTERFACE_POWER,
			       &usb_scd_interface);
	scd_bulk_register(CCID_ADDR_OUT, CCID_ADDR_IN);
	scd_irq_register(CCID_ADDR_IRQ);

	scd_bulk_init();
	ccid_spe_init();
	scd_Escape_init();

	scd_irq_init();
	ccid_display_default();
}
