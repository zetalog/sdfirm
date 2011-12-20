#ifndef __USB_SCD_H_INCLUDE__
#define __USB_SCD_H_INCLUDE__

#include <target/config.h>
#include <target/generic.h>
#include <target/usb.h>
#include <target/scs.h>

#ifdef CONFIG_SCD_DEBUG
#define ccid_debug(tag, val)	dbg_print((tag), (val))
#define iccd_debug(tag, val)	dbg_print((tag), (val))
#else
#define ccid_debug(tag, val)
#define iccd_debug(tag, val)
#endif

#define SCD_DT_SCD			(USB_TYPE_CLASS | 0x01)

/* Smart Card Device Class */
typedef struct scd_desc {
	uint8_t	 bLength;
#define SCD_DT_SCD_SIZE	0x36

	uint8_t	 bDescriptorType;
	uint16_t bcdCCID;
#define CCID_VERSION_DEFAULT	0x100
#define ICCD_VERSION_DEFAULT	0x110

	uint8_t	 bMaxSlotIndex;
	uint8_t	 bVoltageSupport;
#define SCD_VOLTAGE_5V		0x01
#define SCD_VOLTAGE_3V		0x02
#define SCD_VOLTAGE_1_8V	0x04
#define SCD_VOLTAGE_ALL		(SCD_VOLTAGE_5V | \
				 SCD_VOLTAGE_3V | \
				 SCD_VOLTAGE_1_8V)
#define SCD_VOLTAGE_AUTO	0x00

	uint32_t dwProtocols;
#define SCD_PROTOCOL_NONE	0x0000
#define SCD_PROTOCOL_T0		0x0000
#define SCD_PROTOCOL_T1		0x0001
#define SCD_PROTOCOL_T15	0x000F

	uint32_t dwDefaultClock;
	uint32_t dwMaximumClock;
	uint8_t	 bNumClockSupported;
#define ICCD_FIXED_CLOCK	0x0DFC

	uint32_t dwDataRate;
	uint32_t dwMaxDataRate;
	uint8_t	 bNumDataRatesSupported;
#define ICCD_FIXED_DATARATE		0x2580

	uint32_t dwMaxIFSD;
#define ICCD_FIXED_MAX_IFSD		0xFE

	uint32_t dwSynchProtocols;
#define SCD_SYNCH_PROTO_NONE		0x0000
#define SCD_SYNCH_PROTO_2W		0x0001
#define SCD_SYNCH_PROTO_3W		0x0002
#define SCD_SYNCH_PROTO_I2C		0x0004

	uint32_t dwMechanical;
#define SCD_MECHA_NONE			0x00
#define SCD_MECHA_ACCEPT		0x01
#define SCD_MECHA_EJECTION		0x02
#define SCD_MECHA_CAPTURE		0x04
#define SCD_MECHA_LOCK			0x08
#define SCD_MECHA_NONE			0x00

	uint32_t dwFeatures;
#define SCD_FEATURE_NONE		0x00
#define SCD_FEATURE_AUTO_CONFIG		0x00000002
#define SCD_FEATURE_AUTO_ACTIVATE	0x00000004
#define SCD_FEATURE_AUTO_CLASS		0x00000008
#define SCD_FEATURE_AUTO_FREQ		0x00000010
#define SCD_FEATURE_AUTO_DATA		0x00000020

#define SCD_FEATURE_SLOT_MASK		0x0000000C

#define SCD_FEATURE_AUTO_PPS_PROP	0x00000040
#define SCD_FEATURE_AUTO_PPS_BASE	0x00000080

#define SCD_FEATURE_CAP_CLOCK_STOP	0x00000100
#define SCD_FEATURE_CAP_ACCEPT_NAD	0x00000200
#define SCD_FEATURE_AUTO_IFSD		0x00000400

#define SCD_FEATURE_XCHG_CHAR		0x00000000
#define SCD_FEATURE_XCHG_TPDU		0x00010000
#define SCD_FEATURE_XCHG_APDU		0x00020000
#define SCD_FEATURE_XCHG_APDU_EXT	0x00040000
#define SCD_FEATURE_XCHG_MASK		0x00070000

#define SCD_FEATURE_WAKEUP_ICC		0x00100000

#define ICCD_FEATURE_SPECIFIC		0x00000800
#define ICCD_FEATURE_DEFAULT		(ICCD_FEATURE_SPECIFIC | SCD_FEATURE_AUTO_PPS_PROP)

	uint32_t dwMaxCCIDMessageLength;

	uint8_t	 bClassGetResponse;
	uint8_t	 bClassEnvelope;
#define SCD_MUTE_APDU_CLASS		0x00
#define SCD_ECHO_APDU_CLASS		0xFF

	uint16_t wLcdLayout;
#define SCD_LCD_LAYOUT_NONE		0x0000

	uint8_t	 bPINSupport;
#define SCD_SPE_SUPPORT_NONE		0x00
#define SCD_SPE_SUPPORT_VERIFY		0x01
#define SCD_SPE_SUPPORT_MODIFY		0x02
#define SCD_SPE_SUPPORT_ALL		(SCD_SPE_SUPPORT_VERIFY | \
					 SCD_SPE_SUPPORT_MODIFY)

	uint8_t	 bMaxCCIDBusySlots;
#define ICCD_MAX_BUSY_SLOT		0x01
} scd_desc_t;

#endif /* __USB_SCD_H_INCLUDE__ */
