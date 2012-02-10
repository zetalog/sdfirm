#include <driver/pn53x.h>

uint8_t pn53x_stub_cmd[PN53X_BUF_SIZE];
uint8_t pn53x_stub_resp[PN53X_BUF_SIZE];
scs_size_t pn53x_stub_nc;
scs_size_t pn53x_stub_ne;
boolean pn53x_stub_ready;
boolean pn53x_stub_is_resp;
boolean pn53x_stub_is_cmd;
static uint8_t pn53x_flags;
static uint8_t pn53x_serial_br;
static uint8_t pn53x_sfr_p3;
static uint8_t pn53x_sfr_p7;
static uint8_t pn53x_hci_mode = PN53X_HCI_SPI;
static struct nfc_target pn53x_targets[NR_PN53X_SLOTS];
static uint8_t pn53x_nr_targets;
static uint8_t pn53x_ciu_regs[PN53X_REG_CIU_SIZE] = {
	0x00, 0x3B, 0x00, 0x00, 0x80, 0x00, 0x10, 0x84, /* 0x00 */
	0x84, 0x4F, 0x00, 0x00, 0x62, 0x00, 0x00, 0x00, /* 0x08 */
	0x00, 0xFF, 0xFF, 0x88, 0x26, 0x87, 0x48, 0x88, /* 0x10 */
	0x20, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 0x18 */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x80, /* 0x20 */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 0x28 */
	0x00, 0x20, 0x00, 0x00, 0x14, 0x00, 0x00, 0x21, /* 0x30 */
	0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0xA0, 0x00  /* 0x38 */
};
static uint8_t pn53x_ciu_writables[PN53X_REG_CIU_SIZE] = {
	0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xEF, 0xFF, 0xFF, /* 0x00 */
	0xF7, 0xEF, 0xFF, 0xFF, 0xFF, 0x7F, 0xDF, 0x00, /* 0x08 */
	0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, /* 0x10 */
	0x3F, 0x3F, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, /* 0x18 */
	0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x0F, 0x00, /* 0x20 */
	0xFF, 0x3F, 0x3F, 0x00, 0x00, 0x00, 0x00, 0xFF, /* 0x28 */
	0x8F, 0x3F, 0x7F, 0x2F, 0xFF, 0x9F, 0x00, 0x00, /* 0x30 */
	0x98, 0xFF, 0x80, 0x3F, 0xF0, 0xF7, 0x80, 0x00  /* 0x38 */
};
static uint8_t pn53x_ciu_readables[PN53X_REG_CIU_SIZE] = {
	0x00, 0xFF, 0xFF, 0xFF, 0xFB, 0xFB, 0xFF, 0xFF, /* 0x00 */
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, /* 0x08 */
	0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, /* 0x10 */
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, /* 0x18 */
	0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, /* 0x20 */
	0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0xFF, /* 0x28 */
	0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0x7F, 0xFF, 0xFF, /* 0x30 */
	0xFF, 0xFF, 0x7F, 0xFF, 0x3F, 0x7F, 0xFF, 0x00  /* 0x38 */
};
static uint8_t pn53x_ctrl_reg_switch_rng;

void pn53x_build_frame(uint8_t len)
{
	scs_size_t ne;
	uint8_t dcs = 0x00;
	uint8_t i;

	pn53x_stub_resp[PN53X_LEN] = len;
	pn53x_stub_resp[PN53X_LCS] = (uint8_t)(0x00-len);

	/* Calculate DCS. */
	ne = PN53X_NORMAL_SIZE(pn53x_stub_resp);
	for (i = 0; i < pn53x_stub_resp[PN53X_LEN]; i++)
		dcs -= pn53x_stub_resp[PN53X_TFI+i];
	pn53x_stub_resp[ne-PN53X_TAIL_SIZE] = dcs;
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
	pn53x_build_frame(2);
}

void pn53x_response_Diagnose(void)
{
	uint8_t len;
	switch (pn53x_stub_cmd[PN53X_PD(0)]) {
	case PN53X_DIAG_CMDLINE:
		len = pn53x_stub_cmd[PN53X_LEN];
		memory_copy((caddr_t)(pn53x_stub_resp)+PN53X_PD(0),
			    (caddr_t)(pn53x_stub_cmd)+PN53X_PD(0),
			    len-2);
		break;
	case PN53X_DIAG_ROMTEST:
	case PN53X_DIAG_RAMTEST:
		pn53x_stub_resp[PN53X_PD(0)] = PN53X_DIAG_STATUS_OK;
		len = 3;
		break;
	case PN53X_DIAG_POLL_TARGET:
	case PN53X_DIAG_ECHO_BACK:
	case PN53X_DIAG_DETECT_14443:
	case PN53X_DIAG_ANTENNA_TEST:
		pn53x_response_error(PN53X_ERR_CMD);
		return;
	default:
		pn53x_response_error(PN53X_ERR_CMD);
		return;
	}
	pn53x_build_frame(len);
}

void pn53x_response_GetFirmwareVersion(void)
{
	pn53x_stub_resp[PN53X_CMD+1] = 0x32;
	pn53x_stub_resp[PN53X_CMD+2] = 0x01;
	pn53x_stub_resp[PN53X_CMD+3] = 0x06;
	pn53x_stub_resp[PN53X_CMD+4] = 0x07;
	pn53x_build_frame(6);
}

void pn53x_response_SetParameters(void)
{
	pn53x_flags = pn53x_stub_cmd[PN53X_PD(0)];
	/* if (bits_raised(pn53x_flags, PN53X_PARAM_AUTO_ATR_RES) &&
	    bits_raised(pn53x_flags, PN53X_PARAM_AUTO_RATS))
	if (bits_raised(pn53x_flags, PN53X_PARAM_14443_4_PICC)) */
	pn53x_build_frame(2);
}

static uint8_t pn53x_target_iso14443a_size(uint8_t cid)
{
	uint8_t size = 4;
	uint8_t len;

	len = pn53x_targets[cid].nti.nai.szAtsLen;
	if (len)
		size += (1+len);
	size += pn53x_targets[cid].nti.nai.szUidLen;
	return size;
}

static uint8_t pn53x_target_info_size(uint8_t cid)
{
	uint8_t size = 1;
	switch (pn53x_targets[cid].nm.nmt) {
	case NFC_TYPE_ISO14443A:
		size += pn53x_target_iso14443a_size(cid);
		break;
	default:
		BUG();
		break;
	}
	return size;
}

static void pn53x_target_iso14443a_data(uint8_t cid,
					scs_off_t offset)
{
	uint8_t len;
	pn53x_stub_resp[offset++] = pn53x_targets[cid].nti.nai.abtAtqa[0];
	pn53x_stub_resp[offset++] = pn53x_targets[cid].nti.nai.abtAtqa[1];
	pn53x_stub_resp[offset++] = pn53x_targets[cid].nti.nai.btSak;
	len = pn53x_targets[cid].nti.nai.szUidLen;
	pn53x_stub_resp[offset++] = len;
	memory_copy((caddr_t)(pn53x_stub_resp)+offset,
		    (caddr_t)(pn53x_targets[cid].nti.nai.abtUid),
		    len);
	offset += len;
	len = pn53x_targets[cid].nti.nai.szAtsLen;
	if (len) {
		pn53x_stub_resp[offset++] = len;
		memory_copy((caddr_t)(pn53x_stub_resp)+offset,
			    (caddr_t)(pn53x_targets[cid].nti.nai.abtAts),
			    len);
	}
}

static void pn53x_target_info_data(uint8_t cid,
				   scs_off_t offset)
{
	pn53x_stub_resp[offset++] = cid;
	switch (pn53x_targets[cid].nm.nmt) {
	case NFC_TYPE_ISO14443A:
		pn53x_target_iso14443a_data(cid, offset);
		break;
	default:
		BUG();
		break;
	}
}

static void pn53x_poll_targets(uint8_t period)
{
}

void pn53x_response_InAutoPoll(void)
{
	uint8_t nr_poll;
	uint8_t period;
	uint8_t i;
	scs_off_t offset;

	nr_poll = pn53x_stub_cmd[PN53X_PD(0)];
	period = pn53x_stub_cmd[PN53X_PD(1)];
	do {
		if (pn53x_nr_targets == 0)
			pn53x_poll_targets(period);
		if (pn53x_nr_targets > 0)
			break;
		if (nr_poll != PN53X_POLL_INFINITE)
			nr_poll--;
	} while (nr_poll > 0);
	offset = PN53X_PD(0);
	pn53x_stub_resp[offset++] = pn53x_nr_targets;
	for (i = 0; i < pn53x_nr_targets; i++) {
		uint8_t size = pn53x_target_info_size(i);
		pn53x_stub_resp[offset++] =
			pn53x_nm_to_ptt(pn53x_targets[i].nm);
		pn53x_stub_resp[offset++] = size;
		pn53x_target_info_data(i, offset);
		offset += size;
	}
	pn53x_build_frame(offset-PN53X_TFI);
}


static boolean pn53x_is_ciu_register(uint16_t reg)
{
	return (reg >= PN53X_REG_CIU_BASE) &&
	       (reg < PN53X_REG_CIU_BASE + PN53X_REG_CIU_SIZE);
}

void pn53x_ciu_write_register(uint16_t reg, uint8_t val)
{
	uint8_t index = PN53X_CIU_REG2INDEX(reg);
	BUG_ON(!pn53x_is_ciu_register(reg));
	pn53x_ciu_regs[index] = val;
}

uint8_t pn53x_ciu_read_register(uint16_t reg)
{
	uint8_t index = PN53X_CIU_REG2INDEX(reg);
	BUG_ON(!pn53x_is_ciu_register(reg));
	return pn53x_ciu_regs[index];
}

void pn53x_response_ReadRegister(void)
{
	uint8_t i, j;
	uint16_t reg;
	uint8_t val;

	for (i = 0, j = 0;
	     i < (pn53x_stub_cmd[PN53X_LEN]-2);
	     i += 2, j++) {
		reg = MAKEWORD(pn53x_stub_cmd[PN53X_PD(1)+i],
			       pn53x_stub_cmd[PN53X_PD(0)+i]);
		if (!pn53x_is_ciu_register(reg)) {
			if (reg == PN53X_REG_Control_switch_rng) {
				val = pn53x_ctrl_reg_switch_rng;
			} else {
				pn53x_response_error(PN53X_ERR_CMD);
				return;
			}
		} else {
			uint8_t mask;
			uint8_t index = PN53X_CIU_REG2INDEX(reg);
			mask = pn53x_ciu_readables[index];
			val = (pn53x_ciu_read_register(reg) & mask);
		}
		pn53x_stub_resp[PN53X_PD(0)+j] = val;
	}
	pn53x_build_frame(j+2);
}

void pn53x_response_WriteRegister(void)
{
	uint8_t i;
	uint8_t val;
	uint16_t reg;

	for (i = 0;
	     i < (pn53x_stub_cmd[PN53X_LEN]-2);
	     i += 3) {
		reg = MAKEWORD(pn53x_stub_cmd[PN53X_PD(1)+i],
			       pn53x_stub_cmd[PN53X_PD(0)+i]);
		val = pn53x_stub_cmd[PN53X_PD(2)+i];
		if (!pn53x_is_ciu_register(reg)) {
			if (reg == PN53X_REG_Control_switch_rng) {
				pn53x_ctrl_reg_switch_rng = val;
			} else {
				pn53x_response_error(PN53X_ERR_CMD);
				return;
			}
		} else {
			uint8_t mask;
			uint8_t index;
			index = PN53X_CIU_REG2INDEX(reg);
			mask = pn53x_ciu_writables[index];
			pn53x_ciu_write_register(reg, val & mask);
		}
	}
	pn53x_build_frame(2);
}

void pn53x_response_ReadGPIO(void)
{
	pn53x_stub_resp[PN53X_PD(0)] = (pn53x_sfr_p3 & PN53X_P3_MASK);
	pn53x_stub_resp[PN53X_PD(1)] = (pn53x_sfr_p7 & PN53X_P7_MASK);
	pn53x_stub_resp[PN53X_PD(2)] = pn53x_hci_mode;
	pn53x_build_frame(5);
}

void pn53x_response_WriteGPIO(void)
{
	pn53x_sfr_p3 = pn53x_stub_cmd[PN53X_PD(0)];
	pn53x_sfr_p7 = pn53x_stub_cmd[PN53X_PD(1)];
	pn53x_build_frame(2);
}

void pn53x_response_SetSerialBaudRate(void)
{
	pn53x_serial_br = pn53x_stub_cmd[PN53X_PD(0)];
	pn53x_build_frame(2);
}

void pn53x_response_SAMConfiguration(void)
{
	uint8_t mode;
	uint8_t timeout;
	uint8_t irq;
	mode = pn53x_stub_cmd[PN53X_PD(0)];
	timeout = pn53x_stub_cmd[PN53X_PD(1)];
	if (pn53x_stub_cmd[PN53X_LEN] > 4)
		irq = pn53x_stub_cmd[PN53X_PD(2)];
	pn53x_build_frame(2);
}

void pn53x_response_PowerDown(void)
{
	uint8_t wakeupenable;
	uint8_t generateirq;
	wakeupenable = pn53x_stub_cmd[PN53X_PD(0)];
	if (pn53x_stub_cmd[PN53X_LEN] > 3)
		generateirq = pn53x_stub_cmd[PN53X_PD(1)];
	pn53x_response_error(PN53X_ERR_SUCCESS);
}

void pn53x_response_RFConfiguration(void)
{
	uint8_t type = pn53x_stub_cmd[PN53X_PD(0)];

	switch (type) {
	case PN53X_RFCI_FIELD:
	case PN53X_RFCI_RETRY_DATA:
		break;
	case PN53X_RFCI_TIMING:
	case PN53X_RFCI_RETRY_SELECT:
	case PN53X_RFCI_ANALOG_TYPE_B:
		break;
	case PN53X_RFCI_ANALOG_TYPE_A_212_424:
		break;
	case PN53X_RFCI_ANALOG_TYPE_14443_4:
		break;
	case PN53X_RFCI_ANALOG_TYPE_A_106:
		break;
	default:
		BUG();
		break;
	}
	pn53x_build_frame(2);
}

void pn53x_response_InListPassiveTarget(void)
{
	uint8_t i;
	uint8_t modulation;
	uint8_t max_targets, nr_targets;
	scs_off_t offset;

	if (pn53x_stub_cmd[PN53X_PD(0)] > 2) {
		pn53x_response_error(PN53X_ERR_CMD);
		return;
	}
	max_targets = min(pn53x_nr_targets, pn53x_stub_cmd[PN53X_PD(0)]);
	modulation = pn53x_stub_cmd[PN53X_PD(1)];
	nr_targets = 0;
	offset = PN53X_PD(1);
	for (i = 0; i < pn53x_nr_targets; i++) {
		if (pn53x_nm_to_pm(pn53x_targets[i].nm) == modulation) {
			nr_targets++;
			pn53x_target_info_data(i, offset);
			offset += pn53x_target_info_size(i);
		}
	}
	pn53x_stub_resp[PN53X_PD(0)] = nr_targets;
	pn53x_build_frame(offset-PN53X_TFI);
}

void pn53x_xchg_pseudo(void)
{
	uint8_t cmd;
	uint8_t i;
	uint8_t dcs;

	/* Validate LCS. */
	if (0x00 != (uint8_t)(pn53x_stub_cmd[PN53X_LEN]+
			      pn53x_stub_cmd[PN53X_LCS]))
		return;
	/* Validate TFI. */
	if (pn53x_stub_cmd[PN53X_TFI] != PN53X_OUT)
		return;
	/* Validate DCS. */
	for (i = 0, dcs = 0; i <= pn53x_stub_cmd[PN53X_LEN]; i++)
		dcs += pn53x_stub_cmd[PN53X_TFI+i];
	if (dcs != 0x00)
		return;
	pn53x_stub_resp[0] = 0x00;
	pn53x_stub_resp[1] = 0x00;
	pn53x_stub_resp[2] = 0xFF;
	pn53x_stub_resp[PN53X_TFI] = PN53X_IN;
	pn53x_stub_resp[PN53X_CMD] = pn53x_stub_cmd[PN53X_CMD] + 1;
	cmd = pn53x_stub_cmd[PN53X_CMD];
	pn53x_debug(PN53X_DEBUG_CMD_CODE, cmd);
	switch (cmd) {
	case PN53X_Diagnose:
		pn53x_response_Diagnose();
		break;
	case PN53X_GetFirmwareVersion:
		pn53x_response_GetFirmwareVersion();
		break;
	case PN53X_ReadRegister:
		pn53x_response_ReadRegister();
		break;
	case PN53X_WriteRegister:
		pn53x_response_WriteRegister();
		break;
	case PN53X_ReadGPIO:
		pn53x_response_ReadGPIO();
		break;
	case PN53X_WriteGPIO:
		pn53x_response_WriteGPIO();
		break;
	case PN53X_AlparCommandForTDA:
		pn53x_response_error(PN53X_ERR_CMD);
		break;
	case PN53X_SetSerialBaudRate:
		pn53x_response_SetSerialBaudRate();
		break;
	case PN53X_SetParameters:
		pn53x_response_SetParameters();
		break;
	case PN53X_SAMConfiguration:
		pn53x_response_SAMConfiguration();
		break;
	case PN53X_PowerDown:
		pn53x_response_PowerDown();
		break;
	case PN53X_RFConfiguration:
		pn53x_response_RFConfiguration();
		break;
	case PN53X_InJumpForDEP:
	case PN53X_InJumpForPSL:
	case PN53X_InActivateDeactivatePaypass:
		pn53x_response_error(PN53X_ERR_CMD);
		break;
	case PN53X_InListPassiveTarget:
		pn53x_response_InListPassiveTarget();
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
		pn53x_response_InAutoPoll();
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

	pn53x_nr_targets = 1;
	pn53x_targets[0].nm.nmt = NFC_TYPE_ISO14443A;
	pn53x_targets[0].nm.nbr = NFC_BAUD_106;
	pn53x_targets[0].nti.nai.abtAtqa[0] = 0x00;
	pn53x_targets[0].nti.nai.abtAtqa[1] = 0x04;
	pn53x_targets[0].nti.nai.btSak = 0x08;
	pn53x_targets[0].nti.nai.szUidLen = 0x04;
	pn53x_targets[0].nti.nai.abtUid[0] = 0xC3;
	pn53x_targets[0].nti.nai.abtUid[1] = 0xB2;
	pn53x_targets[0].nti.nai.abtUid[2] = 0xEF;
	pn53x_targets[0].nti.nai.abtUid[3] = 0xC8;
	pn53x_targets[0].nti.nai.szAtsLen = 0x00;
}
