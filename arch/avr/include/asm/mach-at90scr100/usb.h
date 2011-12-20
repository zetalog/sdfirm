#ifndef __USB_AT90SCR100_H_INCLUDE__
#define __USB_AT90SCR100_H_INCLUDE__

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

/* USBCR - USB Control Register */
#define USBCR		_SFR_MEM8(0xE0)
#define USBE		1
#define UPUC		5
#define URMWU		7

/* USBPI - USB Protocol Interrupt Register */
#define USBPI		_SFR_MEM8(0xE1)
/* USBPIM - USB Protocol Interrupt Mask Register */
#define USBPIM		_SFR_MEM8(0xE2)
#define SUSI		0
#define RESI		1
#define RMWUI		2
#define SOFI		3
#define FEURI		4

/* USB Endpoint Interrupt, Mask Register */
#define USBEI		_SFR_MEM8(0xE3)
#define USBEIM		_SFR_MEM8(0xE4)
#define EPI(n)		(n)

/* USB Endpoint Number Register */
#define USBENUM		_SFR_MEM8(0xCA)
#define USB_ENUM	(USBENUM & 0x07)

/* USB Control And Status Register for Endpoint X */
#define USBCSEX		_SFR_MEM8(0xCB)
#define TXC		0
#define RCVD		1
#define RXSETUP		2
#define STSENT		3
#define TXPB		4
#define FSTALL		5
#define IERR		6

/* USB Data Byte Count Registers for Endpoint X */
#define USBDBCEX	_SFR_MEM8(0xCC)
#define BCT(n)		(n)

/* USB Function Control Registers for Endpoint X */
#define USBFCEX		_SFR_MEM8(0xCD)
#define EPTYP0		0
#define EPTYP1		1
#define EPDIR		2
#define EPE		7

/* USB Reset Endpoint Register */
#define USBRSTE		_SFR_MEM8(0xE5)
#define RSTE(n)		(n)

/* USB Global State Register */
#define USBGS		_SFR_MEM8(0xE6)
#define FAF		0
#define FCF		1
#define RMWUE		2
#define RSMON		3

/* USB Function Address Register */
#define USBFA		_SFR_MEM8(0xE7)
#define FADD0		0
#define FADD1		1
#define FADD2		2
#define FADD3		3
#define FADD4		4
#define FADD5		5
#define FADD6		6
#define FADD_SIZE	7
#define FADD_MASK	((1<<FADD_SIZE) - 1)

/* USB Frame Number Register */
#define USBFN		_SFR_MEM16(0xE8)
#define USBFNL		_SFR_MEM8(0xE8)
#define USBFNH		_SFR_MEM8(0xE9)
#define USBFNH_MASK	0x07
#define FNERR		3
#define FNEND		4

/* USBDMA Control and Status Register */
#define USBDMACS	_SFR_MEM8(0XEA)
#define USBDMAR		0
#define USBDMADIR	1
#define USBDMAERR	2
#define EPS0		4
#define EPS1		5
#define EPS2		6
#define EPS_OFFSET	4

/* USBDMA Address Register */
#define USBDMAD		_SFR_MEM8(0XEB)
#define USBDMADL	_SFR_MEM8(0XEB)
#define USBDMADH	_SFR_MEM8(0XEC)

/* USBDMA Amount of Bytes Register */
#define USBDMAB		_SFR_MEM8(0XED)

#define __usbd_hw_disable_address()
#define __usbd_hw_enable_address()
void  __usbd_hw_set_address(void);
void  __usbd_hw_claer_address(void);

#define __usbd_hw_state_enter(state)	(USBGS |=  (state))
#define __usbd_hw_state_leave(state)	(USBGS &= ~(state))

#define __usb_hw_disable_macro()	(USBCR &= ~_BV(USBE))
#define __usb_hw_enable_macro()		(USBCR |=  _BV(USBE))

#ifdef SYS_REALTIME
/* Endpoint interrupt */
#define __usbd_hw_enable_eirq(eid)
#define __usbd_hw_disable_eirq(eid)
#define __usbd_hw_eirq_enabled(eid)	(true)
#define __usbd_hw_enable_pirq(irq)
#define __usbd_hw_disable_pirq(irq)
#define __usbd_hw_pirq_enabled(irq)	(true)
#else
#define __usbd_hw_enable_eirq(eid)	(USBEIM |=  _BV(eid))
#define __usbd_hw_disable_eirq(eid)	(USBEIM &= ~_BV(eid))
#define __usbd_hw_eirq_enabled(eid)	(USBEIM &   _BV(eid))
/* Protocol interrupt */
#define __usbd_hw_enable_pirq(irq)	(USBPIM |=  _BV(irq))
#define __usbd_hw_disable_pirq(irq)	(USBPIM &= ~_BV(irq))
#define __usbd_hw_pirq_enabled(irq)	(USBPIM &   _BV(irq))
#endif

/* Protocol interrupt */
#define __usbd_hw_pirq_raised(irq)	(USBPI  &   _BV(irq))
#define __usbd_hw_unraise_pirq(irq)	(USBPI  &= ~_BV(irq))
#define __usbd_hw_pirq_requested(irq)	(__usbd_hw_pirq_enabled(irq) && \
					 __usbd_hw_pirq_raised(irq))

/* USBCSEX: control and status register */
#define __usbd_hw_sta_raised(flag)	(USBCSEX & _BV(flag))
/* can be STSENT / RXSETUP / RCVD / TXC */
#define __usbd_hw_unraise_sta(flag)	(USBCSEX &= ~_BV(flag))
#define __usbd_hw_raise_sta(flag)	(USBCSEX |=  _BV(flag))
#define __usbd_hw_sta_reply(flag)	__usbd_hw_unraise_sta(flag)

/* Endpoint interrupt */
#define __usbd_hw_eirq_raised_eid(eid)	(USBEI & (1 << (eid)))
#define __usbd_hw_eirq_requested(irq)	(__usbd_hw_eirq_enabled(USB_ADDR2EID(usbd_endp)) && \
					 __usbd_hw_sta_raised(irq))

#define __usbd_hw_frame_number()	MAKEWORD(USBFNL, (USBFNH & USBFNH_MASK))
#define __usbd_hw_frame_error()		(USBFNH & _BV(FNERR))
#define __usbd_hw_frame_end()		(USBFNH & _BV(FNEND))

#define __usbd_hw_read_avail()		(USBDBCEX)

/* ============================================================ *
 * USB bus driver
 * ============================================================ */
void usb_hw_ctrl_init(void);

/* ============================================================ *
 * USB device driver
 * ============================================================ */
#ifdef CONFIG_USB_AT90SCR100_MAX_ENDPS
#define NR_USBD_HW_ENDPS	CONFIG_USB_AT90SCR100_MAX_ENDPS
#else
#define NR_USBD_HW_ENDPS	7
#endif
#define USBD_HW_CTRL_SIZE	64

typedef uint8_t utb_size_t;
#define utb_text_size_t text_byte_t

void usbd_hw_ctrl_init(void);
void usbd_hw_ctrl_start(void);
void usbd_hw_ctrl_stop(void);
#define usbd_hw_bus_detach()		(USBCR	&= ~_BV(UPUC))
#define usbd_hw_bus_attach()		(USBCR	|=  _BV(UPUC))

boolean usbd_hw_endp_caps(uint8_t addr, uint8_t type);
utb_size_t usbd_hw_endp_size(uint8_t addr);

void usbd_hw_endp_enable(void);
#define usbd_hw_endp_unhalt()		__usbd_hw_unraise_sta(FSTALL)
#define usbd_hw_endp_halt()		__usbd_hw_raise_sta(FSTALL)
#define usbd_hw_endp_select(addr)	(USBENUM = USB_ADDR2EID(addr))

uint8_t usbd_hw_read_byte(void);
void usbd_hw_write_byte(uint8_t byte);
void usbd_hw_transfer_open(void);
void usbd_hw_transfer_close(void);

void usbd_hw_set_config(void);
void usbd_hw_set_address(void);

void usbd_hw_request_open(void);
void usbd_hw_request_close(void);
void usbd_hw_request_reset(void);

boolean __usbd_hw_is_txaval(void);
boolean __usbd_hw_is_rxaval(void);

#endif /* __USB_AT90SCR100_H_INCLUDE__ */
