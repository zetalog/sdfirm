#include "ut_priv.h"

#define CCID_XCHG_IS_LEVEL(xchg)	\
	((g_desc.dwFeatures & SCD_FEATURE_XCHG_MASK) == xchg)

#define MAX_PPS_LEN	6
#define PPSS		0xff

boolean g_card_in = false;
boolean g_atr_ready = false;
boolean g_pps_ready = false;
scd_desc_t g_desc;
struct libusb_device_descriptor g_dev;
struct libusb_config_descriptor g_conf;
struct libusb_interface_descriptor g_intf;

/* ATR result */
uint32_t g_atr_len;
uint8_t g_atr_buff[33];
uint8_t g_info_xchg = CCID_INFO_XCHG_SPECIFIC;
uint8_t g_ind_conv = ATR_TS_DIRECT;
uint8_t g_ind_fd = ATR_DEFAULT_FD;
uint8_t g_ind_n = ATR_DEFAULT_N;
uint8_t g_ind_t = SCS_PROTO_T0;	
uint8_t g_ind_t0_wi;
uint8_t g_ind_t1_ifsc = ATR_DEFAULT_IFSC;
uint8_t g_ind_t1_cwi = ATR_DEFAULT_CWI;
uint8_t g_ind_t1_bwi = ATR_DEFAULT_BWI;
uint8_t g_ind_t1_crc = ATR_DEFAULT_CRC;
uint8_t g_ind_t1_nad = 0;

/* PPS result */
struct ccid_t0_param g_t0_param;
struct ccid_t1_param g_t1_param;
uint8_t g_curr_t;
uint8_t g_curr_fd;
uint8_t g_curr_n;
uint8_t g_curr_wi;
uint8_t g_curr_cwi;
uint8_t g_curr_bwi;
uint8_t g_curr_ifsc;
uint8_t g_curr_nad;
uint8_t g_curr_crc;
uint8_t g_curr_class = CCID_CLASS_AUTO;
uint32_t g_freq = 0;
uint32_t g_data = 0;
uint8_t g_clk_status = CCID_CLOCK_RUNNING;

uint32_t *g_data_rates = NULL;
uint32_t *g_clock_freqs = NULL;

/* device / config / xxx descriptor info */
uint16_t *g_langids = NULL;
uint8_t g_lang_num = 0;

/* XfrBlock cmd/resp */
uint8_t g_pcsc_cmd[261];
uint8_t g_pcsc_resp[261];

#ifdef NYI
#define RFU -1

static int F_table[16] = {
	372, 372, 558, 744, 1116, 1488, 1860, RFU,
	RFU, 512, 768, 1024, 1536, 2048, RFU, RFU,
};

static int fmax_table[16] = {
	4, 5, 6, 8, 12, 16, 20, RFU,
	RFU, 5, 7, 10, 15, 20, RFU, RFU,
};

static int D_table[16] = {
	RFU, 1, 2, 4, 8, 16, 32, 64,
	12, 20, RFU, RFU, RFU, RFU, RFU, RFU,
};
#endif
static int ccid_build_apdu(uint8_t type, int *ne) __attribute__((unused));

static int ccid_build_pps(void)
{
	uint8_t *p;
	uint8_t ppss = 0, pps0 = 0, pps1 = 0, pck = 0;

	ppss = PPSS;
	pps0 |= g_ind_t;
	pps0 |= 0x10;	/* only hanve pps1 */
	pps1 = g_ind_fd;

	pck = (ppss ^ pps0 ^ pps1);

	p = g_pcsc_cmd;
	*p++ = ppss;
	*p++ = pps0;
	*p++ = pps1;
	*p++ = pck;

	/* pps length */
	return 4;
}

static int ccid_build_apdu(uint8_t type, int *ne)
{
	return pcsc_build_apdu(g_pcsc_cmd, type, ne);
}

static void ccid_indicate_param(struct pcsc_atr *info)
{
	if (info->ib[1][ATR_INTERFACE_BYTE_TA].present)
		g_ind_fd = info->ib[1][ATR_INTERFACE_BYTE_TA].value;
	else
		g_ind_fd = ATR_DEFAULT_FD;

	if (info->ib[1][ATR_INTERFACE_BYTE_TC].present)
		g_ind_n = info->ib[1][ATR_INTERFACE_BYTE_TC].value;
	else
		g_ind_n = ATR_DEFAULT_N;

	if (info->ib[2][ATR_INTERFACE_BYTE_TA].present) {
		g_info_xchg = CCID_INFO_XCHG_SPECIFIC;	
	} else {
		g_info_xchg = CCID_INFO_XCHG_NEGO_PPS;
	}

	if (info->ib[2][ATR_INTERFACE_BYTE_TC].present) {
		g_ind_t0_wi = info->ib[2][ATR_INTERFACE_BYTE_TC].value;
	} else {
		g_ind_t0_wi = 10;
	}

	g_ind_t = info->default_proto;
	g_ind_conv = info->TS;

	log_msg(LOG_DEBUG, "parse atr, ind proto=%02X", g_ind_t);
}

static void ccid_config_t0_param(void)
{
	g_curr_wi = g_ind_t0_wi;
}

static void ccid_config_t1_param(void)
{
	g_curr_cwi = g_ind_t1_cwi;
	g_curr_bwi = g_ind_t1_bwi;
	g_curr_ifsc = g_ind_t1_ifsc;
	g_curr_nad = g_ind_t1_nad;
	g_curr_crc = g_ind_t1_crc;
}

static void ccid_parse_pps(void)
{
	g_curr_t = g_ind_t;
	g_curr_fd = g_ind_fd;
	g_curr_n = g_ind_n;

	if (g_curr_t == SCS_PROTO_T0) {
		ccid_config_t0_param();
	} else if (g_curr_t == SCS_PROTO_T1) {
		ccid_config_t1_param();
	}
}

static void ccid_parse_atr(void)
{
	if (0 == pcsc_parse_atr(g_atr_buff, g_atr_len, &g_pcsc_atr)) {
		ccid_indicate_param(&g_pcsc_atr);
	}
}

void ccid_parse_SlotStatus(uint8_t bStatus, uint8_t bError)
{
	CUNIT_ASSERT_EQUALS(uchar, bStatus, ccid_resp_status());
	CUNIT_ASSERT_EQUALS(uchar, bError, ccid_resp_error());
}

void CUNITCBK ccid_power_off(void)
{
	if (!g_card_in)
		return;

	g_atr_ready = false;
	g_pps_ready = false;
	g_atr_len = -1;
	g_curr_class = CCID_CLASS_AUTO;

	CUNIT_ASSERT_TRUE(ccid_sync_IccPowerOff());
	ccid_parse_SlotStatus(SCD_SLOT_STATUS_INACTIVE, 0);
}

void CUNITCBK ccid_slot_status(void)
{
	uint8_t bStatus;
	g_card_in = false;

	CUNIT_ASSERT_TRUE(ccid_sync_GetSlotStatus());
	bStatus = ccid_resp_status();
	if (!(g_desc.dwFeatures & SCD_FEATURE_AUTO_CLASS)) {
		CUNIT_ASSERT_TRUE(bStatus == SCD_SLOT_STATUS_INACTIVE);
	} else {
		CUNIT_ASSERT_TRUE(bStatus != SCD_SLOT_STATUS_NOTPRESENT);
	}
	g_card_in = true;
}

void ccid_apply_class(uint8_t bClass)
{
	CUNIT_ASSERT_TRUE(ccid_sync_IccPowerOn(bClass));
	CUNIT_ASSERT_TRUE(ccid_resp_length() >= 0);
	ccid_parse_SlotStatus(SCD_SLOT_STATUS_ACTIVE, 0);

	g_atr_len = ccid_resp_block(g_atr_buff, sizeof (g_atr_buff));
	g_atr_ready = true;
	g_curr_class = bClass;
}

void CUNITCBK ccid_power_on(void)
{
	uint8_t bClass = CCID_CLASS_AUTO;

	if (!g_card_in)
		return;

	if (!(g_desc.dwFeatures & SCD_FEATURE_AUTO_CLASS)) {
		do {
			/* have poweron success before */
			if (g_curr_class != CCID_CLASS_AUTO)
				bClass = g_curr_class;
			else
				bClass++;
			ccid_apply_class(bClass);
		} while (!g_atr_ready && bClass < CCID_CLASS_INVAL);
	} else {
		ccid_apply_class(bClass);
	}
	ccid_parse_atr();
}

void CUNITCBK ccid_get_param(void)
{
	if (!g_atr_ready)
		return;

	CUNIT_ASSERT_TRUE(ccid_sync_GetParameters(g_curr_t));
	switch (g_curr_t) {
	case SCS_PROTO_T0:
		ccid_resp_param(&g_t0_param);
		break;
	case SCS_PROTO_T1:
		ccid_resp_param(&g_t1_param);
		break;
	}
}

void CUNITCBK ccid_t0_set_param(void)
{
	struct ccid_t0_param t0;

	t0.bmFindexDindex = g_ind_fd;
	t0.bmTCCKST0 = g_ind_conv << 1;
	t0.bGuardTimeT0 = g_ind_n;
	t0.bWaitingIntegerT0 = g_ind_t0_wi;
	t0.bClockStop = g_clk_status;

	CUNIT_ASSERT_TRUE(ccid_sync_SetParameters(SCS_PROTO_T0, &t0));

	g_curr_t = ccid_resp_param(&g_t0_param);
}

void CUNITCBK ccid_t1_set_param(void)
{
	struct ccid_t1_param t1;

	t1.bmFindexDindex = g_ind_fd;
	t1.bmTCCKST1 = (uint8_t)((g_ind_conv << 1) | g_ind_t1_crc | (0x04 << 2));
	t1.bGuardTimeT1 = g_curr_n;
	t1.bWaitingIntegerT1 = (g_ind_t1_bwi << 4) | g_ind_t1_cwi;
	t1.bClockStop = g_clk_status;
	t1.bIFSC = g_ind_t1_ifsc;
	t1.bNadValue = g_ind_t1_nad;

	CUNIT_ASSERT_TRUE(ccid_sync_SetParameters(SCS_PROTO_T1,
						  &t1));

	g_curr_t = ccid_resp_param(&g_t1_param);
}

void CUNITCBK ccid_nego_pps(void)
{
	if (CCID_XCHG_IS_LEVEL(SCD_FEATURE_XCHG_APDU)) {
		int rlen, slen;
		
		slen = ccid_build_pps();
		CUNIT_ASSERT_TRUE(slen > 0 && slen <= MAX_PPS_LEN);
		CUNIT_ASSERT_TRUE(ccid_sync_XfrBlock(0, 0, g_pcsc_cmd, slen));

		rlen = ccid_resp_block(g_pcsc_resp, sizeof (g_pcsc_resp));
		CUNIT_ASSERT_EQUALS(int, rlen, slen);
	} else {
		/* TODO: NYI */
	}
}

void CUNITCBK ccid_spec_mode(void)
{
	ccid_parse_pps();
}

void CUNITCBK ccid_set_param(void)
{
	if (!g_atr_ready)
		return;

	if (g_info_xchg == CCID_INFO_XCHG_SPECIFIC) {
		ccid_spec_mode();
		goto end;
	}

	if (!(g_desc.dwFeatures & SCD_FEATURE_AUTO_CONFIG)) {
		ccid_nego_pps();
	}

	if (!(g_desc.dwFeatures & SCD_FEATURE_AUTO_PPS_PROP)) {
		switch (g_ind_t) {
		case SCS_PROTO_T0:
			ccid_t0_set_param();
			break;
		case SCS_PROTO_T1:
			ccid_t1_set_param();
			break;
		default:
			break;
		}
	}

end:
	g_pps_ready = true;
	ccid_parse_pps();
}

void CUNITCBK ccid_reset_param(void)
{
	if (!g_pps_ready)
		return;

	g_pps_ready = false;
	CUNIT_ASSERT_TRUE(ccid_sync_ResetParameters());
	g_curr_t = ccid_resp_param(NULL);

	switch (g_curr_t) {
	case SCS_PROTO_T0:
		ccid_resp_param(&g_t0_param);
		break;
	case SCS_PROTO_T1:
		ccid_resp_param(&g_t1_param);
		break;
	}
}

void CUNITCBK ccid_t0_xfr_block(void)
{

	if (!g_atr_ready)
		return;

	if (g_curr_t != SCS_PROTO_T0)
		return;

	if (CCID_XCHG_IS_LEVEL(SCD_FEATURE_XCHG_APDU) ||
	    CCID_XCHG_IS_LEVEL(SCD_FEATURE_XCHG_TPDU)) {
		int rlen, slen;

		slen = pcsc_select_file(g_pcsc_cmd, 0x3F00);
		CUNIT_ASSERT_TRUE(slen > 0);
		CUNIT_ASSERT_TRUE(ccid_sync_XfrBlock(0, 0, g_pcsc_cmd, slen));

		rlen = ccid_resp_block(g_pcsc_resp, sizeof (g_pcsc_resp));
	} else {
		/* TODO: NIY */
	}
}

void CUNITCBK ccid_t1_xfr_block(void)
{
	if (g_curr_t != SCS_PROTO_T1)
		return;
	/* TODO: NIY */
}

void CUNITCBK ccid_icc_clock(void)
{
	uint8_t bClockCommand;

	if (!(g_desc.dwFeatures & SCD_FEATURE_CAP_CLOCK_STOP))
		return;
	if (g_clk_status == CCID_CLOCK_RUNNING)
		bClockCommand = CCID_CLOCK_RESTART;
	else
		bClockCommand = CCID_CLOCK_STOP;
	CUNIT_ASSERT_TRUE(ccid_sync_IccClock(bClockCommand));
}

void CUNITCBK ccid_icc_mecha(void)
{
	if (!(g_desc.dwMechanical & SCD_MECHA_LOCK))
		return;

	/* lock first */
	CUNIT_ASSERT_TRUE(ccid_sync_Mechanical(CCID_MECHA_LOCK_CARD));
	ccid_parse_SlotStatus(SCD_SLOT_STATUS_ACTIVE, 0);

	/* unlock */
	CUNIT_ASSERT_TRUE(ccid_sync_Mechanical(CCID_MECHA_UNLOCK_CARD));
	ccid_parse_SlotStatus(SCD_SLOT_STATUS_ACTIVE, 0);
}

void ccid_parse_DataAndFreq(uint32_t dwClockFreq, uint32_t dwDataRate)
{
	if (ccid_resp_fail())
		return;

	/* IFD must not auto change them */
	CUNIT_ASSERT_EQUALS(ulong, dwClockFreq, g_pcsc_resp[10]);
	CUNIT_ASSERT_EQUALS(ulong, dwDataRate, g_pcsc_resp[14]);
}

void ccid_busy_cmpl1(void *unused)
{
	CUNIT_ASSERT_TRUE(!ccid_resp_fail());
	CUNIT_ASSERT_TRUE(ccid_resp_error() != CCID_ERROR_CMD_SLOT_BUSY);
}

void ccid_busy_cmpl2(void *unused)
{
	if (ccid_resp_fail()) {
		CUNIT_ASSERT_TRUE(ccid_resp_error() == CCID_ERROR_CMD_SLOT_BUSY);
	}
}

void CUNITCBK ccid_slot_busy(void)
{
	if (g_desc.bMaxCCIDBusySlots <= 1)
		return;

	CUNIT_ASSERT_TRUE(ccid_async_GetSlotStatus(ccid_busy_cmpl1, NULL) != 
			  CCID_INVALID_SEQ);
	CUNIT_ASSERT_TRUE(ccid_async_GetSlotStatus(ccid_busy_cmpl2, NULL) !=
			  CCID_INVALID_SEQ);
	ccid_async_in();
}

void ccid_abort_cmpl1(void *unused)
{
	CUNIT_ASSERT_TRUE(ccid_resp_fail());
	CUNIT_ASSERT_TRUE(ccid_resp_error() == CCID_ERROR_CMD_ABORTED);
}

void ccid_abort_cmpl2(void *unused)
{
	CUNIT_ASSERT_TRUE(!ccid_resp_fail());
	CUNIT_ASSERT_TRUE(ccid_resp_error() == 0);
}

/* Abort Sequence:
 * send abortable command(slot, Seq1) ->
 *   get status=failure, error=CMD_ABORTED
 * send ctrl ABORT command(slot, Seq2)
 * send bulk PC_to_RDR_Abort command(slot, Seq2) ->
 *   get status=success
 */
void CUNITCBK ccid_slot_abort(void)
{
	uint8_t seq;

	if (g_desc.bMaxCCIDBusySlots <= 1)
		return;

	/* ccid_async_XfrBlock(ccid_abort_cmpl1, NULL) */
	seq = ccid_seq_alloc();
	CUNIT_ASSERT_TRUE(seq != CCID_INVALID_SEQ);
	log_msg(LOG_INFO, "ccid_slot_abort: seq=%d", seq);
	CUNIT_ASSERT_TRUE(ccid_ctrl_cls(LIBUSB_ENDPOINT_OUT, CCID_REQ_ABORT,
					(uint16_t)(seq << 8 | CCID_SLOT_NO),
					NULL, 0) >= 0);
	CUNIT_ASSERT_TRUE(ccid_async_Abort(seq, ccid_abort_cmpl2, NULL));
	ccid_async_in();
}

void CUNITCBK ccid_pin_verify(void)
{
	int len;
	struct ccid_pv_param pv;

	if ((g_desc.bPINSupport & 0x01) == 0)
		return;

	len = pcsc_pin_verify(&pv, g_pcsc_cmd);
	if (len > 0) {
		CUNIT_ASSERT_TRUE(ccid_sync_Secure(0, 0, CCID_SPE_PIN_VERIFY,
						   &pv, g_pcsc_cmd, len));
	}
}

void CUNITCBK ccid_pin_modify(void)
{
	int len;
	struct ccid_pm_param pm;

	if ((g_desc.bPINSupport & 0x02) == 0)
		return;
	len = pcsc_pin_modify(&pm, g_pcsc_cmd);
	if (len > 0) {
		CUNIT_ASSERT_TRUE(ccid_sync_Secure(0, 0, CCID_SPE_PIN_MODIFY,
						   &pm, g_pcsc_cmd, len));
	}
}

#define CCID_WAIT_FAST		1000
#define CCID_WAIT_SLOW		60*1000

void CUNITCBK ccid_warmup(void)
{
	ccid_set_timeout(CCID_WAIT_FAST);
	while (__ccid_async_in());
	while (!ccid_sync_GetSlotStatus() ||
	       ccid_resp_status() == SCD_SLOT_STATUS_NOTPRESENT);
	ccid_sync_IccPowerOff();
	ccid_set_timeout(CCID_WAIT_SLOW);
}

void CUNITCBK ccid_teardown(void)
{
	ccid_set_timeout(CCID_WAIT_FAST);
	ccid_sync_IccPowerOff();
	ccid_set_timeout(CCID_WAIT_SLOW);
}

void usb_dump_string(const char *head, uint8_t id)
{
	uint16_t len, res;
	uint8_t peak[2], i;
	uint8_t *buffer = NULL;
	char *string = NULL;
	int buffer_len = 0;

	res = usb_get_string(id, g_langids[0], peak, 2, CCID_WAIT_SLOW);
	CUNIT_ASSERT_EQUALS(int, res, 2);

	len = peak[0];
	buffer = malloc(len);
	string = malloc(len/2);
	CUNIT_ASSERT_TRUE(string != NULL);
	CUNIT_ASSERT_TRUE(buffer != NULL);
	buffer_len = len;

	res = usb_get_string(id, g_langids[0], buffer, len, CCID_WAIT_SLOW);
	CUNIT_ASSERT_EQUALS(int, res, len);

	for (i = 0; i < (len-2)/2; i++) {
		string[i] = buffer[2+i*2];
	}
	string[i] = '\0';

	log_msg(LOG_DEBUG, "%s%s", head, string);
}

void CUNITCBK get_string(void)
{
	int len, res;
	uint8_t i;
	uint8_t head[2];
	uint8_t *buffer = NULL;
	int buffer_len = 0;

	if (g_langids) {
		free(g_langids);
		g_langids = NULL;
		g_lang_num = 0;
	}

	res = usb_get_string(USB_STRING_LANGID, 0, head, 2, CCID_WAIT_SLOW);
	CUNIT_ASSERT_EQUALS(int, res, 2);
	CUNIT_ASSERT_TRUE(head[1] == LIBUSB_DT_STRING);

	len = head[0];

	g_lang_num = (len-2)/2;
	g_langids = malloc(g_lang_num * sizeof(uint16_t));
	CUNIT_ASSERT_TRUE(g_langids != NULL);

	buffer = malloc(len);
	buffer_len = len;
	CUNIT_ASSERT_TRUE(buffer != NULL);

	res = usb_get_string(USB_STRING_LANGID, 0,
			     buffer, (uint16_t)len, CCID_WAIT_SLOW);
	CUNIT_ASSERT_EQUALS(int, res, len);
	for (i = 0; i < g_lang_num; i++) {
		g_langids[i] = w2i((const uint8_t *)buffer+2+i*2);
		log_msg(LOG_DEBUG, "  LanguageID          %d", g_langids[i]);
	}
	free(buffer);

	usb_dump_string("  iManufacturer       ", g_dev.iManufacturer);
	usb_dump_string("  iProduct            ", g_dev.iProduct);
	usb_dump_string("  iSerialNumber       ", g_dev.iSerialNumber);
	usb_dump_string("  iConfiguration      ", g_conf.iConfiguration);
	usb_dump_string("  iInterface          ", g_intf.iInterface);
}

void CUNITCBK get_datarate(void)
{
	char *data_rates = NULL;
	int res;
	uint16_t length;
	uint16_t i;

	if (!g_desc.bNumDataRatesSupported)
		return;

	length = g_desc.bNumDataRatesSupported * 4;
	if (!g_data_rates) {
		g_data_rates = malloc(g_desc.bNumDataRatesSupported *
				      sizeof (*g_data_rates));
	}
	CUNIT_ASSERT_TRUE(g_data_rates != NULL);

	data_rates = malloc(length);
	CUNIT_ASSERT_TRUE(data_rates != NULL);
	res = ccid_ctrl_cls(LIBUSB_ENDPOINT_IN,
			    CCID_REQ_GET_DATA_RATES,
			    0,
			    (uint8_t *)data_rates,
			    (uint16_t)(length));
	CUNIT_ASSERT_EQUALS(int, res, length);

	for (i = 0; i < g_desc.bNumDataRatesSupported; i++) {
		g_data_rates[i] = dw2i((const uint8_t *)data_rates + (i*4));
		log_msg(LOG_DEBUG, "data_rates[%d]=%d", i, g_data_rates[i]);
	}
}


void CUNITCBK get_clockfreq(void)
{
	char *clock_freqs = NULL;
	uint16_t length;
	uint16_t i;
	int res;

	if (!g_desc.bNumClockSupported)
		return;

	length = g_desc.bNumClockSupported * 4;
	if (!g_clock_freqs) {
		g_clock_freqs = malloc(g_desc.bNumClockSupported *
				       sizeof (*g_clock_freqs));
	}
	CUNIT_ASSERT_TRUE(g_clock_freqs != NULL);

	clock_freqs = malloc(length);
	CUNIT_ASSERT_TRUE(clock_freqs != NULL);
	res = ccid_ctrl_cls(LIBUSB_ENDPOINT_IN,
			    CCID_REQ_GET_CLOCK_FREQS,
			    0,
			    (uint8_t *)clock_freqs,
			    (uint16_t)(length));
	CUNIT_ASSERT_EQUALS(int, res, length);

	for (i = 0; i < g_desc.bNumClockSupported; i++) {
		g_clock_freqs[i] = dw2i((const uint8_t *)clock_freqs + (i*4));
		log_msg(LOG_DEBUG, "clock_freqs[%d]=%d", i, g_clock_freqs[i]);
	}
}

void CUNITCBK ccid_set_fd(void)
{
	if (!g_atr_ready)
		return;

	/* if IFD auto change FREQ or DATA we can only retrive them */
	if ((g_desc.dwFeatures & SCD_FEATURE_AUTO_FREQ) ||
	    (g_desc.dwFeatures & SCD_FEATURE_AUTO_DATA))
		return;
	/* not necessary change if only 1 */
	if (g_desc.bNumClockSupported == 1 &&
	    g_desc.bNumDataRatesSupported == 1)
		return;

	if (!g_clock_freqs || !g_data_rates)
		return;
}

#define CCID_INTR_MSG_MIN	0x04
#define __ALIGN_MASK(x, mask)	(((x)+(mask))&~(mask))
#define ROUNDUP_BYTE(x)		__ALIGN_MASK(x, 8-1)

int scd_change_length(void)
{
	int len = 2 * (g_desc.bMaxSlotIndex+1);
	len = ROUNDUP_BYTE(len) / 8;
	return len+1;
}

int ccid_intrrupt_length(void)
{
	int len = 2 * (g_desc.bMaxSlotIndex+1);

	len = ROUNDUP_BYTE(len) / 8;
	return max(len+1, CCID_INTR_MSG_MIN);
}

#define BITS_PER_BYTE		8
#define BITOP_MASK(nr)		((uint8_t)1 << ((nr) % BITS_PER_BYTE))
#define BITOP_BYTE(nr)		((uint8_t)(nr) / (uint8_t)BITS_PER_BYTE)
typedef uint8_t bits_t;

boolean test_bit(int nr, const bits_t *addr)
{
	return 1 & (addr[BITOP_BYTE(nr)] >> (nr & (BITS_PER_BYTE-1)));
}

boolean ccid_interrupt_state(uint8_t *changes, uint8_t sid)
{
	return test_bit(sid*2, changes);
}

void CUNITCBK ccid_interrupt(void)
{
	uint8_t *buf;
	int buf_size = ccid_intrrupt_length(), len = buf_size;
	boolean want_state = false, curr_state = false;
	int nr_changes = 0;

	/* case specific warm up */
	ccid_set_timeout(CCID_WAIT_FAST);
	while (len > 0) {
		buf = malloc(buf_size);
		CUNIT_ASSERT_TRUE(buf != NULL);
		len = ccid_intr_in(buf, buf_size);
		free(buf);
	}
	ccid_set_timeout(CCID_WAIT_SLOW);

	log_msg(LOG_INFO, "========================================");
	log_msg(LOG_INFO, "ACTION REQUIRED: remove/insert card once");
	log_msg(LOG_INFO, "========================================");

	while (nr_changes < 2 && !curr_state) {
		buf = malloc(buf_size);
		CUNIT_ASSERT_TRUE(buf != NULL);
		len = ccid_intr_in(buf, buf_size);

		switch (buf[0]) {
		case SCD_RDR2PC_NOTIFYSLOTCHANGE:
			curr_state = ccid_interrupt_state(buf+1, CCID_SLOT_NO);
			free(buf);
			CUNIT_ASSERT_TRUE(want_state == curr_state);
			want_state = !curr_state;
			nr_changes++;
			break;
		default:
			free(buf);
			break;
		}
	}

	log_msg(LOG_DEBUG, "interrupt len=%d", len);
}

CUNIT_BEGIN_SUITE(ccid_atr)
	CUNIT_INCLUDE_CASE(ccid_warmup)
	CUNIT_INCLUDE_CASE(ccid_slot_status)
	CUNIT_INCLUDE_CASE(ccid_power_on)
	CUNIT_INCLUDE_CASE(ccid_teardown)
CUNIT_END_SUITE

CUNIT_BEGIN_SUITE(ccid_pps)
	CUNIT_INCLUDE_CASE(ccid_warmup)
	CUNIT_INCLUDE_CASE(get_datarate)
	CUNIT_INCLUDE_CASE(get_clockfreq)
	CUNIT_INCLUDE_CASE(ccid_power_on)
	CUNIT_INCLUDE_CASE(ccid_set_param)
	CUNIT_INCLUDE_CASE(ccid_set_fd)
	CUNIT_INCLUDE_CASE(ccid_get_param)
	CUNIT_INCLUDE_CASE(ccid_reset_param)
	CUNIT_INCLUDE_CASE(ccid_teardown)
CUNIT_END_SUITE

CUNIT_BEGIN_SUITE(ccid_t0)
	CUNIT_INCLUDE_CASE(ccid_warmup)
	CUNIT_INCLUDE_CASE(ccid_power_on)
	CUNIT_INCLUDE_CASE(ccid_set_param)
	CUNIT_INCLUDE_CASE(ccid_t0_xfr_block)
	CUNIT_INCLUDE_CASE(ccid_teardown)
CUNIT_END_SUITE

CUNIT_BEGIN_SUITE(ccid_t1)
	CUNIT_INCLUDE_CASE(ccid_warmup)
	CUNIT_INCLUDE_CASE(ccid_power_on)
	CUNIT_INCLUDE_CASE(ccid_set_param)
	CUNIT_INCLUDE_CASE(ccid_t1_xfr_block)
	CUNIT_INCLUDE_CASE(ccid_teardown)
CUNIT_END_SUITE

CUNIT_BEGIN_SUITE(ccid_xfr)
	CUNIT_INCLUDE_SUITE(ccid_t0)
	CUNIT_INCLUDE_SUITE(ccid_t1)
CUNIT_END_SUITE

CUNIT_BEGIN_SUITE(ccid_verify)
	CUNIT_INCLUDE_CASE(ccid_warmup)
	CUNIT_INCLUDE_CASE(ccid_power_on)
	CUNIT_INCLUDE_CASE(ccid_set_param)
	CUNIT_INCLUDE_CASE(ccid_pin_verify)
	CUNIT_INCLUDE_CASE(ccid_teardown)
CUNIT_END_SUITE

CUNIT_BEGIN_SUITE(ccid_modify)
	CUNIT_INCLUDE_CASE(ccid_warmup)
	CUNIT_INCLUDE_CASE(ccid_power_on)
	CUNIT_INCLUDE_CASE(ccid_set_param)
	CUNIT_INCLUDE_CASE(ccid_pin_modify)
	CUNIT_INCLUDE_CASE(ccid_teardown)
CUNIT_END_SUITE

CUNIT_BEGIN_SUITE(ccid_spe)
	CUNIT_INCLUDE_SUITE(ccid_verify)
	CUNIT_INCLUDE_SUITE(ccid_modify)
CUNIT_END_SUITE

CUNIT_BEGIN_SUITE(ccid_misc)
	CUNIT_INCLUDE_CASE(ccid_warmup)
	CUNIT_INCLUDE_CASE(ccid_power_on)
	CUNIT_INCLUDE_CASE(ccid_icc_clock)
	CUNIT_INCLUDE_CASE(ccid_icc_mecha)
	CUNIT_INCLUDE_CASE(ccid_teardown)
CUNIT_END_SUITE

CUNIT_BEGIN_SUITE(ccid_busy)
	CUNIT_INCLUDE_CASE(ccid_warmup)
	CUNIT_INCLUDE_CASE(ccid_slot_busy)
	CUNIT_INCLUDE_CASE(ccid_teardown)
CUNIT_END_SUITE

CUNIT_BEGIN_SUITE(ccid_abort)
	CUNIT_INCLUDE_CASE(ccid_warmup)
	CUNIT_INCLUDE_CASE(ccid_power_on)
	CUNIT_INCLUDE_CASE(ccid_set_param)
	CUNIT_INCLUDE_CASE(ccid_slot_abort)
	CUNIT_INCLUDE_CASE(ccid_teardown)
CUNIT_END_SUITE

CUNIT_BEGIN_SUITE(ccid_async)
	CUNIT_INCLUDE_SUITE(ccid_busy)
	CUNIT_INCLUDE_SUITE(ccid_abort)
CUNIT_END_SUITE

CUNIT_BEGIN_SUITE(ccid_intr)
	CUNIT_INCLUDE_CASE(ccid_warmup)
	CUNIT_INCLUDE_CASE(ccid_interrupt)
	CUNIT_INCLUDE_CASE(ccid_teardown)
CUNIT_END_SUITE

CUNIT_BEGIN_SUITE(ccid_ctrl)
	CUNIT_INCLUDE_CASE(ccid_warmup)
	CUNIT_INCLUDE_CASE(get_string)
	CUNIT_INCLUDE_CASE(get_clockfreq)
	CUNIT_INCLUDE_CASE(get_datarate)
	CUNIT_INCLUDE_CASE(ccid_teardown)
CUNIT_END_SUITE

CUNIT_BEGIN_SUITE(ccid_suite)
//	CUNIT_INCLUDE_SUITE(ccid_intr)
#if 0
	CUNIT_INCLUDE_SUITE(ccid_ctrl)
	CUNIT_INCLUDE_SUITE(ccid_atr)
	CUNIT_INCLUDE_SUITE(ccid_pps)
	CUNIT_INCLUDE_SUITE(ccid_xfr)
	CUNIT_INCLUDE_SUITE(ccid_spe)
#endif
	CUNIT_INCLUDE_SUITE(ccid_async)
//	CUNIT_INCLUDE_SUITE(ccid_misc)
CUNIT_END_SUITE

void ccid_run_suite(void)
{
	ccid_get_device(&g_dev);
	ccid_get_config(&g_conf);
	ccid_get_interface(&g_intf);
	ccid_get_descriptor(&g_desc);
	CUNIT_RUN_SUITE(ccid_suite);
}
