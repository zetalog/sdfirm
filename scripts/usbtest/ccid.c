#include "ccid_priv.h"

#define CCID_TIMEOUT			0	/* msec */
#define RFU				-1

struct usb_driver_ctx g_ccid_ctx;
scd_desc_t g_ccid_desc;
struct libusb_device_descriptor g_ccid_device;
struct libusb_interface_descriptor g_ccid_intf;
struct libusb_config_descriptor g_ccid_conf;

static uint8_t g_ccid_in;
static uint8_t g_ccid_out;
static uint8_t g_ccid_intr;
uint8_t g_ccid_seq = 0;

uint8_t g_ccid_cmd[CCID_MAX_BUFFER];
uint8_t g_ccid_resp[CCID_MAX_BUFFER];

int ccid_timeout = CCID_TIMEOUT;

LIST_HEAD(g_ccid_cmds);

#define for_each_cmd(c)	\
	list_for_each_entry(struct ccid_cmd, c, &g_ccid_cmds, link)
#define for_each_cmd_safe(c, n)	\
	list_for_each_entry_safe(struct ccid_cmd, c, n, &g_ccid_cmds, link)

struct ccid_error {
	/* mask */
	uint8_t bIccStatus;
	uint8_t bError;
	const char *desc;
};

#define CCID_MAX_ERRORS		0x20
struct ccid_resp_errors {
	uint8_t cmd;
	uint8_t errors[CCID_MAX_ERRORS];
#define CCID_ERROR_ANY		0x00
#define CCID_ERROR_END		0xFF
};

struct ccid_error ccid_errors[CCID_MAX_ERRORS] = {
/*  1 */{
		CCID_ICC_STATUS_NOTPRESENT, 0x05,
		"bSlot does not exist"
	},
/*  2 */{
		CCID_ICC_STATUS_NOTPRESENT, CCID_ERROR_ICC_MUTE,
		"No ICC present"
	},
/*  3 */{
		CCID_ICC_STATUS_INACTIVE, CCID_ERROR_HW_ERROR,
		"Hardware error"
	},
/*  4 */{
		CCID_ICC_STATUS_INACTIVE, 0x07,
		"bPowerselect error(not supported)"
	},
/*  5 */{
		CCID_ICC_STATUS_INACTIVE, CCID_ERROR_XFR_PARITY_ERROR,
		"parity error on ATR"
	},
/*  6 */{
		CCID_ICC_STATUS_INACTIVE, CCID_ERROR_ICC_MUTE,
		"ICC mute (Time out)"
	},
/*  7 */{
		CCID_ICC_STATUS_INACTIVE, CCID_ERROR_BAD_ATR_TS,
		"Bad TS in ATR"
	},
/*  8 */{
		CCID_ICC_STATUS_INACTIVE, CCID_ERROR_BAD_ATR_TCK,
		"Bad TCK in ATR"
	},
/*  9 */{
		CCID_ICC_STATUS_INACTIVE, CCID_ERROR_PROTO_UNSUPPORT,
		"Protocol not managed"
	},
/*  a */{
		CCID_ICC_STATUS_INACTIVE, CCID_ERROR_ICC_CLASS_NOSUP,
		"ICC class not supported"
	},
/*  b */{
		CCID_ICC_STATUS_INACTIVE, CCID_ERROR_CMD_ABORTED,
		"Command aborted by control pipe"
	},
/*  c */{
		CCID_ICC_STATUS_ACTIVE, 0x00,
		"Command Not Supported"
	},
/*  d */{
		CCID_ICC_STATUS_ACTIVE, CCID_ERROR_CMD_SLOT_BUSY,
		"CCID_ERR_SLOT_BUSY"
	},
/*  e */{
		CCID_ICC_STATUS_INACTIVE, CCID_ERROR_CMD_SLOT_BUSY,
		"CCID_ERR_SLOT_BUSY"
	},
/*  f */{
		CCID_ICC_STATUS_NOTPRESENT, CCID_ERROR_CMD_SLOT_BUSY,
		"CCID_ERR_SLOT_BUSY"
	},
/* 10 */{
		CCID_ICC_STATUS_INACTIVE, CCID_ERROR_BUSY_AUTO_SEQ,
		"Automatic sequence on-going"
	},
/* 11 */{
		CCID_ICC_STATUS_ACTIVE, 0x07,
		"bPowerselect error(not supported)"
	},
/* 12 */{
		CCID_ICC_STATUS_ACTIVE, CCID_ERROR_XFR_PARITY_ERROR,
		"parity error"
	},
/* 13 */{
		CCID_ICC_STATUS_ACTIVE, CCID_ERROR_XFR_OVERRUN,
		"CCID_ERR_XFR_OVERRUN"
	},
/* 14 */{
		CCID_ICC_STATUS_ACTIVE, CCID_ERROR_ICC_MUTE,
		"ICC mute (Time out)"
	},
/* 15 */{
		CCID_ICC_STATUS_ACTIVE, 0x08,
		"Bad wLevelParameter"
	},
/* 16 */{
		CCID_ICC_STATUS_ACTIVE, 0x01,
		"Bad dwLength"
	},
/* 17 */{
		CCID_ICC_STATUS_ACTIVE, CCID_ERROR_CMD_ABORTED,
		"Command aborted by control pipe"
	},
/* 18 */{
		CCID_ICC_STATUS_ACTIVE, 0x07,
		"Protocol invalid or not supported"
	},
/* 19 */{
		CCID_ICC_STATUS_ACTIVE, 0x10,
		"FI ¨C DI pair invalid or not supported"
	},
/* 1a */{
		CCID_ICC_STATUS_ACTIVE, 0x11,
		"Invalid TCCKTS parameter"
	},
/* 1b */{
		CCID_ICC_STATUS_ACTIVE, 0x12,
		"Guard time not supported"
	},
/* 1c */{
		CCID_ICC_STATUS_ACTIVE, 0x13,
		"(T=0)WI/(T=1)BWI/(T=1)CWI invalid or not supported"
	},
/* 1d */{
		CCID_ICC_STATUS_ACTIVE, 0x14,
		"Clock stop support requested invalid or not supported"
	},
/* 1e */{
		CCID_ICC_STATUS_ACTIVE, 0x15,
		"IFSC size invalid or not supported"
	},
/* 1f */{
		CCID_ICC_STATUS_ACTIVE, 0x16,
		"NAD value invalid or not supported"
	},
	{
		0,	    0,
		NULL
	},
};

struct ccid_resp_errors ccid_valid_errors[] = {
	{
		CCID_PC2RDR_ICCPOWERON, 
		{
			0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
			0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
			CCID_ERROR_END,
		},
	},
	{
		CCID_PC2RDR_ICCPOWEROFF,
		{
			0x01, 0x10, 0x0C, 0x0D, 0x0E, 0x0F,
			CCID_ERROR_END,
		},
	},
	{
		CCID_PC2RDR_GETSLOTSTATUS,
		{
			0x01, 0x02, 0x03, 0x0C, 0x0D, 0x0E, 0x0F,
			CCID_ERROR_END,
		},
	},
	{
		CCID_PC2RDR_XFRBLOCK,
		{
			0x01, 0x02, 0x10, 0x03, 0x0C, 0x0D, 0x0E, 0x0F,
			0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
			CCID_ERROR_END,
		},
	},

	{
		CCID_PC2RDR_GETPARAMETERS,
		{
			0x01, 0x02, 0x10, 0x03, 0x0C, 0x0D, 0x0E, 0x0F,
			CCID_ERROR_END,
		},
	},
	{
		CCID_PC2RDR_RESETPARAMETERS,
		{
			0x01, 0x02, 0x10, 0x03, 0x0C, 0x0D, 0x0E, 0x0F,
			CCID_ERROR_END,
		},
	},
	{
		CCID_PC2RDR_SETPARAMETERS,
		{
			0x01, 0x02, 0x10, 0x03, 0x0C, 0x0D, 0x0E, 0x0F,
			0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
			CCID_ERROR_END,
		},
	},
	{
		CCID_PC2RDR_ESCAPE,
		{
			0x01, 0x0C, 0x0D, 0x0E, 0x0F, 0x0B,
			CCID_ERROR_ANY,
			CCID_ERROR_END, 
		},
	},
	{
		CCID_PC2RDR_ICCCLOCK,
		{
			0x01, 0x02, 0x10, 0x03, 0x0C, 0x0D, 0x0E, 0x0F,
			CCID_ERROR_END, 
		},
	},
	{
		CCID_PC2RDR_T0APDU,
		{
			0x01, 0x02, 0x09, 0x03, 0x0B, 0x0C,
			CCID_ERROR_END, 
		},
	},
	{
		CCID_PC2RDR_SECURE,
		{
			0x01, 0x02, 0x10, 0x03, 0x0C, 0x0D, 0x0E, 0x0F,
			0x05, 0x06, 0x0B,
			CCID_ERROR_END, 
		},
	},
	{
		CCID_PC2RDR_MECHANICAL,
		{
			0x01, 0x02, 0x10, 0x03, 0x0C, 0x0D, 0x0E, 0x0F,
			CCID_ERROR_END, 
		},
	},
	{
		CCID_PC2RDR_ABORT,
		{
			0x01, 0x0C, 0x0D, 0x0E, 0x0F,
			CCID_ERROR_END, 
		},
	},
	{
		CCID_PC2RDR_SETDATAANDFREQ,
		{
			0x01, 0x0C,
			CCID_ERROR_END, 
		},
	},
	{
		0,
		{
			CCID_ERROR_END,
		},
	},
};

struct Fi_table {
	int Fi;
	int f;	/* f(max) MHz */
} g_Fi_table[16] = {
	{  372,   4 },
	{  372,   5 },
	{  558,   6 },
	{  744,   8 },
	{ 1116,  12 },
	{ 1488,  16 },
	{ 1860,  20 },
	{  RFU, RFU },
	{  RFU, RFU },
	{  512,   5 },
	{  768,   7 },	/* FIXME: */
	{ 1024,  10 },
	{ 1536,  15 },
	{ 2048,  20 },
	{  RFU, RFU },
	{  RFU, RFU },
};

int g_Di_table[16] = {	
	/* Di */
	RFU,   1,   2,   4,   8,  16,  32,  64,
	 12,  20, RFU, RFU, RFU, RFU, RFU, RFU,
};

static void ccid_dump_scd_desc(void)
{
	scd_desc_t *d = &g_ccid_desc;
	char *p, v_str[20];
	char p_str[20];

	memset(v_str, 0, 20);
	memset(p_str, 0, 20);

	p = v_str;

	if (d->bVoltageSupport & 0x01) {
		strcpy(p, "5.0v ");
		p += 5;
	}
	if (d->bVoltageSupport & 0x02) {
		strcpy(p, "3.0v ");
		p += 5;
	}
	if (d->bVoltageSupport & 0x04) {
		strcpy(p, "1.8v");
		p += 4;
	}
	*p = '\0';

	p = p_str;

	if (d->dwProtocols & 0x01) {
		strcpy(p, "T=0 ");
		p += 4;
	}
	if (d->dwProtocols & 0x02) {
		strcpy(p, "T=1");
		p += 3;
	}
	*p = '\0';

	ccid_log(CCID_LOG_DEBUG, "Smart Card Device Descriptor:");	
	ccid_log(CCID_LOG_DEBUG, "  idVendor            0x%04x",
		 g_ccid_ctx.idVendor);
	ccid_log(CCID_LOG_DEBUG, "  idProduct           0x%04x",
		 g_ccid_ctx.idProduct);
	ccid_log(CCID_LOG_DEBUG, "  bcdDevice           0x%04x",
		 g_ccid_ctx.bcdDevice);

	ccid_log(CCID_LOG_DEBUG, "  bLength             %d",
		 d->bLength);
	ccid_log(CCID_LOG_DEBUG, "  bDescriptorType     %d",
		 d->bDescriptorType);
	ccid_log(CCID_LOG_DEBUG, "  bcdCCID             0x%04Xh",
		 d->bcdCCID);
	ccid_log(CCID_LOG_DEBUG, "  bMaxSlotIndex       %d",
		 d->bMaxSlotIndex);
	ccid_log(CCID_LOG_DEBUG, "  bVoltageSupport     %d (%s)",
		 d->bVoltageSupport, v_str);
	ccid_log(CCID_LOG_DEBUG, "  dwProtocols         %u (%s)",
		 d->dwProtocols, p_str);
	ccid_log(CCID_LOG_DEBUG, "  dwDefaultClock      %u",
		 d->dwDefaultClock);
	ccid_log(CCID_LOG_DEBUG, "  dwMaxiumumClock     %u",
		 d->dwMaximumClock);
	ccid_log(CCID_LOG_DEBUG, "  bNumClockSupported  %d",
		 d->bNumClockSupported);
	ccid_log(CCID_LOG_DEBUG, "  dwDataRate          %u bps",
		 d->dwDataRate);
	ccid_log(CCID_LOG_DEBUG, "  dwMaxDataRate       %u bps",
		 d->dwMaxDataRate);
	ccid_log(CCID_LOG_DEBUG, "  bNumDataRatesSupp   %d",
		 d->bNumDataRatesSupported);
	ccid_log(CCID_LOG_DEBUG, "  dwMaxIFSD           %u",
		 d->dwMaxIFSD);
	ccid_log(CCID_LOG_DEBUG, "  dwSyncProtocols     %d",
		 d->dwSynchProtocols);
	ccid_log(CCID_LOG_DEBUG, "  dwMechanical        %d",
		 d->dwMechanical);
	ccid_log(CCID_LOG_DEBUG, "  dwFeatures          0x%-8X",
		 d->dwFeatures);
	if (d->dwFeatures & 0x00000002)
		ccid_log(CCID_LOG_DEBUG, "    Auto configuration based on ATR");
	if (d->dwFeatures & 0x00000004)
		ccid_log(CCID_LOG_DEBUG, "    Automatic activation on ICC inserting");
	if (d->dwFeatures & 0x00000008)
		ccid_log(CCID_LOG_DEBUG, "    Automatic voltage selection");
	if (d->dwFeatures & 0x00000010)
		ccid_log(CCID_LOG_DEBUG, "    Automatic clock frequency change");
	if (d->dwFeatures & 0x00000020)
		ccid_log(CCID_LOG_DEBUG, "    Automatic baud rate change");
	if (d->dwFeatures & 0x00000040)
		ccid_log(CCID_LOG_DEBUG, "    Automatic parameters negotiation");
	if (d->dwFeatures & 0x00000080)
		ccid_log(CCID_LOG_DEBUG, "    Automatic PPS made by the CCID according to the active parameters");
	if (d->dwFeatures & 0x00000100)
		ccid_log(CCID_LOG_DEBUG, "    CCID can set ICC in clock stop mode");
	if (d->dwFeatures & 0x00000200)
		ccid_log(CCID_LOG_DEBUG, "    NAD value other than 00 accepted (T=1 protocol in use)");
	if (d->dwFeatures & 0x00000400)
		ccid_log(CCID_LOG_DEBUG, "    Automatic IFSD exchange as first exchange (T=1 protocol in use)");
	if (d->dwFeatures & 0x00010000)
		ccid_log(CCID_LOG_DEBUG, "    TPDU level");
	else if (d->dwFeatures & 0x00020000)
		ccid_log(CCID_LOG_DEBUG, "    Short APDU level");
	else if (d->dwFeatures & 0x00040000)
		ccid_log(CCID_LOG_DEBUG, "    Short and Extended APDU level");
	else
		ccid_log(CCID_LOG_DEBUG, "    Character level");
#if 0
	if (d->dwFeatures & 0x00000040)
		ccid_log(CCID_LOG_DEBUG, "    Host shall not try to change the FI, DI, and protocol currently selected.");
#endif
	ccid_log(CCID_LOG_DEBUG, "  dwMaxCCIDMsgLen     %d", d->dwMaxCCIDMessageLength);
	ccid_log(CCID_LOG_DEBUG, "  bClassGetResponse   %d", d->bClassGetResponse);
	ccid_log(CCID_LOG_DEBUG, "  bClassEnvelope      %d", d->bClassEnvelope);
	ccid_log(CCID_LOG_DEBUG, "  wlcdLayout          0x%04X", d->wLcdLayout);
	ccid_log(CCID_LOG_DEBUG, "  bPINSupport         0x%02X", d->bPINSupport);
	ccid_log(CCID_LOG_DEBUG, "  bMaxCCIDBusySlots   %d", d->bMaxCCIDBusySlots);
}

void ccid_dump_dev_desc(void)
{
	struct libusb_device_descriptor *d = &g_ccid_device;

	ccid_log(CCID_LOG_DEBUG, "USB Device Descriptor:");	
	ccid_log(CCID_LOG_DEBUG, "  bcdUSB               0x%02x", d->bcdUSB);
	ccid_log(CCID_LOG_DEBUG, "  bDeviceClass         %d", d->bDeviceClass);
	ccid_log(CCID_LOG_DEBUG, "  bDeviceSubClass      %d", d->bDeviceSubClass);
	ccid_log(CCID_LOG_DEBUG, "  bDeviceProtocol      %d", d->bDeviceProtocol);
	ccid_log(CCID_LOG_DEBUG, "  bMaxPacketSize0      %d", d->bMaxPacketSize0);
	ccid_log(CCID_LOG_DEBUG, "  idVendor             0x%02x", d->idVendor);
	ccid_log(CCID_LOG_DEBUG, "  idProduct            0x%02x", d->idProduct);
	ccid_log(CCID_LOG_DEBUG, "  bcdDevice            0x%02x", d->bcdDevice);
	ccid_log(CCID_LOG_DEBUG, "  iManufacturer        %d", d->iManufacturer);
	ccid_log(CCID_LOG_DEBUG, "  iProduct             %d", d->iProduct);
	ccid_log(CCID_LOG_DEBUG, "  iSerialNumber        %d", d->iSerialNumber);
	ccid_log(CCID_LOG_DEBUG, "  bNumConfigurations   %d", d->bNumConfigurations);
}

void ccid_dump_intf_desc(void)
{
	struct libusb_interface_descriptor *d = &g_ccid_intf;

	ccid_log(CCID_LOG_DEBUG, "USB Interface Descriptor:");
	ccid_log(CCID_LOG_DEBUG, "  bInterfaceNumber     %d", d->bInterfaceNumber);
	ccid_log(CCID_LOG_DEBUG, "  bAlternateSetting    %d", d->bAlternateSetting);
	ccid_log(CCID_LOG_DEBUG, "  bNumEndpoints        %d", d->bNumEndpoints);
	ccid_log(CCID_LOG_DEBUG, "  bInterfaceClass      %d", d->bInterfaceClass);
	ccid_log(CCID_LOG_DEBUG, "  bInterfaceSubClass   %d", d->bInterfaceSubClass);
	ccid_log(CCID_LOG_DEBUG, "  bInterfaceProtocol   %d", d->bInterfaceProtocol);
	ccid_log(CCID_LOG_DEBUG, "  iInterface           %d", d->iInterface);
}

void ccid_dump_conf_desc(void)
{
	struct libusb_config_descriptor *d = &g_ccid_conf;

	ccid_log(CCID_LOG_DEBUG, "USB Configuration Descriptor:");
	ccid_log(CCID_LOG_DEBUG, "  wTotalLength         %d", d->wTotalLength);
	ccid_log(CCID_LOG_DEBUG, "  bNumInterfaces       %d", d->bNumInterfaces);
	ccid_log(CCID_LOG_DEBUG, "  bConfigurationValue  0x%02x", d->bConfigurationValue);
	ccid_log(CCID_LOG_DEBUG, "  iConfiguration       %d", d->iConfiguration);
	ccid_log(CCID_LOG_DEBUG, "  bmAttributes         %02x", d->bmAttributes);
	ccid_log(CCID_LOG_DEBUG, "  bMaxPower            %d mA", 2*d->MaxPower);
}

static boolean ccid_parse_device(void)
{
	struct libusb_device_descriptor *ccid_dev = &g_ccid_device;

	usb_get_device_descriptor(ccid_dev);

	if (ccid_dev->bLength != LIBUSB_DT_DEVICE_SIZE ||
	    ccid_dev->bDescriptorType != LIBUSB_DT_DEVICE) {
		return false;
	}

	ccid_dump_dev_desc();
	return true;
}

static boolean ccid_parse_config(uint8_t bConfigurationValue)
{
	struct libusb_config_descriptor *ccid_conf;

	usb_get_config_descriptor(bConfigurationValue, &ccid_conf);
	g_ccid_conf = *ccid_conf;

	ccid_dump_conf_desc();
	libusb_free_config_descriptor(ccid_conf);
	return true;
}

static boolean ccid_parse_interface(const struct libusb_interface_descriptor *desc)
{
	struct libusb_interface_descriptor *ccid_intf = &g_ccid_intf;

	*ccid_intf = *desc;
	if (ccid_intf->bLength != LIBUSB_DT_INTERFACE_SIZE ||
	    ccid_intf->bDescriptorType != LIBUSB_DT_INTERFACE) {
		return false;
	}

	ccid_dump_intf_desc();
	return true;
}

static boolean ccid_parse_scd_desc(const uint8_t *src, size_t len)
{
	scd_desc_t *ccid_desc = &g_ccid_desc;

	if (len != SCD_DT_SCD_SIZE)
		return false;

	ccid_desc->bLength = src[0];
	ccid_desc->bDescriptorType = src[1];
	ccid_desc->bcdCCID = w2i(src + 2);
	ccid_desc->bMaxSlotIndex = src[4];
	ccid_desc->bVoltageSupport = src[5];
	ccid_desc->dwProtocols = dw2i(src + 6);
	ccid_desc->dwDefaultClock = dw2i(src + 10);
	ccid_desc->dwMaximumClock = dw2i(src + 14);
	ccid_desc->bNumClockSupported = src[18];
	ccid_desc->dwDataRate = dw2i(src + 19);
	ccid_desc->dwMaxDataRate = dw2i(src + 23);
	ccid_desc->bNumDataRatesSupported = src[27];
	ccid_desc->dwMaxIFSD = dw2i(src + 28);
	ccid_desc->dwSynchProtocols = dw2i(src + 32);
	ccid_desc->dwMechanical = dw2i(src + 36);
	ccid_desc->dwFeatures = dw2i(src + 40);
	ccid_desc->dwMaxCCIDMessageLength = dw2i(src + 44);
	ccid_desc->bClassGetResponse = src[48];
	ccid_desc->bClassEnvelope = src[49];
	ccid_desc->wLcdLayout = w2i(src + 50);
	ccid_desc->bPINSupport = src[52];
	ccid_desc->bMaxCCIDBusySlots = src[53];

	ccid_dump_scd_desc();
	return true;
}

void ccid_get_config(struct libusb_config_descriptor *desc)
{
	*desc = g_ccid_conf;
}

void ccid_get_device(struct libusb_device_descriptor *desc)
{
	*desc = g_ccid_device;
}

void ccid_get_interface(struct libusb_interface_descriptor *desc)
{
	*desc = g_ccid_intf;
}

void ccid_get_descriptor(scd_desc_t *desc)
{
	*desc = g_ccid_desc;
}

static const char *ccid_icc_type(uint8_t status)
{
	switch (status) {
	case CCID_ICC_STATUS_NOTPRESENT:
		return "Not present";
	case CCID_ICC_STATUS_INACTIVE:
		return "Inactive";
	case CCID_ICC_STATUS_ACTIVE:
		return "Active";
	default:
		return "Unknown";
	}
}

static const char *ccid_err_type(uint8_t error)
{
	int i;

	for (i = 0; i < (sizeof (ccid_errors) / sizeof (struct ccid_error)); i++) {
		if (ccid_errors[i].bError == error)
			return ccid_errors[i].desc;
	}
	return "Unknown";
}

static const char *ccid_int_type(uint8_t type)
{
	switch (type) {
	case CCID_RDR2PC_NOTIFYSLOTCHANGE:
		return "RDR_to_PC_NotifyChange   ";
	case CCID_RDR2PC_HARDWAREERROR:
		return "RDR_to_PC_HardwareError  ";
	default:
		return "Unknown";
	}
}

static const char *ccid_ctl_type(uint8_t type)
{
	switch (type) {
	case CCID_REQ_ABORT:
		return "ABORT                    ";
	case CCID_REQ_GET_CLOCK_FREQS:
		return "GET CLOCK FREQUENCIES    ";
	case CCID_REQ_GET_DATA_RATES:
		return "GET DATA RATES           ";
	default:
		return "Unknown";
	}
}

static const char *ccid_msg_type(uint8_t type)
{
	switch (type) {
	case CCID_PC2RDR_ICCPOWERON:
		return "PC_to_RDR_IccPowerOn     ";
	case CCID_PC2RDR_ICCPOWEROFF:
		return "PC_to_RDR_IccPowerOff    ";
	case CCID_PC2RDR_SETPARAMETERS:
		return "PC_to_RDR_SetParameters  ";
	case CCID_PC2RDR_GETPARAMETERS:
		return "PC_to_RDR_GetParameters  ";
	case CCID_PC2RDR_RESETPARAMETERS:
		return "PC_to_RDR_ResetParameters";
	case CCID_PC2RDR_ABORT:
		return "PC_to_RDR_Abort          ";
	case CCID_PC2RDR_XFRBLOCK:
		return "PC_to_RDR_XfrBlock       ";
	case CCID_PC2RDR_GETSLOTSTATUS:
		return "PC_to_RDR_GetSlotStatus  ";
	case CCID_PC2RDR_ESCAPE:
		return "PC_to_RDR_Escape         ";
	case CCID_PC2RDR_SECURE:
		return "PC_to_RDR_Secure         ";
	case CCID_PC2RDR_T0APDU:
		return "PC_to_RDR_T0APDU         ";
	case CCID_PC2RDR_ICCCLOCK:
		return "PC_to_RDR_IccClock       ";
	case CCID_PC2RDR_MECHANICAL:
		return "PC_to_RDR_Mechanical     ";
	case CCID_PC2RDR_SETDATAANDFREQ:
		return "PC_to_RDR_SetDataAndFreq ";	
	case CCID_RDR2PC_PARAMETERS:
		return "RDR_to_PC_Parameters     ";	
	case CCID_RDR2PC_ESCAPE:
		return "RDR_to_PC_Escape         ";
	case CCID_RDR2PC_DATABLOCK:
		return "RDR_to_PC_DataBlock      ";
	case CCID_RDR2PC_SLOTSTATUS:
		return "RDR_to_PC_SlotStatus     ";
	case CCID_RDR2PC_DATARATEANDCLOCK:
		return "RDR_to_PC_DataAndFreq    ";
	default:
		return "Unknown";
	}
}

struct ccid_cmd *ccid_cmd_by_seq(uint8_t slot, uint8_t seq)
{
	struct ccid_cmd *c;

	for_each_cmd(c) {
		if (slot == c->bSlot && seq == c->bSeq)
			return c;
	}
	return NULL;
}

uint8_t ccid_seq_alloc(void)
{
	g_ccid_seq++;
	for (; g_ccid_seq < CCID_INVALID_SEQ; g_ccid_seq++) {
		if (!ccid_cmd_by_seq(CCID_SLOT_NO, g_ccid_seq)) 
			return g_ccid_seq;
	}
	return CCID_INVALID_SEQ;
}

boolean ccid_error_valid(void)
{
	uint8_t cmd = g_ccid_cmd[0];
	struct ccid_resp_errors *resp_err;
	uint8_t status = g_ccid_resp[7];
	uint8_t bError = g_ccid_resp[8];
	int i, items;
	uint8_t bIccStatus = (status & CCID_ICC_STATUS_MASK);

	if ((status & CCID_CMD_STATUS_MASK) != CCID_CMD_STATUS_FAIL)
		return true;

	for (i = 0; i < sizeof (ccid_valid_errors); i++) {
		int idx = 0;
		resp_err = &ccid_valid_errors[i];
		if (resp_err->cmd == 0)
			break;
		if (resp_err->cmd != cmd)
			continue;
		while (resp_err->errors[idx] != CCID_ERROR_END) {
			items = resp_err->errors[idx] - 1;
			
			if (bIccStatus == ccid_errors[items].bIccStatus &&
			    (ccid_errors[items].bError == bError ||
			     ccid_errors[items].bError == CCID_ERROR_ANY)) {
				return true;
			}
			idx++;
		}
		break;
	}
	ccid_log(CCID_LOG_ERR, "response error mismatched.");
	return false;
}

boolean ccid_msg_ispair(uint8_t cmd, uint8_t resp)
{
	switch (cmd) {
	case CCID_PC2RDR_ICCPOWERON:
	case CCID_PC2RDR_XFRBLOCK:
	case CCID_PC2RDR_SECURE:
		return (resp == CCID_RDR2PC_DATABLOCK);
	case CCID_PC2RDR_ICCPOWEROFF:
	case CCID_PC2RDR_GETSLOTSTATUS:
	case CCID_PC2RDR_ICCCLOCK:
	case CCID_PC2RDR_T0APDU:
	case CCID_PC2RDR_MECHANICAL:
	case CCID_PC2RDR_ABORT:
		return (resp == CCID_RDR2PC_SLOTSTATUS);
	case CCID_PC2RDR_SETPARAMETERS:
	case CCID_PC2RDR_GETPARAMETERS:
	case CCID_PC2RDR_RESETPARAMETERS:
		return (resp == CCID_RDR2PC_PARAMETERS);
	case CCID_PC2RDR_ESCAPE:
		return (resp == CCID_RDR2PC_ESCAPE);
	case CCID_PC2RDR_SETDATAANDFREQ:
		return (resp == CCID_RDR2PC_DATARATEANDCLOCK);
	default:
		return false;
	}
}

boolean ccid_resp_match(void)
{
	uint8_t cmd = g_ccid_cmd[0];
	uint8_t resp = g_ccid_resp[0];

	if (ccid_msg_ispair(cmd, resp) || resp == CCID_RDR2PC_SLOTSTATUS) {
		if (g_ccid_resp[6] == g_ccid_cmd[6]) {
			return true;
		}
	}
	ccid_log(CCID_LOG_ERR, "response is not valid according to CCID.");
	return false;
}

boolean ccid_resp_valid(uint32_t length)
{
	uint8_t cmd = g_ccid_cmd[0];
	uint8_t status = g_ccid_resp[7];

	if ((status & CCID_CMD_STATUS_MASK) == CCID_CMD_STATUS_FAIL) {
		return length == CCID_HEADER_SIZE;
	}

	switch (cmd) {
	case CCID_PC2RDR_ICCPOWEROFF:
	case CCID_PC2RDR_GETSLOTSTATUS:
	case CCID_PC2RDR_ICCCLOCK:
	case CCID_PC2RDR_T0APDU:
	case CCID_PC2RDR_MECHANICAL:
	case CCID_PC2RDR_ABORT:
		return length == CCID_HEADER_SIZE;
	case CCID_PC2RDR_SETPARAMETERS:
	case CCID_PC2RDR_GETPARAMETERS:
	case CCID_PC2RDR_RESETPARAMETERS:
		if (g_ccid_cmd[7] != g_ccid_resp[9])
			return false;
		switch (g_ccid_cmd[7]) {
		case SCD_PROTOCOL_T0:
			return (CCID_HEADER_SIZE + 5);
		case SCD_PROTOCOL_T1:
			return (CCID_HEADER_SIZE + 7);
		}
	case CCID_PC2RDR_SETDATAANDFREQ:
		return length == (CCID_HEADER_SIZE + 8);
	}
	return true;
}

void ccid_set_timeout(int timeout)
{
	ccid_timeout = timeout;
}

void ccid_reset_timeout(void)
{
	ccid_timeout = CCID_TIMEOUT;
}

boolean ccid_resp_sanity(void)
{
	uint32_t length = CCID_HEADER_SIZE+dw2i(g_ccid_resp+1);
	return ccid_resp_match() && ccid_resp_valid(length) && ccid_error_valid();
}

boolean __ccid_async_in(void)
{
	return (ccid_bulk_in(CCID_MAX_BUFFER) >= CCID_HEADER_SIZE);
}

boolean __ccid_async_out(void)
{
	uint32_t dwLength;
	dwLength = dw2i(g_ccid_cmd+1);
	return ccid_bulk_out((uint16_t)(dwLength + CCID_HEADER_SIZE)) == 0;
}

void ccid_async_in(void)
{
	uint8_t seq;
	struct ccid_cmd *cmd;

	while (__ccid_async_in()) {
		seq = g_ccid_resp[6];
		cmd = ccid_cmd_by_seq(CCID_SLOT_NO, seq);
		if (cmd) {
			cmd->cb(cmd->cb_data);
			list_del(&cmd->link);
			free(cmd);
		}

		if (list_empty(&g_ccid_cmds))
			return;
	}
}

boolean ccid_async_out(ccid_cmpl_cb cb, void *data)
{
	struct ccid_cmd *cmd;

	cmd = malloc(sizeof (struct ccid_cmd));
	if (!cmd)
		return false;

	INIT_LIST_HEAD(&cmd->link);
	cmd->bMessageType = g_ccid_cmd[0];
	cmd->bSlot = g_ccid_cmd[5];
	cmd->bSeq = g_ccid_cmd[6];
	cmd->cb = cb;
	cmd->cb_data = data;
	list_insert_tail(&cmd->link, &g_ccid_cmds);
	return __ccid_async_out();
}

boolean ccid_sync(void)
{
	uint32_t length = CCID_HEADER_SIZE + (dw2i(g_ccid_cmd+1));

	if (ccid_bulk_out(length) != 0)
		return false;
	length = ccid_bulk_in(CCID_MAX_BUFFER);
	if (length != (CCID_HEADER_SIZE+dw2i(g_ccid_resp+1)))
		return false;
	return ccid_resp_sanity();
}

boolean ccid_resp_fail(void)
{
	return (g_ccid_resp[7] & CCID_CMD_STATUS_MASK) != CCID_CMD_STATUS_SUCC;
}

uint32_t ccid_resp_length(void)
{
	return dw2i(g_ccid_resp+1);
}

uint8_t ccid_resp_error(void)
{
	return g_ccid_resp[8];
}

uint8_t ccid_resp_status(void)
{
	return (g_ccid_resp[7] & CCID_ICC_STATUS_MASK);
}

int ccid_resp_block(uint8_t *abData, uint32_t dwLength)
{
	int length = ccid_resp_length();
	if (length > 0)
		memcpy(abData, g_ccid_resp+CCID_HEADER_SIZE, length);
	return length;
}

void ccid_parse_T0Parameters(struct ccid_t0_param *t0)
{
}

void ccid_parse_T1Parameters(struct ccid_t1_param *t1)
{
}

void ccid_parse_Parameters(uint8_t bProtocolNum,
			   void *abProtocolDataStructure)
{
	switch (bProtocolNum) {
	case SCD_PROTOCOL_T0:
		ccid_parse_T0Parameters(abProtocolDataStructure);
	case SCD_PROTOCOL_T1:
		ccid_parse_T1Parameters(abProtocolDataStructure);
	}
}

uint8_t ccid_resp_param(void *abProtocolDataStructure)
{
	uint8_t bProtocolNum = g_ccid_resp[9];

	if (abProtocolDataStructure) {
		ccid_parse_Parameters(bProtocolNum, abProtocolDataStructure);
	}
	return bProtocolNum;
}

static void __ccid_build_common(uint8_t bMessageType, uint32_t dwLength,
				uint8_t bSlot, uint8_t bSeq,
				uint8_t bRFU0, uint8_t bRFU1, uint8_t bRFU2)
{
	g_ccid_cmd[0] = bMessageType;
	i2dw(dwLength, g_ccid_cmd+1);
	g_ccid_cmd[5] = bSlot;
	g_ccid_cmd[6] = bSeq;
	g_ccid_cmd[7] = bRFU0;
	g_ccid_cmd[8] = bRFU1;
	g_ccid_cmd[9] = bRFU2;
}

static void ccid_build_common(uint8_t bMessageType, uint32_t dwLength,
			      uint8_t bSeq, uint8_t bRFU0,
			      uint8_t bRFU1, uint8_t bRFU2)
{
	__ccid_build_common(bMessageType, dwLength,
			    CCID_SLOT_NO, bSeq,
			    bRFU0, bRFU1, bRFU2);
}

static uint8_t ccid_build_GetSlotStatus(void)
{
	uint8_t bSeq = ccid_seq_alloc();
	ccid_build_common(CCID_PC2RDR_GETSLOTSTATUS, 0,
			  bSeq, 0, 0, 0);
	return bSeq;
}

static uint8_t ccid_build_IccPowerOn(uint8_t bClass)
{
	uint8_t bSeq = ccid_seq_alloc();
	ccid_build_common(CCID_PC2RDR_ICCPOWERON, 0,
			  bSeq, bClass, 0, 0);
	return bSeq;
}

static uint8_t ccid_build_IccPowerOff(void)
{
	uint8_t bSeq = ccid_seq_alloc();
	ccid_build_common(CCID_PC2RDR_ICCPOWEROFF, 0,
			  bSeq, 0, 0, 0);
	return bSeq;
}

uint8_t ccid_build_GetParameters(uint8_t bProtocolNum)
{
	uint8_t bSeq = ccid_seq_alloc();
	ccid_build_common(CCID_PC2RDR_GETPARAMETERS, 0,
			  bSeq, bProtocolNum, 0, 0);
	return bSeq;
}

static uint8_t __ccid_build_SetParameters(uint8_t bProtocolNum,
					  uint32_t dwLength,
					  uint8_t bmFindexDindex,
					  uint8_t bmTCCKS,
					  uint8_t bGuardTime,
					  uint8_t bWaitingInteger,
					  uint8_t bClockStop)
{
	uint8_t bSeq = ccid_seq_alloc();
	ccid_build_common(CCID_PC2RDR_SETPARAMETERS, dwLength,
			  bSeq, bProtocolNum, 0, 0);
	g_ccid_cmd[10] = bmFindexDindex;
	g_ccid_cmd[11] = bmTCCKS;
	g_ccid_cmd[12] = bGuardTime;
	g_ccid_cmd[13] = bWaitingInteger;
	g_ccid_cmd[14] = bClockStop;
	return bSeq;
}

static uint8_t ccid_build_SetT0Parameters(struct ccid_t0_param *t0)
{
	return __ccid_build_SetParameters(SCD_PROTOCOL_T0,
					  5,
					  t0->bmFindexDindex,
					  t0->bmTCCKST0,
					  t0->bGuardTimeT0,
					  t0->bWaitingIntegerT0,
					  t0->bClockStop);
}

static uint8_t ccid_build_SetT1Parameters(struct ccid_t1_param *t1)
{
	g_ccid_cmd[15] = t1->bIFSC;
	g_ccid_cmd[16] = t1->bNadValue;
	return __ccid_build_SetParameters(SCD_PROTOCOL_T1,
					  7,
					  t1->bmFindexDindex,
					  t1->bmTCCKST1,
					  t1->bGuardTimeT1,
					  t1->bWaitingIntegerT1,
					  t1->bClockStop);
}

static uint8_t ccid_build_SetParameters(uint8_t bProtocolNum,
					void *abProtocolDataStructure)
{
	switch (bProtocolNum) {
	case SCD_PROTOCOL_T0:
		return ccid_build_SetT0Parameters((struct ccid_t0_param *)abProtocolDataStructure);
	case SCD_PROTOCOL_T1:
		return ccid_build_SetT1Parameters((struct ccid_t1_param *)abProtocolDataStructure);
	default:
		return -1;
	}
}

static uint8_t ccid_build_ResetParameters(void)
{
	uint8_t bSeq = ccid_seq_alloc();
	ccid_build_common(CCID_PC2RDR_RESETPARAMETERS, 0,
			  bSeq, 0, 0, 0);
	return bSeq;
}

static uint8_t ccid_build_XfrBlock(uint8_t bBWI, uint16_t wLevelParameter,
				   uint8_t *abData, uint32_t dwLength)
{
	uint8_t bSeq = ccid_seq_alloc();
	ccid_build_common(CCID_PC2RDR_XFRBLOCK, dwLength,
			  bSeq, bBWI,
			  (uint8_t)((wLevelParameter & 0xFF00) >> 8),
			  (uint8_t)(wLevelParameter & 0x00FF));
	memcpy(g_ccid_cmd+CCID_HEADER_SIZE, abData, dwLength);
	return bSeq;
}

static void ccid_build_PINOperation(struct ccid_po_param *po)
{
	g_ccid_cmd[11] = po->bTimeout;
	g_ccid_cmd[12] = po->bmFormatString;
	g_ccid_cmd[13] = po->bmPINBlockString;
	g_ccid_cmd[14] = po->bmPINLengthFormat;
}

static uint32_t ccid_build_PINVerification(struct ccid_pv_param *pv)
{
	struct ccid_po_param *po = (struct ccid_po_param *)pv;

	g_ccid_cmd[10] = CCID_SPE_PIN_VERIFY;
	ccid_build_PINOperation(po);
	i2w(pv->wPINMaxExtraDigit, g_ccid_cmd+15);
	g_ccid_cmd[17] = pv->bEntryValidationCondition;
	g_ccid_cmd[18] = pv->bNumberMessage;
	i2w(pv->wLangId, g_ccid_cmd+19);
	g_ccid_cmd[21] = pv->bMsgIndex;
	g_ccid_cmd[22] = pv->bTeoPrologue[0];
	g_ccid_cmd[23] = pv->bTeoPrologue[1];
	g_ccid_cmd[24] = pv->bTeoPrologue[2];
	return 25 - CCID_HEADER_SIZE;
}

static uint32_t ccid_build_PINModification(struct ccid_pm_param *pm)
{
	uint32_t dwLength, i;
	struct ccid_po_param *po = (struct ccid_po_param *)pm;

	g_ccid_cmd[10] = CCID_SPE_PIN_MODIFY;
	ccid_build_PINOperation(po);
	g_ccid_cmd[15] = pm->bInsertionOffsetOld;
	g_ccid_cmd[16] = pm->bInsertionOffsetNew;
	i2w(pm->wPINMaxExtraDigit, g_ccid_cmd+17);
	g_ccid_cmd[19] = pm->bConfirmPIN;
	g_ccid_cmd[20] = pm->bEntryValidationCondition;
	g_ccid_cmd[21] = pm->bNumberMessage;
	i2w(pm->wLangId, g_ccid_cmd+22);
	dwLength = 24;
	if (pm->bNumberMessage <= 0x03) {
		for (i = 0; i < pm->bNumberMessage; i++) {
			g_ccid_cmd[dwLength] = pm->bMsgIndex[i];
			dwLength++;
		}
	}
	for (i = 0; i < 3; i++) {
		g_ccid_cmd[dwLength] = pm->bTeoPrologue[i];
		dwLength++;
	}
	return dwLength - CCID_HEADER_SIZE;
}

static uint8_t ccid_build_Secure(uint8_t bBWI,
				 uint16_t wLevelParameter,
				 uint8_t bPINOperation,
				 void *abPINDataStructure,
				 uint8_t *abData, uint32_t dwLength)
{
	uint8_t bSeq = ccid_seq_alloc();
	uint32_t dwDSLength;
	switch (bPINOperation) {
	case CCID_SPE_PIN_VERIFY:
		dwDSLength = ccid_build_PINVerification((struct ccid_pv_param *)abPINDataStructure);
		break;
	case CCID_SPE_PIN_MODIFY:
		dwDSLength = ccid_build_PINModification((struct ccid_pm_param *)abPINDataStructure);
		break;
	default:
		ccid_log(CCID_LOG_ERR, "unsupported operation, op=%d", bPINOperation);
		return 0;
	}
	ccid_build_common(CCID_PC2RDR_SECURE, dwDSLength + dwLength,
			  bSeq, bBWI,
			  (uint8_t)((wLevelParameter & 0xFF00) >> 8),
			  (uint8_t)(wLevelParameter & 0x00FF));
	memcpy(g_ccid_cmd+CCID_HEADER_SIZE+dwDSLength, abData, dwLength);
	return bSeq;
}

static uint8_t ccid_build_SetDataAndFreq(uint32_t dwClockFrequency,
					 uint32_t dwDataRate)
{
	uint8_t bSeq = ccid_seq_alloc();
	ccid_build_common(CCID_PC2RDR_SETDATAANDFREQ, 8,
			  bSeq, 0, 0, 0);
	i2dw(dwClockFrequency, g_ccid_cmd + 10);
	i2dw(dwDataRate, g_ccid_cmd + 14);
	return bSeq;
}

uint8_t ccid_sync_SetDataAndFreq(uint32_t dwClockFrequency,
				 uint32_t dwDataRate)
{
	if (ccid_build_SetDataAndFreq(dwClockFrequency, dwDataRate) == CCID_INVALID_SEQ)
		return false;
	/* slot status is returned even when failure occurs */
	return ccid_sync();
}

#if 0
uint8_t ccid_build_T0APDU(uint8_t a, uint8_t b, uint8_t c)
{
	uint8_t bSeq = ccid_seq_alloc();
	ccid_build_common(CCID_PC2RDR_T0APDU, 0,
			  bSeq, a, b, c);
	return bSeq;
}
#endif

static uint8_t ccid_build_IccClock(uint8_t bClockCommand)
{
	uint8_t bSeq = ccid_seq_alloc();
	ccid_build_common(CCID_PC2RDR_ICCCLOCK, 0,
			  bSeq, bClockCommand, 0, 0);
	return bSeq;
}

static uint8_t ccid_build_Mechanical(uint8_t bFunction)
{
	uint8_t bSeq = ccid_seq_alloc();
	ccid_build_common(CCID_PC2RDR_MECHANICAL, 0,
			  bSeq, bFunction, 0, 0);
	return bSeq;
}

static void ccid_build_Abort(uint8_t bSeq)
{
	__ccid_build_common(CCID_PC2RDR_ABORT, 0,
			    CCID_SLOT_NO, bSeq, 0, 0, 0);
}

boolean ccid_sync_GetSlotStatus(void)
{
	if (ccid_build_GetSlotStatus() == CCID_INVALID_SEQ)
		return false;
	/* slot status is returned even when failure occurs */
	return ccid_sync();
}

boolean ccid_sync_IccPowerOn(uint8_t bClass)
{
	if (ccid_build_IccPowerOn(bClass) == CCID_INVALID_SEQ)
		return false;
	return ccid_sync() && !ccid_resp_fail();
}

boolean ccid_sync_IccPowerOff(void)
{
	if (ccid_build_IccPowerOff() == CCID_INVALID_SEQ)
		return false;
	return ccid_sync() && !ccid_resp_fail();
}

boolean ccid_sync_SetParameters(uint8_t bProtocolNum,
				void *abProtocolDataStructure)
{
	if (ccid_build_SetParameters(bProtocolNum,
				     abProtocolDataStructure) == CCID_INVALID_SEQ) {
		return false;
	}
	return ccid_sync() && !ccid_resp_fail();
}

boolean ccid_sync_GetParameters(uint8_t bProtocolNum)
{
	if (ccid_build_GetParameters(bProtocolNum) == CCID_INVALID_SEQ) {
		return false;
	}
	return ccid_sync() && !ccid_resp_fail();
}

boolean ccid_sync_ResetParameters(void)
{
	if (ccid_build_ResetParameters() == CCID_INVALID_SEQ) {
		return false;
	}
	return ccid_sync() && !ccid_resp_fail();
}

boolean ccid_sync_IccClock(uint8_t bClockCommand)
{
	if (ccid_build_IccClock(bClockCommand) == CCID_INVALID_SEQ) {
		return false;
	}
	return ccid_sync() && !ccid_resp_fail();
}

boolean ccid_sync_Mechanical(uint8_t bFunction)
{
	if (ccid_build_Mechanical(bFunction) == CCID_INVALID_SEQ) {
		return false;
	}
	return ccid_sync() && !ccid_resp_fail();
}

boolean ccid_sync_XfrBlock(uint8_t bBWI, uint16_t wLevelParameter,
			   uint8_t *abData, uint32_t dwLength)
{
	if (ccid_build_XfrBlock(bBWI, wLevelParameter,
				abData, dwLength) == CCID_INVALID_SEQ) {
		return false;
	}
	return ccid_sync() && !ccid_resp_fail();
}

boolean ccid_sync_Secure(uint8_t bBWI, uint16_t wLevelParameter,
			 uint8_t bPINOperation, void *abPINDataStructure,
			 uint8_t *abData, uint32_t dwLength)
{
	if (ccid_build_Secure(bBWI, wLevelParameter,
			      bPINOperation, abPINDataStructure,
			      abData, dwLength) == CCID_INVALID_SEQ) {
		return false;
	}
	return ccid_sync() && !ccid_resp_fail();
}

boolean ccid_async_Abort(uint8_t bSeq, ccid_cmpl_cb cb, void *data)
{
	ccid_build_Abort(bSeq);
	return ccid_async_out(cb, data);
}

uint8_t ccid_async_GetSlotStatus(ccid_cmpl_cb cb, void *data)
{
	uint8_t bSeq = ccid_build_GetSlotStatus();
	if (bSeq == CCID_INVALID_SEQ)
		return CCID_INVALID_SEQ;
	if (!ccid_async_out(cb, data))
		return CCID_INVALID_SEQ;
	return bSeq;
}

static int ccid_control_transfer(boolean class_spec, uint8_t dir,
				 uint8_t bRequest, uint16_t wValue,
				 uint8_t *data, uint16_t wLength)
{
	return usb_control_transfer((uint8_t)(dir |
				    (class_spec ? LIBUSB_REQUEST_TYPE_CLASS : LIBUSB_REQUEST_TYPE_STANDARD)|
				    LIBUSB_RECIPIENT_INTERFACE),
				    bRequest, wValue,
				    (uint16_t)(class_spec ? g_ccid_ctx.bInterfaceValue : 0),
				    data, wLength, ccid_timeout);
}

int ccid_ctrl_dev(uint8_t dir,
		  uint8_t bRequest, uint16_t wValue,
		  uint8_t *data, uint16_t wLength)
{
	return ccid_control_transfer(false, dir, bRequest, wValue,
				     data, wLength);
}

int ccid_ctrl_cls(uint8_t dir,
		  uint8_t bRequest, uint16_t wValue,
		  uint8_t *data, uint16_t wLength)
{
	ccid_log(CCID_LOG_INFO, "(0) [%s] %s wValue=%04x, wLength=%d",
		 ccid_ctl_type(bRequest),
		 dir == LIBUSB_ENDPOINT_IN ? "<<" : ">>",
		 wValue, wLength);
	return ccid_control_transfer(true, dir, bRequest, wValue,
				     data, wLength);
}

static boolean ccid_parse_endpoint(const struct libusb_interface_descriptor *dt_inf)
{
	int i, nr_ep = dt_inf->bNumEndpoints;

	g_ccid_in = -1;
	g_ccid_out = -1;

	for (i = 0; i < nr_ep; i++) {
		struct libusb_endpoint_descriptor *ep = 
			(struct libusb_endpoint_descriptor *)dt_inf->endpoint+i;

		usb_dump_ep_descriptor(ep);

		if ((LIBUSB_TRANSFER_TYPE_MASK & ep->bmAttributes) ==
		    LIBUSB_TRANSFER_TYPE_BULK) {
			if (ep->bEndpointAddress & 0x80) {
				g_ccid_in = ep->bEndpointAddress;
			} else {
				g_ccid_out = ep->bEndpointAddress;
			}
		}
		if ((LIBUSB_TRANSFER_TYPE_MASK &ep->bmAttributes) ==
		    LIBUSB_TRANSFER_TYPE_INTERRUPT) {
			g_ccid_intr = ep->bEndpointAddress;
		}
	}
	if (g_ccid_in == (unsigned char)-1 || g_ccid_out == (unsigned char)-1) {
		ccid_log(CCID_LOG_DEBUG, "config_endpoint failure.");
		return false;
	}
	ccid_log(CCID_LOG_INFO, "IN=%02X, OUT=%02x, INTR=%02x",
		 g_ccid_in & 0x0f, g_ccid_out & 0x0f, g_ccid_intr & 0x0f);
	return true;
}

static boolean ccid_config_interface(uint8_t bConfigurationValue,
				     const struct libusb_interface_descriptor *dt_inf)
{
	if (!ccid_parse_device() ||
	    !ccid_parse_config(bConfigurationValue) ||
	    !ccid_parse_interface(dt_inf))
		return false;

	if (ccid_parse_scd_desc(dt_inf->extra, dt_inf->extra_length)) {
		return ccid_parse_endpoint(dt_inf);
	}
	return false;
}

boolean ccid_probe_interface(void)
{
	return usb_probe_class_driver(0x0B, ccid_config_interface);
}

/* return value:
 *
 * send) = 0 on success 
 * recv) > 0 on success
 * 	 = 0 on failure
 */
static int ccid_bulk_transfer(int type, uint8_t *data, int len)
{
	int ret, transferred = 0;
	unsigned char ep = type ? g_ccid_out : g_ccid_in;
	char prefix[128];

	ret = usb_bulk_transfer(ep, data, len, &transferred, ccid_timeout);
	if (ret) {
		ccid_log(CCID_LOG_ERR, "usb_bulk_transfer failure.");
		return -1;
	}
	if (transferred == 0) {
		ccid_log(CCID_LOG_ERR,
			"bulk-%s error=%s", ep == g_ccid_out ? "out" : "in",
			strerror(errno));
		/* need not go on */
		return 0;
	}

	if (ep == g_ccid_out && transferred != len) {
		ccid_log(CCID_LOG_ERR, "bulk trans less data, transferred=%d, except send=%d\r\n",
			transferred, len);
		return 0;
	}
	sprintf(prefix, "[%s] %s", ccid_msg_type(data[0]),
		ep == g_ccid_in ? "<<" : ">>");
	log_dump(prefix, data, ep == g_ccid_in ? transferred : len);
	if (ep == g_ccid_in) {
		ccid_log(CCID_LOG_INFO, "cmd=%s, icc=%s, err=%s",
			ccid_resp_fail() ? "failure" : "success",
			ccid_icc_type(ccid_resp_status()),
			ccid_resp_fail() ? ccid_err_type(ccid_resp_error()) : "no error");
	}
	if (ep == g_ccid_in) 
		ret = transferred;
	return ret;
}

int ccid_bulk_out(int length)
{
	int r = ccid_bulk_transfer(CCID_ENDP_BULK_OUT, g_ccid_cmd, length);
	return r;
}

int ccid_bulk_in(int length)
{
	return ccid_bulk_transfer(CCID_ENDP_BULK_IN, g_ccid_resp, length);
}


int ccid_intr_in(uint8_t *data, int len)
{
	int ret, transferred = 0;
	unsigned char ep = g_ccid_intr;

	ret = usb_sync_interrupt_transfer(ep, data, len, &transferred, ccid_timeout);
	if (ret) {
		ccid_log(CCID_LOG_ERR, "ccid_intr_transfer failure.");
		return -1;
	}
	if (transferred == 0) {
		ccid_log(CCID_LOG_ERR,
			"intrrupt error=%s", strerror(errno));
		return 0;
	}

	ccid_log(CCID_LOG_INFO, "(0) [%s] << %s=%02x",
		 ccid_int_type(data[0]),
		 data[0] == CCID_RDR2PC_HARDWAREERROR ?
		 "bHardwareErrorCode" : "bmSlotICCState",
		 data[0] == CCID_RDR2PC_HARDWAREERROR ? data[3] : data[1]);
	return transferred;
}

int ccid_init(void)
{
	if (!ccid_probe_interface()) {
		ccid_log(CCID_LOG_ERR, "probe_interface failure.");
		return -1;
	}
	usb_get_context(&g_ccid_ctx);
	ccid_log(CCID_LOG_INFO, "probe_interface success.");
	return 0;
}

void ccid_exit(void)
{
}
