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
 * @(#)usb_dfu.c: device firmware upgrade host interfaces
 * $Id: usb_dfu.h,v 1.108 2011-10-17 01:40:34 zhenglv Exp $
 */
#ifndef __USB_DFU_H_INCLUDE__
#define __USB_DFU_H_INCLUDE__

#include "os_cfg.h"
#include <wdm/os_usb.h>
#include <host/usbdfu.h>

#include <pshpack1.h>

struct dfu_function_desc {
	uint8_t bLength;
	uint8_t bDescriptorType;
	uint8_t bmAttributes;
#define DFU_ATTR_WILL_DETACH		0x08
#define DFU_ATTR_MANIFESTATION_TOLERANT	0x04
#define DFU_ATTR_CAN_UPLOAD		0x02
#define DFU_ATTR_CAN_DNLOAD		0x01
	uint16_t wDetachTimeOut;
	uint16_t wTransferSize;
	uint16_t bcdDFUVersion;
};
#define USB_DT_DFU_FUNCTION	0x21

struct dfu_status {
	/* An indication of the status resulting from the execution of
	 * the most recent request.
	 */
	uint8_t bStatus;
	/* Minimum time, in milliseconds, that the host should wait before
	 * sending a subsequent DFU_GETSTATUS request.
	 */
	uint8_t bwPollTime[3];
	/* An indication of the state. */
	uint8_t bState;
	/* Index of status description in string table. */
	uint8_t iString;
};

struct dfu_suffix {
	uint32_t dwCRC;
	uint8_t bLength;
	uint8_t ucDfuSignature[3];
	uint16_t bcdDFU;
	uint16_t idVendor;
	uint16_t idProduct;
	uint16_t bcdDevice;
};

#include <poppack.h>

struct dfu_device {
	os_device dev;
	reflck_t seq;
	/* WDM status */
	status_t seq_status;
	/* DFU error status */
	uint8_t seq_error;
	int seq_ended;
	reflck_t seq_last;
	int seq_aborting;
	int seq_aborted;
	int seq_failure;

	/* attributes from GETSTATUS */
	uint8_t state;
	uint8_t status;
	uint8_t iString;

	/* attributes from enumerations */
	uint8_t proto;
	char dev_sn[MAX_DFU_SN];
	char dev_name[MAX_DFU_NAME];
	struct dfu_function_desc *dfu_function_desc;

	/* attributes from dnload suffix */
	uint32_t filecrc;
	char suffix_buf[DFU_SUFFIX_SIZE];
	size_t suffix_pos;

	/* asynchronous parameters */
	union {
		struct dfu_status status;
		uint8_t state;
	} params;

	/* The real block size should be wTransferSize, while the pseudo
	 * block size is a roundup of real one for optimization purpose.
	 */
	size_t xfr_size;
	size_t pad_size;
#define DFU_BUFFERED_XFRS	4
	/* buffer storing user's IO data */
	uint8_t *buffer;
	size_t length;
	/* XXX: Synchronizations of Variables
	 * These requirements are not apply to startup (dfu_buffer_init)
	 * and cleanup (dfu_buffer_exit) codes as they are protected by
	 * the DFU operation sequencing.
	 * If some variables are only used inside the serialized work
	 * queue, these requirements are not apply yet.
	 */
	/* When this is signalled for user space, it is required to check
	 * whether there's really a poll condition asserted.
	 * This is because the signal is caused by the condition evaluated
	 * of the circular buffer POS/EOF's snapshot, and the condition
	 * may not match the current POS/EOF's snapshot when the user
	 * space is signalled.
	 */
	os_waiter waiter;
	int waiter_running;
	/* POS is the offset of the firmware, it is modified only when the
	 * DNLOAD/UPLOAD URB has completed.
	 */
	unsigned long pos;
	/* So we only get the following three variables synced across
	 * threads, since rpos/wpos of circular buffer is handled in
	 * seperated threads, its safe to call dfu_buffer_space or
	 * dfu_buffer_count without any lock for threads.  The problem
	 * occurs when the eof_user/eof_firm shoudl be set/got.  Please
	 * refer to the "Circular Buffer POS/EOF" comments.
	 */
	ASSIGN_CIRCBF16_MEMBER(circbf);
	int eof_user;
	int eof_firm;

	int sync_getstatus;		/* require DFU_GETSTATUS */
	int sync_clrstatus;		/* require DFU_CLRSTATUS */
	int synced;

	int stopping;

	os_timer *poll_timer;
	uint32_t poll_time;
};

struct dfu_urb {
	uint8_t request;
};

struct dfu_file {
	int blocks;
	struct dfu_device *dev;
	struct os_usb_file *file;
	reflck_t seq;
};

/* DFU poller */
void dfu_timer_stop(struct dfu_device *dfu);
status_t dfu_timer_start(struct dfu_device *dfu);
void dfu_timer_submit(struct dfu_device *dfu);

/* DFU usb */
status_t dfu_submit_control_async(os_device dev, struct os_usb_urb *urb,
				  uint8_t dir, uint8_t request, uint16_t value,
				  uint8_t *info, uint16_t length);

status_t dfu_poll_interface(os_device dev, uint8_t request);
struct dfu_function_desc *dfu_match_function(struct usb_intf_desc *intf_desc,
					     uint16_t total_length);

/* DFU circbf */
void dfu_buffer_calc(struct dfu_device *dfu, uint16_t xfr_size);
void dfu_buffer_run(struct dfu_device *dfu, int user);
void dfu_buffer_init(struct dfu_device *dfu);
void dfu_buffer_exit(struct dfu_device *dfu);
void dfu_buffer_read(struct dfu_device *dfu, size_t bytes);
void dfu_buffer_write(struct dfu_device *dfu, size_t bytes);
size_t dfu_buffer_space(struct dfu_device *dfu);
size_t dfu_buffer_count(struct dfu_device *dfu);

/* DFU suffix */
void dfu_suffix_init(struct dfu_device *dfu, struct dfu_firm *firm);
void dfu_suffix_exit(struct dfu_device *dfu);

/* DFU userspace */
void dfu_file_idle(struct dfu_device *dfu);
void dfu_file_wake(struct dfu_device *dfu);

/* DFU sequence */
void dfu_init_seq(struct dfu_device *dfu);
status_t dfu_begin_seq(struct dfu_device *dfu, uint8_t proto, reflck_t seq);
void dfu_end_seq(struct dfu_device *dfu, status_t status);

#endif
