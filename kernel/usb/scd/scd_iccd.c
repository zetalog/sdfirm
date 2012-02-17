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
 * @(#)scd_iccd.c: integrated circuit chip devices functions
 * $Id: scd_iccd.c,v 1.90 2011-11-10 07:02:37 zhenglv Exp $
 */

#include <target/usb_scd.h>

#if NR_SCD_ENDPS > 0
uint8_t iccd_addr[NR_SCD_SLOTS][NR_SCD_ENDPS];
#endif

/*=========================================================================
 * ISO7816 translator
 *=======================================================================*/
static scd_sid_t iccd_addr2sid(uint8_t addr)
{
	scd_qid_t id;
	uint8_t i;

	for (id = 0; id < NR_SCD_SLOTS; id++) {
		for (i = 0; i < NR_SCD_ENDPS; i++) {
			if (addr == iccd_addr[scd_qid][i])
				return id;
		}
	}
	return NR_SCD_SLOTS;
}
#define iccd_addr2qid(addr)		((scd_qid_t)(iccd_addr2sid(addr)))

void scd_sid_select(scd_sid_t sid)
{
	scd_slot_select(sid);
}

uint8_t scd_slot_status(void)
{
	uint8_t state = scd_get_slot_status();

	switch (state) {
	case SCS_SLOT_STATUS_ACTIVE:
		return SCD_SLOT_STATUS_ACTIVE;
	case SCS_SLOT_STATUS_NOTPRESENT:
		return SCD_SLOT_STATUS_NOTPRESENT;
	case SCS_SLOT_STATUS_INACTIVE:
	default:
		return SCD_SLOT_STATUS_INACTIVE;
	}
}

uint8_t scd_slot_error(scs_err_t err)
{
	switch (err) {
	case SCS_ERR_OVERRUN:
		return SCD_ERROR_XFR_OVERRUN;
	case SCS_ERR_NOTPRESENT:
		return SCD_ERROR_ICC_MUTE;
	case SCS_ERR_TIMEOUT:
	case SCS_ERR_HW_ERROR:
		return SCD_ERROR_HW_ERROR;
	}
	return SCD_ERROR_HW_ERROR;
}

/*=========================================================================
 * bulk endpoints
 *=======================================================================*/
#ifdef CONFIG_SCD_BULK
#define SCD_ADDR_IN			iccd_addr[scd_qid][SCD_ENDP_BULK_IN]
#define SCD_ADDR_OUT			iccd_addr[scd_qid][SCD_ENDP_BULK_OUT]

void scd_slot_enter(uint8_t state)
{
	if (scd_states[scd_qid] != state) {
		scd_debug(SCD_DEBUG_STATE, state);
		scd_states[scd_qid] = state;
	}
	if (state == SCD_SLOT_STATE_PC2RDR) {
		scd_resps[scd_qid].abRFU3 = 0;
	}
}

uint8_t scd_resp_message(void)
{
	switch (scd_cmds[scd_qid].bMessageType) {
	case SCD_PC2RDR_ICCPOWERON:
	case SCD_PC2RDR_XFRBLOCK:
		return SCD_RDR2PC_DATABLOCK;
	case SCD_PC2RDR_ICCPOWEROFF:
		return SCD_RDR2PC_SLOTSTATUS;
	case SCD_PC2RDR_ESCAPE:
		return SCD_RDR2PC_ESCAPE;
	case SCD_PC2RDR_SETPARAMETERS:
	case SCD_PC2RDR_GETPARAMETERS:
		return SCD_RDR2PC_PARAMETERS;
	}
	return SCD_RDR2PC_SLOTSTATUS;
}

void scd_queue_reset(scd_qid_t qid)
{
	__scd_queue_reset(qid);
}

static uint8_t iccd_Parameter_size(uint8_t proto)
{
	switch (proto) {
	case SCS_PROTO_T0:
		return sizeof (struct scd_t0_param);
		break;
	case SCS_PROTO_T1:
		return sizeof (struct scd_t1_param);
		break;
	}
	return 0;
}

static void iccd_ParametersT0_in(void)
{
	USBD_INB(0x11);	/* IFD_FD_IMPLICIT */
	USBD_INB(0x00); /* IFD_CONV_DIRECT */
	USBD_INB(0x00); /* IFD_N_IMPLICIT */
	USBD_INB(0x0A); /* IFD_T0_WI_IMPLICIT */
	USBD_INB(0x00); /* stop */
}

static void iccd_ParametersT1_in(void)
{
	USBD_INB(0x11);	/* IFD_FD_IMPLICIT */
	USBD_INB(0x00); /* IFD_CONV_DIRECT */
	USBD_INB(0x00); /* IFD_N_IMPLICIT */
	USBD_INB(0xD4); /* IFD_T1_WI_IMPLICIT */
	USBD_INB(0x00); /* stop */
	USBD_INB(0x20); /* IFD_T1_IFS_IMPLICIT */
	USBD_INB(0x00); /* nad */
}

static void iccd_SetParametersT0_out(void)
{
	USBD_OUTB(scd_cmd_data.t0.bmFindexDindex);
	USBD_OUTB(scd_cmd_data.t0.bmTCCKST0);
	USBD_OUTB(scd_cmd_data.t0.bGuardTimeT0);
	USBD_OUTB(scd_cmd_data.t0.bWaitingIntegerT0);
	USBD_OUTB(scd_cmd_data.t0.bClockStop);
}

static void iccd_SetParametersT1_out(void)
{
	USBD_OUTB(scd_cmd_data.t1.bmFindexDindex);
	USBD_OUTB(scd_cmd_data.t1.bmTCCKST1);
	USBD_OUTB(scd_cmd_data.t1.bGuardTimeT1);
	USBD_OUTB(scd_cmd_data.t1.bWaitingIntegerT1);
	USBD_OUTB(scd_cmd_data.t1.bClockStop);
	USBD_OUTB(scd_cmd_data.t1.bIFSC);
	USBD_OUTB(scd_cmd_data.t1.bNadValue);
}

static void iccd_Parameters_in(void)
{
	scd_RespHeader_in(iccd_Parameter_size(scd_resps[scd_qid].abRFU3));
	switch (scd_resps[scd_qid].abRFU3) {
	case SCS_PROTO_T0:
		iccd_ParametersT0_in();
		break;
	case SCS_PROTO_T1:
		iccd_ParametersT1_in();
		break;
	}
}

static void iccd_Parameters_out(void)
{
	__scd_CmdSuccess_out();
	scd_resps[scd_qid].dwLength = iccd_Parameter_size(scd_resps[scd_qid].abRFU3);
}

static void iccd_GetParameters_out(void)
{
	scd_resps[scd_qid].abRFU3 = scd_cmds[scd_qid].abRFU[0];
	iccd_Parameters_out();
}

static void iccd_GetParameters_cmp(void)
{
 	iccd_GetParameters_out();
	scd_CmdResponse_cmp();
}

static void iccd_SetParameters_out(void)
{
	uint8_t bProtocolNum = scd_cmds[scd_qid].abRFU[0];
	switch (bProtocolNum) {
	case SCS_PROTO_T0:
		iccd_SetParametersT0_out();
		break;
	case SCS_PROTO_T1:
		iccd_SetParametersT1_out();
		break;
	}
	scd_resps[scd_qid].abRFU3 = scd_cmds[scd_qid].abRFU[0];
	iccd_Parameters_out();
}

static void iccd_SetParameters_cmp(void)
{
	if (usbd_request_handled() <
	    (SCD_HEADER_SIZE +
	     iccd_Parameter_size(scd_cmds[scd_qid].abRFU[0]))) {
		scd_CmdOffset_cmp(1);
		return;
	}
	iccd_GetParameters_cmp();
}

void scd_handle_bulk_pc2rdr(void)
{
	switch (scd_cmds[scd_qid].bMessageType) {
	case SCD_PC2RDR_GETPARAMETERS:
		break;
	case SCD_PC2RDR_SETPARAMETERS:
		iccd_SetParameters_out();
		break;
	default:
		scd_handle_pc2rdr_default();
		break;
	}
}

void scd_handle_bulk_rdr2pc(void)
{
	switch (scd_cmds[scd_qid].bMessageType) {
	case SCD_PC2RDR_GETPARAMETERS:
	case SCD_PC2RDR_SETPARAMETERS:
		iccd_Parameters_in();
		break;
	default:
		scd_handle_rdr2pc_default();
		break;
	}
}

void scd_complete_bulk_pc2rdr(void)
{
	switch (scd_cmds[scd_qid].bMessageType) {
	case SCD_PC2RDR_SETPARAMETERS:
		iccd_SetParameters_cmp();
		break;
	case SCD_PC2RDR_GETPARAMETERS:
		iccd_GetParameters_cmp();
		break;
	default:
		scd_complete_pc2rdr_default();
		break;
	}
}

void scd_submit_response(void)
{
	__scd_submit_response(SCD_ADDR_IN,
			      iccd_addr2qid(usbd_saved_addr()));
}

void scd_submit_command(void)
{
	__scd_submit_command(SCD_ADDR_OUT,
			     iccd_addr2qid(usbd_saved_addr()));
}

void scd_handle_command(void)
{
	usbd_dump_on(SCD_DUMP_USBD);
	__scd_handle_command(iccd_addr2qid(usbd_saved_addr()));
	usbd_dump_off();
}

void scd_handle_response(void)
{
	usbd_dump_on(SCD_DUMP_USBD);
	__scd_handle_response(iccd_addr2qid(usbd_saved_addr()));
	usbd_dump_off();
}

void scd_complete_response(void)
{
	__scd_complete_response(iccd_addr2qid(usbd_saved_addr()));
}

void scd_complete_command(void)
{
	__scd_complete_command(iccd_addr2qid(usbd_saved_addr()));
}
#endif

/*=========================================================================
 * interrupt data
 *=======================================================================*/
#ifdef CONFIG_SCD_INTERRUPT
#define SCD_ADDR_IRQ			iccd_addr[scd_qid][SCD_ENDP_INTR_IN]

/*=========================================================================
 * dev changes
 *=======================================================================*/
boolean scd_present_changed(void)
{
	return __scd_present_changed_sid(iccd_addr2sid(usbd_saved_addr()));
}

void scd_handle_present(void)
{
	__scd_handle_present_sid(iccd_addr2sid(usbd_saved_addr()));
}

void scd_discard_present(void)
{
	__scd_discard_present_sid(iccd_addr2sid(usbd_saved_addr()));
}

void scd_submit_present(void)
{
	__scd_submit_present_sid(iccd_addr2sid(usbd_saved_addr()));
}

/*=========================================================================
 * generic interupts
 *=======================================================================*/
void scd_submit_interrupt(void)
{
	__scd_submit_interrupt(SCD_ADDR_IRQ);
}

void scd_handle_interrupt(void)
{
	scd_handle_present();
}

void scd_discard_interrupt(void)
{
	scd_discard_present();
}

void scd_irq_init(void)
{
}
#endif

/*=========================================================================
 * control endpoint
 *=======================================================================*/
#define ICCD_REQ_DATA_BLOCK		0x6F
#define ICCD_REQ_GET_ICC_STATUS		0xA0
#define ICCD_REQ_SLOT_STATUS		0x81

static uint32_t iccd_device_features(void)
{
	uint32_t features;
#ifdef CONFIG_ICCD_OLDVER
	features = 0x10030;
#else
	features = ICCD_FEATURE_SPECIFIC | SCD_FEATURE_AUTO_PPS_PROP;
#if SCS_APDU_MAX > 256
	features |= SCD_FEATURE_XCHG_APDU_EXT;
#else
	features |= SCD_FEATURE_XCHG_APDU;
#endif
#endif
	return features;
}

static uint8_t iccd_proto_features(void)
{
	uint8_t protocols = _BV(SCS_PROTO_T0);
	protocols |= _BV(SCS_PROTO_T1);
	return protocols;
}

#ifndef CONFIG_SCD_BULK
static void iccd_req_icc_poweron(void) 
{
}

static void iccd_req_icc_poweroff(void)
{
}

static void iccd_req_xchg_block(void)
{
}

static void iccd_req_data_block(void)
{
}

static void iccd_req_get_icc_status(void)
{
}

static void iccd_req_slot_status(void)
{
}

void scd_handle_ctrl_class(void)
{
	uint8_t req = usbd_control_request_type();

	scd_debug(SCD_DEBUG_CS_REQ, req);

	switch (req) {
	case SCD_PC2RDR_ICCPOWERON:
		iccd_req_icc_poweron();
		break;
	case SCD_PC2RDR_ICCPOWEROFF:
		iccd_req_icc_poweroff();
		break;
	case SCD_PC2RDR_XFRBLOCK:
		iccd_req_xchg_block();
		break;
	case ICCD_REQ_DATA_BLOCK:
		iccd_req_data_block();
		break;
	case ICCD_REQ_GET_ICC_STATUS:
		iccd_req_get_icc_status();
		break;
	case ICCD_REQ_SLOT_STATUS:
		iccd_req_slot_status();
		break;
	default:
		usbd_endpoint_halt();
	}
}
#else
void scd_handle_ctrl_class(void)
{
	usbd_endpoint_halt();
}
#endif

void scd_ctrl_get_desc(void)
{
	USBD_INB(SCD_DT_SCD_SIZE);
	USBD_INB(SCD_DT_SCD);
	USBD_INW(SCD_VERSION);
	USBD_INB(NR_SCD_USB_SLOTS - 1);	/* must be 0 */
	USBD_INB(SCD_VOLTAGE_5V);
	USBD_INL(iccd_proto_features());
	USBD_INL((uint32_t)(ICCD_FIXED_CLOCK));
	USBD_INL((uint32_t)(ICCD_FIXED_CLOCK));
	USBD_INB(0);
	USBD_INL((uint32_t)(ICCD_FIXED_DATARATE));
	USBD_INL((uint32_t)(ICCD_FIXED_DATARATE));
	USBD_INB(0);
	USBD_INL(ICCD_FIXED_MAX_IFSD);
	USBD_INL(SCD_SYNCH_PROTO_NONE);
	USBD_INL(SCD_MECHA_NONE);
	USBD_INL(iccd_device_features());
	USBD_INL(ICCD_MESSAGE_SIZE);
#ifdef CONFIG_ICCD_OLDVER
	USBD_INB(SCD_MUTE_APDU_CLASS);
	USBD_INB(SCD_MUTE_APDU_CLASS);
#else
	USBD_INB(SCD_ECHO_APDU_CLASS);
	USBD_INB(SCD_ECHO_APDU_CLASS);
#endif
	USBD_INW(SCD_LCD_LAYOUT_NONE);
	USBD_INB(SCD_SPE_SUPPORT_NONE);
	USBD_INB(NR_SCD_USB_QUEUES);	/* must be 1 */
	scd_get_bulk_desc(SCD_ADDR_OUT, SCD_ADDR_IN);
	scd_get_irq_desc(SCD_ADDR_IRQ);
}

/*=========================================================================
 * ICCD entrances
 *=======================================================================*/
/* TODO: how to know the slot number we should answer */
static void iccd_handle_ll_cmpl(void)
{
	scs_err_t err = scd_get_slot_error();
	BUG_ON(scd_states[scd_qid] != SCD_SLOT_STATE_RUNNING);

	switch (scd_cmds[scd_qid].bMessageType) {
	case SCD_PC2RDR_ICCPOWERON:
	case SCD_PC2RDR_XFRBLOCK:
		scd_DataBlock_cmp(err);
		break;
	case SCD_PC2RDR_ICCPOWEROFF:
		scd_SlotStatus_cmp();
		break;
	default:
		BUG();
		break;
	}
}

static void iccd_usb_register(void)
{
	scd_qid_t qid, oqid;
	for (qid = 0; qid < NR_SCD_SLOTS; qid++) {
		usbd_declare_interface(50, &usb_scd_interface);
		oqid = scd_qid_save(qid);
		scd_bulk_register(SCD_ADDR_OUT, SCD_ADDR_IN);
		scd_irq_register(SCD_ADDR_IRQ);
		scd_qid_restore(oqid);
	}
}

void iccd_devid_init(void)
{
	scd_devid_init();
	DEVICE_FUNC(SCD_FUNC_APDU);
}

void scd_init(void)
{
	iccd_devid_init();

	scd_register_completion(iccd_handle_ll_cmpl);
	iccd_usb_register();

	scd_bulk_init();
	scd_irq_init();
}
