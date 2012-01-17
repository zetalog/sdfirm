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
 * @(#)usb.h: universal serial bus interfaces
 * $Id: usb.h,v 1.279 2011-10-19 10:19:18 zhenglv Exp $
 */

#ifndef __USB_H_INCLUDE__
#define __USB_H_INCLUDE__

#include <target/config.h>
#include <target/generic.h>
#include <target/timer.h>
#include <target/bulk.h>

#ifdef CONFIG_USB_DEBUG
/* IRQ debug ID */
/* device IRQ */
#define USBD_IRQ_RESET		0	/* RESET signalling */
#define USBD_IRQ_RESUME		1	/* RESUME */
#define USBD_IRQ_SUSPEND	2	/* SUSPEND */
#define USBD_IRQ_SOF		3	/* SOF token received */
#define USBD_IRQ_DETACH		4	/* DETACH, useless */
/* request IRQ */
#define USBD_IRQ_SETUP		5	/* SETUP token received */
#define USBD_IRQ_OUT		6	/* OUT token received */
#define USBD_IRQ_IN		7	/* IN token received */
#define USBD_IRQ_STALL		8	/* STALL handshake sent */
#define USBD_IRQ_NAKOUT		9	/* NAK handshake sent, useless */
#define USBD_IRQ_NAKIN		10	/* NAK handshake received, useless */
/* OTG IRQ */
#define OTG_IRQ_VBUS		11	/* VBUS PIN detection */
#define OTG_IRQ_ID		12	/* ID PIN detection */

#define usb_debug(tag, val)	dbg_print((tag), (val))
#define usb_debug_eid(eid)					\
	do {							\
		usb_debug(USB_DEBUG_ENDP, eid);			\
	} while (0)
#define usb_debug_iid(iid)					\
	do {							\
		usb_debug(USB_DEBUG_INTF, iid);			\
	} while (0)
#define usb_debug_irq(irq)					\
	do {							\
		usb_debug(USB_DEBUG_IRQ, irq);			\
	} while (0)
#else
#define usb_debug(type, val)
#define usb_debug_eid(eid)
#define usb_debug_iid(iid)
#define usb_debug_irq(irq)
#endif

#include <driver/usb.h>

typedef void (*usb_io_cb)(void);
typedef uint16_t (*usb_size_cb)(void);

#define USB_VERSION_DEFAULT		0x110

/* index of configuration */
#define USB_CID_DEFAULT		0

#define USB_CID2CONF(cid)	(cid+1)
#define USB_CONF_DEFAULT	0
#define USB_ADDR_DEFAULT	0

/* ============================================================ *
 * USB2.0 chapter 9
 * ============================================================ */
/* Table 9-1 Device States */
/* NOTE: state before ATTACHED should be handled in silicon logic */
#define USB_STATE_ATTACHED		0x00
#define USB_STATE_POWERED		0x01
#define USB_STATE_SUSPENDED		0x02
#define USB_STATE_DEFAULT		0x03
#define USB_STATE_ADDRESS		0x04
#define USB_STATE_CONFIGURED		0x05

/* 1ms precesion for endpoint polling interval scheduling */
#define USB_ENDP_INTERVAL		1

/* Direction (DIR) & Endpoint ID (EID) Field
 *
 * These bit flags are used in 2 structures defined in the USB standard:
 * 1. the endpoint descriptors' bEndpointAddress (DIR & EID) field; and
 * 2. the control requests' bRequestType (DIR) & wIndex (DIR & EID) field.
 */
#define USB_DIR_OFFSET			7
#define USB_DIR_OUT			0	/* to device */
#define USB_DIR_IN			1	/* to host */
#define USB_DIR_MASK			(1 << USB_DIR_OFFSET)
#define USB_EID_DEFAULT			0	/* endpoint 0 is the default control endpoint */
#define USB_EID_MASK			0x0F
#define USB_ADDR2EID(addr)		((uint8_t)((addr) & USB_EID_MASK))
#define USB_ADDR2DIR(addr)		((uint8_t)(((addr) & USB_DIR_MASK) >> USB_DIR_OFFSET))
#define USB_DIR2ADDR(dir)		((uint8_t)((dir) << USB_DIR_OFFSET))
#define USB_CTRLADDR(eid)		USB_ADDR(USB_DIR_OUT, eid)
#define USB_ENDPADDR(dir, eid)		((uint8_t)(USB_DIR2ADDR(dir) | (eid)))
#ifdef CONFIG_USBD_ENDP_DUPLEX
#define USB_MAX_DIRS			2
#define USB_ADDR(dir, eid)		USB_ENDPADDR(dir, eid)
#define USB_SAVEADDR(addr)		(addr)
#else
#define USB_MAX_DIRS			1
#define USB_ADDR(dir, eid)		(eid)
#define USB_SAVEADDR(addr)		USB_ADDR2EID(addr)
/* TODO: why following codes can not be enabled on Non-Duplex hardwares? */
/*
#define USB_ADDR(dir, eid)		USB_ENDPADDR(dir, eid)
#define USB_SAVEADDR(addr)		(addr)
*/
#endif

/* Table 9-2 */
/**
 * struct usb_ctrlrequest - SETUP data for a USB device control request
 * @bRequestType: matches the USB bmRequestType field
 * @bRequest: matches the USB bRequest field
 * @wValue: matches the USB wValue field (le16 byte order)
 * @wIndex: matches the USB wIndex field (le16 byte order)
 * @wLength: matches the USB wLength field (le16 byte order)
 *
 * This structure is used to send control requests to a USB device.  It
 * matches the different fields of the USB 2.0 Spec section 9.3,
 * table 9-2.  See the USB spec for a fuller description of the different
 * fields, and what they are used for.
 *
 * Note that the driver for any interface can issue control requests.  For
 * most devices, interfaces don't coordinate with each other, so such
 * requests may be made at any time.
 */
struct usb_ctlsetup {
	uint8_t  bmRequestType;

#define USB_TYPE_MASK			(0x03 << 5)
#define USB_TYPE_STANDARD		(0x00 << 5)
#define USB_TYPE_CLASS			(0x01 << 5)
#define USB_TYPE_VENDOR			(0x02 << 5)
#define USB_TYPE_RESERVED		(0x03 << 5)

#define USB_RECP_MASK			0x1F
#define USB_RECP_DEVICE			0x00
#define USB_RECP_INTERFACE		0x01
#define USB_RECP_ENDPOINT		0x02
#define USB_RECP_OTHER			0x03

	/* XXX: EID Mask Correctness
	 * This macro is correct only if USB_EID_MASK is 0x0F, or a
	 * seperate EID mask should be defined for the future extended
	 * control setup request.
	 *
	 * The following tow macros are used to extract DIR from wIndex
	 * and bRequestType.
	 */
#define USB_SETUP2DIR(req)		USB_ADDR2DIR(req)
#define USB_SETUP2TYPE(req)		((uint8_t)(req & USB_TYPE_MASK))
#define USB_SETUP2RECP(req)		((uint8_t)(req & USB_RECP_MASK))

	uint8_t  bRequest;
	uint16_t wValue;
	uint16_t wIndex;
	uint16_t wLength;
};
#define USB_SETUP_REQUEST_SIZE		8

/* Table 9-4 Request Codes */
#define USB_REQ_GET_STATUS		0x00
#define USB_REQ_CLEAR_FEATURE		0x01
#define USB_REQ_SET_FEATURE		0x03
#define USB_REQ_SET_ADDRESS		0x05
#define USB_REQ_GET_DESCRIPTOR		0x06
#define USB_REQ_SET_DESCRIPTOR		0x07
#define USB_REQ_GET_CONFIGURATION	0x08
#define USB_REQ_SET_CONFIGURATION	0x09
#define USB_REQ_GET_INTERFACE		0x0A
#define USB_REQ_SET_INTERFACE		0x0B
#define USB_REQ_SYNCH_FRAME		0x0C

/* Table 9-5 Descriptor Type */
#define USB_DT_DEVICE			0x01
#define USB_DT_CONFIG			0x02
#define USB_DT_STRING			0x03
#define USB_DT_INTERFACE		0x04
#define USB_DT_ENDPOINT			0x05
#define USB_DT_DEVICE_QUALIFIER		0x06
#define USB_DT_OTHER_SPEED_CONFIG	0x07
#define USB_DT_INTERFACE_POWER		0x08

/* Conventional codes for class-specific descriptors.  The convention is
 * defined in the USB "Common Class" Spec (3.11).  Individual class specs
 * are authoritative for their usage, not the "common class" writeup.
 */
#define USB_DT_CS_DEVICE		(USB_TYPE_CLASS | USB_DT_DEVICE)
#define USB_DT_CS_CONFIG		(USB_TYPE_CLASS | USB_DT_CONFIG)
#define USB_DT_CS_STRING		(USB_TYPE_CLASS | USB_DT_STRING)
#define USB_DT_CS_INTERFACE		(USB_TYPE_CLASS | USB_DT_INTERFACE)
#define USB_DT_CS_ENDPOINT		(USB_TYPE_CLASS | USB_DT_ENDPOINT)

/* Table 9-6 Feature Selector */
#define USB_ENDPOINT_HALT		0x00
#define USB_DEVICE_REMOTE_WAKEUP	0x01
#define USB_DEVICE_TEST_MODE		0x02

/* Table 9-8: device descriptor data structure */
struct usb_device_desc {
	uint8_t  bLength;
	uint8_t  bDescriptorType;
	uint16_t bcdUSB;
	uint8_t  bDeviceClass;
/* Device and/or Interface Class codes
 * as found in bDeviceClass or bInterfaceClass
 * and defined by www.usb.org documents
 */
#define USB_INTERFACE_CLASS_INTERFACE	0x00	/* for DeviceClass */
#define USB_INTERFACE_CLASS_HUB		0x09
#define USB_INTERFACE_CLASS_MISC	0xef
#define USB_INTERFACE_CLASS_APP		0xfe
#define USB_INTERFACE_CLASS_VENDOR	0xff

	uint8_t  bDeviceSubClass;
#define USB_DEVICE_SUBCLASS_NONE	0x00
	uint8_t  bDeviceProtocol;
#define USB_PROTO_DEVICE_DEFAULT	0x00
	uint8_t  bMaxPacketSize0;
	uint16_t idVendor;
	uint16_t idProduct;
	uint16_t bcdDevice;
	uint8_t  iManufacture;
	uint8_t  iProduct;
	uint8_t  iSerialNumber;
	uint8_t  bNumConfigurations;
};
__TEXT_TYPE__(struct usb_device_desc, usb_device_desc_t);
#define USB_DT_DEVICE_SIZE		18

/* Table 9-10 */
struct usb_conf_desc {
	uint8_t  bLength;
	uint8_t  bDescriptorType;
	uint16_t wTotalLength;
	uint8_t  bNumInterfaces;
	uint8_t  bConfigurationValue;
	uint8_t  iConfiguration;
	uint8_t  bmAttributes;
#define USB_CONFIG_ATTR_DEFAULT		0x80
#define USB_CONFIG_ATTR_SELF_POWERED	(1<<6)
#define USB_CONFIG_ATTR_REMOTE_WAKEUP	(1<<7)
	uint8_t  bMaxPower;
#define USB_POWER_PER_2MA		0.5
};
__TEXT_TYPE__(struct usb_conf_desc, usb_conf_desc_t);
#define USB_DT_CONFIG_SIZE		9

/* GET_STATUS flags */
#define USB_DEVICE_STATUS_SELF_POWERED	1
#define USB_DEVICE_STATUS_REMOTE_WAKEUP	2

/* Table 9-12 */
struct usb_intf_desc {
	uint8_t bLength;
	uint8_t bDescriptorType;
	uint8_t bInterfaceNumber;
	uint8_t bAlternateSetting;
	uint8_t bNumEndpoints;
	uint8_t bInterfaceClass;
	uint8_t bInterfaceSubClass;
	uint8_t bInterfaceProtocol;
	uint8_t iInterface;
};
__TEXT_TYPE__(struct usb_intf_desc, usb_intf_desc_t);
#define USB_DT_INTERFACE_SIZE		9

#define USB_ENDP_SIZE_8			0
#define USB_ENDP_SIZE_16		1
#define USB_ENDP_SIZE_32		2
#define USB_ENDP_SIZE_64		3
#define USB_ENDP_SIZE_128		4
#define USB_ENDP_SIZE_256		5
#define USB_ENDP_SIZE_512		6
#define USB_ENDP_SIZE_1024		7
uint8_t usb_endpoint_size_type(uint16_t size);

/* Table 9-13 */
struct usb_endp_desc {
	uint8_t  bLength;
	uint8_t  bDescriptorType;
	uint8_t  bEndpointAddress;

	uint8_t  bmAttributes;
#define USB_ENDP_TYPE_MASK	0x03
#define USB_ENDP_CONTROL	0x00
#define USB_ENDP_ISOCHRONOUS	0x01
#define USB_ENDP_BULK		0x02
#define USB_ENDP_INTERRUPT	0x03
#define USB_ENDP_INVALID	0x04
	/* If not isochronous endpoint, bit 5..2 are reserved and must be
	 * set to zero.  If isochronous, they are defined as follows:
	 *	Bits 3..2 Synchronization Type
	 */
#define USB_ENDP_MASK_SYNC	0x0C
#define USB_ENDP_SYNC_NONE	0x00
#define USB_ENDP_SYNC_ASYNC	0x04
#define USB_ENDP_SYNC_ADAPT	0x08
#define USB_ENDP_SYNC_SYNC	0x0C
	/*	Bits 5..4 Usage Type
	 */
#define USB_ENDP_MASK_USAGE	0x30
#define USB_ENDP_USAGE_DATA	0x00
#define USB_ENDP_USAGE_FEEDBACK	0x10
#define USB_ENDP_USAGE_IMPLICIT	0x20

	uint16_t wMaxPacketSize;
	uint8_t  bInterval;
};
__TEXT_TYPE__(struct usb_endp_desc, usb_endp_desc_t);
#define USB_DT_ENDPOINT_SIZE	7

/* GET_STATUS flags */
#define USB_ENDP_STATUS_HALT	0x01

/* Table 9-16 */
struct usb_string_desc {
	uint8_t  bLength;
	uint8_t  bDescriptorType;
#define USB_STRING_LANGID		0x00
#define USB_STRING_FACTORY		0x01
#define USB_STRING_PRODUCT		0x02
#define USB_STRING_SERIALNO		0x03
};

/* ============================================================ *
 * USB LANGIDs
 * ============================================================ */
#define USB_LANGID_EN_US	0x0409
#define USB_LANGID_CN_ZH	0x0804
#define USB_LANGID_JA_JP	0x0411

#include <target/usb_hcd.h>
#include <target/usb_dev.h>

#define USB_MODE_HOST	0x00
#define USB_MODE_DEVICE	0x01

#if defined(CONFIG_USB_HCD)
#define USB_MODE_DEF	USB_MODE_HOST
#elif defined(CONFIG_USB_DEV)
#define USB_MODE_DEF	USB_MODE_DEVICE
#endif

#if defined(CONFIG_USB_DEV) && defined(CONFIG_USB_HCD)
uint8_t usb_get_mode(void);
void usb_set_mode(uint8_t mode);
#else
#define usb_get_mode()		USB_MODE_DEF
#define usb_set_mode(mode)
#endif

void usb_switch_mode(uint8_t mode);
void usb_wakeup_state(void);

tid_t usb_timer_register(void);

#endif /* __USB_H_INCLUDE__ */
