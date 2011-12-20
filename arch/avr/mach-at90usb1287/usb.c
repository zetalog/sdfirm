#include <target/usb.h>
#include <target/irq.h>
#include <target/delay.h>

#ifdef CONFIG_USB_DEV
void usbd_hw_handle_irq(void);
void usbd_hw_handle_eirq(void);
#else
#define usbd_hw_handle_irq()
#define usbd_hw_handle_eirq()
#endif

#ifdef CONFIG_USB_HCD
void hcd_hw_handle_irq(void);
void hcd_hw_handle_pirq(void);
#else
#define hcd_hw_handle_irq()
#define hcd_hw_handle_pirq()
#endif

static void usb_hw_handle_irq(void)
{
	if (usb_get_mode() == USB_MODE_DEVICE) {
		usbd_hw_handle_irq();
	} else {
		hcd_hw_handle_irq();
	}
}

static void usb_hw_handle_eirq(void)
{
	if (usb_get_mode() == USB_MODE_DEVICE) {
		usbd_hw_handle_eirq();
	} else {
		hcd_hw_handle_pirq();
	}
}

DEFINE_ISR(IRQ_USB)
{
	usb_hw_handle_irq();
}

DEFINE_ISR(IRQ_USBT)
{
	usb_hw_handle_eirq();
}

#ifdef SYS_REALTIME
void usb_hw_irq_poll(void)
{
	usb_hw_handle_irq();
	usb_hw_handle_eirq();
}
#else
void usb_hw_irq_init(void)
{
	irq_register_vector(IRQ_USB, IRQ_USB_isr);
	irq_register_vector(IRQ_USBT, IRQ_USBT_isr);
}
#endif

void usb_hw_ctrl_init(void)
{
	/* config mode selection basis and pad regulator */
	__usb_hw_enable_mode();

	/* enable USB clock */
	clk_hw_set_usb();

	/* clearing USBE acts as an hardware reset */
	__usb_hw_disable_macro();
	__usb_hw_enable_macro();
	__usb_hw_config_power();

	/* enable ID transition detection */
	__usb_hw_enable_irq(IDT);
	__usb_hw_enable_irq(VBUST);
}
