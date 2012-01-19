#ifndef __USBDFU_H_INCLUDE__
#define __USBDFU_H_INCLUDE__

#include <host/usbdev.h>
#include <host/types.h>

DEFINE_GUID(GUID_DEVCLASS_FIRMWARE, 
0x7a396e94, 0x754, 0x487e, 0xac, 0xc4, 0x34, 0xd7, 0xd3, 0xf5, 0xa7, 0x6d);
DEFINE_GUID(FIRMWARE_WMI_GUID, 
0xcb277a01, 0x218d, 0x450b, 0xb8, 0x9d, 0x17, 0xc6, 0x22, 0xf8, 0xd7, 0xc9);

#define USBDFU_CLASS		0xFE
#define USBDFU_SUBCLASS		0x01
#define USBDFU_PROTO_RUNTIME	0x01
#define USBDFU_PROTO_DFUMODE	0x02

#define DFU_DETACH		0
#define DFU_DNLOAD		1
#define DFU_UPLOAD		2
#define DFU_GETSTATUS		3
#define DFU_CLRSTATUS		4
#define DFU_GETSTATE		5
#define DFU_ABORT		6
#define DFU_LASTSEQ		61
#define DFU_RESET		62
#define DFU_GETINFO		63

/* No error condition present */
#define DFU_STATUS_OK			0x00
/* File is not targeted file used by this device */
#define DFU_STATUS_errTARGET		0x01
/* File is OK but failed vendor-specific request */
#define DFU_STATUS_errFILE		0x02
/* Device is unable to write memory */
#define DFU_STATUS_errWRITE		0x03
/* Memory erase function failed */
#define DFU_STATUS_errERASE		0x04
/* Memory erase check failed */
#define DFU_STATUS_errCHECK_ERASED	0x05
/* Program memory function failed */
#define DFU_STATUS_errPROG		0x06
/* Programmed memory failed verification */
#define DFU_STATUS_errVERIFY		0x07
/* Received program address out of range */
#define DFU_STATUS_errADDRESS		0x08
/* Received DFU_DNLOAD with wLength = 0, but device does not think it has
 * all the data yet
 */
#define DFU_STATUS_errNOTDONE		0x09
/* Device's firmware is corrupted. It can not return to (non-DFU)
 * operations
 */
#define DFU_STATUS_errFIRMWARE		0x0A
/* Vendor-specific string error */
#define DFU_STATUS_errVENDOR		0x0B
/* Device detected unexpected USB reset signal */
#define DFU_STATUS_errUSBR		0x0C
/* Device detected unexpected power on reset */
#define DFU_STATUS_errPOR		0x0D
/* Unknown error */
#define DFU_STATUS_errUNKNOWN		0x0E
/* Device stalled an unexpected request */
#define DFU_STATUS_errSTALLEDPKT	0x0F

/* Device is running its normal application. */
#define DFU_STATE_appIDLE			0
/* Device received DFU_DETACH and waiting for USB reset. */
#define DFU_STATE_appDETACH			1
/* Device is operating in DFU mode and waiting for requests. */
#define DFU_STATE_dfuIDLE			2
/* Device has received a block and waiting for host to solicit the status
 * via DFU_GETSTATUS.
 */
#define DFU_STATE_dfuDNLOAD_SYNC		3     
/* Device is programming a control-write block into its nonvolatile
 * memories.
 */
#define DFU_STATE_dfuDNBUSY			4     
/* Device is processing a download operation, expecting DFU_DNLOAD
 * requests.
 */
#define DFU_STATE_dfuDNLOAD_IDLE		5     
/* Device has recieved the final block of firmware from host and is
 * waiting for receipt of DFU_GETSTATUS to begin the manifestation phase.
 * Or device has completed the manifestion phase and waiting for
 * DFU_GETSTATUS.
 */
#define DFU_STATE_dfuMANIFEST_SYNC		6     
/* Device in Manifestation phase (Not all devices can response to
 * DFU_GETSTATUS when in this state).
 */
#define DFU_STATE_dfuMANIFEST			7     
/* Device has programmed its memories and is waiting for a USB reset or a
 * power on reset.
 */
#define DFU_STATE_dfuMANIFEST_WAIT_RESET	8
/* Device is processing an upload operation, expecting DFU_UPLOAD
 * requests.
 */
#define DFU_STATE_dfuUPLOAD_IDLE		9
/* An error occurred, awaiting DFU_CLRSTATUS request */
#define DFU_STATE_dfuERROR			10

#define MAX_DFU_SN				16
#define MAX_DFU_NAME				64
#define MAX_DFU_INFO				256

struct dfu_info {
	/* IDs used to match target DFU device */
	char ucInterfaceName[MAX_DFU_NAME];	/* device name */

	uint16_t idVendor;
	uint16_t idProduct;
	uint16_t bcdDevice;
	char ucSerialNumber[MAX_DFU_SN];	/* serial number */

	/* DFU capabilities */
	uint8_t bInterfaceProtocol;
	uint8_t bmAttributes;
	uint16_t wDetachTimeOut;
	uint16_t wTransferSize;
	uint16_t bcdDFUVersion;

	/* DFU controllables */
	uint8_t bState;
	uint8_t bStatus;
	uint8_t iString;			/* addtitional status string ID */
};

/* last sequece status */
struct dfu_seq {
	int seq;
#define DFU_SEQ_IDLE		0x00
#define DFU_SEQ_DNLOAD		0x01 /* Write is allowed */
#define DFU_SEQ_UPLOAD		0x02 /* Read is allowed */
#define DFU_SEQ_DETACH		0x03
#define DFU_SEQ_ABORT		0x04
	int seq_ended;
	/* DFU error status */
	uint8_t seq_error;
};

struct dfu_firm {
	uint16_t idVendor;
	uint16_t idProduct;
	uint16_t bcdDevice;
};

#define DFU_SUFFIX_SIZE			16
#define DFU_SUFFIX_MAGIC		0x10444655	/* 16+DFU */
#define DFU_VERSION			0x0100

#define USB_DFU_IOCTL_INDEX	0x0011
#define USBDFUIODETACCH		(USB_DFU_IOCTL_INDEX+DFU_DETACH)
#define USBDFUIODNLOAD		(USB_DFU_IOCTL_INDEX+DFU_DNLOAD)
#define USBDFUIOUPLOAD		(USB_DFU_IOCTL_INDEX+DFU_UPLOAD)
#define USBDFUIOGETSTATUS	(USB_DFU_IOCTL_INDEX+DFU_GETSTATUS)
#define USBDFUIOCLRSTATUS	(USB_DFU_IOCTL_INDEX+DFU_CLRSTATUS)
#define USBDFUIOGETSTATE	(USB_DFU_IOCTL_INDEX+DFU_GETSTATE)
#define USBDFUIOABORT		(USB_DFU_IOCTL_INDEX+DFU_ABORT)
#define USBDFUIOLASTSEQ		(USB_DFU_IOCTL_INDEX+DFU_LASTSEQ)
#define USBDFUIOGETINFO		(USB_DFU_IOCTL_INDEX+DFU_GETINFO)

#define USB_DFU_IOCTL_CODE(code, dir, method)			\
	IOCTL_CODE(code, dir, method)

#define IOCTL_USB_DFU_DETACH					\
	USB_DFU_IOCTL_CODE(USBDFUIODETACCH,			\
			   IOCTL_INPUT, METHOD_BUFFERED)
#define IOCTL_USB_DFU_DNLOAD					\
	USB_DFU_IOCTL_CODE(USBDFUIODNLOAD,			\
			   IOCTL_INPUT, METHOD_BUFFERED)
#define IOCTL_USB_DFU_UPLOAD					\
	USB_DFU_IOCTL_CODE(USBDFUIOUPLOAD,			\
			   IOCTL_OUTPUT, METHOD_BUFFERED)
#if 0
#define IOCTL_USB_DFU_GETSTATUS					\
	USB_DFU_IOCTL_CODE(USBDFUIOGETSTATUS,			\
			   IOCTL_OUTPUT, METHOD_BUFFERED)
#endif
#define IOCTL_USB_DFU_CLRSTATUS					\
	USB_DFU_IOCTL_CODE(USBDFUIOCLRSTATUS,			\
			   IOCTL_INPUT, METHOD_BUFFERED)
#if 0
#define IOCTL_USB_DFU_GETSTATE					\
	USB_DFU_IOCTL_CODE(USBDFUIOGETSTATE,			\
			   IOCTL_OUTPUT, METHOD_BUFFERED)
#endif
#define IOCTL_USB_DFU_ABORT					\
	USB_DFU_IOCTL_CODE(USBDFUIOABORT,			\
			   IOCTL_INPUT, METHOD_BUFFERED)
#define IOCTL_USB_DFU_LASTSEQ					\
	USB_DFU_IOCTL_CODE(USBDFUIOLASTSEQ,			\
			   IOCTL_OUTPUT, METHOD_BUFFERED)
#define IOCTL_USB_DFU_GETINFO					\
	USB_DFU_IOCTL_CODE(USBDFUIOGETINFO,			\
			   IOCTL_OUTPUT, METHOD_BUFFERED)

#endif /* __USBDFU_H_INCLUDE__ */
