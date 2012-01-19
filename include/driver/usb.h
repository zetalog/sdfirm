#ifndef __USB_DRIVER_H_INCLUDE__
#define __USB_DRIVER_H_INCLUDE__

#include <target/config.h>

typedef uint8_t usb_cid_t;
typedef uint8_t usb_iid_t;
typedef uint8_t usb_addr_t;

#ifdef CONFIG_USBD_XFR_MASS
typedef uint32_t urb_size_t;
#else
typedef uint16_t urb_size_t;
#endif

#ifdef CONFIG_ARCH_HAS_USB
#include <asm/mach/usb.h>
#endif

#ifndef ARCH_HAVE_USB
#define usb_hw_ctrl_init()

typedef uint8_t utb_size_t;
#define utb_text_size_t text_byte_t

#define NR_USBD_HW_ENDPS	0
#define USBD_HW_CTRL_SIZE	64

/* called when USB control is switched to device mode */
#define usbd_hw_ctrl_init()
#define usbd_hw_ctrl_start()
#define usbd_hw_endp_caps(addr, type)		(false)
#define usbd_hw_endp_size(addr)			(0)

#define usbd_hw_endp_enable()
#define usbd_hw_endp_unhalt()
#define usbd_hw_endp_halt()
#define usbd_hw_endp_select(addr)

#define usbd_hw_read_byte()			(0)
#define usbd_hw_write_byte(b)
#define usbd_hw_transfer_open()
#define usbd_hw_transfer_close()
#define usbd_hw_request_open()
#define usbd_hw_request_close()
#define usbd_hw_request_reset()

#define usbd_hw_set_address()
#define usbd_hw_set_config()
#endif

/* following interfaces are default to empty according to settings */
#if defined(ARCH_HAVE_USB) && (CONFIG_PM)
void usbd_hw_pm_suspend(void);
void usbd_hw_pm_resume(void);
void usbd_hw_pm_start(void);
void usbd_hw_pm_stop(void);
#else
#define usbd_hw_pm_suspend()
#define usbd_hw_pm_resume()
#define usbd_hw_pm_start()
#define usbd_hw_pm_stop()
#endif

#if defined(ARCH_HAVE_USB) && (CONFIG_PM)
void hcd_hw_pm_suspend(void);
void hcd_hw_pm_resume(void);
void hcd_hw_pm_start(void);
#else
#define hcd_hw_pm_suspend()
#define hcd_hw_pm_resume()
#define hcd_hw_pm_start()
#endif

#if defined(ARCH_HAVE_USB) && (SYS_REALTIME)
void usb_hw_irq_poll(void);
#define usb_hw_irq_init()
#else
#define usb_hw_irq_poll()
void usb_hw_irq_init(void);
#endif

#endif /* __USB_DRIVER_H_INCLUDE__ */
