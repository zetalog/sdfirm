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

#define ICCD_XB_ERR	iccd_cmd_data.xb.dwIccOutErr
#define ICCD_XB_NC	iccd_cmd_data.xb.dwIccOutCnt
#define ICCD_XB_NE	iccd_cmd_data.xb.dwIccExpCnt
#define ICCD_XB_WI	iccd_cmd_data.xb.bIccWaitInt

typedef union iccd_data {
	struct scd_t0_param t0;
	struct scd_t1_param t1;
	struct scd_db_param db;
	struct scd_xb_param xb;
} iccd_data_t;

static void iccd_dev_reset(iccd_t id);
static iccd_t iccd_addr2id(uint8_t addr);

static void iccd_handle_cmp(scs_err_t err, boolean block);
static void __iccd_XfrBlock_out(scs_size_t hdr_size, scs_size_t blk_size);

struct iccd_dev iccd_devs[NR_SCD_SLOTS];

iccd_data_t iccd_cmd_data;
iccd_t scd_qid = INVALID_SCD_QID;

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
#define __iccd_get_error()		cos_get_error()
#define __iccd_xchg_read(idx)		cos_xchg_read(idx)
#define __iccd_xchg_write(idx, b)	cos_xchg_write(idx, b)
#define __iccd_xchg_block(nc, ne)	cos_xchg_block(nc, ne)
#define __iccd_xchg_avail()		cos_xchg_avail()
#define __iccd_power_off()		cos_power_off()
#define __iccd_power_on()		cos_power_on()
#define __iccd_reg_handlers(cb1, cb2) 		\
	do {					\
		cos_register_handlers(cb2);	\
		cb1();				\
	} while (0)

uint8_t iccd_dev_state(uint8_t state)
{
	return SCD_SLOT_STATUS_ACTIVE;
}

static uint8_t iccd_dev_error(scs_err_t err)
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
#else
#define __iccd_dev_get_state()		scs_get_slot_status()
#define __iccd_get_error()		scs_get_slot_error()
#define __iccd_xchg_read(idx)		scs_slot_xchg_read(idx)	
#define __iccd_xchg_write(idx, b)	scs_slot_xchg_write(idx, b)
#define __iccd_xchg_avail()		scs_slot_xchg_avail()
#define __iccd_xchg_block(nc, ne)	scs_slot_xchg_block(nc, ne)
#define __iccd_power_off()		scs_slot_power_off()
#define __iccd_power_on()		scs_slot_power_on()
#define __iccd_reg_handlers(cb1, cb2) 	scd_notify_slot(cb1, cb2)

uint8_t iccd_dev_state(uint8_t d)
{
	uint8_t state = __iccd_dev_get_state();

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

static uint8_t iccd_dev_error(scs_err_t err)
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
	uint32_t proto = ICC_PROTO_T0;
#endif
#ifdef CONFIG_ICCD_T1
	uint32_t proto = ICC_PROTO_T1;
#endif
	return 1 << proto;
}

#if NR_SCD_SLOTS > 1
static void iccd_cid_restore(iccd_t id)
{
	scd_qid = id;
}

static iccd_t iccd_cid_save(iccd_t id)
{
	iccd_t scid;

	scid = scd_qid;
	iccd_cid_restore(id);
	return scid;
}

#define iccd_cid_select(id)	iccd_cid_restore(id)
#else
#define iccd_cid_restore(id)	(id = id)
#define iccd_cid_save(id)	(id)
#define iccd_cid_select(id)	(scd_qid = id)
#endif

uint8_t scd_slot_status(void)
{
	uint8_t status = iccd_dev_state(0);
	if (status == SCD_SLOT_STATUS_INACTIVE)
		BUG();
	return status;
}

/*=========================================================================
 * ICCD slots
 *=======================================================================*/
void scd_slot_enter(uint8_t state)
{
	if (iccd_devs[scd_qid].state != state) {
		scd_debug(SCD_DEBUG_STATE, state);
		iccd_devs[scd_qid].state = state;
	}
	if (state == SCD_SLOT_STATE_PC2RDR) {
		scd_resps[scd_qid].abRFU3 = 0;
	}
}

static void iccd_dev_reset(iccd_t id)
{
	iccd_devs[id].state = SCD_SLOT_STATE_PC2RDR;
	scd_resps[id].bStatus &= ~SCD_CMD_STATUS_MASK;
}

static boolean iccd_is_cmd_status(uint8_t status)
{
	return ((scd_resps[scd_qid].bStatus & SCD_CMD_STATUS_MASK) == status);
}

static void iccd_submit_response(void)
{
	iccd_t id = iccd_addr2id(usbd_saved_addr());
	iccd_t ocid;
	if (iccd_devs[id].state == SCD_SLOT_STATE_RDR2PC) {
		ocid = iccd_cid_save(id);
		usbd_request_submit(ICCD_ADDR_IN,
				    SCD_HEADER_SIZE + scd_resps[id].dwLength);
		iccd_cid_restore(ocid);
	}
}

static void iccd_submit_command(void)
{
	iccd_t id = iccd_addr2id(usbd_saved_addr());
	iccd_t ocid;
	if (iccd_devs[id].state == SCD_SLOT_STATE_PC2RDR) {
		ocid = iccd_cid_save(id);
		usbd_request_submit(ICCD_ADDR_OUT, SCD_HEADER_SIZE);
		iccd_cid_restore(ocid);
	}
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
	USBD_INW(ICCD_VERSION_DEFAULT);
	USBD_INB(ICCD_MAX_BUSY_SLOT - 1);	/* must be 0 */
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
	USBD_INB(ICCD_MAX_BUSY_SLOT);	/* must be 1 */
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
static void iccd_DataBlock_out(void)
{
	scs_size_t nr = __iccd_xchg_avail();
	scs_size_t ne = ICCD_XB_NE;

	__scd_CmdSuccess_out();
	scd_resps[scd_qid].abRFU3 = 0;
	scd_resps[scd_qid].dwLength = ne ? (nr < ne ? nr : ne) : nr;
}

static void iccd_IccPowerOn_out(void)
{
	if (usbd_request_handled() == SCD_HEADER_SIZE) {
		/* reset SCD error */
		ICCD_XB_ERR = SCS_ERR_SUCCESS;
		/* reset Nc */
		ICCD_XB_NC = 0;
		/* Ne should be 32 (ATR) + 1 (TS) */
		ICCD_XB_NE = ICC_ATR_MAX;
		/* reset WI */
		ICCD_XB_WI = 0;
	}
}

static void __iccd_XfrBlock_out(scs_size_t hdr_size, scs_size_t blk_size)
{
	scs_off_t i;
	uint8_t byte = 0;

	if (usbd_request_handled() == hdr_size) {
		/* reset SCD error */
		ICCD_XB_ERR = SCS_ERR_SUCCESS;
		/* reset Nc */
		ICCD_XB_NC = 0;
		/* Ne is from wLevelParameter
		 * TODO: extended APDU level
		 * more efforts
		 */
		ICCD_XB_NE = (scd_cmds[scd_qid].abRFU[1] << 8) |
			      scd_cmds[scd_qid].abRFU[2];
		/* reset WI */
		ICCD_XB_WI = scd_cmds[scd_qid].abRFU[0];
	}
	/* force USB reap on error */
	if (!scd_slot_success(ICCD_XB_ERR)) {
		return;
	}

	usbd_iter_accel();
	for (i = ICCD_XB_NC; i < blk_size; i++) {
		/* XXX: USBD_OUTB May Fake Reads 'byte'
		 * Following codes are enabled only when USBD_OUTB actually
		 * gets something into the 'byte', which happens when:
		 * handled-1 == NC+hdr_size.
		 */
		USBD_OUT_BEGIN(byte) {
			/* Now byte contains non-fake value. */
			ICCD_XB_ERR = __iccd_xchg_write(ICCD_XB_NC, byte);
			if (!scd_slot_success(ICCD_XB_ERR)) {
				return;
			}
			ICCD_XB_NC++;
		} USBD_OUT_END
	}
}

static void iccd_XfrBlock_out(void)
{
	__iccd_XfrBlock_out(SCD_HEADER_SIZE, scd_cmds[scd_qid].dwLength);
}

static void iccd_handle_slot_pc2rdr(void)
{
	if (scd_slot_status() == SCD_SLOT_STATUS_NOTPRESENT) {
		scd_CmdFailure_out(ICCD_ERROR_ICC_MUTE);
		return;
	}

	switch (scd_cmds[scd_qid].bMessageType) {
	case SCD_PC2RDR_ICCPOWERON:
		iccd_IccPowerOn_out();
		break;
	case SCD_PC2RDR_ICCPOWEROFF:
		/* nothing to do */
		break;
	case SCD_PC2RDR_XFRBLOCK:
		iccd_XfrBlock_out();
		break;
	case SCD_PC2RDR_ESCAPE:
		scd_Escape_out();
		break;
	}
}

static void iccd_handle_command(void)
{
	iccd_t id = iccd_addr2id(usbd_saved_addr());
	iccd_t ocid;

	ocid = iccd_cid_save(id);
	scd_debug(SCD_DEBUG_SLOT, id);

	BUG_ON(iccd_devs[scd_qid].state != SCD_SLOT_STATE_PC2RDR &&
	       iccd_devs[scd_qid].state != SCD_SLOT_STATE_SANITY);

	scd_slot_enter(SCD_SLOT_STATE_SANITY);

	/* CCID message header */
	scd_CmdHeader_out();

	if (usbd_request_handled() < SCD_HEADER_SIZE)
		goto out;

	if (usbd_request_handled() == SCD_HEADER_SIZE) {
		usbd_request_commit(SCD_HEADER_SIZE +
				    scd_cmds[scd_qid].dwLength);
		scd_debug(SCD_DEBUG_PC2RDR, scd_cmds[scd_qid].bMessageType);
	}

	if (scd_cmds[scd_qid].bSlot != ICCD_SINGLE_SLOT_IDX)
		goto out;

	iccd_handle_slot_pc2rdr();
out:
	iccd_cid_restore(ocid);
	return;
}

/*=========================================================================
 * bulk-in data
 *=======================================================================*/
void iccd_DataBlock_in(void)
{
	scs_off_t i;

	scd_RespHeader_in(scd_resps[scd_qid].dwLength);

	usbd_iter_accel();

	for (i = usbd_request_handled()-SCD_HEADER_SIZE;
	     i < scd_resps[scd_qid].dwLength; i++) {
		USBD_INB(__iccd_xchg_read(i));
	}

	BUG_ON(usbd_request_handled() >
	       scd_resps[scd_qid].dwLength + SCD_HEADER_SIZE);
}

static void iccd_handle_response(void)
{
	iccd_t id = iccd_addr2id(usbd_saved_addr());
	iccd_t ocid;

	ocid = iccd_cid_save(id);
	scd_debug(SCD_DEBUG_SLOT, scd_qid);

	BUG_ON(iccd_devs[scd_qid].state != SCD_SLOT_STATE_RDR2PC);

	if (iccd_is_cmd_status(SCD_CMD_STATUS_FAIL)) {
		scd_SlotStatus_in();
		goto out;
	}

	BUG_ON(scd_cmds[scd_qid].bSlot != ICCD_SINGLE_SLOT_IDX);

	switch (scd_cmds[scd_qid].bMessageType) {
	case SCD_PC2RDR_ICCPOWERON:
	case SCD_PC2RDR_XFRBLOCK:
		iccd_DataBlock_in();
		break;
	case SCD_PC2RDR_ICCPOWEROFF:
	case ICCD_PC2RDR_GETPARAMETERS:
		scd_SlotStatus_in();
		break;
	case SCD_PC2RDR_ESCAPE:
		scd_Escape_in();
		break;
	default:
		BUG();
		break;
	}
out:
	iccd_cid_restore(ocid);
}

static void iccd_complete_response(void)
{
	iccd_t id = iccd_addr2id(usbd_saved_addr());
	iccd_t ocid = iccd_cid_save(id);
	scd_slot_enter(SCD_SLOT_STATE_PC2RDR);
	iccd_cid_restore(ocid);
}

/*=========================================================================
 * bulk-out cmpl
 *=======================================================================*/
void iccd_DataBlock_cmp(scs_err_t err)
{
	iccd_handle_cmp(err, true);
}

static void iccd_handle_cmp(scs_err_t err, boolean block)
{
	if (!scd_slot_progress(err)) {
		if (!scd_slot_success(err)) {
			scd_CmdFailure_out(iccd_dev_error(err));
		} else {
			if (block) {
				iccd_DataBlock_out();
			} else {
				scd_SlotStatus_out();
			}
		}
		scd_CmdResponse_cmp();
	} else {
		scd_slot_enter(SCD_SLOT_STATE_ISO7816);
	}
}

static void iccd_IccPowerOn_cmp(void)
{
	scs_err_t err;
	err = __iccd_power_on();
	iccd_handle_cmp(err, true);
}

static void iccd_IccPowerOff_cmp(void)
{
	scs_err_t err;

	err = __iccd_power_off();
	iccd_handle_cmp(err, false);
}

void iccd_XfrBlock_cmp(void)
{
	scs_err_t err = ICCD_XB_ERR;

	/* TODO: wLevelParameter check when XCHG_CHAR or XCHG_APDU_EXT */
	if (scd_slot_success(err) && ICCD_XB_NC != 0) {
		err = __iccd_xchg_block(ICCD_XB_NC, ICCD_XB_NE);
	}
	iccd_handle_cmp(err, true);
}

static void iccd_complete_slot_pc2rdr(void)
{
	if (usbd_request_handled() !=
	    (scd_cmds[scd_qid].dwLength + SCD_HEADER_SIZE)) {
		scd_CmdOffset_cmp(1);
		return;
	}
	if (iccd_is_cmd_status(SCD_CMD_STATUS_FAIL)) {
		scd_CmdResponse_cmp();
		return;
	}

	switch (scd_cmds[scd_qid].bMessageType) {
	case SCD_PC2RDR_ICCPOWERON:
		iccd_IccPowerOn_cmp();
		break;
	case SCD_PC2RDR_ICCPOWEROFF:
		iccd_IccPowerOff_cmp();
		break;
	case SCD_PC2RDR_XFRBLOCK:
		iccd_XfrBlock_cmp();
		break;
	case ICCD_PC2RDR_GETPARAMETERS:
		/* FIXME: windows sent us */
		scd_SlotStatus_cmp();
		break;
	case SCD_PC2RDR_ESCAPE:
		scd_Escape_cmp();
		break;
	default:
		scd_CmdOffset_cmp(0);
		break;
	}
}

static void iccd_complete_command(void)
{
	iccd_t id = iccd_addr2id(usbd_saved_addr());
	iccd_t ocid;
	ocid = iccd_cid_save(id);

	BUG_ON(iccd_devs[scd_qid].state != SCD_SLOT_STATE_PC2RDR &&
	       iccd_devs[scd_qid].state != SCD_SLOT_STATE_SANITY);

	if (usbd_request_handled() < SCD_HEADER_SIZE ||
	    scd_cmds[scd_qid].bSlot != ICCD_SINGLE_SLOT_IDX) {
		scd_SlotNotExist_cmp();
		goto out;
	}

	if (iccd_is_cmd_status(SCD_CMD_STATUS_FAIL)) {
		/* CMD_ABORTED and SLOT_BUSY error */
		scd_CmdResponse_cmp();
		goto out;
	}

	/* SANITY state completed */
	scd_slot_enter(SCD_SLOT_STATE_PC2RDR);

	iccd_complete_slot_pc2rdr();
out:
	iccd_cid_restore(ocid);
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
static boolean __iccd_change_running(iccd_t id)
{
	return test_bit(ICCD_INTR_CHANGE(id), iccd_running_intrs);
}

static boolean __iccd_change_pending(iccd_t id)
{
	return test_bit(ICCD_INTR_CHANGE(id), iccd_pending_intrs);
}

static boolean iccd_change_pending(void)
{
	iccd_t id;
	for (id = 0; id < NR_SCD_SLOTS; id++) {
		if (__iccd_change_pending(id))
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
	iccd_t id = iccd_addr2id(usbd_saved_addr());
	if (__iccd_change_running(id)) {
		clear_bit(ICCD_INTR_CHANGE(id), iccd_running_intrs);
		scd_debug(SCD_DEBUG_INTR, ICCD_INTR_RUNNING_UNSET);
	}
}

static void iccd_change_submit(void)
{
	iccd_t id;

	for (id = 0; id < NR_SCD_SLOTS; id++) {
		/* copy changed bits */
		clear_bit(ICCD_INTR_CHANGE(id), iccd_pending_intrs);
		scd_debug(SCD_DEBUG_INTR, ICCD_INTR_PENDING_UNSET);
		set_bit(ICCD_INTR_CHANGE(id), iccd_running_intrs);
		scd_debug(SCD_DEBUG_INTR, ICCD_INTR_RUNNING_SET);

		/* copy status bits */
		if (test_bit(ICCD_INTR_STATUS(id), iccd_pending_intrs)) {
			set_bit(ICCD_INTR_STATUS(id), iccd_running_intrs);
			scd_debug(SCD_DEBUG_INTR, SCD_SLOT_STATUS_ACTIVE);
		} else {
			clear_bit(ICCD_INTR_STATUS(id), iccd_running_intrs);
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
	iccd_t ocid = iccd_cid_save(scd_sid);
	iccd_change_raise();
	iccd_cid_restore(ocid);
}

static void iccd_intr_start(void)
{
	iccd_t id, ocid;

	for (id = 0; id < NR_SCD_SLOTS; id++) {
		ocid = iccd_cid_save(id);
		iccd_change_raise();
		iccd_cid_restore(ocid);
	}
}
#else
static void iccd_handle_ll_intr(void)
{
}

#define iccd_intr_start()
#define iccd_intr_init()
#endif

/* iccd */
static iccd_t iccd_addr2id(uint8_t addr)
{
	iccd_t id;
	uint8_t i;

	for (id = 0; id < NR_SCD_SLOTS; id++) {
		for (i = 0; i < NR_SCD_ENDPS; i++) {
			if (addr == iccd_addr[scd_qid][i])
				return id;
		}
	}
	return NR_SCD_SLOTS;
}

/*=========================================================================
 * ICCD entrances
 *=======================================================================*/
/* TODO: how to know the slot number we should answer */
static void iccd_handle_ll_cmpl(void)
{
	scs_err_t err = __iccd_get_error();
	BUG_ON(iccd_devs[scd_qid].state != SCD_SLOT_STATE_ISO7816);

	switch (scd_cmds[scd_qid].bMessageType) {
	case SCD_PC2RDR_ICCPOWERON:
	case SCD_PC2RDR_XFRBLOCK:
		iccd_DataBlock_cmp(err);
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
	iccd_submit_command,
	iccd_handle_command,
	iccd_complete_command,
};

usbd_endpoint_t iccd_endpoint_in = {
	USBD_ENDP_BULK_IN,
	ICCD_ENDP_INTERVAL_IN,
	iccd_submit_response,
	iccd_handle_response,
	iccd_complete_response,
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

static void iccd_claim_endp(iccd_t id)
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
	iccd_t id;
	
	/* USB interface and endpoint stuff */
	for (id = 0; id < NR_SCD_SLOTS; id++) {
		usbd_declare_interface(50, &usb_iccd_interface);
		iccd_claim_endp(id);
		iccd_dev_reset(id);
	}
	iccd_cid_select(0);
}

void iccd_init(void)
{
	iccd_devid_init();
	iccd_dev_init();
	iccd_intr_init();
	__iccd_reg_handlers(iccd_handle_ll_intr, iccd_handle_ll_cmpl);
	iccd_start();
}
