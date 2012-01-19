#include <target/usb.h>
#include <target/delay.h>

static boolean __uep_hw_is_rxaval(void);

/* TODO:
 * 1. bus suspend/resume support
 * 2. high speed bulk transfer support
 */

utb_text_size_t usbd_hw_endp_sizes[NR_USBD_ENDPS] = {
#if NR_USBD_ENDPS > 0
	32,
#endif
#if NR_USBD_ENDPS > 1
	8,
#endif
#if NR_USBD_ENDPS > 2
	8,
#endif
#if NR_USBD_ENDPS > 3
	8,
#endif
#if NR_USBD_ENDPS > 4
	64,
#endif
#if NR_USBD_ENDPS > 5
	64,
#endif
#if NR_USBD_ENDPS > 6
	2 * 64,
#endif
};

#define USB_HW_EID_INTR_MIN	1
#define USB_HW_EID_INTR_MAX	3
#define USB_HW_EID_BULK_MIN	4
#define USB_HW_EID_BULK_MAX	5
#define USB_HW_EID_HSBULK	6

#define USB_HW_EID_UNRESERVED	1
#define USB_HW_EID_MAX		7

#define __usbd_hw_read_avail()			UBYCTX

static utb_size_t usbd_hw_read_avail()
{
	/*
	 * XXX: RXSTP Interrupt Comes Right After RXOUT Interrupt
	 *
	 * This hardware will report RXOUT before RXSTP.  But the
	 * reported available transfer size looks like that the FIFO has
	 * contained a SETUP request, while 0 is expected to be returned.
	 */
	if (usbd_endpoint_type() == USB_ENDP_CONTROL) {
		switch (usbd_control_get_stage()) {
		case USBD_CTRL_STAGE_STATUS:
			return 0;
		case USBD_CTRL_STAGE_SETUP:
			return USB_SETUP_REQUEST_SIZE;
		default:
			return __usbd_hw_read_avail();
		}
	} else {
		return __usbd_hw_read_avail();
	}
}

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

void usbd_hw_set_config(void)
{
	if (usbd_config == USB_CONF_DEFAULT)
		__usbd_hw_state_leave(USBCON_CONFIG);
	else
		__usbd_hw_state_enter(USBCON_CONFIG);
}

void usbd_hw_set_address(void)
{
	USBADDR = (usbd_address | USBADDR_FEN);
	if (usbd_address == USB_ADDR_DEFAULT) {
		__usbd_hw_state_leave(USBCON_ADDRESS);
	} else {
		__usbd_hw_state_enter(USBCON_ADDRESS);
	}
}

void usbd_hw_endp_enable(void)
{
	UEPCONX = (_BV(EPEN) |
		   (usbd_endpoint_type() << EPTYPE) |
		   (usbd_endpoint_dir() << EPDIR));
}

void usbd_hw_request_reset(void)
{
	uint8_t eid = USB_ADDR2EID(usbd_endp);
	UEPRST |= (1 << eid);
	UEPRST &= ~(1 << eid);
}

void usbd_hw_endp_halt(void)
{
	__uep_hw_sta_raise(UEPSTA_STALLRQ);
}

void usbd_hw_endp_unhalt(void)
{
	__uep_hw_sta_unraise(UEPSTA_STALLRQ);
}

#if 0
static boolean __usbd_hw_request_retired(void)
{
	if (usbd_endpoint_type() == USB_ENDP_CONTROL &&
	    !usbd_control_setup_staging() &&
	    __uep_hw_irq_requested(UEPSTA_RXSETUP)) {
		return true;
	}
	return __usbd_hw_irq_raised(USBINT_EORINT) ||
	       __uep_hw_irq_requested(UEPSTA_STLCRC);
}
#endif

static boolean __uep_hw_is_rxaval(void)
{
	if (usbd_endpoint_type() == USB_ENDP_CONTROL)
		return __uep_hw_sta_raised(UEPSTA_RXSETUP) || 
		       __uep_hw_sta_raised(UEPSTA_RXOUTB0);
	else
		return __uep_hw_sta_raised(UEPSTA_RXOUTB0);
}

static boolean __uep_hw_is_txaval(void)
{
	/* XXX: Register Description in Data Sheet
	 * Data should be written into the endpoint FIFO only after TXRDY
	 * bit has been cleared.
	 */
	/* XXX: Register Description in Data Sheet
	 * TXCMPL bit should be cleared by the device firmware before
	 * setting TXRDY.
	 */
	return !__uep_hw_sta_raised(UEPSTA_TXRDY) &&
	       !__uep_hw_sta_raised(UEPSTA_TXCMPL);
}

static boolean __uep_hw_is_txcmpl(void)
{
	return __uep_hw_sta_raised(UEPSTA_TXCMPL);
}

void usbd_hw_request_open(void)
{
	__uep_hw_irq_enable(USB_ADDR2EID(usbd_endp));
	if (usbd_endpoint_type() == USB_ENDP_CONTROL) {
		if (usbd_request_dir() == USB_DIR_IN)
			__uep_hw_request_in();
		else
			__uep_hw_request_out();
	}
}

void usbd_hw_request_close(void)
{
	__uep_hw_irq_disable(USB_ADDR2EID(usbd_endp));
}

void usbd_hw_transfer_open(void)
{
	if (usbd_request_dir() == USB_DIR_IN) {
		if (usbd_request_syncing()) {
			while (!__uep_hw_is_txaval());
		}
	} else {
		if (usbd_request_syncing()) {
			while (!__uep_hw_is_rxaval());
		}
		usbd_transfer_submit(usbd_hw_read_avail());
	}
}

void usbd_hw_transfer_close(void)
{
	if (usbd_request_dir() == USB_DIR_IN) {
		__uep_hw_sta_raise(UEPSTA_TXRDY);
		if (usbd_request_syncing()) {
			while (!__uep_hw_is_txcmpl());
			__uep_hw_sta_unraise(UEPSTA_TXCMPL);
		}
	} else {
		/* XXX: Register Description in Data Sheet
		 * RXOUTB0 bit should be cleared by the device firmware
		 * after reading the OUT data from the endpoint FIFO.
		 *
		 * RXSETUP bit is set by hardware when a valid SETUP
		 * packet has been received from the host. Then, all the
		 * other bits (including RXOUTB0) of the register are
		 * cleared by hardware.
		 * For control endpoints, an early SETUP transaction may
		 * overwrite the content of the endpoint FIFO, even if its
		 * Data packet is received while RXOUTB0 bit is set.
		 */
		if (__uep_hw_sta_raised(UEPSTA_RXOUTB0) &&
		    !__uep_hw_sta_raised(UEPSTA_RXSETUP)) {
			__uep_hw_sta_unraise(UEPSTA_RXOUTB0);
		}
		if (usbd_control_setup_staging() &&
		    __uep_hw_sta_raised(UEPSTA_RXSETUP)) {
			/* XXX: Register Description in Data Sheet
			 * RXSETUP should be cleared by the device
			 * firmware after reading the SETUP data from the
			 * endpoint FIFO.
			 */
			__uep_hw_sta_unraise(UEPSTA_RXSETUP);
		}
	}
}

static void usbd_hw_handle_rxout(void)
{
	if (usbd_endpoint_type() == USB_ENDP_CONTROL) {
		if (__uep_hw_irq_requested(UEPSTA_RXSETUP)) {
			usbd_control_setup();
		}
	}
	if (!usbd_request_interrupting(USB_DIR_OUT))
		return;
	if (__uep_hw_irq_requested(UEPSTA_STLCRC)) {
		__uep_hw_sta_unraise(UEPSTA_STLCRC);
		usbd_request_stall();
	}
	if (__uep_hw_irq_requested(UEPSTA_RXOUTB0)) {
		usbd_transfer_rxout();
	}
#if 0
	if (__uep_hw_irq_requested(UEPSTA_RXOUTB1)) {
		__uep_hw_sta_unraise(UEPSTA_RXOUTB1);
		usbd_transfer_rxout();
	}
#endif
}

static void usbd_hw_handle_txin(void)
{
	if (!usbd_request_interrupting(USB_DIR_IN))
		return;
	if (__uep_hw_irq_requested(UEPSTA_STLCRC)) {
		__uep_hw_sta_unraise(UEPSTA_STLCRC);
		usbd_request_stall();
	}
	if (__uep_hw_irq_requested(UEPSTA_TXCMPL)) {
		__uep_hw_sta_unraise(UEPSTA_TXCMPL);
		usbd_transfer_txin();
	}
}

#include <target/uart.h>

static void usbd_hw_handle_irq(void)
{
	uint8_t eid, saddr;

	for (eid = 0; eid < NR_USBD_HW_ENDPS; eid++) {
		if (__uep_hw_irq_raised(eid)) {
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

	if (__usbd_hw_irq_raised(USBINT_EORINT)) {
		usbd_bus_reset();
		__usbd_hw_irq_unraise(USBINT_EORINT);
	}
	if (__usbd_hw_irq_raised(USBINT_SOFINT)) {
		__usbd_hw_irq_unraise(USBINT_SOFINT);
		usbd_frame_start(__usbd_hw_frame_number());
	}
	if (__usbd_hw_irq_raised(USBINT_WUPCPU)) {
		usbd_bus_resume();
		__usbd_hw_irq_unraise(USBINT_WUPCPU);
	}
	if (__usbd_hw_irq_raised(USBINT_SPINT)) {
		/* This bit must be cleared by software 
		 * before powering the microcontroller down
		 */
		__usbd_hw_irq_unraise(USBINT_SPINT);
		usbd_bus_suspend();
	}
}

void DEFINE_ISR(usb_isr(void), IRQ_USB)
{
	usbd_hw_handle_irq();
}

#ifdef CONFIG_PM
/* XXX: USB Macro Issue - Chip Bug
 * For usb macro V1.8RevE2 (Hammer Shark) the WUPCPU bit may not be properly
 * cleared by the CPU if its clock frequency is greater or equal than 
 * 12 Mhz/X2.
 * Workaround: set the cpu clock to frequency strictly lower than 12 Mhz/X2
 * before clearing this bit. The cpu clock frequency can be restored to its
 * initial value after the bit is cleared.
 */
void usbd_hw_pm_resume(void)
{
	__usbd_hw_wakeup_clock();
	irq_local_disable();
	clk_hw_suspend_cpu();
	__usbd_hw_irq_unraise(USBINT_WUPCPU);
	clk_hw_resume_cpu();
	irq_local_enable();
}

void usbd_hw_pm_suspend(void)
{
	__usbd_hw_irq_unraise(USBINT_SPINT);
	__usbd_hw_suspend_clock();
}

void usbd_hw_pm_start(void)
{
	__usbd_hw_irq_enable(USBINT_SPINT);
	__usbd_hw_irq_enable(USBINT_WUPCPU);
}

void usbd_hw_pm_stop(void)
{
	__usbd_hw_irq_disable(USBINT_SPINT);
	__usbd_hw_irq_disable(USBINT_WUPCPU);
}
#endif

#ifdef SYS_REALTIME
void usb_hw_irq_poll(void)
{
	usbd_hw_handle_irq();
}
#else
void usb_hw_irq_init(void)
{
	irq_hw_set_priority(IRQ_USB, IRQ_PRIO_1);
	irq_hw_enable_vector(IRQ_USB, true);
}
#endif

void usbd_hw_ctrl_start(void)
{
	__usbd_hw_irq_enable(USBINT_EORINT);
	__usbd_hw_irq_enable(USBINT_SOFINT);
}

void usbd_hw_ctrl_stop(void)
{
	__usbd_hw_irq_disable(USBINT_EORINT);
	__usbd_hw_irq_disable(USBINT_SOFINT);
}

void usbd_hw_ctrl_init(void)
{
	/* enable hardware */
	__usbd_hw_enable();

	/* detach is also done in the board_early_init */
	usbd_restart();
}

void usb_hw_ctrl_init(void)
{
	/* config clock */
	clk_hw_enable_usb();
}
