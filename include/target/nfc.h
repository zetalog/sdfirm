#ifndef __NFC_H_INCLUDE__
#define __NFC_H_INCLUDE__

#include <target/config.h>
#include <target/generic.h>

/* NFC target information in D.E.P. (Data Exchange Protocol) see
 * ISO/IEC 18092 (NFCIP-1)
 */

struct nfc_dep_info {
	uint8_t abtNFCID3[10];	/* NFCID3 */
	uint8_t btDID;		/* DID */
	uint8_t btBS;		/* Supported send-bit rate */
	uint8_t btBR;		/* Supported receive-bit rate */
	uint8_t btTO;		/* Timeout value */
	uint8_t btPP;		/* PP Parameters */
	uint8_t abtGB[48];	/* General Bytes */
	size_t szGB;
	uint8_t ndm;	/* DEP mode */
#define	NDM_UNDEFINED	0x00
#define	NDM_PASSIVE	0x01
#define	NDM_ACTIVE	0x02
} __packed;

struct nfc_iso14443a_info {
	uint8_t abtAtqa[2];
	uint8_t btSak;
	size_t szUidLen;
	uint8_t abtUid[10];
	size_t szAtsLen;
	/* Maximal theoretical ATS is FSD-2, FSD=256 for FSDI=8 in RATS */
	uint8_t abtAts[254];
} __packed;

struct nfc_felica_info {
	size_t szLen;
	uint8_t btResCode;
	uint8_t abtId[8];
	uint8_t abtPad[8];
	uint8_t abtSysCode[2];
} __packed;

struct nfc_iso14443b_info {
	/* abtPupi store PUPI contained in ATQB
	 * (Answer To reQuest of type B) (see ISO14443-3)
	 */
	uint8_t abtPupi[4];
	/* abtApplicationData store Application Data contained in ATQB
	 * (see ISO14443-3)
	 */
	uint8_t abtApplicationData[4];
	/* abtProtocolInfo store Protocol Info contained in ATQB
	 * (see ISO14443-3)
	 */
	uint8_t abtProtocolInfo[3];
	/* ui8CardIdentifier store CID (Card Identifier) attributted by
	 * PCD to the PICC
	 */
	uint8_t ui8CardIdentifier;
} __packed;

struct nfc_iso14443bi_info {
	uint8_t abtDIV[4];	/* DIV: 4 LSBytes of tag serial number */
	uint8_t btVerLog;	/* Software version & type of REPGEN */
	uint8_t btConfig;	/* Config Byte, present if long REPGEN */
	size_t szAtrLen;	/* ATR, if any */
	uint8_t abtAtr[33];
} __packed;

struct nfc_iso14443b2sr_info {
	uint8_t abtUID[8];
} __packed;

struct nfc_iso14443b2ct_info {
	uint8_t abtUID[4];
	uint8_t btProdCode;
	uint8_t btFabCode;
} __packed;

struct nfc_jewel_info {
	uint8_t  btSensRes[2];
	uint8_t  btId[4];
} __packed;

/* Union between all kind of tags information structures. */
union nfc_target_info {
	struct nfc_iso14443a_info nai;
	struct nfc_felica_info nfi;
	struct nfc_iso14443b_info nbi;
	struct nfc_iso14443bi_info nii;
	struct nfc_iso14443b2sr_info nsi;
	struct nfc_iso14443b2ct_info nci;
	struct nfc_jewel_info nji;
	struct nfc_dep_info ndi;
};

struct nfc_modulation {
	uint8_t nmt;
#define	NFC_TYPE_ISO14443A	0x00
#define	NFC_TYPE_JEWEL		0x01
#define	NFC_TYPE_ISO14443B	0x02
	/* pre-ISO14443B aka ISO/IEC 14443 B' or Type B' */
#define	NFC_TYPE_ISO14443BI	0x03
#define	NFC_TYPE_ISO14443B2SR	0x04 /* ISO14443-2B ST SRx */
#define	NFC_TYPE_ISO14443B2CT	0x05 /* ISO14443-2B ASK CTx */
#define	NFC_TYPE_FELICA		0x06
#define	NFC_TYPE_DEP		0x07
	uint8_t nbr;
#define NFC_BAUD_UNDEFINED	0x00
#define NFC_BAUD_106		0x01
#define NFC_BAUD_212		0x02
#define NFC_BAUD_424		0x03
#define NFC_BAUD_847		0x04
} __packed;

struct nfc_target {
	union nfc_target_info nti;
	struct nfc_modulation nm;
} __packed;

#endif /* __NFC_H_INCLUDE__ */
