#define CCID_SPE_DEFAULT_MSGID	0xff

#define CCID_SPE_PIN_VERIFY	0x00
#define CCID_SPE_PIN_MODIFY	0x01
#define CCID_SPE_PIN_CANCEL	0x02
#define CCID_SPE_RESEND_IBLOCK	0x03
#define CCID_SPE_SEND_NEXTAPDU	0x04

#define CCID_SPE_MAX_PIN	16

#define CCID_SPE_VALID_MAXSIZE	0x01
#define CCID_SPE_VALID_KEYPRESS	0x02
#define CCID_SPE_VALID_TIMEOUT	0x04

#define CCID_SPE_FORMAT_MASK	0x03
#define CCID_SPE_JUSTIFY_MASK	0x04
#define CCID_SPE_PIN_POS_OFFSET	0x03
#define CCID_SPE_PIN_POS_MASK	(0x0F<<CCID_SPE_PIN_POS_OFFSET)
#define CCID_SPE_PIN_UNIT_MASK	0x80
#define CCID_SPE_LEN_POS_MASK	0x0F
#define CCID_SPE_LEN_UNIT_MASK	0x08

#define CCID_SPE_FORMAT_BIN	0x00
#define CCID_SPE_FORMAT_BCD	0x01
#define CCID_SPE_FORMAT_ASCII	0x02

#define CCID_SPE_JUSTIFY()		\
	(ccid_cmd_data.po.bmFormatString&CCID_SPE_JUSTIFY_MASK)
#define CCID_SPE_PIN_UNIT()		\
	(ccid_cmd_data.po.bmFormatString&CCID_SPE_PIN_UNIT_MASK)
#define CCID_SPE_LEN_UNIT()		\
	(ccid_cmd_data.po.bmPINLengthFormat&CCID_SPE_LEN_UNIT_MASK)

#define CCID_SPE_APDU_FIXED		5

typedef void (*ccid_spe_cmpl_cb)(void);

struct ccid_spe {
	ccid_spe_cmpl_cb entry_cmpl;
	uint8_t nr_next;
	uint8_t nr_last;
	boolean aborted;
	uint8_t flags;
	uint8_t pin_offset;
	ifd_sid_t sid;
};

__near__ kbd_event_cb ccid_spe_kh = NULL;
__near__ struct ccid_spe ccid_spe_ctrl;

#define CCID_SPE_STATE_MASK		0x03
#define CCID_SPE_STATE_OLDPIN		0x00
#define CCID_SPE_STATE_NEWPIN		0x01
#define CCID_SPE_STATE_CONFIRM		0x02

#define CCID_SPE_FLAGS_MASK		0xF0
#define CCID_SPE_BAD_CONFIRM		0x10

static void ccid_PinBlock_cmp(void);
static uint8_t ccid_spe_get_state(void);
static void ccid_spe_reset_step(void);
static void ccid_spe_verify_step(void);
static void ccid_spe_modify_step(void);

text_char_t *ccid_spe_message(uint8_t id)
{
	switch (id) {
	case 0x00:
		return "VERIFY PIN";
	case 0x01:
		return "MODIFY PIN";
	case 0x02:
		return "CONFIRM";
	default:
		BUG();
		return NULL;
	}
}

void ccid_spe_count_down(uint8_t secs)
{
	lcd_draw_point((unsigned char)('0'+(uint8_t)div16u(secs, 100)),
			MAX_LCD_COLS-3, 0);
	lcd_draw_point((unsigned char)('0'+(uint8_t)div16u((uint8_t)mod16u(secs, 100),10)),
		       MAX_LCD_COLS-2, 0);
	lcd_draw_point((unsigned char)('0'+(uint8_t)mod16u(secs, 10)),
		       MAX_LCD_COLS-1, 0);
}

static void ccid_spe_indexed_msg(void)
{
	uint8_t index = ccid_spe_get_state();
	uint8_t msgidx;
	BUG_ON(index > 2);

	msgidx = ccid_cmd_data.po.bMsgIndex[index];
	if (msgidx > 0x02)
		ccid_display_default();
	else
		ccid_display_custom(ccid_spe_message(msgidx));
}

static uint8_t ccid_spe_pin_format(void)
{
	return ccid_cmd_data.po.bmFormatString&CCID_SPE_FORMAT_MASK;
}

static void ccid_spe_invs(void)
{
	scs_off_t i;

	if (ifd_get_convention() == IFD_CONV_DIRECT)
		return;

	for (i = CCID_SPE_APDU_FIXED; i < CCID_XB_NC; i++) {
		scd_write_byte(i, bitrev8(scd_read_byte(i)));
	}
}

uint8_t ccid_spe_invb(uint8_t value, uint8_t bits)
{
	if (ifd_get_convention() == IFD_CONV_DIRECT)
		return value;
	return bitrev8(value)>>(BITS_PER_UNIT-bits);
}

static void ccid_spe_uraise_all(void)
{
	ccid_spe_ctrl.flags &= ~CCID_SPE_FLAGS_MASK;
}

#define ccid_spe_raise(event)		raise_bits(ccid_spe_ctrl.flags, event)
#define ccid_spe_raised(event)		bits_raised(ccid_spe_ctrl.flags, event)
#define ccid_spe_unraise(event)		unraise_bits(ccid_spe_ctrl.flags, event)

static void ccid_spe_set_state(uint8_t state)
{
	ccid_spe_ctrl.flags &= ~CCID_SPE_STATE_MASK;
	ccid_spe_ctrl.flags |= (state & CCID_SPE_STATE_MASK);
	ccid_spe_indexed_msg();
	ccid_spe_reset_step();
	if (state == CCID_SPE_STATE_CONFIRM) {
		ccid_spe_unraise(CCID_SPE_BAD_CONFIRM);
	}
}

static uint8_t ccid_spe_get_state(void)
{
	return ccid_spe_ctrl.flags & CCID_SPE_STATE_MASK;
}

static boolean ccid_spe_is_format_bcd(void)
{
	return ccid_spe_pin_format()==CCID_SPE_FORMAT_BCD;
}

static boolean ccid_spe_is_format_ascii(void)
{
	return ccid_spe_pin_format()==CCID_SPE_FORMAT_ASCII;
}

unsigned char ccid_spe_key2digit(unsigned char keycode)
{
	if (ccid_spe_is_format_ascii())
		return keycode;
	else
		return keycode - '0';
}

static boolean ccid_spe_is_pin_bits(void)
{
	return CCID_SPE_PIN_UNIT()==0x00;
}

static boolean ccid_spe_is_len_bits(void)
{
	return CCID_SPE_LEN_UNIT()==0x00;
}

static boolean ccid_spe_is_justify_l(void)
{
	return CCID_SPE_JUSTIFY()==0x00;
}

static uint8_t ccid_spe_pin_pos(void)
{
	return (uint8_t)(ccid_cmd_data.po.bmFormatString&CCID_SPE_PIN_POS_MASK)>>
			CCID_SPE_PIN_POS_OFFSET;
}

static uint8_t ccid_spe_len_pos(void)
{
	return (uint8_t)(ccid_cmd_data.po.bmPINLengthFormat&CCID_SPE_LEN_POS_MASK);
}

static uint8_t ccid_spe_pin_bytes(void)
{
	return LOHALF(ccid_cmd_data.po.bmPINBlockString);
}

static uint8_t ccid_spe_pin_bits(void)
{
	return ccid_spe_is_format_bcd() ? 4 : 8;
}

static uint8_t ccid_spe_len_bits(void)
{
	return HIHALF(ccid_cmd_data.po.bmPINBlockString);
}

scs_off_t ccid_spe_first_byte(boolean len)
{
	uint8_t pos;
	boolean is_bits;
	if (len) {
		pos = ccid_spe_len_pos();
		is_bits = ccid_spe_is_len_bits();
	} else {
		pos = ccid_spe_pin_pos();
		is_bits = ccid_spe_is_pin_bits();
	}
	return  is_bits ? pos/BITS_PER_UNIT : pos;
}

uint8_t ccid_spe_first_bit(boolean len)
{
	uint8_t pos;
	boolean is_bits;
	if (len) {
		pos = ccid_spe_len_pos();
		is_bits = ccid_spe_is_len_bits();
	} else {
		pos = ccid_spe_pin_pos();
		is_bits = ccid_spe_is_pin_bits();
	}
	return is_bits ? pos%BITS_PER_UNIT : 0;
}

#define bits2mask_m(bits)	(uint8_t)(((1<<(bits))-1)<<(BITS_PER_UNIT-(bits)))
#define bits2mask_l(bits)	(uint8_t)((1<<(bits))-1)

void ccid_spe_write_bits(scs_off_t ibyte, uint8_t ibit,
			 uint8_t cont, uint8_t bits)
{
	uint8_t mask = bits2mask_l(bits);

	ifd_write_byte_mask(ibyte, (uint8_t)(cont<<ibit),
			    (uint8_t)(mask<<ibit));
	if ((ibit+hweight8(mask))>BITS_PER_UNIT) {
		ifd_write_byte_mask((scs_off_t)(ibyte+1),
				    (uint8_t)(cont>>(BITS_PER_UNIT-ibit)),
				    (uint8_t)(mask>>(BITS_PER_UNIT-ibit)));
	}
}

uint8_t ccid_spe_read_bits(scs_off_t ibyte, uint8_t ibit, uint8_t bits)
{
	uint8_t b1, b2, cont;
	if (ibit) {
		b1 = scd_read_byte(ibyte)&bits2mask_m(ibit);
		b2 = scd_read_byte((scs_off_t)(ibyte+1))&bits2mask_l(ibit);
		cont = (b1>>(8-ibit)) | (b2<<ibit);
	} else {
		cont = scd_read_byte(ibyte);
	}
	return cont & bits2mask_l(bits);
}

scs_off_t ccid_spe_offset_fixed(void)
{
	return CCID_SPE_APDU_FIXED+ccid_spe_ctrl.pin_offset;
}

void ccid_spe_write_digit(uint8_t nr, uint8_t digit)
{
	uint8_t first_bit;
	uint8_t shift_bits = ccid_spe_pin_bits();
	scs_off_t first_byte;

	/* calculate the initial position */
	first_byte = ccid_spe_first_byte(false)+ccid_spe_offset_fixed();
	first_bit = ccid_spe_first_bit(false);

	/* rotate position */
	first_byte += ((uint8_t)(shift_bits*nr))/BITS_PER_UNIT;
	first_bit += ((uint8_t)(shift_bits*nr))%BITS_PER_UNIT;
	ccid_spe_write_bits(first_byte, first_bit,
			    ccid_spe_invb(digit, shift_bits),
			    shift_bits);
}

uint8_t ccid_spe_read_digit(uint8_t nr)
{
	uint8_t shift_bits = ccid_spe_pin_bits();
	scs_off_t first_byte;
	uint8_t first_bit;
	uint8_t digit;

	first_byte = ccid_spe_first_byte(false)+ccid_spe_offset_fixed();
	first_bit = ccid_spe_first_bit(false);

	first_byte += ((uint8_t)(shift_bits*nr))/BITS_PER_UNIT;
	first_bit += ((uint8_t)(shift_bits*nr))%BITS_PER_UNIT;
	digit = ccid_spe_read_bits(first_byte,
				   first_bit, shift_bits);
	return ccid_spe_invb(digit, shift_bits);
}

void ccid_spe_write_length(uint8_t nr)
{
	uint8_t first_bit;
	scs_off_t first_byte;
	uint8_t shift_bits = ccid_spe_len_bits();

	if (!shift_bits)
		return;
	first_byte = ccid_spe_first_byte(true)+ccid_spe_offset_fixed();
	first_bit = ccid_spe_first_bit(true);
	ccid_spe_write_bits(first_byte, first_bit,
			    ccid_spe_invb(nr, shift_bits),
			    shift_bits);
}

static uint8_t ccid_spe_pin_digits(void)
{
	uint8_t nr_digits = ccid_spe_pin_bytes();
	if (ccid_spe_is_format_bcd())
		nr_digits *= 2;
	return nr_digits;
}

static uint8_t ccid_spe_max_digits(void)
{
	return LOBYTE(ccid_cmd_data.po.wPINMaxExtraDigit);
}

static boolean ccid_spe_validation(uint8_t validation)
{
	return ccid_cmd_data.po.bEntryValidationCondition & validation;
}

void ccid_spe_shift_left(void)
{
	uint8_t pin_digits = ccid_spe_pin_digits();
	uint8_t i;
	uint8_t digit;

	for (i = (pin_digits-ccid_spe_ctrl.nr_next-1); i < pin_digits-1; i++) {
		/* read next digit */
		digit = ccid_spe_read_digit((uint8_t)(i+1));
		/* write current digit */
		ccid_spe_write_digit(i, digit);
	}
}

void ccid_spe_key_confirm(unsigned char keycode)
{
	if (ccid_spe_ctrl.nr_next < ccid_spe_max_digits()) {
		uint8_t digit = ccid_spe_key2digit(keycode);
		scd_debug(SCD_DEBUG_DIGIT, digit);
		if (ccid_spe_is_justify_l()) {
			lcd_draw_point('*', ccid_spe_ctrl.nr_next, 1);
			if (digit != ccid_spe_read_digit(ccid_spe_ctrl.nr_next))
				ccid_spe_raise(CCID_SPE_BAD_CONFIRM);
		} else {
			lcd_draw_point('*', (uint8_t)(MAX_LCD_COLS-ccid_spe_ctrl.nr_next-1), 1);
			if (digit != ccid_spe_read_digit((uint8_t)(ccid_spe_pin_digits()-
								   ccid_spe_ctrl.nr_last+
								   ccid_spe_ctrl.nr_next)))
				ccid_spe_raise(CCID_SPE_BAD_CONFIRM);
		}
		ccid_spe_ctrl.nr_next++;
	}
}

void ccid_spe_key_number(unsigned char keycode)
{
	if (ccid_spe_ctrl.nr_next < ccid_spe_max_digits()) {
		uint8_t digit = ccid_spe_key2digit(keycode);
		scd_debug(SCD_DEBUG_DIGIT, digit);
		if (ccid_spe_is_justify_l()) {
			lcd_draw_point('*', ccid_spe_ctrl.nr_next, 1);
			ccid_spe_write_digit(ccid_spe_ctrl.nr_next, digit);
		} else {
			lcd_draw_point('*', (uint8_t)(MAX_LCD_COLS-ccid_spe_ctrl.nr_next-1), 1);
			/* rotate contents */
			ccid_spe_shift_left();
			ccid_spe_write_digit((uint8_t)(ccid_spe_pin_digits()-1), digit);
		}
		ccid_spe_ctrl.nr_next++;
	}
}

#if NR_SCD_SLOTS > 1
DECLARE_BITMAP(ccid_spe_waits, NR_SCD_SLOTS);

static void ccid_spe_operate_next(void)
{
	ifd_sid_t sid;
	for (sid = 0; sid < NR_SCD_SLOTS; sid++) {
		if (test_bit(sid, ccid_spe_waits)) {
			clear_bit(sid, ccid_spe_waits);
			ccid_sid_select(sid);
			__ccid_spe_operate_init();
			return;
		}
	}
}

static void ccid_spe_operate_wait(void)
{
	set_bit(scd_qid, ccid_spe_waits);
}

static void ccid_spe_slot_init(void)
{
}
#else
#define ccid_spe_operate_next()
#define ccid_spe_operate_wait()
#define ccid_spe_slot_init()
#endif

static void ccid_spe_operate_exit(void)
{
	ccid_spe_invs();

	kbd_set_capture(ccid_spe_kh, 0);
	ccid_spe_kh = NULL;

	/* current input waiter end */
	ccid_PinBlock_cmp();
	/* handle next input waiter */
	ccid_spe_operate_next();
}

void ccid_spe_key_end(void)
{
	ccid_spe_write_length(ccid_spe_ctrl.nr_next);
	if (ccid_spe_ctrl.aborted) {
		ccid_spe_operate_exit();
	} else {
		BUG_ON(!ccid_spe_ctrl.entry_cmpl);
		ccid_spe_ctrl.entry_cmpl();
	}
}

static void ccid_spe_reset_step(void)
{
	lcd_draw_row("", 1);
	ccid_spe_ctrl.nr_last = ccid_spe_ctrl.nr_next;
	ccid_spe_ctrl.nr_next = 0;
}

static void ccid_spe_reset_init(void)
{
	ccid_spe_reset_step();
	ccid_spe_ctrl.aborted = false;
	ccid_spe_ctrl.nr_last = 0;
	ccid_spe_uraise_all();
	ccid_spe_set_state(CCID_SPE_STATE_OLDPIN);
	ccid_spe_ctrl.pin_offset = 0;
}

void ccid_kbd_abort(void)
{
	/* XXX: Sync Abort from USB
	 *
	 * For kbd_capture_sync, ABORT could never be reached. When
	 * kbd_capture_sync is running, no ABORT messages could be
	 * received by the USB device controller as there isn't chances
	 * for USB state machine to handle incoming messages.
	 * Same reason as the ifd_xchg_abort.
	 */
	if (ccid_spe_kh != NULL) {
		kbd_abort_capture();
	}
}

void ccid_Secure_abort(void)
{
	/* XXX: Sync Abort from Keyboard
	 *
	 * This is the only way to abort SPE process.
	 */
	if (ccid_spe_kh != NULL) {
		ccid_spe_ctrl.aborted = true;
		CCID_XB_ERR = SCS_ERR_TIMEOUT;
		ccid_spe_key_end();
	}
}

void ccid_spe_key_escape(void)
{
	if (ccid_spe_ctrl.nr_next != 0) {
		ccid_spe_ctrl.nr_next = 0;
		lcd_draw_row("", 1);
	} else {
		ccid_Secure_abort();
	}
}

static void ccid_spe_key_wait(uint8_t scancode)
{
	if (scancode != 0) {
		ccid_spe_count_down(scancode);
		return;
	}

	if (ccid_spe_validation(CCID_SPE_VALID_TIMEOUT)) {
		ccid_spe_key_end();
	} else {
		ccid_Secure_abort();
	}
}

static void ccid_spe_key_down(uint8_t scancode)
{
	unsigned char keycode = kbd_translate_key(scancode);

	if (keycode == CCID_SPE_CR) {
		if (ccid_spe_validation(CCID_SPE_VALID_KEYPRESS)) {
			ccid_spe_key_end();
		}
	} else  if (keycode == CCID_SPE_ESC) {
		ccid_spe_key_escape();
	} else if (keycode >= '0' && keycode <= '9') {
		if (ccid_spe_get_state() == CCID_SPE_STATE_CONFIRM)
			ccid_spe_key_confirm(keycode);
		else
			ccid_spe_key_number(keycode);

		if (ccid_spe_validation(CCID_SPE_VALID_MAXSIZE) &&
		    ccid_spe_ctrl.nr_next == ccid_spe_max_digits()) {
			ccid_spe_key_end();
		}
	}
}

static void ccid_spe_capture(uint8_t scancode, uint8_t event)
{
	ccid_sid_select(ccid_spe_ctrl.sid);

	switch (event) {
	case KBD_EVENT_KEY_TOUT:
		ccid_spe_key_wait(scancode);
		break;
	case KBD_EVENT_KEY_DOWN:
		ccid_spe_key_down(scancode);
		break;
	}
}

static uint8_t ccid_spe_key_timeout(void)
{
	return (uint8_t)(ccid_cmd_data.po.bTimeout ?
			 ccid_cmd_data.po.bTimeout :
			 CCID_SPE_TIMEOUT);
}

static void ccid_spe_key_begin(ccid_spe_cmpl_cb cb)
{
	ccid_spe_reset_init();
	ccid_spe_ctrl.entry_cmpl = cb;
	ccid_spe_ctrl.sid = scd_qid;
	ccid_spe_kh = kbd_set_capture(ccid_spe_capture,
				      ccid_spe_key_timeout());
	BUG_ON(ccid_spe_kh == ccid_spe_capture);
}

uint8_t ccid_spe_msg_number(void)
{
	if (ccid_cmd_data.po.bNumberMessage < 4)
		return ccid_cmd_data.po.bNumberMessage;
	else
		return 0;
}

/* this function is called when scd_qid is determined
 */
static void __ccid_spe_operate_init(void)
{
	ccid_spe_invs();
	switch (ccid_cmd_data.po.bPINOpeartion) {
	case CCID_SPE_PIN_VERIFY:
		ccid_spe_key_begin(ccid_spe_verify_step);
		break;
	case CCID_SPE_PIN_MODIFY:
		ccid_spe_key_begin(ccid_spe_modify_step);
		ccid_spe_ctrl.pin_offset = ccid_cmd_data.pm.bInsertionOffsetOld;
		break;
	default:
		BUG();
		break;
	}
}

static void ccid_spe_verify_step(void)
{
	if (ccid_spe_ctrl.nr_next < HIBYTE(ccid_cmd_data.po.wPINMaxExtraDigit))
		CCID_XB_ERR = SCS_ERR_OVERRUN;
	ccid_spe_operate_exit();
}

static void ccid_spe_modify_step(void)
{
	switch (ccid_spe_get_state()) {
	case CCID_SPE_STATE_OLDPIN:
		ccid_spe_set_state(CCID_SPE_STATE_NEWPIN);
		ccid_spe_ctrl.pin_offset = ccid_cmd_data.pm.bInsertionOffsetNew;
		break;
	case CCID_SPE_STATE_NEWPIN:
		if (!ccid_cmd_data.pm.bConfirmPIN) {
			ccid_spe_operate_exit();
		} else {
			ccid_spe_set_state(CCID_SPE_STATE_CONFIRM);
		}
		break;
	case CCID_SPE_STATE_CONFIRM:
		if (ccid_spe_ctrl.nr_next != ccid_spe_ctrl.nr_last) {
			ccid_spe_raise(CCID_SPE_BAD_CONFIRM);
		}
		if (ccid_spe_raised(CCID_SPE_BAD_CONFIRM)) {
			ccid_spe_set_state(CCID_SPE_STATE_NEWPIN);
		} else {
			ccid_spe_operate_exit();
		}
		break;
	}
}

static void ccid_spe_operate_init(void)
{
	scd_slot_enter(SCD_SLOT_STATE_ISO7816);
	if (ccid_spe_kh) {
		ccid_spe_operate_wait();
	} else {
		__ccid_spe_operate_init();
	}
}
