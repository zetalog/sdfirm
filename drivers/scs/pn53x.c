#include <driver/pn53x.h>

uint8_t pn53x_nm_to_pm(const struct nfc_modulation nm)
{
	switch (nm.nmt) {
	case NFC_TYPE_ISO14443A:
		return PN53X_MODULATION_ISO14443A_106;
		break;
	case NFC_TYPE_ISO14443B:
		switch (nm.nbr) {
		case NFC_BAUD_106:
			return PN53X_MODULATION_ISO14443B_106;
			break;
		case NFC_BAUD_212:
			return PN53X_MODULATION_ISO14443B_212;
			break;
		case NFC_BAUD_424:
			return PN53X_MODULATION_ISO14443B_424;
			break;
		case NFC_BAUD_847:
			return PN53X_MODULATION_ISO14443B_847;
			break;
		case NFC_BAUD_UNDEFINED:
			/* Nothing to do... */
			break;
		}
		break;
	case NFC_TYPE_JEWEL:
		return PN53X_MODULATION_JEWEL_106;
		break;
	case NFC_TYPE_FELICA:
		switch (nm.nbr) {
		case NFC_BAUD_212:
			return PN53X_MODULATION_FELICA_212;
			break;
		case NFC_BAUD_424:
			return PN53X_MODULATION_FELICA_424;
			break;
		case NFC_BAUD_106:
		case NFC_BAUD_847:
		case NFC_BAUD_UNDEFINED:
			/* Nothing to do... */
			break;
		}
		break;
	case NFC_TYPE_ISO14443BI:
	case NFC_TYPE_ISO14443B2SR:
	case NFC_TYPE_ISO14443B2CT:
	case NFC_TYPE_DEP:
		/* Nothing to do... */
		break;
	}
	return PN53X_MODULATION_UNDEFINED;
}

struct nfc_modulation pn53x_ptt_to_nm(const uint8_t ptt)
{
	struct nfc_modulation nm;
	switch (ptt) {
	case PN53X_TARGET_GENERIC_PASSIVE_106:
	case PN53X_TARGET_GENERIC_PASSIVE_212:
	case PN53X_TARGET_GENERIC_PASSIVE_424:
	case PN53X_TARGET_UNDEFINED:
		/* XXX This should not happend, how handle it cleanly? */
		break;
	case PN53X_TARGET_MIFARE:
	case PN53X_TARGET_ISO14443_4A_106:
		nm.nmt = NFC_TYPE_ISO14443A;
		nm.nbr = NFC_BAUD_106;
		return nm;
		break;
	case PN53X_TARGET_ISO14443_4B_106:
	case PN53X_TARGET_ISO14443_4B_TCL_106:
		nm.nmt = NFC_TYPE_ISO14443B;
		nm.nbr = NFC_BAUD_106;
		return nm;
		break;
	case PN53X_TARGET_JEWEL_106:
		nm.nmt = NFC_TYPE_JEWEL;
		nm.nbr = NFC_BAUD_106;
		return nm;
		break;
	case PN53X_TARGET_FELICA_212:
		nm.nmt = NFC_TYPE_FELICA;
		nm.nbr = NFC_BAUD_212;
		return nm;
		break;
	case PN53X_TARGET_FELICA_424:
		nm.nmt = NFC_TYPE_FELICA;
		nm.nbr = NFC_BAUD_424;
		return nm;
		break;
	case PN53X_TARGET_DEP_PASSIVE_106:
	case PN53X_TARGET_DEP_ACTIVE_106:
		nm.nmt = NFC_TYPE_DEP;
		nm.nbr = NFC_BAUD_106;
		return nm;
		break;
	case PN53X_TARGET_DEP_PASSIVE_212:
	case PN53X_TARGET_DEP_ACTIVE_212:
		nm.nmt = NFC_TYPE_DEP;
		nm.nbr = NFC_BAUD_212;
		return nm;
		break;
	case PN53X_TARGET_DEP_PASSIVE_424:
	case PN53X_TARGET_DEP_ACTIVE_424:
		nm.nmt = NFC_TYPE_DEP;
		nm.nbr = NFC_BAUD_424;
		return nm;
		break;
	}
	/* We should never be here, this line silent compilation warning */
	nm.nmt = NFC_TYPE_ISO14443A;
	nm.nbr = NFC_BAUD_106;
	return nm;
}

uint8_t pn53x_nm_to_ptt(const struct nfc_modulation nm)
{
	switch (nm.nmt) {
	case NFC_TYPE_ISO14443A:
		return PN53X_TARGET_MIFARE;
		/* return PTT_ISO14443_4A_106; */
		break;
	case NFC_TYPE_ISO14443B:
		switch (nm.nbr) {
		case NFC_BAUD_106:
			return PN53X_TARGET_ISO14443_4B_106;
			break;
		case NFC_BAUD_UNDEFINED:
		case NFC_BAUD_212:
		case NFC_BAUD_424:
		case NFC_BAUD_847:
			/* Nothing to do... */
			break;
		}
		break;
	case NFC_TYPE_JEWEL:
		return PN53X_TARGET_JEWEL_106;
		break;
	case NFC_TYPE_FELICA:
		switch (nm.nbr) {
		case NFC_BAUD_212:
			return PN53X_TARGET_FELICA_212;
			break;
		case NFC_BAUD_424:
			return PN53X_TARGET_FELICA_424;
			break;
		case NFC_BAUD_UNDEFINED:
		case NFC_BAUD_106:
		case NFC_BAUD_847:
			/* Nothing to do... */
			break;
		}
		break;
	case NFC_TYPE_ISO14443BI:
	case NFC_TYPE_ISO14443B2SR:
	case NFC_TYPE_ISO14443B2CT:
	case NFC_TYPE_DEP:
		/* Nothing to do... */
		break;
	}
	return PN53X_TARGET_UNDEFINED;
}
