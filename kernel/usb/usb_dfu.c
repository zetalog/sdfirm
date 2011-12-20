/*
 * ZETALOG's Personal COPYRIGHT
 *
 * Copyright (c) 2011
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
 * @(#)usb_dfu.c: device firmware upgrade device functions
 * $Id: usb_dfu.c,v 1.137 2011-10-17 01:40:34 zhenglv Exp $
 */
#include <target/usb_dfu.h>
#include <target/crc32_table.h>

#define BOARD_HW_FIRMWARE_SIZE			0x8000
#define BOARD_HW_TRANSFER_SIZE			200

#define board_hw_firmware_size()		BOARD_HW_FIRMWARE_SIZE
#define board_hw_firmware_manifest(size)
#define board_hw_firmware_write(n, block, length)
#define board_hw_firmware_read(n, block)

#define DFU_DETACH_TIMEOUT		10
#define DFU_POLL_TIMEOUT		((uint32_t)10)
#define DFU_TRANSFER_SIZE		BOARD_HW_TRANSFER_SIZE

#define DFU_STRING_FIRST		40
#define DFU_STRING_INTERFACE		DFU_STRING_FIRST+0
#define DFU_STRING_STATUS_BEGIN		DFU_STRING_INTERFACE+1
#define DFU_STRING_STATUS_END		DFU_STRING_STATUS_BEGIN+DFU_MAX_ERRORS
#define DFU_STRING_LAST			DFU_STRING_STATUS_END

uint8_t dfu_proto;
uint8_t dfu_state;
uint8_t dfu_error;
uint8_t dfu_suffix[DFU_SUFFIX_SIZE];
uint8_t dfu_suffix_length;
uint32_t dfu_crc;
uint16_t dfu_upload_size;
uint16_t dfu_dnload_size;
uint8_t dfu_block[DFU_TRANSFER_SIZE];

void dfu_set_state(uint8_t state)
{
	dfu_debug(DFU_DEBUG_STATE, state);
	dfu_state = state;
}

void dfu_set_error(uint8_t error)
{
	dfu_debug(DFU_DEBUG_ERROR, error);
	dfu_error = error;

	if (error != DFU_STATUS_OK) {
		if (dfu_proto == DFU_INTERFACE_PROTOCOL_DFUMODE)
			dfu_set_state(DFU_STATE_DFU_ERROR);
		usbd_endpoint_halt();
	}
}

#ifdef CONFIG_DFU_AUTO_RESET
#define DFU_ATTR_AUTO_RESET		(DFU_ATTR_WILL_DETACH | \
					 DFU_ATTR_MANIFESTATION_TOLERANT)
void dfu_auto_reset(uint8_t proto)
{
	if (proto == DFU_INTERFACE_PROTOCOL_DFUMODE) {
		dfu_proto = DFU_INTERFACE_PROTOCOL_DFUMODE;
		dfu_set_state(DFU_STATE_DFU_IDLE);
	} else {
		dfu_proto = DFU_INTERFACE_PROTOCOL_RUNTIME;
		dfu_set_state(DFU_STATE_APP_IDLE);
	}
	usbd_restart();
	/* TODO: Toggle bootloader / application */
}
#else
#define DFU_ATTR_AUTO_RESET		0
void dfu_auto_reset(uint8_t proto)
{
	if (proto == DFU_INTERFACE_PROTOCOL_DFUMODE) {
		dfu_set_state(DFU_STATE_APP_DETACH);
	} else {
		dfu_set_state(DFU_STATE_DFU_MANIFEST_WAIT_RESET);
	}
	/* TODO: Toggle bootloader / application */
}
#endif

#if defined(CONFIG_DFU_RUNTIME) || defined(CONFIG_DFU_FULLCMD)
static void dfu_detach(void)
{
	uint16_t tout_ms = usbd_control_request_value();

	if (dfu_proto != DFU_INTERFACE_PROTOCOL_RUNTIME ||
	    dfu_state != DFU_STATE_APP_IDLE ||
	    tout_ms > DFU_DETACH_TIMEOUT) {
		dfu_set_error(DFU_STATUS_ERR_STALLEDPKT);
		return;
	}

	dfu_auto_reset(DFU_INTERFACE_PROTOCOL_DFUMODE);
}
#else
#define dfu_detach()		dfu_set_error(DFU_STATUS_ERR_STALLEDPKT)
#endif

#if defined(CONFIG_DFU_DFUMODE) || defined(CONFIG_DFU_FULLCMD)
void dfu_suffix_writew(uint8_t *buf, uint16_t val)
{
	buf[0] = LOBYTE(val);
	buf[1] = HIBYTE(val);
}

void dfu_suffix_writel(uint8_t *buf, uint32_t val)
{
	dfu_suffix_writew(buf, LOWORD(val));
	dfu_suffix_writew(buf+2, HIWORD(val));
}

uint16_t dfu_suffix_readw(const uint8_t *buf)
{
	return MAKEWORD(buf[0], buf[1]);
}

uint32_t dfu_suffix_readl(const uint8_t *buf)
{
	return MAKELONG(dfu_suffix_readw(buf),
			dfu_suffix_readw(buf+2));
}

uint8_t dfu_suffix_validate(void)
{
	if (dfu_suffix_readw(dfu_suffix+6) > DFU_VERSION)
		return 6;
	if (dfu_suffix_readl(dfu_suffix+8) != DFU_SUFFIX_MAGIC)
		return 8;
	return DFU_SUFFIX_SIZE;
}

void dfu_crc_update(uint32_t *crc, const uint8_t *buf, uint16_t len)
{
	size_t i;
	for (i = 0; i < len; i++)
		crc32_le_byte(*crc, buf[i]);
}

static void dfu_dnload(void)
{
	uint16_t blknr = usbd_control_request_value();
	uint16_t length = usbd_control_request_length();
	uint16_t i, i_suffix;
	uint8_t val;

	if (dfu_proto != DFU_INTERFACE_PROTOCOL_DFUMODE ||
	    (dfu_state != DFU_STATE_DFU_IDLE &&
	     dfu_state != DFU_STATE_DFU_DNLOAD_IDLE &&
	     dfu_state != DFU_STATE_DFU_DNLOAD_SYNC)) {
		dfu_set_error(DFU_STATUS_ERR_STALLEDPKT);
		return;
	}
	if (length > DFU_TRANSFER_SIZE) {
		dfu_set_error(DFU_STATUS_ERR_ADDRESS);
		return;
	}

	if (usbd_request_handled() == 0) {
		if (dfu_state == DFU_STATE_DFU_IDLE) {
			dfu_crc = 0xFFFFFFFF;
			dfu_suffix_length = 0;
			dfu_dnload_size = 0;
		}
	}

	if (length > 0) {
		dfu_set_state(DFU_STATE_DFU_DNLOAD_SYNC);
		if (length < DFU_SUFFIX_SIZE) {
			if (usbd_request_handled() == 0) {
				if ((dfu_suffix_length + length) > DFU_SUFFIX_SIZE) {
					dfu_suffix_length -= (DFU_SUFFIX_SIZE - length);
					for (i = 0; i < DFU_SUFFIX_SIZE - length; i++) {
						dfu_suffix[i] = dfu_suffix[dfu_suffix_length+i];
					}
				}
			}
			i_suffix = 0;
		} else {
			if (usbd_request_handled() == 0) {
				dfu_suffix_length = 0;
			}
			i_suffix = length - DFU_SUFFIX_SIZE;
		}
		for (i = 0; i < length; i++) {
			USBD_OUT_BEGIN(val) {
				if (i >= i_suffix) {
					BUG_ON(dfu_suffix_length >= DFU_SUFFIX_SIZE);
					dfu_suffix[dfu_suffix_length] = val;
					dfu_suffix_length++;
				}
				dfu_block[i] = val;
			} USBD_OUT_END
		}
		if (usbd_request_handled() == length) {
			dfu_crc_update(&dfu_crc, dfu_block, length);
			dfu_set_state(DFU_STATE_DFU_DNBUSY);
			board_hw_firmware_write(blknr, dfu_block, length);
			dfu_set_state(DFU_STATE_DFU_DNLOAD_SYNC);
			dfu_dnload_size += length;
		}
	} else {
		dfu_set_state(DFU_STATE_DFU_MANIFEST_SYNC);
		if ((dfu_suffix_validate() != DFU_SUFFIX_SIZE) ||
		    (0 != dfu_crc)) {
			dfu_set_error(DFU_STATUS_ERR_NOTDONE);
			return;
		}
		dfu_dnload_size -= DFU_SUFFIX_SIZE;
		dfu_set_state(DFU_STATE_DFU_MANIFEST);
		board_hw_firmware_manifest(dfu_dnload_size);
		dfu_auto_reset(DFU_INTERFACE_PROTOCOL_RUNTIME);
	}
}

static void dfu_upload(void)
{
	uint16_t blknr = usbd_control_request_value();
	uint16_t length = usbd_control_request_length();
	uint16_t i;
	uint8_t val;

	if (dfu_proto != DFU_INTERFACE_PROTOCOL_DFUMODE ||
	    (dfu_state != DFU_STATE_DFU_IDLE &&
	     dfu_state != DFU_STATE_DFU_UPLOAD_IDLE)) {
		dfu_set_error(DFU_STATUS_ERR_STALLEDPKT);
		return;
	}
	if (length > DFU_TRANSFER_SIZE) {
		dfu_set_error(DFU_STATUS_ERR_ADDRESS);
		return;
	}

	if (blknr == div16u(dfu_upload_size, DFU_TRANSFER_SIZE))
		length = mod16u(dfu_upload_size, DFU_TRANSFER_SIZE);

	if (usbd_request_handled() == 0) {
		board_hw_firmware_read(blknr, dfu_block);
	}
	for (i = 0; i < length; i++) {
		val = i ? i : blknr;
		USBD_INB(val);
	}
	if (usbd_request_handled() == length) {
		if (length == DFU_TRANSFER_SIZE) {
			dfu_set_state(DFU_STATE_DFU_UPLOAD_IDLE);
		} else {
			dfu_set_state(DFU_STATE_DFU_IDLE);
		}
	}
}

static boolean dfu_state_abortable(void)
{
	switch (dfu_state) {
	case DFU_STATE_DFU_DNBUSY:
	case DFU_STATE_DFU_MANIFEST:
	case DFU_STATE_DFU_MANIFEST_WAIT_RESET:
	case DFU_STATE_DFU_ERROR:
		return false;
	}
	return true;
}

static void dfu_abort(void)
{
	if (dfu_proto != DFU_INTERFACE_PROTOCOL_DFUMODE ||
	    !dfu_state_abortable()) {
		dfu_set_error(DFU_STATUS_ERR_STALLEDPKT);
		return;
	}

	dfu_set_state(DFU_STATE_DFU_IDLE);
}
#else
#define dfu_dnload()		dfu_set_error(DFU_STATUS_ERR_STALLEDPKT)
#define dfu_upload()		dfu_set_error(DFU_STATUS_ERR_STALLEDPKT)
#define dfu_abort()		dfu_set_error(DFU_STATUS_ERR_STALLEDPKT)
#endif

static void dfu_get_status(void)
{
	USBD_INB(dfu_error);
	USBD_INB(LOBYTE(LOWORD(DFU_POLL_TIMEOUT)));
	USBD_INW(HIWORD(DFU_POLL_TIMEOUT));
	USBD_INB(dfu_state);
	USBD_INB(DFU_STRING_STATUS_BEGIN+dfu_error);

	switch (dfu_state) {
	case DFU_STATE_DFU_MANIFEST_SYNC:
	case DFU_STATE_DFU_DNLOAD_SYNC:
		dfu_set_state(DFU_STATE_DFU_DNLOAD_IDLE);
		break;
	}
}

static void dfu_clr_status(void)
{
	if (dfu_proto != DFU_INTERFACE_PROTOCOL_DFUMODE ||
	    dfu_state != DFU_STATE_DFU_ERROR) {
		dfu_set_error(DFU_STATUS_ERR_STALLEDPKT);
		return;
	}

	dfu_set_error(DFU_STATUS_OK);
	dfu_set_state(DFU_STATE_DFU_IDLE);
}

static void dfu_get_state(void)
{
	USBD_INB(dfu_state);
}

static void dfu_handle_class_request(void)
{
	uint8_t req = usbd_control_request_type();

	dfu_debug(DFU_DEBUG_CS_REQ, req);

	switch (req) {
	case DFU_REQ_DETACH:
		dfu_detach();
		break;
	case DFU_REQ_DNLOAD:
		dfu_dnload();
		break;
	case DFU_REQ_UPLOAD:
		dfu_upload();
		break;
	case DFU_REQ_GETSTATUS:
		dfu_get_status();
		break;
	case DFU_REQ_CLRSTATUS:
		dfu_clr_status();
		break;
	case DFU_REQ_GETSTATE:
		dfu_get_state();
		break;
	case DFU_REQ_ABORT:
		dfu_abort();
		break;
	default:
		dfu_set_error(DFU_STATUS_ERR_STALLEDPKT);
		break;
	}
}

static void dfu_get_func_desc(void)
{
	USBD_INB(DFU_DT_FUNCTION_SIZE);
	USBD_INB(DFU_DT_FUNCTION_DESC);
	USBD_INB(DFU_ATTR_AUTO_RESET |
		 DFU_ATTR_CAN_DNLOAD |
		 DFU_ATTR_CAN_UPLOAD);
	USBD_INW(DFU_DETACH_TIMEOUT);
	USBD_INW(DFU_TRANSFER_SIZE);
	USBD_INW(DFU_VERSION);
}

static void dfu_get_intfc_desc(void)
{
	/* IN interface descriptor */
	USBD_INB(0);
	USBD_INB(USB_INTERFACE_CLASS_DFU);
	USBD_INB(DFU_INTERFACE_SUBCLASS);
	USBD_INB(dfu_proto);
	USBD_INB(DFU_STRING_INTERFACE);
}

static void dfu_get_config_desc(void)
{
	dfu_get_intfc_desc();
	dfu_get_func_desc();
}

static void dfu_get_string_desc(void)
{
	uint8_t id = LOBYTE(usbd_control_request_value());

	if (id >= DFU_STRING_STATUS_BEGIN && id <= DFU_STRING_STATUS_END) {
		USBD_INB(0x00);
	}

	switch (id) {
	case DFU_STRING_INTERFACE:
		usb_input_device();
		break;
	default:
		USBD_INB(0x00);
		break;
	}
}

static void dfu_get_descriptor(void)
{
	uint8_t desc;
	
	desc = HIBYTE(usbd_control_request_value());

	switch (desc) {
	case USB_DT_CONFIG:
		dfu_get_config_desc();
		break;
	case USB_DT_STRING:
		dfu_get_string_desc();
		break;
	default:
		usbd_endpoint_halt();
		break;
	}
}

#define dfu_set_descriptor()		usbd_endpoint_halt()

static void dfu_handle_standard_request(void)
{
	uint8_t req = usbd_control_request_type();

	switch (req) {
	case USB_REQ_GET_DESCRIPTOR:
		dfu_get_descriptor();
		break;
	case USB_REQ_SET_DESCRIPTOR:
		dfu_set_descriptor();
		break;
	default:
		usbd_endpoint_halt();
		break;
	}
}

static void dfu_handle_ctrl_data(void)
{
	uint8_t type = usbd_control_setup_type();
	uint8_t recp = usbd_control_setup_recp();

	switch (recp) {
	case USB_RECP_DEVICE:
		switch (type) {
		case USB_TYPE_STANDARD:
			dfu_handle_standard_request();
			return;
		}
		break;
	case USB_RECP_INTERFACE:
		switch (type) {
		case USB_TYPE_STANDARD:
			dfu_handle_standard_request();
			return;
		case USB_TYPE_CLASS:
			dfu_handle_class_request();
			return;
		}
		break;
	}
	usbd_endpoint_halt();
}

static uint16_t dfu_config_length(void)
{
	uint16_t length;
	
	length = USB_DT_INTERFACE_SIZE;
	length += DFU_DT_FUNCTION_SIZE;
	return length;
}

usbd_interface_t usb_dfu_interface = {
	DFU_STRING_FIRST,
	DFU_STRING_LAST,
	dfu_handle_ctrl_data,
	dfu_config_length,
};

#if 0
void dfu_suffix_test(void)
{
	size_t i, length;

	for (i = 0; i < DFU_TRANSFER_SIZE; i++) {
		dfu_block[i] = i;
	}
	dfu_suffix_writew(dfu_suffix, 0x12EF);
	dfu_suffix_writew(dfu_suffix+2, 0xABCD);
	dfu_suffix_writew(dfu_suffix+4, 0x1234);
	dfu_suffix_writew(dfu_suffix+6, DFU_VERSION);
	dfu_suffix_writel(dfu_suffix+8, DFU_SUFFIX_MAGIC);
	i = 0;
	dfu_crc = 0xFFFFFFFF;
	while (i < board_hw_firmware_size()) {
		dfu_block[0] = div16u(i, DFU_TRANSFER_SIZE);
		length = min(board_hw_firmware_size()-i, DFU_TRANSFER_SIZE);
		dfu_crc_update(&dfu_crc, dfu_block, length);
		i += length;
	}
	dfu_crc_update(&dfu_crc, dfu_suffix, DFU_SUFFIX_SIZE-sizeof(dfu_crc));
	dfu_suffix_writel(dfu_suffix+12, dfu_crc);
	for (i = 0; i < DFU_SUFFIX_SIZE; i++)
		dbg_dump(dfu_suffix[i]);
	BUG_ON(dfu_suffix[12] != 0x9D);
	BUG_ON(dfu_suffix[13] != 0x23);
	BUG_ON(dfu_suffix[14] != 0x69);
	BUG_ON(dfu_suffix[14] != 0x04);
	while (1);
}
#else
#define dfu_suffix_test()
#endif

void dfu_init(void)
{
	usbd_declare_interface(50, &usb_dfu_interface);
#if defined(CONFIG_DFU_DFUMODE)
	dfu_proto = DFU_INTERFACE_PROTOCOL_DFUMODE;
	dfu_set_state(DFU_STATE_DFU_IDLE);
#else
	dfu_proto = DFU_INTERFACE_PROTOCOL_RUNTIME;
	dfu_set_state(DFU_STATE_APP_IDLE);
#endif
	dfu_upload_size = board_hw_firmware_size();
	dfu_suffix_test();
}
