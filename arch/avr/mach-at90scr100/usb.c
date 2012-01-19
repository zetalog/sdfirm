#include <target/usb.h>
#include <target/irq.h>
#include <target/delay.h>

/* TODO: Double buffering */
utb_text_size_t usbd_hw_endp_sizes[NR_USBD_ENDPS] = {
#if NR_USBD_ENDPS > 0
	64,
#endif
#if NR_USBD_ENDPS > 1
	64,	/* to allow 8bits utb_size_t */
#endif
#if NR_USBD_ENDPS > 2
	64,
#endif
#if NR_USBD_ENDPS > 3
	64,
#endif
#if NR_USBD_ENDPS > 4
	8,
#endif
#if NR_USBD_ENDPS > 5
	8,
#endif
#if NR_USBD_ENDPS > 6
	8,
#endif
#if NR_USBD_ENDPS > 7
	8,
#endif
};

#define USBD_DMA_SIZE	128
uint8_t usbd_dma_buffer[USBD_DMA_SIZE];
utb_size_t __usbd_hw_dma_count;

static void usbd_hw_dma_submit(void);
static void usbd_hw_dma_submit_rx(utb_size_t length);
static void usbd_hw_dma_reset(void);

#define USB_HW_EID_INTR_MIN	4
#define USB_HW_EID_INTR_MAX	6
#define USB_HW_EID_BULK_MIN	1
#define USB_HW_EID_BULK_MAX	3
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

void usbd_hw_set_config(void)
{
	if (usbd_config == USB_CONF_DEFAULT)
		__usbd_hw_state_leave(_BV(FCF));
	else
		__usbd_hw_state_enter(_BV(FCF));
}

void __usbd_hw_clear_address(void)
{
	__usbd_hw_state_leave(_BV(FAF));
	USBFA &= ~FADD_MASK;
}

void __usbd_hw_set_address(void)
{
	USBFA = usbd_address;
	__usbd_hw_state_enter(_BV(FAF));
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
	return !__usbd_hw_sta_raised(TXPB) || __usbd_hw_pirq_raised(FEURI);
}

boolean __usbd_hw_is_rxaval(void)
{
	if (__usbd_hw_pirq_raised(FEURI))
		return true;

	if (usbd_endpoint_type() == USB_ENDP_CONTROL)
		return __usbd_hw_sta_raised(RXSETUP) ||
		       __usbd_hw_sta_raised(RCVD);
	else
		return __usbd_hw_sta_raised(RCVD);
}

void usbd_hw_request_open(void)
{
	usbd_hw_dma_reset();
	__usbd_hw_enable_eirq(USB_ADDR2EID(usbd_endp));
}

void usbd_hw_request_close(void)
{
	__usbd_hw_disable_eirq(USB_ADDR2EID(usbd_endp));
}

void usbd_hw_endp_enable(void)
{
	USBFCEX = _BV(EPE) |
		  (usbd_endpoint_type()) |
		  (usbd_endpoint_dir() << EPDIR);
}

void usbd_hw_request_reset(void)
{
	uint8_t eid = USB_ADDR2EID(usbd_endp);
	USBRSTE &= ~(_BV(eid));
	USBRSTE |=  (_BV(eid));
	while (USBRSTE & (_BV(eid)));
	USBCSEX = 0x00;
}

static utb_size_t usbd_hw_read_avail()
{
	/* XXX: RXSTP Interrupt Comes Right After RXOUT Interrupt
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

uint8_t usbd_hw_read_byte(void)
{
	BUG_ON(__usbd_hw_dma_count >= USBD_DMA_SIZE);
	return usbd_dma_buffer[__usbd_hw_dma_count++];
}

void usbd_hw_write_byte(uint8_t byte)
{
	BUG_ON(__usbd_hw_dma_count >= USBD_DMA_SIZE);
	usbd_dma_buffer[__usbd_hw_dma_count++] = byte;
}

void usbd_hw_transfer_open(void)
{
	if (usbd_request_dir() == USB_DIR_IN) {
		if (usbd_request_syncing())
			while (!__usbd_hw_is_txaval());
	} else {
		if (usbd_request_syncing()) {
			while (!__usbd_hw_is_rxaval());
		}
		usbd_hw_dma_submit_rx(usbd_hw_read_avail());
		usbd_transfer_submit(usbd_hw_read_avail());
	}
}

void usbd_hw_transfer_close(void)
{
	if (usbd_request_dir() == USB_DIR_IN) {
		while (!__usbd_hw_is_txaval());
		__usbd_hw_unraise_sta(TXC);
		usbd_hw_dma_submit();
		__usbd_hw_raise_sta(TXPB);
	} else {
		if (usbd_control_setup_staging() && 
		    __usbd_hw_sta_raised(RXSETUP)) {
			__usbd_hw_unraise_sta(RXSETUP);
		}
		__usbd_hw_unraise_sta(RCVD);
	}
}

static void usbd_hw_handle_irq(void)
{
	if (__usbd_hw_pirq_raised(FEURI)) {
		usbd_bus_reset();
		__usbd_hw_unraise_pirq(FEURI);
	}
	if (__usbd_hw_pirq_raised(SOFI)) {
		__usbd_hw_unraise_pirq(SOFI);
		usbd_frame_start(__usbd_hw_frame_number());
	}
	if (__usbd_hw_pirq_raised(SUSI)) {
		__usbd_hw_unraise_pirq(SUSI);
		usbd_bus_suspend();
	}
	if (__usbd_hw_pirq_raised(RESI)) {
		usbd_bus_resume();
		__usbd_hw_unraise_pirq(RESI);
	}
}

DEFINE_ISR(IRQ_USB)
{
	usbd_hw_handle_irq();
}

static void usbd_hw_handle_rxout(void)
{
	if (usbd_endpoint_type() == USB_ENDP_CONTROL) {
		if (__usbd_hw_eirq_requested(RXSETUP)) {
			usbd_control_setup();
		}
	}
	if (!usbd_request_interrupting(USB_DIR_OUT))
		return;
	if (__usbd_hw_eirq_requested(STSENT)) {
		__usbd_hw_sta_reply(STSENT);
		usbd_request_stall();
	}
	if (__usbd_hw_eirq_requested(RCVD)) {
		usbd_transfer_rxout();
	}
}

static void usbd_hw_handle_txin(void)
{
	if (!usbd_request_interrupting(USB_DIR_IN))
		return;
	if (__usbd_hw_eirq_requested(STSENT)) {
		__usbd_hw_sta_reply(STSENT);
		usbd_request_stall();
	}
	if (__usbd_hw_eirq_requested(TXC)) {
		__usbd_hw_unraise_sta(TXC);
		usbd_transfer_txin();
	}
}

static void usbd_hw_handle_eirq(void)
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

DEFINE_ISR(IRQ_USBT)
{
	usbd_hw_handle_eirq();
}

/* RX(TX) data to(from) ram */
static void usbd_hw_dma_submit(void)
{
	USBDMAB = __usbd_hw_dma_count;

	/* set address */
	USBDMADH = HIBYTE((uint16_t)usbd_dma_buffer);
	USBDMADL = LOBYTE((uint16_t)usbd_dma_buffer);

	USBDMACS = (USB_ADDR2EID(usbd_endp) << EPS_OFFSET);
	if (usbd_request_dir() == USB_DIR_IN) {
		USBDMACS |= _BV(USBDMADIR);
	} else {
		USBDMACS &= ~_BV(USBDMADIR);
	}
	/* Start to run DMA. This action will stop CPU. */
	USBDMACS |= _BV(USBDMAR);

	usbd_hw_dma_reset();
}

static void usbd_hw_dma_submit_rx(utb_size_t length)
{
	__usbd_hw_dma_count = length;
	usbd_hw_dma_submit();
}

static void usbd_hw_dma_reset(void)
{
	__usbd_hw_dma_count = 0;
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
	__usbd_hw_enable_pirq(SUSI);
	__usbd_hw_enable_pirq(RESI);
}

void usbd_hw_pm_stop(void)
{
	__usbd_hw_disable_pirq(SUSI);
	__usbd_hw_disable_pirq(RESI);
}
#endif

#ifdef SYS_REALTIME
void usb_hw_irq_poll(void)
{
	usbd_hw_handle_irq();
	usbd_hw_handle_eirq();
}
#else
void usb_hw_irq_init(void)
{
	irq_register_vector(IRQ_USB, IRQ_USB_isr);
	irq_register_vector(IRQ_USBT, IRQ_USBT_isr);
}
#endif

void usbd_hw_ctrl_start(void)
{
	/* FEURI is an unmaskable interrupt */
	__usbd_hw_enable_pirq(FEURI);
	__usbd_hw_enable_pirq(SOFI);
}

void usbd_hw_ctrl_stop(void)
{
	/* FEURI is an unmaskable interrupt */
	__usbd_hw_disable_pirq(FEURI);
	__usbd_hw_disable_pirq(SOFI);
}

void usbd_hw_ctrl_init(void)
{
	/* FIXME: Interrupt Still Arrives if USBPIM=0/USBEIM=0
	 * Why USB device controller still generate pirqs/eirqs after we
	 * masked all of them?  Perhaps we should set
	 * USBPIM=0xFF/USBEIM=0xFF?
	 */
#if 0
	/* disable all pirq */
	USBPIM = 0x00;
	/* disable all eirq */
	USBEIM = 0;
	USBPI  = 0x00;
	USBEI  = 0;
#endif
	/* usbd_start here since this is not an OTG device */
	usbd_start();
}

void usb_hw_ctrl_init(void)
{
	/* clearing USBE acts as an hardware reset */
	__usb_hw_disable_macro();
	__usb_hw_enable_macro();
}
