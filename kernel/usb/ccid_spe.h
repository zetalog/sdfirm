#ifdef CONFIG_CCID_SECURE
#define CCID_SPE_CR		0x0D
#define CCID_SPE_ESC		0x1B
#define CCID_SPE_TIMEOUT	120

struct ccid_po_param {
	struct scd_xb_param xb;
	uint8_t bPINOpeartion;
	uint8_t bTimeout;
	uint8_t bmFormatString;
	uint8_t bmPINBlockString;
	uint8_t bmPINLengthFormat;
	uint16_t wPINMaxExtraDigit;
	uint8_t bEntryValidationCondition;
	uint8_t bNumberMessage;
	uint8_t bMsgIndex[3];
	uint16_t wLangId;
	uint8_t bTeoPrologue[3];
};

/* PIN verification */
struct ccid_pv_param {
	struct ccid_po_param po;
};

#define CCID_SPE_VERIFY_FIXED	9	/* bytes before bNumberMessage */
#define CCID_SPE_VERIFY_SIZE	15

/* PIN modification */
struct ccid_pm_param {
	struct ccid_po_param po;
	uint8_t bInsertionOffsetOld;
	uint8_t bInsertionOffsetNew;
	uint8_t bConfirmPIN;
};

#define CCID_SPE_MODIFY_FIXED	12	/* bytes before bNumberMessage */
#define CCID_SPE_MODIFY_SIZE	18

#define CCID_SPE_LCD_LAYOUT	((MAX_LCD_ROWS << 8) | MAX_LCD_COLS)
#define CCID_SPE_SUPPORT_FUNC	SCD_SPE_SUPPORT_ALL

void ccid_Secure_abort(void);
void ccid_kbd_abort(void);

void ccid_Secure_out(void);
void ccid_Secure_cmp(void);
void ccid_spe_init(void);
#else
#define CCID_SPE_LCD_LAYOUT	SCD_LCD_LAYOUT_NONE
#define CCID_SPE_SUPPORT_FUNC	SCD_SPE_SUPPORT_NONE

#define ccid_Secure_abort()
#define ccid_kbd_abort()
#define ccid_Secure_out()
#define ccid_Secure_cmp()	ccid_CmdOffset_cmp(0)
#define ccid_spe_init()
#endif
