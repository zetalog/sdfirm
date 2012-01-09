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
 * @(#)usb_dfu.c: device firmware upgrade host functions
 * $Id: usb_dfu.c,v 1.315 2011-10-17 01:40:34 zhenglv Exp $
 */
#include "usb_dfu.h"
#include <host/crc32.h>

#define DFU_CRC_SIZE		4
#define DFU_ATTRIB_SIZE		(DFU_SUFFIX_SIZE-DFU_CRC_SIZE)
#define DFU_MIN_POLLTIME	50

static void dfu_timer_callback(os_timer *timer, os_device dev, void *data);
static status_t dfu_control_aval(os_usbif *usb, struct os_usb_urb *urb);
static void dfu_control_cmpl(os_usbif *usb, struct os_usb_urb *urb);

const char *dfu_request_string(uint8_t request)
{
#define VALUE_STRING(rq)		\
	case rq:			\
		return #rq;
	switch (request) {
	VALUE_STRING(DFU_DETACH)
	VALUE_STRING(DFU_DNLOAD)
	VALUE_STRING(DFU_UPLOAD)
	VALUE_STRING(DFU_GETSTATUS)
	VALUE_STRING(DFU_CLRSTATUS)
	VALUE_STRING(DFU_GETSTATE)
	VALUE_STRING(DFU_ABORT)
	default:
		return "DFU_UNKNOWN";
	}
}

void dfu_timer_submit(struct dfu_device *dfu)
{
	if (dfu->poll_timer) {
		os_timer_schedule(dfu->poll_timer,
				  min(dfu->poll_time, DFU_MIN_POLLTIME)*1000);
	}
}

void dfu_timer_discard(struct dfu_device *dfu)
{
	if (dfu->poll_timer)
		os_timer_discard(dfu->poll_timer);
}

void dfu_timer_stop(struct dfu_device *dfu)
{
	if (dfu->poll_timer) {
		os_timer_unregister(dfu->poll_timer);
		dfu->poll_timer = NULL;
	}
}

status_t dfu_timer_start(struct dfu_device *dfu)
{
	dfu->poll_timer = os_timer_register(dfu->dev,
					    dfu_timer_callback,
					    NULL);
	if (!dfu->poll_timer) {
		return STATUS_INSUFFICIENT_RESOURCES;
	}
	return STATUS_SUCCESS;
}

void dfu_get_status_sync(struct dfu_device *dfu)
{
	BUG_ON(dfu->sync_clrstatus);
	dfu->sync_getstatus = 1;
	dfu->synced = 0;
	if (dfu->seq == DFU_SEQ_DNLOAD)
		dfu_file_idle(dfu);
	dfu_poll_interface(dfu->dev, DFU_GETSTATUS);
}

void dfu_get_status_unsync(struct dfu_device *dfu)
{
	BUG_ON(dfu->sync_clrstatus);
	dfu->sync_getstatus = 0;
	dfu->synced = 1;
}

void dfu_clear_status_sync(struct dfu_device *dfu)
{
	dfu_get_status_unsync(dfu);
	dfu->sync_clrstatus = 1;
	if (dfu->seq == DFU_SEQ_DNLOAD)
		dfu_file_idle(dfu);
	dfu_poll_interface(dfu->dev, DFU_CLRSTATUS);
}

void dfu_clear_status_unsync(struct dfu_device *dfu)
{
	dfu->sync_clrstatus = 0;
}

status_t dfu_submit_control_async(os_device dev, struct os_usb_urb *urb,
				  uint8_t dir, uint8_t request, uint16_t value,
				  uint8_t *info, uint16_t length)
{
	os_usbif *usb = dev_priv(dev);
	struct dfu_device *dfu = usb_dev_priv(usb);
	status_t status;

	if (dfu->stopping)
		return STATUS_INVALID_DEVICE_STATE;

	os_dbg(OS_DBG_DEBUG, "dfu_submit_control_async(%s) - begins\n",
	       dfu_request_string(request));
	status = os_usb_submit_default_aysnc(dev, urb,
					     USB_MAKECTRL(dir,
							  USB_TYPE_CLASS,
							  USB_RECP_INTERFACE),
					     request,
					     value,
					     os_usb_interface_num(dev),
					     length, info);
	if (OS_IS_SUCCESS(status)) {
		os_dbg(OS_DBG_DEBUG, "dfu_submit_control_async(%s) - pending\n",
		       dfu_request_string(request));
	}
	return status;
}

struct dfu_function_desc *dfu_match_function(struct usb_intf_desc *intf_desc,
					     uint16_t total_length)
{
	struct usb_desc *desc;
	uint8_t *p;
	uint16_t size;

	if (!intf_desc)
		return NULL;

	size = total_length;
	p = (uint8_t *)intf_desc;
	desc = (struct usb_desc *)p;

	while (size && desc->bLength <= size) {
		if (desc->bDescriptorType == USB_DT_DFU_FUNCTION) {
			return (struct dfu_function_desc *)desc;
		}
		size -= desc->bLength;
		p += desc->bLength;
		desc = (struct usb_desc *)p;
	}
	return NULL;
}

status_t dfu_init_device(os_usbif *usb)
{
	struct dfu_device *dfu = usb_dev_priv(usb);
	dfu->poll_timer = NULL;
	dfu->poll_time = 0;
	dfu->buffer = NULL;
	dfu->dev = os_usb_device(usb);
	return STATUS_SUCCESS;
}

void dfu_exit_device(os_usbif *usb)
{
	struct dfu_device *dfu = usb_dev_priv(usb);
	BUG_ON(dfu->buffer);
	BUG_ON(dfu->poll_timer);
}

void dfu_init_seq(struct dfu_device *dfu)
{
	xchg(&dfu->seq, DFU_SEQ_IDLE);
	dfu->waiter_running = 0;
	/* os_waiter_exit(&dfu->waiter); */
}

status_t dfu_begin_seq(struct dfu_device *dfu, uint8_t proto, reflck_t seq)
{
	status_t status = STATUS_SUCCESS;
	if (dfu->proto != proto) {
		status = STATUS_INVALID_DEVICE_STATE;
		goto end;
	}
	if (DFU_SEQ_IDLE != cmpxchg(&dfu->seq,
				    DFU_SEQ_IDLE, seq)) {
		status = STATUS_INVALID_DEVICE_STATE;
		goto end;
	}
	dfu->seq_aborted = 0;
	dfu->seq_failure = 0;
	dfu->seq_error = DFU_STATUS_OK;
	dfu->seq_aborting = 0;
	dfu->seq_status = STATUS_PENDING;
	dfu->seq_ended = 0;
	xchg(&dfu->seq_last, seq);

	if (seq == DFU_DNLOAD || seq == DFU_UPLOAD) {
		os_waiter_init(&dfu->waiter, FALSE);
		dfu->waiter_running = 1;
	}
end:
	return status;
}

void dfu_end_seq(struct dfu_device *dfu, status_t status)
{
	if (status == STATUS_SUCCESS) {
		if (dfu->seq_failure) {
			dfu->seq_status = STATUS_UNSUCCESSFUL;
		} else if (dfu->seq_aborted) {
			dfu->seq_status = STATUS_CANCELLED;
		} else {
			dfu->seq_status = STATUS_SUCCESS;
		}
	} else {
		dfu->seq_status = status;
	}
	dfu->seq_ended = 1;
	dfu_init_seq(dfu);
}

void dfu_file_idle(struct dfu_device *dfu)
{
	os_dbg(OS_DBG_DEBUG, "DFU IDLE: seq=%d\r\n", dfu->seq);
	os_waiter_idle(&dfu->waiter);
}

void dfu_file_wake(struct dfu_device *dfu)
{
	os_dbg(OS_DBG_DEBUG, "DFU WAKE: seq=%d\r\n", dfu->seq);
	if (dfu->waiter_running) {
		/* User space waiter is initialized only when the
		 * DNLOAD/UPLOAD sequence is running.
		 */
		os_waiter_wake(&dfu->waiter);
	}
}

status_t __dfu_poll_interface(os_device dev, uint8_t request)
{
	os_usbif *usb = dev_priv(dev);
	struct dfu_device *dfu = usb_dev_priv(usb);
	int action;
	struct os_usb_urb *urb = NULL;
	struct dfu_urb *dfu_urb = NULL;

	switch (request) {
	case DFU_RESET:
		action = OS_USB_RESET_PORT;
		break;
	default:
		action = OS_USB_SUBMIT_URB;
		break;
	}
	urb = os_usb_alloc_async(dev, USB_EID_DEFAULT,
				 action, sizeof(struct dfu_urb));
	if (!urb) return STATUS_INSUFFICIENT_RESOURCES;
	BUG_ON(!urb);
	dfu_urb = urb_priv(urb);
	dfu_urb->request = request;
	dfu_file_idle(dfu);
	return os_usb_submit_async(dev, USB_EID_DEFAULT, urb);
}

status_t dfu_poll_interface(os_device dev, uint8_t request)
{
	os_usbif *usb = dev_priv(dev);
	struct dfu_device *dfu = usb_dev_priv(usb);
	status_t status = STATUS_SUCCESS;

	if (request == DFU_GETSTATUS) {
		dfu_timer_submit(dfu);
	} else {
		status = __dfu_poll_interface(dev, request);
	}
	return status;
}

static status_t dfu_reset_aval(os_device dev, struct os_usb_urb *urb)
{
	os_usbif *usb = dev_priv(dev);
	struct dfu_device *dfu = usb_dev_priv(usb);
	os_usb_reset_device_async(dev);
	return STATUS_SUCCESS;
}

static status_t dfu_detach_aval(os_device dev, struct os_usb_urb *urb)
{
	os_usbif *usb = dev_priv(dev);
	struct dfu_device *dfu = usb_dev_priv(usb);
	uint16_t len = 0; /* wLength: 0 */
	status_t status;
	status = dfu_submit_control_async(dev, urb, USB_DIR_OUT, DFU_DETACH,
					  dfu->dfu_function_desc->wDetachTimeOut,
					  0, len);
	return status;
}

static status_t dfu_abort_aval(os_device dev, struct os_usb_urb *urb)
{
	uint16_t len = 0; /* wLength: Zero */
	status_t status;
	status = dfu_submit_control_async(dev, urb, USB_DIR_OUT, DFU_ABORT,
					  0, /* wValue */NULL, len);
	return status;
}

static status_t dfu_dnload_aval(os_device dev, struct os_usb_urb *urb)
{
	os_usbif *usb = dev_priv(dev);
	struct dfu_device *dfu = usb_dev_priv(usb);
	status_t status = STATUS_SUCCESS;
	uint16_t len = 0;
	size_t count;
	int eof_firm, eof_user;
	uint16_t blknr = (uint16_t)(dfu->pos/dfu->xfr_size);
	uint8_t *buffer = circbf_rpos(&dfu->circbf);
	
	/* XXX: Circular Buffer POS/EOF Getting Rule
	 * Getting user vars before getting firm vars in firm context.
	 * For user vars, getting eof should be done before count as
	 * setting eof is done after setting count.
	 */
	eof_user = dfu->eof_user;
	/* Append DFU SUFFIX if eof_user. */
	if (eof_user && dfu->suffix_pos < DFU_SUFFIX_SIZE) {
		size_t bytes;
		bytes = dfu_buffer_space(dfu);
		bytes = min(bytes, (size_t)(DFU_SUFFIX_SIZE-dfu->suffix_pos));
		os_mem_copy(circbf_wpos(&dfu->circbf),
			    dfu->suffix_buf+dfu->suffix_pos,
			    bytes);
		dfu->suffix_pos += (uint8_t)bytes;
		dfu_buffer_write(dfu, bytes);
	}
	count = dfu_buffer_count(dfu);
	eof_firm = dfu->eof_firm;

	/* sanity checks */
	if (dfu->sync_clrstatus) {
		os_dbg(OS_DBG_DEBUG, "PIPE HALTED = yes\r\n");
		status = STATUS_DEVICE_BUSY;
		goto end;
	}
	if (dfu->sync_getstatus) {
		os_dbg(OS_DBG_DEBUG, "DNLOAD SYNC\r\n");
		status = STATUS_DEVICE_BUSY;
		goto end;
	}
	if (eof_firm ||
	    (count < dfu->xfr_size && !eof_user)) {
		os_dbg(OS_DBG_DEBUG, "DNLOAD BUSY\r\n");
		status = STATUS_DEVICE_BUSY;
		goto end;
	}

	len = min(count, dfu->xfr_size);
	os_dbg(OS_DBG_DEBUG, "DNLOAD - %08x, %08x\r\n",
	       len, count);

	status = dfu_submit_control_async(dev, urb, USB_DIR_OUT, DFU_DNLOAD,
					  blknr, buffer, len);
end:
	return status;
}

static status_t dfu_upload_aval(os_device dev, struct os_usb_urb *urb)
{
	os_usbif *usb = dev_priv(dev);
	struct dfu_device *dfu = usb_dev_priv(usb);
	status_t status = STATUS_SUCCESS;
	uint16_t len = 0;
	size_t space;
	int eof_firm;
	uint16_t blknr = (uint16_t)(dfu->pos/dfu->xfr_size);
	uint8_t *buffer = circbf_wpos(&dfu->circbf);

	/* XXX: Circular Buffer POS/EOF Getting Rule
	 * Getting user vars before getting firm vars in firm context.
	 * For user vars, getting eof should be done before space as
	 * setting eof is done after setting space.
	 */
	space = dfu_buffer_space(dfu);
	eof_firm = dfu->eof_firm;

	/* sanity checks */
	if (dfu->sync_clrstatus) {
		os_dbg(OS_DBG_DEBUG, "PIPE HALTED = yes\r\n");
		status = STATUS_DEVICE_BUSY;
		goto end;
	}
	if (eof_firm || space < dfu->xfr_size) {
		os_dbg(OS_DBG_DEBUG, "UPLOAD BUSY\r\n");
		status = STATUS_DEVICE_BUSY;
		goto end;
	}

	len = (uint16_t)(dfu->xfr_size);
	os_dbg(OS_DBG_DEBUG, "UPLOAD - %08x, %08x\r\n",
	       len, space);

	status = dfu_submit_control_async(dev, urb, USB_DIR_IN, DFU_UPLOAD,
					  blknr, buffer, len);
end:
	return status;
}

static status_t dfu_get_status_aval(os_device dev, struct os_usb_urb *urb)
{
	os_usbif *usb = dev_priv(dev);
	struct dfu_device *dfu = usb_dev_priv(usb);
	status_t status = STATUS_SUCCESS;
	uint16_t len = 0;

	/* sanity checks */
	if (dfu->seq_ended) {
		os_dbg(OS_DBG_DEBUG, "SEQ ENDED\r\n");
		status = STATUS_DEVICE_BUSY;
		goto end;
	}
	if (dfu->sync_clrstatus) {
		os_dbg(OS_DBG_DEBUG, "ERROR OCCURRED\r\n");
		status = STATUS_DEVICE_BUSY;
		goto end;
	}
	
	len = sizeof (struct dfu_status); /* wLength: 6 */
	status = dfu_submit_control_async(dev, urb, USB_DIR_IN, DFU_GETSTATUS,
					  0, /* wValue */
					  (uint8_t *)&dfu->params.status, len);
end:
	return status;
}

static status_t dfu_clr_status_aval(os_device dev, struct os_usb_urb *urb)
{
	os_usbif *usb = dev_priv(dev);
	struct dfu_device *dfu = usb_dev_priv(usb);
	status_t status = STATUS_SUCCESS;
	uint16_t len = 0;

	/* sanity checks */
	if (!dfu->sync_clrstatus) {
		os_dbg(OS_DBG_DEBUG, "PIPE HALTED = no\r\n");
		status = STATUS_DEVICE_BUSY;
		goto end;
	}
	status = dfu_submit_control_async(dev, urb, USB_DIR_OUT, DFU_CLRSTATUS,
					  0, /* wValue */NULL, len);
end:
	return status;
}

static status_t dfu_get_state_aval(os_device dev, struct os_usb_urb *urb)
{
	os_usbif *usb = dev_priv(dev);
	struct dfu_device *dfu = usb_dev_priv(usb);
	status_t status;
	uint16_t len = 1; /* wLength: 1 */
	status = dfu_submit_control_async(dev, urb, USB_DIR_IN, DFU_GETSTATE,
					  0, /* wValue */
					  &dfu->params.state, len);
	return status;
}

static status_t dfu_control_aval(os_usbif *usb, struct os_usb_urb *urb)
{
	struct dfu_device *dfu = usb_dev_priv(usb);
	struct dfu_urb *dfu_urb = urb_priv(urb);
	status_t status = STATUS_SUCCESS;

	switch (dfu_urb->request) {
	default:
	case DFU_GETSTATUS:
		status = dfu_get_status_aval(dfu->dev, urb);
		break;
	case DFU_CLRSTATUS:
		status = dfu_clr_status_aval(dfu->dev, urb);
		break;
	case DFU_DNLOAD:
		status = dfu_dnload_aval(dfu->dev, urb);
		break;
	case DFU_UPLOAD:
		status = dfu_upload_aval(dfu->dev, urb);
		break;
	case DFU_DETACH:
		status = dfu_detach_aval(dfu->dev, urb);
		break;
	case DFU_ABORT:
		status = dfu_abort_aval(dfu->dev, urb);
		break;
	case DFU_RESET:
		status = dfu_reset_aval(dfu->dev, urb);
		break;
	case DFU_GETSTATE:
		status = dfu_get_state_aval(dfu->dev, urb);
		break;
	}
	return status;
}

static int dfu_state_abortable(uint8_t state)
{
	switch (state) {
	case DFU_STATE_dfuDNLOAD_SYNC:
	case DFU_STATE_dfuDNLOAD_IDLE:
	case DFU_STATE_dfuUPLOAD_IDLE:
	case DFU_STATE_dfuMANIFEST_SYNC:
		return 1;
	}
	return 0;
}

static void dfu_get_status_cmpl(os_device dev, struct os_usb_urb *urb)
{
	os_usbif *usb = dev_priv(dev);
	struct dfu_device *dfu = usb_dev_priv(usb);
	usb_status_t urbstatus = urb->status;

	if (!USBD_IS_SUCCESS(urbstatus)) {
		os_dbg(OS_DBG_ERR, "DFU ERROR: Unrecoverable error!\n");
		dfu->state = DFU_STATE_dfuERROR;
		dfu->status = DFU_STATUS_errUNKNOWN;
		dfu_file_wake(dfu);
		dfu_end_seq(dfu, STATUS_UNSUCCESSFUL);
		return;
	}

	dfu_get_status_unsync(dfu);
	dfu->state = dfu->params.status.bState;
	dfu->status = dfu->params.status.bStatus;
	dfu->iString = dfu->params.status.iString;
	dfu->poll_time = MAKELONG((uint16_t)dfu->params.status.bwPollTime[0],
				  MAKEWORD(dfu->params.status.bwPollTime[1],
					   dfu->params.status.bwPollTime[2]));
	os_dbg(OS_DBG_DEBUG, "bState = %02X\n", dfu->state);
	os_dbg(OS_DBG_DEBUG, "bStatus = %02X\n", dfu->status);
	os_dbg(OS_DBG_DEBUG, "ulPollTime = %d\n", dfu->poll_time);

	if (dfu->state == DFU_STATE_dfuERROR) {
		os_dbg(OS_DBG_DEBUG, "DNLOAD state error\n");
		if (!dfu->seq_failure) {
			dfu->seq_failure = 1;
			dfu->seq_error = dfu->status;
			dfu_clear_status_sync(dfu);
		} else {
			/* DFU_CLRSTATUS has already been tried, so end
			 * the sequence without hesitate.
			 */
			dfu_end_seq(dfu, STATUS_UNSUCCESSFUL);
		}
		return;
	}
	if (dfu->seq_aborting && dfu_state_abortable(dfu->state)) {
		dfu->seq_aborted = 1;
		dfu_file_idle(dfu);
		dfu_poll_interface(dfu->dev, DFU_ABORT);
		return;
	}

	switch (dfu->seq) {
	case DFU_SEQ_DNLOAD:
		switch (dfu->state) {
		case DFU_STATE_dfuDNBUSY:
		case DFU_STATE_dfuDNLOAD_SYNC:
		case DFU_STATE_dfuMANIFEST:
		case DFU_STATE_dfuMANIFEST_SYNC:
			dfu_poll_interface(dfu->dev, DFU_GETSTATUS);
			break;
		case DFU_STATE_dfuDNLOAD_IDLE:
			dfu_buffer_run(dfu, 0);
			break;
		case DFU_STATE_dfuMANIFEST_WAIT_RESET:
			dfu_poll_interface(dfu->dev, DFU_RESET);
			break;
		case DFU_STATE_dfuIDLE:
			dfu_end_seq(dfu, STATUS_SUCCESS);
			break;
		default:
			dfu_end_seq(dfu, STATUS_UNSUCCESSFUL);
			break;
		}
		break;
	case DFU_SEQ_UPLOAD:
		switch (dfu->state) {
		case DFU_STATE_dfuUPLOAD_IDLE:
			dfu_buffer_run(dfu, 0);
			break;
		case DFU_STATE_dfuIDLE:
			dfu_end_seq(dfu, STATUS_SUCCESS);
			break;
		default:
			dfu_end_seq(dfu, STATUS_UNSUCCESSFUL);
			break;
		}
		break;
	case DFU_SEQ_DETACH:
		switch (dfu->state) {
		case DFU_STATE_appDETACH:
			dfu_poll_interface(dfu->dev, DFU_RESET);
			break;
		case DFU_STATE_dfuIDLE:
			dfu_end_seq(dfu, STATUS_SUCCESS);
			break;
		default:
			dfu_end_seq(dfu, STATUS_UNSUCCESSFUL);
			break;
		}
		break;
	case DFU_SEQ_ABORT:
		switch (dfu->state) {
		case DFU_STATE_dfuIDLE:
			dfu_end_seq(dfu, STATUS_SUCCESS);
			break;
		default:
			dfu_end_seq(dfu, STATUS_UNSUCCESSFUL);
			break;
		}
		break;
	default:
		dfu_end_seq(dfu, STATUS_UNSUCCESSFUL);
		break;
	}
}

static void dfu_get_state_cmpl(os_device dev, struct os_usb_urb *urb)
{
	os_usbif *usb = dev_priv(dev);
	struct dfu_device *dfu = usb_dev_priv(usb);
	usb_status_t urbstatus = urb->status;

	dfu->state = dfu->params.state;
	os_dbg(OS_DBG_DEBUG, "bState = %02X\n", dfu->state);
}

static void dfu_clr_status_cmpl(os_device dev, struct os_usb_urb *urb)
{
	os_usbif *usb = dev_priv(dev);
	struct dfu_device *dfu = usb_dev_priv(usb);
	usb_status_t urbstatus = urb->status;

	dfu_clear_status_unsync(dfu);
	dfu_poll_interface(dfu->dev, DFU_GETSTATUS);
}

static void dfu_dnload_cmpl(os_device dev, struct os_usb_urb *urb)
{
	os_usbif *usb = dev_priv(dev);
	struct dfu_device *dfu = usb_dev_priv(usb);
	usb_status_t urbstatus = urb->status;
	size_t len = urb->length;

	if (!USBD_IS_SUCCESS(urbstatus)) {
		if (urbstatus != USBD_STATUS_ERROR_BUSY)
			dfu->eof_firm = 1;
		dfu_buffer_run(dfu, 0);
		return;
	}
	dfu->pos += len;
	os_dbg(OS_DBG_INFO, "DNLOAD length = 0x%08x, 0x%08x\n",
	       len, dfu->pos);
	/* XXX: Circular Buffer POS/EOF Setting Rule
	 * EOF is set after circular buffer positions.
	 */
	dfu_buffer_read(dfu, len);
	if (len == 0) {
		os_dbg(OS_DBG_DEBUG, "====================\r\n");
		os_dbg(OS_DBG_INFO, "DNLOAD total = 0x%08x, suffix = %08x\r\n",
		       dfu->pos, dfu->suffix_pos);
		os_dbg(OS_DBG_DEBUG, "====================\r\n");
		dfu->eof_firm = 1;
	}
	dfu_get_status_sync(dfu);
}

static void dfu_upload_cmpl(os_device dev, struct os_usb_urb *urb)
{
	os_usbif *usb = dev_priv(dev);
	struct dfu_device *dfu = usb_dev_priv(usb);
	usb_status_t urbstatus = urb->status;
	size_t len = urb->length;

	if (!USBD_IS_SUCCESS(urbstatus)) {
		if (urbstatus != USBD_STATUS_ERROR_BUSY)
			dfu->eof_firm = 1;
		dfu_buffer_run(dfu, 0);
		return;
	}
	dfu->pos += len;
	os_dbg(OS_DBG_INFO, "UPLOAD length = 0x%08x, 0x%08x\n",
	       len, dfu->pos);
	/* XXX: Circular Buffer POS/EOF Setting Rule
	 * EOF is set after circular buffer positions.
	 */
	dfu_buffer_write(dfu, len);
	if (len < dfu->xfr_size) {
		os_dbg(OS_DBG_DEBUG, "====================\r\n");
		os_dbg(OS_DBG_INFO, "UPLOAD total = 0x%08x\r\n", dfu->pos);
		os_dbg(OS_DBG_DEBUG, "====================\r\n");
		dfu->eof_firm = 1;
	}
	dfu_buffer_run(dfu, 0);
}

static void dfu_detach_cmpl(os_device dev, struct os_usb_urb *urb)
{
	os_usbif *usb = dev_priv(dev);
	struct dfu_device *dfu = usb_dev_priv(usb);
	usb_status_t urbstatus = urb->status;

	if (dfu->dfu_function_desc->bmAttributes & DFU_ATTR_WILL_DETACH) {
		dfu_poll_interface(dev, DFU_GETSTATUS);
	} else {
		dfu_poll_interface(dev, DFU_RESET);
	}
}

static void dfu_abort_cmpl(os_device dev, struct os_usb_urb *urb)
{
	os_usbif *usb = dev_priv(dev);
	struct dfu_device *dfu = usb_dev_priv(usb);
	usb_status_t urbstatus = urb->status;

	dfu_poll_interface(dev, DFU_GETSTATUS);
}

static void dfu_reset_cmpl(os_device dev, struct os_usb_urb *urb)
{
	os_usbif *usb = dev_priv(dev);
	struct dfu_device *dfu = usb_dev_priv(usb);
	usb_status_t urbstatus = urb->status;

	dfu_poll_interface(dev, DFU_GETSTATUS);
}

static void dfu_control_cmpl(os_usbif *usb, struct os_usb_urb *urb)
{
	struct dfu_device *dfu = usb_dev_priv(usb);
	struct dfu_urb *dfu_urb= urb_priv(urb);

	if (urb->urb) {
		os_dbg(OS_DBG_DEBUG, "dfu_submit_control_async(%s) - ends\n",
		       dfu_request_string(dfu_urb->request));
	}
	if (!USBD_IS_SUCCESS(urb->status)) {
		if (urb->status != USBD_STATUS_ERROR_BUSY &&
		    dfu_urb->request != DFU_GETSTATUS) {
			dfu_get_status_sync(dfu);
			return;
		}
	}
	switch (dfu_urb->request) {
	default:
	case DFU_GETSTATUS:
		dfu_get_status_cmpl(dfu->dev, urb);
		break;
	case DFU_CLRSTATUS:
		dfu_clr_status_cmpl(dfu->dev, urb);
		break;
	case DFU_DNLOAD:
		dfu_dnload_cmpl(dfu->dev, urb);
		break;
	case DFU_UPLOAD:
		dfu_upload_cmpl(dfu->dev, urb);
		break;
	case DFU_DETACH:
		dfu_detach_cmpl(dfu->dev, urb);
		break;
	case DFU_ABORT:
		dfu_abort_cmpl(dfu->dev, urb);
		break;
	case DFU_RESET:
		dfu_reset_cmpl(dfu->dev, urb);
		break;
	case DFU_GETSTATE:
		dfu_get_state_cmpl(dfu->dev, urb);
		break;
	}
}

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

void dfu_crc_update(uint32_t *crc, const uint8_t *buf, size_t len)
{
	*crc = crc32(*crc, buf, len);
}

void dfu_suffix_init(struct dfu_device *dfu, struct dfu_firm *firm)
{
	uint8_t *suffix = dfu->suffix_buf;

	dfu->filecrc = 0xFFFFFFFF;
	dfu->suffix_pos = 0;
	BUG_ON(!firm);
	dfu_suffix_writew(suffix, firm->bcdDevice);
	dfu_suffix_writew(suffix+2, firm->idProduct);
	dfu_suffix_writew(suffix+4, firm->idVendor);
	dfu_suffix_writew(suffix+6, DFU_VERSION);
	dfu_suffix_writel(suffix+8, DFU_SUFFIX_MAGIC);
}

void dfu_suffix_exit(struct dfu_device *dfu)
{
	uint8_t *suffix = dfu->suffix_buf;

	dfu_crc_update(&dfu->filecrc, suffix, DFU_ATTRIB_SIZE);
	dfu_suffix_writel(suffix+12, dfu->filecrc);
}

void dfu_buffer_calc(struct dfu_device *dfu, uint16_t xfr_size)
{
	size_t size;

	BUG_ON(DFU_BUFFERED_XFRS<=1);

	if (xfr_size == 0)
		dfu->xfr_size = 8;
	else
		dfu->xfr_size = xfr_size;
	
	dfu->xfr_size = xfr_size;
	size = __roundup32(dfu->xfr_size*DFU_BUFFERED_XFRS);
	dfu->length = size;
	dfu->pad_size = size-(dfu->xfr_size*DFU_BUFFERED_XFRS);
}

void dfu_buffer_run(struct dfu_device *dfu, int user)
{
	size_t space, count;
	int eof_user, eof_firm;

	/* XXX: Circular Buffer POS/EOF Snapshot Rule
	 * If this is called from user context, then eof_user can be
	 * ensured and count(DNLOAD)/space(UPLOAD) can be ensured big
	 * enough.  Getting eof_firm first can ensure space/count is
	 * correct.
	 * If this is called from firm context, then eof_firm can be
	 * ensured and space(DNLOAD)/count(UPLOAD) can be ensured big
	 * enough.  Getting eof_user first can ensure count/space is
	 * correct.
	 */
	eof_user = dfu->eof_user;
	eof_firm = dfu->eof_firm;
	space = dfu_buffer_space(dfu);
	count = dfu_buffer_count(dfu);

	os_dbg(OS_DBG_DEBUG, "COUNT: 0x%04x (0...size)\r\n", count);
	os_dbg(OS_DBG_DEBUG, "SPACE: 0x%04x (0...size-1)\r\n", space);
	os_dbg(OS_DBG_DEBUG, "EOF: user=%s, firm=%s\r\n",
	       eof_user ? "yes" : "no",
	       eof_firm ? "yes" : "no");

	switch (dfu->seq) {
	case DFU_DNLOAD:
		if (space > 0 || eof_firm) {
			dfu_file_wake(dfu);
		} else if (!user) {
			dfu_file_idle(dfu);
		}
		if (count >= dfu->xfr_size || eof_user) {
			if (!eof_firm)
				dfu_poll_interface(dfu->dev, DFU_DNLOAD);
		}
		break;
	case DFU_UPLOAD:
		if (count > 0 || eof_firm) {
			dfu_file_wake(dfu);
		} else if (!user) {
			dfu_file_idle(dfu);
		}
		if (space >= dfu->xfr_size) {
			if (!eof_firm)
				dfu_poll_interface(dfu->dev, DFU_UPLOAD);
		}
		break;
	}
	if (eof_firm && !user) {
		dfu_poll_interface(dfu->dev, DFU_GETSTATUS);
	}
}

void dfu_buffer_init(struct dfu_device *dfu)
{
	dfu_clear_status_unsync(dfu);
	dfu_get_status_unsync(dfu);
#if 0
	dfu->usb_status = USBD_STATUS_SUCCESS;
	dfu->dfu_status = DFU_STATUS_OK;
#endif

	BUG_ON((dfu->seq != DFU_DNLOAD) &&
	       (dfu->seq != DFU_UPLOAD));

	INIT_CIRCBF_DECLARE(&dfu->circbf);
	dfu->eof_user = 0;
	dfu->eof_firm = 0;
	dfu->pos = 0;
	dfu_buffer_run(dfu, 1);
}

void dfu_buffer_exit(struct dfu_device *dfu)
{
}

void dfu_buffer_read(struct dfu_device *dfu, size_t bytes)
{
	size_t count;
	count = circbf_count_end(&dfu->circbf, dfu->length);
	BUG_ON(bytes > count);
	circbf_read(&dfu->circbf, dfu->length, bytes);
	if (circbf_roff(&dfu->circbf) == (dfu->length-dfu->pad_size))
		circbf_read(&dfu->circbf, dfu->length, dfu->pad_size);
}

void dfu_buffer_write(struct dfu_device *dfu, size_t bytes)
{
	size_t space;
	space = circbf_space_end(&dfu->circbf, dfu->length);
	BUG_ON(bytes > space);
	circbf_write(&dfu->circbf, dfu->length, bytes);
	if (circbf_woff(&dfu->circbf) == (dfu->length-dfu->pad_size))
		circbf_write(&dfu->circbf, dfu->length, dfu->pad_size);
}

size_t dfu_buffer_space(struct dfu_device *dfu)
{
	size_t space = circbf_space_end(&dfu->circbf, dfu->length);
	BUG_ON(circbf_woff(&dfu->circbf) >= dfu->xfr_size*DFU_BUFFERED_XFRS);
	BUG_ON(circbf_roff(&dfu->circbf) >= dfu->xfr_size*DFU_BUFFERED_XFRS);
	if (circbf_roff(&dfu->circbf) <= circbf_woff(&dfu->circbf))
		space -= dfu->pad_size;
	BUG_ON(space > (dfu->xfr_size*DFU_BUFFERED_XFRS-1));
	return space;
}

size_t dfu_buffer_count(struct dfu_device *dfu)
{
	size_t count = circbf_count_end(&dfu->circbf, dfu->length);
	BUG_ON(circbf_woff(&dfu->circbf) >= dfu->xfr_size*DFU_BUFFERED_XFRS);
	BUG_ON(circbf_roff(&dfu->circbf) >= dfu->xfr_size*DFU_BUFFERED_XFRS);
	if (circbf_woff(&dfu->circbf) < circbf_roff(&dfu->circbf))
		count -= dfu->pad_size;
	BUG_ON(count > dfu->xfr_size*DFU_BUFFERED_XFRS);
	return count;
}

#if 0
#define DFU_BUFFER_WAIT_TIMEOUT		120*1000*1000	/* 120 seconds */
#else
#define DFU_BUFFER_WAIT_TIMEOUT		OS_WAIT_INFINITE
#endif

status_t dfu_buffer_poll(struct dfu_device *dfu, unsigned long *poll)
{
	size_t count, space;
	int eof_firm;
	unsigned long cond;
	status_t status = STATUS_SUCCESS;

	BUG_ON(!poll);
	cond = *poll, *poll = 0;
again:
	os_dbg(OS_DBG_DEBUG, "DFU_BUFFER - Waiting\r\n");
	if (os_waiter_wait(&dfu->waiter, DFU_BUFFER_WAIT_TIMEOUT)) {
		os_dbg(OS_DBG_INFO, "DFU_BUFFER - Awaking\r\n");
		if (dfu->seq_status != STATUS_PENDING)
			status = dfu->seq_status;
		if (cond & POLL_READ) {
			/* XXX: Circular Buffer POS/EOF Getting Rule
			 * EOF is get before circular buffer positions.
			 */
			eof_firm = dfu->eof_firm;
			count = dfu_buffer_count(dfu);
			if (count > 0 || eof_firm ||
			    dfu->seq_status != STATUS_PENDING)
				(*poll) |= POLL_READ;
		}
		if (cond & POLL_WRITE) {
			/* XXX: Circular Buffer POS/EOF Getting Rule
			 * EOF is get before circular buffer positions.
			 */
			eof_firm = dfu->eof_firm;
			space = dfu_buffer_space(dfu);
			if (space > 0 || eof_firm ||
			    dfu->seq_status != STATUS_PENDING)
				(*poll) |= POLL_WRITE;
		}
		if (*poll)
			return status;

		/* XXX: Resume Userspace I/Os
		 * This is the easiest and tricky way to resume our
		 * device.  Since dfu->stopping will only occur here at
		 * suspending process which means the kernel side has benn
		 * stopped, it then will be safe to pass the "user=0" to
		 * the dfu_buffer_run function to make the poller etner
		 * the waiting state.
		 */
		if (dfu->stopping)
			dfu_buffer_run(dfu, 0);
		goto again;
	} else {
		/* TODO: Allow Timeout IO for User Space
		 * Is this enough for breaking the timed out pipe?
		 */
		os_dbg(OS_DBG_ERR, "DFU_BUFFER - Timeout\r\n");
		dfu->seq_aborting = 1;
		return STATUS_TIMEOUT;
	}
}

status_t dfu_read_file(struct os_usb_file *file,
		       char *buf, size_t *count)
{
	status_t status = STATUS_SUCCESS;
	struct dfu_file *dfu_file = usb_file_priv(file);
	struct dfu_device *dfu = dfu_file->dev;
	size_t bytes = 0;
	unsigned long poll = POLL_READ;

	if ((dfu->proto != USBDFU_PROTO_DFUMODE) ||
	    (dfu->seq != DFU_SEQ_UPLOAD)) {
		status = STATUS_INVALID_DEVICE_STATE;
		goto end;
	}

	status = dfu_buffer_poll(dfu, &poll);
	if (!OS_IS_SUCCESS(status)) goto end;

	bytes = dfu_buffer_count(dfu);
	os_dbg(OS_DBG_DEBUG, "DFU_READ - count=%d, bytes=%d\r\n",
	       *count, bytes);
	if (bytes > 0)
		bytes = min(*count, bytes);
	if (bytes > 0)
		os_mem_copy(buf, circbf_rpos(&dfu->circbf), bytes);
	/* XXX: Circular Buffer POS/EOF Setting Rule
	 * EOF is set after circular buffer positions.
	 */
	dfu_buffer_read(dfu, bytes);
	if (bytes == 0)
		dfu->eof_user = 1;
	dfu_buffer_run(dfu, 1);
	*count = bytes;
end:
	return status;
}

status_t dfu_write_file(struct os_usb_file *file,
			const char *buf, size_t *count)
{
	status_t status = STATUS_SUCCESS;
	struct dfu_file *dfu_file = usb_file_priv(file);
	struct dfu_device *dfu = dfu_file->dev;
	size_t bytes = 0;
	unsigned long poll = POLL_WRITE;

	if ((dfu->proto != USBDFU_PROTO_DFUMODE) ||
	    (dfu->seq != DFU_SEQ_DNLOAD)) {
		status = STATUS_INVALID_DEVICE_STATE;
		*count = 0;
		goto end;
	}

	status = dfu_buffer_poll(dfu, &poll);
	if (status != STATUS_SUCCESS) goto end;

	bytes = dfu_buffer_space(dfu);
	os_dbg(OS_DBG_DEBUG, "DFU_WRITE - count=%d, bytes=%d\r\n",
	       *count, bytes);
	if (bytes > 0)
		bytes = min(*count, bytes);
	if (bytes > 0) {
		os_mem_copy(circbf_wpos(&dfu->circbf), buf, bytes);
		dfu_crc_update(&dfu->filecrc, buf, bytes);
	}
	/* XXX: Circular Buffer POS/EOF Setting Rule
	 * EOF is set after circular buffer positions.
	 */
	dfu_buffer_write(dfu, bytes);
	if (bytes == 0) {
		os_dbg(OS_DBG_DEBUG, "????? DFU_WRITE - EOF ?????\r\n");
	}
	dfu_buffer_run(dfu, 1);
	*count = bytes;
end:
	return status;
}

static void dfu_timer_callback(os_timer *timer, os_device dev, void *data)
{
	os_usbif *usb = dev_priv(dev);
	struct dfu_device *dfu = usb_dev_priv(usb);
	status_t status;

	if (dfu->stopping)
		return;

	status = __dfu_poll_interface(dev, DFU_GETSTATUS);
	if (!OS_IS_SUCCESS(status)) {
		dfu_timer_submit(dfu);
	}
}

void __dfu_stop_device(struct dfu_device *dfu)
{
	dfu->stopping = 1;
	dfu_timer_discard(dfu);
	os_usb_suspend_async(dfu->dev, USB_EID_DEFAULT);
}

void __dfu_start_device(struct dfu_device *dfu)
{
	dfu->stopping = 0;
	os_usb_resume_async(dfu->dev, USB_EID_DEFAULT);
	dfu_poll_interface(dfu->dev, DFU_GETSTATUS);
}

status_t dfu_start_device(os_usbif *usb)
{
	status_t status = STATUS_SUCCESS;
	struct dfu_function_desc *desc;
	struct usb_string_desc *string = NULL;
	struct dfu_device *dfu = usb_dev_priv(usb);
	os_device dev = dfu->dev;

	BUG_ON(!usb->usb_interface_desc);

	desc = dfu_match_function(usb->usb_interface_desc,
				  usb->usb_interface_desc_length);
	if (!desc || 
	    (desc->wTransferSize == 0 && 
	     usb->usb_interface_desc->bInterfaceProtocol != USBDFU_PROTO_RUNTIME)) {
		status = STATUS_NO_SUCH_DEVICE;
		goto end;
	}

	dfu->proto = usb->usb_interface_desc->bInterfaceProtocol;
	dfu->dfu_function_desc = desc;

	os_dbg(OS_DBG_DEBUG, "----------\n");
	os_dbg(OS_DBG_DEBUG, "DFU Function Desc\n");
	os_dbg(OS_DBG_DEBUG, "bInterfaceProtocol = %02X\n", dfu->proto);
	os_dbg(OS_DBG_DEBUG, "bmAttributes = %02X\n", desc->bmAttributes);
	os_dbg(OS_DBG_DEBUG, "wDetachTimeOut = %d\n", desc->wDetachTimeOut);
	os_dbg(OS_DBG_DEBUG, "wTransferSize = %d\n", desc->wTransferSize);
	os_dbg(OS_DBG_DEBUG, "bcdDFUVersion = %02X\n", desc->bcdDFUVersion);

	if (usb->usb_device_desc->iSerialNumber != 0) {
		status = usb_get_string_desc(dev, &string,
					     usb->usb_device_desc->iSerialNumber);
		if (!OS_IS_SUCCESS(status)) goto end;
		usb_convert_string(string, dfu->dev_sn, MAX_DFU_SN);
		os_mem_free(string);
		string = NULL;
	}

	dfu_buffer_calc(dfu, dfu->dfu_function_desc->wTransferSize);
	dfu->buffer = os_mem_alloc(dfu->xfr_size*DFU_BUFFERED_XFRS, "DFU_IO_BUFFER");
	if (!dfu->buffer) goto end;
	INIT_CIRCBF_ASSIGN(&dfu->circbf, dfu->buffer);

	if (usb->usb_interface_desc->iInterface != 0) {
		status = usb_get_string_desc(dev, &string,
					     usb->usb_interface_desc->iInterface);
		if (!OS_IS_SUCCESS(status)) goto end;
		usb_convert_string(string, dfu->dev_name, MAX_DFU_NAME);
		os_mem_free(string);
		string = NULL;
	} else if (usb->usb_device_desc->iProduct != 0) {
		status = usb_get_string_desc(dev, &string,
					     usb->usb_device_desc->iProduct);
		if (!OS_IS_SUCCESS(status)) goto end;
		usb_convert_string(string, dfu->dev_name, MAX_DFU_NAME);
		os_mem_free(string);
		string = NULL;
	} else {
		/* TODO: prepare a default device name here */
	}

	status = dfu_timer_start(dfu);
	if (!OS_IS_SUCCESS(status)) goto end;

	dfu_init_seq(dfu);
	__dfu_start_device(dfu);

end:
	if (string) os_mem_free(string);
	return status;
}

void dfu_stop_device(os_usbif *usb)
{
	struct dfu_device *dfu = usb_dev_priv(usb);

	__dfu_stop_device(dfu);
	dfu_timer_stop(dfu);
	if (dfu->buffer) {
		os_mem_free(dfu->buffer);
		dfu->buffer = NULL;
		INIT_CIRCBF_DECLARE(&dfu->circbf);
	}
}

void dfu_suspend_device(os_usbif *usb)
{
	struct dfu_device *dfu = usb_dev_priv(usb);
	os_dbg(OS_DBG_DEBUG, "DFU Suspending...\n");
	__dfu_stop_device(dfu);
}

void dfu_resume_device(os_usbif *usb)
{
	struct dfu_device *dfu = usb_dev_priv(usb);
	os_dbg(OS_DBG_DEBUG, "DFU Resuming...\n");
	__dfu_start_device(dfu);
}

status_t dfu_open_file(struct os_usb_file *file)
{
	struct dfu_file *dfu_file;
	struct dfu_device *dfu = usb_dev_priv(file->usb);

	dfu_file = os_mem_alloc(sizeof (struct dfu_file), "DFU_FILE");
	if (!dfu_file)
		return STATUS_INSUFFICIENT_RESOURCES;
	dfu_file->dev = dfu;
	dfu_file->file = file;
	xchg(&dfu_file->seq, DFU_SEQ_IDLE);
	file->priv_data = dfu_file;
	return STATUS_SUCCESS;
}

void dfu_close_file(struct os_usb_file *file)
{
	struct dfu_file *dfu_file = usb_file_priv(file);
	if (dfu_file) {
		struct dfu_device *dfu = dfu_file->dev;
		if (dfu_file->seq == DFU_SEQ_DNLOAD) {
			os_dbg(OS_DBG_DEBUG, "DNLOAD closing...\n");
			dfu_suffix_exit(dfu);
			dfu->eof_user = 1;
			dfu_buffer_run(dfu, 1);
		}
		dfu_buffer_exit(dfu);
		os_mem_free(dfu_file);
	}
}

status_t dfu_ioctl_file(struct os_usb_file *file,
			unsigned long code, void *arg, int *plength)
{
	status_t status = STATUS_SUCCESS;
	struct dfu_file *dfu_file = usb_file_priv(file);
	struct dfu_device *dfu = dfu_file->dev;
	os_usbif *usb = file->usb;
	struct dfu_seq *seq;
	struct dfu_info *info;
	struct dfu_firm *firm;
	int length = plength ? *plength : 0;

	*plength = 0;
	switch (code) {
	case USBDFUIOGETINFO:
		/* TODO: Implement in a Per-Driver Basis Style
		 *
		 * This should be implemented in a per-briver basis style
		 * (WMI?) to avoid file opening requirement.
		 * Currently we do not do too much work on wrappering WMI,
		 * thus we allow non-exclusive opened file descriptors to
		 * get DFU informations.
		 */
		os_dbg(OS_DBG_DEBUG, "USBDFUIOGETINFO - %d\n", length);
		if (length < sizeof (struct dfu_info)) {
			status = STATUS_BUFFER_TOO_SMALL;
			goto end;
		}
		info = (struct dfu_info *)arg;

		info->idVendor = usb->usb_device_desc->idVendor;
		info->idProduct = usb->usb_device_desc->idProduct;
		info->bcdDevice = usb->usb_device_desc->bcdDevice;

		os_mem_move(info->ucSerialNumber, dfu->dev_sn, MAX_DFU_SN);
		os_mem_move(info->ucInterfaceName, dfu->dev_name, MAX_DFU_NAME);

		info->bInterfaceProtocol = dfu->proto;
		info->bmAttributes = dfu->dfu_function_desc->bmAttributes;
		info->wDetachTimeOut = dfu->dfu_function_desc->wDetachTimeOut;
		info->wTransferSize = dfu->dfu_function_desc->wTransferSize;
		info->bcdDFUVersion = dfu->dfu_function_desc->bcdDFUVersion;

		info->bState = dfu->state;
		info->bStatus = dfu->status;
		info->iString = dfu->iString;
		*plength = sizeof (struct dfu_info);
		break;
	case USBDFUIOLASTSEQ:
		os_dbg(OS_DBG_DEBUG, "USBDFUIOLASTSEQ - %d\n", length);
		if (length < sizeof (struct dfu_seq)) {
			status = STATUS_BUFFER_TOO_SMALL;
			goto end;
		}
		seq = (struct dfu_seq *)arg;
		xchg(&seq->seq, dfu->seq_last);
		seq->seq_ended = dfu->seq_ended;
		seq->seq_error = dfu->seq_error;
		*plength = sizeof (struct dfu_seq);
		break;
	case USBDFUIODETACCH:
		os_dbg(OS_DBG_DEBUG, "USBDFUIODETACCH - %d\n", length);
		status = dfu_begin_seq(dfu, USBDFU_PROTO_RUNTIME, DFU_SEQ_DETACH);
		if (!OS_IS_SUCCESS(status)) goto end;
		xchg(&dfu_file->seq, DFU_SEQ_DETACH);
		status = dfu_poll_interface(dfu->dev, DFU_DETACH);
		if (!OS_IS_SUCCESS(status)) {
			dfu_end_seq(dfu, status);
			goto end;
		}
		/* Do not return STATUS_PENDING for ioctl. */
		status = STATUS_SUCCESS;
		break;
	case USBDFUIOABORT:
		os_dbg(OS_DBG_DEBUG, "USBDFUIOABORT - %d\n", length);
		status = dfu_begin_seq(dfu, USBDFU_PROTO_DFUMODE, DFU_SEQ_ABORT);
		if (!OS_IS_SUCCESS(status)) {
			os_dbg(OS_DBG_DEBUG, "Aborting sequence - %d\n", dfu->seq);
			dfu->seq_aborting = 1;
			dfu_get_status_sync(dfu);
			goto aborted;
		}
		xchg(&dfu_file->seq, DFU_SEQ_ABORT);
		status = dfu_poll_interface(dfu->dev, DFU_ABORT);
		if (!OS_IS_SUCCESS(status)) {
			dfu_end_seq(dfu, status);
			goto end;
		}
aborted:
		/* Do not return STATUS_PENDING for ioctl. */
		status = STATUS_SUCCESS;
		break;
	case USBDFUIODNLOAD:
		os_dbg(OS_DBG_DEBUG, "USBDFUIODNLOAD - %d\n", length);
		if (length < sizeof (struct dfu_firm)) {
			status = STATUS_BUFFER_TOO_SMALL;
			goto end;
		}
		firm = (struct dfu_firm *)arg;
		status = dfu_begin_seq(dfu, USBDFU_PROTO_DFUMODE, DFU_SEQ_DNLOAD);
		if (!OS_IS_SUCCESS(status)) goto end;
		xchg(&dfu_file->seq, DFU_SEQ_DNLOAD);
		dfu_suffix_init(dfu, firm);
		dfu_buffer_init(dfu);
		BUG_ON(status != STATUS_SUCCESS);
		break;
	case USBDFUIOUPLOAD:
		os_dbg(OS_DBG_DEBUG, "USBDFUIOUPLOAD - %d\n", length);
		status = dfu_begin_seq(dfu, USBDFU_PROTO_DFUMODE, DFU_SEQ_UPLOAD);
		if (!OS_IS_SUCCESS(status)) goto end;
		xchg(&dfu_file->seq, DFU_SEQ_UPLOAD);
		dfu_buffer_init(dfu);
		BUG_ON(status != STATUS_SUCCESS);
		break;
	default:
		status = STATUS_INVALID_PARAMETER;
		break;
	}
end:
	return status;
}

struct os_usb_driver dfu_driver = {
	USBDFU_CLASS,
	0, /* not surprise removeable */

	dfu_init_device,
	dfu_exit_device,
	dfu_start_device,
	dfu_stop_device,
	dfu_suspend_device,
	dfu_resume_device,

	dfu_open_file,
	dfu_close_file,
	dfu_read_file,
	dfu_write_file,
	dfu_ioctl_file,

	dfu_control_aval,
	dfu_control_cmpl,
};

status_t os_driver_init(void)
{
	return os_usb_declare_interface(&dfu_driver,
					sizeof (struct dfu_device));
}
