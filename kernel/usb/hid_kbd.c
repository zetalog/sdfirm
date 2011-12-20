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
 * @(#)hid_kbd.c: HID keyboard report functions
 * $Id: hid_kbd.c,v 1.18 2011-02-10 07:26:56 zhenglv Exp $
 */

#include <target/usb_hid.h>
#include <target/kbd.h>

#define HID_KBD_STATE_NONE		0
#define HID_KBD_STATE_DOWN		0x8000
#define HID_KBD_STATE_UP		0x4000
#define HID_KBD_STATE_BUSY		0x2000

#define HID_KBD_INPUT_CTRL_BITS		8
#define HID_KBD_INPUT_CTRL_BYTES	HID_KBD_INPUT_CTRL_BITS/8
#define HID_KBD_INPUT_PAD_BITS		8
#define HID_KBD_INPUT_PAD_BYTES		HID_KBD_INPUT_PAD_BITS/8
#define HID_KBD_INPUT_KEYS_BYTES	6
#define HID_KBD_INPUT_BYTES		(HID_KBD_INPUT_CTRL_BYTES +	\
					 HID_KBD_INPUT_PAD_BYTES +	\
					 HID_KBD_INPUT_KEYS_BYTES)
#define HID_KBD_OUTPUT_BYTES		1

static void hid_kbd_capture(uint8_t scancode, uint8_t event);

#if NR_KEYS == 16
static text_byte_t kbd_keyboard_ascii[NR_KEYS] = {
	0x1B,	0x30,	0x0D,	'+',
	0x31,	0x32,	0x33,	'-',
	0x34,	0x35,	0x36,	'*',
	0x37,	0x38,	0x39,	'%',
};

static text_byte_t kbd_keyboard_ctrls[NR_KEYS] = {
	0, 0, 0, 0x20,
	0, 0, 0, 0,
	0, 0, 0, 0x20,
	0, 0, 0, 0,
};

static text_byte_t kbd_keyboard_keys[NR_KEYS] = {
	HID_KEYBOARD_ESCAPE, HID_KEYBOARD_0, HID_KEYBOARD_ENTER, HID_KEYBOARD_PLUS,
	HID_KEYBOARD_1,      HID_KEYBOARD_2, HID_KEYBOARD_3,     HID_KEYBOARD_MINUS,
	HID_KEYBOARD_4,      HID_KEYBOARD_5, HID_KEYBOARD_6,     HID_KEYBOARD_MULTIPLY,
	HID_KEYBOARD_7,      HID_KEYBOARD_8, HID_KEYBOARD_9,     HID_KEYBOARD_DIVIDE,
};
#endif

#if NR_KEYS == 12
static text_byte_t kbd_keyboard_ascii[NR_KEYS] = {
	0x1B,	0x30,	0x0D,
	0x31,	0x32,	0x33,
	0x34,	0x35,	0x36,
	0x37,	0x38,	0x39,
};

static text_byte_t kbd_keyboard_ctrls[NR_KEYS] = {
	0, 0, 0,
	0, 0, 0,
	0, 0, 0,
	0, 0, 0,
};

static text_byte_t kbd_keyboard_keys[NR_KEYS] = {
	HID_KEYBOARD_ESCAPE, HID_KEYBOARD_0, HID_KEYBOARD_ENTER,
	HID_KEYBOARD_1,      HID_KEYBOARD_2, HID_KEYBOARD_3,
	HID_KEYBOARD_4,      HID_KEYBOARD_5, HID_KEYBOARD_6,
	HID_KEYBOARD_7,      HID_KEYBOARD_8, HID_KEYBOARD_9,
};
#endif

#if NR_KEYS == 2
static text_byte_t kbd_keyboard_ascii[NR_KEYS] = {
	0x1B,	0x0D,
};

static text_byte_t kbd_keyboard_ctrls[NR_KEYS] = {
	0, 0,
};

static text_byte_t kbd_keyboard_keys[NR_KEYS] = {
	HID_KEYBOARD_ESCAPE, HID_KEYBOARD_ENTER,
};
#endif

hid_rid_t hid_kbd_rid = 0;
uint16_t hid_kbd_last = HID_KBD_STATE_NONE;

hid_item_t hid_kbd_report_desc[] = {
	HID_UsagePage(HID_UP_GENDESK),
	HID_Usage(HID_GD_KEYBOARD),
	HID_Collection(HID_COLLECTION_APPLICATION),
	HID_UsagePage(HID_UP_KEYBOARD),
	HID_UsageMinimum(HID_KEYSTATE_BEGIN),
	HID_UsageMaximum(HID_KEYSTATE_END),
	HID_LogicalMinimum(0),
	HID_LogicalMaximum(1),
	HID_ReportSize(1),
	HID_ReportCount(HID_KBD_INPUT_CTRL_BITS),
	HID_Input(HID_MAIN_ITEM_DATA | HID_MAIN_ITEM_VARIABLE | HID_MAIN_ITEM_ABSOLUTE),
	HID_ReportCount(1),
	HID_ReportSize(HID_KBD_INPUT_PAD_BITS),
	HID_Input(HID_MAIN_ITEM_CONSTANT),
	HID_ReportCount(5),
	HID_ReportSize(1),
	HID_UsagePage(HID_UP_LED),
	HID_UsageMinimum(1),
	HID_UsageMaximum(5),
	HID_Output(HID_MAIN_ITEM_DATA | HID_MAIN_ITEM_VARIABLE | HID_MAIN_ITEM_ABSOLUTE),
	HID_ReportCount(1),
	HID_ReportSize(3),
	HID_Output(HID_MAIN_ITEM_CONSTANT),
	HID_ReportCount(HID_KBD_INPUT_KEYS_BYTES),
	HID_ReportSize(8),
	HID_LogicalMinimum(0),
	HID_LogicalMaximum(101),
	HID_UsagePage(HID_UP_KEYBOARD),
	HID_UsageMinimum(0),
	HID_UsageMaximum(101),
	HID_Input(HID_MAIN_ITEM_DATA | HID_MAIN_ITEM_ARRAY),
	HID_EndCollection(),
};

static uint8_t hid_kbd_key_ctrls(uint8_t key_code)
{
	uint8_t i;
	for (i = 0; i < NR_KEYS; i++) {
		if (kbd_keyboard_ascii[i] == key_code)
			return kbd_keyboard_ctrls[i];
	}
	return 0;
}

static uint8_t hid_kbd_key_code(uint8_t key_code)
{
	uint8_t i;
	for (i = 0; i < NR_KEYS; i++) {
		if (kbd_keyboard_ascii[i] == key_code)
			return kbd_keyboard_keys[i];
	}
	return 0;
}

void hid_kbd_submit(void)
{
	hid_raise_interrupt(hid_kbd_rid);
}

void hid_kbd_ctrl_data(void)
{
	uint8_t i, nr_items = sizeof (hid_kbd_report_desc) / sizeof (hid_item_t);

	for (i = 0; i < nr_items; i++) {
		hid_in_head(hid_kbd_report_desc[i].format,
			    hid_kbd_report_desc[i].size, hid_kbd_report_desc[i].type,
			    hid_kbd_report_desc[i].tag);
		hid_in_short(hid_kbd_report_desc[i].format,
			     hid_kbd_report_desc[i].size, hid_kbd_report_desc[i].udata);
	}
}

uint16_t hid_kbd_config_len(void)
{
	uint8_t i, nr_items = sizeof (hid_kbd_report_desc) / sizeof (hid_item_t);
	uint16_t length =0;

	for (i = 0; i < nr_items; i++) {
		length += hid_item_length(hid_kbd_report_desc[i].format,
					  hid_kbd_report_desc[i].size);
	}
	return length;
}

static void hid_kbd_irq_in(void)
{
	if (bits_raised(hid_kbd_last, HID_KBD_STATE_DOWN)) {
		/* we are sending DOWN event */
		USBD_INB(hid_kbd_key_ctrls(LOBYTE(hid_kbd_last)));
	} else {
		/* we are sending UP event */
		USBD_INB(0);
	}
	USBD_INB(0);
	if (bits_raised(hid_kbd_last, HID_KBD_STATE_DOWN)) {
		USBD_INB(hid_kbd_key_code(LOBYTE(hid_kbd_last)));
	} else {
		USBD_INB(0);
	}
	USBD_INB(0);
	USBD_INB(0);
	USBD_INB(0);
	USBD_INB(0);
	USBD_INB(0);
}

static void hid_kbd_irq_out(void)
{
	uint8_t hid_kbd_leds = 0;
	USBD_OUTB(hid_kbd_leds);
}

static void hid_kbd_irq_iocb(uint8_t endp)
{
	if (endp == HID_ENDP_IN) {
		hid_kbd_irq_in();
	}
	if (endp == HID_ENDP_OUT) {
		hid_kbd_irq_out();
	}
}

static void hid_kbd_irq_poll(uint8_t endp)
{
	if (endp == HID_ENDP_IN) {
		if (bits_raised_any(hid_kbd_last, HID_KBD_STATE_DOWN)) {
			hid_kbd_submit();
		} else if (bits_raised_any(hid_kbd_last, HID_KBD_STATE_UP)) {
			if (!bits_raised_any(hid_kbd_last, HID_KBD_STATE_DOWN)) {
				hid_kbd_submit();
			}
		}
	}
	if (endp == HID_ENDP_OUT) {
		/* NYI */
	}
}

static void hid_kbd_irq_done(uint8_t endp)
{
	if (endp == HID_ENDP_IN) {
		if (bits_raised_any(hid_kbd_last, HID_KBD_STATE_DOWN)) {
			unraise_bits(hid_kbd_last, HID_KBD_STATE_DOWN);
		} else if (bits_raised_any(hid_kbd_last, HID_KBD_STATE_UP)) {
			if (!bits_raised_any(hid_kbd_last, HID_KBD_STATE_DOWN)) {
				unraise_bits(hid_kbd_last, HID_KBD_STATE_UP | HID_KBD_STATE_BUSY);
			}
		}
	}
	if (endp == HID_ENDP_OUT) {
		/* NYI */
	}
}

void hid_kbd_key_event(uint8_t key_code, uint16_t key_scan)
{
	if (key_scan == HID_KBD_STATE_DOWN) {
		/* report is busy? ignore current DOWN! */
		if (!hid_interrupt_busy(hid_kbd_rid) &&
		    !bits_raised(hid_kbd_last, HID_KBD_STATE_BUSY)) {
			hid_kbd_last = key_code;
			raise_bits(hid_kbd_last, HID_KBD_STATE_BUSY | HID_KBD_STATE_DOWN);
		}
	} else {
		/* DOWN code isn't equal? ignore current UP! */
		if (LOBYTE(hid_kbd_last) == key_code &&
		    bits_raised(hid_kbd_last, HID_KBD_STATE_BUSY)) {
			raise_bits(hid_kbd_last, HID_KBD_STATE_UP);
		}
	}
}

static void hid_kbd_capture(uint8_t scancode, uint8_t event)
{
	uint8_t ascii = kbd_translate_key(scancode);
	if (event == KBD_EVENT_KEY_DOWN) {
		dbg_dump(ascii);
		hid_kbd_key_event(ascii, HID_KBD_STATE_DOWN);
	}
	if (event == KBD_EVENT_KEY_UP) {
		hid_kbd_key_event(ascii, HID_KBD_STATE_UP);
	}
}

hid_report_t hid_kbd_report = {
	HID_KBD_INPUT_BYTES,
	HID_KBD_OUTPUT_BYTES,
	hid_kbd_irq_poll,
	hid_kbd_irq_iocb,
	hid_kbd_irq_done,
	hid_kbd_config_len,
	hid_kbd_ctrl_data,
};

void hid_kbd_init(void)
{
	hid_kbd_rid = hid_register_report(HID_DURATION_NEVER,
					  &hid_kbd_report);
	(void)kbd_set_capture(hid_kbd_capture, 0);
}
