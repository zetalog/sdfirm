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

/* USB External Power Control PINs configuration is only known by the
 * board designer
 */
void usb_epc_pin_init(void)
{
	pm_hw_resume_device(DEV_GPIOH, DEV_MODE_ON);
	/* PH3 USBEPEN */
	gpio_config_mux(GPIOH, 3, GPIOH3_MUX_USB0EPEN);
	gpio_config_pad(GPIOH, 3, GPIO_DIR_NONE,
			GPIO_PAD_PP, 8);
	/* PH4 USBPFLT */
	gpio_config_mux(GPIOH, 4, GPIOH4_MUX_USB0PFLT);
	gpio_config_pad(GPIOH, 4, GPIO_DIR_NONE,
			GPIO_PAD_PP, 8);
}

#if 0
void usb_epc_pin_exit(void)
{
	/* PH3 USBEPEN */
	gpio_config_mux(GPIOH, 3, GPIO_MUX_NONE);
	gpio_config_pad(GPIOH, 3, GPIO_DIR_OUT,
			GPIO_PAD_PP, 8);
	/* PH4 USBPFLT */
	gpio_config_mux(GPIOH, 4, GPIO_MUX_NONE);
	gpio_config_pad(GPIOH, 4, GPIO_DIR_IN,
			GPIO_PAD_PP, 8);
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
