#include <driver/pn53x.h>

uint8_t pn53x_stub_cmd[PN53X_BUF_SIZE];
uint8_t pn53x_stub_resp[PN53X_BUF_SIZE];
scs_size_t pn53x_stub_nc;
scs_size_t pn53x_stub_ne;
boolean pn53x_stub_ready;
boolean pn53x_stub_is_resp;
boolean pn53x_stub_is_cmd;

void pn53x_build_frame(void)
{
	scs_size_t ne = PN53X_NORMAL_SIZE(pn53x_stub_resp);
	uint8_t dcs = 0x00;

	pn53x_stub_resp[PN53X_LCS] = (uint8_t)(0x00 - pn53x_stub_resp[PN53X_LEN]);

	/* TODO: Calculate DCS. */
	pn53x_stub_resp[ne - PN53X_TAIL_SIZE] = dcs;
	pn53x_stub_resp[ne-1] = 0x00;

	pn53x_stub_ne = ne;
	pn53x_stub_is_resp = true;
	pn53x_stub_ready = true;
}

void pn53x_response_error(uint8_t ec)
{
	pn53x_debug(PN53X_DEBUG_ERR_CODE, ec);

	pn53x_stub_resp[PN53X_LEN] = 0x01;
	pn53x_stub_resp[PN53X_ERR] = ec;
	pn53x_build_frame();
}

void pn53x_response_dummy(void)
{
	scs_off_t i;
	for (i = 0; i < pn53x_stub_nc; i++)
		pn53x_stub_resp[i] = pn53x_stub_cmd[i];
	pn53x_build_frame();
}

void pn53x_response_GetFirmwareVersion(void)
{
	pn53x_build_frame();
}

void pn53x_xchg_pseudo(void)
{
	uint8_t cmd;

	/* Validate LCS. */
	if (0x00 != (uint8_t)(pn53x_stub_cmd[PN53X_LEN]+
			      pn53x_stub_cmd[PN53X_LCS]))
		return;
	/* Validate TFI. */
	if (pn53x_stub_cmd[PN53X_TFI] != PN53X_OUT)
		return;
	/* TODO: Validate DCS. */

	pn53x_stub_resp[0] = 0x00;
	pn53x_stub_resp[1] = 0x00;
	pn53x_stub_resp[2] = 0xFF;
	pn53x_stub_resp[PN53X_TFI] = PN53X_IN;

	cmd = pn53x_stub_cmd[PN53X_CMD];
	pn53x_debug(PN53X_DEBUG_CMD_CODE, cmd);
	switch (cmd) {
	case PN53X_Diagnose:
		pn53x_response_error(PN53X_ERR_CMD);
		break;
	case PN53X_GetFirmwareVersion:
		pn53x_response_dummy();
		break;
	case PN53X_ReadRegister:
		pn53x_response_dummy();
		break;
	case PN53X_WriteRegister:
		pn53x_response_dummy();
		break;
	case PN53X_ReadGPIO:
	case PN53X_WriteGPIO:
	case PN53X_AlparCommandForTDA:
		pn53x_response_error(PN53X_ERR_CMD);
		break;
	case PN53X_SetSerialBaudRate:
		pn53x_response_dummy();
		break;
	case PN53X_SetParameters:
		pn53x_response_dummy();
		break;
	case PN53X_SAMConfiguration:
		pn53x_response_dummy();
		break;
	case PN53X_PowerDown:
		pn53x_response_dummy();
		break;
	case PN53X_InJumpForDEP:
	case PN53X_InJumpForPSL:
	case PN53X_InActivateDeactivatePaypass:
		pn53x_response_error(PN53X_ERR_CMD);
		break;
	case PN53X_InListPassiveTarget:
		pn53x_response_dummy();
		break;
	case PN53X_InATR:
	case PN53X_InPSL:
	case PN53X_InDataExchange:
	case PN53X_InCommunicateThru:
	case PN53X_InQuartetByteExchange:
	case PN53X_InDeselect:
	case PN53X_InRelease:
	case PN53X_InSelect:
		pn53x_response_error(PN53X_ERR_CMD);
		break;
	case PN53X_InAutoPoll:
		pn53x_response_dummy();
		break;
	}
}

boolean pn53x_hw_poll_ready(void)
{
	return pn53x_stub_ready;
}

void pn53x_hw_read_cmpl(scs_size_t ne)
{
	pn53x_stub_ready = false;
	if (!pn53x_stub_is_resp && pn53x_stub_is_cmd) {
		pn53x_xchg_pseudo();
	}
}

void pn53x_hw_write_cmpl(scs_size_t nc)
{
	pn53x_stub_nc = nc;

	switch (pn53x_type(pn53x_stub_cmd)) {
	case PN53X_ACK:
		pn53x_stub_is_cmd = false;
		pn53x_stub_is_resp = false;
		pn53x_stub_ne = 0;
		break;
	case PN53X_NAK:
		if (pn53x_stub_ne && pn53x_stub_is_resp)
			pn53x_stub_ready = true;
		break;
	default:
		/* validate LCS, DCS, TFI */
		pn53x_stub_is_cmd = true;
		pn53x_stub_is_resp = false;
		pn53x_stub_resp[0] = 0x00;
		pn53x_stub_resp[1] = 0x00;
		pn53x_stub_resp[2] = 0xFF;
		pn53x_stub_resp[3] = 0x00;
		pn53x_stub_resp[4] = 0xFF;
		pn53x_stub_resp[5] = 0x00;
		pn53x_stub_ne = 6;
		pn53x_stub_ready = true;
		break;
	}
}

uint8_t pn53x_hw_xchg_read(scs_off_t index)
{
	if (index < pn53x_stub_ne)
		return pn53x_stub_resp[index];
	return 0;
}

void pn53x_hw_xchg_write(scs_off_t index, uint8_t val)
{
	pn53x_stub_cmd[index] = val;
}

void pn53x_hw_ctrl_init(void)
{
	pn53x_stub_nc = 0;
	pn53x_stub_ne = 0;
	pn53x_stub_ready = false;
	pn53x_stub_is_resp = false;
	pn53x_stub_is_cmd = false;
}
