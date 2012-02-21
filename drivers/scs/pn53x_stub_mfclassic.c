#include <driver/pn53x.h>
#include <target/mtd.h>
#include <target/arch.h>

#define MFCLASSIC_CMD_AUTH_A	0x60
#define MFCLASSIC_CMD_AUTH_B	0x61
#define MFCLASSIC_CMD_READ	0x30
#define MFCLASSIC_CMD_WRITE	0xA0
#define MFCLASSIC_CMD_TRANSFER	0xB0
#define MFCLASSIC_CMD_DECREMENT	0xC0
#define MFCLASSIC_CMD_INCREMENT	0xC1
#define MFCLASSIC_CMD_STORE	0xC2

#define MFCLASSIC_RESP_INIT	0xFF
#define MFCLASSIC_RESP_NAK	0x00
#define MFCLASSIC_RESP_ACK	0x0A

struct mfclassic_auth {
	uint8_t abtKey[6];
	uint8_t abtUid[4];
} __packed;

struct mfclassic_data {
	uint8_t abtData[16];
} __packed;

struct mfclassic_value {
	uint8_t abtValue[4];
} __packed;

union mfclassic_param {
	struct mfclassic_auth mpa;
	struct mfclassic_data mpd;
	struct mfclassic_value mpv;
};
#define MFCLASSIC_MAX_BUF	(2+sizeof(union mfclassic_param))

uint8_t mfclassic_resp_len;
uint8_t mfclassic_cmd_len;
uint8_t mfclassic_code;
uint8_t mfclassic_cmd[MFCLASSIC_MAX_BUF];
uint8_t mfclassic_resp[MFCLASSIC_MAX_BUF];

extern mtd_t board_mtd;

scs_err_t pn53x_stub_mfclassic_get_info(uint8_t tg,
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

scs_err_t pn53x_stub_mfclassic_read_ready(uint8_t tg)
{
	if (tg != 0)
		return SCS_ERR_NOTPRESENT;
	if (mfclassic_code == MFCLASSIC_RESP_INIT)
		return SCS_ERR_PROGRESS;
	if (mfclassic_code == MFCLASSIC_RESP_ACK)
		return SCS_ERR_SUCCESS;
	return SCS_ERR_HW_ERROR;
}

scs_size_t pn53x_stub_mfclassic_read_count(uint8_t tg)
{
	if (tg != 0)
		return 0;
	return mfclassic_resp_len;
}

uint8_t pn53x_stub_mfclassic_read_byte(uint8_t tg, scs_off_t index)
{
	if ((tg == 0) && (index < mfclassic_resp_len))
		return mfclassic_resp[index];
	return 0;
}

static void pn53x_stub_mfclassic_error(uint8_t err)
{
	mfclassic_code = (err & 0x0F);
}

void pn53x_stub_mfclassic_auth_a(void)
{
	pn53x_stub_mfclassic_error(MFCLASSIC_RESP_NAK);
}

void pn53x_stub_mfclassic_auth_b(void)
{
	pn53x_stub_mfclassic_error(MFCLASSIC_RESP_NAK);
}

void pn53x_stub_mfclassic_read(void)
{
	pn53x_stub_mfclassic_error(MFCLASSIC_RESP_NAK);
}

void pn53x_stub_mfclassic_write(void)
{
	pn53x_stub_mfclassic_error(MFCLASSIC_RESP_NAK);
}

void pn53x_stub_mfclassic_transfer(void)
{
	pn53x_stub_mfclassic_error(MFCLASSIC_RESP_NAK);
}

void pn53x_stub_mfclassic_decrement(void)
{
	pn53x_stub_mfclassic_error(MFCLASSIC_RESP_NAK);
}

void pn53x_stub_mfclassic_increment(void)
{
	pn53x_stub_mfclassic_error(MFCLASSIC_RESP_NAK);
}

void pn53x_stub_mfclassic_store(void)
{
	pn53x_stub_mfclassic_error(MFCLASSIC_RESP_NAK);
}

void pn53x_stub_mfclassic_write_ready(uint8_t tg, scs_size_t count)
{
	if (tg != 0)
		return;

	mfclassic_cmd_len = count;
	mfclassic_resp_len = 0;
	mfclassic_code = MFCLASSIC_RESP_INIT;

	switch (mfclassic_cmd[0]) {
	case MFCLASSIC_CMD_AUTH_A:
		pn53x_stub_mfclassic_auth_a();
		break;
	case MFCLASSIC_CMD_AUTH_B:
		pn53x_stub_mfclassic_auth_b();
		break;
	case MFCLASSIC_CMD_READ:
		pn53x_stub_mfclassic_read();
		break;
	case MFCLASSIC_CMD_WRITE:
		pn53x_stub_mfclassic_write();
		break;
	case MFCLASSIC_CMD_TRANSFER:
		pn53x_stub_mfclassic_transfer();
		break;
	case MFCLASSIC_CMD_DECREMENT:
		pn53x_stub_mfclassic_decrement();
		break;
	case MFCLASSIC_CMD_INCREMENT:
		pn53x_stub_mfclassic_increment();
		break;
	case MFCLASSIC_CMD_STORE:
		pn53x_stub_mfclassic_store();
		break;
	default:
		pn53x_stub_mfclassic_error(MFCLASSIC_RESP_NAK);
		break;
	}
}

void pn53x_stub_mfclassic_write_byte(uint8_t tg, scs_off_t index, uint8_t value)
{
	if ((tg == 0) && (index < MFCLASSIC_MAX_BUF)) {
		mfclassic_cmd[index] = value;
	}
}

pn53x_stub_driver_t pn53x_stub_mfclassic = {
	NFC_MODULATION(NFC_TYPE_ISO14443A, NFC_BAUD_106),
	pn53x_stub_mfclassic_get_info,
	pn53x_stub_mfclassic_read_ready,
	pn53x_stub_mfclassic_read_count,
	pn53x_stub_mfclassic_read_byte,
	pn53x_stub_mfclassic_write_ready,
	pn53x_stub_mfclassic_write_byte,
};

void pn53x_stub_driver_init(void)
{
	pn53x_register_stub_driver(&pn53x_stub_mfclassic);
}
