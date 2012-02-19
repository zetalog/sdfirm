#ifndef __PN53X_H_INCLUDE_
#define __PN53X_H_INCLUDE_

#include <target/config.h>
#include <target/generic.h>
#include <target/scs.h>
#include <target/nfc.h>

#ifdef CONFIG_PN53X_DEBUG
#define pn53x_debug(tag, val)		dbg_print((tag), (val))
#define PN53X_DUMP_USBD			true
#else
#define pn53x_debug(tag, val)
#define PN53X_DUMP_USBD			false
#endif

#define NR_PN53X_TARGETS		PN53X_HW_MAX_TARGETS
#define PN53X_POLL_INFINITE		0xFF

#define PN53X_BUF_SIZE			264
#define PN53X_HEAD_SIZE			6
#define PN53X_TAIL_SIZE			2
#define PN53X_DATA_SIZE			(PN53X_BUF_SIZE-PN53X_HEAD_SIZE-PN53X_TAIL_SIZE)

/* Normal frames */
#define PN53X_LEN			3
#define PN53X_LCS			4
#define PN53X_TFI			5
#define PN53X_ERR			5
#define PN53X_CMD			6
#define PN53X_PD(n)			(PN53X_CMD+(n))

/* Extended frames */
#define PN53X_ELENM			5
#define PN53X_ELENL			6
#define PN53X_ELCS			7
#define PN53X_ETFI			8
#define PN53X_ECMD			9
#define PN53X_EPD(n)			(PN53X_ECMD+(n))

#define PN53X_OUT			0xD4
#define PN53X_IN			0xD5

#define PN53X_ACK			0x00FF
#define PN53X_NAK			0xFF00
#define PN53X_EXT			0xFFFF
#define pn53x_type(buf)			MAKEWORD((buf)[PN53X_LCS], (buf)[PN53X_LEN])
#define PN53X_NORMAL(buf)		\
	((pn53x_type(buf) != PN53X_ACK) && (pn53x_type(buf) != PN53X_NAK))
#define PN53X_EXTEND(buf)		\
	(pn53x_type(buf) == 0xFFFF)
#define PN53X_ERROR(buf)		\
	((buf)[PN53X_LEN] == 0x01 && (buf)[PN53X_TFI] != PN53X_RESP)
#define PN53X_NORMAL_SIZE(buf)		\
	((PN53X_HEAD_SIZE -1) + (buf)[PN53X_LEN] + PN53X_TAIL_SIZE)

/*=========================================================================
 * command codes
 *=======================================================================*/
/* Miscellaneous */
#define PN53X_Diagnose				0x00
#define PN53X_GetFirmwareVersion		0x02
#define PN53X_GetGeneralStatus			0x04
#define PN53X_ReadRegister			0x06
#define PN53X_WriteRegister			0x08
#define PN53X_ReadGPIO				0x0C
#define PN53X_WriteGPIO				0x0E
#define PN53X_SetSerialBaudRate			0x10
#define PN53X_SetParameters			0x12
#define PN53X_SAMConfiguration			0x14
#define PN53X_PowerDown				0x16
#define PN53X_AlparCommandForTDA		0x18
/* RC-S360 has another command 0x18 for reset &..? */

/* RF communication */
#define PN53X_RFConfiguration			0x32
#define PN53X_RFRegulationTest			0x58

/* Initiator */
#define PN53X_InJumpForDEP			0x56
#define PN53X_InJumpForPSL			0x46
#define PN53X_InListPassiveTarget		0x4A
#define PN53X_InATR				0x50
#define PN53X_InPSL				0x4E
#define PN53X_InDataExchange			0x40
#define PN53X_InCommunicateThru			0x42
#define PN53X_InQuartetByteExchange		0x38
#define PN53X_InDeselect			0x44
#define PN53X_InRelease				0x52
#define PN53X_InSelect				0x54
#define PN53X_InActivateDeactivatePaypass	0x48
#define PN53X_InAutoPoll			0x60

/* Target */
#define PN53X_TgInitAsTarget			0x8C
#define PN53X_TgSetGeneralBytes			0x92
#define PN53X_TgGetData				0x86
#define PN53X_TgSetData				0x8E
#define PN53X_TgSetDataSecure			0x96
#define PN53X_TgSetMetaData			0x94
#define PN53X_TgSetMetaDataSecure		0x98
#define PN53X_TgGetInitiatorCommand		0x88
#define PN53X_TgResponseToInitiator		0x90
#define PN53X_TgGetTargetStatus			0x8A

/*=========================================================================
 * error codes
 *=======================================================================*/
#define PN53X_ERR_SUCCESS		0x00
#define PN53X_ERR_TIMEOUT		0x01
#define PN53X_ERR_CRC			0x02
#define PN53X_ERR_PARITY		0x03
#define PN53X_ERR_BITCOUNT		0x04
#define PN53X_ERR_FRAMING		0x05
#define PN53X_ERR_BITCOLL		0x06
#define PN53X_ERR_SMALLBUF		0x07
#define PN53X_ERR_BUFOVF		0x09
#define PN53X_ERR_RFTIMEOUT		0x0a
#define PN53X_ERR_RFPROTO		0x0b
#define PN53X_ERR_OVHEAT		0x0d
#define PN53X_ERR_INBUFOVF		0x0e
#define PN53X_ERR_INVPARAM		0x10
#define PN53X_ERR_DEPUNKCMD		0x12
#define PN53X_ERR_INVRXFRAM		0x13
#define PN53X_ERR_MFAUTH		0x14
#define PN53X_ERR_NSECNOTSUPP		0x18	/* PN533 only */
#define PN53X_ERR_BCC			0x23
#define PN53X_ERR_DEPINVSTATE		0x25
#define PN53X_ERR_OPNOTALL		0x26
#define PN53X_ERR_CMD			0x27
#define PN53X_ERR_TGREL			0x29
#define PN53X_ERR_CID			0x2a
#define PN53X_ERR_CDISCARDED		0x2b
#define PN53X_ERR_NFCID3		0x2c
#define PN53X_ERR_OVCURRENT		0x2d
#define PN53X_ERR_NAD			0x2e

/*=========================================================================
 * command/response parameters
 *=======================================================================*/
/* Diagnose */
#define PN53X_DIAG_CMDLINE		0x00
#define PN53X_DIAG_ROMTEST		0x01
#define PN53X_DIAG_RAMTEST		0x02
#define PN53X_DIAG_STATUS_OK		0x00
#define PN53X_DIAG_STATUS_FAIL		0xFF
#define PN53X_DIAG_POLL_TARGET		0x04
#define PN53X_DIAG_ECHO_BACK		0x05
#define PN53X_DIAG_DETECT_14443		0x06
#define PN53X_DIAG_ANTENNA_TEST		0x07

/* Read/WriteRegister */
#define PN53X_REG_Control_switch_rng 	0x6106

/* Read/WriteGPIO */
#define PN53X_SFR_P3			0xFFB0
#define PN53X_SFR_P7	 		0xFFF7
#define PN53X_P3_MASK			(~(_BV(7) | _BV(6)))
#define PN53X_P7_MASK			(_BV(2) | _BV(1))
/* Host controller configuration mode from I0I1 */
#define PN53X_HCI_HSU			0x00
#define PN53X_HCI_I2C			0x01
#define PN53X_HCI_SPI			0x02

/* SetParameters */
#define PN53X_PARAM_NONE		0x00
#define PN53X_PARAM_NAD_USED		0x01
#define PN53X_PARAM_DID_USED		0x02
#define PN53X_PARAM_AUTO_ATR_RES	0x04
#define PN53X_PARAM_AUTO_RATS		0x10
#define PN53X_PARAM_14443_4_PICC	0x20
#define PN53X_PARAM_NFC_SECURE		0x20
#define PN53X_PARAM_NO_AMBLE		0x40

/* RFConfiguration */
#define PN53X_RFCI_FIELD			0x01
#define PN53X_RFCI_TIMING			0x02
#define PN53X_RFCI_RETRY_DATA			0x04
#define PN53X_RFCI_RETRY_SELECT			0x05
#define PN53X_RFCI_ANALOG_TYPE_A_106		0x0A
#define PN53X_RFCI_ANALOG_TYPE_A_212_424	0x0B
#define PN53X_RFCI_ANALOG_TYPE_B		0x0C
#define PN53X_RFCI_ANALOG_TYPE_14443_4		0x0D

/*=========================================================================
 * target/modulation types
 *=======================================================================*/
/* Undefined target type */
#define PN53X_TARGET_UNDEFINED			0xFF
/* Generic passive 106 kbps (ISO/IEC14443-4A, mifare, DEP) */
#define PN53X_TARGET_GENERIC_PASSIVE_106	0x00
/* Generic passive 212 kbps (FeliCa, DEP) */
#define PN53X_TARGET_GENERIC_PASSIVE_212	0x01
/* Generic passive 424 kbps (FeliCa, DEP) */
#define PN53X_TARGET_GENERIC_PASSIVE_424	0x02
/* Passive 106 kbps ISO/IEC14443-4B */
#define PN53X_TARGET_ISO14443_4B_106		0x03
/* Innovision Jewel tag */
#define PN53X_TARGET_JEWEL_106			0x04
/* Mifare card */
#define PN53X_TARGET_MIFARE			0x10
/* FeliCa 212 kbps card */
#define PN53X_TARGET_FELICA_212			0x11
/* FeliCa 424 kbps card */
#define PN53X_TARGET_FELICA_424			0x12
/* Passive 106 kbps ISO/IEC 14443-4A */
#define PN53X_TARGET_ISO14443_4A_106		0x20
/* Passive 106 kbps ISO/IEC 14443-4B with TCL flag */
#define PN53X_TARGET_ISO14443_4B_TCL_106	0x23
/* DEP passive 106 kbps */
#define PN53X_TARGET_DEP_PASSIVE_106		0x40
/* DEP passive 212 kbps */
#define PN53X_TARGET_DEP_PASSIVE_212		0x41
/* DEP passive 424 kbps */
#define PN53X_TARGET_DEP_PASSIVE_424		0x42
/* DEP active 106 kbps */
#define PN53X_TARGET_DEP_ACTIVE_106		0x80
/* DEP active 212 kbps */
#define PN53X_TARGET_DEP_ACTIVE_212		0x81
/* DEP active 424 kbps */
#define PN53X_TARGET_DEP_ACTIVE_424		0x82

/* Undefined modulation */
#define PN53X_MODULATION_UNDEFINED		0xFF
/* ISO14443-A (NXP MIFARE) http://en.wikipedia.org/wiki/MIFARE */
#define PN53X_MODULATION_ISO14443A_106		0x00
/* JIS X 6319-4 (Sony Felica) http://en.wikipedia.org/wiki/FeliCa */
#define	PN53X_MODULATION_FELICA_212		0x01
/* JIS X 6319-4 (Sony Felica) http://en.wikipedia.org/wiki/FeliCa */
#define	PN53X_MODULATION_FELICA_424		0x02
/* ISO14443-B
 * http://en.wikipedia.org/wiki/ISO/IEC_14443
 * (Not supported by PN531)
 */
#define	PN53X_MODULATION_ISO14443B_106		0x03
/* Jewel Topaz (Innovision Research & Development)
 * (Not supported by PN531)
 */
#define PN53X_MODULATION_JEWEL_106		0x04
/* ISO14443-B
 * http://en.wikipedia.org/wiki/ISO/IEC_14443
 * (Not supported by PN531 nor PN532)
 */
#define PN53X_MODULATION_ISO14443B_212		0x06
/* ISO14443-B
 * http://en.wikipedia.org/wiki/ISO/IEC_14443
 * (Not supported by PN531 nor PN532)
 */
#define PN53X_MODULATION_ISO14443B_424		0x07
/* ISO14443-B
 * http://en.wikipedia.org/wiki/ISO/IEC_14443
 * (Not supported by PN531 nor PN532)
 */
#define PN53X_MODULATION_ISO14443B_847		0x08

/*=========================================================================
 * CIU registers
 *=======================================================================*/
#define PN53X_REG_CIU_BASE		0x6300
#define PN53X_CIU(offset)		(PN53X_REG_BASE+(offset))
#define PN53X_REG_CIU_Mode		PN53X_CIU(0x01)
#define PN53X_REG_CIU_TxMode		PN53X_CIU(0x02)
#define PN53X_REG_CIU_RxMode		PN53X_CIU(0x03)
#define PN53X_REG_CIU_TxControl		PN53X_CIU(0x04)
#define PN53X_REG_CIU_TxAuto		PN53X_CIU(0x05)
#define PN53X_REG_CIU_TxSel		PN53X_CIU(0x06)
#define PN53X_REG_CIU_RxSel		PN53X_CIU(0x07)
#define PN53X_REG_CIU_RxThreshold	PN53X_CIU(0x08)
#define PN53X_REG_CIU_Demod		PN53X_CIU(0x09)
#define PN53X_REG_CIU_FelNFC1		PN53X_CIU(0x0A)
#define PN53X_REG_CIU_FelNFC2		PN53X_CIU(0x0B)
#define PN53X_REG_CIU_MifNFC		PN53X_CIU(0x0C)
#define PN53X_REG_CIU_ManualRCV		PN53X_CIU(0x0D)
#define PN53X_REG_CIU_TypeB		PN53X_CIU(0x0E)
/* #define PN53X_REG_- 0x630F */
/* #define PN53X_REG_- 0x6310 */
#define PN53X_REG_CIU_CRCResultMSB	PN53X_CIU(0x11)
#define PN53X_REG_CIU_CRCResultLSB	PN53X_CIU(0x12)
#define PN53X_REG_CIU_GsNOFF		PN53X_CIU(0x13)
#define PN53X_REG_CIU_ModWidth		PN53X_CIU(0x14)
#define PN53X_REG_CIU_TxBitPhase	PN53X_CIU(0x15)
#define PN53X_REG_CIU_RFCfg		PN53X_CIU(0x16)
#define PN53X_REG_CIU_GsNOn		PN53X_CIU(0x17)
#define PN53X_REG_CIU_CWGsP		PN53X_CIU(0x18)
#define PN53X_REG_CIU_ModGsP		PN53X_CIU(0x19)
#define PN53X_REG_CIU_TMode		PN53X_CIU(0x1A)
#define PN53X_REG_CIU_TPrescaler	PN53X_CIU(0x1B)
#define PN53X_REG_CIU_TReloadVal_hi	PN53X_CIU(0x1C)
#define PN53X_REG_CIU_TReloadVal_lo	PN53X_CIU(0x1D)
#define PN53X_REG_CIU_TCounterVal_hi	PN53X_CIU(0x1E)
#define PN53X_REG_CIU_TCounterVal_lo	PN53X_CIU(0x1F)
/* #define PN53X_REG_- 0x6320 */
#define PN53X_REG_CIU_TestSel1		PN53X_CIU(0x21)
#define PN53X_REG_CIU_TestSel2		PN53X_CIU(0x22)
#define PN53X_REG_CIU_TestPinEn		PN53X_CIU(0x23)
#define PN53X_REG_CIU_TestPinValue	PN53X_CIU(0x24)
#define PN53X_REG_CIU_TestBus		PN53X_CIU(0x25)
#define PN53X_REG_CIU_AutoTest		PN53X_CIU(0x26)
#define PN53X_REG_CIU_Version		PN53X_CIU(0x27)
#define PN53X_REG_CIU_AnalogTest	PN53X_CIU(0x28)
#define PN53X_REG_CIU_TestDAC1		PN53X_CIU(0x29)
#define PN53X_REG_CIU_TestDAC2		PN53X_CIU(0x2A)
#define PN53X_REG_CIU_TestADC		PN53X_CIU(0x2B)
/* #define PN53X_REG_- 0x632C */
/* #define PN53X_REG_- 0x632D */
/* #define PN53X_REG_- 0x632E */
#define PN53X_REG_CIU_RFlevelDet	PN53X_CIU(0x2F)
#define PN53X_REG_CIU_SIC_CLK_en	PN53X_CIU(0x30)
#define PN53X_REG_CIU_Command		PN53X_CIU(0x31)
#define PN53X_REG_CIU_CommIEn		PN53X_CIU(0x32)
#define PN53X_REG_CIU_DivIEn		PN53X_CIU(0x33)
#define PN53X_REG_CIU_CommIrq		PN53X_CIU(0x34)
#define PN53X_REG_CIU_DivIrq		PN53X_CIU(0x35)
#define PN53X_REG_CIU_Error		PN53X_CIU(0x36)
#define PN53X_REG_CIU_Status1		PN53X_CIU(0x37)
#define PN53X_REG_CIU_Status2		PN53X_CIU(0x38)
#define PN53X_REG_CIU_FIFOData		PN53X_CIU(0x39)
#define PN53X_REG_CIU_FIFOLevel		PN53X_CIU(0x3A)
#define PN53X_REG_CIU_WaterLevel	PN53X_CIU(0x3B)
#define PN53X_REG_CIU_Control		PN53X_CIU(0x3C)
#define PN53X_REG_CIU_BitFraming	PN53X_CIU(0x3D)
#define PN53X_REG_CIU_Coll		PN53X_CIU(0x3E)
#define PN53X_REG_CIU_SIZE		0x40
#define PN53X_CIU_REG2INDEX(reg)	((reg) - PN53X_REG_CIU_BASE)

/* chip drivers for PN53x
 * support nothing but PN532
 */
/* #define NR_PN53X_SLOTS			0x02 */
#define PN53X_HW_MAX_TARGETS		2
boolean pn53x_hw_poll_ready(void);
void pn53x_hw_write_cmpl(scs_size_t nc);
void pn53x_hw_read_cmpl(scs_size_t ne);
uint8_t pn53x_hw_xchg_read(scs_off_t index);
void pn53x_hw_xchg_write(scs_off_t index, uint8_t val);
void pn53x_hw_ctrl_init(void);

/* APIs for NFC<->PN53x constants conversion */
uint8_t pn53x_nm_to_pm(const struct nfc_modulation nm);
struct nfc_modulation pn53x_ptt_to_nm(const uint8_t ptt);
uint8_t pn53x_nm_to_ptt(const struct nfc_modulation nm);

/* API used for PN53x USB devices */
#define pn53x_poll_ready()			pn53x_hw_poll_ready()
#define pn53x_write_cmpl(nc)			pn53x_hw_write_cmpl(nc)
#define pn53x_read_cmpl(ne)			pn53x_hw_read_cmpl(ne)
#define pn53x_xchg_read(index)			pn53x_hw_xchg_read(index)
#define pn53x_xchg_write(index, val)		pn53x_hw_xchg_write(index, val)
#define pn53x_ctrl_init()			pn53x_hw_ctrl_init()

struct pn53x_stub_driver {
	struct nfc_modulation nm;
	scs_err_t (*auto_poll)(uint8_t tg, union nfc_target_info *info);
	scs_size_t (*xchg_avail)(uint8_t tg);
	void (*xchg_block)(uint8_t tg, scs_size_t nc, scs_size_t ne);
	uint8_t (*xchg_read)(uint8_t tg, scs_off_t index);
	void (*xchg_write)(uint8_t tg, scs_off_t index, uint8_t value);
};
__TEXT_TYPE__(struct pn53x_stub_driver, pn53x_stub_driver_t);
uint8_t pn53x_register_stub_driver(pn53x_stub_driver_t *driver);
void pn53x_stub_driver_init(void);

#endif /* __PN53X_H_INCLUDE_ */
