#include <target/usb.h>
#include <target/delay.h>
#include <target/io.h>
#include "usb.h"

/* Vbus droop control */
#define __hcd_hw_vdc_irq_enable()	__raw_setb_atomic(VD, USBVDCIM)
#define __hcd_hw_vdc_irq_disable()	__raw_clearb_atomic(VD, USBVDCIM)

void hcd_hw_ctrl_init(void)
{
	__usb_hw_switch_host();
}

void hcd_hw_handle_irq(void)
{
	uint8_t hcd_status = __usb_hw_irq_status();

	if (hcd_status & _BV(HCDVBUSERR)) {
		__usb_hw_vbus_error();
	}
	if (hcd_status & _BV(HCDCONN)) {
	}
	if (hcd_status & _BV(HCDDISCON)) {
	}
}

void hcd_hw_ctrl_start(void)
{
}

void hcd_hw_ctrl_init(void)
{
}
