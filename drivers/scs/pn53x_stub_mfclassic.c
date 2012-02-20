#include <driver/pn53x.h>

scs_err_t pn53x_stub_mfclassic_auto_poll(uint8_t tg,
					 union nfc_target_info *nti)
{
	if (tg != 0)
		return SCS_ERR_HW_ERROR;
	nti->nai.abtAtqa[0] = 0x00;
	nti->nai.abtAtqa[1] = 0x04;
	nti->nai.btSak = 0x08;
	nti->nai.szUidLen = 0x04;
	nti->nai.abtUid[0] = 0xC3;
	nti->nai.abtUid[1] = 0xB2;
	nti->nai.abtUid[2] = 0xEF;
	nti->nai.abtUid[3] = 0xC8;
	nti->nai.szAtsLen = 0x00;
	return SCS_ERR_SUCCESS;
}

pn53x_stub_driver_t pn53x_stub_mfclassic = {
	{
		NFC_TYPE_ISO14443A,
		NFC_BAUD_106,
	},
	pn53x_stub_mfclassic_auto_poll,
};

void pn53x_stub_driver_init(void)
{
	pn53x_register_stub_driver(&pn53x_stub_mfclassic);
}
