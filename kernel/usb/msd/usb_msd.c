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
 * @(#)usb_msd.c: mass storage device functions
 * $Id: usb_msd.c,v 1.44 2011-09-08 02:11:10 zhenglv Exp $
 */

/* TODO:
 */

#include <target/usb_msd.h>

uint8_t msd_addr[NR_MSD_ENDPS];

#define MSD_STRING_FIRST		30
#define MSD_STRING_INTERFACE		MSD_STRING_FIRST+0
#define MSD_STRING_LAST			MSD_STRING_INTERFACE

static void msd_get_config_desc(void)
{
	uint8_t i;

	usbd_input_interface_desc(USB_INTERFACE_CLASS_MSD,
				  MSD_INTERFACE_SUBCLASS,
				  MSD_INTERFACE_PROTOCOL,
				  MSD_STRING_INTERFACE);

	for (i = 0; i < NR_MSD_ENDPS; i++) {
		usbd_input_endpoint_desc(msd_addr[i]);
	}
}

static void msd_get_string_desc(void)
{
	uint8_t id = LOBYTE(usbd_control_request_value());

	switch (id) {
	case MSD_STRING_INTERFACE:
		usbd_input_device_name();
		break;
	default:
		USBD_INB(0x00);
		break;
	}
}

static void msd_get_descriptor(void)
{
	uint8_t desc;
	
	desc = HIBYTE(usbd_control_request_value());

	switch (desc) {
	case USB_DT_CONFIG:
		msd_get_config_desc();
		break;
	case USB_DT_STRING:
		msd_get_string_desc();
		break;
	default:
		usbd_endpoint_halt();
		break;
	}
}

#define msd_set_descriptor()		usbd_endpoint_halt()

static void msd_handle_standard_request(void)
{
	uint8_t req = usbd_control_request_type();

	switch (req) {
	case USB_REQ_GET_DESCRIPTOR:
		msd_get_descriptor();
		break;
	case USB_REQ_SET_DESCRIPTOR:
		msd_set_descriptor();
		break;
	default:
		usbd_endpoint_halt();
	}
}

static void msd_handle_ctrl_data(void)
{
	uint8_t type = usbd_control_setup_type();
	uint8_t recp = usbd_control_setup_recp();

	switch (recp) {
	case USB_RECP_DEVICE:
		switch (type) {
		case USB_TYPE_STANDARD:
			msd_handle_standard_request();
			return;
		}
		break;
	case USB_RECP_INTERFACE:
		switch (type) {
		case USB_TYPE_STANDARD:
			msd_handle_standard_request();
			return;
		case USB_TYPE_CLASS:
			msd_proto_ctrl();
			return;
		}
		break;
	}
	usbd_endpoint_halt();
}

static uint16_t msd_config_length(void)
{
	return 0;
}

uint8_t msd_proto_bulk_type(void)
{
	return BULK_TYPE_CPU;
}

bulk_size_t msd_proto_bulk_size(uint8_t type)
{
	return usbd_endpoint_size();
}

boolean msd_proto_bulk_idle(void)
{
	return true;
}

#ifdef CONFIG_USB_USBIP_DEV
uint8_t msd_handle_intf_attr(uint8_t type)
{
	switch (type) {
	case 0:
		return USB_INTERFACE_CLASS_MSD;
	case 1:
		return MSD_INTERFACE_SUBCLASS;
	case 2:
		return MSD_INTERFACE_PROTOCOL;
	default:
		BUG();
		return 0;
	}
}
#endif

usbd_interface_t usb_msd_interface = {
	MSD_STRING_FIRST,
	MSD_STRING_LAST,
	NR_MSD_ENDPS,
	msd_config_length,
	msd_handle_ctrl_data,
#ifdef CONFIG_USB_USBIP_DEV
	msd_handle_intf_attr,
#endif
};

void msd_start(void)
{
	msd_proto_start();
	msd_class_start();
}

void msd_init(void)
{
	usbd_declare_interface(&usb_msd_interface);

	msd_proto_init();
	msd_class_init();

	msd_start();
}
