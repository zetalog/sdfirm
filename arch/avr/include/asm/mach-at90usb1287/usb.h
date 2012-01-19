#ifndef __USB_AT90USB1287_H_INCLUDE__
#define __USB_AT90USB1287_H_INCLUDE__

#ifndef ARCH_HAVE_USB
#define ARCH_HAVE_USB		1
#else
#error "Multiple USB controller defined"
#endif

#include <target/config.h>
#include <asm/reg.h>
#include <asm/mach/gpio.h>
#include <asm/mach/pm.h>
#include <asm/mach/clk.h>

/* UVCON is PORTE */
#define UVCON_PORT		PORTE
#define UVCON_DDR		DDRE
#define UVCON_PIN		PINE
#define DD_UVCON		DDE7

/*
 * USB General Registers
 */
#define UHWCON			_SFR_MEM8(0XD7)
#define UVREGE			0
#define UVCONE			4
#define UIDE			6
#define UIMOD			7

#define USBCON			_SFR_MEM8(0XD8)
#define OTGPADE			4
#define FRZCLK			5
#define HOST			6
#define USBE			7

#define USBSTA			_SFR_MEM8(0XD9)
#define VBUS			0
#define ID			1
#define SPEED			3

#define USBINT			_SFR_MEM8(0XDA)
#define VBUST			0
#define IDT			1

/*
 * USB Device Registers
 */
#define UDCON			_SFR_MEM8(0XE0)
#define DETACH			0
#define RMWKUP			1
#define LSM			2

#define UDINT			_SFR_MEM8(0XE1)
#define UDIEN			_SFR_MEM8(0XE2)
#define SUSP			0	/* suspend */
#define SOF			2	/* start-of-frame */
#define EORST			3	/* end of reset */
#define WAKEUP			4	/* wakeup CPU */
#define EORSM			5	/* end of resume */
#define UPRSM			6	/* upstream resume */

#define UDADDR			_SFR_MEM8(0XE3)
#define ADDEN			7
#define UADD_SIZE		7
#define UADD_MASK		((1<<UADD_SIZE) - 1)

#define UDFNUM			_SFR_MEM16(0xE4)
#define UDFNUML			_SFR_MEM8(0xE4)
#define UDFNUMH			_SFR_MEM8(0xE5)
#define UDMFN			_SFR_MEM8(0XE6)

/*
 * USB Device Endpoint Registers
 */
#define UENUM			_SFR_MEM8(0XE9)
#define UERST			_SFR_MEM8(0XEA)

#define UECONX			_SFR_MEM8(0XEB)
#define EPEN			0
#define RSTDT			3
#define STALLRQC		4
#define STALLRQ			5

#define UECFG0X			_SFR_MEM8(0XEC)
#define EPDIR			0
#define NYETDIS			1
#define EPTYPE0			6
#define EPTYPE1			7

#define UECFG1X			_SFR_MEM8(0XED)
#define ALLOC			1
#define EPBK0			2
#define EPBK1			3

#define UECFG_8BYTES		0x00
#define UECFG_16BYTES		0x10
#define UECFG_32BYTES		0x20
#define UECFG_64BYTES		0x30
#define UECFG_128BYTES		0x40
#define UECFG_256BYTES		0x50

#define UESTA0X			_SFR_MEM8(0XEE)
#define CFGOK			7
#define UESTA1X			_SFR_MEM8(0XEF)

#define UEINTX			_SFR_MEM8(0XE8)
#define KILLBK			2
#define RWAL			5
#define FIFOCON			7

#define UEIENX			_SFR_MEM8(0XF0)
#define FLERRE			7

/* shared between UEINTX and UEIENX */
#define TXIN			0
#define STALLED			1
#define RXOUT			2
#define RXSTP			3
#define NAKOUT			4
#define NAKIN			6

#define UEDATX			_SFR_MEM8(0XF1)

#define UEBCX			_SFR_MEM16(0xF2)
#define UEBCLX			_SFR_MEM8(0xF2)
#define UEBCHX			_SFR_MEM8(0xF3)
#define UEINT			_SFR_MEM8(0XF4)

/*
 * USB Host Registers
 */
#define UHCON			_SFR_MEM8(0x9E)
#define RESUME			2
#define RESET			1
#define SOFEN			0

#define UHINT			_SFR_MEM8(0x9F)
#define UHIEN			_SFR_MEM8(0xA0)
#define HWUP			6
#define HSOF			5
#define RXRSM			4
#define RSMED			3
#define RST			2
#define DDISC			1
#define DCONN			0

#define UHADDR			_SFR_MEM8(0xA1)

#define UHFNUM			_SFR_MEM16(0xA2)
#define UHFNUML			_SFR_MEM8(0xA2)
#define UHFNUMH			_SFR_MEM8(0xA3)

#define UHFLEN			_SFR_MEM8(0xA4)

/*
 * USB Host Pipe Registers
 */
#define UPINRQX			_SFR_MEM8(0xA5)
#define UPINTX			_SFR_MEM8(0xA6)
#define UPNUM			_SFR_MEM8(0xA7)
#define UPRST			_SFR_MEM8(0xA8)
#define UPCONX			_SFR_MEM8(0xA9)
#define UPCFG0X			_SFR_MEM8(0XAA)
#define UPCFG1X			_SFR_MEM8(0XAB)
#define UPSTAX			_SFR_MEM8(0XAC)
#define UPCFG2X			_SFR_MEM8(0XAD)
#define UPIENX			_SFR_MEM8(0XAE)
#define UPDATX			_SFR_MEM8(0XAF)

#define UPERRX			_SFR_MEM8(0XF5)
#define UPBCX			_SFR_MEM16(0xF6)
#define UPBCLX			_SFR_MEM8(0xF6)
#define UPBCHX			_SFR_MEM8(0xF7)
#define UPINT			_SFR_MEM8(0XF8)

/*
 * USB On-The-Go Registers
 */
#define OTGCON			_SFR_MEM8(0XDD)
#define HNPREQ			5
#define SRPREQ			4
#define SRPSEL			3
#define VBUSHWC			2
#define VBUSREQ			1
#define VBUSRQC			0

#define OTGIEN			_SFR_MEM8(0XDE)
#define OTGINT			_SFR_MEM8(0XDF)
#define STO			5
#define HNPERR			4
#define ROLEEX			3
#define BCERR			2
#define VBERR			1
#define SRP			0

#define OTGTCON			_SFR_MEM8(0XF9)
#define PAGE1			6
#define PAGE0			5
#define VALUE1			1
#define VALUE0			0

#ifdef SYS_REALTIME
#define __usb_hw_enable_irq(irq)
#define __usb_hw_disable_irq(irq)
#define __usbd_hw_enable_irq(irq)
#define __usbd_hw_disable_irq(irq)
#define __usbd_hw_eirq_raised_eid(eid)	(true)
#define __usbd_hw_enable_eirq(irq)
#define __usbd_hw_disable_eirq(irq)
#define __usbd_hw_eirq_enabled(irq)	(true)
#else
#define __usb_hw_enable_irq(irq)	(USBCON |=  _BV(irq))
#define __usb_hw_disable_irq(irq)	(USBCON &= ~_BV(irq))
#define __usbd_hw_enable_irq(irq)	(UDIEN |=  _BV(irq))
#define __usbd_hw_disable_irq(irq)	(UDIEN &= ~_BV(irq))
#define __usbd_hw_eirq_raised_eid(eid)	(UEINT & (1 << (eid)))
#define __usbd_hw_enable_eirq(irq)	(UEIENX |=  _BV(irq))
#define __usbd_hw_disable_eirq(irq)	(UEIENX &= ~_BV(irq))
#define __usbd_hw_eirq_enabled(irq)	(UEIENX &   _BV(irq))
#endif
#define __usb_hw_irq_raised(irq)	(USBINT &   _BV(irq))
#define __usb_hw_unraise_irq(irq)	(USBINT &= ~_BV(irq))

#define __usb_hw_freeze_clock()		(USBCON |=  _BV(FRZCLK))
#define __usb_hw_unfreeze_clock()	(USBCON &= ~_BV(FRZCLK))

#define __otg_hw_disable_power()	(OTGCON |= _BV(VBUSRQC))
#define __otg_hw_enable_power()		(OTGCON |= _BV(VBUSREQ))

#define __usbd_hw_frame_error()		(UDMFN & _BV(FNCERR))
#define __usbd_hw_frame_number()	MAKEWORD(UDFNUML, UDFNUMH)

#ifdef CONFIG_USB_HCD
#ifndef CONFIG_USB_AT90USB1287_VBUS_SWC
#define __usb_hw_enable_power()		__otg_hw_enable_power()
#define __usb_hw_disable_power()	__otg_hw_disable_power()
#define __usb_hw_config_power()			\
	do {					\
		UHWCON |= _BV(UVCONE);		\
		OTGCON = 0;			\
		__usb_hw_enable_power();	\
	} while (0)
#else
#define __usb_hw_enable_power()		(UVCON_PORT |=  _BV(DD_UVCON))
#define __usb_hw_disable_power()	(UVCON_PORT &= ~_BV(DD_UVCON))
#define __usb_hw_config_power()			\
	do {					\
		UVCON_DDR |= _BV(DD_UVCON);	\
		OTGCON = _BV(VBUSHWC);		\
		__otg_hw_disable_power();	\
		__usb_hw_enable_power();	\
	} while (0)
#endif
#else
#define __usb_hw_config_power()		__otg_hw_disable_power()
#define __usb_hw_enable_power()
#define __usb_hw_disable_power()
#endif

#define __usb_hw_select_device()	(USBCON &= ~_BV(HOST))
#define __usb_hw_select_host()		(USBCON |= _BV(HOST))

#ifdef CONFIG_USB_OTG
/* enable UID pin, allowing mode configuration through UID pin detection  */
#define __usb_hw_enable_mode()		(UHWCON = (_BV(UIDE) | _BV(UVREGE)))
#define __usb_hw_config_device()
#define __usb_hw_config_host()
#else
/* disable UID pin, allowing mode configuration through UIMOD */
#define __usb_hw_enable_mode()		(UHWCON |= (_BV(UVREGE)))
#define __usb_hw_config_device()	(UHWCON |=  _BV(UIMOD))
#define __usb_hw_config_host()		(UHWCON &= ~_BV(UIMOD))
#endif

#define __usbd_hw_disable_address()	(UDADDR &= ~_BV(ADDEN))
#define __usbd_hw_enable_address()	(UDADDR |=  _BV(ADDEN))
#define __usbd_hw_clear_address()	(UDADDR &= ~UADD_MASK)
#define __usbd_hw_set_address()		(UDADDR = usbd_address)
#define __usb_hw_enable_macro()		(USBCON |= (_BV(USBE) | _BV(OTGPADE)))
#define __usb_hw_disable_macro()	(USBCON &= ~(_BV(USBE)))

#define __usb_hw_vbus_is_high()		(USBSTA & _BV(VBUS))
#define __usb_hw_vbus_is_low()		(!__usb_hw_vbus_is_high())
#define __usb_hw_id_is_device()		(USBSTA & _BV(ID))
#define __usb_hw_id_is_host()		(!__usb_hw_id_is_device())

#define __usbd_hw_enable_stall()	(UECONX |=  _BV(STALLRQ))
#define __usbd_hw_disable_stall()	(UECONX |=  _BV(STALLRQC))

#define __usbd_hw_irq_raised(irq)	(UDINT &   _BV(irq))
#define __usbd_hw_unraise_irq(irq)	(UDINT &= ~_BV(irq))

#define __usbd_hw_eirq_raised(irq)	(UEINTX &   _BV(irq))
#define __usbd_hw_eirq_requested(irq)	(__usbd_hw_eirq_enabled(irq) && \
					 __usbd_hw_eirq_raised(irq))

#define __usbd_hw_unraise_eirq(irq)	(UEINTX &= ~_BV(irq))

#define __usbd_hw_read_avail()		(UEBCLX)

#define __hcd_hw_enable_irq(irq)	(UHIEN |=  _BV(irq))
#define __hcd_hw_disable_irq(irq)	(UHIEN &= ~_BV(irq))
#define __hcd_hw_disable_sof()		(UHCON &= ~_BV(SOFEN))
#define __hcd_hw_send_reset()		(UHCON |=  _BV(RESET))
#define __hcd_hw_send_resume()		(UHCON |=  _BV(RESUME))

/* ============================================================ *
 * USB bus driver
 * ============================================================ */
void usb_hw_ctrl_init(void);

/* ============================================================ *
 * USB device driver
 * ============================================================ */
#ifdef CONFIG_USB_AT90USB1287_MAX_ENDPS
#define NR_USBD_HW_ENDPS	CONFIG_USB_AT90USB1287_MAX_ENDPS
#else
#define NR_USBD_HW_ENDPS	7
#endif
#define USBD_HW_CTRL_SIZE	64

typedef uint8_t utb_size_t;
#define utb_text_size_t text_byte_t

void usbd_hw_ctrl_init(void);
void usbd_hw_ctrl_start(void);
void usbd_hw_ctrl_stop(void);

#define usbd_hw_bus_detach()		(UDCON |=  _BV(DETACH))
#define usbd_hw_bus_attach()		(UDCON &= ~_BV(DETACH))

boolean usbd_hw_endp_caps(uint8_t addr, uint8_t type);
utb_size_t usbd_hw_endp_size(uint8_t addr);

void usbd_hw_endp_enable(void);
void usbd_hw_endp_unhalt(void);
void usbd_hw_endp_halt(void);
#define usbd_hw_endp_select(addr)	(UENUM = USB_ADDR2EID(addr))

#define usbd_hw_read_byte()		(UEDATX)
#define usbd_hw_write_byte(byte)	(UEDATX = (byte))
void usbd_hw_transfer_open(void);
void usbd_hw_transfer_close(void);

void usbd_hw_set_address(void);
#define usbd_hw_set_config()

void usbd_hw_request_open(void);
void usbd_hw_request_close(void);
void usbd_hw_request_reset(void);

boolean __usbd_hw_is_txaval(void);
boolean __usbd_hw_is_rxaval(void);

#endif /* __USB_AT90USB1287_H_INCLUDE__ */
