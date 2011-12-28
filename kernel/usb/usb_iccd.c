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
#include <target/cos.h>
#include <target/scd.h>

#ifdef CONFIG_ICCD_COS
#define NR_ICCD_DEVICES 1
#define iccd_id		0
typedef cos_err_t 	iccd_err_t;
typedef cos_size_t 	iccd_size_t;
typedef cos_off_t 	iccd_off_t;
#else
#define NR_ICCD_DEVICES NR_SCD_DEVICES
#define iccd_id		scd_id
typedef scd_err_t 	iccd_err_t;
typedef scd_size_t 	iccd_size_t;
typedef scd_off_t 	iccd_off_t;
#endif
#define INVALID_ICCD_UNIT NR_ICCD_DEVICES

#define ICCD_REQ_ICC_POWERON		0x62
#define ICCD_REQ_ICC_POWEROFF		0x63
#define ICCD_REQ_XFR_BLOCK		0x65
#define ICCD_REQ_DATA_BLOCK		0x6F
#define ICCD_REQ_GET_ICC_STATUS		0xA0
#define ICCD_REQ_SLOT_STATUS		0x81

struct iccd_xb_param {
	iccd_err_t dwIccOutErr;
	urb_size_t dwIccOutCnt;
	urb_size_t dwIccExpCnt;
	uint8_t bIccWaitInt;
#define ICCD_XB_ERR	iccd_cmd_data.xb.dwIccOutErr
#define ICCD_XB_NC	iccd_cmd_data.xb.dwIccOutCnt
#define ICCD_XB_NE	iccd_cmd_data.xb.dwIccExpCnt
#define ICCD_XB_WI	iccd_cmd_data.xb.bIccWaitInt
};

/* DataBlock */
struct iccd_db_param {
	urb_size_t dwIccOutIter;
	iccd_err_t dwIccOutErr;
};

typedef union iccd_data {
	struct iccd_t0_param t0;
	struct iccd_t1_param t1;
	struct iccd_db_param db;
	struct iccd_xb_param xb;
} iccd_data_t;

typedef uint8_t	iccd_t;

static void iccd_dev_enter(uint8_t state);
static void iccd_dev_reset(iccd_t id);
static void iccd_submit_response(iccd_t id);
static void iccd_submit_command(iccd_t id);

static void iccd_handle_cmp(iccd_err_t err, boolean block);
static void __iccd_XfrBlock_out(iccd_size_t hdr_size, iccd_size_t blk_size);

struct iccd_cmd iccd_cmds[NR_ICCD_DEVICES];
struct iccd_dev iccd_devs[NR_ICCD_DEVICES];
struct iccd_resp iccd_resps[NR_ICCD_DEVICES];

iccd_data_t iccd_cmd_data;
iccd_t iccd_cid = INVALID_ICCD_UNIT;

#ifdef CONFIG_ICCD_INTERRUPT_IN
DECLARE_BITMAP(iccd_running_intrs, NR_ICCD_DEVICES+NR_ICCD_DEVICES);
DECLARE_BITMAP(iccd_pending_intrs, NR_ICCD_DEVICES+NR_ICCD_DEVICES);
#endif

#if NR_ICCD_ENDPS > 0
uint8_t iccd_addr[NR_ICCD_DEVICES][NR_ICCD_ENDPS];
#define ICCD_ADDR_IN			iccd_addr[iccd_cid][ICCD_ENDP_BULK_IN]
#define ICCD_ADDR_OUT			iccd_addr[iccd_cid][ICCD_ENDP_BULK_OUT]
#define ICCD_ADDR_IRQ			iccd_addr[iccd_cid][ICCD_ENDP_INTR_IN]
#define ICCD_ENDP_INTERVAL_INTR		0x7F
#define ICCD_ENDP_INTERVAL_NONE		0x00
#define ICCD_ENDP_INTERVAL_IN		0x01
#define ICCD_ENDP_INTERVAL_OUT		0x01
#endif

#define ICCD_STRING_FIRST	0x10
#define ICCD_STRING_INTERFACE	ICCD_STRING_FIRST+0
#define ICCD_STRING_LAST	ICCD_STRING_INTERFACE

/* all interface can use it */
usb_endp_desc_t iccd_endpoints[NR_ICCD_ENDPS] = {
#if NR_ICCD_ENDPS > 1
	{
		USB_DT_ENDPOINT_SIZE,
		USB_DT_ENDPOINT,
		USB_DIR2ADDR(USB_DIR_IN) | 0,	/* !bEndpointAddress */
		USB_ENDP_BULK,			/* bmAttributes */
		0,				/* !wMaxPacketSize */
		ICCD_ENDP_INTERVAL_IN,
	},
	{
		USB_DT_ENDPOINT_SIZE,
		USB_DT_ENDPOINT,
		USB_DIR2ADDR(USB_DIR_OUT) | 0,	/* !bEndpointAddress */
		USB_ENDP_BULK,			/* bmAttributes */
		0,				/* !wMaxPacketSize */
		ICCD_ENDP_INTERVAL_OUT,
	},
#endif
#if NR_ICCD_ENDPS == 1 || NR_ICCD_ENDPS == 3
	{
		USB_DT_ENDPOINT_SIZE,
		USB_DT_ENDPOINT,
		USB_DIR2ADDR(USB_DIR_IN) | 0,	/* !bEndpointAddress */
		USB_ENDP_INTERRUPT,		/* bmAttributes */
		0,				/* !wMaxPacketSize */
		ICCD_ENDP_INTERVAL_INTR,
	},
#endif
};

#ifdef CONFIG_ICCD_COS
#define ICCD_DEV_STATE_PRESENT		COS_DEV_STATE_PRESENT
#define ICCD_DEV_STATE_HWERROR		COS_DEV_STATE_PRESENT+1
#define ICCD_DEV_STATE_NOTPRESENT	COS_DEV_STATE_PRESENT+2

#define __iccd_get_error()		cos_get_error()
#define __iccd_xchg_read(idx)		cos_xchg_read(idx)
#define __iccd_xchg_write(idx, b)	cos_xchg_write(idx, b)
#define __iccd_xchg_block(nc, ne)	cos_xchg_block(nc, ne)
#define __iccd_xchg_avail()		cos_xchg_avail()
#define __iccd_power_off()		cos_power_off()
#define __iccd_power_on()		cos_power_on()
#define __iccd_err_success()		COS_ERR_SUCCESS
#define __iccd_reg_handlers(cb1, cb2) 		\
	do {					\
		cos_register_handlers(cb2);	\
		cb1();				\
	} while (0)

uint8_t iccd_dev_state(uint8_t state)
{
	return ICCD_DEV_STATE_PRESENT;
}

static boolean iccd_dev_progress(iccd_err_t err)
{
	return err == COS_ERR_PROGRESS;
}

static boolean iccd_dev_success(iccd_err_t err)
{
	return err == COS_ERR_SUCCESS;
}

static uint8_t iccd_dev_error(iccd_err_t err)
{
	switch (err) {
	case COS_ERR_OVERRUN:
		return ICCD_ERROR_XFR_OVERRUN;
	case COS_ERR_NOTPRESENT:
		return ICCD_ERROR_ICC_MUTE;
	case COS_ERR_TIMEOUT:
	case COS_ERR_HW_ERROR:
		return ICCD_ERROR_HW_ERROR;
	}
	return ICCD_ERROR_HW_ERROR;
}
#else
#define ICCD_DEV_STATE_HWERROR		SCD_DEV_STATE_HWERROR
#define ICCD_DEV_STATE_NOTPRESENT	SCD_DEV_STATE_NOTPRESENT
#define ICCD_DEV_STATE_PRESENT		SCD_DEV_STATE_PRESENT

#define __iccd_dev_get_state()		scd_dev_get_state()
#define __iccd_get_error()		scd_get_error()
#define __iccd_xchg_read(idx)		scd_xchg_read(idx)	
#define __iccd_xchg_write(idx, b)	scd_xchg_write(idx, b)
#define __iccd_xchg_avail()		scd_xchg_avail()
#define __iccd_xchg_block(nc, ne)	scd_xchg_block(nc, ne)
#define __iccd_power_off()		scd_power_off()
#define __iccd_power_on()		scd_power_on()
#define __iccd_err_success()		SCD_ERR_SUCCESS
#define __iccd_reg_handlers(cb1, cb2) 	scd_register_handlers(cb1, cb2)

uint8_t iccd_dev_state(uint8_t d)
{
	uint8_t state = __iccd_dev_get_state();

	switch (state) {
	default:
	case SCD_DEV_STATE_HWERROR:
		return ICCD_DEV_STATE_HWERROR;
	case SCD_DEV_STATE_NOTPRESENT:
		return ICCD_DEV_STATE_NOTPRESENT;
	}
}

static boolean iccd_dev_progress(iccd_err_t err)
{
	return err == SCD_ERR_PROGRESS;
}

static boolean iccd_dev_success(iccd_err_t err)
{
	return err == SCD_ERR_SUCCESS;
}

static uint8_t iccd_dev_error(iccd_err_t err)
{
	switch (err) {
	case SCD_ERR_OVERRUN:
		return ICCD_ERROR_XFR_OVERRUN;
	case SCD_ERR_NOTPRESENT:
		return ICCD_ERROR_ICC_MUTE;
	case SCD_ERR_TIMEOUT:
	case SCD_ERR_HW_ERROR:
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

static uint8_t iccd_resp_message(void)
{
	switch (iccd_cmds[iccd_cid].bMessageType) {
	case ICCD_PC2RDR_ICCPOWERON:
	case ICCD_PC2RDR_XFRBLOCK:
		return ICCD_RDR2PC_DATABLOCK;
	case ICCD_PC2RDR_ICCPOWEROFF:
		return ICCD_RDR2PC_SLOTSTATUS;
	}
	return ICCD_RDR2PC_SLOTSTATUS;
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

#if NR_ICCD_DEVICES > 1
static void iccd_cid_restore(iccd_t id)
{
	iccd_cid = id;
}

static iccd_t iccd_cid_save(iccd_t id)
{
	iccd_t scid;

	scid = iccd_cid;
	iccd_cid_restore(id);
	return scid;
}

#define iccd_cid_select(id)	iccd_cid_restore(id)
#else
#define iccd_cid_restore(id)	(id = id)
#define iccd_cid_save(id)	(id)
#define iccd_cid_select(id)	(iccd_cid = id)
#endif

static uint8_t iccd_dev_status(void)
{
	uint8_t status = iccd_dev_state(0);
	if (status == ICCD_DEV_STATE_HWERROR)
		BUG();
	return status;
}

/*=========================================================================
 * ICCD slots
 *=======================================================================*/
static void iccd_dev_enter(uint8_t state)
{
	if (iccd_devs[iccd_cid].state != state) {
		scd_debug(SCD_DEBUG_STATE, state);
		iccd_devs[iccd_cid].state = state;
	}
	if (state == ICCD_SLOT_STATE_PC2RDR) {
		iccd_resps[iccd_cid].abRFU3 = 0;
	}
}

static void iccd_dev_reset(iccd_t id)
{
	iccd_devs[id].state = ICCD_SLOT_STATE_PC2RDR;
	iccd_resps[id].bStatus &= ~ICCD_CMD_STATUS_MASK;
}

static boolean iccd_is_cmd_status(uint8_t status)
{
	return ((iccd_resps[iccd_cid].bStatus & ICCD_CMD_STATUS_MASK) == status);
}

static void iccd_submit_response(iccd_t id)
{
	iccd_t ocid;
	if (iccd_devs[id].state == ICCD_SLOT_STATE_RDR2PC) {
		ocid = iccd_cid_save(id);
		usbd_request_submit(ICCD_ADDR_IN,
				    ICCD_HEADER_SIZE + iccd_resps[id].dwLength);
		iccd_cid_restore(ocid);
	}
}

static void iccd_submit_command(iccd_t id)
{
	iccd_t ocid;
	if (iccd_devs[id].state == ICCD_SLOT_STATE_PC2RDR) {
		ocid = iccd_cid_save(id);
		usbd_request_submit(ICCD_ADDR_OUT, ICCD_HEADER_SIZE);
		iccd_cid_restore(ocid);
	}
}

/*=========================================================================
 * control data
 *=======================================================================*/
static uint16_t iccd_config_length(void)
{
	uint8_t i;
	uint16_t length;
	
	length = USB_DT_INTERFACE_SIZE;
	length += SCD_DT_SCD_SIZE;
	for (i = 0; i < NR_ICCD_ENDPS; i++) {
		length += USB_DT_ENDPOINT_SIZE;
	}
	return length;
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

static void iccd_get_intfc_desc(void)
{
	/* IN interface descriptor */
	USBD_INB(NR_ICCD_ENDPS);
	USBD_INB(USB_INTERFACE_CLASS_ICCD);
	USBD_INB(USB_DEVICE_SUBCLASS_NONE);
	USBD_INB(USB_PROTOCOL_ICCD);
	USBD_INB(ICCD_STRING_INTERFACE);
}

static void iccd_get_config_desc(void)
{
	uint8_t i;

	iccd_get_intfc_desc();
	iccd_get_iccd_desc();

	for (i = 0; i < NR_ICCD_ENDPS; i++) {
		USBD_INB(iccd_endpoints[i].bLength);
		USBD_INB(iccd_endpoints[i].bDescriptorType);
		USBD_INB(iccd_endpoints[i].bEndpointAddress | iccd_addr[iccd_cid][i]);
		USBD_INB(iccd_endpoints[i].bmAttributes);
		USBD_INW(usbd_endpoint_size_addr(iccd_addr[iccd_cid][i]));
		/* XXX: ICCD spec says bulk's interval is 0x00.
		 * But usbd stack use this value to drive endpoint. */
		if (iccd_endpoints[i].bmAttributes == USB_ENDP_BULK)
			USBD_INB(ICCD_ENDP_INTERVAL_NONE);
		else
			USBD_INB(iccd_endpoints[i].bInterval);
	}
}

static void iccd_get_string_desc(void)
{
	uint8_t id = LOBYTE(usbd_control_request_value());

	switch (id) {
	case ICCD_STRING_INTERFACE:
		usb_input_device();
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
	case ICCD_REQ_ICC_POWERON:
		iccd_req_icc_poweron();
		break;
	case ICCD_REQ_ICC_POWEROFF:
		iccd_req_icc_poweroff();
		break;
	case ICCD_REQ_XFR_BLOCK:
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
static void __iccd_CmdSuccess_out(void)
{
	iccd_resps[iccd_cid].bStatus = iccd_dev_status();
	iccd_resps[iccd_cid].bError = 0;
}

static void __iccd_CmdFailure_out(uint8_t error, uint8_t status)
{
	iccd_resps[iccd_cid].bStatus = ICCD_CMD_STATUS_FAIL |
				       status;
	iccd_resps[iccd_cid].bError = error;
	iccd_resps[iccd_cid].abRFU3 = 0;
	iccd_resps[iccd_cid].dwLength = 0;
}

static void iccd_CmdFailure_out(uint8_t error)
{
	__iccd_CmdFailure_out(error, iccd_dev_status());
}

static void iccd_SlotStatus_out(void)
{
	__iccd_CmdSuccess_out();
	iccd_resps[iccd_cid].abRFU3 = 0;
	iccd_resps[iccd_cid].dwLength = 0;
}

static void iccd_DataBlock_out(void)
{
	iccd_size_t nr = __iccd_xchg_avail();
	iccd_size_t ne = ICCD_XB_NE;

	__iccd_CmdSuccess_out();
	iccd_resps[iccd_cid].abRFU3 = 0;
	iccd_resps[iccd_cid].dwLength = ne ? (nr < ne ? nr : ne) : nr;
}

static void iccd_IccPowerOn_out(void)
{
	if (usbd_request_handled() == ICCD_HEADER_SIZE) {
		/* reset SCD error */
		ICCD_XB_ERR = __iccd_err_success();
		/* reset Nc */
		ICCD_XB_NC = 0;
		/* Ne should be 32 (ATR) + 1 (TS) */
		ICCD_XB_NE = ICC_ATR_MAX;
		/* reset WI */
		ICCD_XB_WI = 0;
	}
}

static void __iccd_XfrBlock_out(iccd_size_t hdr_size, iccd_size_t blk_size)
{
	iccd_off_t i;
	uint8_t byte = 0;

	if (usbd_request_handled() == hdr_size) {
		/* reset SCD error */
		ICCD_XB_ERR = __iccd_err_success();
		/* reset Nc */
		ICCD_XB_NC = 0;
		/* Ne is from wLevelParameter
		 * TODO: extended APDU level
		 * more efforts
		 */
		ICCD_XB_NE = (iccd_cmds[iccd_cid].abRFU[1] << 8) |
			      iccd_cmds[iccd_cid].abRFU[2];
		/* reset WI */
		ICCD_XB_WI = iccd_cmds[iccd_cid].abRFU[0];
	}
	/* force USB reap on error */
	if (!iccd_dev_success(ICCD_XB_ERR)) {
		return;
	}

	usbd_iter_accel();
	for (i = ICCD_XB_NC; i < blk_size; i++) {
		/* TODO: Bug Fix Required
		 *
		 * Codes here need to be refined using USBD_OUT_BEGIN and
		 * USBD_OUT_END pair.
		 */
		USBD_OUTB(byte);

		/* XXX: USBD_OUTB May Fake Reads 'byte'
		 * Following codes are enabled only when USBD_OUTB actually
		 * gets something into the 'byte', which happens when:
		 * handled-1 == NC+hdr_size.
		 */
		if (usbd_request_handled() != (ICCD_XB_NC + hdr_size + 1)) {
			/* Skip fake readings. */
			continue;
		}

		/* Now byte contains non-fake value. */
		ICCD_XB_ERR = __iccd_xchg_write(ICCD_XB_NC, byte);
		if (!iccd_dev_success(ICCD_XB_ERR)) {
			return;
		}
		ICCD_XB_NC++;
	}
}

static void iccd_XfrBlock_out(void)
{
	__iccd_XfrBlock_out(ICCD_HEADER_SIZE, iccd_cmds[iccd_cid].dwLength);
}

static void iccd_handle_slot_pc2rdr(void)
{
	if (iccd_dev_status() == ICCD_DEV_STATE_NOTPRESENT) {
		iccd_CmdFailure_out(ICCD_ERROR_ICC_MUTE);
		return;
	}

	switch (iccd_cmds[iccd_cid].bMessageType) {
	case ICCD_PC2RDR_ICCPOWERON:
		iccd_IccPowerOn_out();
		break;
	case ICCD_PC2RDR_ICCPOWEROFF:
		/* nothing to do */
		break;
	case ICCD_PC2RDR_XFRBLOCK:
		iccd_XfrBlock_out();
		break;
	}
}

static void iccd_handle_command(iccd_t id)
{
	iccd_t ocid;

	ocid = iccd_cid_save(id);
	scd_debug(SCD_DEBUG_SLOT, id);

	BUG_ON(iccd_devs[iccd_cid].state != ICCD_SLOT_STATE_PC2RDR &&
	       iccd_devs[iccd_cid].state != ICCD_SLOT_STATE_SANITY);

	iccd_dev_enter(ICCD_SLOT_STATE_SANITY);

	/* CCID message header */
	USBD_OUTB(iccd_cmds[iccd_cid].bMessageType);
	USBD_OUTL(iccd_cmds[iccd_cid].dwLength);
	USBD_OUTB(iccd_cmds[iccd_cid].bSlot);
	USBD_OUTB(iccd_cmds[iccd_cid].bSeq);
	USBD_OUTB(iccd_cmds[iccd_cid].abRFU[0]);
	USBD_OUTB(iccd_cmds[iccd_cid].abRFU[1]);
	USBD_OUTB(iccd_cmds[iccd_cid].abRFU[2]);

	if (usbd_request_handled() < ICCD_HEADER_SIZE)
		goto out;

	if (usbd_request_handled() == ICCD_HEADER_SIZE) {
		usbd_request_commit(ICCD_HEADER_SIZE +
				   iccd_cmds[iccd_cid].dwLength);
		scd_debug(SCD_DEBUG_PC2RDR, iccd_cmds[iccd_cid].bMessageType);
	}

	if (iccd_cmds[iccd_cid].bSlot != ICCD_SINGLE_SLOT_IDX)
		goto out;

	iccd_handle_slot_pc2rdr();
out:
	iccd_cid_restore(ocid);
	return;
}

/*=========================================================================
 * bulk-in data
 *=======================================================================*/
static void iccd_RespHeader_in(iccd_size_t length)
{
	USBD_INB(iccd_resp_message());
	USBD_INL(length);
	USBD_INB(iccd_cmds[iccd_cid].bSlot);
	USBD_INB(iccd_cmds[iccd_cid].bSeq);
	USBD_INB(iccd_resps[iccd_cid].bStatus);
	USBD_INB(iccd_resps[iccd_cid].bError);
	USBD_INB(iccd_resps[iccd_cid].abRFU3);
}

void iccd_SlotStatus_in(void)
{
	iccd_RespHeader_in(0);
}

void iccd_DataBlock_in(void)
{
	iccd_off_t i;

	iccd_RespHeader_in(iccd_resps[iccd_cid].dwLength);

	usbd_iter_accel();

	for (i = usbd_request_handled()-ICCD_HEADER_SIZE;
	     i < iccd_resps[iccd_cid].dwLength; i++) {
		USBD_INB(__iccd_xchg_read(i));
	}

	BUG_ON(usbd_request_handled() >
	       iccd_resps[iccd_cid].dwLength + ICCD_HEADER_SIZE);
}

static void iccd_handle_response(iccd_t id)
{
	iccd_t ocid;

	ocid = iccd_cid_save(id);
	scd_debug(SCD_DEBUG_SLOT, iccd_cid);

	BUG_ON(iccd_devs[iccd_cid].state != ICCD_SLOT_STATE_RDR2PC);

	if (iccd_is_cmd_status(ICCD_CMD_STATUS_FAIL)) {
		iccd_SlotStatus_in();
		goto out;
	}


	BUG_ON(iccd_cmds[iccd_cid].bSlot != ICCD_SINGLE_SLOT_IDX);

	switch (iccd_cmds[iccd_cid].bMessageType) {
	case ICCD_PC2RDR_ICCPOWERON:
	case ICCD_PC2RDR_XFRBLOCK:
		iccd_DataBlock_in();
		break;
	case ICCD_PC2RDR_ICCPOWEROFF:
	case ICCD_PC2RDR_GETPARAMETERS:
		iccd_SlotStatus_in();
		break;
	default:
		BUG();
		break;
	}
out:
	iccd_cid_restore(ocid);
}

static void iccd_complete_response(iccd_t id)
{
	iccd_t ocid = iccd_cid_save(id);
	iccd_dev_enter(ICCD_SLOT_STATE_PC2RDR);
	iccd_cid_restore(ocid);
}

/*=========================================================================
 * bulk-out cmpl
 *=======================================================================*/
void iccd_DataBlock_cmp(iccd_err_t err)
{
	iccd_handle_cmp(err, true);
}

static void iccd_SlotStatus_cmp(void)
{
	iccd_SlotStatus_out();
	iccd_CmdResponse_cmp();
}

void iccd_CmdResponse_cmp(void)
{
	iccd_dev_enter(ICCD_SLOT_STATE_RDR2PC);
}

static void iccd_handle_cmp(iccd_err_t err, boolean block)
{
	if (!iccd_dev_progress(err)) {
		if (!iccd_dev_success(err)) {
			iccd_CmdFailure_out(iccd_dev_error(err));
		} else {
			if (block) {
				iccd_DataBlock_out();
			} else {
				iccd_SlotStatus_out();
			}
		}
		iccd_CmdResponse_cmp();
	} else {
		iccd_dev_enter(ICCD_SLOT_STATE_ISO7816);
	}
}

static void iccd_IccPowerOn_cmp(void)
{
	iccd_err_t err;

	err = __iccd_power_on();
	iccd_handle_cmp(err, true);
}

static void iccd_IccPowerOff_cmp(void)
{
	iccd_err_t err;

	err = __iccd_power_off();
	iccd_handle_cmp(err, false);
}

void iccd_SlotNotExist_cmp(void)
{
	__iccd_CmdFailure_out(5, ICCD_DEV_STATE_NOTPRESENT);
	iccd_CmdResponse_cmp();
}

void iccd_CmdOffset_cmp(uint8_t offset)
{
	iccd_CmdFailure_out(offset);
	iccd_CmdResponse_cmp();
}

void iccd_XfrBlock_cmp(void)
{
	iccd_err_t err = ICCD_XB_ERR;

	/* TODO: wLevelParameter check when XCHG_CHAR or XCHG_APDU_EXT */
	if (iccd_dev_success(err) && ICCD_XB_NC != 0) {
		err = __iccd_xchg_block(ICCD_XB_NC, ICCD_XB_NE);
	}
	iccd_handle_cmp(err, true);
}

static void iccd_complete_slot_pc2rdr(void)
{
	if (usbd_request_handled() !=
	    (iccd_cmds[iccd_cid].dwLength + ICCD_HEADER_SIZE)) {
		iccd_CmdOffset_cmp(1);
		return;
	}
	if (iccd_is_cmd_status(ICCD_CMD_STATUS_FAIL)) {
		iccd_CmdResponse_cmp();
		return;
	}

	switch (iccd_cmds[iccd_cid].bMessageType) {
	case ICCD_PC2RDR_ICCPOWERON:
		iccd_IccPowerOn_cmp();
		break;
	case ICCD_PC2RDR_ICCPOWEROFF:
		iccd_IccPowerOff_cmp();
		break;
	case ICCD_PC2RDR_XFRBLOCK:
		iccd_XfrBlock_cmp();
		break;
	case ICCD_PC2RDR_GETPARAMETERS:
		/* FIXME: windows sent us */
		iccd_SlotStatus_cmp();
		break;
	default:
		iccd_CmdOffset_cmp(0);
		break;
	}
}

static void iccd_complete_command(iccd_t id)
{
	iccd_t ocid;
	ocid = iccd_cid_save(id);

	BUG_ON(iccd_devs[iccd_cid].state != ICCD_SLOT_STATE_PC2RDR &&
	       iccd_devs[iccd_cid].state != ICCD_SLOT_STATE_SANITY);

	if (usbd_request_handled() < ICCD_HEADER_SIZE ||
	    iccd_cmds[iccd_cid].bSlot != ICCD_SINGLE_SLOT_IDX) {
		iccd_SlotNotExist_cmp();
		goto out;
	}

	if (iccd_is_cmd_status(ICCD_CMD_STATUS_FAIL)) {
		/* CMD_ABORTED and SLOT_BUSY error */
		iccd_CmdResponse_cmp();
		goto out;
	}

	/* SANITY state completed */
	iccd_dev_enter(ICCD_SLOT_STATE_PC2RDR);

	iccd_complete_slot_pc2rdr();
out:
	iccd_cid_restore(ocid);
}

/*=========================================================================
 * interrupt data
 *=======================================================================*/
#ifdef CONFIG_ICCD_INTERRUPT_IN
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
	for (id = 0; id < NR_ICCD_DEVICES; id++) {
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

static void iccd_change_discard(iccd_t id)
{
	if (__iccd_change_running(id)) {
		clear_bit(ICCD_INTR_CHANGE(id), iccd_running_intrs);
		scd_debug(SCD_DEBUG_INTR, ICCD_INTR_RUNNING_UNSET);
	}
}

static void iccd_change_submit(void)
{
	iccd_t id;

	for (id = 0; id < NR_ICCD_DEVICES; id++) {
		/* copy changed bits */
		clear_bit(ICCD_INTR_CHANGE(id), iccd_pending_intrs);
		scd_debug(SCD_DEBUG_INTR, ICCD_INTR_PENDING_UNSET);
		set_bit(ICCD_INTR_CHANGE(id), iccd_running_intrs);
		scd_debug(SCD_DEBUG_INTR, ICCD_INTR_RUNNING_SET);

		/* copy status bits */
		if (test_bit(ICCD_INTR_STATUS(id), iccd_pending_intrs)) {
			set_bit(ICCD_INTR_STATUS(id), iccd_running_intrs);
			scd_debug(SCD_DEBUG_INTR, ICCD_DEV_STATE_PRESENT);
		} else {
			clear_bit(ICCD_INTR_STATUS(id), iccd_running_intrs);
			scd_debug(SCD_DEBUG_INTR, ICCD_DEV_STATE_NOTPRESENT);
		}
	}
}

static void iccd_change_raise(void)
{
	boolean changed = false;
	if (iccd_dev_status() == ICCD_DEV_STATE_NOTPRESENT) {
		if (test_bit(ICCD_INTR_STATUS(iccd_cid), iccd_pending_intrs)) {
			clear_bit(ICCD_INTR_STATUS(iccd_cid),
				  iccd_pending_intrs);
			changed = true;
		}
	} else {
		if (!test_bit(ICCD_INTR_STATUS(iccd_cid), iccd_pending_intrs)) {
			set_bit(ICCD_INTR_STATUS(iccd_cid), iccd_pending_intrs);
			changed = true;
		}
	}
	if (changed) {
		set_bit(ICCD_INTR_CHANGE(iccd_cid), iccd_pending_intrs);
		scd_debug(SCD_DEBUG_INTR, ICCD_INTR_PENDING_SET);
	}
}

static uint16_t iccd_change_length(void)
{
	return 1 + div16u(ALIGN(NR_ICCD_DEVICES, 4), 4);
}

static void iccd_change_init(void)
{
}

#define iccd_handle_interrupt()		iccd_change_data()
#define iccd_intr_init()		iccd_change_init()
#define iccd_complete_interrupt(id)	iccd_change_discard(id)

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
	iccd_t ocid = iccd_cid_save(iccd_id);
	iccd_change_raise();
	iccd_cid_restore(ocid);
}

static void iccd_intr_start(void)
{
	iccd_t id, ocid;

	for (id = 0; id < NR_ICCD_DEVICES; id++) {
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

	for (id = 0; id < NR_ICCD_DEVICES; id++) {
		for (i = 0; i < NR_ICCD_ENDPS; i++) {
			if (addr == iccd_addr[iccd_cid][i])
				return id;
		}
	}
	return NR_ICCD_DEVICES;
}

/*=========================================================================
 * ICCD entrances
 *=======================================================================*/
static void iccd_handle_endp_done(void)
{
	iccd_t id = iccd_addr2id(usbd_saved_addr());

	if (usbd_saved_addr() == ICCD_ADDR_OUT) {
		iccd_complete_command(id);
	}
	if (usbd_saved_addr() == ICCD_ADDR_IN) {
		iccd_complete_response(id);
	}
#ifdef CONFIG_ICCD_INTERRUPT_IN
	if (usbd_saved_addr() == ICCD_ADDR_IRQ) {
		iccd_complete_interrupt(id);
	}
#endif
}

static void iccd_handle_endp_iocb(void)
{
	iccd_t id = iccd_addr2id(usbd_saved_addr());

	if (usbd_saved_addr() == ICCD_ADDR_OUT) {
		iccd_handle_command(id);
		return;
	}
	if (usbd_saved_addr() == ICCD_ADDR_IN) {
		iccd_handle_response(id);
		return;
	}
#ifdef CONFIG_ICCD_INTERRUPT_IN
	if (usbd_saved_addr() == ICCD_ADDR_IRQ) {
		iccd_handle_interrupt();
		return;
	}
#endif
}

static void iccd_handle_endp_poll(void)
{
	iccd_t id = iccd_addr2id(usbd_saved_addr());

	if (usbd_saved_addr() == ICCD_ADDR_OUT) {
		iccd_submit_command(id);
	}
	if (usbd_saved_addr() == ICCD_ADDR_IN) {
		iccd_submit_response(id);
	}
#ifdef CONFIG_ICCD_INTERRUPT_IN
	if (usbd_saved_addr() == ICCD_ADDR_IRQ) {
		iccd_submit_interrupt();
	}
#endif
}

/* TODO: how to know the slot number we should answer */
static void iccd_handle_ll_cmpl(void)
{
	iccd_err_t err = __iccd_get_error();
	BUG_ON(iccd_devs[iccd_cid].state != ICCD_SLOT_STATE_ISO7816);

	switch (iccd_cmds[iccd_cid].bMessageType) {
	case ICCD_PC2RDR_ICCPOWERON:
	case ICCD_PC2RDR_XFRBLOCK:
		iccd_DataBlock_cmp(err);
		break;
	case ICCD_PC2RDR_ICCPOWEROFF:
		iccd_SlotStatus_cmp();
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

usbd_endpoint_t usb_iccd_endpoint = {
	iccd_handle_endp_poll,
	iccd_handle_endp_iocb,
	iccd_handle_endp_done,
};

usbd_interface_t usb_iccd_interface = {
	ICCD_STRING_FIRST,
	ICCD_STRING_LAST,
	iccd_handle_ctrl_data,
	iccd_config_length,
};

static void iccd_claim_endp(iccd_t id)
{
	uint8_t addr;
	uint8_t type;

	for (type = 0; type < NR_ICCD_ENDPS; type++) {
		addr = usbd_claim_endpoint(iccd_endpoints[type].bmAttributes,
					   USB_ADDR2DIR(iccd_endpoints[type].bEndpointAddress),
					   iccd_endpoints[type].bInterval,
					   true,
					   &usb_iccd_endpoint);
		iccd_addr[id][type] = addr;
	}
}

static void iccd_dev_init(void)
{
	iccd_t id;
	
	/* USB interface and endpoint stuff */
	for (id = 0; id < NR_ICCD_DEVICES; id++) {
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
