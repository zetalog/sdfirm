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
 * @(#)usb_iccd.c: integrated circuit chip devices functions
 * $Id: usb_iccd.c,v 1.90 2011-11-10 07:02:37 zhenglv Exp $
 */

#include <target/usb_scd.h>

#define ICCD_REQ_DATA_BLOCK		0x6F
#define ICCD_REQ_GET_ICC_STATUS		0xA0
#define ICCD_REQ_SLOT_STATUS		0x81

#ifdef CONFIG_SCD_INTERRUPT
DECLARE_BITMAP(iccd_running_intrs, NR_SCD_SLOTS+NR_SCD_SLOTS);
DECLARE_BITMAP(iccd_pending_intrs, NR_SCD_SLOTS+NR_SCD_SLOTS);
#endif

#if NR_SCD_ENDPS > 0
uint8_t iccd_addr[NR_SCD_SLOTS][NR_SCD_ENDPS];
#define ICCD_ADDR_IN			iccd_addr[scd_qid][SCD_ENDP_BULK_IN]
#define ICCD_ADDR_OUT			iccd_addr[scd_qid][SCD_ENDP_BULK_OUT]
#define ICCD_ADDR_IRQ			iccd_addr[scd_qid][SCD_ENDP_INTR_IN]
#define ICCD_ENDP_INTERVAL_INTR		0x7F
#define ICCD_ENDP_INTERVAL_NONE		0x00
#define ICCD_ENDP_INTERVAL_IN		0x01
#define ICCD_ENDP_INTERVAL_OUT		0x01
#endif

#define ICCD_STRING_FIRST	0x10
#define ICCD_STRING_INTERFACE	ICCD_STRING_FIRST+0
#define ICCD_STRING_LAST	ICCD_STRING_INTERFACE

#ifdef CONFIG_ICCD_COS
uint8_t scd_slot_status(void)
{
	return SCD_SLOT_STATUS_ACTIVE;
}

uint8_t scd_slot_error(scs_err_t err)
{
	switch (err) {
	case SCS_ERR_OVERRUN:
		return ICCD_ERROR_XFR_OVERRUN;
	case SCS_ERR_NOTPRESENT:
		return ICCD_ERROR_ICC_MUTE;
	case SCS_ERR_TIMEOUT:
	case SCS_ERR_HW_ERROR:
		return ICCD_ERROR_HW_ERROR;
	}
	return ICCD_ERROR_HW_ERROR;
}

void scd_sid_select(scd_sid_t sid)
{
}
#else
uint8_t scd_slot_status(void)
{
	uint8_t state = scs_get_slot_status();

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
		return ICCD_ERROR_XFR_OVERRUN;
	case SCS_ERR_NOTPRESENT:
		return ICCD_ERROR_ICC_MUTE;
	case SCS_ERR_TIMEOUT:
	case SCS_ERR_HW_ERROR:
		return ICCD_ERROR_HW_ERROR;
	}
	return ICCD_ERROR_HW_ERROR;
}

void scd_sid_select(scd_sid_t sid)
{
	scd_qid_select(sid);
	BUG_ON(sid >= NR_SCD_SLOTS);
	scs_slot_select(sid);
}
#endif

/*=========================================================================
 * ISO7816 translator
 *=======================================================================*/
static uint32_t iccd_device_features(void)
{
	uint32_t features = ICCD_FEATURE_DEFAULT;

#ifdef CONFIG_ICCD_XCHG_CHAR
	features |= SCD_FEATURE_XCHG_CHAR;
#endif
#ifdef CONFIG_ICCD_XCHG_APDU
	features |= SCD_FEATURE_XCHG_APDU;
#endif
#ifdef CONFIG_ICCD_XCHG_APDU_EXT
	features |= SCD_FEATURE_XCHG_APDU_EXT;
#endif
	return features;
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
	}
	return SCD_RDR2PC_SLOTSTATUS;
}

static uint32_t iccd_proto_features(void)
{
#ifdef CONFIG_ICCD_T0
	uint32_t proto = SCS_PROTO_T0;
#endif
#ifdef CONFIG_ICCD_T1
	uint32_t proto = SCS_PROTO_T1;
#endif
	return 1 << proto;
}

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

/*=========================================================================
 * ICCD slots
 *=======================================================================*/
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

void scd_queue_reset(scd_qid_t qid)
{
	__scd_queue_reset(qid);
}

void scd_submit_response(void)
{
	__scd_submit_response(ICCD_ADDR_IN,
			      iccd_addr2qid(usbd_saved_addr()));
}

void scd_submit_command(void)
{
	__scd_submit_command(ICCD_ADDR_OUT,
			     iccd_addr2qid(usbd_saved_addr()));
}

/*=========================================================================
 * control data
 *=======================================================================*/
static uint16_t iccd_config_length(void)
{
	return SCD_DT_SCD_SIZE;
}

static void iccd_get_iccd_desc(void)
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
	USBD_INB(SCD_ECHO_APDU_CLASS);
	USBD_INB(SCD_ECHO_APDU_CLASS);
	USBD_INW(SCD_LCD_LAYOUT_NONE);
	USBD_INB(SCD_SPE_SUPPORT_NONE);
	USBD_INB(NR_SCD_USB_QUEUES);	/* must be 1 */
}

static void iccd_get_config_desc(void)
{
	usbd_input_interface_desc(USB_INTERFACE_CLASS_SCD,
				  USB_DEVICE_SUBCLASS_NONE,
				  USB_INTERFACE_PROTOCOL_ICCD,
				  ICCD_STRING_INTERFACE);
	iccd_get_iccd_desc();
#ifdef CONFIG_SCD_BULK
	usbd_input_endpoint_desc(ICCD_ADDR_IN);
	usbd_input_endpoint_desc(ICCD_ADDR_OUT);
#endif
#ifdef CONFIG_SCD_INTERRUPT
	usbd_input_endpoint_desc(ICCD_ADDR_IRQ);
#endif
}

static void iccd_get_string_desc(void)
{
	uint8_t id = LOBYTE(usbd_control_request_value());

	switch (id) {
	case ICCD_STRING_INTERFACE:
		usbd_input_device_name();
		break;
	default:
		USBD_INB(0x00);
		break;
	}
}

static void iccd_get_descriptor(void)
{
	uint8_t desc;
	
	desc = HIBYTE(usbd_control_request_value());

	switch (desc) {
	case USB_DT_CONFIG:
		iccd_get_config_desc();
		break;
	case USB_DT_STRING:
		iccd_get_string_desc();
		break;
	default:
		usbd_endpoint_halt();
		break;
	}
}

#ifndef CONFIG_ICCD_PROTO_BULK
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
#endif

#define iccd_set_descriptor()		usbd_endpoint_halt()

static void iccd_handle_standard_request(void)
{
	uint8_t req = usbd_control_request_type();

	switch (req) {
	case USB_REQ_GET_DESCRIPTOR:
		iccd_get_descriptor();
		break;
	case USB_REQ_SET_DESCRIPTOR:
		iccd_set_descriptor();
		break;
	default:
		usbd_endpoint_halt();
	}
}

static void iccd_handle_class_request(void)
{
	uint8_t req = usbd_control_request_type();

	scd_debug(SCD_DEBUG_CS_REQ, req);

	switch (req) {
#ifndef CONFIG_ICCD_PROTO_BULK
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
#endif
	default:
		usbd_endpoint_halt();
	}
}

static void iccd_handle_ctrl_data(void)
{
	uint8_t type = usbd_control_setup_type();
	uint8_t recp = usbd_control_setup_recp();

	switch (recp) {
	case USB_RECP_DEVICE:
		switch (type) {
		case USB_TYPE_STANDARD:
			iccd_handle_standard_request();
			return;
		}
		break;
	case USB_RECP_INTERFACE:
		switch (type) {
		case USB_TYPE_STANDARD:
			iccd_handle_standard_request();
			return;
		case USB_TYPE_CLASS:
			iccd_handle_class_request();
			return;
		}
		break;
	}
	usbd_endpoint_halt();
}

/*=========================================================================
 * bulk-out data
 *=======================================================================*/
void scd_handle_bulk_pc2rdr(void)
{
	scd_handle_pc2rdr_default();
}

void scd_handle_command(void)
{
	__scd_handle_command(iccd_addr2qid(usbd_saved_addr()));
}

/*=========================================================================
 * bulk-in data
 *=======================================================================*/
static void iccd_handle_response(void)
{
	scd_qid_t qid = iccd_addr2qid(usbd_saved_addr());
	scd_qid_t oqid;

	oqid = scd_qid_save(qid);
	scd_debug(SCD_DEBUG_SLOT, scd_qid);

	BUG_ON(scd_states[scd_qid] != SCD_SLOT_STATE_RDR2PC);

	if (scd_is_cmd_status(SCD_CMD_STATUS_FAIL)) {
		scd_SlotStatus_in();
		goto out;
	}

	BUG_ON(scd_cmds[scd_qid].bSlot != ICCD_SID_USB);

	switch (scd_cmds[scd_qid].bMessageType) {
	case SCD_PC2RDR_ICCPOWERON:
	case SCD_PC2RDR_XFRBLOCK:
		scd_DataBlock_in();
		break;
	case SCD_PC2RDR_ICCPOWEROFF:
		scd_SlotStatus_in();
		break;
	case SCD_PC2RDR_ESCAPE:
		scd_Escape_in();
		break;
	case SCD_PC2RDR_GETPARAMETERS:
		scd_SlotStatus_in();
		break;
	default:
		BUG();
		break;
	}
out:
	scd_qid_restore(oqid);
}

void scd_complete_response(void)
{
	__scd_complete_response(iccd_addr2qid(usbd_saved_addr()));
}

/*=========================================================================
 * bulk-out cmpl
 *=======================================================================*/
void scd_complete_bulk_pc2rdr(void)
{
	switch (scd_cmds[scd_qid].bMessageType) {
	case SCD_PC2RDR_GETPARAMETERS:
		/* FIXME: windows sent us */
		scd_SlotStatus_cmp();
		break;
	default:
		scd_complete_pc2rdr_default();
		break;
	}
}

void scd_complete_command(void)
{
	__scd_complete_command(iccd_addr2qid(usbd_saved_addr()));
}

/*=========================================================================
 * interrupt data
 *=======================================================================*/
#ifdef CONFIG_SCD_INTERRUPT
/*=========================================================================
 * dev changes
 *=======================================================================*/
#define ICCD_INTR_CHANGE(id)		((id<<1)+1)
#define ICCD_INTR_STATUS(id)		((id<<1))
static boolean __iccd_change_running(scd_sid_t sid)
{
	return test_bit(ICCD_INTR_CHANGE(sid), iccd_running_intrs);
}

static boolean __iccd_change_pending(scd_sid_t sid)
{
	return test_bit(ICCD_INTR_CHANGE(sid), iccd_pending_intrs);
}

static boolean iccd_change_pending(void)
{
	scd_sid_t sid;
	for (sid = 0; sid < NR_SCD_SLOTS; sid++) {
		if (__iccd_change_pending(sid))
			return true;
	}
	return false;
}

static void iccd_change_data(void)
{
	uint8_t i;
	
	USBD_INB(ICCD_RDR2PC_NOTIFYSLOTCHANGE);
	for (i = 0; i < sizeof (iccd_running_intrs); i++) {
		USBD_INB(iccd_running_intrs[i]);
	}
}

static void iccd_change_discard(void)
{
	scd_sid_t sid = iccd_addr2sid(usbd_saved_addr());
	if (__iccd_change_running(sid)) {
		clear_bit(ICCD_INTR_CHANGE(sid), iccd_running_intrs);
		scd_debug(SCD_DEBUG_INTR, ICCD_INTR_RUNNING_UNSET);
	}
}

static void iccd_change_submit(void)
{
	scd_sid_t sid;

	for (sid = 0; sid < NR_SCD_SLOTS; sid++) {
		/* copy changed bits */
		clear_bit(ICCD_INTR_CHANGE(sid), iccd_pending_intrs);
		scd_debug(SCD_DEBUG_INTR, ICCD_INTR_PENDING_UNSET);
		set_bit(ICCD_INTR_CHANGE(sid), iccd_running_intrs);
		scd_debug(SCD_DEBUG_INTR, ICCD_INTR_RUNNING_SET);

		/* copy status bits */
		if (test_bit(ICCD_INTR_STATUS(sid), iccd_pending_intrs)) {
			set_bit(ICCD_INTR_STATUS(sid), iccd_running_intrs);
			scd_debug(SCD_DEBUG_INTR, SCD_SLOT_STATUS_ACTIVE);
		} else {
			clear_bit(ICCD_INTR_STATUS(sid), iccd_running_intrs);
			scd_debug(SCD_DEBUG_INTR, SCD_SLOT_STATUS_NOTPRESENT);
		}
	}
}

static void iccd_change_raise(void)
{
	boolean changed = false;
	if (scd_slot_status() == SCD_SLOT_STATUS_NOTPRESENT) {
		if (test_bit(ICCD_INTR_STATUS(scd_qid), iccd_pending_intrs)) {
			clear_bit(ICCD_INTR_STATUS(scd_qid),
				  iccd_pending_intrs);
			changed = true;
		}
	} else {
		if (!test_bit(ICCD_INTR_STATUS(scd_qid), iccd_pending_intrs)) {
			set_bit(ICCD_INTR_STATUS(scd_qid), iccd_pending_intrs);
			changed = true;
		}
	}
	if (changed) {
		set_bit(ICCD_INTR_CHANGE(scd_qid), iccd_pending_intrs);
		scd_debug(SCD_DEBUG_INTR, ICCD_INTR_PENDING_SET);
	}
}

static uint16_t iccd_change_length(void)
{
	return 1 + div16u(ALIGN(NR_SCD_SLOTS, 4), 4);
}

static void iccd_change_init(void)
{
}

void iccd_handle_interrupt(void)
{
	iccd_change_data();
}

void iccd_complete_interrupt(void)
{
	iccd_change_discard();
}

#define iccd_intr_init()		iccd_change_init()

/*=========================================================================
 * generic interupts
 *=======================================================================*/
static void iccd_submit_interrupt(void)
{
	if (iccd_change_pending()) {
		if (usbd_request_submit(ICCD_ADDR_IRQ,
					iccd_change_length())) {
			iccd_change_submit();
		}
	}
}

static void iccd_handle_ll_intr(void)
{
	scd_qid_t oqid = scd_qid_save(scd_sid);
	iccd_change_raise();
	scd_qid_restore(oqid);
}

static void iccd_intr_start(void)
{
	scd_qid_t qid, oqid;

	for (qid = 0; qid < NR_SCD_SLOTS; qid++) {
		oqid = scd_qid_save(qid);
		iccd_change_raise();
		scd_qid_restore(oqid);
	}
}
#else
static void iccd_handle_ll_intr(void)
{
}

#define iccd_intr_start()
#define iccd_intr_init()
#endif

/*=========================================================================
 * ICCD entrances
 *=======================================================================*/
/* TODO: how to know the slot number we should answer */
static void iccd_handle_ll_cmpl(void)
{
	scs_err_t err = __iccd_get_error();
	BUG_ON(scd_states[scd_qid] != SCD_SLOT_STATE_ISO7816);

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

void iccd_start(void)
{
	iccd_intr_start();
}

void iccd_devid_init(void)
{
}

#ifdef CONFIG_SCD_BULK
usbd_endpoint_t iccd_endpoint_out = {
	USBD_ENDP_BULK_OUT,
	ICCD_ENDP_INTERVAL_OUT,
	scd_submit_command,
	scd_handle_command,
	scd_complete_command,
};

usbd_endpoint_t iccd_endpoint_in = {
	USBD_ENDP_BULK_IN,
	ICCD_ENDP_INTERVAL_IN,
	scd_submit_response,
	iccd_handle_response,
	scd_complete_response,
};
#endif

#ifdef CONFIG_SCD_INTERRUPT
usbd_endpoint_t iccd_endpoint_irq = {
	USBD_ENDP_INTR_IN,
	ICCD_ENDP_INTERVAL_INTR,
	iccd_submit_interrupt,
	iccd_handle_interrupt,
	iccd_complete_interrupt,
};
#endif

usbd_interface_t usb_iccd_interface = {
	ICCD_STRING_FIRST,
	ICCD_STRING_LAST,
	NR_SCD_ENDPS,
	iccd_config_length,
	iccd_handle_ctrl_data,
};

static void iccd_claim_endp(scd_qid_t id)
{
#ifdef CONFIG_SCD_BULK
	iccd_addr[id][SCD_ENDP_BULK_OUT] = usbd_claim_endpoint(true, &iccd_endpoint_out);
	iccd_addr[id][SCD_ENDP_BULK_IN] = usbd_claim_endpoint(true, &iccd_endpoint_in);
#endif
#ifdef CONFIG_SCD_INTERRUPT
	iccd_addr[id][SCD_ENDP_INTR_IN] = usbd_claim_endpoint(true, &iccd_endpoint_irq);
#endif
}

static void iccd_dev_init(void)
{
	scd_qid_t id;
	
	/* USB interface and endpoint stuff */
	for (id = 0; id < NR_SCD_SLOTS; id++) {
		usbd_declare_interface(50, &usb_iccd_interface);
		iccd_claim_endp(id);
		scd_queue_reset(id);
	}
	scd_qid_select(0);
}

void iccd_init(void)
{
	iccd_devid_init();
	iccd_dev_init();
	iccd_intr_init();
	__iccd_reg_handlers(iccd_handle_ll_intr, iccd_handle_ll_cmpl);
	iccd_start();
}
