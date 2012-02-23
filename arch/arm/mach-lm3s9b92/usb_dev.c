#include <target/usb.h>
#include <target/delay.h>
#include <target/io.h>
#include <target/irq.h>
#include "usb.h"

#ifdef CONFIG_USB_LM3S9B92_DUMP_REGS
void __usbd_hw_dump_regs(uint8_t hint);
#else
#define __usbd_hw_dump_regs(hint)
#endif

#if 0
/* XXX: USBD_ENDP_DUPLEX Modification
 * Enable the definition below if duplex modification has succeeded.
 */
#define __USBD_HW_DIR()			USB_ADDR2DIR(usbd_endp)
#else
#define __USBD_HW_DIR()			usbd_endpoint_dir()
#endif

#define __USBD_HW_SPEED_FULL		0x80
#define __USBD_HW_SPEED_LOW		0xC0

#define __usbd_hw_csetup_raised()	__raw_testb_atomic(SETEND, USBCSRL0)
#define __usbd_hw_cstall_raised()	__raw_testb_atomic(STALLED, USBCSRL0)
#define	__usbd_hw_ctxrdy_raised()	__raw_testb_atomic(TXRDY, USBCSRL0)
#define	__usbd_hw_crxrdy_raised()	__raw_testb_atomic(RXRDY, USBCSRL0)
#define __usbd_hw_unraise_csetup()	__raw_clearb_atomic(SETENDC, USBCSRL0)

/* XXX: STATUS Stage Offloading Issue - Chip Bug
 * On LM3S9B92, NO DATA stage control requests have a special logic in the
 * controller.  It seems the controller has sent a ZLP for such requests while
 * it should be sent by the CPU in may firmware designs.  Note that a control
 * IRQ will arrive with USBCSR0 is 0x0000 (TXRDY completion) even when the CPU
 * side does not perform any TX activities for the STATUS stage for such
 * requests.  We could take this IRQ as a STATUS stage TX completion and
 * carefully not send any ZLPs in the STATUS stages for such requests.
 * This workaround can work for the USB device controller though it should
 * have presented a more comprehensive flag for such offloading.
 */
static uint8_t __usbd_hw_is_cso;

uint16_t __usbd_hw_endp_txrdy;
#define __usbd_hw_set_txrdy(eid)	\
	raise_bits(__usbd_hw_endp_txrdy, ((uint16_t)1)<<(eid))
#define __usbd_hw_clear_txrdy(eid)	\
	unraise_bits(__usbd_hw_endp_txrdy, ((uint16_t)1)<<(eid))
#define __usbd_hw_test_txrdy(eid)	\
	bits_raised_any(__usbd_hw_endp_txrdy, ((uint16_t)1)<<(eid))

static uint16_t __usbd_hw_fifo_addr = 8;

static inline void __usbd_hw_cso_capture(void)
{
 	if (USB_ADDR2EID(usbd_endp) == USB_EID_DEFAULT) {
 		switch (usbd_control_request_type()) {
 		case USB_REQ_SET_ADDRESS:
 			__usbd_hw_is_cso = 1;
			break;
 		case USB_REQ_SET_CONFIGURATION:
			usbd_config_apply();
 			break;
 		}
 	}
}

static inline void __usbd_hw_cso_release(void)
{
 	if (USB_ADDR2EID(usbd_endp) == USB_EID_DEFAULT) {
		if (__usbd_hw_is_cso == 1) {
			usbd_config_apply();
			__usbd_hw_is_cso = 0;
		}
	}
}

static uint16_t __usbd_hw_tx_status = 0;
static uint16_t __usbd_hw_rx_status = 0;
static uint8_t __usbd_hw_dev_status = 0;

static void __usbd_hw_dirq_reset(void)
{
	__usb_hw_irq_status();
	__usbd_hw_dev_status = 0;
}

static inline void __usbd_hw_dirq_save(void)
{
	__usbd_hw_dev_status |= __usb_hw_irq_status();
}

static inline void __usbd_hw_dirq_restore(void)
{
	__usbd_hw_dev_status = 0;
}

static void __usbd_hw_eirq_reset(void)
{
	__raw_readw(USBTXIS);
	__raw_readw(USBRXIS);
	__usbd_hw_tx_status = 0;
	__usbd_hw_rx_status = 0;
}

#define USBCTXIE		0
#define USBCRXIE		1
#ifdef SYS_REALTIME
#define __usbd_hw_eirq_enable()
#define __usbd_hw_eirq_disable()
#define __usbd_hw_eirq_enabled(dir)	true
#define __usbd_hw_cirq_enabled(irq)	true
#else
static uint8_t __usbd_hw_ctrl_flags = 0;

#define __usbd_hw_cirq_enabled(irq)	(__usbd_hw_ctrl_flags & _BV(irq))

static inline void __usbd_hw_eirq_enable(void)
{
	uint8_t eid = USB_ADDR2EID(usbd_endp);
	uint8_t dir = usbd_request_dir();

	if (eid == USB_EID_DEFAULT) {
		__usbd_hw_ctrl_flags |= _BV(USBCTXIE);
		__usbd_hw_ctrl_flags |= _BV(USBCRXIE);
		__raw_setw_atomic(eid, USBTXIE);
	} else {
		if (dir == USB_DIR_IN) {
			__raw_setw_atomic(eid, USBTXIE);
		} else {
			__raw_setw_atomic(eid, USBRXIE);
		}
	}
}

static inline void __usbd_hw_eirq_disable(void)
{
	uint8_t eid = USB_ADDR2EID(usbd_endp);
	uint8_t dir = usbd_request_dir();

	if (eid == USB_EID_DEFAULT) {
		__usbd_hw_ctrl_flags &= ~_BV(USBCTXIE);
		__usbd_hw_ctrl_flags &= ~_BV(USBCRXIE);
		__raw_clearw_atomic(eid, USBTXIE);
	} else {
		if (dir == USB_DIR_IN) {
			__raw_clearw_atomic(eid, USBTXIE);
		} else {
			__raw_clearw_atomic(eid, USBRXIE);
		}
	}
}

static boolean __usbd_hw_eirq_enabled(uint8_t dir)
{
	uint8_t eid = USB_ADDR2EID(usbd_endp);

	if (eid == USB_EID_DEFAULT) {
		if (dir == USB_DIR_IN) {
			return (__usbd_hw_ctrl_flags & _BV(USBCTXIE));
		} else {
			return (__usbd_hw_ctrl_flags & _BV(USBCRXIE));
		}
	} else {
		if (dir == USB_DIR_IN) {
			return __raw_testw_atomic(eid, USBTXIE);
		} else {
			return __raw_testw_atomic(eid, USBRXIE);
		}
	}
}
#endif

static inline void __usbd_hw_eirq_save(void)
{
	uint16_t cirq = _BV(USB_EID_DEFAULT);
	uint16_t tx_status = __raw_readw(USBTXIS);
	uint16_t rx_status = __raw_readw(USBRXIS);

	/* Split control irq into direction specific IRQ. */
	if (tx_status & cirq) {
		tx_status &= ~cirq;
		if (__usbd_hw_endp_txrdy & cirq) {
			if (__usbd_hw_cstall_raised() ||
			    !__usbd_hw_ctxrdy_raised()) {
				tx_status |= cirq;
			}
		} else {
			if (__usbd_hw_cstall_raised()) {
				rx_status |= cirq;
			}
		}
		if (__usbd_hw_csetup_raised() ||
		    __usbd_hw_crxrdy_raised()) {
			rx_status |= cirq;
		}
	}

	__usbd_hw_tx_status |= tx_status;
	__usbd_hw_rx_status |= rx_status;
}

static inline void __usbd_hw_eirq_restore(void)
{
	uint8_t cirq = _BV(USB_EID_DEFAULT);
	uint16_t tx_enable = __raw_readw(USBTXIE);
	uint16_t rx_enable = __raw_readw(USBRXIE);

	tx_enable &= ~cirq;
	if (__usbd_hw_cirq_enabled(USBCTXIE))
		tx_enable |= cirq;
	if (__usbd_hw_cirq_enabled(USBCRXIE))
		rx_enable |= cirq;

	if ((__usbd_hw_tx_status & tx_enable) ||
	    (__usbd_hw_rx_status & rx_enable)) {
		__usbd_hw_dump_regs(0xA0);
		usb_hw_irq_trigger();
	}
}

#define __usbd_hw_set_address()		__raw_writeb(usbd_address & 0x7F, USBFADDR)
#define __usbd_hw_hub_connect()		__raw_setb_atomic(DEVSOFTCONN, USBPOWER)
#define __usbd_hw_hub_disconnect()	__raw_clearb_atomic(DEVSOFTCONN, USBPOWER)

static boolean __usbd_hw_txirq_raised(void)
{
	uint8_t eid = USB_ADDR2EID(usbd_endp);

	return __usbd_hw_eirq_enabled(USB_DIR_IN) &&
	       (__usbd_hw_tx_status & _BV(eid));
}

static boolean __usbd_hw_rxirq_raised(void)
{
	uint8_t eid = USB_ADDR2EID(usbd_endp);

	return __usbd_hw_eirq_enabled(USB_DIR_OUT) &&
	       (__usbd_hw_rx_status & _BV(eid));
}

static void __usbd_hw_unraise_txirq(void)
{
	uint8_t eid = USB_ADDR2EID(usbd_endp);

	__usbd_hw_tx_status &= ~_BV(eid);
}

static void __usbd_hw_unraise_rxirq(void)
{
	uint8_t eid = USB_ADDR2EID(usbd_endp);

	__usbd_hw_rx_status &= ~_BV(eid);
}

static inline void __usbd_hw_raise_flush(void)
{
	uint8_t eid = USB_ADDR2EID(usbd_endp);
	uint8_t dir = __USBD_HW_DIR();

	if (eid == USB_EID_DEFAULT) {
		__raw_setb_atomic(FLUSH, USBCSRH0);
	} else {
		if (dir == USB_DIR_IN) {
			__raw_setb_atomic(TXFLUSH, USBTXCSRL(eid));
		} else {
			__raw_setb_atomic(RXFLUSH, USBRXCSRL(eid));
		}
	}
}

static inline void __usbd_hw_toggle_data(void)
{
	uint8_t eid = USB_ADDR2EID(usbd_endp);
	uint8_t dir = __USBD_HW_DIR();

	if (eid != USB_EID_DEFAULT) {
		if (dir == USB_DIR_IN) {
			__raw_setb_atomic(TXCLRDT, USBTXCSRL(eid));
		} else {
			__raw_setb_atomic(RXCLRDT, USBRXCSRL(eid));
		}
	}
}

static void __usbd_hw_mark_dataend(void)
{
	if ((usbd_control_get_stage() == USBD_CTRL_STAGE_DATA) &&
	    usbd_transfer_last()) {
		__raw_setb_atomic(DATAEND, USBCSRL0);
	}
}

static boolean __usbd_hw_stall_raised(void)
{
	uint8_t eid = USB_ADDR2EID(usbd_endp);
	uint8_t dir = __USBD_HW_DIR();

	if (eid == USB_EID_DEFAULT) {
		return __usbd_hw_cstall_raised();
	} else {
		if (dir == USB_DIR_IN) {
			return __raw_testb_atomic(TXSTALLED, USBTXCSRL(eid));
		} else {
			return __raw_testb_atomic(RXSTALLED, USBRXCSRL(eid));
		}
	}
}

static boolean __usbd_hw_is_txrdy(void)
{
	uint8_t eid = USB_ADDR2EID(usbd_endp);

	if (eid == USB_EID_DEFAULT) {
		return __usbd_hw_ctxrdy_raised();
	} else {
		return __raw_testb_atomic(TXTXRDY, USBTXCSRL(eid));
	}
}

static boolean __usbd_hw_is_rxrdy(void)
{
	uint8_t eid = USB_ADDR2EID(usbd_endp);

	if (eid == USB_EID_DEFAULT) {
		return __usbd_hw_crxrdy_raised();
	} else {
		return __raw_testb_atomic(RXRXRDY, USBRXCSRL(eid));
	}
}

static void __usbd_hw_unraise_rxrdy(void)
{
	uint8_t eid = USB_ADDR2EID(usbd_endp);

	if (eid == USB_EID_DEFAULT) {
		__raw_setb_atomic(RXRDYC, USBCSRL0);
		__usbd_hw_mark_dataend();
	} else {
		__raw_clearb(_BV(RXRXRDY) |
			     _BV(RXDATAERR) | _BV(RXOVER),
			     USBRXCSRL(eid));
	}
}

static void __usbd_hw_unraise_stall(void)
{
	uint8_t eid = USB_ADDR2EID(usbd_endp);
	uint8_t dir = __USBD_HW_DIR();

	if (eid == USB_EID_DEFAULT) {
		__raw_clearb_atomic(STALLED, USBCSRL0);
	} else {
		if (dir == USB_DIR_IN) {
			__raw_clearb_atomic(TXSTALLED, USBTXCSRL(eid));
		} else {
			__raw_clearb_atomic(RXSTALLED, USBRXCSRL(eid));
		}
	}
}

static boolean __usbd_hw_is_txcmpl(void)
{
	return __usbd_hw_test_txrdy(USB_ADDR2EID(usbd_endp)) &&
	       !__usbd_hw_is_txrdy();
}

static void __usbd_hw_raise_txrdy(void)
{
	uint8_t eid = USB_ADDR2EID(usbd_endp);

	__usbd_hw_set_txrdy(eid);
	if (eid == USB_EID_DEFAULT) {
		__raw_setb_atomic(TXRDY, USBCSRL0);
		__usbd_hw_mark_dataend();
	} else {
		__raw_setb_atomic(TXTXRDY, USBTXCSRL(eid));
	}
}

static void __usbd_hw_unraise_txcmpl(void)
{
	uint8_t eid = USB_ADDR2EID(usbd_endp);

	if (eid != USB_EID_DEFAULT) {
		__raw_clearb(_BV(TXUNDRN), USBTXCSRL(eid));
	}
	__usbd_hw_clear_txrdy(eid);
}

/*=========================================================================
 * usb device implementation
 *=======================================================================*/
utb_text_size_t usbd_hw_endp_sizes[NR_USBD_ENDPS] = {
	64,
#if CONFIG_USB_LM3S9B92_MAX_ISOS > 0
	USBD_HW_ISO_SIZE,
#endif
#if CONFIG_USB_LM3S9B92_MAX_ISOS > 1
	USBD_HW_ISO_SIZE,
#endif
#if USB_HW_EID_MAX_NONISOS > 0
	64,
#endif
#if USB_HW_EID_MAX_NONISOS > 1
	64,
#endif
#if USB_HW_EID_MAX_NONISOS > 2
	64,
#endif
#if USB_HW_EID_MAX_NONISOS > 3
	64,
#endif
#if USB_HW_EID_MAX_NONISOS > 4
	64,
#endif
#if USB_HW_EID_MAX_NONISOS > 5
	64,
#endif
#if USB_HW_EID_MAX_NONISOS > 6
	64,
#endif
#if USB_HW_EID_MAX_NONISOS > 7
	64,
#endif
#if USB_HW_EID_MAX_NONISOS > 8
	64,
#endif
#if USB_HW_EID_MAX_NONISOS > 9
	64,
#endif
#if USB_HW_EID_MAX_NONISOS > 10
	64,
#endif
#if USB_HW_EID_MAX_NONISOS > 11
	64,
#endif
#if USB_HW_EID_MAX_NONISOS > 12
	64,
#endif
#if USB_HW_EID_MAX_NONISOS > 13
	64,
#endif
#if USB_HW_EID_MAX_NONISOS > 14
	64,
#endif
#if CONFIG_USB_LM3S9B92_MAX_ISOS > 2
	USBD_HW_ISO_SIZE,
#endif
};

boolean usbd_hw_endp_caps(uint8_t addr, uint8_t type)
{
	uint8_t eid = USB_ADDR2EID(addr);
	if (eid >= NR_USBD_ENDPS)
		return false;
	if (type == USB_ENDP_CONTROL) {
		if (eid != USB_EID_DEFAULT)
			return false;
	}
	if (type == USB_ENDP_BULK || type == USB_ENDP_INTERRUPT) {
		if (eid < USB_HW_EID_BI_MIN || eid > USB_HW_EID_BI_MAX)
			return false;
	}
	return true;
}

utb_size_t usbd_hw_endp_size(uint8_t addr)
{
	uint8_t eid = USB_ADDR2EID(addr);
	if (eid >= NR_USBD_ENDPS)
		return 0;
	return usbd_hw_endp_sizes[eid];
}

void usbd_hw_set_address(void)
{
	__usbd_hw_set_address();
}

void usbd_hw_set_config(void)
{
	/* Leave the CONFIGURED state with FIFO configuration reset. */
	if (usbd_config == USB_CONF_DEFAULT)
		__usbd_hw_fifo_addr = 8;
}

void usbd_hw_endp_select(uint8_t addr)
{
	__raw_writeb(USB_ADDR2EID(addr), USBEPIDX);
}

void usbd_hw_endp_halt(void)
{
	uint8_t eid = USB_ADDR2EID(usbd_endp);
	uint8_t dir = __USBD_HW_DIR();

	/* XXX: Control STALL Handshake
	 * Set to 1 will terminate the current transaction and transmit
	 * the STALL handshake.  The STALL bit of USBCSRL0 is cleared
	 * automatically after the STALL handshake is transmitted.
	 */
	if (eid != USB_EID_DEFAULT) {
		if (dir == USB_DIR_IN) {
			__raw_setb_atomic(TXSTALL, USBTXCSRL(eid));
		} else {
			__raw_setb_atomic(RXSTALL, USBRXCSRL(eid));
		}
	}
}

void usbd_hw_endp_unhalt(void)
{
	uint8_t eid = USB_ADDR2EID(usbd_endp);
	uint8_t dir = __USBD_HW_DIR();

	if (eid == USB_EID_DEFAULT) {
		__raw_clearb_atomic(STALL, USBCSRL0);
	} else {
		if (dir == USB_DIR_IN) {
			__raw_clearb_atomic(TXSTALL, USBTXCSRL(eid));
		} else {
			__raw_clearb_atomic(RXSTALL, USBRXCSRL(eid));
		}
	}
}

static inline uint8_t __usbd_hw_type_bits(void)
{
	return USB_ENDP_ISOCHRONOUS == usbd_endpoint_type() ?
	       _BV(TXISO) : 0;
}

static uint16_t __usbd_hw_fifoadd_inc(void)
{
	return usbd_endpoint_size() >> USB_LM3S9B92_FIFOADD_S;
}

static uint8_t __usbd_hw_fifosz_value(void)
{
	uint8_t size_type = usb_endpoint_size_type(usbd_endpoint_size());
	return (size_type | (USB_LM3S9B92_FIFOSZ_DPB<<DPB));
}

void usbd_hw_endp_enable(void)
{
	uint8_t eid = USB_ADDR2EID(usbd_endp);
	uint8_t dir = __USBD_HW_DIR();

	if (eid != USB_EID_DEFAULT) {
		BUG_ON(usbd_config == USB_CONF_DEFAULT);
		if (dir == USB_DIR_IN) {
			__raw_writew(__usbd_hw_fifo_addr, USBTXFIFOADD);
			__raw_writeb(__usbd_hw_fifosz_value(), USBTXFIFOSZ);
			__raw_writew(usbd_endpoint_size(), USBTXMAXP(eid));
			__raw_writeb(__usbd_hw_type_bits() | _BV(TXMODE),
				     USBTXCSRH(eid));
		} else {
			__raw_writew(__usbd_hw_fifo_addr, USBRXFIFOADD);
			__raw_writeb(__usbd_hw_fifosz_value(), USBRXFIFOSZ);
			__raw_writew(usbd_endpoint_size(), USBRXMAXP(eid));
			__raw_writeb(__usbd_hw_type_bits(), USBRXCSRH(eid));
		}
		__usbd_hw_fifo_addr += __usbd_hw_fifoadd_inc();
	}
}

static utb_size_t usbd_hw_read_avail(void)
{
	if (usbd_endpoint_type() == USB_ENDP_CONTROL) {
		return __raw_readw(USBCOUNT0);
	} else {
		return __raw_readw(USBRXCOUNT(USB_ADDR2EID(usbd_endp)));
	}
}

void usbd_hw_request_reset(void)
{
	__usbd_hw_toggle_data();
	__usbd_hw_raise_flush();
	__usbd_hw_dump_regs(0x90);
}

void usbd_hw_request_open(void)
{
	__usbd_hw_dump_regs(0x70);
	__usbd_hw_eirq_enable();
}

void usbd_hw_request_close(void)
{
	__usbd_hw_eirq_disable();
	__usbd_hw_dump_regs(0x80);
}

void usbd_hw_transfer_open(void)
{
	__usbd_hw_dump_regs(0x30);
	if (usbd_request_dir() == USB_DIR_IN) {
		if (usbd_request_syncing())
			while (__usbd_hw_is_txrdy());
	} else {
		if (usbd_request_syncing())
			while (!__usbd_hw_is_rxrdy());
		usbd_transfer_submit(usbd_hw_read_avail());
	}
	__usbd_hw_dump_regs(0x40);
}

void usbd_hw_transfer_close(void)
{
	__usbd_hw_dump_regs(0x50);
	if (usbd_request_dir() == USB_DIR_IN) {
		__usbd_hw_raise_txrdy();
		if (usbd_request_syncing()) {
			while (__usbd_hw_is_txrdy());
			__usbd_hw_unraise_txcmpl();
		}
	} else {
		__usbd_hw_cso_capture();
		if (usbd_control_setup_staging()) {
			if (__usbd_hw_csetup_raised()) {
				__usbd_hw_unraise_csetup();
			}
		}
		__usbd_hw_unraise_rxrdy();
	}
	__usbd_hw_dump_regs(0x60);
}

static void usbd_hw_handle_rxout(void)
{
	if (!usbd_request_interrupting(USB_DIR_OUT))
		return;
	if (__usbd_hw_stall_raised()) {
		__usbd_hw_unraise_stall();
		usbd_request_stall();
	}
	if (__usbd_hw_is_rxrdy()) {
		usbd_transfer_rxout();
	}
	__usbd_hw_dump_regs(0x10);
}

static void usbd_hw_handle_txin(void)
{
	if (!usbd_request_interrupting(USB_DIR_IN))
		return;
	if (__usbd_hw_stall_raised()) {
		__usbd_hw_unraise_stall();
		usbd_request_stall();
	}
	if (__usbd_hw_is_txcmpl()) {
		__usbd_hw_unraise_txcmpl();
		usbd_transfer_txin();
	}
	__usbd_hw_dump_regs(0x20);
}

void usbd_hw_handle_irq(void)
{
	uint8_t saddr;
	uint8_t eid;

	__usbd_hw_dirq_save();
	__usbd_hw_eirq_save();

	for (eid = 0; eid < NR_USBD_HW_ENDPS; eid++) {
		saddr = usbd_addr_save(USB_ADDR(USB_DIR_OUT, eid));
		__usbd_hw_cso_release();
		if (__usbd_hw_rxirq_raised()) {
			__usbd_hw_unraise_rxirq();
			if (eid == USB_EID_DEFAULT) {
				if (__usbd_hw_csetup_raised()) {
					usbd_control_reset();
				}
			}
			usbd_hw_handle_rxout();
		}
		usbd_addr_restore(saddr);

		saddr = usbd_addr_save(USB_ADDR(USB_DIR_IN, eid));
		if (__usbd_hw_txirq_raised()) {
			__usbd_hw_unraise_txirq();
			usbd_hw_handle_txin();
		}
		usbd_addr_restore(saddr);
	}

	if (__otg_hw_id_irq_status()) {
	}
	if (__usb_hw_dr_irq_status()) {
	}
	if (__usbd_hw_dev_status & _BV(DEVSOF)) {
		usbd_frame_start(__raw_readw(USBFRAME));
	}
	if (__usbd_hw_dev_status & _BV(DEVRESET)) {
		usbd_bus_reset();
	}
	if (__usbd_hw_dev_status & _BV(DEVRESUME)) {
		usbd_bus_resume();
	}
	if (__usbd_hw_dev_status & _BV(DEVSUSPEND)) {
		usbd_bus_suspend();
	}
	if (__usbd_hw_dev_status & _BV(DEVDISCON)) {
		usb_debug_irq(USBD_IRQ_DETACH);
	}

	__usbd_hw_eirq_restore();
	__usbd_hw_dirq_restore();
}

#ifdef CONFIG_USB_LM3S9B92_DUMP_REGS
void __usbd_hw_dump_regs(uint8_t hint)
{
	uint8_t eid = USB_ADDR2EID(usbd_endp);
	uint8_t dir = usbd_request_dir();

	dbg_dump(hint++);
	dbg_dump(__raw_readb(USBFADDR));
	dbg_dump(hint++);
	dbg_dump(usbd_hw_read_avail());
	dbg_dump(hint++);
	if ((eid == USB_EID_DEFAULT) || (dir == USB_DIR_IN)) {
		dbg_dump((USB_ENDPADDR(dir, eid) |
			 ((0x01 & (__usbd_hw_endp_txrdy >> eid)) << 6) |
			 ((0x01 & (__usbd_hw_is_cso)) << 5) |
			 ((0x01 & (__raw_readw(USBTXIE) >> eid)) << 4)));
	} else {
		dbg_dump((USB_ENDPADDR(dir, eid) |
			 ((0x01 & (__usbd_hw_endp_txrdy >> eid)) << 6) |
			 ((0x01 & (__usbd_hw_is_cso)) << 5) |
			 ((0x01 & (__raw_readw(USBRXIE) >> eid)) << 4)));
	}
	dbg_dump(hint++);
	if (eid == USB_EID_DEFAULT) {
		dbg_dump(__raw_readb(USBCSRH0));
		dbg_dump(hint++);
		dbg_dump(__raw_readb(USBCSRL0));
	} else {
		if (dir == USB_DIR_IN) {
			dbg_dump(__raw_readb(USBTXCSRH(eid)));
			dbg_dump(hint++);
			dbg_dump(__raw_readb(USBTXCSRL(eid)));
		} else {
			dbg_dump(__raw_readb(USBRXCSRH(eid)));
			dbg_dump(hint++);
			dbg_dump(__raw_readb(USBRXCSRL(eid)));
		}
	}
	dbg_dump(hint++);
}
#endif

#ifdef CONFIG_PM
void usbd_hw_pm_suspend(void)
{
}

void usbd_hw_pm_resume(void)
{
}

void usbd_hw_pm_start(void)
{
	__usb_hw_irq_enable(DEVRESUME);
	__usb_hw_irq_enable(DEVSUSPEND);
}

void usbd_hw_pm_stop(void)
{
	__usb_hw_irq_disable(DEVRESUME);
	__usb_hw_irq_disable(DEVSUSPEND);
}
#endif

void usbd_hw_bus_detach(void)
{
	__usbd_hw_hub_disconnect();
}

void usbd_hw_bus_attach(void)
{
	__usbd_hw_hub_connect();
}

#ifdef CONFIG_USBD_SELF_POWERED
void usbd_hw_power_init(void)
{
	__usbd_hw_disable_vbus();
	/* TODO: more configuration for USBEPC */
	__raw_setb_atomic(EPENDE, USBEPC);
	__raw_setb_atomic(PFLTEN, USBEPC);
	__usb_hw_epc_irq_enable();
}
#else
void usbd_hw_power_init(void)
{
	__raw_clearb_atomic(PFLTEN, USBEPC);
	__raw_clearb_atomic(EPENDE, USBEPC);
	__usb_hw_epc_irq_disable();
}
#endif

void usbd_hw_ctrl_start(void)
{
	__usb_hw_irq_enable(DEVRESET);
	__usb_hw_irq_enable(DEVSOF);
	__usb_hw_irq_enable(DEVDISCON);
}

void usbd_hw_ctrl_stop(void)
{
	/* disable & clear all USB interrupt */
	__raw_writeb(0, USBIE);
	__usbd_hw_eirq_reset();
	__usbd_hw_dirq_reset();
#if 0
	__usb_hw_irq_disable(DEVRESET);
	__usb_hw_irq_disable(DEVSOF);
	__usb_hw_irq_disable(DEVDISCON);
#endif
}

void usbd_hw_ctrl_init(void)
{
	usbd_hw_power_init();

	/* PM reset device before mode switch */
	pm_hw_reset_device(DEV_USB0);
	/* enable required IRQs in attachment period */
	__usb_hw_switch_device();

	__usbd_hw_ctrl_restart();
 	__usbd_hw_is_cso = 0;
	__usbd_hw_endp_txrdy = 0;
}
