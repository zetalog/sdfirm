#include "mach.h"
#include <target/usb.h>

/* enabling context */
uint8_t usbd_hw_eid = 0x00;
uint8_t usbd_hw_dir = 0x00;
uint8_t usbd_hw_addr = 0x00;
uint8_t usbd_hw_conf = 0;
uint16_t usbd_hw_frame = 0;

void usbd_hw_enable_dirq(uint8_t irq)
{
	sim_usbd_enable_dirq(irq);
}

void usbd_hw_disable_dirq(uint8_t irq)
{
	sim_usbd_disable_dirq(irq);
}

boolean usbd_hw_endp_caps(uint8_t addr, uint8_t type)
{
	if (type == USB_ENDP_ISOCHRONOUS)
		return false;
	return true;
}

utb_size_t usbd_hw_endp_size(uint8_t addr)
{
	return SIM_USB_SIZE;
}

void usbd_hw_endp_enable(void)
{
	sim_usbd_enable_fifo(usbd_endpoint_dir(), usbd_hw_eid, usbd_endpoint_type());
}

void usbd_hw_endp_disable(void)
{
	if (usbd_endpoint_type() == USB_ENDP_CONTROL)
		sim_usbd_disable_fifo(USB_DIR_IN, usbd_hw_eid);
	sim_usbd_disable_fifo(usbd_endpoint_dir(), usbd_hw_eid);
}

void usbd_hw_endp_halt(void)
{
	sim_usbd_raise_stall(usbd_hw_dir, usbd_hw_eid);
}

void usbd_hw_endp_unhalt(void)
{
	sim_usbd_unraise_stall(usbd_hw_dir, usbd_hw_eid);
}

void usbd_hw_endp_select(uint8_t addr)
{
	usbd_hw_eid = USB_ADDR2EID(addr);
	usbd_hw_dir = USB_ADDR2EID(addr);
}

uint8_t usbd_hw_read_byte(void)
{
	return sim_usb_read_fifo(USB_DIR_OUT, usbd_hw_eid);
}

void usbd_hw_write_byte(uint8_t byte)
{
	sim_usb_write_fifo(USB_DIR_IN, usbd_hw_eid, byte);
}

static utb_size_t usbd_hw_read_avail()
{
	return sim_usbd_fifo_count(USB_DIR_OUT, usbd_hw_eid);
}

boolean __usbd_hw_is_txaval(void)
{
	return sim_usbd_eirq_raised(usbd_hw_eid, USBD_IRQ_IN);
}

boolean __usbd_hw_is_rxaval(void)
{
	if (usbd_endpoint_type() == USB_ENDP_CONTROL)
		return sim_usbd_eirq_raised(usbd_hw_eid, USBD_IRQ_SETUP) ||
		       sim_usbd_eirq_raised(usbd_hw_eid, USBD_IRQ_OUT);
	else
		return sim_usbd_eirq_raised(usbd_hw_eid, USBD_IRQ_IN);
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
}

void usbd_hw_transfer_close(void)
{
	if (usbd_control_setup_staging()) {
		sim_usbd_unraise_eirq(usbd_hw_eid, USBD_IRQ_SETUP);
	} else {
		if (usbd_request_dir() == USB_DIR_IN) {
			sim_usbd_unraise_eirq(usbd_hw_eid, USBD_IRQ_IN);
		} else {
			sim_usbd_unraise_eirq(usbd_hw_eid, USBD_IRQ_OUT);
		}
	}
}

void usbd_hw_set_address(void)
{
	usbd_hw_addr = usbd_address;
}

void usbd_hw_set_config(void)
{
	usbd_hw_conf = usbd_config;
}

void usbd_hw_request_open(void)
{
	sim_usbd_enable_eirq(usbd_request_dir(), usbd_hw_eid);
}

void usbd_hw_request_close(void)
{
	sim_usbd_disable_eirq(USB_DIR_OUT, usbd_hw_eid);
	sim_usbd_disable_eirq(USB_DIR_IN, usbd_hw_eid);
}

uint16_t usbd_hw_current_frame(void)
{
	return usbd_hw_frame;
}

void usbd_hw_request_reset(void)
{
	sim_usbd_reset_fifo(usbd_request_dir(), usbd_hw_eid);
}

static void usbd_hw_handle_rxout(void)
{
	if (usbd_hw_eid == USB_EID_DEFAULT)
		printf("OUT\n");
	if (usbd_endpoint_type() == USB_ENDP_CONTROL) {
		if (sim_usbd_eirq_raised(usbd_hw_eid, USBD_IRQ_SETUP)) {
			usbd_control_setup();
		}
	}
	if (!usbd_request_interrupting(USB_DIR_OUT))
		return;
	if (sim_usbd_stall_raised(USB_DIR_OUT, usbd_hw_eid)) {
		sim_usbd_unraise_stall(USB_DIR_OUT, usbd_hw_eid);
		usbd_request_stall();
	}
	if (sim_usbd_eirq_raised(usbd_hw_eid, USBD_IRQ_OUT)) {
		usbd_transfer_rxout();
	}
}

static void usbd_hw_handle_txin(void)
{
	if (usbd_hw_eid == USB_EID_DEFAULT)
		printf("IN\n");
	if (!usbd_request_interrupting(USB_DIR_IN))
		return;
	if (sim_usbd_stall_raised(USB_DIR_IN, usbd_hw_eid)) {
		sim_usbd_unraise_stall(USB_DIR_IN, usbd_hw_eid);
		usbd_request_stall();
	}
	if (sim_usbd_eirq_raised(usbd_hw_eid, USBD_IRQ_IN)) {
		usbd_transfer_txin();
	}
}

void usb_hw_handle_irq(void)
{
	uint8_t eid, saddr;

	if (sim_usbd_dirq_raised(USBD_IRQ_RESET)) {
		usbd_bus_reset();
		sim_usbd_unraise_dirq(USBD_IRQ_RESET);
	}
	if (sim_usbd_dirq_raised(USBD_IRQ_SOF)) {
		sim_usbd_unraise_dirq(USBD_IRQ_SOF);
		usbd_frame_start(usbd_hw_current_frame());
	}
	if (sim_usbd_dirq_raised(USBD_IRQ_SUSPEND)) {
		sim_usbd_unraise_dirq(USBD_IRQ_SUSPEND);
		usbd_bus_suspend();
	}
	if (sim_usbd_dirq_raised(USBD_IRQ_RESUME)) {
		usbd_bus_resume();
		sim_usbd_unraise_dirq(USBD_IRQ_RESUME);
	}

	for (eid = 0; eid < NR_USBD_HW_ENDPS; eid++) {
		if (sim_usbd_eirq_raised_any(eid)) {
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

#ifdef SYS_REALTIME
void usb_hw_irq_poll(void)
{
	usb_hw_handle_irq();
}
#else
void usb_hw_irq_init(void)
{
	irq_hw_register_irq(IRQ_USB, usb_hw_handle_irq);
	irq_hw_irq_enable(IRQ_USB);
}
#endif

#ifdef CONFIG_PM
void usbd_hw_pm_resume(void)
{
}

void usbd_hw_pm_suspend(void)
{
}

void usbd_hw_pm_start(void)
{
	usbd_hw_enable_dirq(USBD_IRQ_SUSPEND);
	usbd_hw_enable_dirq(USBD_IRQ_RESUME);
}

void usbd_hw_pm_stop(void)
{
	usbd_hw_disable_dirq(USBD_IRQ_SUSPEND);
	usbd_hw_disable_dirq(USBD_IRQ_RESUME);
}
#endif

void usbd_hw_ctrl_init(void)
{
	usbd_start();
}

void usbd_hw_ctrl_start(void)
{
	usbd_hw_enable_dirq(USBD_IRQ_RESET);
	usbd_hw_enable_dirq(USBD_IRQ_SOF);
}

void usbd_hw_ctrl_stop(void)
{
	usbd_hw_disable_dirq(USBD_IRQ_RESET);
	usbd_hw_disable_dirq(USBD_IRQ_SOF);
}

void usbd_hw_bus_attach(void)
{
}

void usbd_hw_bus_detach(void)
{
}

void usb_hw_ctrl_init(void)
{
}
