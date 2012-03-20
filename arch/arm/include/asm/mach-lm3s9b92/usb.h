#ifndef __USB_LM3S9B92_H_INCLUDE__
#define __USB_LM3S9B92_H_INCLUDE__

#ifndef ARCH_HAVE_USB
#define ARCH_HAVE_USB		1
#else
#error "Multiple USB controller defined"
#endif

#include <target/config.h>
#include <target/generic.h>
#include <asm/reg.h>
#include <asm/mach/pm.h>
#include <asm/mach/clk.h>
#include <target/gpio.h>

#define USB_BASE		0x40050000
#define USB(offset)		(USB_BASE + offset)

/* USB Power Register */
#define USBPOWER		USB(0x001)
#define PWRRESET		3
#define PWRRESUME		2
#define PWRSUSPEND		1
#define PWRDNPHY		0
/* OTG B/DEV Mode Bits */
#define DEVISOUP		7
#define DEVSOFTCONN		6

/* USB Function Address Register */
#define USBFADDR		USB(0x000)

/* USB Transmit Interrupt Status Register */
#define USBTXIS			USB(0x002)
/* USB Receive Interrupt Status Register */
#define USBRXIS			USB(0x004)
/* USB Transmit Interrupt Enable Register */
#define USBTXIE			USB(0x006)
/* USB Receive Interrupt Enable Register */
#define USBRXIE			USB(0x008)

/* USB Endpoint Index Register */
#define USBEPIDX		USB(0x00E)

/* USB General Interrupt Status Register */
#define USBIS			USB(0x00A)
/* USB Interrupt Enable Register */
#define USBIE			USB(0x00B)

/* OTG A/HCD Mode */
#define HCDVBUSERR		7
#define HCDSESREQ		6
#define HCDDISCON		5
#define HCDCONN			4
#define HCDSOF			3
#define HCDBABBLE		2
#define HCDRESUME		1

/* OTG B/DEV Mode */
#define DEVDISCON		5
#define DEVSOF			3
#define DEVRESET		2
#define DEVRESUME		1
#define DEVSUSPEND		0

/* USB Frame Value Register */
#define USBFRAME		USB(0x00C)

/* USB FIFO Endpoint n */
#define	USBFIFO(n)		USB(0x020 + (n)*4)

/* USB Test Mode */
#define USBTEST			USB(0x00F)
#define FIFOACC			6
#define FORCEFS			5

/* USB Device Control */
#define USBDEVCTL		USB(0x60)
#define DEV			7
#define FSDEV			6
#define LSDEV			5
#define VBUS_OFFSET		3
#define VBUS_MASK		(0x03 << VBUS_OFFSET)
#define VBUS(v)			(((v) & VBUS_MASK) >> VBUS_OFFSET)
#define VBUS_INVALID		0x00
#define VBUS_VA_SESS_VALID	0x01
#define VBUS_VA_VBUS_VALID	0x02
#define VBUS_VB_VBUS_VALID	0x03
#define HOST			2
#define HOSTREQ			1
#define SESSION			0

/* USB Transmit Dynamic FIFO Sizing */
#define USBTXFIFOSZ		USB(0x062)
/* USB Receive Dynamic FIFO Sizing */
#define USBRXFIFOSZ		USB(0x063)
#define DPB			4
#define SIZE			0

/* USB Transmit FIFO Start Address */
#define USBTXFIFOADD		USB(0x064)
/* USB Receive FIFO Start Address */
#define USBRXFIFOADD		USB(0x066)

/* USB Maximum Transmit Data Endpoint n */
#define USBTXMAXP(n)		USB(0x110 + ((n)-1)*16)

/* USB Control and Status Endpoint 0 Low */
#define USBCSRL0		USB(0x102)
#define SETENDC			7
#define RXRDYC			6
#define STALL			5
#define SETEND			4
#define DATAEND			3
#define STALLED			2
#define TXRDY			1
#define RXRDY			0

/* USB Control and Status Endpoint 0 High */
#define USBCSRH0		USB(0x103)
#define FLUSH			0

/* USB Receive Byte Count Endpoint 0 */
#define USBCOUNT0		USB(0x108)

/* USB Transmit Control and Status Endpoint n Low */
#define USBTXCSRL(n)		USB(0x112 + ((n)-1)*16)		
#define TXCLRDT			6
#define TXSTALLED		5
#define TXSTALL			4
#define TXFLUSH			3
#define TXUNDRN			2
#define	TXFIFONE		1
#define	TXTXRDY			0

/* USB Transmit Control and Status Endpoint n High */
#define USBTXCSRH(n)		USB(0x113 + ((n)-1)*16)
#define TXAUTOSET		7
#define TXISO			6
#define TXMODE			5
#define TXDMAEN			4
#define TXFDT			3
#define TXDMAMOD		2

/* USB Maximum Receive Data Endpoint n */
#define USBRXMAXP(n)		USB(0x114 + ((n)-1)*16)

/* USB Receive Control and Status Endpoint n Low */
#define USBRXCSRL(n)		USB(0x116 + ((n)-1)*16)
#define RXCLRDT			7
#define RXSTALLED		6
#define RXSTALL			5
#define RXFLUSH			4
#define RXDATAERR		3
#define RXOVER			2
#define RXFULL			1
#define RXRXRDY			0

/* USB Receive Control and Status Endpoint n High */
#define USBRXCSRH(n)		USB(0x117 + ((n)-1)*16)
#define RXAUTOCL		7
#define RXISO			6
#define RXDMAEN			5
#define RXDISNYET		4
#define RXPIDERR		4
#define RXDMAMOD		3

/* USB Receive Byte Count Endpoint n */
#define USBRXCOUNT(n)		USB(0x118 + ((n)-1)*16)

/* USB Receive Double Packet Buffer Disable */
#define USBRXDPKTBUFDIS		USB(0x340)

/* USB Transmit Double Packet Buffer Disable */
#define USBTXDPKTBUFDIS		USB(0x342)

/* USB External Power Control */
#define USBEPC			USB(0x400)
#define MASK_FAU_TRISTATE	(0x01 << 8)
#define MASK_FAU_LOW		(0x02 << 8)
#define MASK_FAU_HIGH		(0x03 << 8)
#define PFLTAEN			6
#define PFLTSEN			5
#define PFLTEN			4
#define EPENDE			2
#define MASK_EN_LOW		0x00
#define MASK_EN_HIGH		0x01
#define MASK_VBUS_LOW		0x02
#define MASK_VBUS_HIGH		0x03

/* USB External Power Control Raw Interrupt Status */
#define USBEPCRIS		USB(0x404)
/* USB External Power Control Interrupt Mask */
#define USBEPCIM		USB(0x408)
/* USB External Power Control Interrupt Status and Clear */
#define USBEPCISC		USB(0x40C)
#define PF			0

/* USB Device RESUME Raw Interrupt Status */
#define USBDRRIS		USB(0x410)
/* USB Device RESUME Interrupt Mask */
#define USBDRIM			USB(0x414)
/* USB Device RESUME Interrupt Status and Clear */
#define USBDRISC		USB(0x418)
#define RESUME			0

/* USB General-Purpose Control and Status */
#define USBGPCS			USB(0x41C)
#define DEVMODOTG		1
#define DEVMOD			0

/* USB VBUS Droop Control */
#define USBVDC			USB(0x430)
/* USB VBUS Droop Control Raw Interrupt Status */
#define USBVDCRIS		USB(0x434)
/* USB VBUS Droop Control Interrupt Mask */
#define USBVDCIM		USB(0x438)
/* USB VBUS Droop Control Interrupt Status and Clear */
#define USBVDCISC		USB(0x43C)
#define VD			0

/* USB ID Valid Detect Raw Interrupt Status */
#define USBIDVRIS		USB(0x444)
/* USB ID Valid Detect Interrupt Mask */
#define USBIDVIM		USB(0x448)
/* USB ID Valid Detect Interrupt Status and Clear */
#define USBIDVISC		USB(0x44C)
#define ID			0

/* USB DMA Select */
#define USBDMASEL		USB(0x450)
#define MASK_DMACT(n)		((n) << 20)
#define MASK_DMACR(n)		((n) << 16)
#define MASK_DMABT(n)		((n) << 12)
#define MASK_DMABR(n)		((n) << 8)
#define MASK_DMAAT(n)		((n) << 4)
#define MASK_DMAAR(n)		(n)

/* GPIO PIN B0 */
#define GPIOB0_MUX_USB0ID	0x01
/* GPIO PIN B1 */
#define GPIOB1_MUX_USB0VBUS	0x01

/* GPIO PIN A6 */
#define GPIOA6_MUX_USB0EPEN	0x08
/* GPIO PIN B2 */
#define GPIOB2_MUX_USB0EPEN	0x08
/* GPIO PIN C5 */
#define GPIOC5_MUX_USB0EPEN	0x06
/* GPIO PIN G0 */
#define GPIOG0_MUX_USB0EPEN	0x07
/* GPIO PIN H3 */
#define GPIOH3_MUX_USB0EPEN	0x04

/* GPIO PIN A7 */
#define GPIOA7_MUX_USB0PFLT	0x08
/* GPIO PIN B3 */
#define GPIOB3_MUX_USB0PFLT	0x08
/* GPIO PIN C6 */
#define GPIOC6_MUX_USB0PFLT	0x07
/* GPIO PIN C7 */
#define GPIOC7_MUX_USB0PFLT	0x06
/* GPIO PIN E0 */
#define GPIOE0_MUX_USB0PFLT	0x09
/* GPIO PIN H4 */
#define GPIOH4_MUX_USB0PFLT	0x04
/* GPIO PIN J1 */
#define GPIOJ1_MUX_USB0PFLT	0x09

#ifdef CONFIG_USB_LM3S9B92_FIFO_DOUBLE
#define USB_LM3S9B92_FIFO_SIZE	128
#define USB_LM3S9B92_FIFOADD_S	2
#define USB_LM3S9B92_FIFOSZ_DPB	1
#else
#define USB_LM3S9B92_FIFO_SIZE	64
#define USB_LM3S9B92_FIFOADD_S	3
#define USB_LM3S9B92_FIFOSZ_DPB	0
#endif

typedef uint8_t utb_size_t;
#define utb_text_size_t text_byte_t

#define USBD_HW_ISO_SIZE_RAW						\
	((4096-USBD_HW_CTRL_SIZE-USB_LM3S9B92_FIFO_SIZE*(15-CONFIG_USB_LM3S9B92_MAX_ISOS))/CONFIG_USB_LM3S9B92_MAX_ISOS)
#define USBD_HW_ISO_SIZE						\
	(USBD_HW_ISO_SIZE_RAW>1023?1023:USBD_HW_ISO_SIZE_RAW)
#define USBD_HW_MAX_BI_RAW						\
	((4096-USBD_HW_CTRL_SIZE-1023*CONFIG_USB_LM3S9B92_MAX_ISOS)/USB_LM3S9B92_FIFO_SIZE)
#define USBD_HW_MAX_BI							\
	(USBD_HW_MAX_BI_RAW < (15-CONFIG_USB_LM3S9B92_MAX_ISOS) ?	\
	 USBD_HW_MAX_BI_RAW : (15-CONFIG_USB_LM3S9B92_MAX_ISOS))
#define NR_USBD_HW_ENDPS	(1+CONFIG_USB_LM3S9B92_MAX_ISOS+USBD_HW_MAX_BI)

#define USB_HW_EID_BI_MIN	(1+CONFIG_USB_LM3S9B92_MAX_ISOS)
#define USB_HW_EID_BI_MAX	(NR_USBD_HW_ENDPS-1)
#define USB_HW_EID_ISO_MIN	1
#define USB_HW_EID_ISO_MAX	CONFIG_USB_LM3S9B92_MAX_ISOS
#define USB_HW_EID_MAX_NONISOS	(USB_HW_EID_BI_MAX-USB_HW_EID_BI_MIN+1)

/*
 * Default endpoint 0 is automatically configured to use the first 64 bytes
 * of the FIFO.
 */
#define USBD_HW_CTRL_SIZE	64

void usb_hw_ctrl_init(void);

void usbd_hw_ctrl_init(void);
void usbd_hw_ctrl_start(void);
void usbd_hw_ctrl_stop(void);

void usbd_hw_bus_detach(void);
void usbd_hw_bus_attach(void);

boolean usbd_hw_endp_caps(uint8_t addr, uint8_t type);
utb_size_t usbd_hw_endp_size(uint8_t addr);

void usbd_hw_endp_enable(void);
void usbd_hw_endp_unhalt(void);
void usbd_hw_endp_halt(void);
void usbd_hw_endp_select(uint8_t addr);

#define usbd_hw_read_byte()		__raw_readb(USBFIFO(USB_ADDR2EID(usbd_endp)))
#define usbd_hw_write_byte(byte)	__raw_writeb(byte, USBFIFO(USB_ADDR2EID(usbd_endp)))

void usbd_hw_transfer_open(void);
void usbd_hw_transfer_close(void);

void usbd_hw_set_address(void);
void usbd_hw_set_config(void);

void usbd_hw_request_open(void);
void usbd_hw_request_close(void);
void usbd_hw_request_reset(void);

#endif /* __USB_LM3S9B92_H_INCLUDE__ */
