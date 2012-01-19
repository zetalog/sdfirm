#include "pcsc_priv.h"

static boolean pcsc_tcos_probe(uint8_t *atr, int len)
{
	struct pcsc_atr *info = &g_pcsc_atr;

	if (info->historical_bytes[0] == 0x57 &&
	    info->historical_bytes[1] == 0x44)
		return true;

	return false;
}

struct pcsc_cmds tcos_apdus[] = {
	{ AT_SELECT_FILE, {0x00, 0xA4, 0x00, 0x00, 0x02, 0x3F, 0x00}, 0x07, 0x00},
	{ AT_READ_BINARY, {0x00, 0xB0, 0x00, 0x00, 0x01}, 0x05, 0x01},
	{ AT_INVALID },
};

static int pcsc_tcos_build(uint8_t *apdu, uint8_t type, int *ne)
{
	struct pcsc_cmds *cmd = &tcos_apdus[0];

	while (cmd->type != AT_INVALID) {
		if (cmd->type == type) {
			memcpy(apdu, cmd->cmd, cmd->len);
			if (ne)
				*ne = cmd->ne;
			return cmd->len;
		}
		cmd++;
	}
	return 0;
}

static int pcsc_tcos_select_file(uint8_t *apdu, uint16_t fid)
{
	int len = pcsc_tcos_build(apdu, AT_SELECT_FILE, NULL);

	if (len) {
		apdu[5] = (fid & 0xFF00) >> 8;
		apdu[6] = (fid & 0x00FF);
	}
	return len;
}

/* can only read current selected file */
static int pcsc_tcos_read_binary(uint8_t *apdu, int length)
{
	int len = pcsc_tcos_build(apdu, AT_READ_BINARY, NULL);
	
	if (len) {
		apdu[4] = (uint8_t)length; 
	}
	return len;
}

static struct pcsc_card pcsc_card_tcos = {
	PCSC_CARD_WD_TCOS,
	"TCOS (Watchdata)",
	pcsc_tcos_probe,
	pcsc_tcos_build,
	pcsc_tcos_select_file,
	pcsc_tcos_read_binary,
};

int pcsc_tcos_init(void)
{
	return pcsc_register_card(&pcsc_card_tcos);
}

void pcsc_tcos_exit(void)
{
	pcsc_unregister_card(&pcsc_card_tcos);
}
