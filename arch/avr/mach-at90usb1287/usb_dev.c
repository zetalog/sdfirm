#include <target/usb.h>
#include <target/delay.h>

utb_text_size_t usbd_hw_endp_sizes[NR_USBD_ENDPS] = {
#if NR_USBD_ENDPS > 0
	64,
#endif
#if NR_USBD_ENDPS > 1
	128,	/* 8-bit utb_size_t and 2 banks */
#endif
#if NR_USBD_ENDPS > 2
	64,
#endif
#if NR_USBD_ENDPS > 3
	64,
#endif
#if NR_USBD_ENDPS > 4
	64,
#endif
#if NR_USBD_ENDPS > 5
	64,
#endif
#if NR_USBD_ENDPS > 6
	64,
#endif
};

#define USB_HW_EID_INTR_MIN	2
#define USB_HW_EID_INTR_MAX	6
#define USB_HW_EID_BULK_MIN	2
#define USB_HW_EID_BULK_MAX	6
#define USB_HW_EID_HSBULK	1

#define USB_HW_EID_UNRESERVED	1
#define USB_HW_EID_MAX		7

utb_size_t usbd_hw_endp_size(uint8_t addr)
{
	uint8_t eid = USB_ADDR2EID(addr);
	if (eid >= NR_USBD_ENDPS)
		return 0;
	return usbd_hw_endp_sizes[eid];
}

boolean usbd_hw_endp_caps(uint8_t addr, uint8_t type)
{
	uint8_t eid = USB_ADDR2EID(addr);

	if (eid >= NR_USBD_ENDPS)
		return false;
	if (type == USB_ENDP_CONTROL) {
		if (eid != USB_EID_DEFAULT)
			return false;
	}
	if (type == USB_ENDP_BULK) {
		if (eid < USB_HW_EID_BULK_MIN || eid > USB_HW_EID_BULK_MAX)
			return false;
	}
	if (type == USB_ENDP_INTERRUPT) {
		if (eid < USB_HW_EID_INTR_MIN || eid > USB_HW_EID_INTR_MAX)
			return false;
	}
	return true;
}

void usbd_hw_set_address(void)
{
	if (usbd_address == USB_ADDR_DEFAULT) {
		__usbd_hw_disable_address();
		__usbd_hw_clear_address();
	} else {
		__usbd_hw_set_address();
	}
	__usbd_hw_enable_address();
}

boolean __usbd_hw_is_txaval(void)
{
	if (usbd_endpoint_type() == USB_ENDP_CONTROL)
		return __usbd_hw_eirq_raised(TXIN);
	else
		return __usbd_hw_eirq_raised(RWAL);
}

boolean __usbd_hw_is_rxaval(void)
{
	if (usbd_endpoint_type() == USB_ENDP_CONTROL)
		return __usbd_hw_eirq_raised(RXSTP) ||
		       __usbd_hw_eirq_raised(RXOUT);
	else
		return __usbd_hw_eirq_raised(RWAL);
}

void usbd_hw_request_open(void)
{
	/* SETUP should always be enabled, since it would act as a control
	 * reset.
	 */
	if (usbd_endpoint_type() == USB_ENDP_CONTROL) {
		__usbd_hw_enable_eirq(RXSTP);
	}
	if (usbd_endpoint_type() != USB_ENDP_CONTROL ||
	    !usbd_control_setup_staging()) {
		/* Enabling TXIN/RXOUT except while the control setup
		 * stage.
		 */
		if (usbd_request_dir() == USB_DIR_IN) {
			__usbd_hw_enable_eirq(TXIN);
		} else {
			__usbd_hw_enable_eirq(RXOUT);
		}
	}
	__usbd_hw_enable_eirq(STALLED);
}

void usbd_hw_request_close(void)
{
	__usbd_hw_disable_eirq(STALLED);
	__usbd_hw_disable_eirq(RXOUT);
	__usbd_hw_disable_eirq(TXIN);
	__usbd_hw_disable_eirq(RXSTP);
}

void __usbd_hw_toggle_data(void)
{
	UECONX |= _BV(RSTDT);
}

void usbd_hw_request_reset(void)
{
	uint8_t eid;
	eid = USB_ADDR2EID(usbd_endp);
	UERST |=  _BV(eid);
	UERST &= ~_BV(eid);
	/* XXX: After Endpoint Reset
	 *
	 * The data toggle field remains unchanged.
	 * The other registers remain unchanged.
	 * The endpoint configuration remains active and the endpoint is
	 * still enabled.
	 */
	__usbd_hw_toggle_data();
}

void usbd_hw_endp_enable(void)
{
	UECONX = _BV(EPEN);
	UEIENX = 0;
	UECFG0X |= ((usbd_endpoint_type() << EPTYPE0) |
		    (usbd_endpoint_dir() << EPDIR) |
		    _BV(NYETDIS));
	UECFG1X = (_BV(ALLOC) | UECFG_64BYTES);
	while (!(UESTA0X & _BV(CFGOK)));
}

void usbd_hw_endp_unhalt(void)
{
	__usbd_hw_disable_stall();
	/* XXX: Data Toggle Reset
	 *
	 * According to 22.3 Endpoint reset
	 * The endpoint reset may be associated with a clear of
	 * the data toggle command (RSTDT bit) as an answer to the
	 * CLEAR_FEATURE USB command.
	 *
	 * But this has been done in the usbd_endpoint_unhalt().
	 */
}

void usbd_hw_endp_halt(void)
{
	__usbd_hw_enable_stall();
}

static utb_size_t usbd_hw_read_avail()
{
	if (usbd_endpoint_type() == USB_ENDP_CONTROL) {
		switch (usbd_control_get_stage()) {
		case USBD_CTRL_STAGE_STATUS:
			return 0;
		default:
			return __usbd_hw_read_avail();
		}
	} else {
		return __usbd_hw_read_avail();
	}
}

void usbd_hw_transfer_open(void)
{
	if (usbd_request_dir() == USB_DIR_IN) {
		if (usbd_request_syncing())
			while (!__usbd_hw_is_txaval());
	} else {
		if (usbd_request_syncing())
			while (!__usbd_hw_is_rxaval());
	}
	if (usbd_request_dir() == USB_DIR_OUT) {
		usbd_transfer_submit(usbd_hw_read_avail());
	}
	if (usbd_endpoint_type() != USB_ENDP_CONTROL) {
		if (usbd_request_dir() == USB_DIR_IN) {
			__usbd_hw_unraise_eirq(TXIN);
		} else {
			__usbd_hw_unraise_eirq(RXOUT);
		}
	}
}

void usbd_hw_transfer_close(void)
{
	if (usbd_endpoint_type() == USB_ENDP_CONTROL) {
		if (usbd_control_setup_staging()) {
			__usbd_hw_unraise_eirq(RXSTP);
		} else {
			if (usbd_request_dir() == USB_DIR_IN) {
				__usbd_hw_unraise_eirq(TXIN);
			} else {
				__usbd_hw_unraise_eirq(RXOUT);
			}
		}
	} else {
		/* The FIFOCON and RWAL fields are irrelevant with CONTROL
		 * endpoints. The firmware shall thus never use them on
		 * that endpoints.
		 */
		__usbd_hw_unraise_eirq(FIFOCON);
	}
}

void usbd_hw_handle_irq(void)
{
	if (__usbd_hw_irq_raised(EORST)) {
		usbd_bus_reset();
		__usbd_hw_unraise_irq(EORST);
	}
	if (__usbd_hw_irq_raised(SOF)) {
		__usbd_hw_unraise_irq(SOF);
		usbd_frame_start(__usbd_hw_frame_number());
	}
	if (__usbd_hw_irq_raised(SUSP)) {
		__usbd_hw_unraise_irq(SUSP);
		usbd_bus_suspend();
	}
	if (__usbd_hw_irq_raised(EORSM)) {
		__usbd_hw_unraise_irq(EORSM);
		usbd_bus_resume();
	}
	if (__usb_hw_irq_raised(IDT)) {
		__usb_hw_unraise_irq(IDT);
		/* The software anyway has to select the mode in order to
		 * access to the Device controller registers or to the
		 * Host controller registers, which are multiplexed.
		 */
		if (__usb_hw_id_is_device()) {
			usb_switch_mode(USB_MODE_DEVICE);
		} else {
			usb_switch_mode(USB_MODE_HOST);
		}
	}
	if (__usb_hw_irq_raised(VBUST)) {
		__usb_hw_unraise_irq(VBUST);

		if (__usb_hw_vbus_is_high()) {
			__usb_hw_unfreeze_clock();
			usbd_start();
		} else {
			usbd_stop();
			__usb_hw_freeze_clock();
		}
	}
}

static void usbd_hw_handle_rxout(void)
{
	if (usbd_endpoint_type() == USB_ENDP_CONTROL) {
		if (__usbd_hw_eirq_requested(RXSTP)) {
			usbd_control_setup();
		}
	}
	if (!usbd_request_interrupting(USB_DIR_OUT)) {
		return;
	}
	if (__usbd_hw_eirq_requested(STALLED)) {
		__usbd_hw_unraise_eirq(STALLED);
		usbd_request_stall();
	}
	if (__usbd_hw_eirq_requested(RXOUT)) {
		usbd_transfer_rxout();
	}
}

static void usbd_hw_handle_txin(void)
{
	if (!usbd_request_interrupting(USB_DIR_IN)) {
		return;
	}
	if (__usbd_hw_eirq_requested(STALLED)) {
		__usbd_hw_unraise_eirq(STALLED);
		usbd_request_stall();
	}
	if (__usbd_hw_eirq_requested(TXIN)) {
		usbd_transfer_txin();
	}
}

void usbd_hw_handle_eirq(void)
{
	uint8_t eid, saddr;

	for (eid = 0; eid < NR_USBD_HW_ENDPS; eid++) {
		if (__usbd_hw_eirq_raised_eid(eid)) {
			/* handle OUT events */
			saddr = usbd_addr_save(USB_ADDR(USB_DIR_OUT, eid));
			usbd_hw_handle_rxout();
			usbd_addr_restore(saddr);

			/* handle IN events */
			saddr = usbd_addr_save(USB_ADDR(USB_DIR_IN, eid));
			usbd_hw_handle_txin();
			usbd_addr_restore(saddr);
		}
	}
}

#ifdef CONFIG_PM
void usbd_hw_pm_resume(void)
{
}

void usbd_hw_pm_suspend(void)
{
}

void usbd_hw_pm_start(void)
{
	__usbd_hw_enable_irq(SUSP);
	__usbd_hw_enable_irq(EORSM);
}

void usbd_hw_pm_stop(void)
{
	__usbd_hw_disable_irq(SUSP);
	__usbd_hw_disable_irq(EORSM);
}
#endif

void usbd_hw_ctrl_start(void)
{
	__usbd_hw_enable_irq(EORST);
	__usbd_hw_enable_irq(SOF);
}

void usbd_hw_ctrl_stop(void)
{
	__usbd_hw_disable_irq(EORST);
	__usbd_hw_disable_irq(SOF);
}

void usbd_hw_ctrl_init(void)
{
	__usb_hw_select_device();
	__usb_hw_config_device();
}
