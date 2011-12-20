#ifndef __USB_AT8XC5122_H_INCLUDE__
#define __USB_AT8XC5122_H_INCLUDE__

#include <target/config.h>
#include <target/generic.h>

#include <asm/reg.h>
#include <asm/mach/irq.h>
#include <asm/mach/clk.h>

#ifdef CONFIG_USB_AT8XC5122
#ifndef ARCH_HAVE_USB
#define ARCH_HAVE_USB		1
#else
#error "Multiple USB controller defined"
#endif
#endif

/* ============================================================ *
 * definitions
 * ============================================================ */
Sfr(UFNUML,	0xBA);		/* USB Frame Number Low */
Sfr(UFNUMH,	0xBB);		/* USB Frame Number Low */
Sfr(USBCON,	0xBC);		/* USB Global Control */
Sfr(USBINT,	0xBD);		/* USB Global Interrupt */
Sfr(USBIEN,	0xBE);		/* USB Global Interrupt enable */
Sfr(UEPIEN,	0xC2);		/* USB Endpoint Interrupt enable */
Sfr(USBADDR,	0xC6);		/* USB Address */
Sfr(UEPNUM,	0xC7);		/* USB Endpoint Number */
Sfr(UEPSTAX,	0xCE);		/* USB Endpoint X Status */
Sfr(UEPDATX,	0xCF);		/* USB Endpoint X Fifo Data */
Sfr(UEPCONX,	0xD4);		/* USB Endpoint X Control */
Sfr(UEPRST,	0xD5);		/* USB Endpoint Reset */
Sfr(UBYCTX,	0xE2);		/* USB Byte Counter Low (EPX) */
Sfr(UEPINT,	0xF8);		/* USB Endpoint Interrupt */

#define USBCON_USBE		0x80	/* USBCON */
#define MSK_USBCON_SUSPCLK	0x40
#define MSK_USBCON_SDRMWUP	0x20
#define MSK_USBCON_DETACH	0x10
#define MSK_USBCON_UPRSM	0x08
#define MSK_USBCON_RMWUPE	0x04
#define USBCON_ADDRESS		0x01
#define USBCON_CONFIG		0x02

#define USBADDR_FEN		0x80	/* USBADDR */
#define USBADDR_UADDR		0x7F

#define USBINT_SPINT		0
#define USBINT_SOFINT		3
#define USBINT_EORINT		4
#define USBINT_WUPCPU		5

/* UEPCONX */
#define UEPCON_NAKIEN		0x40
#define UEPCON_NAKOUT		0x20
#define UEPCON_NAKIN		0x10
#define UEPCON_DTGL		0x08

#define EPTYPE			0
#define EPDIR			2
#define EPEN			7
#define UEPCON_ENABLE		0x80
#define UEPCON_IN		0x04
#define UEPCON_OUT		0x00
#define UEPCON_DIR_MASK		0x04

/* endpoint irqs */
#define UEPSTA_TXCMPL		0
#define UEPSTA_RXOUTB0		1
#define UEPSTA_RXSETUP		2
#define UEPSTA_STLCRC		3
#define UEPSTA_STALLRQ		5
#define UEPSTA_RXOUTB1		6

/* endpoint flags */
#define UEPSTA_TXRDY		4
#define UEPSTA_DIR		7

#define UFNUM_CRCOIK		0x20	/* UFNUMH */
#define UFNUM_CRCERR		0x10

/* ============================================================ *
 * functions
 * ============================================================ */
/* general management */
#define __usbd_hw_enable()		(USBCON |=  USBCON_USBE)
#define __usbd_hw_disable()		(USBCON &= ~USBCON_USBE)
#define __usbd_hw_suspend_clock()	(USBCON |= MSK_USBCON_SUSPCLK)
#define __usbd_hw_wakeup_clock()	(USBCON &= ~MSK_USBCON_SUSPCLK)

#define __usbd_hw_is_upstream_resume()	(USBCON & MSK_USBCON_UPRSM)

/* remote wakeup */
#define __usbd_hw_set_remote_wakeup()		(USBCON |=  MSK_USBCON_SDRMWUP)
#define __usbd_hw_clear_remote_wakeup()		(USBCON &= ~MSK_USBCON_SDRMWUP)
#define __usbd_hw_is_remote_wakeup()		(USBCON &   MSK_USBCON_RMWUPE)
#define __usbd_hw_enable_remote_wakeup()	(USBCON |=  MSK_USBCON_RMWUPE)
#define __usbd_hw_disable_remote_wakeup()	(USBCON &= ~MSK_USBCON_RMWUPE)

#define __usbd_hw_frame_number()		MAKEWORD(UFNUML, UFNUMH)

/* interrupt management */
#ifdef SYS_REALTIME
#define __usbd_hw_irq_enable(irq)
#define __usbd_hw_irq_disable(irq)
#define __usbd_hw_irq_enabled(irq)	(true)
#define __uep_hw_irq_enable(eid)
#define __uep_hw_irq_disable(eid)
#else
#define __usbd_hw_irq_enable(irq)	(USBIEN |=  (1<<(irq)))
#define __usbd_hw_irq_disable(irq)	(USBIEN &= ~(1<<(irq)))
#define __usbd_hw_irq_enabled(irq)	(USBIEN &   (1<<(irq)))
#define __uep_hw_irq_enable(eid)	(UEPIEN |=  (1<<(eid)))
#define __uep_hw_irq_disable(eid)	(UEPIEN &= ~(1<<(eid)))
#endif
#define __usbd_hw_irq_raised(irq)	(USBINT &   (1<<(irq)))
#define __usbd_hw_irq_unraise(irq)	(USBINT &= ~(1<<(irq)))
#define __uep_hw_irq_raised(eid)	(UEPINT &   (1<<(eid)))
#define __uep_hw_irq_requested(irq)	(__uep_hw_sta_raised(irq))

/* endpoint */
#define __uep_hw_valid_data()		(UEPCONX &   UEPCON_DTGL)

#define __uep_hw_sta_raise(flag)	(UEPSTAX |=  (1<<(flag)))
#define __uep_hw_sta_unraise(flag)	(UEPSTAX &= ~(1<<(flag)))
#define __uep_hw_sta_raised(flag)	(UEPSTAX &   (1<<(flag)))

#define __uep_hw_request_in()		__uep_hw_sta_raise(UEPSTA_DIR)
#define __uep_hw_request_out()		__uep_hw_sta_unraise(UEPSTA_DIR)

#define __usbd_hw_state_enter(state)	(USBCON |=  (state))
#define __usbd_hw_state_leave(state)	(USBCON &= ~(state))

/* ============================================================ *
 * USB bus driver
 * ============================================================ */
/* power management */
void usb_hw_ctrl_init(void);

/* ============================================================ *
 * USB device driver
 * ============================================================ */
#ifdef CONFIG_USB_AT8XC5122_MAX_ENDPS
#define NR_USBD_HW_ENDPS	CONFIG_USB_AT8XC5122_MAX_ENDPS
#else
#define NR_USBD_HW_ENDPS	7
#endif
#define USBD_HW_CTRL_SIZE	32

typedef uint8_t utb_size_t;
#define utb_text_size_t text_byte_t

/* capabilities */
boolean usbd_hw_endp_caps(uint8_t addr, uint8_t type);
utb_size_t usbd_hw_endp_size(uint8_t addr);

/* device */
void usbd_hw_ctrl_init(void);
void usbd_hw_ctrl_start(void);
void usbd_hw_ctrl_stop(void);

#define usbd_hw_bus_detach()		(USBCON |= MSK_USBCON_DETACH)
#define usbd_hw_bus_attach()		(USBCON &= ~MSK_USBCON_DETACH)

/* endpoint */
void usbd_hw_endp_enable(void);
void usbd_hw_endp_unhalt(void);
void usbd_hw_endp_halt(void);
#define usbd_hw_endp_select(addr)	(UEPNUM = USB_ADDR2EID(addr))

/* transfer */
#define usbd_hw_read_byte()		(UEPDATX)
#define usbd_hw_write_byte(byte)	(UEPDATX = (byte))
void usbd_hw_transfer_open(void);
void usbd_hw_transfer_close(void);

/* default pipe states */
void usbd_hw_set_address(void);
void usbd_hw_set_config(void);

void usbd_hw_request_open(void);
void usbd_hw_request_close(void);
void usbd_hw_request_reset(void);

#endif /* __USB_AT8XC5122_H_INCLUDE__ */
