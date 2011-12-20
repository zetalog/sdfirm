#ifndef __USB_OTG_LM3S9B92_PRIV_H_INCLUDE__
#define __USB_OTG_LM3S9B92_PRIV_H_INCLUDE__

/* ID valid detect */
#define __otg_hw_id_irq_enable()		__raw_setb_atomic(ID, USBIDVIM)
#define __otg_hw_id_irq_disable()		__raw_clearb_atomic(ID, USBIDVIM)
#define __otg_hw_id_irq_status()		__raw_testb_atomic(ID, USBIDVISC)

#ifdef CONFIG_USB_OTG
#define __USB_HW_MODE_OTG			_BV(DEVMODOTG)
#define __usbd_hw_ctrl_restart()
#define __hcd_hw_ctrl_start()
#define __otg_hw_dev_start()			usbd_start()
#define __otg_hw_dev_stop()			usbd_stop()
#define __otg_hw_hcd_start()			hcd_start()
#define __usb_hw_irq_init()			\
	do {					\
		__usb_hw_dr_irq_enable();	\
		__otg_hw_id_irq_enable();	\
	} while (0)
#else
/* BUG: this controller cannot work as dedicated device... */
/* BUG: should we disable OTG mode... */
#define __USB_HW_MODE_OTG			_BV(DEVMODOTG)
#define __usbd_hw_ctrl_restart()		usbd_restart()
#define __hcd_hw_ctrl_start()			hcd_start()
#define __otg_hw_dev_start()
#define __otg_hw_dev_stop()
#define __otg_hw_hcd_start()
/* BUG: should we disable OTG ID IRQ... */
#define __usb_hw_irq_init()			\
	do {					\
		__usb_hw_dr_irq_enable();	\
		__otg_hw_id_irq_disable();	\
	} while (0)
#endif

#endif /* __USB_OTG_LM3S9B92_PRIV_H_INCLUDE__ */
