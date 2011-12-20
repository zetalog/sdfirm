/* collecting of another kind of implementation that can waste ROMs */

usb_device_desc_t usbd_desc_device = {
	USB_DT_DEVICE_SIZE,		/* bLength */
	USB_DT_DEVICE,			/* bDescriptorType */
	USB_VERSION_DEFAULT,		/* bcdUSB */
	USB_INTERFACE_CLASS_INTERFACE,	/* bDeviceClass */
	USB_DEVICE_SUBCLASS_NONE,	/* bDeviceSubClass */
	USB_PROTO_DEVICE_DEFAULT,	/* bDeviceProtocol */
	USBD_HW_CTRL_SIZE,		/* bMaxPacketSize0 */
	USBD_VENDOR_ID,			/* idVendor */
	USBD_PRODUCT_ID,		/* idProduct */
	system_device_id,		/* bcdDevice */
	USB_STRING_FACTORY,		/* iManufacture */
	USB_STRING_PRODUCT,		/* iProduct */
	USB_STRING_SERIALNO,		/* iSerialNumber */
	NR_USBD_CONFS,			/* bNumConfigurations */
};
	USBD_INB(usbd_desc_device.bLength);
	USBD_INB(usbd_desc_device.bDescriptorType);
	USBD_INW(usbd_desc_device.bcdUSB);
	USBD_INB(usbd_desc_device.bDeviceClass);
	USBD_INB(usbd_desc_device.bDeviceSubClass);
	USBD_INB(usbd_desc_device.bDeviceProtocol);
	USBD_INB(usbd_desc_device.bMaxPacketSize0);
	USBD_INW(usbd_desc_device.idVendor);
	USBD_INW(usbd_desc_device.idProduct);
	USBD_INW(usbd_desc_device.bcdDevice);
	USBD_INB(usbd_desc_device.iManufacture);
	USBD_INB(usbd_desc_device.iProduct);
	USBD_INB(usbd_desc_device.iSerialNumber);
	USBD_INB(usbd_desc_device.bNumConfigurations);

usb_conf_desc_t usbd_desc_config = {
	USB_DT_CONFIG_SIZE,		/* bLength */
	USB_DT_CONFIG,			/* bDescriptorType */
	USB_DT_CONFIG_SIZE,		/* !wTotalLength */
	0,				/* !bNumInterfaces */
	USB_CID2CONF(USB_CID_DEFAULT),	/* bConfigurationValue */
	USB_STRING_PRODUCT,		/* iConfiguration */
	USB_CONFIG_ATTR_DEFAULT,	/* bmAttributes */
	50 * USB_POWER_PER_2MA,		/* !bMaxPower */
};
	USBD_INB(usbd_desc_config.bLength);
	USBD_INB(usbd_desc_config.bDescriptorType);
	USBD_INW(usb_config_length());
	USBD_INB(usbd_nr_intfs);
	USBD_INB(usbd_desc_config.bConfigurationValue);
	USBD_INB(usbd_desc_config.iConfiguration);
	USBD_INB(usbd_desc_config.bmAttributes);
	USBD_INB(usbd_config_power);

usb_intf_desc_t ccid_desc_interface = {
	USB_DT_INTERFACE_SIZE,
	USB_DT_INTERFACE,
	0,				/* !bInterfaceNumber */
	0,
	0,				/* !bNumEndpoints */
	USB_INTERFACE_CLASS_CCID,
	USB_DEVICE_SUBCLASS_NONE,
	USB_INTERFACE_PROTOCOL_CCID,
	CCID_STRING_INTERFACE,
};
	USBD_INB(ccid_desc_interface.bLength);
	USBD_INB(ccid_desc_interface.bDescriptorType);
	USBD_INB(ccid_iid);
	USBD_INB(ccid_desc_interface.bAlternateSetting);
	USBD_INB(NR_CCID_ENDPS);
	USBD_INB(ccid_desc_interface.bInterfaceClass);
	USBD_INB(ccid_desc_interface.bInterfaceSubClass);
	USBD_INB(ccid_desc_interface.bInterfaceProtocol);
	USBD_INB(ccid_desc_interface.iInterface);

scd_desc_t scd_desc ccid_desc_device = {
	SCD_DT_SCD_SIZE,
	SCD_DT_SCD,
	CCID_VERSION_DEFAULT,
	(NR_IFD_SLOTS-1),
	SCD_VOLTAGE_ALL,
	SCD_PROTOCOL_T0,
	IFD_HW_FREQ_DEF,
	IFD_HW_FREQ_MAX,
	0,					/* !bNumClockSupported */
	IFD_HW_DATA_DEF,
	IFD_HW_DATA_MAX,
	0x00,					/* !bNumDataRatesSupported */
	IFD_T1_MAX_IFSD,
	SCD_SYNCH_PROTO_NONE,
	SCD_MECHA_NONE,
	SCD_FEATURE_NONE,
	0,					/* !dwMaxCCIDMessageLength */
	SCD_MUTE_APDU_CLASS,
	SCD_MUTE_APDU_CLASS,
	CCID_SPE_LCD_LAYOUT,
	CCID_SPE_SUPPORT_FUNC,
	NR_CCID_QUEUES,
};
	USBD_INB(ccid_desc_device.bLength);
	USBD_INB(ccid_desc_device.bDescriptorType);
	USBD_INW(ccid_desc_device.bcdCCID);
	USBD_INB(ccid_desc_device.bMaxSlotIndex);
	USBD_INB(ccid_desc_device.bVoltageSupport);
	USBD_INL(ccid_proto_features());
	USBD_INL(ccid_desc_device.dwDefaultClock);
	USBD_INL(ccid_desc_device.dwMaximumClock);
	USBD_INB(ifd_cf_nr_freq());
	USBD_INL(ccid_desc_device.dwDataRate);
	USBD_INL(ccid_desc_device.dwMaxDataRate);
	USBD_INB(ifd_cf_nr_data());
	USBD_INL(ccid_desc_device.dwMaxIFSD);
	USBD_INL(ccid_desc_device.dwSynchProtocols);
	USBD_INL(ccid_desc_device.dwMechanical);
	USBD_INL(ccid_device_features());
	USBD_INL(CCID_MESSAGE_SIZE);
	USBD_INB(ccid_desc_device.bClassGetResponse);
	USBD_INB(ccid_desc_device.bClassEnvelope);
	USBD_INW(ccid_desc_device.wLcdLayout);
	USBD_INB(ccid_desc_device.bPINSupport);
	USBD_INB(ccid_desc_device.bMaxCCIDBusySlots);

usb_intf_desc_t hid_desc_interface = {
	USB_DT_INTERFACE_SIZE,
	USB_DT_INTERFACE,
	0,				/* !bInterfaceNumber */
	0,
	NR_HID_ENDPS,			/* !bNumEndpoints */
	USB_INTERFACE_CLASS_HID,
	USB_INTERFACE_SUBCLASS_BOOT,
	USB_INTERFACE_PROTOCOL_KEYBOARD,
	HID_STRING_INTERFACE,
};
	USBD_INB(hid_desc_interface.bLength);
	USBD_INB(hid_desc_interface.bDescriptorType);
	USBD_INB(hid_iid);
	USBD_INB(hid_desc_interface.bAlternateSetting);
	USBD_INB(NR_HID_ENDPS);
	USBD_INB(hid_desc_interface.bInterfaceClass);
	USBD_INB(hid_desc_interface.bInterfaceSubClass);
	USBD_INB(hid_desc_interface.bInterfaceProtocol);
	USBD_INB(hid_desc_interface.iInterface);

hid_desc_t hid_desc hid_desc_device = {
	HID_DT_HID_SIZE,		/* !bLength */
	HID_DT_HID,
	HID_VERSION_DEFAULT,
	0,				/* bCountryCode */
	0,				/* !bNumDescriptors */
};
	USBD_INB(hid_desc_device.bDescriptorType);
	USBD_INW(hid_desc_device.bcdHID);
	USBD_INB(hid_desc_device.bCountryCode);


usb_intf_desc_t msd_desc_interface = {
	USB_DT_INTERFACE_SIZE,
	USB_DT_INTERFACE,
	0,				/* !bInterfaceNumber */
	0,
	NR_MSD_ENDPS,			/* !bNumEndpoints */
	USB_INTERFACE_CLASS_MSD,
	USB_INTERFACE_SUBCLASS_RBC,
	USB_INTERFACE_PROTOCOL_BBB,
	MSD_STRING_INTERFACE,
};

#define HID_KEYPAD_DIVIDE	84
#define HID_KEYPAD_MULTIPLY	85
#define HID_KEYPAD_MINUS	86
#define HID_KEYPAD_PLUS		87
#define HID_KEYPAD_ENTER	88
#define HID_KEYPAD_1		89
#define HID_KEYPAD_2		90
#define HID_KEYPAD_3		91
#define HID_KEYPAD_4		92
#define HID_KEYPAD_5		93
#define HID_KEYPAD_6		94
#define HID_KEYPAD_7		95
#define HID_KEYPAD_8		96
#define HID_KEYPAD_9		97
#define HID_KEYPAD_0		98
#define HID_KEYPAD_DOT		99
static text_byte_t kbd_keypad_keys[NR_KEYS] = {
	HID_KEYPAD_DOT, HID_KEYPAD_0, HID_KEYPAD_ENTER, HID_KEYPAD_PLUS,
	HID_KEYPAD_1,   HID_KEYPAD_2, HID_KEYPAD_3,     HID_KEYPAD_MINUS,
	HID_KEYPAD_4,   HID_KEYPAD_5, HID_KEYPAD_6,     HID_KEYPAD_MULTIPLY,
	HID_KEYPAD_7,   HID_KEYPAD_8, HID_KEYPAD_9,     HID_KEYPAD_DIVIDE,
};
