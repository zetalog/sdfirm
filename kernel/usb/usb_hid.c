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
 * @(#)usb_hid.c: human interface device functions
 * $Id: usb_hid.c,v 1.44 2011-04-21 09:47:19 zhenglv Exp $
 */

/* TODO:
 * 1. field registeration
 * 2. SET_PROTOCOL/GET_PROTOCOL support
 * 3. report ID in report format support
 */

#include <target/usb_hid.h>
#include <target/kbd.h>

#define HID_ADDR_IN	hid_addr[HID_ENDP_IN]
#define HID_ADDR_OUT	hid_addr[HID_ENDP_OUT]

static void hid_handle_in_data(void);
static void hid_handle_out_data(void);
static void hid_handle_ctrl_data(void);
static void hid_handle_standard_request(void);
static void hid_handle_class_request(void);

static void hid_get_descriptor(void);
#define hid_set_descriptor()		usbd_endpoint_halt()
static void hid_get_report(void);
static void hid_set_report(void);
#define hid_get_protocol()		usbd_endpoint_halt()
#define hid_set_protocol()		usbd_endpoint_halt()

#define HID_STRING_FIRST		20
#define HID_STRING_INTERFACE		HID_STRING_FIRST+0
#define HID_STRING_LAST			HID_STRING_INTERFACE

usb_endp_desc_t hid_endpoints[NR_HID_ENDPS] = {
	{
		USB_DT_ENDPOINT_SIZE,
		USB_DT_ENDPOINT,
		USB_DIR2ADDR(USB_DIR_IN) | 0,	/* !bEndpointAddress */
		USB_ENDP_INTERRUPT,		/* bmAttributes */
		0,				/* !wMaxPacketSize */
		HID_ENDP_INTERVAL,
	},
#if NR_HID_ENDPS > 1
	/* optional */
	{
		USB_DT_ENDPOINT_SIZE,
		USB_DT_ENDPOINT,
		USB_DIR2ADDR(USB_DIR_OUT) | 0,	/* !bEndpointAddress */
		USB_ENDP_INTERRUPT,		/* bmAttributes */
		0,				/* !wMaxPacketSize */
		HID_ENDP_INTERVAL,
	},
#endif
};

uint8_t hid_nr_reports = 0;
uint8_t hid_addr[NR_HID_ENDPS];
struct hid_report_ctrl hid_rept_ctrls[NR_HID_REPORTS];

hid_report_t *hid_reports[NR_HID_REPORTS];

#ifdef CONFIG_HID_DURATION
static void hid_get_idle(void)
{
	uint8_t reportid = LOBYTE(usbd_control_request_value());

	if (hid_report_registered(reportid)) {
		USBD_INB(hid_rept_ctrls[HID_REPORTRID(reportid)].duration);
	} else {
		usbd_endpoint_halt();
	}
}

static void hid_set_idle(void)
{
	uint8_t duration = HIBYTE(usbd_control_request_value());
	uint8_t reportid = LOBYTE(usbd_control_request_value());
	hid_rid_t rid;

	if (reportid == HID_REPORTID_ALL) {
		for (rid = 0; rid < hid_nr_reports; rid++) {
			hid_rept_ctrls[rid].duration = duration;
		}
	} else if (hid_report_registered(reportid)) {
		hid_rept_ctrls[HID_REPORTRID(reportid)].duration = duration;
	} else {
		usbd_endpoint_halt();
	}
}

void hid_init_duration(hid_rid_t rid, uint8_t duration)
{
	hid_rept_ctrls[rid].duration = duration;
	hid_rept_ctrls[rid].last_poll = 0;
}

void hid_poll_duration(hid_rid_t rid)
{
	if (hid_rept_ctrls[rid].duration) {
		uint16_t span = jiffies - hid_rept_ctrls[rid].last_poll;
		/* span * 1/HZ (s) * 1000 (ms/s) > 4 (ms) * duration */
		if (mul16u(span, div16u(1000, HZ)) >
		    mul16u(4, hid_rept_ctrls[rid].duration)) {
			hid_rept_ctrls[rid].last_poll = jiffies;
			hid_raise_interrupt(rid);
		}
	}
}
#else
#define hid_poll_duration(rid)
#define hid_init_duration(rid, duration)
#define hid_get_idle()				USBD_INB(HID_DURATION_NEVER)
#define hid_set_idle()
#endif

/*=========================================================================
 * report APIs
 *=======================================================================*/
hid_rid_t hid_register_report(uint8_t duration, hid_report_t *creport)
{
	hid_rid_t rid = hid_nr_reports;
	if (rid < NR_HID_REPORTS) {
		hid_reports[rid] = creport;

		hid_init_duration(rid, duration);
		hid_rept_ctrls[rid].flags = 0;
		hid_nr_reports++;
	}
	return rid;
}

boolean hid_report_registered(uint8_t reportid)
{
	return HID_REPORTRID(reportid) < hid_nr_reports;
}

static void hid_report_input_data(hid_rid_t rid)
{
	hid_report_t *creport;
	creport = hid_reports[rid];
	if (creport->iocb)
		creport->iocb(HID_ENDP_IN);
}

static void hid_report_output_data(hid_rid_t rid)
{
	hid_report_t *creport;
	creport = hid_reports[rid];
	if (creport->iocb)
		creport->iocb(HID_ENDP_OUT);
}

/*=========================================================================
 * config endpoint
 *=======================================================================*/
static uint8_t hid_descriptor_length(void)
{
	hid_rid_t i;
	uint8_t length = sizeof (hid_desc_t);

	for (i = 0; i < hid_nr_reports; i++) {
		length += HID_DT_HID_OPT_SIZE;
	}
	return length;
}

static uint16_t hid_config_length(void)
{
	uint8_t i;
	uint16_t length;
	
	length = sizeof (usb_intf_desc_t);
	length += hid_descriptor_length();
	for (i = 0; i < NR_HID_ENDPS; i++) {
		length += USB_DT_ENDPOINT_SIZE;
	}
	return length;
}

static uint16_t hid_opt_descriptor_length(hid_rid_t rid)
{
	uint8_t length = 0;
	hid_report_t *creport;

	creport = hid_reports[rid];
	BUG_ON(!creport->config_len);
	length += creport->config_len();
	return length;
}

static void hid_get_hid_desc(void)
{
	hid_rid_t i;

	USBD_INB(hid_descriptor_length());
	USBD_INB(HID_DT_HID);
	USBD_INW(HID_VERSION_DEFAULT);
	USBD_INB(0);
	USBD_INB(hid_nr_reports);
	for (i = 0; i < hid_nr_reports; i++) {
		USBD_INB(HID_DT_REPORT);
		USBD_INW(hid_opt_descriptor_length(i));
	}
}

static void hid_get_intfc_desc(void)
{
	/* IN interface descriptor */
	USBD_INB(NR_HID_ENDPS);
	USBD_INB(USB_INTERFACE_CLASS_HID);
	USBD_INB(USB_INTERFACE_SUBCLASS_BOOT);
	USBD_INB(USB_INTERFACE_PROTOCOL_KEYBOARD);
	USBD_INB(HID_STRING_INTERFACE);
}

static void hid_get_config_desc(void)
{
	hid_rid_t i;

	hid_get_intfc_desc();
	hid_get_hid_desc();

	for (i = 0; i < NR_HID_ENDPS; i++) {
		USBD_INB(hid_endpoints[i].bLength);
		USBD_INB(hid_endpoints[i].bDescriptorType);
		USBD_INB(hid_endpoints[i].bEndpointAddress | hid_addr[i]);
		USBD_INB(hid_endpoints[i].bmAttributes);
		USBD_INW(usbd_endpoint_size_addr(hid_addr[i]));
		USBD_INB(hid_endpoints[i].bInterval);
	}
}

static void hid_get_string_desc(void)
{
	uint8_t id = LOBYTE(usbd_control_request_value());

	switch (id) {
	case HID_STRING_INTERFACE:
		usb_input_device();
		break;
	default:
		USBD_INB(0x00);
		break;
	}
}

static void hid_get_report_desc(hid_rid_t rid)
{
	hid_report_t *creport;
	if (rid < hid_nr_reports) {
		creport = hid_reports[rid];
		BUG_ON(!creport->ctrl);
		creport->ctrl();
	}
}

static void hid_get_descriptor(void)
{
	uint8_t desc, index;
	
	desc = HIBYTE(usbd_control_request_value());
	index = LOBYTE(usbd_control_request_value());

	hid_debug(HID_DEBUG_DESC, desc);

	switch (desc) {
	case USB_DT_CONFIG:
		hid_get_config_desc();
		break;

	case USB_DT_STRING:
		hid_get_string_desc();
		break;

	/* following CS descriptors should be returned only when
	 * RECIP = INTERFACE
	 */
	case HID_DT_HID:
		if (usbd_control_setup_recp() == USB_RECP_INTERFACE)
			hid_get_hid_desc();
		else
			usbd_endpoint_halt();
		break;

	case HID_DT_REPORT:
		if (usbd_control_setup_recp() == USB_RECP_INTERFACE)
			hid_get_report_desc(index);
		else
			usbd_endpoint_halt();
		break;
	default:
		usbd_endpoint_halt();
		break;
	}
}

static void hid_get_report(void)
{
	uint8_t repttype = HIBYTE(usbd_control_request_value());
	uint8_t reportid = LOBYTE(usbd_control_request_value());

	if (reportid == HID_REPORTID_ALL) {
		switch (repttype) {
		case HID_REPORT_INPUT:
			hid_handle_in_data();
			return;
		}
	} else if (hid_report_registered(reportid)) {
		switch (repttype) {
		case HID_REPORT_INPUT:
			hid_report_input_data(HID_REPORTRID(reportid));
			return;
		}
	}
	usbd_endpoint_halt();
}

static void hid_set_report(void)
{
	uint8_t repttype = HIBYTE(usbd_control_request_value());
	uint8_t reportid = LOBYTE(usbd_control_request_value());

	if (reportid == HID_REPORTID_ALL) {
		switch (repttype) {
		case HID_REPORT_OUTPUT:
			hid_handle_out_data();
			return;
		}
	} else if (hid_report_registered(reportid)) {
		switch (repttype) {
		case HID_REPORT_OUTPUT:
			hid_report_output_data(HID_REPORTRID(reportid));
			return;
		}
	}
	usbd_endpoint_halt();
}

static void hid_handle_standard_request(void)
{
	uint8_t req = usbd_control_request_type();

	hid_debug(HID_DEBUG_CS_REQ, req);

	switch (req) {
	case USB_REQ_GET_DESCRIPTOR:
		hid_get_descriptor();
		break;
	case USB_REQ_SET_DESCRIPTOR:
		hid_set_descriptor();
		break;
	default:
		usbd_endpoint_halt();
		break;
	}
}

static void hid_handle_class_request(void)
{
	uint8_t req = usbd_control_request_type();

	hid_debug(HID_DEBUG_CS_REQ, req);

	switch (req) {
	case HID_REQ_GET_REPORT:
		hid_get_report();
		break;
	case HID_REQ_GET_IDLE:
		hid_get_idle();
		break;
	case HID_REQ_GET_PROTOCOL:
		hid_get_protocol();
		break;
	case HID_REQ_SET_REPORT:
		hid_set_report();
		break;
	case HID_REQ_SET_IDLE:
		hid_set_idle();
		break;
	case HID_REQ_SET_PROTOCOL:
		hid_set_protocol();
		break;
	default:
		usbd_endpoint_halt();
		break;
	}
}

static void hid_handle_ctrl_data(void)
{
	uint8_t type = usbd_control_setup_type();
	uint8_t recp = usbd_control_setup_recp();

	switch (recp) {
	case USB_RECP_DEVICE:
		switch (type) {
		case USB_TYPE_STANDARD:
			hid_handle_standard_request();
			return;
		}
		break;
	case USB_RECP_INTERFACE:
		switch (type) {
		case USB_TYPE_STANDARD:
			hid_handle_standard_request();
			return;
		case USB_TYPE_CLASS:
			hid_handle_class_request();
			return;
		}
		break;
	}
	usbd_endpoint_halt();
}

uint16_t hid_item_length(uint8_t format, uint8_t size)
{
	uint16_t length = 1 + size;

	if (format == HID_ITEM_FORMAT_LONG)
		length += 2;
	return length;
}

uint8_t hid_item_length_value(uint8_t length)
{
	if (length > 2)
		return 3;
	else
		return length;
}

void hid_in_head(uint8_t format, uint8_t size,
		 uint8_t type, uint8_t tag)
{
	switch (format) {
	case HID_ITEM_FORMAT_LONG:
		USBD_INB(HID_LONG_ITEM_LEADING);
		USBD_INB(size);
		USBD_INB(tag);
		break;
	case HID_ITEM_FORMAT_SHORT:
		USBD_INB((hid_item_length_value(size) << 0) |
			 ((type & 0x03) << 2) |
			 ((tag  & 0x0f) << 4));
		break;
	}
}

void hid_in_short(uint8_t format, uint8_t size, uint32_t udata)
{
	if (format == HID_ITEM_FORMAT_SHORT) {
		switch (size) {
		case 1:
			USBD_INB(LOBYTE(LOWORD(udata)));
			break;
		case 2:
			USBD_INW(LOWORD(udata));
			break;
		case 4:
			USBD_INL(udata);
			break;
		}
	} else {
		/* TODO: HID_ITEM_FORMAT_LONG not supported */
		BUG();
	}
}

/*=========================================================================
 * interrupt endpoint
 *=======================================================================*/
void hid_raise_interrupt(hid_rid_t rid)
{
	if (rid < hid_nr_reports) {
		hid_debug(HID_DEBUG_REPORT, rid);
		/* this function should only be called from poll callback */
		BUG_ON(hid_interrupt_busy(rid));
		hid_rept_ctrls[rid].flags |= HID_REPORT_PENDING;
		hid_debug(HID_DEBUG_INTR, HID_INTR_PENDING_SET);
	}
}

boolean hid_interrupt_busy(hid_rid_t rid)
{
	return hid_rept_ctrls[rid].flags & (HID_REPORT_RUNNING | HID_REPORT_PENDING);
}

static boolean hid_interrupt_running(hid_rid_t rid)
{
	return hid_rept_ctrls[rid].flags & HID_REPORT_RUNNING;
}

static boolean hid_interrupt_pending(hid_rid_t rid)
{
	return hid_rept_ctrls[rid].flags & HID_REPORT_PENDING;
}

static void hid_poll_interrupt(hid_rid_t rid)
{
	hid_poll_duration(rid);

	if (!hid_interrupt_busy(rid)) {
		hid_report_t *creport;
		creport = hid_reports[rid];
		if (creport->poll)
			creport->poll(HID_ENDP_IN);
	}
}

static void hid_discard_interrupt(hid_rid_t rid)
{
	hid_report_t *creport;

	hid_debug(HID_DEBUG_REPORT, rid);
	creport = hid_reports[rid];
	if (creport->done)
		creport->done(HID_ENDP_IN);
	hid_rept_ctrls[rid].flags &= ~HID_REPORT_RUNNING;
	hid_debug(HID_DEBUG_INTR, HID_INTR_RUNNING_UNSET);
}

urb_size_t hid_output_length(void)
{
	hid_rid_t rid;
	uint16_t length = 0;
	hid_report_t *creport;

	/* idle support */
	for (rid = 0; rid < hid_nr_reports; rid++) {
		creport = hid_reports[rid];
		length += creport->output_len;
	}
	return length;
}

urb_size_t hid_input_length(void)
{
	hid_rid_t rid;
	urb_size_t length = 0;
	hid_report_t *creport;

	/* idle support */
	for (rid = 0; rid < hid_nr_reports; rid++) {
		creport = hid_reports[rid];
		length += creport->input_len;
	}
	return length;
}

boolean hid_input_running(void)
{
	hid_rid_t rid;
	for (rid = 0; rid < hid_nr_reports; rid++) {
		if (hid_interrupt_running(rid))
			return true;
	}
	return false;
}

boolean hid_input_pending(void)
{
	hid_rid_t rid;
	for (rid = 0; rid < hid_nr_reports; rid++) {
		if (hid_interrupt_pending(rid))
			return true;
	}
	return false;
}

void hid_input_poll(void)
{
	hid_rid_t rid;
	for (rid = 0; rid < hid_nr_reports; rid++) {
		hid_poll_interrupt(rid);
	}
}

void hid_input_discard(void)
{
	hid_rid_t rid;
	for (rid = 0; rid < hid_nr_reports; rid++) {
		if (hid_interrupt_running(rid)) {
			hid_discard_interrupt(rid);
		}
	}
}

void hid_input_submit(void)
{
	if (hid_input_pending()) {
		if (usbd_request_submit(HID_ADDR_IN,
					hid_input_length())) {
			hid_rid_t rid;
			for (rid = 0; rid < hid_nr_reports; rid++) {
				hid_debug(HID_DEBUG_REPORT, rid);
				/* move pending to running */
				hid_rept_ctrls[rid].flags &= ~HID_REPORT_PENDING;
				hid_debug(HID_DEBUG_INTR, HID_INTR_PENDING_UNSET);
				hid_rept_ctrls[rid].flags |= HID_REPORT_RUNNING;
				hid_debug(HID_DEBUG_INTR, HID_INTR_RUNNING_SET);
			}
		}
	}
}

static void hid_handle_endp_poll(void)
{
	if (usbd_saved_addr() == HID_ADDR_IN) {
		hid_input_poll();
		hid_input_submit();
	}
	if (usbd_saved_addr() == HID_ADDR_OUT) {
		/* allow next out transfer */
		usbd_request_submit(HID_ADDR_OUT, hid_output_length());
	}
}

static void hid_handle_endp_done(void)
{
	if (usbd_saved_addr() == HID_ADDR_IN) {
		hid_input_discard();
	}
	/* nothing to do for OUT status stage */
}

static void hid_handle_in_data(void)
{
	hid_rid_t rid;
	for (rid = 0; rid < hid_nr_reports; rid++) {
		hid_report_input_data(rid);
	}
}

static void hid_handle_out_data(void)
{
	hid_rid_t rid;
	for (rid = 0; rid < hid_nr_reports; rid++) {
		hid_report_output_data(rid);
	}
}

static void hid_handle_endp_iocb(void)
{
	if (usbd_saved_addr() == HID_ADDR_IN) {
		hid_handle_in_data();
		return;
	}
	if (usbd_saved_addr() == HID_ADDR_OUT) {
		hid_handle_out_data();
		return;
	}
	BUG();
}

static void hid_start(void)
{
	hid_rid_t rid;
	/* report once */
	for (rid = 0; rid < hid_nr_reports; rid++)
		hid_raise_interrupt(rid);
}

usbd_endpoint_t usb_hid_endpoint = {
	hid_handle_endp_poll,
	hid_handle_endp_iocb,
	hid_handle_endp_done,
};

usbd_interface_t usb_hid_interface = {
	HID_STRING_FIRST,
	HID_STRING_LAST,
	hid_handle_ctrl_data,
	hid_config_length,
};

void hid_init(void)
{
	uint8_t i;

	usbd_declare_interface(50, &usb_hid_interface);
	for (i = 0; i < NR_HID_ENDPS; i++) {
		hid_addr[i] = usbd_claim_endpoint(hid_endpoints[i].bmAttributes,
						  USB_ADDR2DIR(hid_endpoints[i].bEndpointAddress),
						  hid_endpoints[i].bInterval,
						  true,
						  &usb_hid_endpoint);
	}
	hid_kbd_init();
	hid_start();
}
