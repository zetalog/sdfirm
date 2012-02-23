#include <target/usb.h>
#include <target/delay.h>
#include <target/io.h>
#include "usb.h"

#ifdef CONFIG_USB_DEV
void usbd_hw_handle_irq(void);
#else
#define usbd_hw_handle_irq()
#endif

#ifdef CONFIG_USB_HCD
void hcd_hw_handle_irq(void);
#else
#define hcd_hw_handle_irq()
#endif

static void __usb_hw_handle_irq(void)
{
	if (usb_get_mode() == USB_MODE_DEVICE) {
		usbd_hw_handle_irq();
	} else {
		hcd_hw_handle_irq();
	}
}

#ifdef SYS_REALTIME
void usb_hw_irq_poll(void)
{
	__usb_hw_handle_irq();
}
#else
void usb_hw_irq_trigger(void)
{
	vic_hw_irq_trigger(IRQ_USB);
}

void usb_hw_irq_init(void)
{
	vic_hw_register_irq(IRQ_USB, __usb_hw_handle_irq);
	vic_hw_irq_enable(IRQ_USB);
}
#endif

void usb_hw_ctrl_init(void)
{
	usb_epc_pin_init();
	//usb_otg_pin_init();

	pm_hw_resume_device(DEV_USB0, DEV_MODE_ON);
	/* ensure USB clock is sourced */
	clk_hw_usb_enable();
	__usb_hw_irq_init();
}
