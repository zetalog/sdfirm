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
 * @(#)usb_hid.h: human interface device interfaces
 * $Id: usb_hid.h,v 1.54 2011-02-21 02:12:58 zhenglv Exp $
 */

#ifndef __USB_HID_H_INCLUDE__
#define __USB_HID_H_INCLUDE__

#include <target/usb.h>
#include <target/jiffies.h>

typedef uint8_t hid_rid_t;
typedef void (*hid_io_cb)(uint8_t endp);

#ifdef CONFIG_HID_DEBUG
#define hid_debug(tag, val)	dbg_print((tag), (val))
#else
#define hid_debug(tag, val)
#endif

#define HID_INTR_PENDING_SET	0x00
#define HID_INTR_PENDING_UNSET	0x01
#define HID_INTR_RUNNING_SET	0x02
#define HID_INTR_RUNNING_UNSET	0x03

/*
 * USB HID (Human Interface Device) interface class code
 */

#define USB_INTERFACE_CLASS_HID		3

/*
 * USB HID interface subclass and protocol codes
 */

#define USB_INTERFACE_SUBCLASS_BOOT	1
#define USB_INTERFACE_PROTOCOL_KEYBOARD	1
#define USB_INTERFACE_PROTOCOL_MOUSE	2

/*
 * HID class requests
 */

#define HID_REQ_GET_REPORT		0x01
#define HID_REQ_GET_IDLE		0x02
#define HID_REQ_GET_PROTOCOL		0x03
#define HID_REQ_SET_REPORT		0x09
#define HID_REQ_SET_IDLE		0x0A
#define HID_REQ_SET_PROTOCOL		0x0B

/*
 * HID class descriptor types
 */

#define HID_DT_HID			(USB_TYPE_CLASS | 0x01)
#define HID_DT_REPORT			(USB_TYPE_CLASS | 0x02)
#define HID_DT_PHYSICAL			(USB_TYPE_CLASS | 0x03)

/*
 * We parse each description item into this structure. Short items data
 * values are expanded to 32-bit signed int, long items contain a pointer
 * into the data area.
 */

struct hid_item {
	uint8_t format;
	uint8_t size;
	uint8_t type;
	uint8_t tag;
	uint32_t udata;
};
__TEXT_TYPE__(struct hid_item, hid_item_t);

/*
 * HID report item format
 */

#define HID_ITEM_FORMAT_SHORT	0
#define HID_ITEM_FORMAT_LONG	1

/*
 * Special tag indicating long items
 */

#define HID_ITEM_TAG_LONG	15
#define HID_LONG_ITEM_LEADING	0xFE

/*
 * HID report descriptor item type (prefix bit 2,3)
 */

#define HID_ITEM_TYPE_MAIN		0
#define HID_ITEM_TYPE_GLOBAL		1
#define HID_ITEM_TYPE_LOCAL		2
#define HID_ITEM_TYPE_RESERVED		3

/*
 * HID report descriptor main item tags
 */

#define HID_MAIN_ITEM_TAG_INPUT			8
#define HID_MAIN_ITEM_TAG_OUTPUT		9
#define HID_MAIN_ITEM_TAG_FEATURE		11
#define HID_MAIN_ITEM_TAG_COLLECTION		10
#define HID_MAIN_ITEM_TAG_END_COLLECTION	12

/*
 * HID report descriptor main item contents
 */

#define HID_MAIN_ITEM_DATA		0x000
#define HID_MAIN_ITEM_CONSTANT		0x001

#define HID_MAIN_ITEM_ARRAY		0x000
#define HID_MAIN_ITEM_VARIABLE		0x002

#define HID_MAIN_ITEM_ABSOLUTE		0x000
#define HID_MAIN_ITEM_RELATIVE		0x004

#define HID_MAIN_ITEM_WRAP		0x008
#define HID_MAIN_ITEM_NONLINEAR		0x010
#define HID_MAIN_ITEM_NO_PREFERRED	0x020
#define HID_MAIN_ITEM_NULL_STATE	0x040
#define HID_MAIN_ITEM_VOLATILE		0x080
#define HID_MAIN_ITEM_BUFFERED_BYTE	0x100


/*
 * HID report descriptor collection item types
 */

#define HID_COLLECTION_PHYSICAL		0
#define HID_COLLECTION_APPLICATION	1
#define HID_COLLECTION_LOGICAL		2
#define HID_COLLECTION_REPORT		3
#define HID_COLLECTION_NAMED_ARRAY	4
#define HID_COLLECTION_USAGE_SWITCH	5
#define HID_COLLECTION_USAGE_MODIFIER	6

/*
 * HID report descriptor global item tags
 */

#define HID_GLOBAL_ITEM_TAG_USAGE_PAGE		0
#define HID_GLOBAL_ITEM_TAG_LOGICAL_MINIMUM	1
#define HID_GLOBAL_ITEM_TAG_LOGICAL_MAXIMUM	2
#define HID_GLOBAL_ITEM_TAG_PHYSICAL_MINIMUM	3
#define HID_GLOBAL_ITEM_TAG_PHYSICAL_MAXIMUM	4
#define HID_GLOBAL_ITEM_TAG_UNIT_EXPONENT	5
#define HID_GLOBAL_ITEM_TAG_UNIT		6
#define HID_GLOBAL_ITEM_TAG_REPORT_SIZE		7
#define HID_GLOBAL_ITEM_TAG_REPORT_ID		8
#define HID_GLOBAL_ITEM_TAG_REPORT_COUNT	9
#define HID_GLOBAL_ITEM_TAG_PUSH		10
#define HID_GLOBAL_ITEM_TAG_POP			11

/*
 * HID report descriptor local item tags
 */

#define HID_LOCAL_ITEM_TAG_USAGE		0
#define HID_LOCAL_ITEM_TAG_USAGE_MINIMUM	1
#define HID_LOCAL_ITEM_TAG_USAGE_MAXIMUM	2
#define HID_LOCAL_ITEM_TAG_DESIGNATOR_INDEX	3
#define HID_LOCAL_ITEM_TAG_DESIGNATOR_MINIMUM	4
#define HID_LOCAL_ITEM_TAG_DESIGNATOR_MAXIMUM	5
#define HID_LOCAL_ITEM_TAG_STRING_INDEX		7
#define HID_LOCAL_ITEM_TAG_STRING_MINIMUM	8
#define HID_LOCAL_ITEM_TAG_STRING_MAXIMUM	9
#define HID_LOCAL_ITEM_TAG_DELIMITER		10

/*
 * HID usage tables
 */

#define HID_USAGE_PAGE		0xffff0000

#define HID_UP_UNDEFINED	0x0000
#define HID_UP_GENDESK		0x0001
#define HID_UP_SIMULATION	0x0002
#define HID_UP_KEYBOARD		0x0007
#define HID_UP_LED		0x0008
#define HID_UP_BUTTON		0x0009
#define HID_UP_ORDINAL		0x000a
#define HID_UP_CONSUMER		0x000c
#define HID_UP_DIGITIZER	0x000d
#define HID_UP_PID		0x000f
#define HID_UP_HPVENDOR         0xff7f
#define HID_UP_MSVENDOR		0xff00
#define HID_UP_CUSTOM		0x00ff
#define HID_UP_LOGIVENDOR	0xffbc

#define HID_USAGE		0x0000ffff

#define HID_GD_POINTER		0x0001
#define HID_GD_KEYBOARD		0x0006
#define HID_GD_KEYPAD		0x0007

/*
 * HID report types --- Ouch! HID spec says 1 2 3!
 */

#define HID_INPUT_REPORT	0
#define HID_OUTPUT_REPORT	1
#define HID_FEATURE_REPORT	2

#define HID_UsagePage(page)				\
	{						\
		HID_ITEM_FORMAT_SHORT,			\
		1,					\
		HID_ITEM_TYPE_GLOBAL,			\
		HID_GLOBAL_ITEM_TAG_USAGE_PAGE,		\
		page,					\
	}

#define HID_Usage(type)					\
	{						\
		HID_ITEM_FORMAT_SHORT,			\
		1,					\
		HID_ITEM_TYPE_LOCAL,			\
		HID_LOCAL_ITEM_TAG_USAGE,		\
		type,					\
	}

#define HID_Collection(type)				\
	{						\
		HID_ITEM_FORMAT_SHORT,			\
		1,					\
		HID_ITEM_TYPE_MAIN,			\
		HID_MAIN_ITEM_TAG_COLLECTION,		\
		type,					\
	}

#define HID_EndCollection()				\
	{						\
		HID_ITEM_FORMAT_SHORT,			\
		0,					\
		HID_ITEM_TYPE_MAIN,			\
		HID_MAIN_ITEM_TAG_END_COLLECTION,	\
		0,					\
	}

#define HID_UsageMinimum(val)				\
	{						\
		HID_ITEM_FORMAT_SHORT,			\
		1,					\
		HID_ITEM_TYPE_LOCAL,			\
		HID_LOCAL_ITEM_TAG_USAGE_MINIMUM,	\
		val,					\
	}

#define HID_UsageMaximum(val)				\
	{						\
		HID_ITEM_FORMAT_SHORT,			\
		1,					\
		HID_ITEM_TYPE_LOCAL,			\
		HID_LOCAL_ITEM_TAG_USAGE_MAXIMUM,	\
		val,					\
	}

#define HID_LogicalMinimum(val)				\
	{						\
		HID_ITEM_FORMAT_SHORT,			\
		1,					\
		HID_ITEM_TYPE_GLOBAL,			\
		HID_GLOBAL_ITEM_TAG_LOGICAL_MINIMUM,	\
		val,					\
	}

#define HID_LogicalMaximum(val)				\
	{						\
		HID_ITEM_FORMAT_SHORT,			\
		1,					\
		HID_ITEM_TYPE_GLOBAL,			\
		HID_GLOBAL_ITEM_TAG_LOGICAL_MAXIMUM,	\
		val,					\
	}

#define HID_ReportSize(size)				\
	{						\
		HID_ITEM_FORMAT_SHORT,			\
		1,					\
		HID_ITEM_TYPE_GLOBAL,			\
		HID_GLOBAL_ITEM_TAG_REPORT_SIZE,	\
		size,					\
	}

#define HID_ReportCount(count)				\
	{						\
		HID_ITEM_FORMAT_SHORT,			\
		1,					\
		HID_ITEM_TYPE_GLOBAL,			\
		HID_GLOBAL_ITEM_TAG_REPORT_COUNT,	\
		count,					\
	}

#define HID_Input(flags)				\
	{						\
		HID_ITEM_FORMAT_SHORT,			\
		1,					\
		HID_ITEM_TYPE_MAIN,			\
		HID_MAIN_ITEM_TAG_INPUT,		\
		flags,					\
	}

#define HID_Output(flags)				\
	{						\
		HID_ITEM_FORMAT_SHORT,			\
		1,					\
		HID_ITEM_TYPE_MAIN,			\
		HID_MAIN_ITEM_TAG_OUTPUT,		\
		flags,					\
	}

#define HID_KEYBOARD_1		30
#define HID_KEYBOARD_2		31
#define HID_KEYBOARD_3		32
#define HID_KEYBOARD_4		33
#define HID_KEYBOARD_5		34
#define HID_KEYBOARD_6		35
#define HID_KEYBOARD_7		36
#define HID_KEYBOARD_8		37
#define HID_KEYBOARD_9		38
#define HID_KEYBOARD_0		39
#define HID_KEYBOARD_ENTER	40
#define HID_KEYBOARD_ESCAPE	41

#define HID_KEYBOARD_DIVIDE	56
#define HID_KEYBOARD_MULTIPLY	37
#define HID_KEYBOARD_MINUS	45
#define HID_KEYBOARD_PLUS	46

#define HID_KEYSTATE_BEGIN	224
#define HID_KEYBOARD_LSHIFT	225
#define HID_KEYSTATE_END	231

struct hid_desc {
	uint8_t bLength;
	uint8_t bDescriptorType;
	uint16_t bcdHID;
#define HID_VERSION_DEFAULT		0x111

	uint8_t bCountryCode;
	uint8_t bNumDescriptors;

#if 0
	/* optional part of hid_desc */
	/*
	struct hid_desc_opt {
		uint8_t bDescriptorType;
		uint16_t wDescriptorLength;
	};
	*/
	/* struct hid_desc_opt[bNumDescriptors] */
#endif
};
__TEXT_TYPE__(struct hid_desc, hid_desc_t);
#define HID_DT_HID_SIZE		6
#define HID_DT_HID_OPT_SIZE	3

struct hid_field_desc {
	uint8_t physical;		/* physical usage for this field */
	uint8_t logical;		/* logical usage for this field */
	uint8_t application;		/* application usage for this field */
	uint8_t flags;			/* main-item flags (i.e. volatile,array,constant) */
	uint8_t report_offset;		/* bit offset in the report */
	uint8_t report_size;		/* size of this field in the report */
	uint8_t report_count;		/* number of this field in the report */
	uint8_t report_type;		/* (input,output,feature) */
	int32_t logical_minimum;
	int32_t logical_maximum;
	int32_t physical_minimum;
	int32_t physical_maximum;
	int32_t unit_exponent;
	uint8_t unit;
};

struct hid_report_desc {
	hid_rid_t rid;			/* id of this report */
	uint8_t type;			/* report type */
	uint8_t nr_fields;		/* maximum valid field index */
	struct hid_field_desc *field;	/* fields of the report */
};

struct hid_report_ctrl {
#if CONFIG_HID_DURATION
	uint8_t duration;
	tick_t last_poll;
#endif
	uint8_t flags;
#define HID_REPORT_RUNNING	0x01
#define HID_REPORT_PENDING	0x02
};

struct hid_report {
	urb_size_t input_len;
	urb_size_t output_len;
	hid_io_cb poll;
	hid_io_cb iocb;
	hid_io_cb done;
	usb_size_cb config_len;
	io_cb ctrl;
};
__TEXT_TYPE__(const struct hid_report, hid_report_t);

#define NR_HID_ENDPS		2
#ifdef CONFIG_HID_MAX_REPORTS
#define NR_HID_REPORTS		CONFIG_HID_MAX_REPORTS
#else
#define NR_HID_REPORTS		1
#endif
#define HID_ENDP_INTERVAL_INTERRUPT	10

#define HID_ENDP_IN	0
#define HID_ENDP_OUT	1

#define INVALID_HID_RID		NR_HID_REPORTS
#define HID_REPORTID_ALL	0
#define HID_DURATION_NEVER	0

#define HID_REPORTID(rid)	(rid+1)
#define HID_REPORTRID(id)	(id-1)

#define HID_REPORT_INPUT	0x01
#define HID_REPORT_OUTPUT	0x02
#define HID_REPORT_FEATURE	0x03

hid_rid_t hid_register_report(uint8_t duration,
			      hid_report_t *creport);
boolean hid_report_registered(uint8_t reportid);

void hid_raise_interrupt(hid_rid_t rid);
boolean hid_interrupt_busy(hid_rid_t rid);

void hid_input_discard(void);
void hid_input_submit(void);
boolean hid_input_running(void);
boolean hid_input_pending(void);
urb_size_t hid_input_length(void);

/* IO for hid_desc items */
uint16_t hid_item_length(uint8_t format, uint8_t size);
void hid_in_head(uint8_t format, uint8_t size,
		 uint8_t type, uint8_t tag);
void hid_in_short(uint8_t format, uint8_t size,
		  uint32_t udata);

#ifdef CONFIG_HID_KBD
void hid_kbd_init(void);
#else
#define hid_kbd_init()
#endif

#endif /* __USB_HID_H_INCLUDE__ */
