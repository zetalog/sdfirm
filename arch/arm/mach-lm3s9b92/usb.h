#ifndef __USB_LM3S9B92_PRIV_H_INCLUDE__
#define __USB_LM3S9B92_PRIV_H_INCLUDE__

#include "usb_otg.h"

#ifdef SYS_REALTIME
#define __usb_hw_irq_enable(irq)
#define __usb_hw_epc_irq_enable()
#define __usb_hw_epc_irq_disable()
#define __usb_hw_dr_irq_enable()
#define __usb_hw_dr_irq_disable()
#define usb_hw_irq_trigger()
#else
void usb_hw_irq_trigger(void);
#define __usb_hw_irq_enable(irq)	__raw_setb_atomic((irq), USBIE)
/* external power control */
#define __usb_hw_epc_irq_enable()	__raw_setb_atomic(PF, USBEPCIM)
#define __usb_hw_epc_irq_disable()	__raw_clearb_atomic(PF, USBEPCIM)
/* device resume */
#define __usb_hw_dr_irq_enable()	__raw_setb_atomic(RESUME, USBDRIM)
#define __usb_hw_dr_irq_disable()	__raw_clearb_atomic(RESUME, USBDRIM)
#endif

#define __usb_hw_irq_status()		__raw_readb(USBIS)
#define __usb_hw_dr_irq_status()	__raw_testb_atomic(RESUME, USBDRISC)

#define __usb_hw_switch_device()	__raw_writeb(__USB_HW_MODE_OTG | _BV(DEVMOD), USBGPCS)
#define __usb_hw_switch_host()		__raw_writeb(__USB_HW_MODE_OTG, USBGPCS)

/* functions exported by board.c for USBEPEN & USBPFLT pins configuration */
void usb_epc_pin_init(void);

/* USB0ID and USB0VBUS are configured by clearing the appropriate
 * DEN bit in the GPIODEN register
 */
/* configure GPIOB0 as analog USB0ID pin */
/* configure GPIOB1 as analog USB0VBUS pin */
#define usb_otg_pin_init()						\
	do {								\
		pm_hw_resume_device(DEV_GPIOB, DEV_MODE_ON);		\
		gpio_config_mux(GPIOB, 0, GPIOB0_MUX_USB0ID);		\
		gpio_config_pad(GPIOB, 0, GPIO_DIR_NONE,		\
				GPIO_PAD_ANALOG_IO, 8);			\
		gpio_config_mux(GPIOB, 1, GPIOB1_MUX_USB0VBUS);		\
		gpio_config_pad(GPIOB, 1, GPIO_DIR_NONE,		\
				GPIO_PAD_ANALOG_IO, 8);			\
	} while (0)

/* TODO: Dedicated Device Requires ID PIN Disabling
 *
 * Deconfigure GPIOB0 (USB0ID) as GPIO input pin, but this seemed useless.
 * Perhaps ID pin should only be disabled through board layout.
 * Don't use following macro unless you know something more than me.
 */
#define __usb_hw_disable_id()						\
	do {								\
		gpio_config_mux(GPIOB, 0, GPIO_MUX_NONE);		\
		gpio_config_pad(GPIOB, 0, GPIO_DIR_OUT,			\
				GPIO_PAD_PP, 2);			\
	} while (0)
/* Deconfigure GPIOB1 (USB0VBUS) as GPIO output pin, this will also
 * disable the D+/D- pull up resistors.
 */
#define __usb_hw_disable_vbus()						\
	do {								\
		gpio_config_mux(GPIOB, 1, GPIOB1_MUX_NONE);		\
		gpio_config_pad(GPIOB, 1, GPIO_DIR_OUT,			\
				GPIO_PAD_DIGITAL_IO, 2);		\
	} while (0)
/* Configure GPIOB1 as analog USB0VBUS pin. */
#define __usb_hw_enable_vbus()						\
	do {								\
		gpio_config_mux(GPIOB, 1, GPIOB1_MUX_USB0VBUS);		\
		gpio_config_pad(GPIOB, 1, GPIO_DIR_NONE,		\
				GPIO_PAD_ANALOG_IO, 8);		\
	} while (0)

#endif /* __USB_LM3S9B92_PRIV_H_INCLUDE__ */
