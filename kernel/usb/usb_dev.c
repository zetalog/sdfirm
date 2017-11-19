/*
 * ZETALOG's Personal COPYRIGHT
 *
 * Copyright (c) 2009 - 2011
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
 * @(#)usb_dev.c: universal serial bus functions
 * $Id: usb_dev.c,v 1.674 2011-10-28 03:30:54 zhenglv Exp $
 */
/* TODO:
 * 1. SET_DESCRIPTOR support
 * 2. device remote wakeup support
 * 3. isochronous transfer support
 * 4. device test mode support
 */

#include <target/usb.h>
#include <target/delay.h> /* usbd_restart sequencing */
#include <target/bh.h>

/* Standard device requests handlers. */
static void usbd_get_status(void);
#define usbd_clear_feature()
#define usbd_set_feature()
static void usbd_set_address(void);
static void usbd_get_descriptor(void);
#define usbd_set_descriptor()	usbd_endpoint_halt()
static void usbd_get_configuration(void);
static void usbd_set_configuration(void);
/* Interface specific device requests handlers. */
static void usb_interface_ctrl_data(usb_iid_t iid);

static void usbd_endpoint_get_status(uint8_t addr);
static void usbd_endpoint_clear_feature(uint8_t addr);
static void usbd_endpoint_set_feature(uint8_t addr);

static void usbd_handle_ctrl_poll(void);
static void usbd_handle_ctrl_iocb(void);
static void usbd_handle_ctrl_done(void);

/* type = STANDARD, recp = DEVICE */
static void usbd_handle_device_standard(void);
/* type = STANDARD, recp = INTERFACE */
static void usbd_handle_interface_standard(void);
/* type = STANDARD, recp = ENDPOINT */
static void usbd_handle_endpoint_standard(uint8_t addr);
/* recp = ENDPOINT */
static void usbd_handle_endpoint_request(uint8_t addr);
/* recp = INTERFACE */
static void usbd_handle_interface_request(void);

static void __usbd_request_init(void);
static void __usbd_request_exit(void);
static void usbd_request_exit(void);
static void usbd_request_open(void);
static void usbd_request_close(void);
static void usbd_request_set_soft(void);
static void usbd_request_clear_soft(void);
static void usbd_request_set_hard(void);
static void usbd_request_clear_hard(void);
static void usbd_request_set_async(void);
static void usbd_request_clear_async(void);
/* asynchronous emulation is enabled */
static boolean usbd_request_asyncing(void);

#ifdef CONFIG_USBD_POLL_NOTIMER
static void usbd_request_poll_all(void);
#else
#define usbd_request_poll_all()
#endif
static void usbd_endpoint_start_all(void);

static void usbd_transfer_iocb(void);
static void usbd_transfer_limit(void);
static boolean usbd_transfer_partial(void);
static void usbd_transfer_reset(void);

__near__ struct usb_ctlsetup usbd_ctrl_setup;

uint8_t usbd_endp = INVALID_USBD_EID;

/* USB configuration attributes */
__near__ uint8_t usbd_nr_intfs = 0;

/* USB registrations */
usbd_interface_t * __near__ usbd_interfaces[NR_USBD_INTFS];

/* USB device attributes */
__near__ usb_addr_t usbd_address = USB_ADDR_DEFAULT;
__near__ uint8_t usbd_config = USB_CONF_DEFAULT;
__near__ uint8_t usbd_state_resume;
__near__ uint8_t usbd_state;

#ifdef CONFIG_USBD_ENDP_DUPLEX
#define usbd_endp_func_by_addr(addr)	(usbd_endpoints[USB_ADDR2DIR(addr)][USB_ADDR2EID(addr)])
#define usbd_endp_ctrl_by_addr(addr)	(usbd_endp_ctrls[USB_ADDR2DIR(addr)][USB_ADDR2EID(addr)])
#define usbd_endp_xfr_by_addr(addr)	(usb_transfers[USB_ADDR2DIR(addr)][USB_ADDR2EID(addr)])

#define usbd_duplex_start_addr(eid)					\
	do {								\
		usbd_endpoint_start_addr(USB_ADDR(USB_DIR_OUT, eid));	\
		usbd_endpoint_start_addr(USB_ADDR(USB_DIR_IN, eid));	\
	} while (0)
#define usbd_duplex_async_addr(eid)					\
	do {								\
		usbd_async_iocb_addr(USB_ADDR(USB_DIR_OUT, eid));	\
		usbd_async_iocb_addr(USB_ADDR(USB_DIR_IN, eid));	\
	} while (0)
#define usbd_duplex_poll_addr(eid)					\
	do {								\
		usbd_endpoint_poll_addr(USB_ADDR(USB_DIR_OUT, eid));	\
		usbd_endpoint_poll_addr(USB_ADDR(USB_DIR_IN, eid));	\
	} while (0)
#define usbd_duplex_poll2_addr(eid)					\
	do {								\
		usbd_request_poll_addr(USB_ADDR(USB_DIR_OUT, eid));	\
		usbd_request_poll_addr(USB_ADDR(USB_DIR_IN, eid));	\
	} while (0)

#define USBD_ADDR2BITMAP(addr)		(USB_ADDR2EID(addr)+(USB_ADDR2DIR(addr)?NR_USBD_ENDPS:0))
#define __usbd_endp_reg_test(addr)	test_bit(USBD_ADDR2BITMAP(addr), usbd_endp_regs)
#define __usbd_endp_reg_set(addr)	set_bit(USBD_ADDR2BITMAP(addr), usbd_endp_regs)
#define __usbd_endp_reg_clear(addr)	clear_bit(USBD_ADDR2BITMAP(addr), usbd_endp_regs)
#define __usbd_endp_zlp_test(addr)	test_bit(USBD_ADDR2BITMAP(addr), usbd_endp_zlps)
#define __usbd_endp_zlp_set(addr)	set_bit(USBD_ADDR2BITMAP(addr), usbd_endp_zlps)
#define __usbd_endp_zlp_clear(addr)	clear_bit(USBD_ADDR2BITMAP(addr), usbd_endp_zlps)

__near__ struct usb_transfer usb_transfers[USB_MAX_DIRS][NR_USBD_ENDPS];
__near__ struct usbd_endp_ctrl usbd_endp_ctrls[USB_MAX_DIRS][NR_USBD_ENDPS];
usbd_endpoint_t * __near__ usbd_endpoints[USB_MAX_DIRS][NR_USBD_ENDPS];
DECLARE_BITMAP(usbd_endp_regs, USB_MAX_DIRS*NR_USBD_ENDPS);
DECLARE_BITMAP(usbd_endp_zlps, USB_MAX_DIRS*NR_USBD_ENDPS);
#else
#define usbd_endp_func_by_addr(addr)	(usbd_endpoints[USB_ADDR2EID(addr)])
#define usbd_endp_ctrl_by_addr(addr)	(usbd_endp_ctrls[USB_ADDR2EID(addr)])
#define usbd_endp_xfr_by_addr(addr)	(usb_transfers[USB_ADDR2EID(addr)])

#define usbd_duplex_start_addr(eid)					\
	usbd_endpoint_start_addr(USB_ADDR(USB_DIR_OUT, eid))
#define usbd_duplex_async_addr(eid)					\
	usbd_async_iocb_addr(USB_ADDR(USB_DIR_OUT, eid))
#define usbd_duplex_poll_addr(eid)					\
	usbd_endpoint_poll_addr(USB_ADDR(USB_DIR_OUT, eid))
#define usbd_duplex_poll2_addr(eid)					\
	usbd_request_poll_addr(USB_ADDR(USB_DIR_OUT, eid));

#define __usbd_endp_reg_test(addr)	test_bit(USB_ADDR2EID(addr), usbd_endp_regs)
#define __usbd_endp_reg_set(addr)	set_bit(USB_ADDR2EID(addr), usbd_endp_regs)
#define __usbd_endp_reg_clear(addr)	clear_bit(USB_ADDR2EID(addr), usbd_endp_regs)
#define __usbd_endp_zlp_test(addr)	test_bit(USB_ADDR2EID(addr), usbd_endp_zlps)
#define __usbd_endp_zlp_set(addr)	set_bit(USB_ADDR2EID(addr), usbd_endp_zlps)
#define __usbd_endp_zlp_clear(addr)	clear_bit(USB_ADDR2EID(addr), usbd_endp_zlps)

__near__ struct usb_transfer usb_transfers[NR_USBD_ENDPS];
__near__ struct usbd_endp_ctrl usbd_endp_ctrls[NR_USBD_ENDPS];
usbd_endpoint_t * __near__ usbd_endpoints[NR_USBD_ENDPS];
DECLARE_BITMAP(usbd_endp_regs, NR_USBD_ENDPS);
DECLARE_BITMAP(usbd_endp_zlps, NR_USBD_ENDPS);
#endif

#define usbd_endp_func_by_save()	usbd_endp_func_by_addr(usbd_endp)
#define usbd_endp_ctrl_by_save()	usbd_endp_ctrl_by_addr(usbd_endp)
#define usbd_endp_xfr_by_save()		usbd_endp_xfr_by_addr(usbd_endp)

#ifdef CONFIG_DEBUG_PRINT
boolean usbd_dbg = false;

void usbd_dump_restore(boolean dbg)
{
	usbd_dbg = dbg;
}

boolean usbd_dump_save(boolean dbg)
{
	boolean odbg = usbd_dbg;
	usbd_dump_restore(dbg);
	return odbg;
}

static inline void usbd_debug_dump(uint8_t byte)
{
	if (usbd_dbg) dbg_dump(byte);
}
#else
#define usbd_debug_dump(byte)
#endif

uint8_t usbd_saved_addr(void)
{
	return USB_SAVEADDR(usbd_endp);
}

void usbd_addr_restore(uint8_t addr)
{
	usbd_endp = addr;
	usbd_hw_endp_select(addr);
}

uint8_t usbd_addr_save(uint8_t addr)
{
	uint8_t saddr = usbd_endp;
	usbd_addr_restore(addr);
	return saddr;
}

usb_iid_t usbd_declare_interface(usbd_interface_t *cintf)
{
	usb_iid_t iid = usbd_nr_intfs;
	if (iid < NR_USBD_INTFS) {
		usbd_interfaces[iid] = cintf;
		usbd_nr_intfs++;
	}
	return iid;
}

#ifdef CONFIG_USB_USBIP_DEV
uint8_t usbd_interface_get_attr(uint8_t iid, uint8_t type)
{
	usbd_interface_t *cintf;
	cintf = usbd_interfaces[iid];
	BUG_ON(!cintf->attr);
	return cintf->attr(type);
}
#endif

static void usbd_register_endpoint(uint8_t addr,
				   boolean explicit_zlp,
				   usbd_endpoint_t *cendp)
{
	usbd_endp_func_by_addr(addr) = cendp;
	__usbd_endp_reg_set(addr);
	if (explicit_zlp)
		__usbd_endp_zlp_set(addr);

	/* Take care of not to access USB hardware specific stuff here
	 * as the controller may not be powered at this time, access to
	 * its registers may result in bus errors on PM capable chips. 
	 */
}

static void usbd_register_control(uint8_t eid, usbd_endpoint_t *cendp)
{
	/* Control endpoints are duplex endpoints. */
	usbd_register_endpoint(USB_ADDR(USB_DIR_OUT, eid), true, cendp);
	usbd_register_endpoint(USB_ADDR(USB_DIR_IN, eid), true, cendp);
}

uint8_t usbd_claim_endpoint(boolean explicit_zlp,
			    usbd_endpoint_t *cendp)
{
	uint8_t eid;
	uint8_t dir = USBD_ATTR2DIR(cendp->attrs);
	uint8_t type = USBD_ATTR2TYPE(cendp->attrs);

	for (eid = USB_EID_DEFAULT+1; eid < NR_USBD_ENDPS; eid++) {
		uint8_t addr = USB_ENDPADDR(dir, eid);
		if (usbd_hw_endp_caps(addr, type)) {
			if (type == USB_ENDP_CONTROL) {
				if (!usbd_endpoint_claimed(USB_ADDR(USB_DIR_OUT, eid)) &&
				    !usbd_endpoint_claimed(USB_ADDR(USB_DIR_IN, eid))) {
					usbd_register_control(eid, cendp);
					return USB_ADDR(dir, eid);
				}
			} else {
				if (!usbd_endpoint_claimed(addr)) {
					usbd_register_endpoint(addr, explicit_zlp, cendp);
					return USB_ADDR(dir, eid);
				}
			}
		}
	}
	BUG();
	return INVALID_USBD_EID;
}

usbd_endpoint_t usbd_ctrl = {
	USBD_ENDP_CTRL,
	0,
	usbd_handle_ctrl_poll,
	usbd_handle_ctrl_iocb,
	usbd_handle_ctrl_done,
};

static void usbd_reserve_endpoint0(void)
{
	usbd_register_control(USB_EID_DEFAULT, &usbd_ctrl);
}

boolean usbd_endpoint_claimed(uint8_t addr)
{
	return __usbd_endp_reg_test(addr);
}

uint8_t usbd_endpoint_attrs_addr(uint8_t addr)
{
	usbd_endpoint_t *cendp = usbd_endp_func_by_addr(addr);
	return cendp->attrs;
}

uint8_t usbd_endpoint_interval_addr(uint8_t addr)
{
	usbd_endpoint_t *cendp = usbd_endp_func_by_addr(addr);
	return cendp->interval;
}

uint8_t usbd_endpoint_type_addr(uint8_t addr)
{
	if (USB_ADDR2EID(addr) < NR_USBD_ENDPS)
		return USBD_ATTR2TYPE(usbd_endpoint_attrs_addr(addr));
	return USB_ENDP_INVALID;
}

uint8_t usbd_endpoint_dir_addr(uint8_t addr)
{
	if (USB_ADDR2EID(addr) < NR_USBD_ENDPS)
		return USBD_ATTR2DIR(usbd_endpoint_attrs_addr(addr));
	return USB_DIR_OUT;
}

uint8_t usbd_control_setup_dir(void)
{
	if (USB_ADDR2EID(usbd_endp) < NR_USBD_ENDPS)
		return USB_SETUP2DIR(usbd_ctrl_setup.bmRequestType);
	return USB_DIR_OUT;
}

uint8_t usbd_control_request_addr(void)
{
	return LOBYTE(usbd_control_request_index());
}

#if 0
uint8_t usbd_control_request_iid(void)
{
	return LOBYTE(usbd_control_request_index());
}
#endif

uint8_t usbd_control_request_type(void)
{
	return (usbd_ctrl_setup.bRequest);
}

uint16_t usbd_control_request_value(void)
{
	return (usbd_ctrl_setup.wValue);
}

uint16_t usbd_control_request_index(void)
{
	return (usbd_ctrl_setup.wIndex);
}

uint16_t usbd_control_request_length(void)
{
	return (usbd_ctrl_setup.wLength);
}

uint8_t usbd_control_setup_type(void)
{
	return USB_SETUP2TYPE(usbd_ctrl_setup.bmRequestType);
}

uint8_t usbd_control_setup_recp(void)
{
	return USB_SETUP2RECP(usbd_ctrl_setup.bmRequestType);
}

uint8_t usbd_state_get(void)
{
	return usbd_state;
}

void usbd_state_set(uint8_t state)
{
	usbd_debug_state(state);
	usbd_state = state;
}

/*=========================================================================
 * request/transfer
 *=======================================================================*/
urb_size_t usbd_request_handled(void)
{
	return usbd_endp_xfr_by_save().req_cur;
}

urb_size_t usbd_request_unhandled(void)
{
	return usbd_endp_xfr_by_save().req_all -
	       usbd_endp_xfr_by_save().req_cur;
}

utb_size_t usbd_transfer_unhandled(void)
{
	return usbd_endp_xfr_by_save().xfr_all -
	       usbd_transfer_handled();
}

utb_size_t usbd_transfer_handled(void)
{
	return usbd_endp_xfr_by_save().xfr_cur;
}

void usbd_request_reap(void)
{
	uint8_t reap = 0;

	if (usbd_request_dir() != USB_DIR_OUT)
		return;

	while (usbd_transfer_unhandled() > 0) {
		USBD_OUTB(reap);
	}
}

uint8_t usbd_request_dir(void)
{
	if (usbd_endpoint_type() == USB_ENDP_CONTROL) {
		switch (usbd_control_get_stage()) {
		case USBD_CTRL_STAGE_SETUP:
			return USB_DIR_OUT;
		case USBD_CTRL_STAGE_DATA:
			return usbd_control_setup_dir();
		case USBD_CTRL_STAGE_STATUS:
			return usbd_control_setup_dir() == USB_DIR_OUT ?
			       USB_DIR_IN : USB_DIR_OUT;
		}
	} else {
		return usbd_endpoint_dir();
	}
	return USB_DIR_OUT;
}

/* XXX: No Timer Polling
 *
 * Can NAK IRQs or SOF IRQ be used for calling this function?  So that the
 * timer of the USB device can be eliminated from the core USB device
 * stack.
 */
void usbd_request_poll(void)
{
	usbd_endpoint_t *cendp = usbd_endp_func_by_save();
	BUG_ON(!cendp->poll);
	usbd_debug_call(USBD_CALL_POLL);
	cendp->poll();
}

void usbd_transfer_reset(void)
{
	usbd_endp_xfr_by_save().xfr_cur = 0;
	usbd_transfer_limit();
	usbd_hw_transfer_open();
}

void usbd_request_reset(void)
{
	unraise_bits(usbd_endp_ctrl_by_save().ctrls, USBD_REQ_FLAG_MASK);
	usbd_endp_xfr_by_save().req_all = 0;
	usbd_endp_xfr_by_save().req_cur = 0;
	usbd_endp_xfr_by_save().xfr_all = 0;
	usbd_endp_xfr_by_save().xfr_cur = 0;
	usbd_request_poll();
}

void usbd_transfer_limit(void)
{
	if (usbd_request_dir() == USB_DIR_IN) {
		usbd_endp_xfr_by_save().xfr_all = usbd_endpoint_size();
		if ((urb_size_t)(usbd_endp_xfr_by_save().xfr_all) >
		    (urb_size_t)(usbd_request_unhandled() - usbd_transfer_handled())) {
			usbd_endp_xfr_by_save().xfr_all =
				(utb_size_t)(usbd_request_unhandled() -
					     usbd_transfer_handled());
		}
	}
}

static void usbd_request_limit(urb_size_t bytes)
{
	usbd_endp_xfr_by_save().req_all = bytes;
	usbd_transfer_limit();
}

/* OUT endpoint handlers SHOULD call this as soon as they know how many
 * bytes (req_all) they will receive.
 * OUT endpoint handlers MUST call this whenever they know the old req_all
 * value acknowledged by the USB stack has been too small to keep the
 * whole OUT data.  The newer req_all value might be parsed from the
 * outgoing OUT data in the IO data handling stage.
 * Thus, the req_all variable will take a value from:
 * 1. usbd_endpoint_size if no usbd_request_commit/usbd_request_submit has
 *    been called, or
 * 2. 'length' passed by the last usbd_request_commit/usbd_request_submit
 *    call.
 *
 * NOTE: this function SHOULD be called in an endpoint's IO data handling
 *       (iocb) stage
 */
void usbd_request_commit(urb_size_t length)
{
	BUG_ON(!usbd_request_pending());
	usbd_request_limit(length);
}

/* NOTE: this function SHOULD be called in an endpoint's IO event polling
 *       (poll) stage
 */
boolean usbd_request_submit(uint8_t addr, urb_size_t length)
{
	uint8_t saddr;
	boolean result = false;

	saddr = usbd_addr_save(addr);

	/* XXX: No Future Requests on STALL
	 *
	 * If a non-isochronous pipe encounters a condition that causes it
	 * to send a STALL to the host or three bus errors are encountered
	 * on any packet of an IRP, the IRP is aborted/retired, no further
	 * IRPs are accepted.
	 */
	if (usbd_request_pending() || usbd_request_halting())
		goto end;

	/* Do not allow endpoints other than control endpoint submission
	 * before CONFIGURED.
	 */
	if (USB_ADDR2EID(usbd_endp) != USB_EID_DEFAULT &&
	    usbd_state_get() != USB_STATE_CONFIGURED)
		goto end;

	usb_debug_eid(usbd_endp);

	usbd_request_limit(length);

	usbd_request_set_soft();
	usbd_request_open();
	result = true;
end:
	usbd_addr_restore(saddr);
	return result;
}

void usbd_request_discard(void)
{
	if (usbd_request_pending())
		usbd_request_commit(usbd_endp_xfr_by_save().req_cur);
}

void usbd_request_discard_addr(uint8_t addr)
{
	uint8_t saddr;
	saddr = usbd_addr_save(addr);
	usbd_request_discard();
	usbd_addr_restore(saddr);
}

/* XXX: Edge/Level Triggered Transmission IRQ (TXCMPL v.s. TXAVAL)
 *
 * This USB device stack would choose TXAVAL interrupts as default
 * behaviour of the lower USB device controller.  These IRQ are level
 * triggered IRQ interrupting the CPU as long as the transmission FIFO is
 * empty.  Since these interrupts would always be asserted whenever the
 * FIFOs are not filled with requests, care should be taken on handling
 * such interrupts: when no requests are pending to be transmitted on this
 * endpoint, TXAVAL interrupts should be masked.  This could be done
 * through usbd_hw_request_open/usbd_hw_request_close driver elements.
 * TXAVAL interrupts are the faster way for the USB device stack, since no
 * additional task context handling would be required for the
 * transmission, thus this is the recommended way for the USB device
 * stack.
 * But there are many USB device controllers using TXCMPL interrupts to
 * inform software the transmission completion, as these interrupts would
 * only be asserted once on the transmission completion, we call it
 * "Edge Triggered Transmission Completion IRQ" (TXCMPL).  These kind of
 * controllers require additional task context handling if stack memory is
 * not big enough to handle the recursive calls of the usbd_transfer_iocb.
 */
#ifdef CONFIG_USBD_XFR_ASYNC_TXAVAL
static void usbd_request_begin(void)
{
	/* XXX: Stack Tuning on TXAVAL Emulation
	 *
	 * Stack comsuption is too large to call usbd_transfer_iocb
	 * directly.  Thus, the first usb IN transfer is issued
	 * asynchronously here if TXCMPL is edge triggered as a completion
	 * indication.
	 */
	if (usbd_request_dir() == USB_DIR_IN) {
		usbd_request_set_async();
	}
}
#else
#define usbd_request_begin()
#endif

void usbd_inb(uint8_t byte)
{
	if (usbd_request_syncing()) {
		usbd_inb_sync(byte);
	} else {
		usbd_inb_async(byte);
	}
}

void usbd_inw(uint16_t word)
{
	usbd_inb(LOBYTE(word));
	usbd_inb(HIBYTE(word));
}

void usbd_inl(uint32_t dword)
{
	usbd_inw(LOWORD(dword));
	usbd_inw(HIWORD(dword));
}

uint8_t usbd_outb(uint8_t byte)
{
	if (usbd_request_syncing()) {
		return usbd_outb_sync();
	} else {
		return usbd_outb_async(byte);
	}
}

uint16_t usbd_outw(uint16_t word)
{
	uint8_t lo = LOBYTE(word);
	uint8_t hi = HIBYTE(word);
	lo = usbd_outb(lo);
	hi = usbd_outb(hi);
	return lo | ((uint16_t)hi << 8);
}

uint32_t usbd_outl(uint32_t dword)
{
	uint16_t lo = LOWORD(dword);
	uint16_t hi = HIWORD(dword);
	lo = usbd_outw(lo);
	hi = usbd_outw(hi);
	return lo | ((uint32_t)hi << 16);
}

#ifdef CONFIG_USBD_XFR_SYNC
void usbd_inb_sync(uint8_t byte)
{
	if (usbd_transfer_handled() == usbd_endpoint_size())
		usbd_transfer_reset();

	if (usbd_request_unhandled() > 0 &&
	    usbd_endp_xfr_by_save().req_cur == usbd_endp_xfr_by_save().iter) {
		usbd_hw_write_byte(byte);
		usbd_debug_dump(byte);
		usbd_endp_xfr_by_save().xfr_cur++;
		usbd_endp_xfr_by_save().req_cur++;

		if (usbd_transfer_handled() == usbd_endpoint_size()) {
			usbd_hw_transfer_close();
		}
	}

	usbd_endp_xfr_by_save().iter++;
}

void usbd_inw_sync(uint16_t word)
{
	usbd_inb_sync(LOBYTE(word));
	usbd_inb_sync(HIBYTE(word));
}

void usbd_inl_sync(uint32_t dword)
{
	usbd_inw_sync(LOWORD(dword));
	usbd_inw_sync(HIWORD(dword));
}

uint8_t usbd_outb_sync(void)
{
	uint8_t newval = 0;

	if (usbd_transfer_handled() == usbd_endpoint_size())
		usbd_transfer_reset();

	if (usbd_request_unhandled() > 0 &&
	    usbd_endp_xfr_by_save().req_cur == usbd_endp_xfr_by_save().iter) {
		newval = usbd_hw_read_byte();
		usbd_debug_dump(newval);
		usbd_endp_xfr_by_save().xfr_cur++;
		usbd_endp_xfr_by_save().req_cur++;

		if (usbd_transfer_handled() == usbd_endpoint_size()) {
			usbd_hw_transfer_close();
		}
	}

	usbd_endp_xfr_by_save().iter++;
	return newval;
}

uint16_t usbd_outw_sync(void)
{
	uint8_t lo;
	uint8_t hi;
	lo = usbd_outb_sync();
	hi = usbd_outb_sync();
	return lo | ((uint16_t)hi << 8);
}

uint32_t usbd_outl_sync(void)
{
	uint16_t lo;
	uint16_t hi;
	lo = usbd_outw_sync();
	hi = usbd_outw_sync();
	return lo | ((uint32_t)hi << 16);
}

#ifdef CONFIG_USBD_BULK
void usbd_bulk_open(size_t offset)
{
	usbd_transfer_reset();
}

void usbd_bulk_close(bulk_size_t size)
{
	usbd_endp_xfr_by_save().req_cur += size;
	usbd_endp_xfr_by_save().xfr_cur += size;
	if (size == usbd_endpoint_size()) {
		usbd_hw_transfer_close();
	}
}

void usbd_bulk_put(uint8_t byte)
{
	usbd_hw_write_byte(byte);
}

uint8_t usbd_bulk_get(void)
{
	return usbd_hw_read_byte();
}

size_t usbd_bulk_in(uint8_t *buffer, bulk_size_t size)
{
	urb_size_t i = 0, len_hw, off_hw;
	utb_size_t esize = usbd_endpoint_size();
	utb_size_t len_sw;

	while (size > i) {
		if (usbd_transfer_handled() == esize) {
			usbd_transfer_reset();
		}

		len_sw = (utb_size_t)min(size - i, (urb_size_t)esize);
		off_hw = len_sw -
			 (utb_size_t)min((urb_size_t)len_sw,
					 usbd_endp_xfr_by_save().req_cur -
					 usbd_endp_xfr_by_save().iter);
		len_hw = min(usbd_request_unhandled(), off_hw);

		usbd_endp_xfr_by_save().xfr_cur += (utb_size_t)len_hw;
		usbd_endp_xfr_by_save().req_cur += (utb_size_t)len_hw;
		usbd_endp_xfr_by_save().iter += len_sw;
		i += len_sw;

		while (len_hw) {
			usbd_hw_write_byte(buffer[i-off_hw]);
			len_hw--, off_hw--;
		}

		if (usbd_transfer_handled() == esize) {
			usbd_hw_transfer_close();
		}
	}

	return i;
}

size_t usbd_bulk_out(uint8_t *buffer, bulk_size_t size)
{
	urb_size_t i = 0, len_hw, off_hw;
	utb_size_t esize = usbd_endpoint_size();
	utb_size_t len_sw;

	while (size > i) {
		if (usbd_transfer_handled() == esize) {
			usbd_transfer_reset();
		}

		len_sw = (utb_size_t)min(size - i, (urb_size_t)esize);
		off_hw = len_sw -
			 (utb_size_t)min((urb_size_t)len_sw,
					 usbd_endp_xfr_by_save().req_cur -
					 usbd_endp_xfr_by_save().iter);
		len_hw = min(usbd_request_unhandled(), off_hw);

		usbd_endp_xfr_by_save().xfr_cur += (utb_size_t)len_hw;
		usbd_endp_xfr_by_save().req_cur += (utb_size_t)len_hw;
		usbd_endp_xfr_by_save().iter += len_sw;
		i += len_sw;

		while (len_hw) {
			buffer[i-off_hw] = usbd_hw_read_byte();
			len_hw--, off_hw--;
		}

		if (usbd_transfer_handled() == esize) {
			usbd_hw_transfer_close();
		}
	}

	return i;
}
#endif
#endif

#ifdef CONFIG_USBD_XFR_ASYNC
void usbd_inb_async(uint8_t byte)
{
	if (usbd_transfer_unhandled() > 0 &&
	    usbd_endp_xfr_by_save().req_cur == usbd_endp_xfr_by_save().iter) {
		usbd_hw_write_byte(byte);
		usbd_debug_dump(byte);
		usbd_endp_xfr_by_save().xfr_cur++;
		usbd_endp_xfr_by_save().req_cur++;
		if (usbd_transfer_handled() == usbd_endpoint_size()) {
			usbd_hw_transfer_close();
		}
	}
	usbd_endp_xfr_by_save().iter++;
}

void usbd_inw_async(uint16_t word)
{
	usbd_inb_async(LOBYTE(word));
	usbd_inb_async(HIBYTE(word));
}

void usbd_inl_async(uint32_t dword)
{
	usbd_inw_async(LOWORD(dword));
	usbd_inw_async(HIWORD(dword));
}

uint8_t usbd_outb_async(uint8_t byte)
{
	uint8_t newval = byte;

	if ((usbd_transfer_unhandled() > 0) &&
	    (usbd_endp_xfr_by_save().req_cur == usbd_endp_xfr_by_save().iter)) {
		newval = usbd_hw_read_byte();
		usbd_debug_dump(newval);
		usbd_endp_xfr_by_save().xfr_cur++;
		usbd_endp_xfr_by_save().req_cur++;
		if (usbd_transfer_handled() == usbd_endpoint_size()) {
			usbd_hw_transfer_close();
		}
	}
	usbd_endp_xfr_by_save().iter++;
	return newval;
}

uint16_t usbd_outw_async(uint16_t word)
{
	uint8_t lo = LOBYTE(word);
	uint8_t hi = HIBYTE(word);
	lo = usbd_outb_async(lo);
	hi = usbd_outb_async(hi);
	return lo | ((uint16_t)hi << 8);
}

uint32_t usbd_outl_async(uint32_t dword)
{
	uint16_t lo = LOWORD(dword);
	uint16_t hi = HIWORD(dword);
	lo = usbd_outw_async(lo);
	hi = usbd_outw_async(hi);
	return lo | ((uint32_t)hi << 16);
}
#endif

#ifdef CONFIG_USBD_XFR_ADAPT
void usbd_request_set_sync(void)
{
	raise_bits(usbd_endp_ctrl_by_save().ctrls, USBD_REQ_FLAG_SYNC);
}

void usbd_request_clear_sync(void)
{
	unraise_bits(usbd_endp_ctrl_by_save().ctrls, USBD_REQ_FLAG_SYNC);
}

boolean usbd_request_syncing(void)
{
	return bits_raised(usbd_endp_ctrl_by_save().ctrls, USBD_REQ_FLAG_SYNC);
}
#endif

/* XXX: Ending the IO Callbacks
 *
 * This might be the most important function in the USB device protocol
 * stack.
 * This function must take account of the HALT feature, ZLP requirements,
 * SYNC/ASYNC transfers, and the hardwre abstract layer, etc.  The
 * function has to deal with lots of such variations here.
 * If the __usbd_request_exit() call is expected to be synchronously
 * called in the fly, this is the right position to perform the tuning.
 */
void usbd_request_end(void)
{
	/* XXX: Abort Outstanding Requests on STALL
	 *
	 * If a non-isochronous pipe encounters a condition that causes it
	 * to send a STALL to the host or three bus errors are encountered
	 * on any packet of an IRP, the IRP is aborted/retired, all
	 * outstanding IRPs are also retired.
	 */
	if (usbd_endpoint_halting()) {
		/* TODO: For No STALL IRQ Hardware
		 * Hardware stuff shouldn't be advanced as the endpoint is
		 * halted.
		 * Asynchronous software event emulation are needed to
		 * issue __usbd_request_exit().
		 * This emulation may not required for those controller
		 * can raise STALL IRQ after STALL has been sent.
		 */
		usbd_request_set_async();
		return;
	}
	if (usbd_transfer_partial()) {
		/* For hardware indications:
		 * Despite of SYNC/ASYNC variations, hardwares are dealing
		 * with the partial packets need to be acknowledged here.
		 *
		 * For software indications:
		 * For OUT endpoints:
		 * The last entrance of this function might be an
		 * asynchronous software event emulation, and no hardware
		 * stuff should be done for it.
		 * For IN endpoints:
		 * The first entrance of this function might be an
		 * asynchronous software event emulation, and hardware stuff
		 * should be done for it.
		 */
		if (usbd_request_dir() == USB_DIR_IN ||
		    !usbd_request_asyncing()) {
			usbd_hw_transfer_close();
		}
	} else {
		/* For SYNC transfers:
		 * If last IO functions transmitted exactly endpoint size
		 * packet, ZLP should be appended here.
		 *
		 * For ASYNC transfers:
		 * The last IO functions will lead to reentrance of this
		 * function, ZLP can be automatically added in the
		 * previous usbd_transfer_partial() condition.
		 */
		if (usbd_request_syncing() &&
		    __usbd_endp_zlp_test(usbd_endp)) {
			usbd_transfer_reset();
			usbd_hw_transfer_close();
		}
	}
	if (usbd_transfer_last()) {
		/* For OUT endpoints:
		 * Software emulations are required to be performed here
		 * to trigger a __usbd_request_exit().
		 *
		 * For IN endpoints:
		 * Since asynchronous events were eaten by the
		 * usbd_hw_transfer_close, we need to emulate a software
		 * event here for them to trigger a __usbd_request_exit().
		 */
		if (usbd_request_dir() == USB_DIR_OUT ||
		    usbd_request_syncing()) {
			usbd_request_set_async();
		}
	}
}

void usbd_request_open(void)
{
	usbd_hw_request_open();
	usbd_request_begin();
}

void usbd_request_close(void)
{
	if (usbd_request_running()) {
		usbd_hw_request_close();
		usbd_request_clear_hard();
		if (usbd_request_pending()) {
			usbd_endpoint_t *cendp = usbd_endp_func_by_save();
			BUG_ON(!cendp->done);
			usbd_debug_call(USBD_CALL_DONE);
			cendp->done();
			usbd_request_clear_soft();
		}
	}
}

static void __usbd_transfer_data(void)
{
	usbd_endpoint_t *cendp;

	if (usbd_request_halting()) {
		/* XXX: Continuation of STALL Returning
		 *
		 * Once a function's endpoint is halted, the function
		 * must continue returning STALL until the condition
		 * causing the halt has been cleared through host
		 * intervention.
		 */
		usbd_endpoint_halt();
		return;
	}

	usbd_iter_reset();
	usbd_transfer_reset();

	cendp = usbd_endp_func_by_save();

	BUG_ON(usbd_endp_xfr_by_save().req_cur >
	       usbd_endp_xfr_by_save().req_all);
	BUG_ON(!cendp->iocb);

	usbd_debug_call(USBD_CALL_DATA);
	cendp->iocb();

	usbd_request_reap();
	usbd_request_end();
}

boolean usbd_transfer_partial(void)
{
	return usbd_transfer_handled() < usbd_endpoint_size();
}

boolean usbd_transfer_last(void)
{
	if (__usbd_endp_zlp_test(usbd_endp)) {
		return usbd_transfer_partial();
	} else {
		return usbd_request_unhandled() == 0;
	}
}

void usbd_transfer_submit(utb_size_t bytes)
{
	usbd_endp_xfr_by_save().xfr_all = bytes;
}

void usbd_transfer_txin(void)
{
	usbd_debug_eirq(USBD_IRQ_IN);
	usbd_transfer_iocb();
}

void usbd_transfer_rxout(void)
{
	usbd_debug_eirq(USBD_IRQ_OUT);
	usbd_transfer_iocb();
}

void usbd_request_set_async(void)
{
	raise_bits(usbd_endp_ctrl_by_save().ctrls, USBD_REQ_FLAG_ASYNC);
	usbd_hw_request_close();
	usb_wakeup_state();
}

void usbd_request_clear_async(void)
{
	unraise_bits(usbd_endp_ctrl_by_save().ctrls, USBD_REQ_FLAG_ASYNC);
	usbd_hw_request_open();
}

boolean usbd_request_asyncing(void)
{
	return bits_raised(usbd_endp_ctrl_by_save().ctrls, USBD_REQ_FLAG_ASYNC);
}

boolean usbd_request_interrupting(uint8_t dir)
{
	return usbd_endpoint_claimed(usbd_endp) && /* Endpoint is claimed. */
	       usbd_request_pending() && /* Request has been submitted. */
	       usbd_request_dir() == dir && /* Direction is correct. */
	       !usbd_request_asyncing(); /* And there isn't an asynchronous tuning. */
}

boolean usbd_request_polling(uint8_t dir)
{
	return usbd_endpoint_claimed(usbd_endp) && /* Endpoint is claimed. */
	       usbd_endpoint_type() != USB_ENDP_CONTROL && /* Control isn't pollable. */
	       usbd_endpoint_dir() == dir && /* Direction is correct. */
	       usbd_state_get() != USB_STATE_CONFIGURED && /* After CONFIGURED state. */
	       !usbd_request_pending(); /* Request hasn't been submitted. */
}

void usbd_iter_reset(void)
{
	usbd_endp_xfr_by_save().iter = 0;
}

void usbd_iter_accel(void)
{
	usbd_endp_xfr_by_save().iter = usbd_endp_xfr_by_save().req_cur;
}

void usbd_async_iocb(void)
{
	if (usbd_request_asyncing()) {
		usbd_request_clear_async();
		usbd_transfer_iocb();
	}
}

static void __usbd_request_init(void)
{
	usbd_request_set_hard();
}

static void __usbd_request_exit(void)
{
	usbd_request_close();
	/* XXX: Automatically FIFO Reset by Hardware
	 *
	 * Do not call usbd_hw_request_reset() here as most of the
	 * controllers must have internally reset the data toggle bit by
	 * itself to achieve the maximum throughput for error-free data
	 * requests.
	 */
	usbd_request_reset();
}

void usbd_transfer_iocb(void)
{
	if (!usbd_request_running()) {
		__usbd_request_init();
		__usbd_transfer_data();
	} else if (usbd_endpoint_halting() || usbd_transfer_last()) {
		__usbd_request_exit();
	} else {
		__usbd_transfer_data();
	}
}

void usbd_request_set_soft(void)
{
	raise_bits(usbd_endp_ctrl_by_save().ctrls, USBD_REQ_FLAG_SOFT);
}

void usbd_request_clear_soft(void)
{
	unraise_bits(usbd_endp_ctrl_by_save().ctrls, USBD_REQ_FLAG_SOFT);
	usbd_request_poll_all();
}

boolean usbd_request_pending(void)
{
	return bits_raised(usbd_endp_ctrl_by_save().ctrls, USBD_REQ_FLAG_SOFT);
}

void usbd_request_set_hard(void)
{
	raise_bits(usbd_endp_ctrl_by_save().ctrls, USBD_REQ_FLAG_HARD);
}

void usbd_request_clear_hard(void)
{
	unraise_bits(usbd_endp_ctrl_by_save().ctrls, USBD_REQ_FLAG_HARD);
}

boolean usbd_request_running(void)
{
	return bits_raised(usbd_endp_ctrl_by_save().ctrls, USBD_REQ_FLAG_HARD);
}

/* This is a functional STALL */
void usbd_endpoint_stall(void)
{
	raise_bits(usbd_endp_ctrl_by_save().ctrls, USBD_ENDP_FLAG_HALT);
}

/* This is a protocol STALL */
void usbd_endpoint_halt(void)
{
	usbd_endpoint_stall();
	usbd_hw_endp_halt();
}

void usbd_endpoint_unhalt(void)
{
	if (usbd_endpoint_halting()) {
		usbd_hw_endp_unhalt();
		/* XXX: Manually FIFO Reset by Software
		 *
		 * Call usbd_hw_request_reset() here as some controller
		 * may not implement auto data toggle reset on the end of
		 * the STALL condition.
		 */
		usbd_hw_request_reset();
		unraise_bits(usbd_endp_ctrl_by_save().ctrls, USBD_ENDP_FLAG_HALT);
	}
}

void usbd_request_stall(void)
{
	usbd_debug_eirq(USBD_IRQ_STALL);
	usbd_endpoint_stall();
	usbd_transfer_iocb();
}

boolean usbd_endpoint_halting(void)
{
	return bits_raised(usbd_endp_ctrl_by_save().ctrls, USBD_ENDP_FLAG_HALT);
}

boolean usbd_request_halting(void)
{
	switch (usbd_endpoint_type()) {
	case USB_ENDP_ISOCHRONOUS:
		return false;
	case USB_ENDP_CONTROL:
		if (usbd_control_get_stage() == USBD_CTRL_STAGE_SETUP)
			return false;
	default:
		return usbd_endpoint_halting();
	}
}

/*=========================================================================
 * control pipe
 *=======================================================================*/
void usbd_control_set_stage(uint8_t stage)
{
	usbd_debug_stage(stage);
	unraise_bits(usbd_endp_ctrl_by_save().ctrls,
		     USBD_CTRL_STAGE_MASK);
	raise_bits(usbd_endp_ctrl_by_save().ctrls, stage);
	usbd_addr_restore(USB_ADDR(usbd_request_dir(), USB_ADDR2EID(usbd_endp)));
}

uint8_t usbd_control_get_stage(void)
{
	return (usbd_endp_ctrl_by_save().ctrls & USBD_CTRL_STAGE_MASK);
}

boolean usbd_control_setup_staging(void)
{
	return (usbd_endpoint_type() == USB_ENDP_CONTROL &&
		((usbd_control_get_stage() == USBD_CTRL_STAGE_SETUP) ||
		 (usbd_control_request_length() == 0 &&
		  usbd_control_get_stage() == USBD_CTRL_STAGE_DATA)));
}

void __usbd_control_reset(void)
{
	BUG_ON(usbd_endpoint_type() != USB_ENDP_CONTROL);
	/* XXX: Recover Condition - SETUP (CONTROL)
	 *
	 * A protocol STALL is returned during the Data or Status
	 * stage of a control transfer, and the STALL condition
	 * terminates at the beginning of the next control
	 * transfer (Setup).
	 * STALL state recovers from the condition (in an
	 * implementation-dependent way) and acknowledges the halt
	 * or error condition via a USBD call.
	 */
	/* The usbd_endpoint_unhalt() call should always be called
	 * before the usbd_request_reset() call.
	 */
	usbd_endpoint_unhalt();
	if (usbd_control_get_stage() != USBD_CTRL_STAGE_SETUP) {
		usbd_control_set_stage(USBD_CTRL_STAGE_SETUP);
		usbd_request_reset();
	}
}

void usbd_control_reset(void)
{
	usbd_debug_eirq(USBD_IRQ_SETUP);
	__usbd_control_reset();
}

void usbd_control_setup(void)
{
	usbd_control_reset();
	usbd_transfer_iocb();
}

static void usbd_handle_ctrl_poll(void)
{
	switch (usbd_control_get_stage()) {
	case USBD_CTRL_STAGE_SETUP:
		usbd_request_submit(usbd_endp,
				    USB_SETUP_REQUEST_SIZE);
		break;
	case USBD_CTRL_STAGE_DATA:
		usbd_request_submit(usbd_endp,
				    usbd_control_request_length());
		break;
	case USBD_CTRL_STAGE_STATUS:
		usbd_request_submit(usbd_endp, 0);
		break;
	}
}

static void usbd_handle_ctrl_data(void)
{
	uint8_t type;
	uint8_t addr;

	type = usbd_control_setup_type();
	switch (usbd_control_setup_recp()) {
	case USB_RECP_DEVICE:
		switch (type) {
		case USB_TYPE_STANDARD:
			usbd_handle_device_standard();
			return;
		}
		break;
	case USB_RECP_INTERFACE:
		switch (type) {
		case USB_TYPE_STANDARD:
			usbd_handle_interface_standard();
			return;
		case USB_TYPE_CLASS:
			usbd_handle_interface_request();
			return;
		}
		break;
	case USB_RECP_ENDPOINT:
		addr = usbd_control_request_addr();
		if (usbd_endpoint_claimed(addr)) {
			switch (type) {
			case USB_TYPE_STANDARD:
				usbd_handle_endpoint_standard(addr);
				return;
			case USB_TYPE_CLASS:
				usbd_handle_endpoint_request(addr);
				return;
			}
		}
		break;
	}
	usbd_endpoint_halt();
}

static void usbd_handle_ctrl_iocb(void)
{
	usbd_dump_on(USBD_DUMP_CONTROL);

	switch (usbd_control_get_stage()) {
	case USBD_CTRL_STAGE_SETUP:
		usbd_request_set_sync();
		USBD_OUTB(usbd_ctrl_setup.bmRequestType);
		USBD_OUTB(usbd_ctrl_setup.bRequest);
		USBD_OUTW(usbd_ctrl_setup.wValue);
		USBD_OUTW(usbd_ctrl_setup.wIndex);
		USBD_OUTW(usbd_ctrl_setup.wLength);
		usbd_request_clear_sync();
		if (usbd_control_request_length() > 0) {
			break;
		} else {
			/* XXX: "No Data Stage" Control Transfers
			 *
			 * Take care of the ZERO length data stage, fall
			 * through to skip no data stage transfers.
			 */
			usbd_control_set_stage(USBD_CTRL_STAGE_DATA);
		}
	case USBD_CTRL_STAGE_DATA:
		usbd_request_set_sync();
		usbd_handle_ctrl_data();
		usbd_request_clear_sync();
		break;
	}

	usbd_dump_off();
}

/* XXX: Control Endpoints Status Offloading
 *
 * Some USB device controller can handle control status stage without
 * software intervention.  We call this "Control Status Offloading" (CSO).
 * For CSO capable hardware, it is needed to skip the status stage in the
 * software USB device stack.
 */
#ifdef CONFIG_ARCH_HAS_USBD_ENDP_CSO
static void usbd_handle_ctrl_cso(void)
{
	//usbd_control_set_stage(USBD_CTRL_STAGE_SETUP);
	__usbd_control_reset();
}
#else
static void usbd_handle_ctrl_cso(void)
{
	usbd_control_set_stage(USBD_CTRL_STAGE_STATUS);
	if (usbd_endpoint_halting()) {
		__usbd_control_reset();
	}
}
#endif

static void usbd_handle_ctrl_done(void)
{
	switch (usbd_control_get_stage()) {
	case USBD_CTRL_STAGE_SETUP:
		usbd_control_set_stage(USBD_CTRL_STAGE_DATA);
		break;
	case USBD_CTRL_STAGE_DATA:
		usbd_handle_ctrl_cso();
		break;
	case USBD_CTRL_STAGE_STATUS:
		usbd_config_apply();
		usbd_control_set_stage(USBD_CTRL_STAGE_SETUP);
		break;
	}
}

static void usbd_get_status(void)
{
	/* TODO: support REMOTE_WAKEUP & SELF_POWERED */
	uint16_t usb_status = 0;
	USBD_INW(usb_status);
}

static void usbd_set_address(void)
{
	usbd_address = LOBYTE(usbd_control_request_value());

	/* USB2.0 9.4.6 Set Address
	 * The USB device does not change its device address until after
	 * the Status stage of this request is completed successfully.
	 *
	 * Thus, usbd_apply_address() will be called after STATUS stage
	 * completes.
	 */
}

static uint16_t usb_config_length(void)
{
	usb_iid_t iid;
	uint16_t length = USB_DT_CONFIG_SIZE;
	usbd_interface_t *cintf;

	for (iid = 0; iid < usbd_nr_intfs; iid++) {
		uint8_t i;
		length += USB_DT_INTERFACE_SIZE;
		cintf = usbd_interfaces[iid];
		BUG_ON(!cintf->config_len);
		length += cintf->config_len();
		for (i = 0; i < cintf->nr_endps; i++) {
			length += USB_DT_ENDPOINT_SIZE;
		}
	}
	return length;
}

static void usb_interface_ctrl_data(usb_iid_t iid)
{
	usbd_interface_t *cintf;
	cintf = usbd_interfaces[iid];
	BUG_ON(!cintf->ctrl);
	cintf->ctrl();
}

static void usb_get_config_desc(void)
{
	usb_iid_t iid;
	usbd_interface_t *cintf;

	if (LOBYTE(usbd_control_request_value()) != USB_CID_DEFAULT) {
		usbd_endpoint_halt();
		return;
	}

	USBD_INB(USB_DT_CONFIG_SIZE);
	USBD_INB(USB_DT_CONFIG);
	USBD_INW(usb_config_length());
	USBD_INB(usbd_nr_intfs);
	USBD_INB(USB_CID2CONF(USB_CID_DEFAULT));
	USBD_INB(USB_STRING_PRODUCT);
	USBD_INB(USB_CONFIG_ATTR_DEFAULT);
	USBD_INB(USBD_MAX_POWER >> 1);

	for (iid = 0; iid < usbd_nr_intfs; iid++) {
		cintf = usbd_interfaces[iid];
		USBD_INB(USB_DT_INTERFACE_SIZE);
		USBD_INB(USB_DT_INTERFACE);
		USBD_INB(iid);
		USBD_INB(0);
		USBD_INB(cintf->nr_endps);
		usb_interface_ctrl_data(iid);
	}
}

void usbd_input_string(text_char_t *string)
{
	urb_size_t i, n = text_strlen(string);

	USBD_INB((uint8_t)((n<<1) + 2));
	USBD_INB(USB_DT_STRING);
	for (i = 0; i < n; i++) {
		USBD_INB(string[i]);
		USBD_INB(0);
	}
}

void usbd_input_device_name(void)
{
	usbd_input_string(system_device_name);
}

void usbd_input_serialb(uint8_t num)
{
	USBD_INB((uint8_t)(num > 9 ? num-10+'A' : num+'0'));
	USBD_INB('\0');
}

void usbd_input_serialw(uint8_t num)
{
	usbd_input_serialb((uint8_t)(num / 0x10));
	usbd_input_serialb((uint8_t)(num % 0x10));
}

void usbd_input_serial_no(void)
{
	/* XXX: Serial Number Digits
	 *
	 * According to [MSD BBB Revision 1.0] 4.1.1 Serial Number,  MSD
	 * requires that serial number shall contain at least 12 digits.
	 */
#define USB_SERIAL_LENGTH		12
	USBD_INB(USB_SERIAL_LENGTH*2 + 2);
	USBD_INB(USB_DT_STRING);
	usbd_input_serialw(HIBYTE(USBD_VENDOR_ID));
	usbd_input_serialw(LOBYTE(USBD_VENDOR_ID));
	usbd_input_serialw(HIBYTE(system_product_id()));
	usbd_input_serialw(LOBYTE(system_product_id()));
	usbd_input_serialw(HIBYTE(USBD_SERIAL_NO));
	usbd_input_serialw(LOBYTE(USBD_SERIAL_NO));
}

void usbd_input_interface_desc(uint8_t cls, uint8_t subcls,
			       uint8_t proto, uint8_t istring)
{
	USBD_INB(cls);
	USBD_INB(subcls);
	USBD_INB(proto);
	USBD_INB(istring);
}

void usbd_input_endpoint_desc(uint8_t addr)
{
	/* XXX: Endpoint Context Bug
	 *
	 * There could be an easily committed bug like:
	 * saddr = usbd_save_addr(addr);
	 * USBD_INB(usbd_endpoint_size());
	 * usbd_restore_addr(saddr);
	 * Where:
	 * 1. USBD_INB want the default endpoint as its context;
	 * 2. usbd_endpoint want the endpoint whose address is addr as its
	 *    context.
	 * The usbd_save_addr will make a mess of the context value, and
	 * this could always finally lead to a bug.
	 */
	USBD_INB(USB_DT_ENDPOINT_SIZE);
	USBD_INB(USB_DT_ENDPOINT);
	USBD_INB(USB_ENDPADDR(usbd_endpoint_dir_addr(addr),
			      USB_ADDR2EID(addr)));
	USBD_INB(usbd_endpoint_type_addr(addr));
	USBD_INW(usbd_endpoint_size_addr(addr));
	USBD_INB(usbd_endpoint_interval_addr(addr));
}

static void usb_get_string_desc(void)
{
	uint8_t id = LOBYTE(usbd_control_request_value());
	usb_iid_t iid;
	usbd_interface_t *cintf;

	for (iid = 0; iid < usbd_nr_intfs; iid++) {
		cintf = usbd_interfaces[iid];
		if (id >= cintf->string_first &&
		    id <= cintf->string_last) {
			usb_interface_ctrl_data(iid);
			return;
		}
	}
	switch (id) {
	case USB_STRING_LANGID:
		USBD_INB(2+1*2);
		USBD_INB(USB_DT_STRING);
		USBD_INW(USB_LANGID_EN_US);
		break;
	case USB_STRING_FACTORY:
		usbd_input_string(system_vendor_name);
		break;
	case USB_STRING_PRODUCT:
		usbd_input_device_name();
		break;
	case USB_STRING_SERIALNO:
		usbd_input_serial_no();
		break;
	default:
		USBD_INB(0x00);
		break;
	}
	/* usbd_endpoint_halt(); */
}

static void usb_get_device_desc(void)
{
	USBD_INB(USB_DT_DEVICE_SIZE);
	USBD_INB(USB_DT_DEVICE);
	USBD_INW(USB_VERSION_DEFAULT);
	USBD_INB(USB_INTERFACE_CLASS_INTERFACE);
	USBD_INB(USB_DEVICE_SUBCLASS_NONE);
	USBD_INB(USB_PROTO_DEVICE_DEFAULT);
	USBD_INB(USBD_HW_CTRL_SIZE);
	USBD_INW(USBD_VENDOR_ID);
	USBD_INW(system_product_id());
	USBD_INW((uint16_t)system_device_id);
	USBD_INB(USB_STRING_FACTORY);
	USBD_INB(USB_STRING_PRODUCT);
	USBD_INB(USB_STRING_SERIALNO);
	USBD_INB(NR_USBD_CONFS);
}

static void usbd_get_descriptor(void)
{
	uint8_t desc = HIBYTE(usbd_control_request_value());

	usb_debug(USB_DEBUG_DESC, desc);

	switch (desc) {
	case USB_DT_DEVICE:
		usb_get_device_desc();
		break;
	case USB_DT_CONFIG:
		usb_get_config_desc();
		break;
	case USB_DT_STRING:
		usb_get_string_desc();
		break;
	default:
		usbd_endpoint_halt();
		break;
	}
}

static void usbd_get_configuration(void)
{
	USBD_INB(usbd_config);
}

static void usbd_set_configuration(void)
{
	usbd_config = (uint8_t)usbd_control_request_value();

	if (usbd_config != USB_CID2CONF(USB_CID_DEFAULT))
		usbd_config = USB_CONF_DEFAULT;
}

static void usbd_handle_device_standard(void)
{
	usb_debug(USB_DEBUG_SETUP, usbd_control_request_type());

	switch (usbd_control_request_type()) {
	case USB_REQ_GET_STATUS:
		usbd_get_status();
		break;
	case USB_REQ_CLEAR_FEATURE:
		usbd_clear_feature();
		break;
	case USB_REQ_SET_FEATURE:
		usbd_set_feature();
		break;
	case USB_REQ_SET_ADDRESS:
		usbd_set_address();
		break;
	case USB_REQ_GET_DESCRIPTOR:
		usbd_get_descriptor();
		break;
	case USB_REQ_SET_DESCRIPTOR:
		usbd_set_descriptor();
		break;
	case USB_REQ_GET_CONFIGURATION:
		usbd_get_configuration();
		break;
	case USB_REQ_SET_CONFIGURATION:
		usbd_set_configuration();
		break;
	default:
		usbd_endpoint_halt();
		break;
	}
}

static void usbd_endpoint_get_status(uint8_t addr)
{
	uint8_t saddr = usbd_addr_save(addr);
	uint16_t status = 0;

	if (usbd_endpoint_halting())
		raise_bits(status, USB_ENDP_STATUS_HALT);
	USBD_INW(status);
	usbd_addr_restore(saddr);
}

void usbd_endpoint_unhalt_addr(uint8_t addr)
{
	uint8_t saddr = usbd_addr_save(addr);
	usbd_endpoint_unhalt();
	usbd_addr_restore(saddr);
}

void usbd_endpoint_halt_addr(uint8_t addr)
{
	uint8_t saddr = usbd_addr_save(addr);
	usbd_endpoint_halt();
	usbd_addr_restore(saddr);
}

static void usbd_endpoint_clear_feature(uint8_t addr)
{
	switch (usbd_control_request_value()) {
	case USB_ENDPOINT_HALT:
		/* XXX: Recover Condition - CLEAR_FEATURE (HALT)
		 *
		 * STALL state recovers from the condition (in an
		 * implementation-dependent way) and acknowledges the halt
		 * or error condition via a USBD call.
		 */
		usbd_endpoint_unhalt_addr(addr);
		break;
	}
}

static void usbd_endpoint_set_feature(uint8_t addr)
{
	switch (usbd_control_request_value()) {
	case USB_ENDPOINT_HALT:
		usbd_endpoint_halt_addr(addr);
		break;
	}
}

#define usbd_endpoint_synch_frame(addr)		usbd_endpoint_halt()

static void usbd_handle_endpoint_standard(uint8_t addr)
{
	usb_debug(USB_DEBUG_SETUP, usbd_control_request_type());

	switch (usbd_control_request_type()) {
	case USB_REQ_GET_STATUS:
		usbd_endpoint_get_status(addr);
		break;
	case USB_REQ_CLEAR_FEATURE:
		usbd_endpoint_clear_feature(addr);
		break;
	case USB_REQ_SET_FEATURE:
		usbd_endpoint_set_feature(addr);
		break;
	case USB_REQ_SYNCH_FRAME:
		usbd_endpoint_synch_frame(addr);
		break;
	default:
		usbd_handle_endpoint_request(addr);
		break;
	}
}

static void usbd_handle_endpoint_request(uint8_t addr)
{
#ifdef CONFIG_USBD_CLASS_RECIP_ENDP
	/* XXX: No Class Specific USB_RECP_ENDPOINT Requests
	 * Thus we can safely save ROM consumptions by disabling the
	 * ctrl interface for endpoint structures.  If there appears
	 * to be any such requirement, feel free to enable it again.
	 */
	usbd_endpoint_t *cendp;
	cendp = usbd_endp_func_by_addr(addr);
	BUG_ON(!cendp->ctrl);
	cendp->ctrl();
#else
	usbd_endpoint_halt();
#endif
}

static void usb_interface_get_interface(usb_iid_t iid)
{
	/* This USB device stack only support 1 alternate setting. */
	USBD_INB(0);
}

static void usb_interface_set_interface(usb_iid_t iid)
{
	/* This USB device stack only support 1 alternate setting. */
	if (LOBYTE(usbd_control_request_value()) != 0)
		usbd_endpoint_halt();
}

#define usb_interface_get_status(iid)		USBD_INW(0)
#define usb_interface_clear_feature(iid)
#define usb_interface_set_feature(iid)

static void usbd_handle_interface_standard(void)
{
	usb_iid_t iid = (usb_iid_t)usbd_control_request_index();

	usb_debug(USB_DEBUG_SETUP, usbd_control_request_type());
	usb_debug_iid(iid);

	if (iid < usbd_nr_intfs) {
		switch (usbd_control_request_type()) {
		case USB_REQ_GET_STATUS:
			usb_interface_get_status(iid);
			break;
		case USB_REQ_CLEAR_FEATURE:
			usb_interface_clear_feature(iid);
			break;
		case USB_REQ_SET_FEATURE:
			usb_interface_set_feature(iid);
			break;
		case USB_REQ_GET_INTERFACE:
			usb_interface_get_interface(iid);
			break;
		case USB_REQ_SET_INTERFACE:
			usb_interface_set_interface(iid);
			break;
		default:
			usbd_handle_interface_request();
			break;
		}
	} else {
		usbd_endpoint_halt();
	}
}

static void usbd_handle_interface_request(void)
{
	usb_iid_t iid = (usb_iid_t)usbd_control_request_index();

	if (iid < usbd_nr_intfs) {
		usb_interface_ctrl_data(iid);
	} else {
		usbd_endpoint_halt();
	}
}

static void usbd_apply_address(void)
{
	usbd_hw_set_address();
	if (usbd_address != USB_ADDR_DEFAULT) {
		if (usbd_state_get() == USB_STATE_DEFAULT)
			usbd_state_set(USB_STATE_ADDRESS);
	} else {
		usbd_state_set(USB_STATE_DEFAULT);
		usbd_control_start();
	}
}

static void usbd_apply_config(void)
{
	usbd_hw_set_config();
	if (usbd_config == USB_CID2CONF(USB_CID_DEFAULT)) {
		if (usbd_state_get() == USB_STATE_ADDRESS) {
			usbd_state_set(USB_STATE_CONFIGURED);
			usbd_endpoint_start_all();
		}
	} else {
		usbd_apply_address();
	}
}

void usbd_config_apply(void)
{
	BUG_ON(USB_ADDR2EID(usbd_endp) != USB_EID_DEFAULT);

	switch (usbd_control_request_type()) {
	case USB_REQ_SET_ADDRESS:
		usbd_apply_address();
		break;
	case USB_REQ_SET_CONFIGURATION:
		usbd_apply_config();
		break;
	}
}

void usbd_config_reset(void)
{
	usbd_address = USB_ADDR_DEFAULT;
	usbd_config = USB_CONF_DEFAULT;
	usbd_apply_config();
}

/*=========================================================================
 * endpoint API
 *=======================================================================*/
static void usbd_request_exit(void)
{
	__usbd_request_exit();
	if (usbd_endpoint_type() == USB_ENDP_CONTROL)
		__usbd_control_reset();
}

void usbd_endpoint_reset(void)
{
	/* XXX: Recover Condition - BUS RESET
	 *
	 * STALL state recovers from the condition (in an
	 * implementation-dependent way) and acknowledges the halt or
	 * error condition via a USBD call.
	 */

	/* XXX: Hardware Reset v.s. Software Reset
	 *
	 * The real reset that resets the endpoint hardware, but this
	 * should not be always required.  At most cases, USB device stack
	 * should call usbd_request_reset() since the controller should
	 * reset "FIFO contents/data toggle bit/IRP state" itsself to
	 * achieve maximum throuput, only when the pipe is stalled,
	 * usbd_hw_request_reset() is required to be called to retire the
	 * stalled IRP in the FIFO and reset Data Toggle Bit by software.
	 */
	usbd_hw_request_reset();
	/* The usbd_endpoint_unhalt() call should always be called before
	 * the usbd_request_reset() call.
	 */
	usbd_endpoint_unhalt();
	usbd_request_exit();
}

void usbd_endpoint_start_addr(uint8_t addr)
{
	uint8_t saddr;
	
	saddr = usbd_addr_save(addr);
	if (usbd_endpoint_claimed(usbd_endp)) {
		usbd_hw_endp_enable();
		usbd_endpoint_reset();
	}
	usbd_addr_restore(saddr);
}

void usbd_endpoint_reset_addr(uint8_t addr)
{
	uint8_t saddr;
	
	saddr = usbd_addr_save(addr);
	if (usbd_endpoint_claimed(usbd_endp)) {
		usbd_endpoint_reset();
	}
	usbd_addr_restore(saddr);
}

void usbd_control_start(void)
{
	usbd_endpoint_start_addr(USB_CTRLADDR(USB_EID_DEFAULT));
}

void usbd_endpoint_start_all(void)
{
	uint8_t eid;
	for (eid = USB_EID_DEFAULT+1; eid < NR_USBD_ENDPS; eid++) {
		usbd_duplex_start_addr(eid);
	}
}

static void usbd_async_iocb_addr(uint8_t addr)
{
	uint8_t saddr = usbd_addr_save(addr);
	if (usbd_endpoint_claimed(usbd_endp))
		usbd_async_iocb();
	usbd_addr_restore(saddr);
}

static void usbd_async_handler(void)
{
	uint8_t eid;
	/* XXX: EID ordering
	 * Some interface drivers require changes to be made according to
	 * the EID ordering implemented here, see usb_scd.c for details.
	 */
	usbd_async_iocb_addr(USB_ADDR(USB_DIR_OUT, USB_EID_DEFAULT));
	for (eid = USB_EID_DEFAULT+1; eid < NR_USBD_ENDPS; eid++) {
		usbd_duplex_async_addr(eid);
	}
}

#ifdef CONFIG_PM
void usbd_bus_suspend(void)
{
	if (usbd_state_get() != USB_STATE_SUSPENDED) {
		usb_debug_irq(USBD_IRQ_SUSPEND);
		usbd_hw_pm_suspend();
		usbd_state_resume = usbd_state_get();
		usbd_state_set(USB_STATE_SUSPENDED);
	}
}

void usbd_bus_resume(void)
{
	if (usbd_state_get() == USB_STATE_SUSPENDED) {
		usb_debug_irq(USBD_IRQ_RESUME);
		usbd_hw_pm_resume();
		usbd_state_set(usbd_state_resume);
	}
}
#endif

void usbd_bus_reset(void)
{
	uint8_t saddr;
	
	usb_debug_irq(USBD_IRQ_RESET);
	usbd_config_reset();
	saddr = usbd_addr_save(USB_EID_DEFAULT);
	__usbd_control_reset();
	usbd_addr_restore(saddr);
}

#ifdef SYS_BOOTLOAD
#define usbd_timer_init()
void usbd_frame_start(uint16_t frame)
{
}
#else
#ifndef CONFIG_USBD_POLL_NOTIMER
__near__ tid_t usbd_tid = INVALID_TID;

static void usbd_endpoint_poll(void)
{
	if (!usbd_request_pending()) {
		usbd_request_poll();
	}
}

static void usbd_endpoint_poll_addr(uint8_t addr)
{
	uint8_t saddr = usbd_addr_save(addr);
	if (usbd_endpoint_claimed(usbd_endp))
		usbd_endpoint_poll();
	usbd_addr_restore(saddr);
}

void usbd_endpoint_poll_all(void)
{
	uint8_t eid;
	if (usbd_state_get() != USB_STATE_CONFIGURED)
		return;
	for (eid = USB_EID_DEFAULT+1; eid < NR_USBD_ENDPS; eid++) {
		usbd_duplex_poll_addr(eid);
	}
}

static void usbd_timer_handler(void)
{
	usbd_endpoint_poll_all();
	timer_schedule_shot(usbd_tid, USBD_POLL_INTERVAL);
}

timer_desc_t usbd_timer = {
	TIMER_BH,
	usbd_timer_handler,
};

static void usbd_timer_init(void)
{
	usbd_tid = timer_register(&usbd_timer);
	timer_schedule_shot(usbd_tid, USBD_POLL_INTERVAL);
}

void usbd_frame_start(uint16_t frame)
{
#if 0
	usb_debug_irq(USBD_IRQ_SOF);
	usbd_endpoint_poll_all();
#endif
}
#else
#define usbd_timer_init()

void usbd_frame_start(uint16_t frame)
{
}

static void usbd_request_poll_addr(uint8_t addr)
{
	uint8_t saddr = usbd_addr_save(addr);
	if (usbd_endpoint_claimed(usbd_endp))
		usbd_request_poll();
	usbd_addr_restore(saddr);
}

void usbd_request_poll_all(void)
{
	uint8_t eid;
	if (usbd_state_get() != USB_STATE_CONFIGURED)
		return;
	for (eid = USB_EID_DEFAULT+1; eid < NR_USBD_ENDPS; eid++) {
		usbd_duplex_poll2_addr(eid);
	}
}
#endif
#endif

void usbd_handler(uint8_t event)
{
	switch (event) {
	case BH_WAKEUP:
		usbd_async_handler();
		break;
	default:
		BUG();
	}
}

void usbd_start(void)
{
	/* This function is called whenever VBUS is powered, trying to
	 * call usbd_hw_ctrl_start to issue a USB RESET interrupt.
	 */
	usbd_hw_bus_attach();
	usbd_hw_ctrl_start();
	usbd_hw_pm_start();
}

void usbd_stop(void)
{
	usbd_hw_pm_stop();
	usbd_hw_ctrl_stop();
	usbd_hw_bus_detach();
}

void usbd_restart(void)
{
	usbd_stop();
	mdelay(100);
	usbd_start();
}

void usbd_switch(void)
{
	/* This function is called whenever ID is indicating a B device,
	 * HNP result is a B device, or this is called by the usb_init for
	 * dedicated device.
	 */
	usbd_state_set(USB_STATE_ATTACHED);
	usbd_hw_ctrl_init();
}

void usbd_init(void)
{
	usbd_reserve_endpoint0();
	usbd_timer_init();
}
