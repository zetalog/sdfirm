#ifndef __PN53X_H_INCLUDE_
#define __PN53X_H_INCLUDE_

#include <target/config.h>
#include <target/generic.h>
#include <target/scs.h>

#ifdef CONFIG_PN53X_DEBUG
#define pn53x_debug(tag, val)		dbg_print((tag), (val))
#define PN53X_DUMP_USBD			true
#else
#define pn53x_debug(tag, val)
#define PN53X_DUMP_USBD			false
#endif

#define PN53X_BUF_SIZE			264
#define PN53X_HEAD_SIZE			6
#define PN53X_TAIL_SIZE			2
#define PN53X_DATA_SIZE			(PN53X_BUF_SIZE-PN53X_HEAD_SIZE-PN53X_TAIL_SIZE)

#define PN53X_LEN			3
#define PN53X_LCS			4
#define PN53X_TFI			5
#define PN53X_ERR			5
#define PN53X_CMD			6

#define PN53X_OUT			0xD4
#define PN53X_IN			0xD5

#define PN53X_ACK			0x00FF
#define PN53X_NAK			0xFF00
#define pn53x_type(buf)			MAKEWORD((buf)[PN53X_LCS], (buf)[PN53X_LEN])
#define PN53X_NORMAL(buf)		\
	((pn53x_type(buf) != PN53X_ACK) && (pn53x_type(buf) != PN53X_NAK))
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
 * command codes
 *=======================================================================*/
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

boolean pn53x_hw_poll_ready(void);
void pn53x_hw_write_cmpl(scs_size_t nc);
void pn53x_hw_read_cmpl(scs_size_t ne);
uint8_t pn53x_hw_xchg_read(scs_off_t index);
void pn53x_hw_xchg_write(scs_off_t index, uint8_t val);
void pn53x_hw_ctrl_init(void);

/* API used for PN53x USB devices */
#define pn53x_poll_ready()			pn53x_hw_poll_ready()
#define pn53x_write_cmpl(nc)			pn53x_hw_write_cmpl(nc)
#define pn53x_read_cmpl(ne)			pn53x_hw_read_cmpl(ne)
#define pn53x_xchg_read(index)			pn53x_hw_xchg_read(index)
#define pn53x_xchg_write(index, val)		pn53x_hw_xchg_write(index, val)
#define pn53x_ctrl_init()			pn53x_hw_ctrl_init()

#endif /* __PN53X_H_INCLUDE_ */
