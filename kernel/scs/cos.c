#include <target/bitops.h>
#include <target/cos.h>

#define PPS_PPSS		0xFF
#define COS_RESERVE_FID		0x0000

struct cos_state {
	scs_err_t error;
	cos_sw_t sw;
	boolean activated;
};

#if NR_MAX_CHANNELS > 1
struct cos_chan cos_chan[NR_MAX_CHANNELS];
DECLARE_BITMAP(cos_cstate, NR_MAX_CHANNELS);
uint8_t cos_cid = COS_DEF_CID;
#define cos_curr_chan		cos_chan[cos_cid]
#else
struct cos_chan cos_curr_chan;
#define cos_cid			COS_DEF_CID
#define cos_curr_cstate		COS_CHAN_STATE_OPEND
#endif

struct cos_xfr		cos_xfr;
struct cos_apdu_hdr	cos_apdu;
uint8_t			cos_buf[COS_APDU_SIZE];

struct cos_state		cos_intfc;
uint8_t data_uint_size;
uint8_t data_coding_byte;
scs_cmpl_cb cos_complete = NULL;

static void cos_chan_set_curr(cos_fid_t fid);

void cos_set_sw(cos_sw_t err);

#ifdef CONFIG_ICC_SCD
static void cos_scd_seq_complete(icc_err_t err);
#else
#define cos_scd_set_state(state)
#define cos_scd_init()
#define cos_scd_seq_complete(err)
#endif

#define __cos_write_byte(index, b)	(cos_buf[index] = b)
#define __cos_read_byte(index)		(cos_buf[index])

/* absolute (MF) or relative (current DF) */
scs_err_t cos_fs_select_by_path(uint8_t type)
{
	cos_set_sw(COS_SW_PARAM_ERR);
	return SCS_ERR_SUCCESS;
}

scs_err_t cos_fs_select_by_name(void)
{
	/* Loop all DFs and check the name whether match or not.
	 * If true then select it.
	 */
	cos_set_sw(COS_SW_PARAM_ERR);
	return SCS_ERR_SUCCESS;
}

/* FID or SFID */
scs_err_t cos_fs_select_by_fid(uint8_t type)
{
	/* Loop all files ((depends upon the type)) and check the FID or SFID 
	 * whether match or not. If true then select it. Otherwise report the 
	 * error. 
	 */
	cos_set_sw(COS_SW_PARAM_ERR);
	return SCS_ERR_SUCCESS;
}

/*=========================================================================
 * ISO7816-4 Organization
 *=======================================================================*/
#define __bit_mask(x)	(1 << (x-1))
#define __bits_mask(x)	(__bit_mask(x) - 1)

struct cos_btlv_tag cos_btlv_tag;
struct cos_btlv_length cos_btlv_length;
struct cos_btlv_value cos_btlv_value;

#define cos_btlv_ctag		cos_btlv_tag.tag
#define cos_btlv_ctag_first	cos_btlv_tag.first
#define cos_btlv_clength	cos_btlv_tag.length
#define cos_btlv_cvalue		cos_btlv_value.value

#define P1			cos_apdu.p1
#define P2			cos_apdu.p2
#define P3			cos_apdu.p3
#define INS			cos_apdu.ins
#define CLA			cos_apdu.cla
#define COS_APDU_CLA_CLASS_MSK	0x80

#define cos_is_even_ins()	((INS & __bit_mask(1)) == 0)
#define cos_is_odd_ins()	((INS & __bit_mask(1)) == 1)
#define COS_SFID_BIT_MASK	0x1F

static void cos_parse_header(void)
{
	cos_apdu.cla = __cos_read_byte(0);
	cos_apdu.ins = __cos_read_byte(1);
	cos_apdu.p1  = __cos_read_byte(2);
	cos_apdu.p2  = __cos_read_byte(3);
	cos_apdu.p3  = __cos_read_byte(4);
}

static boolean cos_apdu_is_prop(void)
{
	return (CLA & COS_APDU_CLA_CLASS_MSK);
}

uint8_t cos_btlv_tag_length(uint8_t *field)
{
	uint8_t len;
	uint8_t first = *field;
	if ((first & __bits_mask(5)) != __bits_mask(5))
		return 1;
	len = 2;
	field++;
	while (1) {
		if (((*field++) & __bit_mask(8)) == 0)
			break;
		len++;
	}
	return len;
}

uint8_t cos_btlv_length_length(uint8_t *field)
{
	uint8_t first = *field;

	if (first <= 0x7F)
		return 1;
	return (first & __bits_mask(5)) + 1;
}

static void cos_btlv_t(uint8_t *t)
{
	uint8_t len = cos_btlv_tag_length(t);
	uint8_t first = *t;
	uint8_t d;

	BUG_ON (len > 3);

	cos_btlv_tag.first = *t;
	cos_btlv_tag.flen = len;
	cos_btlv_tag.tag = 0;
	if (len == 1) {
		cos_btlv_tag.tag = first & __bits_mask(5);
	} else {
		d = (*(t+2)) & __bits_mask(7);
		cos_btlv_tag.tag |= d;
		d = (*(t+1)) & __bits_mask(7);
		cos_btlv_tag.tag |= ((uint16_t)(d) << 7);
	}
}

static void cos_btlv_l(uint8_t *l)
{
	uint8_t len = cos_btlv_length_length(l);
	uint8_t i;

	BUG_ON (len > 5);

	cos_btlv_length.flen = len;
	cos_btlv_length.length = 0;
	if (len == 1) {
		cos_btlv_length.length = *l;
	} else {
		i = 1;
		while (i++ != len) {
			cos_btlv_length.length |= (*(l+i)) << (i<<3);
		}
	}
}

static void cos_btlv_v(uint8_t *v)
{
	/* value may be a TLV */
	cos_btlv_value.value = v;
}

static uint8_t cos_btlv_parse(uint8_t *tlv)
{
	cos_btlv_t(tlv);
	cos_btlv_l(tlv);
	cos_btlv_v(tlv);

	return cos_btlv_tag.flen + cos_btlv_length.flen + cos_btlv_length.length;
}

/* loop all TLVs in command data field.
 * start from the 1st TLV, if found, stop at there.
 */
static boolean cos_find_cmd_tag_all(uint16_t tag)
{
	/* the 1st TLV object */
	uint8_t *tlv = cos_apdu.hdr_data;
	uint8_t total_len = cos_apdu.p3;
	uint8_t tlv_len;

	while (total_len) {
		tlv_len = cos_btlv_parse(tlv);
		if (tag == cos_btlv_tag.tag)
			return true;
		total_len -= tlv_len;
		tlv += tlv_len;
	}
	return false;
}

#if NR_MAX_CHANNELS > 1
static void cos_chan_active(boolean on)
{
	if (on)
		cos_curr_chan.actived = true;
	else
		cos_curr_chan.actived = false;
}

static void cos_chan_open(boolean on)
{
	if (on)
		cos_curr_chan.opened = true;
	else
		cos_curr_chan.opened = false;
}

static void cos_chan_switch(uint8_t cla)
{
	uint8_t cid = cla2cid(cla);

	if (cid == cos_cid)
		return;
	cos_chan_deactive(false);
	cos_cid = cid;
	cos_chan_open(true);
	cos_chan_deactive(true);
}
#else
#define cos_chan_switch(cla)
#endif

static void cos_chan_set_curr(cos_fid_t fid)
{
	cos_curr_chan.pfid = cos_curr_chan.cfid;
	cos_curr_chan.cfid = fid;
}

static cos_fid_t cos_chan_cfid(void)
{
	return cos_curr_chan.cfid;
}

#define __cos_select_fid(fid)	SCS_ERR_SUCCESS
scs_err_t cos_chan_switch_file(uint16_t fid)
{
	scs_err_t err = SCS_ERR_SUCCESS;

	if (cos_chan_cfid() != fid)
		err = __cos_select_fid(fid);
	return err;
}

/* TODO: retrieve data unit from object. */
scs_err_t cos_decap_data(void)
{
	return SCS_ERR_SUCCESS;
}

/* TODO: encapsulate data unit to object. */
scs_err_t cos_encap_data(void)
{
	return SCS_ERR_SUCCESS;
}

/*=========================================================================
 * ISO7816-4 (chapter 7) Commands for interchange
 *=======================================================================*/
#define cos_get_file(fid)	NULL

static boolean cos_dtype_match(uint8_t dtype)
{
	struct cos_file *file = cos_get_file(cos_chan_cfid());

	if (dtype == COS_DATA_TYPE_UNIT || dtype == COS_DATA_TYPE_RECORD)
		return (file->ftype == ICC_FILE_TYPE_EF) && (file->dtype == dtype);
	return file->dtype == dtype;
}

#define cos_security_check()	false
/* Command(s) sanity check.
 * - check data type.
 * - check security status.
 */
static scs_err_t cos_cmd_sanity_check(uint8_t dtype)
{
	if (COS_DATA_TYPE_MASK != dtype &&
	    !cos_dtype_match(dtype)) {
		cos_set_sw(COS_SW_CMD_INCOMP);
		return SCS_ERR_SANITY;
	}
	if (!cos_security_check()) {
		cos_set_sw(COS_SW_SECU_NOT_SATISFY);
		return SCS_ERR_SANITY;
	}
	return SCS_ERR_SUCCESS;
}

/* Card Management (7816-9) */
scs_err_t cos_cmds_management(void)
{
	cos_set_sw(COS_SW_UNSUPP_INS);
	return SCS_ERR_SUCCESS;
}

#define cos_fs_curr_fid()	0x0000

/* Selection (7816-4) */
scs_err_t cos_cmd_select(void)
{
	scs_err_t err;

	if (P1 & __bit_mask(4)) {
		err = cos_fs_select_by_path(P1 & __bit_mask(1));
	} else if (P1 & __bit_mask(3)) {
		err = cos_fs_select_by_name();
	} else {
		err = cos_fs_select_by_fid(P1 & __bits_mask(3));
	}
	if (err == SCS_ERR_PROGRESS || err != SCS_ERR_SUCCESS)
		return err;
	
	cos_chan_switch(CLA);
#if 0
	cos_chan_set_curr((cos_fid_t)cos_fs_curr_fid());
#endif
	return SCS_ERR_SUCCESS;
}

scs_err_t cos_cmds_selection(void)
{
	scs_err_t err = cos_cmd_sanity_check(COS_DATA_TYPE_MASK);
	if (err != SCS_ERR_SUCCESS)
		return err;

	if (cos_apdu.ins == COS_INS_SELECT)
		return cos_cmd_select();

	cos_set_sw(COS_SW_UNSUPP_INS);
	return SCS_ERR_SUCCESS;
}

#define cos_read_bin(offset)	SCS_ERR_SUCCESS
#define cos_write_bin(offset)	SCS_ERR_SUCCESS
#define cos_update_bin(offset)	SCS_ERR_SUCCESS
#define cos_search_bin(offset)	SCS_ERR_SUCCESS
#define cos_erase_bin(offset)	SCS_ERR_SUCCESS

/* Data unit handling (7816-4) */
/* TODO: adjust data_uint_size first? */
scs_err_t cos_cmds_dataunit(void)
{
	uint16_t offset;
	uint16_t fid = COS_RESERVE_FID;
	scs_err_t err = cos_cmd_sanity_check(COS_DATA_TYPE_UNIT);

	if (err != SCS_ERR_SUCCESS)
		return err;

	/* obtain fid and offset */
	if (cos_is_even_ins()) {
		if (P1 & __bit_mask(8)) {
			fid = P1 & COS_SFID_BIT_MASK;
			offset = P2;
		} else {
			offset = (P1 << 8) | P2;
		}
	} else {
#ifdef CONFIG_COS_DATAUNIT_BTLV
		BUG_ON (!cos_find_cmd_tag_all(0x54));
		offset = *(uint16_t *)cos_btlv_cvalue;

		if ((P1 == 0 && (P2 & ~COS_SFID_BIT_MASK) == 0) &&
		    (((P2 & COS_SFID_BIT_MASK) != 0) &&
		      (P2 & COS_SFID_BIT_MASK) != COS_SFID_BIT_MASK)) {
			fid = P2 & COS_SFID_BIT_MASK;
		} else {
			fid = (P1 << 8) | P2;
		}
#else
		BUG();
#endif
	}

	if (cos_is_odd_ins())
		cos_decap_data();

	if (INS == COS_INS_READ_BINARY0 || INS == COS_INS_READ_BINARY1)
		err = cos_read_bin(offset);
	if (INS == COS_INS_WRITE_BINARY0 || INS == COS_INS_WRITE_BINARY1)
		err = cos_write_bin(offset);
	if (INS == COS_INS_UPDATE_BINARY0 || INS == COS_INS_UPDATE_BINARY1)
		err = cos_update_bin(offset);
	if (INS == COS_INS_SEARCH_BINARY0 || INS == COS_INS_SEARCH_BINARY1)
		err = cos_search_bin(offset);
	if (INS == COS_INS_ERASE_BINARY0 || INS == COS_INS_ERASE_BINARY1)
		err = cos_erase_bin(offset);

	if (cos_is_odd_ins())
		cos_encap_data();

	/* XXX: Assume current always exist.
	 * Some groups say we should switch current after completed if any fid present.
	 * And if failure we should return memory-changed error.
	 * why do not put it at the head?
	 * */
	if (fid != COS_RESERVE_FID)
		err = cos_chan_switch_file(fid);

	return err;
}

#define cos_read_record(P1, P2)		SCS_ERR_SUCCESS
#define cos_write_record(P1, P2)	SCS_ERR_SUCCESS
#define cos_update_record(P1, P2)	SCS_ERR_SUCCESS
#define cos_append_record(P1, P2)	SCS_ERR_SUCCESS
#define cos_search_record(P1, P2)	SCS_ERR_SUCCESS
#define cos_erase_record(P1, P2)	SCS_ERR_SUCCESS

/* Record handling (7816-4) */
scs_err_t cos_cmds_record(void)
{
	scs_err_t err;
	uint16_t fid = COS_RESERVE_FID;

	err = cos_cmd_sanity_check(COS_DATA_TYPE_RECORD);
	if (err != SCS_ERR_SUCCESS)
		return err;
	
	/* record number or identifier */
	if (P1 == 0x00 || P1 == 0xFF) {
		cos_set_sw(COS_SW_PARAM_ERR);
		return SCS_ERR_SUCCESS;
	}

	if (cos_is_odd_ins())
		cos_decap_data();

	if (INS == COS_INS_READ_RECORD0 || INS == COS_INS_READ_RECORD1)
		err = cos_read_record(P1, P2);
	if (INS == COS_INS_WRITE_RECORD)
		err = cos_write_record(P1, P2);
	if (INS == COS_INS_UPDATE_RECORD0 || INS == COS_INS_UPDATE_RECORD1)
		err = cos_update_record(P1, P2);
	if (INS == COS_INS_APPEND_RECORD)
		err = cos_append_record(P1, P2);
	if (INS == COS_INS_SEARCH_RECORD)
		err = cos_search_record(P1, P2);
	if (INS == COS_INS_ERASE_RECORDS)
		err = cos_erase_record(P1, P2);

	if (cos_is_odd_ins())
		cos_encap_data();

	fid = (P2 & ~__bits_mask(3)) >> 3;
	if (fid != COS_RESERVE_FID)
		err = cos_chan_switch_file(fid);
	
	return err;
}

/* Data object handling (7816-4) */
scs_err_t cos_cmd_dataobj(void)
{
	cos_set_sw(COS_SW_UNSUPP_INS);
	return SCS_ERR_SUCCESS;
}

/* Basic security handling (7816-4) */
scs_err_t cos_cmds_base(void)
{
	cos_set_sw(COS_SW_UNSUPP_INS);
	return SCS_ERR_SUCCESS;
}

/* Transmission handling (7816-4) */
scs_err_t cos_cmds_trans(void)
{
	cos_set_sw(COS_SW_UNSUPP_INS);
	return SCS_ERR_SUCCESS;
}

/*=========================================================================
 * ISO7816-4 Organization
 *=======================================================================*/
void cos_set_sw(cos_sw_t err)
{
	/* Only allow set error once. The initial value of SW is 0x9000. */
	if (HIBYTE(err) == 0x90)
		cos_intfc.sw = err;
	else
		BUG(/* TODO: check the logic */);
}

void cos_reset_sw(void)
{
	cos_set_sw(COS_SW_NORMAL);
}

cos_sw_t cos_get_sw(void)
{
	return cos_intfc.sw;
}

/*=========================================================================
 * xfr
 *=======================================================================*/
void cos_set_error(scs_err_t err)
{
	cos_intfc.error = err;
}

scs_err_t cos_get_error(void)
{
	return cos_intfc.error;
}

void cos_xchg_reset(scs_size_t tx)
{
	cos_xfr.rx = 0;
	cos_xfr.tx = tx;
}

/* called after having received whole apdu command */
static scs_err_t cos_handle_apdu(void)
{
	scs_err_t err = SCS_ERR_SUCCESS;

	cos_write_resp(0x90);
	cos_write_resp(0x00);
	return err;

#if 0
	switch (INS) {
	case COS_INS_CREATE_FILE:
		err = cos_cmds_management();
		break;
	case COS_INS_SELECT:
	case COS_INS_MANAGE_CHANNEL:
		err = cos_cmds_selection();
		break;
	case COS_INS_READ_BINARY0: case COS_INS_READ_BINARY1:
	case COS_INS_WRITE_BINARY0: case COS_INS_WRITE_BINARY1:
	case COS_INS_UPDATE_BINARY0: case COS_INS_UPDATE_BINARY1:
	case COS_INS_SEARCH_BINARY0: case COS_INS_SEARCH_BINARY1:
	case COS_INS_ERASE_BINARY0: case COS_INS_ERASE_BINARY1:
		err = cos_cmds_dataunit();
		break;
	case COS_INS_READ_RECORD0: case COS_INS_READ_RECORD1:
	case COS_INS_WRITE_RECORD:
		err = cos_cmds_record();
		break;
	case COS_INS_GET_DATA0: case COS_INS_GET_DATA1:
		err = cos_cmd_dataobj();
		break;
	case COS_INS_INTERNAL_AUTH:
	case COS_INS_GET_CHALLENGE:
	case COS_INS_EXTERNAL_AUTH:
		err = cos_cmds_base();
		break;
	case COS_INS_GET_RESPONSE:
	case COS_INS_ENVELOPE0:
	case COS_INS_ENVELOPE1:
		err = cos_cmds_trans();
		break;
	default:
		BUG();
		break;
	}
#endif
}

static void cos_resp_sw(void)
{
	uint8_t sw1, sw2;

	sw1 = HIBYTE(cos_get_sw());
	sw2 = LOBYTE(cos_get_sw());

	if (sw1 >= 0x64 && sw1 <= 0x6F)
		/* TODO: data field should be absend */;
	cos_write_resp(sw1);
	cos_write_resp(sw2);
	cos_complete();
}

void cos_write_resp(uint8_t byte)
{
	if (cos_xfr.rx < COS_APDU_SIZE) {
		__cos_write_byte(cos_xfr.rx, byte);
		cos_xfr.rx++;
	}
}

scs_err_t cos_xchg_write(scs_off_t index, uint8_t byte)
{
	if (!cos_intfc.activated)
		return SCS_ERR_NOTPRESENT;
	if (index >= COS_APDU_SIZE)
		return SCS_ERR_OVERRUN;
	__cos_write_byte(index, byte);
	return SCS_ERR_SUCCESS;
}

uint8_t cos_xchg_read(scs_off_t index)
{
#if 0
	if (!cos_intfc.activated)
		return SCS_ERR_NOTPRESENT;
#endif
	return __cos_read_byte(index);
}

scs_size_t cos_xchg_avail(void)
{
	return cos_xfr.rx;
}

scs_err_t cos_xchg_block(scs_size_t nc, scs_size_t ne)
{
	if (!cos_intfc.activated)
		return SCS_ERR_NOTPRESENT;
	if (!nc)
		return SCS_ERR_OVERRUN;
	cos_xchg_reset(nc);
	cos_parse_header();
	return cos_handle_apdu();
}

scs_err_t cos_power_on(void)
{
	scs_err_t err;

	cos_xchg_reset(0);
	cos_write_resp(0x3B);
	/* cos_fsid = fs_mount(cos_fs_path, cos_fs_type); */
	err = cos_vs_activate();
	BUG_ON(err == SCS_ERR_PROGRESS);
	if (err == SCS_ERR_SUCCESS)
		cos_intfc.activated = true;
	/*
	else
		fs_umount(cos_fsid);
	*/
	return err;
}

scs_err_t cos_power_off(void)
{
	if (cos_intfc.activated) {
		cos_xchg_reset(0);
		cos_vs_deactivate();
		/* fs_umount(cos_fsid); */
		cos_intfc.activated = false;
	}
	return SCS_ERR_SUCCESS;
}

void cos_register_handlers(scs_cmpl_cb completion)
{
	cos_complete = completion;
}

/*=========================================================================
 * SCD_COS
 *=======================================================================*/
#ifdef CONFIG_ICC_SCD

scd_t cos_scds[NR_MAX_ICC];

static scs_err_t cos_scd_err(icc_err_t err)
{
	switch (err) {
	case SCS_ERR_SUCCESS:
	case SCS_ERR_PROGRESS:
		return err;
	default:
		return SCS_ERR_HW_ERROR;
	}
}

uint8_t cos_scd_state(uint8_t state)
{
	if (state < COS_STATE_POWERED)
		return ICC_STATE_PRESENT;
	return ICC_STATE_ACTIVE;
}

static scd_t cos_scd_dev(icc_t id)
{
	return cos_scds[id];
}

static icc_t icc_scd2id(scd_t scd)
{
	icc_t id;

	for (id = 0; id < NR_MAX_ICC; id++) {
		if (scd == icc_scds[id])
			return id;
	}
	BUG();
	return INVALID_ICC_ID;
}

#define icc_id_select()

static void cos_scd_select(void)
{
	icc_id_select(icc_scd2id(scd_id));
}

static scs_err_t cos_scd_activate(void)
{
	scs_err_t err;
	if (icc_id == INVALID_ICC_ID)
		return SCS_ERR_HW_ERROR;
	err = cos_power_on();
	return cos_scd_err(err);
}

static scs_err_t cos_scd_deactivate(void)
{
	if (icc_id == INVALID_ICC_ID)
		return SCS_ERR_HW_ERROR;
	return SCS_ERR_SUCCESS;
}

static scs_err_t cos_scd_xchg_block(scs_size_t nc, scs_size_t ne)
{
	if (icc_id == INVALID_ICC_ID)
		return SCS_ERR_HW_ERROR;
	return SCS_ERR_SUCCESS;
}

static scs_size_t cos_scd_xchg_avail(void)
{
	return cos_xchg_avail();
}

static scs_err_t cos_scd_xchg_write(scs_off_t index, uint8_t byte)
{
	if (icc_id == INVALID_ICC_ID)
		return SCS_ERR_HW_ERROR;

	cos_xchg_write(index, byte);
	return SCS_ERR_SUCCESS;
}

static uint8_t cos_scd_xchg_read(scs_off_t index)
{
	if (icc_id == INVALID_ICC_ID)
		return 0x00;

	return cos_xchg_read(index);
}

icc_driver_t cos_scd = {
	cos_scd_select,
	cos_scd_activate,
	cos_scd_deactivate,
	cos_scd_xchg_block,
	cos_scd_xchg_avail,
	cos_scd_xchg_write,
	cos_scd_xchg_read,
};

static void cos_scd_seq_complete(icc_err_t err)
{
	scd_dev_select(cos_scd_dev(ifd_slid));
	scd_seq_complete(cos_scd_err(err));
}

static void cos_scd_set_state(uint8_t state)
{
	scd_dev_select(cos_scd_dev(icc_id));
	scd_dev_set_state(cos_scd_state(state));
}

static void cos_scd_init(void)
{
	icc_t id;

	for (id = 0; id < NR_MAX_ICC; id++) {
		cos_scds[id] = scd_register_device(&cos_scd);
	}
}
#endif

void cos_init(void)
{
	DEVICE_FUNC(DEVICE_FUNC_COS);
	cos_scd_init();
	cos_vs_init();
}
