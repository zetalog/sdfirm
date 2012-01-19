#include "pcsc_priv.h"

static boolean pcsc_acos5_probe(uint8_t *atr, int len)
{
	struct pcsc_atr *info = &g_pcsc_atr;

	if (info->historical_bytes[0] == 0x41 &&
	    info->historical_bytes[1] == 0x05 &&
	    info->historical_bytes[2] == 0x10)
		    return true;
	return false;
}

struct pcsc_cmds acs_apdus[] = {
	{ AT_XFR_BLOCK,   {0x00, 0x20, 0x00, 0x81, 0x00}, 0x05, 0x00 },
	{ AT_SELECT_FILE, {0x00, 0xA4, 0x00, 0x00, 0x02, 0x3F, 0x00}, 0x07, 0x00},
	{ AT_READ_BINARY, {0x00, 0xB0, 0x00, 0x00, 0x01}, 0x05, 0x01},
	{ AT_INVALID },
};

static int pcsc_acos5_build(uint8_t *buf, uint8_t type, int *ne)
{
	struct pcsc_cmds *cmd = &acs_apdus[0];

	while (cmd->type != AT_INVALID) {
		if (cmd->type == type) {
			memcpy(buf, cmd->cmd, cmd->len);
			if (ne)
				*ne = cmd->ne;
			return cmd->len;
		}
		cmd++;
	}
	return 0;
}

static int pcsc_acos5_select_file(uint8_t *apdu, uint16_t fid)
{
	int len = pcsc_acos5_build(apdu, AT_SELECT_FILE, NULL);

	if (len) {
		apdu[5] = (fid & 0xFF00) >> 8;
		apdu[6] = (fid & 0x00FF);
	}
	return len;
}

static int pcsc_acos5_read_binary(uint8_t *apdu, int length)
{
	int len = pcsc_acos5_build(apdu, AT_READ_BINARY, NULL);
	
	if (len) {
		apdu[4] = (uint8_t)length; 
	}
	return len;
}

static int pcsc_acos5_pin_verify(struct ccid_pv_param *pv, uint8_t *apdu)
{
	int i;

	pv->po.bTimeout = 20;
	pv->po.bmFormatString = 0x02;
	pv->po.bmPINBlockString = 0x08;
	pv->po.bmPINLengthFormat = 0x00;
	pv->wPINMaxExtraDigit = 0x0808;
	pv->bEntryValidationCondition = 0x03;
	pv->bNumberMessage = 0x01;
	pv->wLangId = 0x0409;
	pv->bMsgIndex = 0x00;
	pv->bTeoPrologue[0] = 0;
	pv->bTeoPrologue[1] = 0;
	pv->bTeoPrologue[2] = 0;
	i = 0;
	apdu[i++] = 0x00;
	apdu[i++] = 0x20;
	apdu[i++] = 0x00;
	apdu[i++] = 0x81;
	apdu[i++] = 0x08;
	apdu[i++] = 0xFF; apdu[i++] = 0xFF;
	apdu[i++] = 0xFF; apdu[i++] = 0xFF;
	apdu[i++] = 0xFF; apdu[i++] = 0xFF;
	apdu[i++] = 0xFF; apdu[i++] = 0xFF;
	return i;
}

static int pcsc_acos5_pin_modify(struct ccid_pm_param *pm, uint8_t *apdu)
{
	int i;

	pm->po.bTimeout = 0x00;
	pm->po.bmFormatString = 0x02;
	pm->po.bmPINBlockString = 0x08;
	pm->po.bmPINLengthFormat = 0x00;
	pm->bInsertionOffsetOld = 0x00;
	pm->bInsertionOffsetNew = 0x08;
	pm->wPINMaxExtraDigit = 0x0808;
	pm->bConfirmPIN = 0x01;
	pm->bEntryValidationCondition = 0x03;
	pm->bNumberMessage = 0x03;
	pm->wLangId = 0x0409;
	pm->bMsgIndex[0] = 0x00;
	pm->bMsgIndex[1] = 0x01;
	pm->bMsgIndex[2] = 0x02;

	pm->bTeoPrologue[0] = 0x00;
	pm->bTeoPrologue[1] = 0x00;
	pm->bTeoPrologue[2] = 0x00;

	i = 0;

	apdu[i++] = 0x00;
	apdu[i++] = 0x24;
	apdu[i++] = 0x00;
	apdu[i++] = 0x81;
	apdu[i++] = 0x10;
	apdu[i++] = 0xFF; apdu[i++] = 0xFF;
	apdu[i++] = 0xFF; apdu[i++] = 0xFF;
	apdu[i++] = 0xFF; apdu[i++] = 0xFF;
	apdu[i++] = 0xFF; apdu[i++] = 0xFF;
	apdu[i++] = 0xFF; apdu[i++] = 0xFF;
	apdu[i++] = 0xFF; apdu[i++] = 0xFF;
	apdu[i++] = 0xFF; apdu[i++] = 0xFF;
	apdu[i++] = 0xFF; apdu[i++] = 0xFF;
	return i;
}

static struct pcsc_card card_acs = {
	PCSC_CARD_ACS_ACOS5,
	"ACOS5 (ACS)",
	pcsc_acos5_probe,
	pcsc_acos5_build,
	pcsc_acos5_select_file,
	pcsc_acos5_read_binary,
	pcsc_acos5_pin_verify,
	pcsc_acos5_pin_modify,
};

int pcsc_acos5_init(void)
{
	return pcsc_register_card(&card_acs);
}

void pcsc_acos5_exit(void)
{
	pcsc_unregister_card(&card_acs);
}
