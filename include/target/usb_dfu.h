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
 * @(#)usb_dfu.h: device firmware upgrade interfaces
 * $Id: usb_dfu.h,v 1.13 2011-09-05 04:22:29 zhenglv Exp $
 */

#ifndef __USB_DFU_H_INCLUDE__
#define __USB_DFU_H_INCLUDE__

#include <target/config.h>
#include <target/generic.h>
#include <target/usb.h>
#include <target/jiffies.h>

#ifdef CONFIG_DFU_DEBUG
#define dfu_debug(tag, val)	dbg_print((tag), (val))
#else
#define dfu_debug(tag, val)
#endif

#define USB_INTERFACE_CLASS_DFU			0xFE
#define DFU_INTERFACE_SUBCLASS			1
#define DFU_INTERFACE_PROTOCOL_RUNTIME		1
#define DFU_INTERFACE_PROTOCOL_DFUMODE		2

#define DFU_VERSION				0x0100

#define DFU_REQ_DETACH				0x00
#define DFU_REQ_DNLOAD				0x01
#define DFU_REQ_UPLOAD				0x02
#define DFU_REQ_GETSTATUS			0x03
#define DFU_REQ_CLRSTATUS			0x04
#define DFU_REQ_GETSTATE			0x05
#define DFU_REQ_ABORT				0x06

#define DFU_STATE_APP_IDLE			0x00
#define DFU_STATE_APP_DETACH			0x01
#define DFU_STATE_DFU_IDLE			0x02
#define DFU_STATE_DFU_DNLOAD_SYNC		0x03
#define DFU_STATE_DFU_DNBUSY			0x04
#define DFU_STATE_DFU_DNLOAD_IDLE		0x05
#define DFU_STATE_DFU_MANIFEST_SYNC		0x06
#define DFU_STATE_DFU_MANIFEST			0x07
#define DFU_STATE_DFU_MANIFEST_WAIT_RESET	0x08
#define DFU_STATE_DFU_UPLOAD_IDLE		0x09
#define DFU_STATE_DFU_ERROR			0x0A

#define DFU_STATUS_OK				0x00
#define DFU_STATUS_ERR_TARGET			0x01
#define DFU_STATUS_ERR_FILE			0x02
#define DFU_STATUS_ERR_WRITE			0x03
#define DFU_STATUS_ERR_ERASE			0x04
#define DFU_STATUS_ERR_CHECK_ERASED		0x05
#define DFU_STATUS_ERR_PROG			0x06
#define DFU_STATUS_ERR_VERIFY			0x07
#define DFU_STATUS_ERR_ADDRESS			0x08
#define DFU_STATUS_ERR_NOTDONE			0x09
#define DFU_STATUS_ERR_FIRMWARE			0x0A
#define DFU_STATUS_ERR_VENDOR			0x0B
#define DFU_STATUS_ERR_USER			0x0C
#define DFU_STATUS_ERR_POR			0x0D
#define DFU_STATUS_ERR_UNKNOWN			0x0E
#define DFU_STATUS_ERR_STALLEDPKT		0x0F
#define DFU_MAX_ERRORS				0x10

struct dfu_function_desc {
	uint8_t bLength;
	uint8_t bDescriptorType;
#define DFU_DT_FUNCTION_DESC		0x21
	uint8_t bmAttributes;
#define DFU_ATTR_WILL_DETACH		0x08
#define DFU_ATTR_MANIFESTATION_TOLERANT	0x04
#define DFU_ATTR_CAN_UPLOAD		0x02
#define DFU_ATTR_CAN_DNLOAD		0x01
	uint16_t wDetachTimeOut;
	uint16_t wTransferSize;
	uint16_t bcdDFUVersion;
};
#define DFU_DT_FUNCTION_SIZE		9

struct dfu_suffix {
	uint32_t dwCRC;
	uint8_t bLength;
	uint8_t ucDfuSignature[3];
	uint16_t bcdDFU;
	uint16_t idVendor;
	uint16_t idProduct;
	uint16_t bcdDevice;
};
#define DFU_SUFFIX_SIZE			16
#define DFU_SUFFIX_MAGIC		0x10444655	/* 16+DFU */

#endif /* __USB_DFU_H_INCLUDE__ */
