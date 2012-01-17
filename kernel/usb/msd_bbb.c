/*
 * ZETALOG's Personal COPYRIGHT
 *
 * Copyright (c) 2010
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
 * @(#)msd_bbb.c: MSD bulk only protocol functions
 * $Id: msd_bbb.c,v 1.132 2011-04-19 07:15:31 zhenglv Exp $
 */

#include <target/usb_msd.h>

#define MSD_ADDR_IN			msd_addr[MSD_BBB_ENDP_IN]
#define MSD_ADDR_OUT			msd_addr[MSD_BBB_ENDP_OUT]

struct msd_cbw msd_cbw;
struct msd_csw msd_csw;
uint8_t msd_state;

usb_endp_desc_t msd_endpoints[NR_MSD_ENDPS] = {
	/* .MSD_BBB_ENDP_IN */
	{
		USB_DT_ENDPOINT_SIZE,
		USB_DT_ENDPOINT,
		USB_DIR2ADDR(USB_DIR_IN) | 0,	/* !bEndpointAddress */
		USB_ENDP_BULK,			/* bmAttributes */
		0,				/* !wMaxPacketSize */
		MSD_ENDP_INTERVAL,
	},
	/* .MSD_BBB_ENDP_OUT */
	{
		USB_DT_ENDPOINT_SIZE,
		USB_DT_ENDPOINT,
		USB_DIR2ADDR(USB_DIR_OUT) | 0,	/* !bEndpointAddress */
		USB_ENDP_BULK,			/* bmAttributes */
		0,				/* !wMaxPacketSize */
		MSD_ENDP_INTERVAL,
	},
};

static void msd_proto_submit_out(void);
static void msd_proto_submit_in(void);

uint8_t msd_proto_get_dir(void)
{
	return MSD_BBB_CBW2DIR(msd_cbw.bmCBWFlags);
}

static boolean msd_proto_cb_valid(void)
{
	return msd_cbw.dCBWSignature == MSD_BBB_CBW_SIGNATURE &&
	       usbd_request_handled() == MSD_BBB_CBW_SIZE;
}

uint8_t msd_proto_get_lun(void)
{
	return msd_cbw.bCBWLUN;
}

uint32_t msd_proto_get_expect(void)
{
	return msd_cbw.dCBWDataTransferLength;
}

static void msd_proto_status_reset(void)
{
	msd_csw.dCSWDataResidue = 0;
	msd_csw.bCSWStatus = MSD_BBB_STATUS_COMMAND_PASSED;
}

static void msd_proto_set_state(uint8_t state)
{
	msd_debug(MSD_DEBUG_STATE, state);

	msd_state = state;
	switch (state) {
	case MSD_BBB_STATE_CBW:
		msd_proto_status_reset();
		msd_proto_submit_out();
		break;
	case MSD_BBB_STATE_CSW:
		msd_class_cb_complete();
		msd_proto_submit_in();
		break;
	case MSD_BBB_STATE_DATA:
		if (msd_proto_get_dir() == USB_DIR_IN)
			msd_proto_submit_in();
		else
			msd_proto_submit_out();
		break;
	}
}

static void msd_proto_halt(uint8_t status)
{
	msd_debug_halt(true);
	msd_csw.bCSWStatus = status;
	msd_proto_set_state(MSD_BBB_STATE_DATA);
	usbd_endpoint_halt_addr(MSD_ADDR_IN);
}

static void msd_proto_pass(void)
{
	msd_debug_halt(false);
	msd_csw.bCSWStatus = MSD_BBB_STATUS_COMMAND_PASSED;
	msd_csw.dCSWDataResidue = msd_proto_get_expect()-msd_class_get_expect();
	msd_proto_set_state((uint8_t)(msd_proto_get_expect() ?
				      MSD_BBB_STATE_DATA : MSD_BBB_STATE_CSW));
}

#ifdef CONFIG_MSD_BBB_LESS_HALTING
static void msd_proto_less_data(void)
{
	if (msd_class_get_expect() < msd_proto_get_expect()) {
		msd_proto_halt(MSD_BBB_STATUS_COMMAND_PASSED);
	}
}
#endif
#ifdef CONFIG_MSD_BBB_LESS_PADDING
static void msd_proto_less_data(void)
{
	if (msd_class_get_expect() < msd_proto_get_expect()) {
		urb_size_t i;
		for (i = msd_class_get_expect();
		     i < msd_proto_get_expect(); i++) {
			if (msd_proto_get_dir() == USB_DIR_IN)
				usbd_inb_sync(0x00);
			else
				(void)usbd_outb_sync();
		}
	}
}
#endif

static void msd_proto_submit_out(void)
{
	if (msd_state == MSD_BBB_STATE_CBW) {
		usbd_request_submit(MSD_ADDR_OUT, MSD_BBB_CBW_SIZE);
	} else if (msd_state == MSD_BBB_STATE_DATA &&
		   msd_proto_get_dir() == USB_DIR_OUT) {
		usbd_request_submit(MSD_ADDR_OUT, msd_proto_get_expect());
	}
}

static void msd_proto_submit_in(void)
{
	if (msd_state == MSD_BBB_STATE_CSW) {
		usbd_request_submit(MSD_ADDR_IN, MSD_BBB_CSW_SIZE);
	} else if (msd_state == MSD_BBB_STATE_DATA &&
		   msd_proto_get_dir() == USB_DIR_IN) {
		usbd_request_submit(MSD_ADDR_IN, msd_proto_get_expect());
	}
}

static void msd_proto_handle_out(void)
{
	uint8_t i;

	if (msd_state == MSD_BBB_STATE_CBW) {
		usbd_dump_on(MSD_DUMP_USBD);
		USBD_OUTL(msd_cbw.dCBWSignature);
		USBD_OUTL(msd_csw.dCSWTag);	/* copy tag */
		USBD_OUTL(msd_cbw.dCBWDataTransferLength);
		USBD_OUTB(msd_cbw.bmCBWFlags);
		USBD_OUTB(msd_cbw.bCBWLUN);
		USBD_OUTB(msd_cbw.bCBWCBLength);
		for (i = 0; i < MSD_BBB_CB_SIZE; i++) {
			USBD_OUTB(msd_cbw.CBWCB[i]);
		}
		usbd_dump_off();
	} else if (msd_state == MSD_BBB_STATE_DATA) {
		usbd_request_set_sync();
		msd_class_io();
		msd_proto_less_data();
		usbd_request_clear_sync();
	}
}

static void msd_proto_handle_in(void)
{
	if (msd_state == MSD_BBB_STATE_CSW) {
		usbd_dump_on(MSD_DUMP_USBD);
		USBD_INL(MSD_BBB_CSW_SIGNATURE);
		USBD_INL(msd_csw.dCSWTag);
		USBD_INL(msd_csw.dCSWDataResidue);
		USBD_INB(msd_csw.bCSWStatus);
		usbd_dump_off();
	} else if (msd_state == MSD_BBB_STATE_DATA) {
		usbd_request_set_sync();
		msd_class_io();
		msd_proto_less_data();
		usbd_request_clear_sync();
	}
}

static void msd_proto_complete_cb(boolean result)
{
	uint8_t status = result ?
			 MSD_BBB_STATUS_COMMAND_PASSED :
			 MSD_BBB_STATUS_COMMAND_FAILED;
#if 0
	if (msd_class_get_expect() > msd_proto_get_expect()) {
		msd_proto_halt(MSD_BBB_STATUS_PHASE_ERROR);
	} else
#endif
	if (status == MSD_BBB_STATUS_COMMAND_FAILED) {
		msd_proto_halt(status);
	} else {
		msd_proto_pass();
	}
}

static void msd_proto_complete_out(void)
{
	if (msd_state == MSD_BBB_STATE_CBW) {
		if (!msd_proto_cb_valid() ||
		    !msd_class_cb_valid(msd_cbw.CBWCB, msd_cbw.bCBWCBLength)) {
			msd_proto_halt(MSD_BBB_STATUS_COMMAND_FAILED);
			return;
		}
		if (msd_class_cb_schedule(msd_proto_complete_cb)) {
			msd_proto_complete_cb(msd_class_get_status());
		}
	} else if (msd_state == MSD_BBB_STATE_DATA) {
		msd_proto_set_state(MSD_BBB_STATE_CSW);
	}
}

static void msd_proto_complete_in(void)
{
	if (msd_state == MSD_BBB_STATE_CSW) {
		msd_proto_set_state(MSD_BBB_STATE_CBW);
	} else if (msd_state == MSD_BBB_STATE_DATA) {
		msd_proto_set_state(MSD_BBB_STATE_CSW);
	}
}

void msd_proto_poll(void)
{
	if (usbd_saved_addr() == MSD_ADDR_OUT) {
		msd_proto_submit_out();
	}
	if (usbd_saved_addr() == MSD_ADDR_IN) {
		msd_proto_submit_in();
	}
}

void msd_proto_iocb(void)
{
	if (usbd_saved_addr() == MSD_ADDR_OUT) {
		msd_proto_handle_out();
	}
	if (usbd_saved_addr() == MSD_ADDR_IN) {
		msd_proto_handle_in();
	}
}

void msd_proto_done(void)
{
	if (usbd_saved_addr() == MSD_ADDR_OUT) {
		msd_proto_complete_out();
	}
	if (usbd_saved_addr() == MSD_ADDR_IN) {
		msd_proto_complete_in();
	}
}

void msd_proto_ctrl(void)
{
	uint8_t req = usbd_control_request_type();

	msd_debug(MSD_DEBUG_CS_REQ, req);

	switch (req) {
	case MSD_REQ_BBB_MASS_STORAGE_RESET:
		msd_proto_reset();
		msd_class_reset();
		break;
	case MSD_REQ_BBB_GET_MAX_LUN:
		USBD_INB((uint8_t)(msd_class_max_luns()-1));
		break;
	default:
		usbd_endpoint_halt();
		break;
	}
}

void msd_proto_reset(void)
{
	usbd_endpoint_reset_addr(MSD_ADDR_IN);
	usbd_endpoint_reset_addr(MSD_ADDR_OUT);
	msd_proto_set_state(MSD_BBB_STATE_CBW);
}

void msd_proto_start(void)
{
	msd_proto_set_state(MSD_BBB_STATE_CBW);
}

void msd_proto_init(void)
{
}
